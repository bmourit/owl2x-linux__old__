#ifndef  __LINUX_USB_AOTG_DEBUG_H__ 
#define  __LINUX_USB_AOTG_DEBUG_H__ 

//#define DEBUG_HCD
//#define DEBUG_HUB
//#define DEBUG_SETUP_DATA
//#define DEBUG_EP_CONFIG
//#define IN_PROCESS_DEBUG
//#define DEBUG_IN_DATA
//#define OUT_PROCESS_DEBUG
//#define DEBUG_OUT_DATA
//#define DEBUG_DMA
#define AOTG_REG_DUMP
//#define AOTG_DEBUG_RECORD_URB
#define AOTG_DEBUG_FILE

#define ACT_HCD_ERR		printk("%s:%d, err!\n", __func__, __LINE__);
#define ACT_HCD_DBG		printk("%s:%d, dbg!\n", __func__, __LINE__);

#ifdef  ERR
#undef  ERR
#endif
#define ERR(fmt, args...) printk(KERN_ERR fmt, ## args)

#ifdef DEBUG_HUB
#define HUB_DEBUG(fmt, args...) printk(KERN_ERR fmt, ## args)
#else
#define HUB_DEBUG(fmt, args...) do {} while (0)
#endif

#ifdef DEBUG_HCD
#define HCD_DEBUG(fmt, args...) printk(KERN_ERR fmt, ## args)
#else
#define HCD_DEBUG(fmt, args...) do {} while (0)
#endif

#ifdef IN_PROCESS_DEBUG  //ep1, ep2
#define IN_DEBUG(fmt, args...) printk(KERN_ERR fmt, ## args)
#else
#define IN_DEBUG(fmt, args...) do {} while (0)
#endif

#ifdef  OUT_PROCESS_DEBUG
#define OUT_DEBUG(fmt, args...) printk(KERN_ERR fmt, ## args)
#else
#define OUT_DEBUG(fmt, args...) do {} while (0)
#endif

#ifdef DEBUG_DMA
#define ACT_DMA_DEBUG(fmt, args...) printk(KERN_ERR fmt, ## args)
#else
#define ACT_DMA_DEBUG(fmt, args...) do {} while (0)
#endif

#define HCD_WARN(fmt, args...) printk(KERN_WARNING fmt, ## args)
#define HCD_WARNING(fmt, args...) printk(KERN_WARNING fmt, ## args)


void aotg_dbg_put_info(char *info0, unsigned int info1, unsigned int info2, unsigned int info3);
void aotg_dbg_output_info(void);

void aotg_dbg_put_q(struct aotg_queue *q, unsigned int num, unsigned int type, unsigned int len);
void aotg_dbg_finish_q(struct aotg_queue *q);

void aotg_dbg_proc_output_ep(void);
void aotg_dbg_proc_output_ep_state(struct aotg_hcd *acthcd);

void create_debug_file(struct aotg_hcd *acthcd);
void remove_debug_file(struct aotg_hcd *acthcd);

void aotg_dbg_regs(struct aotg_hcd *acthcd);

void aotg_hcd_show_ep_info(struct aotg_hcd *acthcd);
void create_acts_hcd_proc(void);
void remove_acts_hcd_proc(void);

#endif /* __LINUX_USB_AOTG_DEBUG_H__ */ 
 
