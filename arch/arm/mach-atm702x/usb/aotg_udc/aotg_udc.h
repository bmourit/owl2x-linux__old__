	
#ifndef  __LINUX_USB_GADGET_AOTG_H
#define  __LINUX_USB_GADGET_AOTG_H

#include <linux/io.h>

//#define   CONFIG_USB_GADGET_DEBUG_FS

#define   EP0_PACKET_SIZE       64

#define   BULK_HS_PACKET_SIZE		512
#define   BULK_FS_PACKET_SIZE		64

#define   INT_HS_PACKET_SIZE   	1024
#define   INT_FS_PACKET_SIZE     	64

#define   ISO_HS_PACKET_SIZE   	1024
#define   ISO_FS_PACKET_SIZE    1023


#define 	IRQ_AOTG_DMA    			0

#define  	USB_UDC_IN_MASK       0x10
#define   EP_ADDR_MASK          0x0F

#define		EP1_BULK_OUT_STARTADD   		0x00000080
#define 	EP1_BULK_IN_STARTADD  			0x00000480
#define		EP2_BULK_IN_STARTADD   			0x00000880
#define 	EP2_BULK_OUT_STARTADD  			0x00000A80

#define 	EP2_ISO_IN_STARTADD  			0x00000080
#define 	EP1_INT_IN_STARTADD  			0x00000C80

#define AOTG_UDC_NUM_ENDPOINTS      11


#define UDC_BULK_EP(index,name,addr,buftype) \
		aotg_ep_setup(udc, index, name "-bulk", addr, \
				USB_ENDPOINT_XFER_BULK, buftype);
#define UDC_INT_EP(index,name,addr, buftype) \
		aotg_ep_setup(udc, index, name "-int", addr, \
				USB_ENDPOINT_XFER_INT, buftype);
#define UDC_ISO_EP(index, name, addr, buftype) \
		aotg_ep_setup(udc , index, name "-iso", addr, \
				USB_ENDPOINT_XFER_ISOC, buftype);


struct epstats {
		unsigned long		ops;
		unsigned long		bytes;
	};

struct aotg_udc;
struct aotg_ep {
	struct usb_ep       ep;
	struct aotg_udc     *dev;
	const struct usb_endpoint_descriptor  *desc;
	struct list_head    queue;
	char                name[14];
	unsigned long       udc_irqs;
	unsigned long       dma_irqs;
	struct epstats      read;
	struct epstats      write;
	short               dma;	//by hgl
	unsigned short      maxpacket;
	u8            bEndpointAddress;
	u8            bmAttributes;
	unsigned      stopped : 1;
	unsigned      dma_working : 1;
	u8						mask;
	u8						buftype;
	u32						dma_bytes;						 
	unsigned long					reg_udccs;/*read only after init assignment*/
	unsigned long					reg_udccon;/*read only after init assignment*/
	unsigned long					reg_udcbc;/*read only after init assignment*/
	unsigned long					reg_udcfifo;/*read only after init assignment*/
	unsigned long					reg_maxckl;
	unsigned long					reg_fifostaddr;
	/* FIXME ISO support */
	/* struct list_head				iso; */
};

struct aotg_request {
	struct usb_request				req;
	struct list_head				queue;
};


enum ep0_state {
        EP0_WAIT_FOR_SETUP,
        EP0_IN_DATA_PHASE,
        EP0_OUT_DATA_PHASE,
        EP0_END_XFER,
        EP0_STALL,
};
                                                                                                             
enum udc_state {
        UDC_UNKNOWN,
        UDC_IDLE,
        UDC_ACTIVE,
        UDC_SUSPEND,
        UDC_DISABLE,
};
struct udc_stats {
        struct epstats                  		read, write;
        unsigned long                   		irqs;
};


struct aotg_udc {
	void __iomem  *regs;
	int   irq;
  int   dma_chan;
  int   dma_nr0;
	int   dma_nr1;
  int   reset_cnt;
	struct clk    *clk;
  struct mutex  rw_lock;
  spinlock_t    lock;

	struct usb_gadget   gadget;
	struct usb_gadget_driver  *driver;
	struct device     *dev;
	struct usb_phy    *transceiver;

	enum ep0_state    ep0state;
	enum udc_state    state;
	struct udc_stats  stats;

	struct aotg_ep    ep[AOTG_UDC_NUM_ENDPOINTS];


	unsigned    enabled:1;
	unsigned    pullup_on:1;
	unsigned    pullup_resume:1;
	unsigned    softconnect:1;
	unsigned    config:2;
	unsigned    highspeed:1;
	unsigned    suspend:1;
	unsigned    disconnect:1;
	unsigned    req_pending:1;
	unsigned    req_std:1;
	unsigned    req_config:1;
	unsigned    rwk:1;

#ifdef CONFIG_PM
	unsigned    udccsr0;
#endif
#ifdef CONFIG_USB_GADGET_DEBUG_FS
  struct dentry *debugfs_udc;
#endif
};


#define udc_readb(udc, reg)	\
	__raw_readb((udc)->regs + (reg))
#define udc_writeb(udc, reg, value)	\
	__raw_writeb((value), (udc)->regs + (reg))
#define udc_readw(udc, reg)	\
	__raw_readw((udc)->regs + (reg))
#define udc_writew(udc, reg, value)	\
	__raw_writew((value), (udc)->regs + (reg))		
#define udc_readl(udc, reg)	\
	__raw_readl((udc)->regs + (reg))
#define udc_writel(udc, reg, value)	\
	__raw_writel((value), (udc)->regs + (reg))

#define udc_setb_mask(udc, reg, mask)	\
    udc_writeb(udc, reg, udc_readb(udc, reg)|(mask))
#define udc_clearb_mask(udc, reg, mask)	\
	udc_writeb(udc, reg, udc_readb(udc, reg)& ~(mask))	
#define udc_setl_mask(udc, reg, mask)	\
	udc_writel(udc, reg, udc_readl(udc, reg)|(mask))
#define udc_clearl_mask(udc, reg, mask)	\
	udc_writel(udc,reg,udc_readl(udc, reg)& ~(mask))
		
	
#define		dplus_up(udc)  	udc_clearb_mask(udc, USBCS, USBCS_DISCONN)
#define		dplus_down(udc)  	udc_setb_mask(udc, USBCS, USBCS_DISCONN)

#define   extern_irq_enable(udc) udc_setb_mask(udc, USBEIEN, USB_EXTENAL_IRQ_ENABLE)

//#define DEBUG_UDC

#ifdef DEBUG_UDC
#define UDC_PRINTK(fmt, args...)    printk(KERN_INFO fmt, ## args)
#else
#define UDC_PRINTK(fmt, args...)    /*not printk*/
#endif

#ifdef ERR_UDC
#define UDC_ERR(fmt, args...)    printk(KERN_ERR fmt, ## args)
#else
#define UDC_ERR(fmt, args...)    /*not printk*/
#endif

#ifdef ERR_UDC_DMA
#define UDC_DMA_DEBUG(fmt, args...)    printk(KERN_ERR fmt, ## args)
#else
#define UDC_DMA_DEBUG(fmt, args...)    /*not printk*/
#endif

#define UDC_WARNING(fmt, args...) printk(KERN_WARNING fmt, ## args)        

#endif /* __LINUX_USB_GADGET_AOTG_H */





