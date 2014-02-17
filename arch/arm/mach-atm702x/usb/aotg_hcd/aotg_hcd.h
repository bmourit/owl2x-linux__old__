#ifndef  __LINUX_USB_HOST_AOTG_H 
#define  __LINUX_USB_HOST_AOTG_H 

#include <linux/usb/atm702x_reg.h>
#include <mach/hardware.h>
  
#define	PERIODIC_SIZE		64 
#define MAX_PERIODIC_LOAD	500 //50%

#define  AOTG_BULK_FIFO_START_ADDR	0x0080
#define  AOTG_INTERRUPT_FIFO_START_ADDR		0xC00    //max 1k

 
#define  USB_HCD_IN_MASK	0x00 
#define  USB_HCD_OUT_MASK 0x10 
 
#define  AOTG_MAX_FIFO_SIZE    (512*10 + 64*2)	//5k
#define  ALLOC_FIFO_UNIT        64
//#define  AOTG_MIN_DMA_SIZE	  512
//#define  AOTG_MIN_DMA_SIZE	  64
#define  AOTG_MIN_DMA_SIZE	  0
 
#define AOTG_PORT_C_MASK  ((USB_PORT_STAT_C_CONNECTION \
	| USB_PORT_STAT_C_ENABLE \
	| USB_PORT_STAT_C_SUSPEND \
	| USB_PORT_STAT_C_OVERCURRENT \
	| USB_PORT_STAT_C_RESET) << 16) 

#define MAX_EP_NUM		6   //count of each type, 1st is reserved
#define MAX_SG_TABLE	(0x1 << 9)

#define MAX_ERROR_COUNT	10

#define SHORT_PACK_487_490 //for package 487--490 bytes

struct hcd_stats { 
	unsigned long  insrmv; 
	unsigned long  wake; 
	unsigned long sof; 
}; 
 
enum aotg_rh_state { 
	AOTG_RH_POWEROFF, 
	AOTG_RH_POWERED, 
	AOTG_RH_ATTACHED, 
	AOTG_RH_NOATTACHED, 
	AOTG_RH_RESET, 
	AOTG_RH_ENABLE, 
	AOTG_RH_DISABLE, 
	AOTG_RH_SUSPEND, 
	AOTG_RH_ERR 
}; 

enum control_phase {
	PHASE_UNDEFINED,
	PHASE_SETUP,
	PHASE_DATA,
	PHASE_STATUS,
};

struct aotg_sg_table {
	u32 src;
	u32 dst;
	u32 length;
};

struct aotg_queue {
	struct aotg_hcep *ep;
	struct urb *urb;
	int is_start_dma;
	int is_xfer_start;
	int intr_xfer_start;
	int need_zero;
	int data_done;
	struct list_head dma_q_list;
	struct list_head ep_q_list;
	struct list_head enqueue_list;
	struct list_head dequeue_list;
	struct list_head finished_list;
	int status; //for dequeue
	int length;
	unsigned int seq_info;
	
	struct scatterlist *cur_sg;
	int cur_sg_actual;
	
	int inpacket_count;
	int err_count;
	int in_using;
} __attribute__ ((aligned (4)));

struct aotg_hcd { 
	int	id;
	spinlock_t lock;
	spinlock_t tasklet_lock;
	volatile int tasklet_retry;
	void __iomem *base;
	struct device *dev;
	struct aotg_plat_data  *port_specific;
	struct proc_dir_entry *pde; 
	enum   usb_device_speed  speed; 
	int    inserted;	/*imply a USB deivce inserting in MiniA receptacle*/ 
	u32 port;		/*indicate portstatus and portchange*/
	enum aotg_rh_state  rhstate;

	int hcd_exiting;
	
	struct aotg_hcep    *ep0;
	struct aotg_hcep	*inep[MAX_EP_NUM];   // 0 for reserved
	struct aotg_hcep	*outep[MAX_EP_NUM];  // 0 for reserved
	struct aotg_hcep 	*periodic[PERIODIC_SIZE]; 
	unsigned 			periodic_count; 
	u16 				fmindex; 

	//struct aotg_hcep * next_periodic;
	//struct aotg_hcep * sof_periodic_entry;
	u16				frame;

	#define NO_FRAME ((u16)~0)			/* pick new start */
	int				sof_kref;  //for sof enable or disable
	ulong			periodic_reserved;
	
	dma_addr_t sg_dma;
	//struct aotg_sg_table *sg_tbl;

	//int	dma_nr;
	int	dma_nr0;
	int	dma_nr1;
	int 	use_dma;
	//int 	dma_working0;
	//int 	dma_working1;
	unsigned int 	dma_working[2];     /* dma_working[0] for bulkin, dma_working[1] for bulkout.*/
	//struct list_head dma_queue0;
	//struct list_head dma_queue1;
	struct list_head dma_queue[2];

	/* to avoid hardware's bug. if bulkin zero packet occur in a bulkout transfer, 
	 * the next bulk in transfer will be err. 
	 */
	unsigned int last_bulkin_dma_len;

	struct list_head hcd_enqueue_list;
	struct list_head hcd_dequeue_list;
	struct list_head hcd_finished_list;
	struct tasklet_struct urb_tasklet;

	struct timer_list hotplug_timer;
	struct timer_list trans_wait_timer;
	ulong fifo_map[AOTG_MAX_FIFO_SIZE/ALLOC_FIFO_UNIT];

	int put_aout_msg;
	int suspend_request_pend;

	#define AOTG_BULKOUT_NULL		0
	#define AOTG_BULKOUT_DMA_WAIT		1
	#define AOTG_BULKOUT_DMA_TIMEOUT	2
	#define AOTG_BULKOUT_FIFO_BUSY		3
	int timer_bulkout_state;
	struct aotg_hcep *bulkout_wait_ep;
	int bulkout_ep_busy_cnt;

	int setup_processing;
	int uhc_irq;

    struct workqueue_struct *periodic_wq;       // for interrupt tranfer only
	#define AOTG_QUEUE_POOL_CNT	12
	struct aotg_queue *queue_pool[AOTG_QUEUE_POOL_CNT];
}; 
 
struct aotg_hcep { 
	struct usb_host_endpoint *hep; 
	struct usb_device *udev; 
	struct aotg_hcd *dev;
	int   index;
	u32 maxpacket; 
	u8 epnum; 
	u8 nextpid; 
	u16 error_count; 
	u16 length;
	u8 mask; 
	u8 type; 
	u8 buftype; 
	void __iomem *reg_hcepcs; 
	void __iomem *reg_hcepcon; 
   	void __iomem *reg_hcepctrl; 
	void __iomem *reg_hcepbc; 
	void __iomem *reg_hcfifo; 
	void __iomem *reg_hcmaxpck; 
	void __iomem *reg_hcepaddr; 
	void __iomem *reg_hcincount_wt;		/* for write. */
	void __iomem *reg_hcincount_rd;		/* for read. */
	void __iomem *reg_hcerr;
    				
	int is_use_pio;
	unsigned int urb_enque_cnt; 
	unsigned int urb_endque_cnt; 
	unsigned int urb_stop_stran_cnt; 
	unsigned int urb_unlinked_cnt; 

	struct delayed_work dwork;  // periodic works for interrupt transfer
	u32 dma_bytes;
	u16 period; 
	u16 branch; 
	u16 load; 
	struct aotg_hcep *next; 
	u8 fmindex; 

#ifdef SHORT_PACK_487_490	//for 487~490package, avoid the dup 4 bytes for next package
	bool is_shortpack_487_490;
	u32  shortpack_length;
	u32  q_length;
#define DO_PING 0X1 << 6
#endif

	int bulkout_zero_cnt;
	ulong fifo_addr;
	struct list_head q_list;
}; 

#define  get_hcepcon_reg(dir , x , y , z)   ((dir ? x : y) + (z - 1)*8)
#define  get_hcepcs_reg(dir , x , y , z)   ((dir ? x : y) + (z - 1)*8)
#define  get_hcepctrl_reg(dir , x , y , z)   ((dir ? x : y) + (z - 1)*4)
#define  get_hcepbc_reg(dir , x , y , z)   ((dir ? x : y) + (z - 1)*8)
#define  get_hcepmaxpck_reg(dir , x , y , z)   ((dir ? x : y) + (z - 1)*2)
#define  get_hcfifo_reg(x , z)   (x + (z - 1)*4)
#define  get_hcepaddr_reg(dir , x , y , z)  ((dir ? x : y) + (z - 1)*4)
 
static inline struct aotg_hcd *hcd_to_aotg(struct usb_hcd *hcd) 
{ 
	return (struct aotg_hcd *) (hcd->hcd_priv); 
} 
 
static inline struct usb_hcd *aotg_to_hcd(struct aotg_hcd *acthcd) 
{ 
	return container_of((void *)acthcd, struct usb_hcd, hcd_priv); 
}

extern struct aotg_hcd * p_aotg_hcd0;
extern struct aotg_hcd * p_aotg_hcd1;

/* 0 is all enable, 1 -- just usb0 enable, 2 -- usb1 enable, 
 * 3 -- usb0 and usb1 enable,but reversed. 
 */
extern int hcd_ports_en_ctrl;

#endif /* __LINUX_USB_HOST_AOTG_H */ 
 
