	
#ifndef  __LINUX_USB_GADGET_DUMMY_H
#define  __LINUX_USB_GADGET_DUMMY_H


#define	EPCON_BUF_TRIPLE	        (0x02)
#define	EPCON_BUF_DOUBLE	        (0x01)
#define	EPCON_BUF_SINGLE	        (0x00)
#define EP_ADDR_MASK              0x0F


struct dummy_udc;

struct dummy_ep {
	struct usb_ep     ep;
	struct dummy_udc   *dev;
	struct list_head    queue;
  char    name[14];
  u8    bEndpointAddress;
	u8    bmAttributes;
	unsigned short      maxpacket;
};


struct dummy_udc {
	struct usb_gadget   gadget;
	struct usb_gadget_driver  *driver;
	struct device   *dev;
  struct dummy_ep    ep[9];
};

struct dummy_request {
	struct usb_request				req;
	struct list_head				queue;
};


#define UDC_BULK_EP(index,name,addr,buftype) \
		dummy_ep_setup(udc, index, name "-bulk", addr, \
				USB_ENDPOINT_XFER_BULK, buftype);
#define UDC_INT_EP(index,name,addr, buftype) \
		dummy_ep_setup(udc, index, name "-int", addr, \
				USB_ENDPOINT_XFER_INT, buftype);
#define UDC_ISO_EP(index, name, addr, buftype) \
		dummy_ep_setup(udc , index, name "-iso", addr, \
				USB_ENDPOINT_XFER_ISOC, buftype);

//#define DEBUG_UDC

#ifdef DEBUG_UDC
#define DUMMY_UDC_PRINTK(fmt, args...)    printk(KERN_INFO fmt, ## args)
#else
#define DUMMY_UDC_PRINTK(fmt, args...)    /*not printk*/
#endif

#ifdef ERR_UDC
#define DUMMY_UDC_ERR(fmt, args...)    printk(KERN_ERR fmt, ## args)
#else
#define DUMMY_UDC_ERR(fmt, args...)    /*not printk*/
#endif

#ifdef ERR_UDC_DMA
#define DUMMY_UDC_DMA_DEBUG(fmt, args...)    printk(KERN_ERR fmt, ## args)
#else
#define DUMMY_UDC_DMA_DEBUG(fmt, args...)    /*not printk*/
#endif

#define UDC_WARNING(fmt, args...) printk(KERN_WARNING fmt, ## args)        

#endif /* __LINUX_USB_GADGET_DUMMY_H */





