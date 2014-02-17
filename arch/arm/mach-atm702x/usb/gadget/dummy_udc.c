/*
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file "COPYING" in the main directory of this archive
 *  for more details.
 *  Copyright (A) 2012 Actions Semi Inc.
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

#include "dummy_udc.h"


static const char driver_name[] = "dummy_udc";
static struct dummy_udc *the_controller;

static int dummy_udc_endpoint_config(struct dummy_udc *udc, struct usb_gadget_driver *driver);
static inline void dummy_ep_setup(struct dummy_udc *udc, u8 index, char *name, u8 addr, u8 type, u8 buftype);


static int dummy_udc_start(struct usb_gadget_driver *driver,
		int (*bind)(struct usb_gadget *))
{
	struct dummy_udc *udc = the_controller;
	int retval;

	/* first hook up the driver ... */
	udc->driver = driver;
	udc->gadget.dev.driver = &driver->driver;

	retval = device_add(&udc->gadget.dev);
	if (retval) {
		dev_err(udc->dev, "device_add error %d\n", retval);
		goto add_fail;
	}
	
	dummy_udc_endpoint_config(udc, driver);	
	
	retval = bind(&udc->gadget);
  if (retval) {
    dev_err(udc->dev, "bind to driver %s --> error %d\n",
			driver->driver.name, retval);
		goto bind_fail;
	}

	return 0;
bind_fail:
	device_del(&udc->gadget.dev);	
add_fail:
	udc->driver = NULL;
	udc->gadget.dev.driver = NULL;
	return retval;
}

static void dummy_udc_release(struct device *dev)
{
  unsigned int i;
  i = 1;
}


static int dummy_udc_stop(struct usb_gadget_driver *driver)
{
	struct dummy_udc *udc = the_controller;

	if (!udc)
		return -ENODEV;
	if (!driver || driver != udc->driver || !driver->unbind)
		return -EINVAL;
  
  udc->driver->disconnect(&udc->gadget);

	driver->unbind(&udc->gadget);
	udc->driver = NULL;

	device_del(&udc->gadget.dev);

	return 0;
}


static int dummy_ep_enable(struct usb_ep *_ep,
			  const struct usb_endpoint_descriptor *desc)
{
	return 0;
}

static int dummy_ep_disable(struct usb_ep *_ep)
{
	return 0;
}

static int
dummy_ep_queue(struct usb_ep *_ep, struct usb_request *_req, unsigned gfp_flags)
{
	return 0;
}

static int dummy_ep_dequeue(struct usb_ep *_ep, struct usb_request *_req)
{
	return 0;
}



static struct usb_request *dummy_ep_alloc_request(struct usb_ep *_ep,
						 unsigned gfp_flags)
{
	struct dummy_request *req;

	req = kmalloc(sizeof(*req), gfp_flags);
	if (!req)
		return NULL;
	memset(req, 0, sizeof(*req));
	INIT_LIST_HEAD(&req->queue);
	return &req->req;
}


static void dummy_ep_free_request(struct usb_ep *_ep, struct usb_request *_req)
{
	struct dummy_request *req;

	req = container_of(_req, struct dummy_request, req);
	kfree(req);

}

static int dummy_ep_set_halt(struct usb_ep *_ep, int value)
{
	return 0;
}

static void dummy_ep_fifo_flush(struct usb_ep *_ep)
{
  
}


static struct usb_ep_ops dummy_ep_ops = {
  .enable = dummy_ep_enable,
	.disable = dummy_ep_disable,
  
	.alloc_request = dummy_ep_alloc_request,
	.free_request = dummy_ep_free_request,
	
  .queue = dummy_ep_queue,
	.dequeue = dummy_ep_dequeue,

	.set_halt = dummy_ep_set_halt,
	.fifo_flush = dummy_ep_fifo_flush,	/*not sure */
};

static int dummy_udc_get_frame(struct usb_gadget *_gadget)
{
	u16 frmnum = 0;
	return frmnum;
}

static int dummy_udc_wakeup(struct usb_gadget *_gadget)
{
	return 0;
}

static int dummy_udc_vbus_session(struct usb_gadget *_gadget, int is_active)
{
	return 0;
}

static int dummy_udc_pullup(struct usb_gadget *_gadget, int is_on)
{
	return 0;
}

static int dummy_udc_vbus_draw(struct usb_gadget *_gadget, unsigned mA)
{
	return 0;
}

static const struct usb_gadget_ops dummy_udc_ops = {
	.start		= dummy_udc_start,
	.stop     = dummy_udc_stop,
  .get_frame	= dummy_udc_get_frame,
	.wakeup		= dummy_udc_wakeup,
	.pullup		= dummy_udc_pullup,
	.vbus_session	= dummy_udc_vbus_session,
	.vbus_draw	= dummy_udc_vbus_draw,
};


static struct dummy_udc memory = {
	.gadget = {
		   .ops = &dummy_udc_ops,
		   .name = driver_name,
		   .dev = {
			   .init_name	= "gadget",
			   },		   
		   },
	.ep[0]  = {
		  .ep = {
			 .ops = &dummy_ep_ops,
			 },
      .dev = &memory,
		  },
  .ep[1]  = {
      .ep = {
        .ops = &dummy_ep_ops,
			 },
      .dev = &memory,
		  },
  .ep[2]  = {
      .ep = {
        .ops = &dummy_ep_ops,
			 },
      .dev = &memory,
		  },
  .ep[3]  = {
      .ep = {
        .ops = &dummy_ep_ops,
			 },
      .dev = &memory,
		  },
  .ep[4]  = {
      .ep = {
        .ops = &dummy_ep_ops,
			 },
      .dev = &memory,
		  },
  .ep[5]  = {
      .ep = {
        .ops = &dummy_ep_ops,
			 },
      .dev = &memory,
		  },
  .ep[6]  = {
      .ep = {
        .ops = &dummy_ep_ops,
			 },
      .dev = &memory,
		  },
  .ep[7]  = {
      .ep = {
        .ops = &dummy_ep_ops,
			 },
      .dev = &memory,
		  },	 
  .ep[8]  = {
      .ep = {
        .ops = &dummy_ep_ops,
			 },
      .dev = &memory,
		  },			   
};

static void udc_reinit(struct dummy_udc *dev)
{
	unsigned i;

	/*device/ep0 records init */
	INIT_LIST_HEAD(&dev->gadget.ep_list);
	INIT_LIST_HEAD(&dev->gadget.ep0->ep_list);

	/*basic endpoint records init */
	for (i = 0; i < 9; i++) {
		struct dummy_ep *ep = &dev->ep[i];
		if (i != 0)
			list_add_tail(&ep->ep.ep_list, &dev->gadget.ep_list);
		INIT_LIST_HEAD(&ep->queue);
	}
}

static int dummy_udc_endpoint_config(struct dummy_udc *udc, struct usb_gadget_driver *driver)
{	      
  struct dummy_ep *ep;
  
	UDC_BULK_EP(1, "ep1out", USB_DIR_OUT | 1, EPCON_BUF_DOUBLE);
	UDC_BULK_EP(2, "ep1in", USB_DIR_IN | 1, EPCON_BUF_DOUBLE);
  UDC_BULK_EP(3, "ep2out", USB_DIR_OUT | 1, EPCON_BUF_DOUBLE);
	UDC_BULK_EP(4, "ep2in", USB_DIR_IN | 1, EPCON_BUF_DOUBLE);
  UDC_BULK_EP(5, "ep3out", USB_DIR_OUT | 1, EPCON_BUF_DOUBLE);
	UDC_BULK_EP(6, "ep3in", USB_DIR_IN | 1, EPCON_BUF_DOUBLE);
	
  UDC_ISO_EP(7,"ep4in", USB_DIR_IN | 2, EPCON_BUF_TRIPLE);
  UDC_INT_EP(8,"ep5in", USB_DIR_IN | 1, EPCON_BUF_SINGLE);
  
  ep = &udc->ep[8];
  
  ep->ep.maxpacket = 64;
  ep->maxpacket = 64; 

	return 0;
}

static inline void dummy_ep_setup(struct dummy_udc *udc, u8 index, char *name, u8 addr, u8 type, u8 buftype)
{
	struct dummy_ep *ep;
	ep = &udc->ep[index];

	strlcpy(ep->name, name, sizeof ep->name);
	ep->ep.name = ep->name;
	ep->bmAttributes = type;
	ep->bEndpointAddress = addr;
	
	return ;
}


//void udc_ep_packet_config(enum usb_device_speed usb_speed,struct aotg_udc *udc)
//{
//  int i;
//	u16 packsize;
//	struct dummy_ep *ep;
//	
//  for(i = 1 ;i < AOTG_UDC_NUM_ENDPOINTS; i++)
//  {
//		ep = &udc->ep[i];
//		if(ep->bmAttributes == USB_ENDPOINT_XFER_BULK){
//			packsize = (usb_speed == USB_SPEED_FULL) ? BULK_FS_PACKET_SIZE
//				:BULK_HS_PACKET_SIZE;	
//		}else if(ep->bmAttributes == USB_ENDPOINT_XFER_INT){
//			packsize = (usb_speed == USB_SPEED_FULL) ? INT_FS_PACKET_SIZE
//				:INT_HS_PACKET_SIZE;						
//		}else if(ep->bmAttributes == USB_ENDPOINT_XFER_ISOC){
//			packsize = (usb_speed == USB_SPEED_FULL) ? ISO_FS_PACKET_SIZE
//				:ISO_HS_PACKET_SIZE;
//		}else{
//			continue ; 
//		}
//		ep->ep.maxpacket = packsize;
//		ep->maxpacket = packsize; 
//		udc_writel(udc, ep->reg_maxckl, packsize);	
//  } 
//	return ;
//}


static int dummy_udc_probe(struct platform_device *pdev)
{
	struct dummy_udc *udc = &memory;
	int retval = 0;		

	device_initialize(&udc->gadget.dev);
	udc->gadget.dev.parent = &pdev->dev;
	udc->gadget.dev.dma_mask = pdev->dev.dma_mask;
	
	udc->gadget.ep0 = &udc->ep[0].ep;

	the_controller = udc;
	platform_set_drvdata(pdev, udc);
	
	udc_reinit(udc);

	retval = usb_add_gadget_udc(&pdev->dev, &udc->gadget);
	return retval;
}

static int __exit dummy_udc_remove(struct platform_device *_dev)
{	
	struct dummy_udc *udc = platform_get_drvdata(_dev);

	usb_del_gadget_udc(&udc->gadget);
	usb_gadget_unregister_driver(udc->driver);

	platform_set_drvdata(_dev, NULL);
	the_controller = NULL;
	
	return 0;
}

static u64 dummy_dmamask = 0;

static struct platform_device dummy_device = {
	.name = "dummy_udc",
	.id = 0,
	.dev = {
		.dma_mask = &dummy_dmamask,
		.coherent_dma_mask = 0,
		.release = dummy_udc_release,
	},
	.num_resources = 0,
};

static struct platform_driver dummy_driver = {
	.driver = {
		.name = "dummy_udc",
		.owner = THIS_MODULE,
	},
	.probe = dummy_udc_probe,
	.remove = __exit_p(dummy_udc_remove),
};

static int __init dummy_usb_init(void)
{
	int status;
	
	status = platform_device_register(&dummy_device);
	if (status){
		return status;
	}
	platform_driver_register(&dummy_driver);
	
	return 0;
}
module_init(dummy_usb_init);

static void __exit dummy_usb_exit(void)
{	
  platform_device_unregister(&dummy_device);	
	platform_driver_unregister(&dummy_driver);	
}
module_exit(dummy_usb_exit);

MODULE_LICENSE("GPL");

