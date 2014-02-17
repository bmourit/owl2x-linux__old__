/*
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file "COPYING" in the main directory of this archive
 *  for more details.
 *  Copyright (C) 2009 Actions Semi Inc.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <linux/kdev_t.h>
#include <linux/interrupt.h>
#include <asm/byteorder.h>
#include <linux/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/unaligned.h>

#include <linux/usb.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/otg.h>
#include <linux/clk.h>
#include <mach/clock.h>


#include "aotg_udc.h"
#include "aotg_regs.h"
#include "aotg_dma.h"
#include "aotg_plat_data.h"



/*function declare*/	
static unsigned int aotg_udc_kick_dma(struct aotg_ep *ep,
				      struct aotg_request *req);
static void aotg_udc_cancel_dma(struct aotg_ep *ep);
static void aotg_udc_dma_handler(struct aotg_udc *udc);

static void aotg_ep_reset(struct aotg_udc *udc, u8 ep_mask, u8 type_mask);
static int aotg_ep_disable(struct usb_ep *_ep);
static void nuke(struct aotg_ep *ep, int status);
static inline void handle_status(struct aotg_udc *udc);
static void done(struct aotg_ep *ep, struct aotg_request *req, int status);
static int write_ep0_fifo(struct aotg_ep *ep, struct aotg_request *req);
static int write_fifo(struct aotg_ep *ep, struct aotg_request *req);	
static int read_fifo(struct aotg_ep *ep, struct aotg_request *req);
static int pullup(struct aotg_udc *udc, int is_active);
static int aotg_udc_endpoint_config(struct aotg_udc *udc, struct usb_gadget_driver *driver);
static int write_packet(struct aotg_ep *ep, struct aotg_request *req, unsigned max);


#define DMAEPSEL_VAL(ep) (u8)((ep->bEndpointAddress >> 7)? 0 : 1)  \
                                |((ep->bEndpointAddress & EP_ADDR_MASK) << 1)
#define FIFOCTRL_VAL(ep) (u8)((ep->mask >> 4) << 4)  \
                                | (ep->mask & EP_ADDR_MASK)                                                       

#define	DRIVER_VERSION    "Version1.0-May-2006"
#define	DRIVER_DESC       "ATJ213X  USB Device Controller Driver"

static const char driver_name[] = "aotg_udc";
static struct aotg_udc *the_controller;

static const char ep0name[] = "ep0";

static unsigned long  flags;

#define EP_IRQ_BASE         0x16
#define EP_IRQ_OFFSET       0x0c

#define DEV_PLL             0xb0150004
/*
 * Debug filesystem
 */
#ifdef CONFIG_USB_GADGET_DEBUG_FS

#include <linux/seq_file.h>
#include <linux/debugfs.h>

static int
udc_seq_show(struct seq_file *m, void *_d)
{
	struct aotg_udc	*udc = m->private;
	unsigned long		flags;
	int			i;
	u8			tmp;
	struct aotg_plat_data *plat_data = udc->dev->platform_data;

	local_irq_save(flags);

	/* basic device status */
	seq_printf(m, DRIVER_DESC "\n"
		"%s version: %s\nGadget driver: %s\n\n",
		driver_name, DRIVER_VERSION,
		udc->driver ? udc->driver->driver.name : "(none)");

	/* registers for device and ep0 */
	seq_printf(m,
		"usb20_ecs %04x otgien %02x, otgstate %02X, otgctl %02X, ep0cs %02X, out0bc %02X, \
		in0bc %02X, dev_pll %02x, usbeien %02x, usbcs %02x\n",
		usb_readl(plat_data->usbecs),udc_readb(udc, OTGIEN), udc_readb(udc, OTGSTATE), udc_readb(udc,OTGCTRL), udc_readb(udc,EP0CS), udc_readb(udc,OUT0BC),
		udc_readb(udc,IN0BC), usb_readl((void __iomem *)DEV_PLL), udc_readb(udc, USBEIEN), udc_readb(udc, USBCS));


	tmp = udc_readb(udc,IVECT);
	seq_printf(m,
		"IVECT %02X =%s%s%s%s%s%s%s%s%s%s%s\n", tmp,
		(tmp == UIV_OTGIRQ) ? " otgirq" : "",
		(tmp == UIV_USBRESET) ? " reset" : "",
		(tmp == UIV_HSPEED) ? " hspeed" : "",
		(tmp == UIV_SUSPEND) ? " suspend" : "",
		(tmp == UIV_SUDAV) ? " sudav" : "",
		(tmp == UIV_EP0IN) ? " ep0in" : "",
		(tmp == UIV_EP0OUT) ? " ep0out" : "",
		(tmp == UIV_EP2IN) ? " ep2in" : "",
		(tmp == UIV_EP1OUT) ? " ep1out" : "",
		(tmp == UIV_EP1IN) ? " ep1in" : "",
		(tmp == UIV_EP2OUT) ? " ep2out" : "");

	tmp = udc_readb(udc,OTGIRQ);
	seq_printf(m,
		"OTGIRQ %02X =%s%s%s%s%s\n", tmp,
		(tmp & OTGIRQ_PERIPH) ? " periph" : "",
		(tmp & OTGIRQ_VBUSEER) ? " vbuseer" : "",
		(tmp & OTGIRQ_LOCSOF) ? " locsof" : "",
		(tmp & OTGIRQ_SRPDET) ? " srpdet" : "",
		(tmp & OTGIRQ_IDLE) ? " idle" : "");


	tmp = udc_readb(udc,USBIRQ);
	seq_printf(m,
		"udccs0 %02X =%s%s%s%s%s%s\n", tmp,
		(tmp & USBIRQ_HS) ? " hs" : "",
		(tmp & USBIRQ_URES) ? " rne" : "",
		(tmp & USBIRQ_SUSP) ? " susp" : "",
		(tmp & USBIRQ_SUTOK) ? " sutok" : "",
		(tmp & USBIRQ_SOF) ? " sof" : "",
		(tmp & USBIRQ_SUDAV) ? " sudav" : "");

	if (!udc->driver)
		goto done;

	/* dump endpoint queues */
	for (i = 0; i < AOTG_UDC_NUM_ENDPOINTS; i++) {
		struct aotg_ep	*ep = &udc->ep [i];
		struct aotg_request	*req;

		if (i != 0) {
			const struct usb_endpoint_descriptor	*desc;

			desc = ep->desc;
			if (!desc)
				continue;
			tmp = udc_readb(udc,ep->reg_udccs);
			seq_printf(m,
				"%s max %d udccs %02x irqs %lu\n",
				ep->ep.name, le16_to_cpu(desc->wMaxPacketSize),
				tmp, ep->udc_irqs);
			/* TODO translate all five groups of udccs bits! */

		} else /* ep0 should only have one transfer queued */
			seq_printf(m, "ep0 max 16 pio irqs %lu\n",
				ep->udc_irqs);

		if (list_empty(&ep->queue)) {
			seq_printf(m, "\t(nothing queued)\n");
			continue;
		}
		list_for_each_entry(req, &ep->queue, queue) {
			seq_printf(m,
					"\treq %p len %d/%d buf %p\n",
					&req->req, req->req.actual,
					req->req.length, req->req.buf);
		}
	}
done:
	local_irq_restore(flags);
	return 0;
}

static int
udc_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, udc_seq_show, inode->i_private);
}

static const struct file_operations debug_fops = {
	.open		= udc_debugfs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.owner		= THIS_MODULE,
};

#define create_debug_files(dev) \
	do { \
		dev->debugfs_udc = debugfs_create_file(dev->gadget.name, \
			S_IRUGO, NULL, dev, &debug_fops); \
	} while (0)
#define remove_debug_files(dev) \
	do { \
		if (dev->debugfs_udc) \
			debugfs_remove(dev->debugfs_udc); \
	} while (0)

#else	

#define create_debug_files(dev) do {} while (0)
#define remove_debug_files(dev) do {} while (0)

#endif	//CONFIG_USB_GADGET_DEBUG_FS

/*-------------------------------------------------------------------------*/
static inline void aotg_udc_clk_enable(struct aotg_udc *udc)
{ 
  u32 usbpll_reg;
  struct aotg_plat_data *plat_data = udc->dev->platform_data;
  
  usbpll_reg = usb_readl(plat_data->usbpll);
  usb_writel(plat_data->usbclk_bits | usbpll_reg, plat_data->usbpll);
  udelay(100);
}

static inline void aotg_udc_dev_reset(struct aotg_udc *udc )
{
  u32 devrst_reg;
  int i = 0;
  
  struct aotg_plat_data *plat_data = udc->dev->platform_data;

  devrst_reg = usb_readl(plat_data->devrst);
  devrst_reg &= ~plat_data->devrst_bits;
  usb_writel(devrst_reg, plat_data->devrst);
  udelay(1);
  devrst_reg = usb_readl(plat_data->devrst);
  usb_writel(plat_data->devrst_bits | devrst_reg, plat_data->devrst);
  udelay(100);
  
  /*check and wait reset finish*/
  if ((udc_readb(udc, USBERESET) & 0x01) != 0) {
  	UDC_PRINTK( "AOTG BEGIN TO RESET ... \n");
  }
  while ((udc_readb(udc, USBERESET) & 0x01) != 0 && (i < 500000)){
  	/* waiting for reset */
  	i++;
  	udelay(1);
  } 
  
  if ((udc_readb(udc, USBERESET) & 0x01) != 0) {
  	UDC_ERR( "<USB_DEV_RESET>AOTG reset not complete\n");
  } else {
  	UDC_PRINTK( "AOTG RESET complete.\n"); 
  }
}

static void pio_irq_disable(struct aotg_ep *ep)
{
	struct aotg_udc *udc = ep->dev;
	u8 is_in = ep->mask & USB_UDC_IN_MASK;
	u8 ep_num = ep->mask & EP_ADDR_MASK;

	UDC_PRINTK("<PIO_IRQ_DISABLE> ep mask %02x pio irq disable\n", ep->mask);
	if (is_in)
		udc_clearb_mask(udc, IN07IEN, (1 << ep_num));
	else
		udc_clearb_mask(udc, OUT07IEN,(1 << ep_num));
}

static void pio_irq_enable(struct aotg_ep *ep)
{
	struct aotg_udc *udc = ep->dev;
	u8 is_in = ep->mask & USB_UDC_IN_MASK;
	u8 ep_num = ep->mask & EP_ADDR_MASK;

	UDC_PRINTK("<PIO_IRQ_ENABLE>ep mask %02x pio irq enable\n", ep->mask);
	if (is_in)
		udc_setb_mask(udc, IN07IEN, (1 << ep_num));
	else
		udc_setb_mask(udc, OUT07IEN, (1 << ep_num));
}

static void pio_irq_clear(struct aotg_ep *ep)
{
	struct aotg_udc *udc = ep->dev;
	u8 is_in = ep->mask & USB_UDC_IN_MASK;
	u8 ep_num = ep->mask & EP_ADDR_MASK;

	UDC_PRINTK("<PIO_IRQ_CLEAR>ep mask %02x pio irq clear\n", ep->mask);
	if (is_in)
		udc_writeb(udc, IN07IRQ, (1 << ep_num));
	else {
		/* OUT EP */
		udc_writeb(udc, OUT07IRQ, (1 << ep_num));
	}
}

void udc_ep_packet_config(enum usb_device_speed usb_speed,struct aotg_udc *udc)
{
  int i;
	u16 packsize;
	struct aotg_ep *ep;
	
  for(i = 1 ;i < AOTG_UDC_NUM_ENDPOINTS; i++)
  {
		ep = &udc->ep[i];
		if(ep->bmAttributes == USB_ENDPOINT_XFER_BULK){
			packsize = (usb_speed == USB_SPEED_FULL) ? BULK_FS_PACKET_SIZE
				:BULK_HS_PACKET_SIZE;	
		}else if(ep->bmAttributes == USB_ENDPOINT_XFER_INT){
			packsize = (usb_speed == USB_SPEED_FULL) ? INT_FS_PACKET_SIZE
				:INT_HS_PACKET_SIZE;						
		}else if(ep->bmAttributes == USB_ENDPOINT_XFER_ISOC){
			packsize = (usb_speed == USB_SPEED_FULL) ? ISO_FS_PACKET_SIZE
				:ISO_HS_PACKET_SIZE;
		}else{
			continue ; 
		}
		ep->ep.maxpacket = packsize;
		ep->maxpacket = packsize; 
		udc_writel(udc, ep->reg_maxckl, packsize);	
  } 
	return ;
}

/*---------------------------------------------------------------------------
 *	endpoint related parts of the api to the usb controller hardware,
 *	used by gadget driver; and the inner talker-to-hardware core.
 *---------------------------------------------------------------------------*/

void udc_set_phy(struct aotg_udc *udc, u8 reg, u8 value)
{
  
	u8 addrlow, addrhigh;
	int time = 1;

	addrlow = reg & 0x0f;
	addrhigh = (reg >> 4) & 0x0f;

	/*write vstatus: */
	udc_writeb(udc, VDSTATE, value);
	
	mb();
	
	/*write vcontrol: */
	udc_writeb(udc, VDCTRL, addrlow | 0x10);
	udelay(time); //the vload period should > 33.3ns
	udc_writeb(udc, VDCTRL, addrlow & 0xf);
	
	udelay(time);
	mb();
	
	udc_writeb(udc, VDCTRL, addrhigh | 0x10);
	udelay(time);
	udc_writeb(udc, VDCTRL, addrhigh & 0x0f);
	udelay(time);
	udc_writeb(udc, VDCTRL, addrhigh | 0x10);
	udelay(time);  
}

void aotg_udc_phy_config(struct aotg_udc *udc)
{
  int time = 600;
  
    /*write vstatus: */
	udc_writeb(udc, VDSTATE, 0x0a);
	mb();
	udelay(time);
	/*write vcontrol: */
	udc_writeb(udc, VDCTRL, 0x17);
	udelay(time); //the vload period should > 33.3ns
	udc_writeb(udc, VDCTRL, 0x07);
	udelay(time);
	udc_writeb(udc, VDCTRL, 0x17);
	udelay(time);
	mb();
	udc_writeb(udc, VDCTRL, 0x1e);
	udelay(time);
  udc_writeb(udc, VDCTRL, 0x0e);
	udelay(time);
	udc_writeb(udc, VDCTRL, 0x1e);
	udelay(time);		
  
  udc_set_phy(udc, 0xe2, 0x60);  
  udc_set_phy(udc, 0xe7, 0x0a);
  udc_set_phy(udc, 0xe7, 0x0e);
  udc_set_phy(udc, 0xe2, 0x61);
}


static int aotg_ep_enable(struct usb_ep *_ep,
			  const struct usb_endpoint_descriptor *desc)
{
	struct aotg_ep *ep;
	struct aotg_udc *udc;

	ep = container_of(_ep, struct aotg_ep, ep);
	/*sanity check */
	if (!_ep || !desc || ep->desc || _ep->name == ep0name
	    || desc->bDescriptorType != USB_DT_ENDPOINT
	    || ep->bEndpointAddress != desc->bEndpointAddress
	    || ep->maxpacket < le16_to_cpu(desc->wMaxPacketSize)){
		UDC_ERR("<AOTG_EP_ENABLE>%s, bad ep or descriptor\n", __func__);
		return -EINVAL;
	}

	/*xfer types must match, except that interrupt ~= bulk */
	if (ep->bmAttributes != desc->bmAttributes
	    && ep->bmAttributes != USB_ENDPOINT_XFER_BULK
	    && ep->bmAttributes != USB_ENDPOINT_XFER_ISOC
	    && ep->bmAttributes != USB_ENDPOINT_XFER_INT){
		UDC_ERR("<AOTG_EP_ENABLE>%s: %s type mismatch\n", __func__, _ep->name);
		return -EINVAL;
	}

	if ((desc->bmAttributes == USB_ENDPOINT_XFER_BULK
	     && le16_to_cpu(desc->wMaxPacketSize)
	     != ep->maxpacket)
	    || !desc->wMaxPacketSize) {
		UDC_ERR("<AOTG_EP_ENABLE>%s: bad %s maxpacket\n", __func__, _ep->name);
		return -ERANGE;
	}

	udc = ep->dev;
	if (!udc->driver || udc->gadget.speed == USB_SPEED_UNKNOWN) {
		if (!udc->driver)
			UDC_PRINTK("no driver\n");
		if (udc->gadget.speed == USB_SPEED_UNKNOWN)
			UDC_PRINTK("UNKNOW speed\n");
    UDC_PRINTK("%s: bogus device state\n", __func__);
		return -ESHUTDOWN;
	}
  spin_lock_irqsave(&udc->lock, flags);
	ep->desc = desc;
	ep->stopped = 0;
	ep->udc_irqs = ep->dma_irqs = 0;
	ep->read.bytes = 0;
	ep->read.ops = 0;
	ep->write.bytes = 0;
	ep->write.ops = 0;
	ep->ep.maxpacket = le16_to_cpu(desc->wMaxPacketSize);
	ep->dma_working = 0;
	ep->dma = -1;
	//reset
	aotg_ep_reset(udc, ep->mask, ENDPRST_FIFORST | ENDPRST_TOGRST);
	udc_setb_mask(udc, ep->reg_udccon, EPCON_VAL);
  udc_setb_mask(udc, FIFOCTRL, FIFOCTRL_VAL(ep));

	spin_unlock_irqrestore(&udc->lock, flags);
	UDC_PRINTK("<EP ENABLE>%s enable, reg_udccon is %02x\n", _ep->name, udc_readb(udc, ep->reg_udccon));

	if (udc->dma_chan >= 0) { 
		switch (ep->bmAttributes) {
		case USB_ENDPOINT_XFER_BULK:
		case USB_ENDPOINT_XFER_ISOC:
			aotg_dma_reset(udc->dma_nr1);
			aotg_dma_enable_irq(udc->dma_nr1);
			aotg_dma_clear_pend(udc->dma_nr1);
			ep->dma = 0;
			if (udc->dma_chan >= 0) {
        UDC_PRINTK("%s using dma channel %d \n", _ep->name, udc->dma_chan);
			}
    break;
		}
	}
	
	return 0;
}

static int aotg_ep_disable(struct usb_ep *_ep)
{
	struct aotg_ep *ep;
	struct aotg_udc *udc;

	ep = container_of(_ep, struct aotg_ep, ep);
	if (!_ep || !ep->desc) {
		UDC_PRINTK("<EP DISABLE> %s not enabled\n", _ep ? ep->ep.name : NULL);
		return -EINVAL;
	}

	udc = ep->dev;
	spin_lock_irqsave(&udc->lock, flags);
	nuke(ep, -ESHUTDOWN);
	
	if (udc->dma_chan >= 0) {
		if (ep->dma >= 0) {
			UDC_PRINTK("free dma channel %d\n", udc->dma_chan);
			ep->dma = -1;
		}
	}
	udc_clearb_mask(udc, ep->reg_udccon, EPCON_VAL);
	pio_irq_disable(ep);

	ep->desc = NULL;
	ep->stopped = 1;

	spin_unlock_irqrestore(&udc->lock, flags);
	UDC_PRINTK("<EP DISABLE>%s disable\n", _ep->name);
	return 0;
}

static struct usb_request *aotg_ep_alloc_request(struct usb_ep *_ep,
						 unsigned gfp_flags)
{
	struct aotg_request *req;

	UDC_PRINTK("<EP ALLOC REQ>%s, flags is %d\n", _ep->name, gfp_flags);
	req = kmalloc(sizeof(*req), gfp_flags);
	if (!req)
		return NULL;
	memset(req, 0, sizeof(*req));
	INIT_LIST_HEAD(&req->queue);
	return &req->req;
}

static void aotg_ep_free_request(struct usb_ep *_ep, struct usb_request *_req)
{
	struct aotg_request *req;

	UDC_PRINTK("<EP FREE REQ>%s, %p\n", _ep->name, _req);
	req = container_of(_req, struct aotg_request, req);
	if (!list_empty(&req->queue))
		UDC_PRINTK("<EP FREE REQ>ep's queue is not empty\n");
	kfree(req);

}

static int
aotg_ep_queue(struct usb_ep *_ep, struct usb_request *_req, unsigned gfp_flags)
{
	struct aotg_ep *ep;
	struct aotg_udc *udc;
	struct aotg_request *req;
	unsigned int retval_kick_dma;
	
	/*sanity check */	
	req = container_of(_req, struct aotg_request, req);
	if (unlikely(!_req || !_req->complete || !_req->buf
		     || !list_empty(&req->queue))) {
		UDC_PRINTK("<EP QUEUE>bad params\n");
		return -EINVAL;
	}

	ep = container_of(_ep, struct aotg_ep, ep);
	if (unlikely(!_ep || (!ep->desc && ep->ep.name != ep0name))) {
		UDC_PRINTK("<EP QUEUE> bad ep\n");
		return -EINVAL;
	}

	udc = ep->dev;
	if (unlikely(!udc->driver || udc->gadget.speed == USB_SPEED_UNKNOWN)) {
		UDC_PRINTK("<EP QUEUE> bogus device state\n");
		return -ESHUTDOWN;
	}

	UDC_PRINTK("<EP QUEUE>%s queue req %p, len %d buf %p\n", _ep->name, _req, _req->length, _req->buf);
	/*it may be very noisy */

	spin_lock_irqsave(&udc->lock, flags);
	_req->status = -EINPROGRESS;
	_req->actual = 0;

	/*only if the req queue of ep is empty and ep is working ,
	   we kick start the queue */
	if (list_empty(&ep->queue) && !ep->stopped) {
		if (ep->bEndpointAddress == 0) {	/*EP0 */
			if (!udc->req_pending) {
				UDC_PRINTK("<EP QUEUE> something wrong with Control Xfers, req_pending is missing\n");
				spin_unlock_irqrestore(&udc->lock, flags);
				return -EL2HLT;
			}

			switch (udc->ep0state) {
			case EP0_OUT_DATA_PHASE:
				udc->stats.read.ops++;
				/*No-Data Control Xfer */
				if (!req->req.length) {
					/*ACK*/
					handle_status(udc);
					/*cleanup */
					udc->req_pending = 0;
					udc->ep0state = EP0_WAIT_FOR_SETUP;
					done(ep, req, 0);
					req = NULL;
					spin_unlock_irqrestore(&udc->lock, flags);
					return 0;

					/*Control Write Xfer */
				} else {
					/*in this case, we just arm the OUT EP0
					   for first OUT transaction during
					   the data stage
					   hang this req at the tail of
					   queue aossciated with EP0
					   expect OUT EP0 interrupt
					   to advance the i/o queue
					 */
					udc_writeb(udc, OUT0BC, 0);
				}
				break;
			case EP0_IN_DATA_PHASE:
				udc->stats.write.ops++;
				/*Control Read Xfer */
				if (write_ep0_fifo(ep, req))
					udc->ep0state = EP0_END_XFER; //!!!!handle_ep0_in will call done(ep, req, 0);
				break;
			default:
				UDC_PRINTK("<EP QUEUE> ep0 i/o, odd state %d\n", udc->ep0state);
				spin_unlock_irqrestore(&udc->lock, flags);
				return -EL2HLT;
			}			
		}
    //bulk using dma
		else if (ep->dma >= 0 && ep->bmAttributes
			    == USB_ENDPOINT_XFER_BULK &&
			    udc->dma_chan >= 0) {			    
			retval_kick_dma = aotg_udc_kick_dma(ep, req);
			if (retval_kick_dma == 0) {
				UDC_PRINTK("<ep_queue>:req.length = %x , req.actual = %x  req = %p \n",
						req->req.length,
						req->req.actual, req);
				if (req->req.length != 0
				    && req->req.length == req->req.actual) {
					req = NULL;
				}
			} else {
				UDC_PRINTK("<ep_queue>:req.length = %x ,req.actual = %x  req = %p req = NULL!!!! \n",
						req->req.length,
						req->req.actual, req);
				req = NULL;
			}
		}
		//bulk use pio
		/*EP IN */
		else if (((ep->bEndpointAddress & USB_DIR_IN) != 0) &&
			 (ep->bmAttributes == USB_ENDPOINT_XFER_BULK)) {
			UDC_PRINTK("(fifo <--) HERE<-PC\n");
			if (!(udc_readb(udc, ep->reg_udccs) & EPCS_BUSY)) {
				/*fifo can access */
				UDC_PRINTK("<EP QUEUE>%s fifo is all ready,\
						wrtie immediately\n",
						ep->ep.name);
				pio_irq_clear(ep);
				/*avoid nonsensical interrupt */
				if (write_fifo(ep, req))
					req = NULL;
			}
		}
		/*EP OUT */
		else if (((ep->bEndpointAddress & USB_DIR_IN) == 0) &&
			 (ep->bmAttributes == USB_ENDPOINT_XFER_BULK)) {
			UDC_PRINTK("(fifo -->) HERE->PC\n");
			if (!(udc_readb(udc, ep->reg_udccs) & EPCS_BUSY)) {
				/*in multi-buffer mode
				   there may be some packets in fifo already */
				UDC_PRINTK("<EP QUEUE>%s fifo \
						has data already,\
						read immediately\n",
						ep->ep.name);
				pio_irq_clear(ep);
				/*avoid nonsensical interrupt */
				if (read_fifo(ep, req))
					req = NULL;
			}
		}
		/*INT IN */
		else if (((ep->bEndpointAddress & USB_DIR_IN) != 0) &&
			 (ep->bmAttributes == USB_ENDPOINT_XFER_INT)) {
			UDC_PRINTK("<QUEUE>:INT IN \n");
			if (!(udc_readb(udc,ep->reg_udccs) & EPCS_BUSY)) {
				/*fifo can access */
				pio_irq_clear(ep);
				/*avoid nonsensical interrupt */
				if (write_fifo(ep, req))
					req = NULL;
			}
		}
		/*INT OUT */
		else if (((ep->bEndpointAddress & USB_DIR_IN) == 0) &&
			 (ep->bmAttributes == USB_ENDPOINT_XFER_INT)) {

			UDC_PRINTK("<QUEUE>:INT OUT\n");
			if (!(udc_readb(udc, ep->reg_udccs) & EPCS_BUSY)) {
				/*in multi-buffer mode
				   there may be some packets in fifo already */
				UDC_PRINTK("<EP QUEUE>%s fifo has data already,read immediately\n", ep->ep.name);
				pio_irq_clear(ep);
				/*avoid nonsensical interrupt */
				if (read_fifo(ep, req))
					req = NULL;
			}
		}
		else {
			UDC_PRINTK("****ERROR****aotg_ep_queue :this ep is not support\n\n\n");
		}
		if (udc->dma_chan >= 0) {
			if (ep->dma_working == 0) {
				if (likely(req && ep->desc))
				  {
					  pio_irq_enable(ep);
					}
			}
		} else {
			if (likely(req && ep->desc))
				pio_irq_enable(ep);
		}
	}
	/*pio or dma irq handler advances the queue. */
	if (likely(req != NULL)){
		list_add_tail(&req->queue, &ep->queue);
		UDC_PRINTK("<EP QUEUE>the req of %s is not be done completely,queueing and wait irq kickstart\n",
				ep->ep.name);
	}
	spin_unlock_irqrestore(&udc->lock, flags);
	return 0;
}

static int aotg_ep_dequeue(struct usb_ep *_ep, struct usb_request *_req)
{
	struct aotg_ep *ep;
	struct aotg_request *req;
	struct aotg_udc *udc;
	
	UDC_PRINTK("<EP DEQUEUE> %s dequeues one req %p\n", _ep->name, _req);
	ep = container_of(_ep, struct aotg_ep, ep);

	if (!_ep || ep->ep.name == ep0name)
		return -EINVAL;
	
	udc = ep->dev;	
	spin_lock_irqsave(&udc->lock, flags);
	/*make sure it's actually queued on this endpoint */
	list_for_each_entry(req, &ep->queue, queue) {
		if (&req->req == _req)
			break;
	}
	if (&req->req != _req) {
		spin_unlock_irqrestore(&udc->lock, flags);
		return -EINVAL;
	}

	if (ep->dev->dma_chan >= 0) {
		if ((ep->dma_working) && ep->queue.next == &req->queue
		    && !ep->stopped) {
			UDC_PRINTK( "when dma, dequeue\n");
			aotg_udc_cancel_dma(ep);
			done(ep, req, -ECONNRESET);
			/*restart I/O */
			if (!list_empty(&ep->queue)) {
				req = list_entry(ep->queue.next,
						 struct aotg_request, queue);
				aotg_udc_kick_dma(ep, req);
			}
		} else
			done(ep, req, -ECONNRESET);
	} 
	else	
		done(ep, req, -ECONNRESET);
		
	spin_unlock_irqrestore(&udc->lock, flags);
	return 0;
}

static int aotg_ep_set_halt(struct usb_ep *_ep, int value)
{
	struct aotg_ep *ep;
	struct aotg_udc *udc;
	int retval = -EOPNOTSUPP;

	ep = container_of(_ep, struct aotg_ep, ep);
	if (!_ep || (!ep->desc && ep->ep.name != ep0name)) {
		UDC_PRINTK("<EP HALT>, bad ep\n");
		return -EINVAL;
	}

	udc = ep->dev;
	spin_lock_irqsave(&udc->lock, flags);
	/*EP0 */
	if (ep->bEndpointAddress == 0) {
		if (value) {
      udc_setb_mask(udc, EP0CS, EP0CS_STALL);
			udc->req_pending = 0;
			udc->ep0state = EP0_STALL;
			retval = 0;
		} else {
      udc_clearb_mask(udc, EP0CS, EP0CS_STALL);
			udc->ep0state = EP0_WAIT_FOR_SETUP;
			retval = 0;
		}

		/*otherwise, all active non-ISO endpoints can halt */
	} else if (ep->bmAttributes != USB_ENDPOINT_XFER_ISOC && ep->desc) {
		/*IN endpoints must already be idle */
		if ((ep->bEndpointAddress & USB_DIR_IN)
		    && !list_empty(&ep->queue)) {
			UDC_PRINTK("<EP HALT>dangrous,epin queue is not empty\n");
			retval = 0;
			goto done;
		}
		/*IN endpoint FIFO must be empty */
		if (ep->bEndpointAddress & USB_DIR_IN) {
			u8 not_empty;
			not_empty = (udc_readb(udc, ep->reg_udccs) & EPCS_BUSY)
			    || (ep->buftype
				- ((udc_readb(udc, ep->reg_udccs) >> 2) & 0x03));
			if (not_empty) {
				UDC_PRINTK("<EP HALT>dangrous, epin fifo is not empty\n");
				retval = 0;
				goto done;
			}
		}

		if (value) {
      /*set the stall bit */
			udc_setb_mask(udc, ep->reg_udccon, EPCON_STALL);
			ep->stopped = 1;
		} else {
      /*clear the stall bit */
			udc_clearb_mask(udc, ep->reg_udccon, EPCON_STALL);
			ep->stopped = 0;
		}
		retval = 0;
	}

done:
	UDC_PRINTK("<EP HALT>%s %s halt stat %d\n", ep->ep.name, value ? "set" : "clear", retval);
	spin_unlock_irqrestore(&udc->lock, flags);
	
	return retval;
}

static void aotg_ep_fifo_flush(struct usb_ep *_ep)
{
	struct aotg_udc *udc;
	struct aotg_ep *ep;
	ep = container_of(_ep, struct aotg_ep, ep);
	if (!_ep || ep->ep.name == ep0name || !list_empty(&ep->queue)) {
		UDC_PRINTK("<EP FIFO FLUSH>bad ep\n");
		return;
	}
	UDC_PRINTK("<EP FIFO FLUSH>%s fifo flush\n", ep->ep.name);
  udc = ep->dev;
	aotg_ep_reset(udc, ep->mask, ENDPRST_FIFORST);
	
}

static struct usb_ep_ops aotg_ep_ops = {
	.enable = aotg_ep_enable,
	.disable = aotg_ep_disable,

	.alloc_request = aotg_ep_alloc_request,
	.free_request = aotg_ep_free_request,

	.queue = aotg_ep_queue,
	.dequeue = aotg_ep_dequeue,

	.set_halt = aotg_ep_set_halt,
	//.fifo_status  = aotg_ep_fifo_status, 
	.fifo_flush = aotg_ep_fifo_flush,	/*not sure */

};


/*---------------------------------------------------------------------------
 *	device operations  related parts of
 *	the api to the usb controller hardware,
 *	which don't involve endpoints (or i/o), used by gadget driver;
 *	and the inner talker-to-hardware core.
 *---------------------------------------------------------------------------
 */
static int aotg_udc_get_frame(struct usb_gadget *_gadget)
{
	struct aotg_udc *udc;
	u16 frmnum = 0;

	udc = container_of(_gadget, struct aotg_udc, gadget);

	UDC_PRINTK("<UDC_GET_FRAME>Frame No.: %d\n", frmnum);
	return frmnum;

}

static int aotg_udc_wakeup(struct usb_gadget *_gadget)
{
	struct aotg_udc *udc;
	int retval = -EHOSTUNREACH;

	udc = container_of(_gadget, struct aotg_udc, gadget);

	return retval;
}

static int aotg_udc_vbus_session(struct usb_gadget *_gadget, int is_active)
{
	struct aotg_udc *udc;

	UDC_PRINTK("<UDC_VBUS_SESSION> VBUS %s\n", is_active ? "on" : "off");
	udc = container_of(_gadget, struct aotg_udc, gadget);
	spin_lock_irqsave(&udc->lock, flags);
	pullup(udc, is_active);
	spin_unlock_irqrestore(&udc->lock, flags);

	return 0;

}

static int aotg_udc_pullup(struct usb_gadget *_gadget, int is_on)
{
	struct aotg_udc *udc;

	udc = container_of(_gadget, struct aotg_udc, gadget);
	spin_lock_irqsave(&udc->lock, flags);
	udc->softconnect = (is_on != 0);
	pullup(udc, is_on);
	spin_unlock_irqrestore(&udc->lock, flags);

	return 0;
}

static int aotg_udc_vbus_draw(struct usb_gadget *_gadget, unsigned mA)
{
	struct aotg_udc *udc;

	udc = container_of(_gadget, struct aotg_udc, gadget);
	if (udc->transceiver)
		return usb_phy_set_power(udc->transceiver, mA);
	return -EOPNOTSUPP;
}

static int aotg_udc_start(struct usb_gadget_driver *driver,
		int (*bind)(struct usb_gadget *))
{
	struct aotg_udc *udc = the_controller;
	int retval;

	if (!driver || driver->max_speed < USB_SPEED_FULL || !bind
			|| !driver->disconnect || !driver->setup)
		return -EINVAL;
	if (!udc)
		return -ENODEV;
	if (udc->driver)
		return -EBUSY;

	/* first hook up the driver ... */
	udc->driver = driver;
	udc->gadget.dev.driver = &driver->driver;

	retval = device_add(&udc->gadget.dev);
	if (retval) {
		dev_err(udc->dev, "device_add error %d\n", retval);
		goto add_fail;
	}

	aotg_udc_endpoint_config(udc, driver);	
    
	retval = bind(&udc->gadget);
	if (retval) {
		dev_err(udc->dev, "bind to driver %s --> error %d\n",
			driver->driver.name, retval);
		goto bind_fail;
	}
	dev_dbg(udc->dev, "registered gadget driver '%s'\n",
		driver->driver.name);

	if (udc->transceiver) {
		retval = otg_set_peripheral(udc->transceiver->otg,
						&udc->gadget);
		if (retval) {
			dev_err(udc->dev, "can't bind to transceiver\n");
			goto transceiver_fail;
		}
	}
  aotg_udc_pullup(&udc->gadget, 1);

	return 0;

transceiver_fail:
	if (driver->unbind)
		driver->unbind(&udc->gadget);
bind_fail:
	device_del(&udc->gadget.dev);
add_fail:
	udc->driver = NULL;
	udc->gadget.dev.driver = NULL;
	return retval;
}

static int aotg_udc_stop(struct usb_gadget_driver *driver)
{
	struct aotg_udc *udc = the_controller;

	if (!udc)
		return -ENODEV;
	if (!driver || driver != udc->driver || !driver->unbind)
		return -EINVAL;

	udc->driver->disconnect(&udc->gadget);
	aotg_udc_pullup(&udc->gadget, 0);

	driver->unbind(&udc->gadget);
	udc->driver = NULL;

	device_del(&udc->gadget.dev);

	if (udc->transceiver)
		return otg_set_peripheral(udc->transceiver->otg, NULL);
	return 0;
}

static const struct usb_gadget_ops aotg_udc_ops = {
	.get_frame	= aotg_udc_get_frame,
	.wakeup		= aotg_udc_wakeup,
	.pullup		= aotg_udc_pullup,
	.vbus_session	= aotg_udc_vbus_session,
	.vbus_draw	= aotg_udc_vbus_draw,
	.start		= aotg_udc_start,
	.stop		= aotg_udc_stop,
};

/*---------------------------------------------------------------------------
 *
 *	DMA functions. aotg_udc_kick_dma()=>
 *	aotg_udc_start_dma()=>aotg_udc_dma_handler()=>
 *	usb_gadget_register_driver()
 *
 *---------------------------------------------------------------------------*/
static void aotg_udc_start_dma(struct aotg_ep *ep,
			       struct aotg_request *req, int is_in)
{
	struct aotg_udc *udc = ep->dev;
	u32 dcmd = 0;
	u32 buf = 0;
	u32 fifo = 0;
	
	udc_writeb(udc, FIFOCTRL, FIFOCTRL_VAL(ep)| FIFOAUTO_BIT);	/*set fifoauto */
		
	if (is_in == USB_DIR_IN) {	/*ddr -> usb */
		dcmd = req->req.length;
		
		usb_gadget_map_request(&udc->gadget, &req->req, 1);
		fifo = ep->reg_udcfifo;
		buf = req->req.actual + req->req.dma;
		if ((ep->desc != NULL) && (ep->desc->wMaxPacketSize > 0)) {
			dcmd = (req->req.length - req->req.actual) -
			    ((req->req.length - req->req.actual) % ep->desc->wMaxPacketSize);
		} else {
			UDC_PRINTK( "aotg:error and return, %d, %s\n", __LINE__, __FILE__);
			return;
		}		
		ep->dma_bytes = dcmd;	
					    		
	} else {  /*OUT      usb -> ddr */
	
		dcmd = req->req.length;		
		usb_gadget_map_request(&udc->gadget, &req->req, 0);
		fifo = ep->reg_udcfifo;
		buf = req->req.actual + req->req.dma;
		
		ep->dma_bytes = dcmd;
	}
	
  aotg_dma_stop(udc->dma_nr1);
	aotg_dma_reset(udc->dma_nr1);
  aotg_dma_set_memaddr(udc->dma_nr1, req->req.dma);
	aotg_dma_set_mode(udc->dma_nr1, DMAEPSEL_VAL(ep));
	aotg_dma_set_cnt(udc->dma_nr1, ep->dma_bytes);

	aotg_dma_start(udc->dma_nr1);
	
	return;
}

static unsigned int aotg_udc_kick_dma(struct aotg_ep *ep,
				      struct aotg_request *req)
{
  struct aotg_udc *udc = ep->dev;
  //unsigned length;
	int is_in = ep->bEndpointAddress & USB_DIR_IN;
	
	if (is_in) {
		/*if left data >= wMaxPacketSize use DMA
		   otherwise use PIO */
		if ((unlikely(req->req.length - req->req.actual == 0)) ||
		    ((req->req.length - req->req.actual) < ep->maxpacket)) {  
			 pio_irq_enable(ep);
			if (!(udc_readb(udc, ep->reg_udccs) & EPCS_BUSY))
			{
				return write_fifo(ep, req);
			}
		} else {
			ep->dma_working = 1;
			pio_irq_disable(ep);
			aotg_udc_start_dma(ep, req, USB_DIR_IN);
			return 0;
		}
	} else {
		/*only when length > maxpacket use dma */			
		if ((unlikely(req->req.length - req->req.actual == 0)) ||
		    ((req->req.length - req->req.actual) <= ep->ep.maxpacket)) {
			pio_irq_enable(ep);
			if (!(udc_readb(udc, ep->reg_udccs) & EPCS_BUSY))
			  { 
				  return read_fifo(ep, req);
				}
		} else {
			ep->dma_working = 1;
			pio_irq_disable(ep);
			aotg_udc_start_dma(ep, req, USB_DIR_OUT);
			return 0;
		}
	}
	return 0;
}

static void aotg_udc_cancel_dma(struct aotg_ep *ep)
{
	struct aotg_udc *udc = ep->dev;
	
	UDC_PRINTK("<UDC_CANCEL_DMA>\n");
	ep->dma_working = 0;
	
	if (udc->dma_chan >= 0) {
    aotg_dma_reset(udc->dma_nr1);
	}	
	udc_writeb(udc, FIFOCTRL,  FIFOCTRL_VAL(ep)); /*clear fifoauto */
	aotg_ep_reset(udc, ep->mask, ENDPRST_FIFORST);
}

static void aotg_udc_dma_handler(struct aotg_udc *udc)
{
	struct aotg_ep *ep = (struct aotg_ep *)(&(udc->ep));
	struct aotg_request *req = NULL;
	unsigned int is_in = 0;
	unsigned int i;
	u32 completed;

	for (i = 0; i < AOTG_UDC_NUM_ENDPOINTS; i++) {
		if (ep[i].dma_working == 1) {
			ep = &ep[i];
			break;
		}
	}

	if (ep->dma_working == 0) {
		for (i = 1; i < AOTG_UDC_NUM_ENDPOINTS; i++) {
			UDC_PRINTK("<dma_handler>:ep[%d].dma_bytes = %x\n", i, ep[i].dma_bytes);
		}
		return;
	}
	
	is_in = ep->bEndpointAddress & USB_DIR_IN;
	req = list_entry(ep->queue.next, struct aotg_request, queue);
	
	udc_writeb(udc, FIFOCTRL, FIFOCTRL_VAL(ep));

	UDC_PRINTK("<dma_handler>:ep address =%d \n", ep->bEndpointAddress);

	ep->dma_irqs++;
	ep->dev->stats.irqs++;

	/*update transfer status */
	req->req.actual += ep->dma_bytes;
	completed = 0;

	if (is_in == USB_DIR_IN) {
		if (((req->req.length - req->req.actual) > 0)
		    && ((req->req.length - req->req.actual)
			< ep->maxpacket)) {
			if (udc_readb(udc, ep->reg_udccs) & EPCS_BUSY) {
				UDC_PRINTK("<dma_handler>: IN ep BUSY..\n");
				pio_irq_enable(ep);
				ep->dma_working = 0;
				completed = 0;
			} else {
				UDC_PRINTK("<dma_handler>:IN ep NOT busy.\n");
				write_packet(ep, req, ep->maxpacket);
				done(ep, req, 0);
				ep->dma_working = 0;
				completed = 1;
			}
		} else if ((req->req.length -
					req->req.actual) >= ep->maxpacket) {
			UDC_PRINTK("<dma_handler>: length - actual >= 512K\n");
			aotg_udc_kick_dma(ep, req);
			completed = 0;
		} else {
			done(ep, req, 0);
			ep->dma_working = 0;
			completed = 1;
		}

	} else {
		 /*OUT*/
		if (((req->req.length - req->req.actual) > 0)
			     && ((req->req.length - req->req.actual)
				 < ep->maxpacket)) {
			UDC_PRINTK("<dma_hander>:OUT : short packet\n");
			pio_irq_clear(ep);
			pio_irq_enable(ep);
			read_fifo(ep, req);
			completed = 0;
			ep->dma_working = 0;
		} else if (req->req.length == req->req.actual) {
			UDC_PRINTK("<dma_hander>:OUT: packet compeleted\n");
			completed = 1;
			done(ep, req, 0);
      ep->dma_working = 0;
		} else {
			completed = 0;
			UDC_PRINTK("<dma_hander>:***continue !!***\n");
			aotg_udc_start_dma(ep, req, USB_DIR_OUT);
		}
	}

	if (completed == 1) {
		if (likely(!list_empty(&ep->queue))) {
			req = list_entry(ep->queue.next,
					 struct aotg_request, queue);
			UDC_PRINTK("restart dma!!!!!!!!!\n");
			pio_irq_clear(ep);
			aotg_udc_kick_dma(ep, req);
		}
	} else {
		pio_irq_clear(ep);
	}
	
	return;
}
/*---------------------------------------------------------------------------
 *	handle  interrupt
 *---------------------------------------------------------------------------
 */

static void done(struct aotg_ep *ep, struct aotg_request *req, int status)
{
	unsigned stopped = ep->stopped;
	u8 direction;
	struct aotg_udc *udc = ep->dev;
	
	list_del_init(&req->queue);

	if (likely(req->req.status == -EINPROGRESS))
		req->req.status = status;
	else
		status = req->req.status;

	if (status && status != -ESHUTDOWN)
		UDC_PRINTK("<REQ RELEASE>complete %s req %p stat %d len %u/%u\n",
				ep->ep.name, &req->req, status,
				req->req.actual, req->req.length);

  if(ep->dma_working == 1){
    direction = (ep->mask & USB_UDC_IN_MASK)? 1 :0 ;
    usb_gadget_unmap_request(&udc->gadget, &req->req, direction);
  }

	/*don't modify queue heads during completion callback */
	ep->stopped = 1;
  spin_unlock(&ep->dev->lock);
	req->req.complete(&ep->ep, &req->req);
  spin_lock(&ep->dev->lock);
	ep->stopped = stopped;
}

static inline void aotg_ep_setup(struct aotg_udc *udc, u8 index, char *name, u8 addr, u8 type, u8 buftype)
{
	struct aotg_ep *ep;
	ep = &udc->ep[index];

	strlcpy(ep->name, name, sizeof ep->name);
	ep->ep.name = ep->name;
	ep->bmAttributes = type;
	ep->bEndpointAddress = addr;

	if(type == USB_ENDPOINT_XFER_BULK){
		if(ep->bEndpointAddress & USB_DIR_IN){
			if((ep->bEndpointAddress & EP_ADDR_MASK)== 1)
				udc_writel(udc, ep->reg_fifostaddr, EP1_BULK_IN_STARTADD);
			else
				udc_writel(udc, ep->reg_fifostaddr, EP2_BULK_IN_STARTADD);
		}else{
			if((ep->bEndpointAddress & EP_ADDR_MASK)== 1)
				udc_writel(udc, ep->reg_fifostaddr, EP1_BULK_OUT_STARTADD);
			else
				udc_writel(udc, ep->reg_fifostaddr, EP2_BULK_OUT_STARTADD);			
		}
		
	}else if(type == USB_ENDPOINT_XFER_INT){			
		udc_writel(udc, ep->reg_fifostaddr, EP1_INT_IN_STARTADD);		
	}else if(type == USB_ENDPOINT_XFER_ISOC){		
		udc_writel(udc, ep->reg_fifostaddr, EP2_ISO_IN_STARTADD);
	}else{
		return ; 
	}
	
	ep->buftype = buftype;
	if(type == USB_ENDPOINT_XFER_ISOC)
		udc_writeb(udc, ep->reg_udccon, (type << 2) | buftype | (1 << 5));
	else
		udc_writeb(udc, ep->reg_udccon, (type << 2) | buftype);
	return ;
}

static void aotg_ep_reset(struct aotg_udc *udc, u8 ep_mask, u8 type_mask)
{
	u8 val;

	udc_writeb(udc, ENDPRST, ep_mask);	/*select ep */
	val = ep_mask | type_mask;
	udc_writeb(udc, ENDPRST, val);	/*reset ep */
}

static inline void handle_status(struct aotg_udc *udc)
{	
	udc_setb_mask(udc, EP0CS, EP0CS_HSNAK);
	UDC_PRINTK("<CTRL>ACK the status stage\n");
}

static void nuke(struct aotg_ep *ep, int status)
{
	struct aotg_request *req;
	
	if (ep->dev->dma_chan >= 0) {
		if ((ep->dma_working != 0) && !ep->stopped)
			aotg_udc_cancel_dma(ep);
	}	
	UDC_PRINTK("<EP NUKE> %s is nuked with status %d\n",
		   ep->ep.name, status);
	while (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct aotg_request, queue);
		done(ep, req, status);
	}
	if (ep->desc)
		pio_irq_disable(ep);
}

static void handle_setup(struct aotg_udc *dev)
{
	u16 w_value, w_length, w_index;
	u32 ackval = 0;
	int reciptype;
	int ep_num;
	struct aotg_ep *ep;
	struct aotg_udc *udc = dev;
	struct aotg_ep *ep0 = &udc->ep[0];
	union {
		struct usb_ctrlrequest r;
		u8 raw[8];
	} u;
	int i, status = 0;
	unsigned long addr = SETUPDAT0;
	
	if (udc->ep0state != EP0_WAIT_FOR_SETUP) {
		nuke(ep0, -ESHUTDOWN);
		udc->ep0state = EP0_WAIT_FOR_SETUP;
	} else
		nuke(ep0, -EPROTO);
	for (i = 0; i < 8; i++) {
		u.raw[i] = udc_readb(udc, addr);
		addr++;
	}
	w_value = le16_to_cpup(&u.r.wValue);
	w_length = le16_to_cpup(&u.r.wLength);
  w_index = le16_to_cpup(&u.r.wIndex);
  
	UDC_PRINTK("<CTRL> SETUP %02x.%02x v%04x i%04x l%04x\n",
		  u.r.bRequestType, u.r.bRequest, w_value, w_index, w_length);
	/*Delegate almost all control requests to the gadget driver,
	 *except for a handful of ch9 status/feature requests that
	 *hardware doesn't autodecode and the gadget API hides.
	 */
	udc->req_std = (u.r.bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD;
	udc->req_config = 0;
	udc->req_pending = 1;
	ep0->stopped = 0;
	reciptype = (u.r.bRequestType & USB_RECIP_MASK);
	switch (u.r.bRequest) {
	case USB_REQ_GET_STATUS:
		UDC_PRINTK("<CTRL>USB_REQ_GET_STATUS\n");
		if (reciptype == USB_RECIP_INTERFACE) {
			/*according to USB spec,
			   this does nothing but return 0 */
			break;
		} else if (reciptype == USB_RECIP_DEVICE) {
			/*return self powered and remote wakeup status
			 *we are self powered , so just check wakeup character
			 */
			if (udc->rwk)
				ackval = 0x02;
			else
				ackval = 0x00;
		} else if (reciptype == USB_RECIP_ENDPOINT) {
			ep_num = u.r.wIndex & EP_ADDR_MASK;

			if (ep_num > (AOTG_UDC_NUM_ENDPOINTS - 1)
			    || u.r.wLength > 2)
				goto stall;
			ep = &udc->ep[ep_num];
			if ((ep_num != 0)
			    && (ep->bEndpointAddress != u.r.wIndex))
				goto stall;
			if (ep->bEndpointAddress == 0)
				ackval |= ((udc_readb(udc, EP0CS)
					    & EP0CS_STALL) >> 1);
			/* weird is there should do right twist? */
			else
				ackval |= ((udc_readb(udc, ep->reg_udccon)
					    & EPCON_STALL) >> 6);
		} else
			goto stall;

		/*back the status */
		/*FIXME not check whether ep0 fifo is empty */
		udc_writel(udc, EP0INDAT0, ackval);
		udc_writeb(udc, IN0BC, 2);
		udc->ep0state = EP0_END_XFER;
		return;
	case USB_REQ_CLEAR_FEATURE:
		if ((u.r.bRequestType & 0x60) == 0x20) {
			UDC_PRINTK("hgl: is class request = 0x%x 0x%x\n", u.r.bRequestType, u.r.bRequest);
			break;
		}
		if ((u.r.bRequestType == USB_RECIP_DEVICE)
		    && (u.r.wValue == USB_DEVICE_REMOTE_WAKEUP)) {
			UDC_PRINTK("<CTRL> clear remote wakeup feature\n");
			udc->rwk = 0;	/*clear the remote wakeup character */
		} else if ((u.r.bRequestType == USB_RECIP_ENDPOINT)
			   && (u.r.wValue == USB_ENDPOINT_HALT)) {
			ep_num = u.r.wIndex & EP_ADDR_MASK;

			if (ep_num > (AOTG_UDC_NUM_ENDPOINTS - 1)
			    || u.r.wLength > 2)
				goto stall;
			ep = &udc->ep[ep_num];
			if ((ep_num != 0)
			    && (ep->bEndpointAddress != u.r.wIndex))
				goto stall;

			if (ep->bEndpointAddress == 0) {
				udc_clearb_mask(udc, EP0CS, EP0CS_STALL);
			} else {
				udc_clearb_mask(udc, ep->reg_udccon, EPCON_STALL);
				ep->stopped = 0;
				aotg_ep_reset(udc, ep->mask, ENDPRST_TOGRST);
				/*reset the ep toggle */
			}
			UDC_PRINTK("<CTRL> clear %s halt feature\n", ep->ep.name);
		} else
			goto stall;
		/*ACK the status stage */
		handle_status(udc);
		return;
	case USB_REQ_SET_FEATURE:
		if ((u.r.bRequestType == USB_RECIP_DEVICE)) {
			switch (u.r.wValue) {
			case USB_DEVICE_REMOTE_WAKEUP:
				udc->rwk = 1;
				/*clear the remmote wakeup character */
				break;
			case USB_DEVICE_B_HNP_ENABLE:
				UDC_PRINTK("<UDC:handle_setup()>:b_hnp_enable = 1\n");
				udc->gadget.b_hnp_enable = 1;
				//set_b_hnp_en();
				break;
			case USB_DEVICE_A_HNP_SUPPORT:
				UDC_PRINTK("<UDC:handle_setup()>:a_hnp_support = 1\n");
				udc->gadget.a_hnp_support = 1;
				break;
			case USB_DEVICE_A_ALT_HNP_SUPPORT:
				UDC_PRINTK("<UDC:handle_setup()>:a_alt_hnp_support = 1\n");
				udc->gadget.a_alt_hnp_support = 1;
				break;
			default:
				goto stall;
			}
		} else if ((u.r.bRequestType == USB_RECIP_ENDPOINT)
			   && (u.r.wValue == USB_ENDPOINT_HALT)) {
			ep_num = u.r.wIndex & EP_ADDR_MASK;

			if (ep_num > (AOTG_UDC_NUM_ENDPOINTS - 1)
			    || u.r.wLength > 2)
				goto stall;
			ep = &udc->ep[ep_num];
			if ((ep_num != 0)
			    && (ep->bEndpointAddress != u.r.wIndex))
				goto stall;
			if (ep->bEndpointAddress == 0) {
				udc_setb_mask(udc, EP0CS, EP0CS_STALL);
				udc->ep0state = EP0_STALL;
			} else
				udc_setb_mask(udc, ep->reg_udccon, EPCON_STALL);
		} else
			goto stall;

		/*ACK the status stage */
		handle_status(udc);
		return;
	case USB_REQ_SET_ADDRESS:
		UDC_PRINTK("<CTRL>USB_REQ_SET_ADDRESS\n");
		udc->req_pending = 0;
		/*automatically reponse this request by hardware,
		   so hide it to software */
		return;
	case USB_REQ_SET_INTERFACE:
		UDC_PRINTK("<CTRL>USB_REQ_SET_INTERFACE\n");
		udc->req_config = 1;
		if (w_length != 0)
			goto stall;
		goto delegate;
		/*delegate to the upper gadget driver */
	case USB_REQ_SET_CONFIGURATION:
		UDC_PRINTK("<CTRL>USB_REQ_SET_CONFIGURATION\n");
		if (u.r.bRequestType == USB_RECIP_DEVICE) {
			if (w_length != 0)
				goto stall;
			udc->req_config = 1;
			if (w_value == 0) {
				/*enter address state and all endpoint
				   should be disabled except for endpoint0 */
				UDC_PRINTK("<CTRL>disable all ep\n");
				for (i = 1; i < AOTG_UDC_NUM_ENDPOINTS; i++)
					udc_clearb_mask(udc, udc->ep[i].reg_udccon, EPCON_VAL);
			} else {	/*enter configured state */
				UDC_PRINTK("<CTRL>enter configured state\n");
				for (i = 1; i < AOTG_UDC_NUM_ENDPOINTS; i++)
					udc_setb_mask(udc, udc->ep[i].reg_udccon, EPCON_VAL);
			}
		} else
			goto stall;
		/*delegate to the upper gadget driver */
		break;
	default:
		/*delegate to the upper gadget driver */
		break;
	}
	/*gadget drivers see class/vendor specific requests,
	 *{SET, GET}_{INTERFACE, DESCRIPTOR, CONFIGURATION},
	 *and more
	 *The gadget driver may return an error here,
	 *causing an immediate protocol stall.
	 *
	 *Else it must issue a response, either queueing a
	 *response buffer for the DATA stage, or halting ep0
	 *(causing a protocol stall, not a real halt).  A
	 *zero length buffer means no DATA stage.
	 *
	 *It's fine to issue that response after the setup()
	 *call returns.
	 */

delegate:
	UDC_PRINTK("<CTRL>delegate\n");
	if (u.r.bRequestType & USB_DIR_IN)
		udc->ep0state = EP0_IN_DATA_PHASE;
	else
		udc->ep0state = EP0_OUT_DATA_PHASE;

	spin_unlock_irqrestore(&udc->lock, flags);
	status = udc->driver->setup(&udc->gadget, &u.r);	/*delegate */
	spin_lock_irqsave(&udc->lock, flags);
	if (status < 0) {
stall:
		UDC_PRINTK("<CTRL> req %02x.%02x protocol STALL, err  %d\n", u.r.bRequestType, u.r.bRequest, status);
		if (udc->req_config) {
			UDC_PRINTK("<CTRL>config change erro\n");
		}
		udc_setb_mask(udc, EP0CS, EP0CS_STALL);
		udc->req_pending = 0;
		udc->ep0state = EP0_STALL;
	}
	return;
}

static void handle_ep0_in(struct aotg_udc *dev)
{
	struct aotg_udc *udc = dev;
	struct aotg_ep *ep = &udc->ep[0];
	struct aotg_request *req;
	ep->udc_irqs++;
	if (list_empty(&ep->queue))
		req = NULL;
	else
		req = list_entry(ep->queue.next, struct aotg_request, queue);

	UDC_PRINTK("<CTRL>ep0in irq handler, state is %d, queue is %s\n",
		  udc->ep0state, (req == NULL) ? "empty" : "not empty");

	switch (udc->ep0state) {
	case EP0_IN_DATA_PHASE:
		if (req) {
			if (write_ep0_fifo(ep, req))
				udc->ep0state = EP0_END_XFER;
		}
		break;
	case EP0_END_XFER:
		 /*ACK*/
		handle_status(udc);
		/*cleanup */
		udc->req_pending = 0;
		udc->ep0state = EP0_WAIT_FOR_SETUP;
		if (req) {
			done(ep, req, 0);
			req = NULL;
		}
		break;
	case EP0_STALL:
		if (req) {
			done(ep, req, -ESHUTDOWN);
			req = NULL;
		}
		break;
	default:
		UDC_PRINTK("<CTRL>ep0in irq error, odd state %d\n", udc->ep0state);
	}
}

/*
 *	EP0 related operations
 */
static inline int write_ep0_packet(struct aotg_udc *udc, struct aotg_request *req, unsigned max)
{
	u32 *buf;
	unsigned length, count;
	unsigned long addr = EP0INDAT0;

	buf = (u32 *) (req->req.buf + req->req.actual);
	prefetch(buf);
	/*how big will this packet be? */
	length = min(req->req.length - req->req.actual, max);
	req->req.actual += length;

	count = length / 4;	/*wirte in DWORD */
	if ((length % 4) != 0)
		count++;

	while (likely(count--)) {
		udc_writel(udc, addr, *buf);
		buf++;
		addr += 4;
	}
	udc_writeb(udc, IN0BC, length);
	/*arm IN EP0 for the next IN transaction */
	return length;
}

static int read_ep0_fifo(struct aotg_ep *ep, struct aotg_request *req)
{
	u8 *buf, byte;
	unsigned bufferspace, count, length;
	unsigned long addr;
	struct aotg_udc *udc;
  
  udc = ep->dev;
	if (udc_readb(udc, EP0CS) & EP0CS_OUTBSY)	/*data is not ready */
		return 0;
	/*fifo can be accessed validly */
	else {
		buf = req->req.buf + req->req.actual;
		bufferspace = req->req.length - req->req.actual;

		length = count = udc_readb(udc, OUT0BC);
		addr = EP0OUTDAT0;
		//while (count--) {
		for (; count != 0; count--) {
			byte = udc_readb(udc, addr);
			if (unlikely(bufferspace == 0)) {
				/*this happens when the driver's buffer
				 *is smaller than what the host sent.
				 *discard the extra data.
				 */
				if (req->req.status != -EOVERFLOW)
					UDC_PRINTK("%s overflow\n", ep->ep.name);
				req->req.status = -EOVERFLOW;
				break;
			} else {
				*buf++ = byte;
				req->req.actual++;
				bufferspace--;
				addr++;
				ep->dev->stats.read.bytes++;
			}
		}
	}

	UDC_PRINTK("ep0out %d bytes %s %d left %p\n", length,
		  (req->req.actual >= req->req.length) ? "/L" : "",
		  req->req.length - req->req.actual, req);
	if (req->req.actual >= req->req.length)
		return 1;
	return 0;
}

static int write_ep0_fifo(struct aotg_ep *ep, struct aotg_request *req)
{
	unsigned count;
	int is_last;
	struct aotg_udc *udc = ep->dev;

	count = write_ep0_packet(udc, req, EP0_PACKET_SIZE);
	ep->dev->stats.write.bytes += count;

	/*last packet must be a short packet or zlp */
	if (count != EP0_PACKET_SIZE)
		is_last = 1;
	else {
		if ((req->req.length != req->req.actual) || req->req.zero)
			is_last = 0;
		else
			is_last = 1;
	}
	UDC_PRINTK("ep0in %d bytes %s %d left %p\n", count,
		  is_last ? "/L" : "", req->req.length - req->req.actual, req);

	return is_last;
}

static void handle_ep0_out(struct aotg_udc *dev)
{
	struct aotg_udc *udc = dev;
	struct aotg_ep *ep = &udc->ep[0];
	struct aotg_request *req;
	ep->udc_irqs++;

	if (list_empty(&ep->queue)) {
		/* empty queue */
		req = NULL;
	} else
		req = list_entry(ep->queue.next, struct aotg_request, queue);

	UDC_PRINTK("<CTRL>ep0out irq handler, state is %d, queue is %s\n",
		  udc->ep0state, (req == NULL) ? "empty" : "not empty");

	switch (udc->ep0state) {
	case EP0_OUT_DATA_PHASE:
		if (req) {
			if (read_ep0_fifo(ep, req)) {
				/*ACK*/
				handle_status(udc);
				/*cleanup */
				udc->req_pending = 0;
				udc->ep0state = EP0_WAIT_FOR_SETUP;
				done(ep, req, 0);
				req = NULL;
			} else
				udc_writeb(udc, OUT0BC, 0);
			/*write OUT0BC with any value to
			   enable next OUT transaction */
		} else
			UDC_PRINTK("<CTRL>ep0out irq error, queue is empty but state is EP0_OUT_DATA_PHASE\n");
		/*never enter this branch */
		break;
	case EP0_STALL:
		if (req) {
			done(ep, req, -ESHUTDOWN);
			req = NULL;
		}
		break;
	default:
		UDC_PRINTK("<CTRL>ep0out irq error, odd state %d\n",
			  udc->ep0state);
	}

}

static void handle_ep_in(struct aotg_udc *dev, u8 index)
{
	struct aotg_udc *udc = dev;
	struct aotg_ep *ep;
	struct aotg_request *req;
	u8 completed;

	ep = &udc->ep[index];
	ep->udc_irqs++;
	do {
		completed = 0;
		if (likely(!list_empty(&ep->queue)))
			req = list_entry(ep->queue.next,
					 struct aotg_request, queue);
		else {
			req = NULL;
			UDC_PRINTK("<BULK>%s queue is empty, do nothing\n",ep->ep.name);
			return;
		}
		completed = write_fifo(ep, req);
	} while (completed);
}

static void handle_ep_out(struct aotg_udc *dev, u8 index)
{
	struct aotg_udc *udc = dev;
	struct aotg_ep *ep;
	struct aotg_request *req;
	u8 completed;
	ep = &udc->ep[index];
	UDC_PRINTK("%s: data of %s arrives\n", __func__, ep->ep.name);
	ep->udc_irqs++;
	do {
		completed = 0;
		if (likely(!list_empty(&ep->queue)))
			req = list_entry(ep->queue.next,
					 struct aotg_request, queue);
		else {
			req = NULL;
			UDC_PRINTK("<BULK>%s queue is empty, do nothing\n", ep->ep.name);
			return;
		}
		if (!(udc_readb(udc, ep->reg_udccs) & EPCS_BUSY)) {
			UDC_PRINTK("<BULK>%s queue and fifo are all not empty, reading\n", ep->ep.name);
			completed = read_fifo(ep, req);
		} else
			UDC_PRINTK("%s queue is not empty, but fifo has no data, return and wait\n", ep->ep.name);
	} while (completed);
}

static int write_fifo(struct aotg_ep *ep, struct aotg_request *req)
{
	/*list two variables here to tell whether they are equal,
	   just for debugging */
	unsigned descmax;	/*maxpacket assigned by the upper driver */
	unsigned max;		/*maxpacket assigned by udc driver */
	struct  aotg_udc *udc = ep->dev;
	
	descmax = le16_to_cpu(ep->desc->wMaxPacketSize);
	max = ep->maxpacket;
	while (!(udc_readb(udc, ep->reg_udccs) & EPCS_BUSY)) {
		unsigned count;
		int is_last;
		count = write_packet(ep, req, max);
		/*last packet is usually short (or a zlp) */
		if (unlikely(count != max))
			is_last = 1;
		else {
			if (likely(req->req.length != req->req.actual)
			    || req->req.zero)
				is_last = 0;
			else
				is_last = 1;
		}

		UDC_PRINTK("%s wrote %d bytes%s %d left maxpacket %d \
				desc maxpacket %d\n",
				ep->ep.name, count, is_last ? "/L" : "",
				req->req.length - req->req.actual,
				max, descmax);
		/*requests complete when all IN data is in the FIFO */
		if (is_last) {
			done(ep, req, 0);
			if (list_empty(&ep->queue)) {
				UDC_PRINTK("%s queue is empty now,\
						do nothing\n", ep->ep.name);
				pio_irq_disable(ep);
#if 0
				if (is_dma_mode()) {
					if (unlikely(!list_empty(&ep->queue))) {
						req = list_entry(ep->queue.next,
								 struct
								 aotg_request,
								 queue);
						aotg_udc_kick_dma(ep, req);
						return 0;
					}
				}
#endif
			}
			//UDC_PRINTK("data transferred ?  reg_udccs %02x\n",
      //udc_readb(udc, ep->reg_udccs));
			return 1;
		}
	}
	return 0;
}

static int read_fifo(struct aotg_ep *ep, struct aotg_request *req)
{
  struct  aotg_udc *udc = ep->dev;
	while (!(udc_readb(udc, ep->reg_udccs) & EPCS_BUSY)) {
		u8 *bytebuf, byte;
		u32 *buf, dword;
		unsigned bufferspace, count, remain, length, is_short;
		unsigned long fifoaddr;
		int i;

		buf = (u32 *) (req->req.buf + req->req.actual);
		bufferspace = req->req.length - req->req.actual;

		length = udc_readw(udc, ep->reg_udcbc);
		is_short = (length < ep->maxpacket);
		if (unlikely(length > bufferspace)) {
			/*this happens when the driver's buffer
			 *is smaller than what the host sent.
			 *discard the extra data.
			 */
			count = bufferspace;
			if (req->req.status != -EOVERFLOW)
				UDC_PRINTK("%s: %s overflow\n", __func__, ep->ep.name);
			req->req.status = -EOVERFLOW;
		} else
			count = length;
			
		ep->read.bytes += count;
		ep->read.ops++;
		remain = count % 4;
		count = count / 4;	/*read in DWORD */

		while (count) {
			dword = udc_readl(udc, ep->reg_udcfifo);
			*buf++ = dword;
			req->req.actual += 4;
			count--;
		}

		if (remain != 0) {	/*read in BYTE */
			bytebuf = req->req.buf + req->req.actual;
			fifoaddr = ep->reg_udcfifo;
			for (i = 0; i < remain; i++) {
				byte = udc_readb(udc, fifoaddr++);
				*bytebuf++ = byte;
				req->req.actual++;
			}
		}
    /*arm this ep(sub buffer) for the next OUT transaction */		
		udc_writeb(udc, ep->reg_udccs, 0);	
		
		if (is_short || req->req.length == req->req.actual) {
			done(ep, req, 0);
			if (list_empty(&ep->queue)) {
				UDC_PRINTK("%s queue is empty now, do nothing\n", ep->ep.name);
				pio_irq_disable(ep);
			}
			return 1;
		}
	}
	return 0;
}

static void udc_enable(struct aotg_udc *dev)
{
  struct aotg_udc *udc =dev;
  
	dev->ep0state = EP0_WAIT_FOR_SETUP;
	dev->stats.irqs = 0;
	dev->state = UDC_IDLE;
	dev->gadget.dev.parent->power.power_state = PMSG_ON;
	dev->gadget.dev.power.power_state = PMSG_ON;
	UDC_PRINTK("<UDC_ENABLE> %p, AOTG enters :%d state\n",
		 dev, udc_readb(udc, OTGSTATE));

	UDC_PRINTK( "Pull up D+\n");
	/*pull up D+ to let  host  see us */
//	if ((usb_readl(plat_data->usbecs) & USB0_ECS_BVALID) == 0) {
//		//schedule_work(&dev->notifier_work);
//	} else
  dplus_up(udc);
}

static void udc_disable(struct aotg_udc *dev)
{
  struct aotg_udc *udc = dev;
  
	UDC_PRINTK("<UDC_DISABLE> %p\n", dev);
	/*FIX ME: clear some irqs */
	UDC_PRINTK( "Pull down D+\n");
	/*Pull down D+ */	
	dplus_down(udc);

	/*Clear software state */
	dev->ep0state = EP0_WAIT_FOR_SETUP;
	dev->gadget.speed = USB_SPEED_UNKNOWN;
	dev->state = UDC_DISABLE;
}

static int pullup(struct aotg_udc *udc, int is_active)
{
	is_active = is_active && udc->softconnect;
	UDC_PRINTK("<PULL_UP> %s\n", is_active ? "active" : "inactive");
	if (is_active)
		udc_enable(udc);
	else
		udc_disable(udc);

	return 0;
}

static int write_packet(struct aotg_ep *ep, struct aotg_request *req, unsigned max)
{
	u32 *buf;
	unsigned length, count;
	struct aotg_udc *udc = ep->dev;

	buf = (u32 *) (req->req.buf + req->req.actual);
	prefetch(buf);

	/*how big will this packet be? */
	length = min(req->req.length - req->req.actual, max);
	req->req.actual += length;
	ep->write.bytes += length;
	ep->write.ops++;
	count = length / 4;	/*wirte in DWORD */
	if ((length % 4) != 0)
		count++;
	while (likely(count--)) {
		udc_writel(udc, ep->reg_udcfifo, *buf);
		buf++;
	}
	udc_writew(udc, ep->reg_udcbc, length);
	/*tell the hw how many bytes is loaded actually */
	udc_writeb(udc, ep->reg_udccs, 0);
	/*arm IN EP for the next IN transaction */
	return length;
}

irqreturn_t aotg_udc_irq(int irq, void *_dev)
{
	struct aotg_udc *udc = the_controller;
	irqreturn_t retval = IRQ_HANDLED;
	u8 irqvector,external_irq;
	u8 otgint, otg_state;
	u8 ep_offset,ep_dir;
  
	spin_lock_irqsave(&udc->lock, flags);	
	
  external_irq = udc_readb(udc, USBEIRQ);
  udc_writeb(udc, USBEIRQ, external_irq);  
	/* connect disconnect happened */  
  if ((external_irq & 0x50) != 0) {
    
    UDC_PRINTK( "external irq: %x\n",external_irq);                 
    spin_unlock_irqrestore(&udc->lock, flags);
  	return retval;
  	
	}
	
  //add for dma handle
  if (aotg_dma_is_irq(udc->dma_nr0)) {
    
    aotg_dma_wait_finish(udc->dma_nr1);
    aotg_dma_clear_pend(udc->dma_nr1);
    aotg_udc_dma_handler(udc);
    spin_unlock_irqrestore(&udc->lock, flags);
    
    return retval;
	}
	
	irqvector = udc_readb(udc, IVECT);
	switch (irqvector) {
	case UIV_OTGIRQ:
		otgint = udc_readb(udc, OTGIEN) & udc_readb(udc, OTGIRQ);
		udc_writeb(udc, OTGIRQ, otgint);
		otg_state = udc_readb(udc, OTGSTATE);
		UDC_PRINTK("OTG_STATE is %x\n", otg_state);
		switch (otgint) {
		case OTGIRQ_PERIPH:
			udc_setb_mask(udc, USBIEN, USBIEN_URES |USBIEN_HS | USBIEN_SUDAV |USBIEN_SUSP);
			udc->state = UDC_ACTIVE;
			udc->disconnect = 0;
			break;
		case OTGIRQ_IDLE:
			UDC_PRINTK("Enter B-IDLE\n");
			udc->state = UDC_IDLE;
			break;
		default:
			break;

		}
		UDC_PRINTK( "UIV_OTG_IRQ %x\n", otgint);	
		break;
	case UIV_USBRESET:
		udc_writeb(udc, USBIRQ, 0xdf);
		udc_writeb(udc, OTGIRQ, 0xff);
		udc_writeb(udc, IN07IRQ, 0xff);
		udc_writeb(udc, OUT07IRQ, 0xff);
		udc_setb_mask(udc, IN07IEN, 1);
		udc_setb_mask(udc, OUT07IEN, 1);

		if (udc->dma_chan >=0) {
			/* reset dma channel */
			aotg_dma_reset(udc->dma_nr1);
		}

		UDC_PRINTK("gadget %s, USB reset done\n", udc->driver->driver.name);
		/*when bus reset, we assume the current speed is FS */
		udc->gadget.speed = USB_SPEED_FULL;
		udc->gadget.b_hnp_enable = 0;
		udc->gadget.a_hnp_support = 0;
		udc->gadget.a_alt_hnp_support = 0;		
		udc->highspeed = 0;
		udc_ep_packet_config(USB_SPEED_FULL, udc);
		mdelay(4);
		udc->reset_cnt++;
		break;
	case UIV_HSPEED:
		udc_setb_mask(udc, USBIRQ, USBIRQ_HS);
		udc->gadget.speed = USB_SPEED_HIGH;
		udc->highspeed = 1;	
		udc_ep_packet_config(USB_SPEED_HIGH, udc);
		if(udc->reset_cnt > 2)
		{
		  udc_setb_mask(udc, BKDOOR, 0x80);
		}
		break;
	case UIV_SUSPEND:
		udc_setb_mask(udc, USBIRQ, USBIRQ_SUSP);
		udc->state = UDC_SUSPEND;
		udc->reset_cnt = 0;
		break;
	case UIV_SUDAV:
		udc_setb_mask(udc, USBIRQ, USBIRQ_SUDAV);
		handle_setup(udc);
		break;
    case UIV_EP0IN:
    case UIV_EP0OUT:
    case UIV_EP1IN:
    case UIV_EP1OUT:
    case UIV_EP2IN:    
	case UIV_EP2OUT:
    case UIV_EP3IN:
    case UIV_EP3OUT:
    case UIV_EP4IN:
    case UIV_EP4OUT:
    case UIV_EP5IN:
    case UIV_EP5OUT:
        ep_offset = (irqvector - EP_IRQ_BASE) / EP_IRQ_OFFSET;
        ep_dir = (irqvector - EP_IRQ_BASE - 2) % EP_IRQ_OFFSET ? 1 : 0;/*0:in  1:out*/
        if(ep_dir){
		      udc_setb_mask(udc, OUT07IRQ, 1 << ep_offset);
  		    if(ep_offset)
  		        handle_ep_out(udc, 2 * (ep_offset -1) + 1);
  		    else
  		        handle_ep0_out(udc);
		    }else{
  		    udc_setb_mask(udc, IN07IRQ, 1 << ep_offset);
  		    if(ep_offset)
  		        handle_ep_in(udc, 2 * ep_offset);
  		    else
  		        handle_ep0_in(udc);
		    }    
		break;

	default:
		UDC_PRINTK( "The ivent is 0x%x.\n", irqvector);
		retval = IRQ_NONE;
	}
		
	spin_unlock_irqrestore(&udc->lock, flags);
	return retval;
}


#ifdef CONFIG_PM
static int aotg_udc_suspend(struct platform_device *dev, pm_message_t state)
{
	//struct aotg_udc	*udc = platform_get_drvdata(dev);

	return 0;
}

static int aotg_udc_resume(struct platform_device *dev)
{
	//struct aotg_udc *udc = platform_get_drvdata(dev);

	return 0;
}
#else
#define aotg_udc_suspend	NULL
#define aotg_udc_resume	NULL
#endif


static void udc_reinit(struct aotg_udc *dev)
{
	unsigned i;

	/*device/ep0 records init */
	INIT_LIST_HEAD(&dev->gadget.ep_list);
	INIT_LIST_HEAD(&dev->gadget.ep0->ep_list);

	dev->ep0state = EP0_WAIT_FOR_SETUP;
	dev->gadget.speed = USB_SPEED_UNKNOWN;
	dev->state = UDC_UNKNOWN;
	memset(&dev->stats, 0, sizeof(struct udc_stats));

	/*basic endpoint records init */
	for (i = 0; i < AOTG_UDC_NUM_ENDPOINTS; i++) {
		struct aotg_ep *ep = &dev->ep[i];

		if (i != 0)
			list_add_tail(&ep->ep.ep_list, &dev->gadget.ep_list);

		ep->desc = 0;
		ep->stopped = 0;
		INIT_LIST_HEAD(&ep->queue);
		ep->udc_irqs = ep->dma_irqs = 0;
	}
}

void aotg_udc_hardware_init(struct aotg_udc *udc)
{
  struct aotg_plat_data *plat_data = udc->dev->platform_data;  
  /*usb2 clk0 enable*/
  aotg_udc_clk_enable(udc);
  
  local_irq_save(flags); 
  
  aotg_udc_dev_reset(udc);  
  
  aotg_udc_phy_config(udc);
  
	udc_writeb(udc, BKDOOR, (1 << 4));	/*clk40m */	
  usb_writel(0, plat_data->usbecs);
	udelay(1);
	usb_writel((usb_readl(plat_data->usbecs) | (0x0F00003F)), plat_data->usbecs);
	mdelay(5);
	udc_setb_mask(udc, USBCS, USBCS_DISCONN);
	
  local_irq_restore(flags);
  	
  udc_writeb(udc, USBIRQ, 0xff);
  udc_writeb(udc, OTGIRQ, 0xff);
  udc_writeb(udc, USBEIRQ, udc_readb(udc, USBEIRQ));
  udc_writeb(udc, OTGIEN, 0xff);
  udc_writeb(udc, USBEIRQ, 0x0D);
  
  /*enable extern irq for usb module*/
  extern_irq_enable(udc);

  return;
}

static int aotg_udc_endpoint_config(struct aotg_udc *udc, struct usb_gadget_driver *driver)
{	
      
	UDC_BULK_EP(1, "ep1out", USB_DIR_OUT | 1, EPCON_BUF_DOUBLE);
	UDC_BULK_EP(2, "ep1in", USB_DIR_IN | 1, EPCON_BUF_DOUBLE);
	UDC_BULK_EP(3, "ep2in", USB_DIR_IN | 2, EPCON_BUF_SINGLE);
	UDC_BULK_EP(4, "ep2out", USB_DIR_OUT | 2, EPCON_BUF_SINGLE); 

	if(!strcmp(driver->function, "g_webcam")){
		UDC_ISO_EP(2,"ep2in", USB_DIR_IN | 2, EPCON_BUF_TRIPLE);
		UDC_INT_EP(3,"ep1in", USB_DIR_IN | 1, EPCON_BUF_SINGLE);
	}
	udc_ep_packet_config(USB_SPEED_FULL, udc);

	return 0;
}

static struct aotg_udc memory = {
  .lock = __SPIN_LOCK_UNLOCKED(memory.lock),
	.gadget = {
		   .ops = &aotg_udc_ops,
		   .ep0 = &memory.ep[0].ep,
		   .name = driver_name,
		   .dev = {
			   .init_name	= "gadget",
			   },
		   },
	/*control endpoint */
	.ep[0] = {
		  .ep = {
			 .name = ep0name,
			 .ops = &aotg_ep_ops,
			 .maxpacket = EP0_PACKET_SIZE,
			 },
		  .dev = &memory,
		  .maxpacket = EP0_PACKET_SIZE,
		  },
	/*bulk out endpoint */
	.ep[1] = {
		  .ep = {
			 .ops = &aotg_ep_ops,
			 },
		  .dev = &memory,
		  .bEndpointAddress = 1,
		  .mask = 1,
		  .reg_udccs = OUT1CS,
		  .reg_udccon = OUT1CON,
		  .reg_udcbc = OUT1BCL,
		  .reg_udcfifo = FIFO1DATA,
		  .reg_maxckl = HCIN1MAXPCKL,
		  .reg_fifostaddr = OUT1STARTADDRESS,	  
		  },

	/*bulk in endpoint */
	.ep[2] = {
		  .ep = {
			 .ops = &aotg_ep_ops,
			 },
		  .dev = &memory,
		  .bEndpointAddress = USB_DIR_IN | 1,
		  .mask = USB_UDC_IN_MASK | 1,
		  .reg_udccs = IN1CS,
		  .reg_udccon = IN1CON,
		  .reg_udcbc = IN1BCL,
		  .reg_udcfifo = FIFO1DATA,
		  .reg_maxckl = HCOUT1MAXPCKL,
      .reg_fifostaddr = IN1STARTADDRESS,		  
		  },
	/*bulk in endpoint */
	.ep[3] = {
		  .ep = {
			 .ops = &aotg_ep_ops,
			 },
		  .dev = &memory,
		  .bEndpointAddress = USB_DIR_IN | 1,
		  .mask = USB_UDC_IN_MASK | 1,
		  .reg_udccs = IN2CS,
		  .reg_udccon = IN2CON,
		  .reg_udcbc = IN2BCL,
		  .reg_udcfifo = FIFO2DATA,
		  .reg_maxckl = HCOUT2MAXPCKL,
          .reg_fifostaddr = IN2STARTADDRESS,		  
		  },

	/*bulk out endpoint */
	.ep[4] = {
		  .ep = {
			 .ops = &aotg_ep_ops,
			 },
		  .dev = &memory,
		  .bEndpointAddress = 2,
		  .mask = 2,
		  .reg_udccs = OUT2CS,
		  .reg_udccon = OUT2CON,
		  .reg_udcbc = OUT2BCL,
		  .reg_udcfifo = FIFO2DATA,
		  .reg_maxckl = HCIN2MAXPCKL,
      .reg_fifostaddr = OUT2STARTADDRESS,
		  },  
     /*bulk out endpoint */
     .ep[5] = {
           .ep = {
              .ops = &aotg_ep_ops,
              },
           .dev = &memory,
           .bEndpointAddress = USB_DIR_IN | 3,
           .mask = USB_UDC_IN_MASK | 3,
           .reg_udccs = IN3CS,
           .reg_udccon = IN3CON,
           .reg_udcbc = IN3BCL,
           .reg_udcfifo = FIFO3DATA,
           .reg_maxckl = HCOUT3MAXPCKL,
           .reg_fifostaddr = IN3STADDR,          
           },
     
     /*bulk out endpoint */
     .ep[6] = {
           .ep = {
              .ops = &aotg_ep_ops,
              },
           .dev = &memory,
           .bEndpointAddress = 3,
           .mask = 3,
           .reg_udccs = OUT3CS,
           .reg_udccon = OUT3CON,
           .reg_udcbc = OUT3BCL,
           .reg_udcfifo = FIFO3DATA,
           .reg_maxckl = HCIN3MAXPCKL,
           .reg_fifostaddr = OUT3STADDR,
           },  
      /*bulk in endpoint */
      .ep[7] = {
            .ep = {
               .ops = &aotg_ep_ops,
               },
            .dev = &memory,
            .bEndpointAddress = USB_DIR_IN | 4,
            .mask = USB_UDC_IN_MASK | 4,
            .reg_udccs = IN4CS,
            .reg_udccon = IN4CON,
            .reg_udcbc = IN4BCL,
            .reg_udcfifo = FIFO4DATA,
            .reg_maxckl = HCOUT4MAXPCKL,
            .reg_fifostaddr = IN4STADDR,          
            },
      
      /*bulk out endpoint */
      .ep[8] = {
            .ep = {
               .ops = &aotg_ep_ops,
               },
            .dev = &memory,
            .bEndpointAddress = 4,
            .mask = 4,
            .reg_udccs = OUT4CS,
            .reg_udccon = OUT4CON,
            .reg_udcbc = OUT4BCL,
            .reg_udcfifo = FIFO4DATA,
            .reg_maxckl = HCIN4MAXPCKL,
            .reg_fifostaddr = OUT4STADDR,
            },  
       /*bulk in endpoint */
       .ep[9] = {
             .ep = {
                .ops = &aotg_ep_ops,
                },
             .dev = &memory,
             .bEndpointAddress = USB_DIR_IN | 5,
             .mask = USB_UDC_IN_MASK | 5,
             .reg_udccs = IN5CS,
             .reg_udccon = IN5CON,
             .reg_udcbc = IN5BCL,
             .reg_udcfifo = FIFO5DATA,
             .reg_maxckl = HCOUT5MAXPCKL,
             .reg_fifostaddr = IN5STADDR,          
             },
       
       /*bulk out endpoint */
       .ep[10] = {
             .ep = {
                .ops = &aotg_ep_ops,
                },
             .dev = &memory,
             .bEndpointAddress = 5,
             .mask = 5,
             .reg_udccs = OUT5CS,
             .reg_udccon = OUT5CON,
             .reg_udcbc = OUT5BCL,
             .reg_udcfifo = FIFO5DATA,
             .reg_maxckl = HCIN5MAXPCKL,
             .reg_fifostaddr = OUT5STADDR,
             },  

};

static int aotg_udc_probe(struct platform_device *pdev)
{
	struct resource *regs;
	struct aotg_udc *udc = &memory;
	int retval = 0;	

	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs)
		return -ENXIO;
	udc->irq = platform_get_irq(pdev, 0);
	if (udc->irq < 0)
		return udc->irq;
	UDC_PRINTK("udc->irq is %02x\n", udc->irq);

	udc->dev = &pdev->dev;
	udc->transceiver =NULL; //usb_get_transceiver();????????
	
	mutex_init(&udc->rw_lock);
  
  udc->clk = clk_get_sys(CLK_NAME_DEVPLL, NULL); /*根据clk_name获取clk结构体*/
  if (IS_ERR(udc->clk)){
      UDC_ERR("<AOTG_UDC_PROBE>get clock err!\n");
      retval = PTR_ERR(udc->clk);
      goto err_clk;
    }			
  clk_enable(udc->clk);

	if (!request_mem_region(regs->start, regs->end - regs->start + 1, dev_name(&pdev->dev))) {
		dev_err(&pdev->dev, "<AOTG_PROBE>request_mem_region failed\n");
		retval = -EBUSY;
		goto err_map;
	}

	udc->regs = ioremap(regs->start, resource_size(regs));
	if (!udc->regs) {
		dev_err(&pdev->dev, "Unable to map UDC I/O memory\n");
		goto err_map;
	}
  aotg_udc_hardware_init(udc);

	device_initialize(&udc->gadget.dev);
	udc->gadget.dev.parent = &pdev->dev;
	udc->gadget.dev.dma_mask = pdev->dev.dma_mask;

	the_controller = udc;
	platform_set_drvdata(pdev, udc);
	udc_reinit(udc);
	
  if(udc->gadget.dev.dma_mask){	
    retval = aotg_dma_request(udc);
    if (retval >= 0) {                   
      udc->dma_chan = retval;
      udc->dma_nr0 = retval;
      udc->dma_nr1 = retval + 1;
			aotg_dma_reset(udc->dma_nr1);
			aotg_dma_enable_irq(udc->dma_nr1);
			aotg_dma_clear_pend(udc->dma_nr1);
    }else{
 			UDC_PRINTK("<UDC_INSTALL>Wow! Can't register IRQ for DMA\n");       
    }
  }else{
      udc->dma_chan = -1;
  }
  	
	/* irq setup after old hardware state is cleaned up */
	//retval = (request_threaded_irq(udc->irq, NULL, aotg_udc_irq_thread,
  //    IRQF_DISABLED |IRQF_ONESHOT, driver_name, udc)!= 0)? 1: 0;	
	retval = request_irq(udc->irq, aotg_udc_irq,
			IRQF_SHARED, driver_name, udc);		
	if (retval != 0) {
    dev_err(udc->dev, "%s: can't get irq %i, err %d\n",
    driver_name, udc->irq, retval);
    UDC_ERR("<AOTG_UDC_PROBE>register irq err");
		goto err_irq;
	}
	retval = usb_add_gadget_udc(&pdev->dev, &udc->gadget);
	if (retval)
		goto err_add_udc;

	create_debug_files(udc);	
  
	return 0;
err_add_udc:
	free_irq(udc->irq, udc);
err_irq:
	iounmap(udc->regs);
err_map:
	clk_disable(udc->clk);
err_clk:
	return retval;
}

static int __exit aotg_udc_remove(struct platform_device *_dev)
{	
	struct aotg_udc *udc = platform_get_drvdata(_dev);

	usb_del_gadget_udc(&udc->gadget);
	usb_gadget_unregister_driver(udc->driver);
	free_irq(udc->irq, udc);
	remove_debug_files(udc);

	//usb_put_transceiver(udc->transceiver);
	udc->transceiver = NULL;
	platform_set_drvdata(_dev, NULL);
	the_controller = NULL;
	clk_disable(udc->clk);
	iounmap(udc->regs);
	
	return 0;
}


static struct aotg_plat_data otg_data_0 = {
	.usbecs = (void __iomem *)USB2_0ECS,
	.usbecs_bits = USB_ECS_WKUPBVLDEN | USB_ECS_DEVICE_MOD,
	.usbpll = (void __iomem *)CMU_USBPLL,
	.usbclk_bits = 0x3f00,//CMU_USB2PLL_0CCE | CMU_USB2PHY_0CE | CMU_USB2PLL_0EN,	
	.devrst = (void __iomem *)CMU_DEVRST1,
	.devrst_bits = CMU_DEVRST1_USB0,
	.devclk = (void __iomem *)CMU_DEVCLKEN0,
	.devclk_bits = 0,
	.mon_flags = 0,
	.no_hs = 0,
};

static struct resource udc_resources_0[] = {
	[0] = {
       .start = AOTG0_BASE,
       .end = AOTG0_BASE + AOTG_REGS_SIZE - 1,
       .flags = IORESOURCE_MEM,
	},
	
	[1] = {
		.start = IRQ_ASOC_USB2_0,
		.end = IRQ_ASOC_USB2_0,
		.flags = IORESOURCE_IRQ,
	},
};


static u64 udc_dmamask_0 = DMA_BIT_MASK(32);

static struct platform_device udc_device_0 = {
	.name = "aotg_udc",
	.id = 0,
	.dev = {
		.dma_mask = &udc_dmamask_0,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &otg_data_0,
	},
    .num_resources = ARRAY_SIZE(udc_resources_0),
    .resource = udc_resources_0,
};

static struct platform_device *udc_device[] = {
	&udc_device_0,
};

static struct platform_driver udc_driver = {
	.driver = {
		.name = "aotg_udc",
		.owner = THIS_MODULE,
	},
	.probe = aotg_udc_probe,
	.remove = __exit_p(aotg_udc_remove),
	.suspend      = aotg_udc_suspend, 
	.resume       = aotg_udc_resume, 
};

static int __init aotg_usb_init(void)
{
	int status;
	status = platform_add_devices(udc_device, ARRAY_SIZE(udc_device));
	if (status){
		UDC_PRINTK("Can't register UDC platform device\n");
		return status;
	}
	platform_driver_register(&udc_driver);
	return 0;
}
module_init(aotg_usb_init);

static void __exit aotg_usb_exit(void)
{
	int i;
	
	for (i = 0; i < ARRAY_SIZE(udc_device); i++) {
		platform_device_del(udc_device[i]);	
	}
	platform_driver_unregister(&udc_driver);	
}
module_exit(aotg_usb_exit);

#if 0
static int udc_char_read_poll(struct aotg_ep *ep, char *buf, unsigned int buf_len)
{
	u8 *bytebuf, byte;
	u32 *buf_u32, dword;
	unsigned int count, remain, len, is_short;
	unsigned long fifoaddr;
	unsigned int actual = 0;
	int i = 0;

	buf_u32 = (u32 *) buf;
	ep->read.bytes = 0;

	while ((udc_readb(udc, ep->reg_udccs) & EPCS_BUSY) != 0) 
	{
		/* i++ is used just for delay, wait for a char come in. */
		i++;
	}

	if ((udc_readb(udc, ep->reg_udccs) & EPCS_BUSY) == 0) 
	{
		len = aotg_readw(ep->reg_udcbc);
		is_short = (len < ep->maxpacket);
		if (unlikely(len > buf_len)) {
			/*this happens when the driver's buffer
			 *is smaller than what the host sent.
			 *discard the extra data.
			 */
			count = buf_len;
		}
		else
		{
			count = len;
		}
		ep->read.bytes += count;
		ep->read.ops++;
		remain = count % 4;
		count = count / 4;	/*read in DWORD */

		while (count) {
			dword = udc_readl(udc,ep->reg_udcfifo);
			*buf_u32++ = dword;
			actual += 4;
			count--;
		}

		if (remain != 0) {	/*read in BYTE */
			bytebuf = buf + actual;
			fifoaddr = ep->reg_udcfifo;
			for (i = 0; i < remain; i++) {
				byte = udc_readb(udc, fifoaddr++);
				*bytebuf++ = byte;
				actual++;
			}
		}
		/*arm this ep(sub buffer) for the next OUT transaction */
		udc_writeb(udc, ep->reg_udccs, 0);
		/* i=0 is used just for delay. */
		i = 0;
		actual = 0;
	}

	pio_irq_disable(ep);

	return (int)ep->read.bytes;
}

static int udc_char_write_poll(struct aotg_ep *ep, char *buf, unsigned int buf_len)
{
	u32 *buf_u32;
	int len, count;

	ep->write.bytes = 0;
	len = 0;
	while ((udc_readb(udc, ep->reg_udccs) & EPCS_BUSY) != 0)
	{
		len++;	/* do nothing. */
	}

	buf_u32 = (u32 *) buf;
	prefetch(buf_u32);

	/*how big will this packet be? */
	len = buf_len;
	ep->write.bytes += len;
	ep->write.ops++;
	count = len / 4;	/*wirte in DWORD */
	if ((len % 4) != 0)
		count++;
	while (likely(count--)) {
		udc_writel(udc, ep->reg_udcfifo, *buf_u32);
		buf_u32++;
	}
	aotg_writew(len, ep->reg_udcbc);
	/*tell the hw how many bytes is loaded actually */
	udc_writeb(udc, ep->reg_udccs, 0);
	/*arm IN EP for the next IN transaction */
	return len;
}

/* 
 * input:
 *   direct -- 0 is read, 1 is write. 
 */
int udc_char_rw_poll(int direct, struct usb_ep *_ep, char *buf, unsigned int buf_len)
{
	struct aotg_ep *ep;

	ep = container_of(_ep, struct aotg_ep, ep);

	if (direct == 0)
	{
		return udc_char_read_poll(ep, buf, buf_len);
	}
	else
	{
		return udc_char_write_poll(ep, buf, buf_len);
	}
	return 0;
}
EXPORT_SYMBOL(udc_char_rw_poll);
#endif
MODULE_LICENSE("GPL");

