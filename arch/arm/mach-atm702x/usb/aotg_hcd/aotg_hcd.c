/*
 * (C) Copyright www.actions-semi.com 2012-2014
 *	   Written by houjingkun. <houjingkun@actions-semi.com>
 * 
 * This	program	is free	software; you can redistribute it and/or modify	it
 * under the terms of the GNU General Public License as	published by the
 * Free	Software Foundation; either	version	2 of the License, or (at your
 * option) any later version.
 *
 * This	program	is distributed in the hope that	it will	be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A	PARTICULAR PURPOSE.	 See the GNU General Public	License
 * for more	details.
 *
 * You should have received	a copy of the GNU General Public License
 * along with this program;	if not,	write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139,	USA.
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/scatterlist.h>
#include <linux/dma-mapping.h>
#include <linux/highmem.h>
#include <linux/usb.h>
#include <linux/usb/ch9.h>
#include <linux/usb/otg.h>
#include <linux/usb/hcd.h>

#include <asm/irq.h>
#include <asm/system.h>

#include <linux/regulator/consumer.h>
//#include <asm/mach-actions/aotg_otg.h>

#include <mach/actions_reg_gl5202.h>
#include <mach/hardware.h>
#include <linux/clk.h>
#include <mach/clock.h>
#include <linux/wakelock.h>
#include <linux/suspend.h>
#include <mach/atc260x/atc260x.h>
#include <mach/debug.h>
#include <asm/prom.h>
#include <mach/gpio.h>

#include "aotg_hcd.h"
#include "aotg_regs.h"
#include "aotg_plat_data.h"
#include "aotg_dma.h"
#include "aotg_debug.h"

#define	DRIVER_VERSION	"FEB-2011"
#define	DRIVER_DESC	"AOTG USB Host Controller Driver"

static int aotg_hcd_start_dma(struct aotg_hcd *acthcd, struct aotg_queue *q, int dmanr);
static int handle_bulkout_packet(struct	aotg_hcd *acthcd, struct aotg_queue	*q);
static int handle_bulkin_packet(struct aotg_hcd	*acthcd, struct	aotg_queue *q);
static int handle_intr_packet(struct aotg_hcd *acthcd, struct aotg_queue *q);
static int handle_setup_packet(struct aotg_hcd *acthcd,	struct aotg_queue *q);
static void	handle_hcep0_in(struct aotg_hcd	*acthcd);
static void	handle_hcep0_out(struct	aotg_hcd *acthcd);

#define	MAX_PACKET(x)	((x)&0x7FF)

struct aotg_hcd	* p_aotg_hcd0 =	NULL;
struct aotg_hcd	* p_aotg_hcd1 =	NULL;
static void	__iomem	*aotg_base = NULL;
static void	__iomem	*aotg0_base	= NULL;
static void	__iomem	*aotg1_base	= NULL;

/* because usb0	and	usb1's pll is all controlled together, 
 * we couldn't enable aotg0	and	aotg1 seperately. 
 */
static int hcd_2clk_bits_en	= 0;
/* 0 is	all	enable,	1 -- just usb0 enable, 2 --	usb1 enable, 
 * 3 --	usb0 and usb1 enable,but reversed. 
 */
int	hcd_ports_en_ctrl =	0;

/* forbid to enter suspend when	driver is installed. */
struct wake_lock acts_hcd_wakelock;

/* for debug */
//#define	DEBUG_INFO
#define	TIMEOUT_VALUE 40/* 2s */ 
#define MULTI_OUT_FIFO


static ulong get_fifo_addr(struct aotg_hcd *acthcd,	int	size)
{
	int	i, j;
	ulong addr = 0;
	int	mul	= size / ALLOC_FIFO_UNIT;
	int	max_unit = AOTG_MAX_FIFO_SIZE/ALLOC_FIFO_UNIT;
	int	find_next =	0;

	if (mul	== 0)
		mul	= 1;

	for	(i = 2;	i <	max_unit;) {
		if (acthcd->fifo_map[i]	!= 0) {
			i++;
			continue;	 //find	first unused addr
		}

		for	(j = i;	j <	max_unit; j++) {
			if ((j - i + 1)	== mul)
				break;

			if (acthcd->fifo_map[j]) {
				i =	j;
				find_next =	1;
				break;
			}
		}

		if (j == 64) {
			break;
		} else if (find_next) {
			find_next =	0;
			continue;
		} else {
			int	k;
			for	(k = i;	k <= j;	k++) {
				acthcd->fifo_map[k]	= (1 <<	31)	| (i * 64);
			}
			addr = i * ALLOC_FIFO_UNIT;
			break;
		}
	}

	return addr;
}

static void	release_fifo_addr(struct aotg_hcd *acthcd, ulong addr)
{
	int	i;

	for	(i = addr/ALLOC_FIFO_UNIT; i < AOTG_MAX_FIFO_SIZE/ALLOC_FIFO_UNIT ;	i++) {
		if ((acthcd->fifo_map[i] & 0x7FFFFFFF) == addr)
			acthcd->fifo_map[i]	= 0;
		else
			break;
	}
	return;
}

static struct aotg_queue * aotg_hcd_get_queue(struct aotg_hcd *acthcd, unsigned	mem_flags)
{
	int	i;
	int	empty_idx =	-1;
	struct aotg_queue *q = NULL;

	for	(i = 0;	i <	AOTG_QUEUE_POOL_CNT; i++) {
		if (acthcd->queue_pool[i] != NULL) {
			if (acthcd->queue_pool[i]->in_using	== 0) {
				q =	acthcd->queue_pool[i];
				break;
			}
		} else {
			if (empty_idx <	0) {
				empty_idx =	i;
			}
		}
	}
	if (i == AOTG_QUEUE_POOL_CNT) {
		q =	kzalloc(sizeof(*q),	GFP_ATOMIC);
		if (unlikely(!q)) {
			dev_err(acthcd->dev, "aotg_hcd_get_queue failed\n");
			return NULL;
		}
		if ((empty_idx >= 0) &&	(empty_idx < AOTG_QUEUE_POOL_CNT)) {
			acthcd->queue_pool[empty_idx] =	q;
		}
	}

	memset(q, 0, sizeof(*q));
	q->length =	-1;
	INIT_LIST_HEAD(&q->dma_q_list);
	INIT_LIST_HEAD(&q->ep_q_list);
	INIT_LIST_HEAD(&q->enqueue_list);
	INIT_LIST_HEAD(&q->dequeue_list);
	INIT_LIST_HEAD(&q->finished_list);
	q->in_using	= 1;

	return q;
}

static void	aotg_hcd_release_queue(struct aotg_hcd *acthcd,	struct aotg_queue *q)
{
	int	i;

	/* release all */
	if (q == NULL) {
		for	(i = 0;	i <	AOTG_QUEUE_POOL_CNT; i++) {
			if (acthcd->queue_pool[i] != NULL) {
				kfree(acthcd->queue_pool[i]);
				acthcd->queue_pool[i] =	NULL;
			}
		}
		return;
	}

	for	(i = 0;	i <	AOTG_QUEUE_POOL_CNT; i++) {
		if (acthcd->queue_pool[i] == q)	{
			acthcd->queue_pool[i]->in_using	= 0;
			return;
		}
	}
	kfree(q);
	return;
}

static __inline__ void ep_setup(struct aotg_hcep *ep, u8 type, u8 buftype)
{
	ep->buftype	= buftype;

	usb_writeb(type	| buftype, ep->reg_hcepcon);
}

static __inline__ void aotg_hcep_reset(struct aotg_hcd *acthcd,	u8 ep_mask,	u8 type_mask)
{
	u8 val;

	usb_writeb(ep_mask,	acthcd->base + ENDPRST);	/*select ep	*/
	val	= ep_mask |	type_mask;
	usb_writeb(val,	acthcd->base + ENDPRST);	/*reset	ep */
}

static __inline__ void pio_irq_disable(struct aotg_hcd *acthcd,	u8 mask)
{
	u8 is_out =	mask & USB_HCD_OUT_MASK;
	u8 ep_num =	mask & 0x0f;

	if (is_out)	{
		usb_clearbitsb(1 <<	ep_num,	acthcd->base + HCOUT07IEN);
	} else {
		usb_clearbitsb(1 <<	ep_num,	acthcd->base + HCIN07IEN);
	}
}

static __inline__ void pio_irq_enable(struct aotg_hcd *acthcd, u8 mask)
{
	u8 is_out =	mask & USB_HCD_OUT_MASK;
	u8 ep_num =	mask & 0x0f;

	if (is_out)	{
		usb_setbitsb(1 << ep_num, acthcd->base + HCOUT07IEN);
	} else {
		usb_setbitsb(1 << ep_num, acthcd->base + HCIN07IEN);
	}
}

static __inline__ void pio_irq_clear(struct	aotg_hcd *acthcd, u8 mask)
{
	u8 is_out =	mask & USB_HCD_OUT_MASK;
	u8 ep_num =	mask & 0x0f;

	if (is_out)	{
		usb_writeb(1 <<	ep_num,	acthcd->base + HCOUT07IRQ);
	}
	else {
		usb_writeb(1 <<	ep_num,	acthcd->base + HCIN07IRQ);
	}
}

static __inline__ void ep_enable(struct	aotg_hcep *ep)
{
	usb_setbitsb(0x80, ep->reg_hcepcon);
}

static __inline__ void ep_disable(struct aotg_hcep *ep)
{
	usb_clearbitsb(0x80, ep->reg_hcepcon);
}

static __inline__ void __hc_in_stop(struct aotg_hcd	*acthcd, struct	aotg_hcep *ep)
{
	u8 set = 0x1 <<	ep->index;

	if (usb_readb(acthcd->base + OUTXSHORTPCKIRQ) &	set) {
		usb_writeb(set,	acthcd->base + OUTXSHORTPCKIRQ);
	}
	usb_clearbitsb(set,	acthcd->base + HCINXSTART);
	usb_writew(0, ep->reg_hcincount_wt);
	//usb_clearbitsb(set, acthcd->base + HCINCTRL);
}

static __inline__ void __hc_in_start(struct	aotg_hcd *acthcd, struct aotg_hcep *ep,	const u16 w_packets)
{
	u8 set = 0x1 <<	ep->index;

	usb_writew(w_packets, ep->reg_hcincount_wt);
	usb_setbitsb(set, acthcd->base + OUTXNAKCTRL);
	usb_setbitsb(set, acthcd->base + HCINXSTART);
}

static __inline__ void __hc_in_pause(struct	aotg_hcd *acthcd, struct aotg_hcep *ep)
{
	u8 set = 0x1 <<	ep->index;

	if (usb_readb(acthcd->base + OUTXSHORTPCKIRQ) &	set) {
		usb_writeb(set,	acthcd->base + OUTXSHORTPCKIRQ);
	}
	usb_clearbitsb(set,	acthcd->base + HCINXSTART);
}

static __inline__ void __hc_in_resume(struct aotg_hcd *acthcd, struct aotg_hcep	*ep)
{
	u8 set = 0x1 <<	ep->index;

	usb_setbitsb(set, acthcd->base + OUTXNAKCTRL);
	usb_setbitsb(set, acthcd->base + HCINXSTART);
}

static __inline__ void aotg_sofirq_on(struct aotg_hcd *acthcd)
{
	usb_setbitsb((1	<< 1), acthcd->base	+ USBIEN);
}

static __inline__ void aotg_sofirq_off(struct aotg_hcd *acthcd)
{
	usb_clearbitsb(1 <<	1, acthcd->base	+ USBIEN);
}

static __inline__ int get_subbuffer_count(u8 buftype)
{
	int	count =	0;

	switch (buftype) {
	case EPCON_BUF_SINGLE:
		count =	1;
		break;
	case EPCON_BUF_DOUBLE:
		count =	2;
		break;
	case EPCON_BUF_TRIPLE:
		count =	3;
		break;
	case EPCON_BUF_QUAD:
		count =	4;
		break;
	}

	return count;
}

static __inline__ void aotg_enable_irq(struct aotg_hcd *acthcd)
{
	//usb_setbitsb(USBEIRQ_USBIEN |	USBEIRQ_CON_DISCONIEN, acthcd->base	+ USBEIRQ);
	usb_writeb(USBEIRQ_USBIEN, acthcd->base	+ USBEIRQ);
	usb_setbitsb(USBEIRQ_USBIEN, acthcd->base +	USBEIEN);
	usb_setbitsb(0x1<<2, acthcd->base +	OTGIEN);
	//printk("USBEIRQ(0x%p): 0x%02X\n",	acthcd->base + USBEIRQ,	usb_readb(acthcd->base + USBEIRQ));
	usb_setbitsb(OTGCTRL_BUSREQ, acthcd->base +	OTGCTRL);	
}

static __inline__ void aotg_disable_irq(struct aotg_hcd	*acthcd)
{
	//usb_setbitsb(USBEIRQ_USBIEN |	USBEIRQ_CON_DISCONIEN, acthcd->base	+ USBEIRQ);
	usb_writeb(USBEIRQ_USBIEN, acthcd->base	+ USBEIRQ);
	usb_clearbitsb(USBEIRQ_USBIEN, acthcd->base	+ USBEIEN);
	usb_clearbitsb(0x1<<2, acthcd->base	+ OTGIEN);
	usb_clearbitsb(OTGCTRL_BUSREQ, acthcd->base	+ OTGCTRL);	
}

static __inline__ void __clear_dma(struct aotg_hcd *acthcd,	struct aotg_queue *q, int dma_no)
{
	struct aotg_hcep *ep = q->ep;

	aotg_dma_enable_irq(dma_no,	0);

	//printk("clear	dma: %d, inpacket_count: %d	\n", dma_no, q->inpacket_count);
	//printk("DMASRC: 0x%x ", aotg_dma_get_memaddr(dma_no));
	//printk("cnt: 0x%x	", aotg_dma_get_cnt(dma_no));
	//printk("remain: 0x%x \n",	aotg_dma_get_remain(dma_no));
	//WARN_ON(q);

	/* be sure bulk	out	dma	is working.	*/
	if ((dma_no	& 0x1) && (acthcd->dma_working[1] != 0)) {
		printk("%s,%d,dma_no: %d, timer_bulkout_state: %d\n", __FUNCTION__,__LINE__,dma_no,	acthcd->timer_bulkout_state);
		acthcd->timer_bulkout_state	= AOTG_BULKOUT_NULL;
	}

	list_del_init(&q->dma_q_list);
	aotg_dma_stop(dma_no);
	aotg_dma_clear_pend(dma_no);
	q->is_start_dma	= 0;

	acthcd->dma_working[dma_no&0x1]	= 0;

	aotg_hcep_reset(acthcd,	ep->mask, ENDPRST_FIFORST |	ENDPRST_TOGRST);

	if ((acthcd->dma_working[0]	== 0) && (acthcd->dma_working[1] ==	0))	{
		acthcd->last_bulkin_dma_len	= 1;
		aotg_dma_reset_2(dma_no);
		//printk("dma reset! \n");
//		printk("%s,%d,dma reset!!! \n",__FUNCTION__,__LINE__);
	}
	aotg_dma_enable_irq(dma_no,	1);
	return;
}

//support 3	bulk, 1	interrupt
static int aotg_hcep_config(struct aotg_hcd	*acthcd,
				struct aotg_hcep *ep,
				u8 type, u8	buftype, int is_out)
{
	int	index =	0;
	ulong addr = 0;
	int	get_ep = 0;
	int	subbuffer_count;
	u8 fifo_ctrl;

	if (0 == (subbuffer_count =	get_subbuffer_count(buftype))) {
		dev_err(acthcd->dev, "error	buftype: %02X, %s, %d\n", buftype, __func__, __LINE__);
		return -EPIPE;
	}

	if (is_out)	{
		for	(index = 1;	index <	MAX_EP_NUM;	index++) {
			if (acthcd->outep[index] ==	NULL) {
				ep->index =	index;
				ep->mask = (u8)	(USB_HCD_OUT_MASK |	index);
				acthcd->outep[index] = ep;
				get_ep = 1;
#ifdef SHORT_PACK_487_490
				ep->shortpack_length = 0;
				ep->is_shortpack_487_490 = false;
#endif
				break;
			}
		}
	}
	else {
		for	(index = 1;	index <	MAX_EP_NUM;	index++) {
			if (acthcd->inep[index]	== NULL) {
				ep->index =	index;
				ep->mask = (u8)	index;
				acthcd->inep[index]	= ep;
				get_ep = 1;
				break;
			}
		}
	}

	if (!get_ep) {
		dev_err(acthcd->dev, "%s: no more available	space for ep\n", __func__);
		return -ENOSPC;
	}

	addr = get_fifo_addr(acthcd, subbuffer_count * MAX_PACKET(ep->maxpacket));
	if (addr ==	0) {
		dev_err(acthcd->dev, "buffer configuration overload!! addr:	%08X, subbuffer_count: %d, ep->maxpacket: %u\n",
				(u32)addr, subbuffer_count,	MAX_PACKET(ep->maxpacket));
		if (is_out)	{
			acthcd->outep[ep->index] = NULL;
		}
		else {
			acthcd->inep[ep->index]	= NULL;
		}
		return -ENOSPC;
	}
	else {
		ep->fifo_addr =	addr;
	}

	ep->reg_hcepcon	= get_hcepcon_reg(is_out, 
								acthcd->base + HCOUT1CON, 
								acthcd->base + HCIN1CON, 
								ep->index);
	ep->reg_hcepcs = get_hcepcs_reg(is_out,	
								acthcd->base + HCOUT1CS, 
								acthcd->base + HCIN1CS,	
								ep->index);
	ep->reg_hcepbc = get_hcepbc_reg(is_out,	
								acthcd->base + HCOUT1BCL, 
								acthcd->base + HCIN1BCL, 
								ep->index);
	ep->reg_hcepctrl = get_hcepctrl_reg(is_out,	
								acthcd->base + HCOUT1CTRL, 
								acthcd->base + HCIN1CTRL, 
								ep->index);
	ep->reg_hcmaxpck = get_hcepmaxpck_reg(is_out, 
								acthcd->base + HCOUT1MAXPCKL, 
								acthcd->base + HCIN1MAXPCKL, 
								ep->index);
	ep->reg_hcepaddr = get_hcepaddr_reg(is_out,	
								acthcd->base + IN1STARTADDRESS,	
								acthcd->base + OUT1STARTADDRESS, 
								ep->index);
	ep->reg_hcfifo = get_hcfifo_reg(acthcd->base + FIFO1DATA, ep->index);
	if (!is_out) {
		/* 5202	is just	for	write, read's HCINXCOUNT address is	not	the	same with write	address. */
		ep->reg_hcincount_wt = acthcd->base	+ HCIN1_COUNTL + (ep->index	- 1) * 4;
		ep->reg_hcincount_rd = acthcd->base	+ HCIN1_COUNTL + (ep->index	- 1) * 2;
		ep->reg_hcerr =	acthcd->base + HCIN0ERR	+ ep->index	* 0x4;
	}
	else {
		ep->reg_hcerr =	acthcd->base + HCOUT0ERR + ep->index * 0x4;
	}

#ifdef DEBUG_EP_CONFIG
	dev_info(acthcd->dev, "== ep->index: %d, is_out: %d, fifo addr:	%08X\n", ep->index,	is_out,	(u32)addr);
	dev_info(acthcd->dev, "== reg_hcepcon: %08lX, reg_hcepcs: %08lX, reg_hcepbc: %08lX,	reg_hcepctrl: %08lX, reg_hcmaxpck: %08lX, ep->reg_hcepaddr:	%08lX,reg_hcfifo: %08lX\n",
			ep->reg_hcepcon,
			ep->reg_hcepcs,
			ep->reg_hcepbc,
			ep->reg_hcepctrl,
			ep->reg_hcmaxpck,
			ep->reg_hcepaddr,
			ep->reg_hcfifo);
#endif
	
	pio_irq_disable(acthcd,	ep->mask);
	pio_irq_clear(acthcd, ep->mask);
	
	if (!is_out) {
		ep_disable(ep);
		__hc_in_stop(acthcd, ep);
	}

	/*allocate buffer address of ep	fifo */
	usb_writel(addr, ep->reg_hcepaddr);
	usb_writew(ep->maxpacket, ep->reg_hcmaxpck);
	ep_setup(ep, type, buftype);	/*ep setup */
	
	/*reset	this ep	*/
	usb_settoggle(ep->udev,	ep->epnum, is_out, 0);
	aotg_hcep_reset(acthcd,	ep->mask, ENDPRST_FIFORST |	ENDPRST_TOGRST);
	usb_writeb(ep->epnum, ep->reg_hcepctrl);
	fifo_ctrl =	(1<<5) | ((!!is_out) <<	4) | ep->index;	//set auto fifo
	usb_writeb(fifo_ctrl, acthcd->base + FIFOCTRL);

	//pio_irq_enable(acthcd, ep->mask);
	return 0;
}

static void	finish_request(struct aotg_hcd *acthcd,
			   struct aotg_queue *q,
			   int status)
{
	struct urb *urb	= q->urb;

	if (unlikely((acthcd ==	NULL) || (q	== NULL) ||	(urb ==	NULL)))	{
		WARN_ON(1);
		return;
	}

	q->status =	status;
	list_add_tail(&q->finished_list, &acthcd->hcd_finished_list);

	tasklet_hi_schedule(&acthcd->urb_tasklet);
	return;
}


static void	tasklet_finish_request(struct aotg_hcd *acthcd,
			   struct aotg_queue *q,
			   int status)
{
	struct usb_hcd *hcd	= aotg_to_hcd(acthcd);
	struct urb *urb	= q->urb;
	struct aotg_hcep *ep = q->ep;
	int	rc;

	if (unlikely((acthcd ==	NULL) || (q	== NULL) ||	(urb ==	NULL)))	{
		WARN_ON(1);
		return;
	}
	/* maybe dequeued in urb_dequeue. */
	if (q->ep_q_list.next == LIST_POISON1) {
		ACT_HCD_DBG
		q->urb = NULL;
		q->ep =	NULL;
		return;
	}

	if (usb_pipeint(urb->pipe))	{
		acthcd->sof_kref--;
		hcd->self.bandwidth_int_reqs--;
		if (acthcd->sof_kref <=	0) {
			aotg_sofirq_off(acthcd);
		}
	}

	aotg_dbg_finish_q(q);
	list_del(&q->ep_q_list);
	aotg_hcd_release_queue(acthcd, q);

	//usb_hcd_unlink_urb_from_ep(hcd, urb);
	//usb_hcd_giveback_urb(hcd,	urb, status);

	ep->urb_endque_cnt++;

	if (usb_pipetype(urb->pipe)	== PIPE_CONTROL) {
		acthcd->setup_processing = 0;
	} else if ((usb_pipetype(urb->pipe)	== PIPE_BULK) && (usb_pipeout(urb->pipe))) {
		if ((acthcd->timer_bulkout_state ==	AOTG_BULKOUT_DMA_TIMEOUT) && 
			(acthcd->bulkout_ep_busy_cnt >=	TIMEOUT_VALUE))	{
			printk("timeout	finish:%x, cnt:%d,[ep->index:%d,%s,]\n", (unsigned int)jiffies,acthcd->bulkout_ep_busy_cnt,	\
			ep->index,(usb_pipeout(urb->pipe)) ? "out":"in");
#ifdef DEBUG_INFO
				aotg_hcd_show_ep_info(acthcd);
				aotg_dbg_regs(acthcd);
#endif
		}
		acthcd->timer_bulkout_state	= AOTG_BULKOUT_NULL;
		acthcd->bulkout_ep_busy_cnt	= 0;
		mod_timer(&acthcd->trans_wait_timer, jiffies + msecs_to_jiffies(0x3fffffff));
	} else {
		//tmp_seq_bulkout_cnt =	0;
	}

	if (list_empty(&ep->q_list)) {
		return;
	}

	ACT_HCD_ERR

	q =	list_entry(ep->q_list.next,	struct aotg_queue, ep_q_list);
	urb	= q->urb;
	ep = q->ep;
	switch (usb_pipetype(urb->pipe)) {
	case PIPE_CONTROL:
		rc = handle_setup_packet(acthcd, q);
		break;
	
	case PIPE_BULK:
		if (usb_pipeout(urb->pipe))	{
			rc = handle_bulkout_packet(acthcd, q);
		}
		else {
			rc = handle_bulkin_packet(acthcd, q);
		}
		break;

	case PIPE_INTERRUPT:
		rc = handle_intr_packet(acthcd,	q);
		break;

	case PIPE_ISOCHRONOUS:
	default:
		rc = -ENODEV;
		break;
	}

	if (unlikely(rc	< 0)) {
		ACT_HCD_ERR
		finish_request(acthcd, q, rc);
	}
}

static __inline__ void handle_status(struct	aotg_hcd *acthcd, struct aotg_hcep *ep,	int	is_out)
{
	/*status always	DATA1,set 1	to ep0 toggle */
	usb_writeb(EP0CS_HCSETTOOGLE, acthcd->base + HCEP0CS);

	if (is_out)	{
		usb_writeb(0, acthcd->base + HCIN0BC); //recv 0	packet
	}
	else {
		usb_writeb(0, acthcd->base + HCOUT0BC);	//send 0 packet
	}
}

static void	write_hcep0_fifo(struct	aotg_hcd *acthcd, struct aotg_hcep *ep,	struct urb *urb)
{
	u32	*buf;
	int	length,	count;
	void __iomem *addr = acthcd->base +	HCEP0OUTDAT;

	if (!(usb_readb(acthcd->base + HCEP0CS)	& EP0CS_HCOUTBSY)) {
		buf	= (u32 *) (urb->transfer_buffer	+ urb->actual_length);
		prefetch(buf);

		/* how big will	this packet	be?	*/
		length = min((int)ep->maxpacket, (int)urb->transfer_buffer_length -	(int)urb->actual_length);

		count =	length >> 2;	/*wirte	in DWORD */
		if (length & 0x3) count++;

		while (likely(count--))	{
			usb_writel(*buf, addr);
			buf++;
			addr +=	4;
		}

		ep->length = length;
		usb_writeb(length, acthcd->base	+ HCOUT0BC);
		usb_dotoggle(urb->dev, usb_pipeendpoint(urb->pipe),	1);
	}
	else {
		dev_err(acthcd->dev, "<CTRL>OUT	data is	not	ready\n");
	}
}

static void	read_hcep0_fifo(struct aotg_hcd	*acthcd, struct	aotg_hcep *ep, struct urb *urb)
{
	u8 *buf;
	unsigned overflag, is_short, shorterr, is_last;
	unsigned length, count;
	struct usb_device *udev;
	void __iomem *addr = acthcd->base +	EP0OUTDATA_W0;	//HCEP0INDAT0;
	unsigned bufferspace;

	overflag = 0;
	is_short = 0;
	shorterr = 0;
	is_last	= 0;
	udev = ep->udev;

	if (usb_readb(acthcd->base + HCEP0CS) &	EP0CS_HCINBSY) {
		dev_err(acthcd->dev, "<CTRL>IN data	is not ready\n");
		return;
	} else {
		usb_dotoggle(udev, ep->epnum, 0);
		buf	= urb->transfer_buffer + urb->actual_length;
		bufferspace	= urb->transfer_buffer_length -	urb->actual_length;
		//prefetch(buf);

		length = count = usb_readb(acthcd->base	+ HCIN0BC);
		if (length > bufferspace) {
			count =	bufferspace;
			urb->status	= -EOVERFLOW;
			overflag = 1;
		}

		urb->actual_length += count;
		while (count--)	{
			*buf++ = usb_readb(addr);
#if	0
			buf--;
			printk("ep0in:%x, cnt:%d\n", (unsigned int)*buf, count);
			buf++;
#endif
			addr++;
		}

		if (urb->actual_length >= urb->transfer_buffer_length) {
			ep->nextpid	= USB_PID_ACK;
			is_last	= 1;
			handle_status(acthcd, ep, 0);
		} else if (length <	ep->maxpacket) {
			is_short = 1;
			is_last	= 1;
			if (urb->transfer_flags	& URB_SHORT_NOT_OK)	{
				urb->status	= -EREMOTEIO;
				shorterr = 1;
			}
			ep->nextpid	= USB_PID_ACK;
			handle_status(acthcd, ep, 0);
		}
		else {
			usb_writeb(0, acthcd->base + HCIN0BC);
		}
	}
}

static int handle_setup_packet(struct aotg_hcd *acthcd,
		struct aotg_queue *q)
{
	struct urb *urb	= q->urb;
	struct aotg_hcep *ep = q->ep;
	int	i =	0;
	
	u32	*buf;
	void __iomem *addr = acthcd->base +	HCEP0OUTDAT;

#ifdef DEBUG_SETUP_DATA
	u16	w_value, w_index, w_length;
	struct usb_ctrlrequest *ctrlreq;

	ctrlreq	= (struct usb_ctrlrequest *)urb->setup_packet;
	w_value	= le16_to_cpu(ctrlreq->wValue);
	w_index	= le16_to_cpu(ctrlreq->wIndex);
	w_length = le16_to_cpu(ctrlreq->wLength);
	dev_info(acthcd->dev, "<CTRL>SETUP stage  %02x.%02x	V%04x I%04x	L%04x\n	",
		  ctrlreq->bRequestType, ctrlreq->bRequest,	w_value, w_index,
		  w_length);
#endif

	if (q->is_xfer_start ||	ep->q_list.next	!= &q->ep_q_list) {
		ACT_HCD_DBG
		return 0;
	}

	if (unlikely(!HC_IS_RUNNING(aotg_to_hcd(acthcd)->state))) {
		ACT_HCD_DBG
		return -ESHUTDOWN;
	}

	acthcd->setup_processing = 1;
	q->is_xfer_start = 1;
	usb_settoggle(urb->dev,	usb_pipeendpoint(urb->pipe), 1,	1);
	ep->nextpid	= USB_PID_SETUP;
	buf	= (u32 *) urb->setup_packet;

	/*initialize the setup stage */
	usb_writeb(EP0CS_HCSET,	acthcd->base + EP0CS);
	while (usb_readb(acthcd->base +	HCEP0CS) & EP0CS_HCOUTBSY) {
		usb_writeb(EP0CS_HCSET,	acthcd->base + EP0CS);
		i++;
		if (i >	2000000) {
			printk("handle_setup timeout!\n");
			break;
		}
	}
	
	if (!(usb_readb(acthcd->base + HCEP0CS)	& EP0CS_HCOUTBSY)) {
		/*fill the setup data in fifo */
		usb_writel(*buf, addr);
		addr +=	4;
		buf++;
		usb_writel(*buf, addr);
		usb_writeb(8, acthcd->base + HCOUT0BC);
	}
	else {
		dev_warn(acthcd->dev, "setup ep	busy!!!!!!!\n");
	}

	return 0;
}

static void	handle_hcep0_out(struct	aotg_hcd *acthcd)
{
	struct aotg_hcep *ep;
	struct urb *urb;
	struct usb_device *udev;
	struct aotg_queue *q;
	
	ep = acthcd->ep0;
	
	if (unlikely(!ep ||	list_empty(&ep->q_list))) {
		return;
	}
	
	q =	list_entry(ep->q_list.next,	struct aotg_queue, ep_q_list);
	urb	= q->urb;
	udev = ep->udev;

	switch (ep->nextpid) {
	case USB_PID_SETUP:
		if (urb->transfer_buffer_length	== urb->actual_length) {
			ep->nextpid	= USB_PID_ACK;
			handle_status(acthcd, ep, 1);	/*no-data transfer */
		} else if (usb_pipeout(urb->pipe)) {
			usb_settoggle(udev,	0, 1, 1);
			ep->nextpid	= USB_PID_OUT;
			write_hcep0_fifo(acthcd, ep, urb);
		} else {
			usb_settoggle(udev,	0, 0, 1);
			ep->nextpid	= USB_PID_IN;
			usb_writeb(0, acthcd->base + HCIN0BC);
		}
		break;
	case USB_PID_OUT:
		urb->actual_length += ep->length;
		usb_dotoggle(udev, ep->epnum, 1);
		if (urb->actual_length >= urb->transfer_buffer_length) {
			ep->nextpid	= USB_PID_ACK;
			handle_status(acthcd, ep, 1);	/*control write	transfer */
		} else {
			ep->nextpid	= USB_PID_OUT;
			write_hcep0_fifo(acthcd, ep, urb);
		}
		break;
	case USB_PID_ACK:
		finish_request(acthcd, q, 0);
		break;
	default:
		dev_err(acthcd->dev, "<CTRL>ep0	out	,odd pid %d, %s, %d\n",	
				ep->nextpid, __func__, __LINE__);
	}
}

static void	handle_hcep0_in(struct aotg_hcd	*acthcd)
{
	struct aotg_hcep *ep;
	struct urb *urb;
	struct usb_device *udev;
	struct aotg_queue *q;
	
	ep = acthcd->ep0;
	if (unlikely(!ep ||	list_empty(&ep->q_list))) {
		return;
	}

	q =	list_entry(ep->q_list.next,	struct aotg_queue, ep_q_list);
	urb	= q->urb;
	udev = ep->udev;

	switch (ep->nextpid) {
	case USB_PID_IN:
		read_hcep0_fifo(acthcd,	ep,	urb);
		break;
	case USB_PID_ACK:
		finish_request(acthcd, q, 0);
		break;
	default:
		dev_err(acthcd->dev, "<CTRL>ep0	out	,odd pid %d\n",	ep->nextpid);
	}
}

static void	handle_hcep_in(struct aotg_hcd *acthcd,	int	index)
{
	struct aotg_hcep *ep;
	struct urb *urb;
	struct aotg_queue *q;
	void *buf;
	int	bufferspace;
	int	count, length, fifo_cnt;
	int	remain;
	u32	word;
	void * __iomem fifo;
	u8 *byte_buf;
	u16	*short_buf;
	u32	*word_buf;
	int	is_short = 0;
	void *kmap_addr	= NULL;
	int	highmem_kmaped = 0;

	//ACT_HCD_DBG
	ep = acthcd->inep[index];
	if (!ep	|| list_empty(&ep->q_list))	{
		return;
	}

	q =	list_entry(ep->q_list.next,	struct aotg_queue, ep_q_list);
	urb	= q->urb;
	ep = q->ep;
	
	if (unlikely(ep->is_use_pio	== 0))
		return;

	while ((!(usb_readb(ep->reg_hcepcs)	& EPCS_BUSY) &&	(urb->transfer_buffer_length > urb->actual_length))
		|| (usb_readb(acthcd->base + OUTXSHORTPCKIRQ) &	(0x1 <<	index))) {
		highmem_kmaped = 0;
		if (q->cur_sg) {

			if (!PageHighMem(sg_page(q->cur_sg))) {
				kmap_addr =	sg_virt(q->cur_sg);	
				buf	= kmap_addr+q->cur_sg_actual;
			} else {
				ACT_HCD_ERR
				highmem_kmaped = 1;
				return;
			}
			bufferspace	= q->cur_sg->length	- q->cur_sg_actual;
		} else {
			if (urb->sg	!= NULL) {
				if (PageHighMem(sg_page(urb->sg))) {
					ACT_HCD_ERR
					buf	= kmap_addr	+ urb->actual_length % PAGE_SIZE;
					highmem_kmaped = 1;
				} else {
					kmap_addr =	sg_virt(urb->sg); 
					buf	= kmap_addr	+ urb->actual_length;
				}
				bufferspace	= urb->sg->length -	urb->actual_length;
			} else if (urb->transfer_buffer	!= NULL) {
				buf	= urb->transfer_buffer + urb->actual_length;
				bufferspace	= urb->transfer_buffer_length -	urb->actual_length;
			} else {
				WARN_ON(1);
				return;
			}
		}
		
		fifo_cnt = (int)(usb_readw(ep->reg_hcepbc) & 0xFFF);

		if ((usb_readb(acthcd->base	+ OUTXSHORTPCKIRQ) & (0x1 << index)) &&	
			(fifo_cnt == ep->maxpacket)) {
			fifo_cnt = 0;
		}
		length = min(fifo_cnt, bufferspace);
		is_short = (fifo_cnt < (int)ep->maxpacket) ? 1 : 0;
		remain = length	& 0x3;
		count =	length >> 2;

		if ((u32)buf & 0x1)	{
			byte_buf = (u8 *)buf;
			buf	+= count * 4;
			while (likely(count--))	{
				word = usb_readl(ep->reg_hcfifo);
				*byte_buf++	= (u8)word;
				*byte_buf++	= (u8)(word	>> 8);
				*byte_buf++	= (u8)(word	>> 16);
				*byte_buf++	= (u8)(word	>> 24);
				
			}
		} else if ((u32)buf	& 0x2) {
			short_buf =	(u16 *)buf;
			buf	+= count * 4;
			while (likely(count--))	{
				word = usb_readl(ep->reg_hcfifo);
				*short_buf++ = (u16)word;
				*short_buf++ = (u16)(word >> 16);
			}
		} else {
			word_buf = (u32	*)buf;
			buf	+= count * 4;
			while (likely(count--))	{
				word = usb_readl(ep->reg_hcfifo);
				*word_buf++	= word;
			}
		}

		if (remain)	{
			u8 *remain_buf = (u8 *)buf;
			fifo = ep->reg_hcfifo;
			
			while (remain--) {
				*remain_buf++ =	usb_readb(fifo);
				fifo++;
			}	
		}
		urb->actual_length += length;
		usb_dotoggle(urb->dev, usb_pipeendpoint(urb->pipe),	0);

		if (q->cur_sg) {
			dma_sync_sg_for_device(aotg_to_hcd(acthcd)->self.controller,q->cur_sg, 1, DMA_TO_DEVICE);
			//dma_cache_wback((u32)(sg_virt(q->cur_sg) + q->cur_sg_actual),	length);
			q->cur_sg_actual +=	length;
			if (q->cur_sg_actual >=	q->cur_sg->length) {
				q->cur_sg =	sg_next(q->cur_sg);
				q->cur_sg_actual = 0;
			}
		}
		if (highmem_kmaped != 0) {
			ACT_HCD_ERR
			highmem_kmaped = 0;
		}
		if (is_short) {
			//if (fifo_cnt == 0) 
				usb_writeb(0, ep->reg_hcepcs);
			break;
		}
	}
	
	if (urb->actual_length >= urb->transfer_buffer_length) {
		ep_disable(ep);
		__hc_in_stop(acthcd, ep);
		pio_irq_disable(acthcd,	ep->mask);
		pio_irq_clear(acthcd, ep->mask);
		usb_writeb(0x3f, acthcd->base +	HCIN07ERRIRQ); //hid device	should clear it.

		/* q->cur_sg maybe set to NULL in q->cur_sg	= sg_next(q->cur_sg); */
		if (!q->cur_sg && urb->actual_length > 0) {
					//if (urb->transfer_buffer != NULL)
				//dma_cache_wback((u32)urb->transfer_buffer, urb->actual_length);
		}
		finish_request(acthcd, q, 0);
	} else if (is_short) {
		ep_disable(ep);
		__hc_in_stop(acthcd, ep);
		pio_irq_disable(acthcd,	ep->mask);
		pio_irq_clear(acthcd, ep->mask);
		usb_writeb(0x3f, acthcd->base +	HCIN07ERRIRQ); //hid device	should clear it.

		if (urb->transfer_flags	& URB_SHORT_NOT_OK)	{
			finish_request(acthcd, q, -EREMOTEIO);
		} else { 
			finish_request(acthcd, q, 0);
		}
	}

	return;
}

static int handle_hcep_pio(struct aotg_hcd *acthcd,	
		struct aotg_hcep *ep, struct aotg_queue	*q)
{
	int	count;
	int	remain;
	u16	*short_buf;
	u32	*word_buf;
	void * __iomem fifo;
	u8 *p;
	int	highmem_kmaped = 0;
	struct urb *urb	= q->urb;
	int	i =	0;
	int	n =	0;
	int	length = 0;
	void *buf =	NULL;

	//ACT_HCD_DBG

	if (urb->actual_length >= urb->transfer_buffer_length) {
		if (q->need_zero) {
			q->need_zero = 0;
			usb_writew(0, ep->reg_hcepbc);
			usb_writeb(0, ep->reg_hcepcs);
			usb_dotoggle(urb->dev, usb_pipeendpoint(urb->pipe),	1);
		} else {
			return 1;
		}
	} else {
		while ((!(usb_readb(ep->reg_hcepcs)	& EPCS_BUSY))
				&& (urb->transfer_buffer_length	> urb->actual_length)) {
			i++;
			if (i >	1000000) {
				printk("%s timeout!\n",	__func__);
				break;
			}
			highmem_kmaped = 0;
			if (q->cur_sg) {
				ACT_HCD_ERR
			} else {
				if (urb->sg	!= NULL) {
					ACT_HCD_ERR
				} else if (urb->transfer_buffer	!= NULL) {
					buf	= urb->transfer_buffer + urb->actual_length;
					length = min((int)urb->transfer_buffer_length -	(int)urb->actual_length, 
							(int)ep->maxpacket);
				} else {
					WARN_ON(1);
				}
			}
			
			WARN_ON(length == 0);
			count =	length >> 2;
			remain = length	& 0x3;
			prefetch(buf);

			if ((u32)buf & 0x1)	{
				u8 *byte_buf = (u8 *)buf;
				
				while (likely(count--))	{
					u32	word = *byte_buf | *(byte_buf +	1) << 8	
							| *(byte_buf + 2) << 16	| *(byte_buf + 3) << 24;
					byte_buf +=	4;
					usb_writel(word, ep->reg_hcfifo);
				}
				buf	= byte_buf;
			} else if ((u32)buf	& 0x2) {
				short_buf =	(u16 *)buf;

				while (likely(count--))	{
					u32	word = *short_buf |	*(short_buf	+ 1) <<	16;
					short_buf += 2;
					usb_writel(word, ep->reg_hcfifo);
				}
				buf	= short_buf;
			}
			else {
				word_buf = (u32	*)buf;

				while (likely(count--))	{
					usb_writel(*word_buf++,	ep->reg_hcfifo);
				}
				buf	= word_buf;
			}
			
			if (remain)	{
				fifo = ep->reg_hcfifo;
				p =	(u8	*)buf;
				for	(n = 0;	n <	remain;	n++) {
					usb_writeb(*p++, fifo);
					/* do not increase fifo	addr */
					//fifo += 1;
				}
			}
			urb->actual_length += length;

			//if (length < ep->maxpacket) {
				usb_writew(length, ep->reg_hcepbc);
				usb_writeb(0, ep->reg_hcepcs);
			//}
			
			if (q->cur_sg) {
				//dma_sync_sg_for_device(aotg_to_hcd(acthcd)->self.controller,q->cur_sg, 1,	DMA_TO_DEVICE);
				q->cur_sg_actual +=	length;
				if (q->cur_sg_actual >=	q->cur_sg->length) {
					q->cur_sg =	sg_next(q->cur_sg);
					q->cur_sg_actual = 0;
				}
			}

			if (highmem_kmaped != 0) {
				ACT_HCD_ERR
			}
			
			usb_dotoggle(urb->dev, usb_pipeendpoint(urb->pipe),	1);
		}
	}
	
	pio_irq_enable(acthcd, ep->mask);
	return 0;
}

static void	handle_hcep_out(struct aotg_hcd	*acthcd, int index)
{
	struct aotg_hcep *ep;
	struct aotg_queue *q;

	ep = acthcd->outep[index];
	if (!ep	|| list_empty(&ep->q_list))	{
		return;
	}
	
	if (usb_readb(ep->reg_hcepcs) &	EPCS_BUSY)
		return;

	q =	list_entry(ep->q_list.next,	struct aotg_queue, ep_q_list);
	if (unlikely(q->is_start_dma))		//avoid	hardware bug
		return;
	
	if (ep->is_use_pio)	{
		if (handle_hcep_pio(acthcd,	ep,	q))
			finish_request(acthcd, q, 0);
	} else {
		if (q->data_done) {
			q->data_done = 0;
			if (q->need_zero) {
				q->need_zero = 0;
				usb_writew(0, ep->reg_hcepbc);
				usb_writeb(0, ep->reg_hcepcs);
			}
			else {
				finish_request(acthcd, q, 0);
			}
		}
	}
	return;
}

static int aotg_hcd_start_dma(struct aotg_hcd *acthcd, struct aotg_queue *q, int dmanr)
{
	struct urb *urb	= q->urb;
	struct aotg_hcep *ep = q->ep;
	u32	dma, i;
	u8 is_in = !usb_pipeout(urb->pipe);
	u8 ep_select;
	u32	dma_length = 0;
	u8 reg_hcoutxerr;

	if (acthcd->dma_queue[dmanr & 0x1].next != &q->dma_q_list || q->is_start_dma)
		goto done;

	if (unlikely(!HC_IS_RUNNING(aotg_to_hcd(acthcd)->state)))
		return -ESHUTDOWN;
	i =	aotg_dma_get_cmd(dmanr);
	if (i & 0x1) {
		ACT_HCD_ERR
	} 

	ACT_DMA_DEBUG("expect length: %d, dir: %s, %s, %d\n",
			q->length, usb_pipeout(urb->pipe)?"out":"in",
			__func__, __LINE__);

	ep->dma_bytes = 0;
	if (q->cur_sg) {
		ACT_HCD_ERR
		goto done;
	} else {
		if (urb->sg	== NULL) {
			//dma =	((u32)urb->transfer_dma	+ urb->actual_length) |	0x80000000;
			if (urb->actual_length != 0) {
				ACT_HCD_ERR
				printk("urb->actual_length:%d\n", urb->actual_length);
				urb->actual_length = 0;
			}
			dma	= (u32)urb->transfer_dma + urb->actual_length;
			ep->dma_bytes =	q->length;
			dma_length = ep->dma_bytes;
		} else {
			//ACT_HCD_ERR
			if (PageHighMem(sg_page(urb->sg))) {
				ACT_HCD_ERR
				urb->transfer_dma &= 0xfffffff;
				urb->transfer_dma |= 0x90000000;
			}
			//dma =	((u32)urb->transfer_dma	+ urb->actual_length) |	0x80000000;
			dma	= (u32)urb->transfer_dma + urb->actual_length;
			ep->dma_bytes =	q->length;
			dma_length = ep->dma_bytes;
		}
	}

	WARN_ON(dma_length == 0	|| ep->dma_bytes > q->length);

	if (is_in) {

		if ((EPCS_BUSY & usb_readb(ep->reg_hcepcs))	== 0) {
			ACT_HCD_ERR
			printk("fifo_cnt = %d\n", (int)(usb_readw(ep->reg_hcepbc) &	0xFFF));
			i =	0;
			while ((EPCS_BUSY &	usb_readb(ep->reg_hcepcs)) == 0) {
				i++;
				usb_readl(ep->reg_hcfifo);
			}
			printk("i =	%d\n", i);
			if ((0x1 <<	ep->index) & usb_readb(acthcd->base	+ HCINXSTART)) {
				printk("HCINXSTART:%x\n", (unsigned	int)usb_readb(acthcd->base + HCINXSTART));
				printk("ep->index:%d\n", ep->index);
			}
		}
	} else {
#ifdef MULTI_OUT_FIFO
		if ((EPCS_BUSY & usb_readb(ep->reg_hcepcs))	!= 0 ||\
		     (EPCS_NPAK & usb_readb(ep->reg_hcepcs)) != (0x01 << 2)) {
#else
		if ((EPCS_BUSY & usb_readb(ep->reg_hcepcs))	!= 0 ) {
#endif
			ACT_HCD_ERR
		}
#if	0
		i =	100000;
		while (((EPCS_BUSY & usb_readb(ep->reg_hcepcs))	!= 0) && i)	{ i--; udelay(1);};
		if (i == 0)	{ 
			printk("i:%d\n", i);

			printk("acthcd->base:%x	\n", (unsigned int)acthcd->base);
			printk("reg_hcepcs:%x \n", (unsigned int)ep->reg_hcepcs);
			printk("HCOUT1CS:%x	\n", (unsigned int)HCOUT1CS);
			printk("HCOUT2CS:%x	\n", (unsigned int)HCOUT2CS);
			printk("EPCS_BUSY:%x \n", (unsigned	int)EPCS_BUSY);
			printk("val:%x \n",	(unsigned int)usb_readb(ep->reg_hcepcs));

			aotg_dbg_proc_output_ep_state(acthcd);
			printk("\n"); printk("\n");	printk("\n"); 
			aotg_dbg_regs(acthcd);
			printk("\n"); printk("\n");	printk("\n");

			usb_writeb(0x1<<6 |	0x1<<5,	acthcd->base + HCPORTCTRL);	/*portrst &	55ms */
			aotg_dbg_output_info();
			printk("\n"); printk("\n");	printk("\n");
			aotg_dbg_proc_output_ep();
			WARN_ON(1);
		}
#endif
	}

	pio_irq_disable(acthcd,	ep->mask);
	acthcd->dma_working[dmanr &	0x1] = urb->pipe;

	q->is_start_dma	= dmanr	+ 1;

	ep_select =	(ep->index << 1) | is_in;
	aotg_dma_set_mode(dmanr, ep_select);
	
	if (is_in) {
		if (q->cur_sg) {
			ACT_HCD_ERR
		}
		else {
			aotg_dma_set_memaddr(dmanr,	dma);
		}
		aotg_dma_set_cnt(dmanr,	dma_length);
	} else {
		if (q->cur_sg) {
			ACT_HCD_ERR
		}
		else {
			aotg_dma_set_memaddr(dmanr,	dma);
		}
		
#ifdef SHORT_PACK_487_490
		ep->shortpack_length = dma_length % 512;
			
		if(ep->shortpack_length >= 487 && ep->shortpack_length <= 490 \
			&& ep->shortpack_length == dma_length){
			printk("WARNING:dma_length == short_pack_length = %u\n", ep->shortpack_length);
		    i = 100000;
		
#ifdef MULTI_OUT_FIFO		
			while(((usb_readb(ep->reg_hcepcs) & EPCS_BUSY) != 0 \
			|| (EPCS_NPAK & usb_readb(ep->reg_hcepcs))!= (0x01 << 2)) && i)
			{
#else		
			while((usb_readb(ep->reg_hcepcs) & EPCS_BUSY) && i)
			{
#endif
				i--;
				udelay(10);
			}	
		
			if(!i)
			{
				printk("%s,ERR:wait 1000 ms, fifo still busy!--->sending the SHORT_PACK_487_490\n",__FUNCTION__);
				aotg_hcd_show_ep_info(acthcd);
				aotg_dbg_regs(acthcd);
			}		
		
			//ping before next dma transfer
//			printk("%s,sending PING package:[ep->reg_hcepcs = 0x%X]\n",__FUNCTION__,usb_readb(ep->reg_hcepcs));
			reg_hcoutxerr = usb_readb(ep->reg_hcerr);
			usb_writeb(reg_hcoutxerr | DO_PING, ep->reg_hcerr); //DO_PING 0x1 << 6
			
			ep->shortpack_length = 0;
			ep->is_shortpack_487_490 = false;			
		}else if(ep->shortpack_length >= 487 && ep->shortpack_length <= 490){
			printk("WARNING:dma_length = %u, short_pack_length = %u\n", dma_length,ep->shortpack_length);
			ep->dma_bytes = dma_length - ep->shortpack_length;
			dma_length = ep->dma_bytes;
			ep->is_shortpack_487_490 = true;
			printk("WARNING:dma_bytes = %u\n",ep->dma_bytes);
			printk("WARNING:go to deal with SHORT_PACK_487_490--->\n");
		}else{
			ep->shortpack_length = 0;
			ep->is_shortpack_487_490 = false;
		}
#endif

		aotg_dma_set_cnt(dmanr,	dma_length);
	}

	if (q->cur_sg) {
		ACT_HCD_ERR
	}

	//if (is_in) {
		//aotg_hcep_reset(acthcd, ep->mask,	ENDPRST_FIFORST	| ENDPRST_TOGRST);
	//}
	usb_writew(0, ep->reg_hcepbc);
#if	0
	/* 
	 * check dma addr right	or not.	
	 * why when	we call	get_memaddr, tcp seq out of	order is decreased?	
	 */
	if ((unsigned int)urb->transfer_dma	!= aotg_dma_get_memaddr(dmanr))	{
		ACT_HCD_ERR
		printk("transfer_dma:%x\n",	(unsigned int)urb->transfer_dma);
		printk("get_memaddr:%x\n", (unsigned int)aotg_dma_get_memaddr(dmanr));
	}
#endif
	aotg_dma_start(dmanr);
#if	0
	if (is_in) {
		if (aotg_dma_get_cnt(acthcd->dma_nr0) != aotg_dma_get_remain(acthcd->dma_nr0)) {
			ACT_HCD_ERR
			printk("dmanr:%d\n", dmanr);
			printk("acthcd->dma_nr0:%d\n", acthcd->dma_nr0);
			printk("cnt:%d\n", aotg_dma_get_cnt(acthcd->dma_nr0));
			printk("remain:%d\n", aotg_dma_get_remain(acthcd->dma_nr0));
		}
	}
#endif
done:
	return 0;
}

static void	aotg_hcd_dma_handler(int dma_no, void *dev_id)
{
	struct aotg_hcd	*acthcd	= (struct aotg_hcd *)dev_id;
	struct aotg_queue *q;
	struct urb *urb;
	struct aotg_hcep *ep;
	int	length;
	
	u8 ep_select;
	u8 reg_hcoutxerr;
	u32 dma;
	u32 i;
	//unsigned int intoken_cnt = 0;

	//WARN_ON(list_empty(&acthcd->dma_queue[dma_no & 0x1]));
	if (list_empty(&acthcd->dma_queue[dma_no & 0x1])) {
		ACT_HCD_ERR
		return;
	}

	q =	list_entry(acthcd->dma_queue[dma_no	& 0x1].next, struct	aotg_queue,	dma_q_list);
	urb	= q->urb;
	ep = q->ep;

#if	0
	/* 
	 * check dma addr right	or not.	
	 * why when	we call	get_memaddr, tcp seq out of	order is decreased?	
	 */
	if ((unsigned int)urb->transfer_dma	!= aotg_dma_get_memaddr(dma_no)) {
		ACT_HCD_ERR
		printk("transfer_dma:%x\n",	(unsigned int)urb->transfer_dma);
		printk("get_memaddr:%x\n", (unsigned int)aotg_dma_get_memaddr(dma_no));
	}
#endif

	if (q->cur_sg)
		length = ep->dma_bytes;
	else
		length = ep->dma_bytes - aotg_dma_get_remain(dma_no);
	urb->actual_length += length;
	
#ifdef SHORT_PACK_487_490
	if((usb_pipeout(urb->pipe)) && ep->is_shortpack_487_490){
//		if(ep->shortpack_length >= 487 && ep->shortpack_length <= 490)
		printk("WARNING:deal with the SHORT_PACK_487_490\n");
		
		i = 100000;
#ifdef MULTI_OUT_FIFO
		while(((usb_readb(ep->reg_hcepcs) & EPCS_BUSY) != 0 \
		|| (EPCS_NPAK & usb_readb(ep->reg_hcepcs))!= (0x01 << 2)) && i)
		{
#else
		while(((usb_readb(ep->reg_hcepcs) & EPCS_BUSY) != 0) && i)
		{
#endif
			i--;
			udelay(10);
		}	
		
		if(!i)
		{
			printk("%s,ERR:wait 1000 ms, fifo still busy!--->sending the SHORT_PACK_487_490\n",__FUNCTION__);
			aotg_hcd_show_ep_info(acthcd);
			aotg_dbg_regs(acthcd);
		}	
		
		//ping before next dma transfer
//		printk("%s,sending PING package:[ep->reg_hcepcs = 0x%X]\n",__FUNCTION__,usb_readb(ep->reg_hcepcs));
		reg_hcoutxerr = usb_readb(ep->reg_hcerr);
		usb_writeb(reg_hcoutxerr | DO_PING, ep->reg_hcerr); //DO_PING 0x1 << 6
		
		ep_select = (ep->index << 1) | 0x0; //0x0 means OUT
		dma = (u32)urb->transfer_dma + urb->actual_length;
		ep->dma_bytes = ep->shortpack_length;
		
		aotg_dma_set_mode(acthcd->dma_nr1, ep_select);
		aotg_dma_set_memaddr(acthcd->dma_nr1, dma);
		aotg_dma_set_cnt(acthcd->dma_nr1, ep->shortpack_length);
		
		usb_writew(0, ep->reg_hcepbc);
		aotg_dma_start(acthcd->dma_nr1);
				
//		if(ep->shortpack_length >= 487 && ep->shortpack_length <= 490){
//			printk("WARNING:urb->actual_length = %u, ep->dma_bytes = %u\n",urb->actual_length,ep->dma_bytes);
//			printk("WARNING:finish SHORT_PACK_487_490\n");		
//		}
		printk("WARNING:urb->actual_length = %u, ep->dma_bytes = %u\n",urb->actual_length,ep->dma_bytes);
		printk("WARNING:finish SHORT_PACK_487_490\n");	
		
		ep->shortpack_length = 0;
		ep->is_shortpack_487_490 = false;
		return;			
	}
#endif
	
	acthcd->dma_working[dma_no & 0x1] =	0;

	if (usb_pipeout(urb->pipe))	{
		if (aotg_dma_get_remain(dma_no)	!= 0) {
			printk("bytes:%d, remain:%d\n",	ep->dma_bytes, aotg_dma_get_remain(dma_no));
		}
	} else {
		acthcd->last_bulkin_dma_len	= (unsigned	int)urb->actual_length;
	}
#if	0
	{
		acthcd->last_bulkin_dma_len	= (unsigned	int)urb->actual_length;
		//intoken_cnt =	(unsigned int)usb_readw(ep->reg_hcincount_rd);
		//intoken_cnt =	(urb->transfer_buffer_length / ep->maxpacket) -	intoken_cnt;

		if ((intoken_cnt * ep->maxpacket) <	urb->actual_length)	{
			ACT_HCD_DBG
			printk("in count:%d\n",	(urb->transfer_buffer_length / ep->maxpacket));
			printk("urb->actual_length:%d\n", urb->actual_length);
			printk("hcepbc:%d\n", (int)(usb_readw(ep->reg_hcepbc) &	0xFFF));
			printk("hcin_cnt:%d\n",	usb_readw(ep->reg_hcincount_rd));
		}
	}
#endif

	q->is_start_dma	= 0;
	list_del_init(&q->dma_q_list);
	//aotg_dma_stop(dma_no);
	//aotg_dma_reset_2(dma_no);

	if ((acthcd->dma_working[0]	== 0) && (acthcd->dma_working[1] ==	0))	{
		if ((0x1 & aotg_dma_get_cmd(acthcd->dma_nr0)) || (0x1 &	aotg_dma_get_cmd(acthcd->dma_nr1)))	{
			ACT_HCD_ERR
			printk("dma0:%d\n",	aotg_dma_get_cmd(acthcd->dma_nr0));
			printk("dma1:%d\n",	aotg_dma_get_cmd(acthcd->dma_nr1));
		} else {
			acthcd->last_bulkin_dma_len	= 1;
			aotg_dma_reset_2(acthcd->dma_nr0);
		}
		//printk("dma reset! \n");
	}
	
	if (((length + ep->maxpacket - 1)/ ep->maxpacket) &	1)
		usb_dotoggle(urb->dev, usb_pipeendpoint(urb->pipe),	usb_pipeout(urb->pipe));

	ACT_DMA_DEBUG("actual length: %d, dir: %s, %s, %d\n",
		length,	usb_pipeout(urb->pipe)?"out":"in",
		__func__, __LINE__);

	if (usb_pipein(urb->pipe)) {
		if (!(usb_readb(ep->reg_hcepcs)	& EPCS_BUSY)) {
			usb_writeb(0, ep->reg_hcepcs);
			usb_dotoggle(urb->dev, usb_pipeendpoint(urb->pipe),	0);
		} else {
			//ACT_HCD_DBG
		}

		ep_disable(ep);
		__hc_in_stop(acthcd, ep);
		pio_irq_clear(acthcd, ep->mask);
		//pio_irq_enable(acthcd, ep->mask);

		if (urb->transfer_flags	& URB_SHORT_NOT_OK)	{
			finish_request(acthcd, q, 
					urb->actual_length < urb->transfer_buffer_length ? -EREMOTEIO :	0);	
		} else {
			finish_request(acthcd, q, 0);
		}
	} else {
#if	0
		u32	mask = ep->maxpacket - 1;
		int	remain = length	& mask;

		if (remain)	{
			if (!(usb_readb(ep->reg_hcepcs)	& EPCS_BUSY)) {
				usb_writew(remain, ep->reg_hcepbc);	 
				usb_writeb(0, ep->reg_hcepcs);
				q->data_done = 1;
			}
		}
#endif
		/* 
		 * maybe because it's single fifo, and data	just stay in fifo, 
		 * that	cause EPCS_BUSY	bit	be set.	
		 */	
		// if (usb_readb(ep->reg_hcepcs) & EPCS_BUSY)
#ifdef MULTI_OUT_FIFO
		if (((usb_readb(ep->reg_hcepcs) & EPCS_BUSY) != 0 || ((EPCS_NPAK & usb_readb(ep->reg_hcepcs)) !=	\
		(0x01 << 2))) && (urb->actual_length < urb->transfer_buffer_length))	{
#else
		if ((usb_readb(ep->reg_hcepcs) & EPCS_BUSY ) &&	
			(urb->actual_length	< urb->transfer_buffer_length))	{
#endif
			q->data_done = 1;
			pio_irq_clear(acthcd, ep->mask);
			//pio_irq_enable(acthcd, ep->mask);

		} else if ((q->need_zero) && 
				   ((urb->actual_length	== urb->transfer_buffer_length)	&& (urb->actual_length % ep->maxpacket == 0))) {
			q->data_done = 1;
			q->need_zero = 0;
			if ((EPCS_BUSY & usb_readb(ep->reg_hcepcs))	!= 0) {
				//printk("aotg_hcd_seq_info:%d\n", aotg_hcd_seq_info);
			}
			usb_writew(0, ep->reg_hcepbc);
			usb_writeb(0, ep->reg_hcepcs);
			usb_dotoggle(urb->dev, usb_pipeendpoint(urb->pipe),	1);
			pio_irq_clear(acthcd, ep->mask);
			//pio_irq_enable(acthcd, ep->mask);
			finish_request(acthcd, q, 0);

		} else {
#ifdef MULTI_OUT_FIFO
			if ((EPCS_BUSY & usb_readb(ep->reg_hcepcs)) != 0 || (EPCS_NPAK & usb_readb(ep->reg_hcepcs)) \
				  != (0x01 << 2)) {
#else
			if ((EPCS_BUSY & usb_readb(ep->reg_hcepcs))	!= 0 ) {
#endif
				//ACT_HCD_DBG
			}
			//length = 10000;
			//while	(((EPCS_BUSY & usb_readb(ep->reg_hcepcs)) != 0)	&& length) { length--; udelay(1);};
			//if (length ==	0) printk("length:%d\n", length);
			pio_irq_clear(acthcd, ep->mask);
			//pio_irq_enable(acthcd, ep->mask);
			//if ((EPCS_BUSY & usb_readb(ep->reg_hcepcs)) != 0)	{
				//ACT_HCD_DBG
			//}
			finish_request(acthcd, q, 0);
		}
	}

	return;
}

static __inline__ void __handle_hcout_packet(struct	aotg_hcd *acthcd, struct aotg_queue	*q)
{
	struct urb *urb	= q->urb;
	struct aotg_hcep *ep = q->ep;
	
	if (acthcd->use_dma) {
		if (((acthcd->dma_working[1] ==	0) && (urb->transfer_buffer_length >= AOTG_MIN_DMA_SIZE)) || 
			(urb->pipe == acthcd->dma_working[1])) {
			q->length =	urb->transfer_buffer_length;
			list_add_tail(&q->dma_q_list, &acthcd->dma_queue[1]);
			ep->is_use_pio = 0;
			aotg_hcd_start_dma(acthcd, q, acthcd->dma_nr1);
			return;
		}
	} 

	//printk("dma_working[1]:%x	\n", (unsigned int)acthcd->dma_working[1]);
	//printk("transfer_buffer_length:%x	\n", (unsigned int)urb->transfer_buffer_length);
	//printk("pipe:%x \n", (unsigned int)urb->pipe);

	ep->is_use_pio = 1;
	handle_hcep_pio(acthcd,	ep,	q);
	return;
}

static int handle_bulkout_packet(struct	aotg_hcd *acthcd, struct aotg_queue	*q)
{
	struct aotg_hcep *ep = q->ep;
	//int i, i0, i1, cnt;

	if (q->is_xfer_start ||	ep->q_list.next	!= &q->ep_q_list)
		return 0;

	//printk("out %x: ", usb_pipeendpoint(urb->pipe));
	//printk("buf:%x ",	(unsigned int)urb->transfer_buffer);
	//printk("dma:%x ",	(unsigned int)urb->transfer_dma);
	//printk("len:%d ",	(unsigned int)urb->transfer_buffer_length);
	//printk("act:%d\n", (unsigned int)urb->actual_length);
	//printk("\n");

	if (!HC_IS_RUNNING(aotg_to_hcd(acthcd)->state))
		return -ESHUTDOWN;

	q->is_xfer_start = 1;
	__handle_hcout_packet(acthcd, q);
#if	0
	cnt	= 0; i0	= 0; i1	= 0;
	for	(i = 1;	i <	MAX_EP_NUM;	i++) {
		ep = acthcd->outep[i];
		if (ep)	{
			if (ep->urb_enque_cnt >	(ep->urb_endque_cnt	+ ep->urb_stop_stran_cnt)) {
				i0 = i1;
				i1 = i;
				cnt++;
			}
		}
	}
	if (cnt	>= 2) {
		printk("i0:%d, i1:%d\n", i0, i1);
	}
#endif

	return 0;
}

static int handle_bulkin_packet(struct aotg_hcd	*acthcd, struct	aotg_queue *q)
{
	struct urb *urb	= q->urb;
	struct aotg_hcep *ep = q->ep;
	u16	w_packets;
	int	retval = 0;
	
	if (q->is_xfer_start ||	ep->q_list.next	!= &q->ep_q_list)
		return 0;

	if (!HC_IS_RUNNING(aotg_to_hcd(acthcd)->state))
		return -ESHUTDOWN;

	q->is_xfer_start = 1;
	w_packets =	urb->transfer_buffer_length	/ ep->maxpacket;
	if (urb->transfer_buffer_length	& (ep->maxpacket - 1))
		w_packets++;

	if (acthcd->use_dma) {
		if (((acthcd->dma_working[0] ==	0) && (urb->transfer_buffer_length >= AOTG_MIN_DMA_SIZE)) || 
			(acthcd->dma_working[0]	== urb->pipe)) {
			q->length =	urb->transfer_buffer_length;
			list_add_tail(&q->dma_q_list, &acthcd->dma_queue[0]);
			ep->is_use_pio = 0;
			aotg_hcd_start_dma(acthcd, q, acthcd->dma_nr0);
			goto xfer_start;
		} 
	}
	//printk("dma_working[1]:%x	\n", (unsigned int)acthcd->dma_working[0]);
	//printk("transfer_buffer_length:%x	\n", (unsigned int)urb->transfer_buffer_length);
	//printk("pipe:%x \n", (unsigned int)urb->pipe);

	ep->is_use_pio = 1;
xfer_start:	
	/* do not worry	about re enable	again, for at the func's start position, 
	 * it already check	(ep->q_list.next !=	&q->ep_q_list).	
	 */
	ep_enable(ep);
#if	1
	if (ep->is_use_pio)	{
		aotg_hcep_reset(acthcd,	ep->mask, ENDPRST_FIFORST);
		pio_irq_clear(acthcd, ep->mask);
		pio_irq_enable(acthcd, ep->mask);
	}
#endif
	__hc_in_start(acthcd, ep, w_packets);
	ep->nextpid	= USB_PID_IN;
	q->inpacket_count =	w_packets;
	
	return retval;
}

static int handle_intr_packet(struct aotg_hcd *acthcd, struct aotg_queue *q)
{
	struct aotg_hcep *ep = q->ep;

	q->is_xfer_start = 1;
	aotg_to_hcd(acthcd)->self.bandwidth_allocated += ep->load /	ep->period;
	aotg_to_hcd(acthcd)->self.bandwidth_int_reqs++;

	queue_delayed_work(acthcd->periodic_wq,	&ep->dwork,	msecs_to_jiffies(ep->period));
	return 0;
}

static int aotg_hcd_period_transfer(struct aotg_hcd	*acthcd, struct	aotg_hcep *ep)
{
	struct aotg_hcep *next = ep;
	struct urb *urb;
	struct aotg_queue *q;

	if (!ep)
		return 0;

	do {
		if (likely(!list_empty(&next->q_list)))	{
		q =	list_entry(next->q_list.next, struct aotg_queue, ep_q_list);
		urb	= q->urb;
		WARN_ON(q->ep != next);
		WARN_ON(next->type != PIPE_INTERRUPT &&	next->type != PIPE_ISOCHRONOUS);
			switch (next->nextpid) {
			case USB_PID_IN: 
		{
				u16	w_packets;

			w_packets =	urb->transfer_buffer_length	/ MAX_PACKET(next->maxpacket);
			if (urb->transfer_buffer_length	& (MAX_PACKET(next->maxpacket) - 1)) {
				 w_packets++;
			}

			//reset	fifo, skip the undo	data
			aotg_hcep_reset(acthcd,	next->mask,	ENDPRST_FIFORST);
			ep_enable(next);
			ep->is_use_pio = 1;
			__hc_in_start(acthcd, next,	w_packets);
				break;
			}

			case USB_PID_OUT: //not	support
				__handle_hcout_packet(acthcd, q);
				break;
			default:
				dev_err(acthcd->dev, "<ASYNC XFERS>odd bulk	xfers stage, %s, %d\n",	
						__func__, __LINE__);
				break;
			}
			}
		next = next->next;
	} while	(next);

	return 0;
}

static void	aotg_hcd_error(struct aotg_hcd *acthcd,	u32	irqvector)
{
	struct aotg_queue *q;
	struct aotg_hcep *ep = NULL;
	struct urb *urb;
	int	status;
	u8 error = 0;
	u8 reset = 0;

//	printk("irqvector: 0x%x\n",	irqvector);

	switch (irqvector) {
	case 0x16:
		usb_writeb(1 <<	0, acthcd->base	+ HCOUT07ERRIRQ);
		ep = acthcd->ep0;
		error =	usb_readb(acthcd->base + HCOUT0ERR);
		break;

	case 0x1a:
		usb_writeb(1 <<	0, acthcd->base	+ HCIN07ERRIRQ);
		ep = acthcd->ep0;
		error =	usb_readb(acthcd->base + HCIN0ERR);
		break;

	case UIV_HCOUT1ERR:
		usb_writeb(1 <<	1, acthcd->base	+ HCOUT07ERRIRQ);
		ep = acthcd->outep[1];
		error =	usb_readb(acthcd->base + HCOUT1ERR);
		break;

	case 0x26:
		usb_writeb(1 <<	1, acthcd->base	+ HCIN07ERRIRQ);
		ep = acthcd->inep[1];
		error =	usb_readb(acthcd->base + HCIN1ERR);
		break;

	case 0x2E:
		usb_writeb(1 <<	2, acthcd->base	+ HCOUT07ERRIRQ);
		ep = acthcd->outep[2];
		error =	usb_readb(acthcd->base + HCOUT2ERR);
		break;

	case UIV_HCIN2ERR:
		usb_writeb(1 <<	2, acthcd->base	+ HCIN07ERRIRQ);
		ep = acthcd->inep[2];
		error =	usb_readb(acthcd->base + HCIN2ERR);
		break;
		
	default:
		dev_err(acthcd->dev, "no such irq! %s %d\n", __func__, __LINE__);
		WARN_ON(1);
	}
//	printk("error:%d, %x\n", error,	error);

	if (ep && !list_empty(&ep->q_list))	{
		q =	list_entry(ep->q_list.next,	struct aotg_queue, ep_q_list);
		urb	= q->urb;

		switch (error &	(7<<2))	{
		case 0:
			status = 0;
			break;
		
		case 3<<2:
			status = -EPIPE;
			reset =	ENDPRST_FIFORST	| ENDPRST_TOGRST;
			break;
		
		case 4<<2:
			status = -ETIMEDOUT;
			reset =	ENDPRST_FIFORST	| ENDPRST_TOGRST;
			break;

		default:
			status = -EIO;
			reset =	ENDPRST_FIFORST	| ENDPRST_TOGRST;
		}

		if (status < 0)	{
			if (ep->index >	0 && status	!= -EPIPE) {
				ACT_HCD_DBG
				if (q->err_count < MAX_ERROR_COUNT)	{
					ACT_HCD_DBG
					q->err_count++;
					usb_writeb(0x1 << 5, ep->reg_hcerr);
					return;
				}
			}

			ACT_HCD_DBG
			printk("status:%d\n", status);

			if (ep->index >	0) {
				if (usb_pipeout(urb->pipe))	{
					ACT_HCD_DBG
					aotg_hcep_reset(acthcd,	ep->mask | USB_HCD_OUT_MASK, reset);
				} else {
					ACT_HCD_DBG
					if (ep->index >	0) {
						ACT_HCD_DBG
						ep_disable(ep);
						__hc_in_stop(acthcd, ep);
					}
					aotg_hcep_reset(acthcd,	ep->mask, reset);
				}
			}
			dev_info(acthcd->dev, "%s ep %d	error [0x%02X] error type [0x%02X],	reset it...\n",
					usb_pipeout(urb->pipe)?"HC OUT":"HC	IN", ep->index,	error, (error>>2)&0x7);

			if (q->is_start_dma) {
				ACT_HCD_DBG
				__clear_dma(acthcd,	q, q->is_start_dma-1);
			}

			finish_request(acthcd, q, status);
		}
	}
	tasklet_hi_schedule(&acthcd->urb_tasklet);

	return;
}

static int inline aotg_hcd_handle_dma_pend(struct aotg_hcd *acthcd)
{
	unsigned int dma_pend, dma_cmd;

	dma_pend = (unsigned int)aotg_dma_is_irq_2(acthcd->dma_nr0);
	if (dma_pend ==	0)
		return 0;

do_dma_pending:
	if (dma_pend & 0x1)	{
		aotg_dma_clear_pend(acthcd->dma_nr0);
		dma_cmd	= 0x1 &	aotg_dma_get_cmd(acthcd->dma_nr0);
		if (dma_cmd) {
			ACT_HCD_ERR
		} else if (acthcd->dma_working[0] == 0)	{
			ACT_HCD_ERR
		} else {
			aotg_hcd_dma_handler(acthcd->dma_nr0, acthcd);
		}
	}
	if (dma_pend & 0x2)	{
		aotg_dma_clear_pend(acthcd->dma_nr1);
		dma_cmd	= 0x1 &	aotg_dma_get_cmd(acthcd->dma_nr1);
		if (dma_cmd) {
			ACT_HCD_ERR
		} else if (acthcd->dma_working[1] == 0)	{
			ACT_HCD_ERR
		} else {
			aotg_hcd_dma_handler(acthcd->dma_nr1, acthcd);
		}
	}
	dma_pend = (unsigned int)aotg_dma_is_irq_2(acthcd->dma_nr0);
	if (dma_pend)
		goto do_dma_pending;

	/* deal	with some wrong	state that dma0	and	dma1 is	stopped	but	no dma irq pending.	*/
	dma_cmd	= 0x1 &	aotg_dma_get_cmd(acthcd->dma_nr0);
	if ((dma_cmd ==	0) && (acthcd->dma_working[0] != 0)) {
		dma_pend = (unsigned int)aotg_dma_is_irq_2(acthcd->dma_nr0);
		if (dma_pend) {
			goto do_dma_pending;
		}
		ACT_HCD_ERR
		aotg_hcd_dma_handler(acthcd->dma_nr0, acthcd);
	}
	dma_cmd	= 0x1 &	aotg_dma_get_cmd(acthcd->dma_nr1);
	if ((dma_cmd ==	0) && (acthcd->dma_working[1] != 0)) {
		dma_pend = (unsigned int)aotg_dma_is_irq_2(acthcd->dma_nr1);
		if (dma_pend) {
			goto do_dma_pending;
		}
		ACT_HCD_ERR
		aotg_hcd_dma_handler(acthcd->dma_nr1, acthcd);
	}

	dma_pend = (unsigned int)aotg_dma_is_irq_2(acthcd->dma_nr0);
	if (dma_pend) {
		goto do_dma_pending;
	}

	return 0;
}

static inline void handle_usbreset_isr(struct aotg_hcd *acthcd)
{
	usb_writeb(USBIRQ_URES,	acthcd->base + USBIRQ);	/*clear	usb	reset irq */

	if (acthcd->port & (USB_PORT_STAT_POWER	| USB_PORT_STAT_CONNECTION)) {
		acthcd->speed =	USB_SPEED_FULL;	/*FS is	the	default	*/
		acthcd->port |=	(USB_PORT_STAT_C_RESET << 16);
		acthcd->port &=	~USB_PORT_STAT_RESET;

		/*reset	all	ep-in */
		aotg_hcep_reset(acthcd,	USB_HCD_IN_MASK, ENDPRST_FIFORST | ENDPRST_TOGRST);
		/*reset	all	ep-out */
		aotg_hcep_reset(acthcd,	USB_HCD_OUT_MASK, ENDPRST_FIFORST |	ENDPRST_TOGRST);

		acthcd->port |=	USB_PORT_STAT_ENABLE;
		acthcd->rhstate	= AOTG_RH_ENABLE;
		/*now root port	is enabled fully */
		if (usb_readb(acthcd->base + USBCS)	& USBCS_HFMODE)	{
			acthcd->speed =	USB_SPEED_HIGH;
			acthcd->port |=	USB_PORT_STAT_HIGH_SPEED;
			usb_writeb(USBIRQ_HS, acthcd->base + USBIRQ);
			HCD_DEBUG("%s: USB device is  HS\n", __func__);
		}
		else if	(usb_readb(acthcd->base	+ USBCS) & USBCS_LSMODE) {
			acthcd->speed =	USB_SPEED_LOW;
			acthcd->port |=	USB_PORT_STAT_LOW_SPEED;
			HCD_DEBUG("%s: USB device is  LS\n", __func__);
		} else {
			acthcd->speed =	USB_SPEED_FULL;
			HCD_DEBUG("%s: USB device is  FS\n", __func__);
		}

		/*usb_clearbitsb(USBIEN_URES,USBIEN);*/	/*disable reset	irq	*/
		/*khu del for must enable USBIEN_URES again*/

		//usb_clearbitsb(0x3e, acthcd->base	+ OUTXSHORTPCKIEN);
		//usb_writeb(0x3e, acthcd->base	+ OUTXSHORTPCKIRQ);
		usb_writeb(0x3f, acthcd->base +	HCIN07ERRIRQ);
		usb_writeb(0x3f, acthcd->base +	HCOUT07ERRIRQ);
		usb_writeb(0x3f, acthcd->base +	HCIN07IRQ);
		usb_writeb(0x3f, acthcd->base +	HCOUT07IRQ);

		usb_setbitsb((1	<< 0x00) | (1 << 1)	| (1 <<	2),	acthcd->base + HCIN07ERRIEN);
		usb_setbitsb((1	<< 0x00) | (1 << 1)	| (1 <<	2),	acthcd->base + HCOUT07ERRIEN);

		HCD_DEBUG("%s: USB reset end\n", __func__);
	}

	return;
}

static irqreturn_t aotg_hcd_irq(struct usb_hcd *hcd)
{
	struct platform_device *pdev;
	unsigned int port_no;
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	u8 eirq_mask = usb_readb(acthcd->base +	USBEIEN);
	u8 eirq_pending	= usb_readb(acthcd->base + USBEIRQ);

	/* take	cate to	use	lock, because in irq ->	dma_handler	-> finish_request -> 
	 * usb_hcd_giveback_urb	-> urb->complete(),	it maybe call enqueue and get spin_lock	again.
	 */
	//spin_lock(&acthcd->lock);

	pdev = to_platform_device(hcd->self.controller);
	port_no	= pdev->id & 0xff;
	
	if (eirq_pending & USBEIRQ_USBIRQ) {
		u32	irqvector =	(u32)usb_readb(acthcd->base	+ IVECT);
		usb_writeb(eirq_mask | USBEIRQ_USBIRQ, acthcd->base	+ USBEIRQ);
		//printk("irqvector:%d,	%x\n", irqvector, irqvector);

		switch (irqvector) {
		case UIV_OTGIRQ:
			if (usb_readb(acthcd->base + OTGIRQ) & (0x1<<2)) {
				usb_writeb(0x1<<2, acthcd->base	+ OTGIRQ);
				acthcd->put_aout_msg = 0;
				mod_timer(&acthcd->hotplug_timer, jiffies +	msecs_to_jiffies(1));
				printk("port_no:%d OTG IRQ,	OTGSTATE: 0x%02X, USBIRQ:0x%02X\n",	
					port_no, usb_readb(acthcd->base	+ OTGSTATE),
					usb_readb(acthcd->base + USBIRQ));
				//usb_writeb(0x02, acthcd->base	+ USBIRQ);
			}
			else {
				printk("port_no:%d error OTG irq! OTGIRQ: 0x%02X\n", 
					port_no, usb_readb(acthcd->base	+ OTGIRQ));
			}
			
			break;
		case UIV_SOF:
			usb_writeb(1 <<	1, acthcd->base	+ USBIRQ);
			{
				u16	index;
				struct aotg_hcep *ep;
	
				index =	acthcd->frame;
				ep = acthcd->periodic[index];
				aotg_hcd_period_transfer(acthcd, ep);
	
				acthcd->frame++;
				acthcd->frame =	acthcd->frame %	PERIODIC_SIZE;
			}
	
			break;
		case UIV_USBRESET:
			handle_usbreset_isr(acthcd);
			break;
	
		case UIV_EP0IN:
			usb_writeb(1, acthcd->base + HCOUT07IRQ);	/*clear	hcep0out irq */
			handle_hcep0_out(acthcd);
			break;
		case UIV_EP0OUT:
			usb_writeb(1, acthcd->base + HCIN07IRQ);	/*clear	hcep0in	irq	*/
			handle_hcep0_in(acthcd);
			break;
		case UIV_EP1IN:
			usb_writeb(1<<1, acthcd->base +	HCOUT07IRQ);	/*clear	hcep1out irq */
			handle_hcep_out(acthcd,	1);
			break;
		case UIV_EP1OUT:
			usb_writeb(1<<1, acthcd->base +	HCIN07IRQ);	/*clear	hcep1in	irq	*/
			handle_hcep_in(acthcd, 1);
			break;
		case UIV_EP2IN:
			usb_writeb(1<<2, acthcd->base +	HCOUT07IRQ);	/*clear	hcep2out irq */
			handle_hcep_out(acthcd,	2);
			break;
		case UIV_EP2OUT:
			usb_writeb(1<<2, acthcd->base +	HCIN07IRQ);	/*clear	hcep2in	irq	*/
			handle_hcep_in(acthcd, 2);
			break;
		case UIV_EP3IN:
			usb_writeb(1<<3, acthcd->base +	HCOUT07IRQ);	/*clear	hcep3out irq */
			handle_hcep_out(acthcd,	3);
			break;
		case UIV_EP3OUT:
			usb_writeb(1<<3, acthcd->base +	HCIN07IRQ);	/*clear	hcep3in irq */
			handle_hcep_in(acthcd,	3);
			break;
		case UIV_EP4IN:
			usb_writeb(1<<4, acthcd->base +	HCOUT07IRQ);	/*clear	hcep4out irq */
			handle_hcep_out(acthcd,	4);
			break;
		case UIV_EP4OUT:
			usb_writeb(1<<4, acthcd->base +	HCIN07IRQ);	/*clear	hcep4in	irq	*/
			handle_hcep_in(acthcd, 4);
			break;
		case UIV_EP5IN:
			usb_writeb(1<<5, acthcd->base +	HCOUT07IRQ);	/*clear	hcep5out irq */
			handle_hcep_out(acthcd,	5);
			break;
		case UIV_EP5OUT:
			usb_writeb(1<<5, acthcd->base +	HCIN07IRQ);	/*clear	hcep5in	irq	*/
			handle_hcep_in(acthcd, 5);
			break;
		case UIV_HCOUT0ERR:
		case UIV_HCIN0ERR:
		case UIV_HCOUT1ERR:
		case UIV_HCIN1ERR:
		case UIV_HCOUT2ERR:
		case UIV_HCIN2ERR:
		case UIV_HCOUT3ERR:
		case UIV_HCIN3ERR:
		case UIV_HCOUT4ERR:
		case UIV_HCIN4ERR:
		case UIV_HCOUT5ERR:
		case UIV_HCIN5ERR:
//			ACT_HCD_DBG
			aotg_hcd_error(acthcd, irqvector);
			break;
		default:
			dev_err(acthcd->dev, "error	interrupt, pls check it! irqvector:	0x%02X\n", (u8)irqvector);
			//spin_unlock(&acthcd->lock);
			return IRQ_NONE;
		}
	}

	if (acthcd->use_dma) {
		aotg_hcd_handle_dma_pend(acthcd);
	}
	//spin_unlock(&acthcd->lock);
	return IRQ_HANDLED;
}

static void	aotg_hcd_hotplug_timer(unsigned	long data)
{
	struct aotg_hcd	*acthcd	= (struct aotg_hcd *)data;
	struct usb_hcd *hcd	= aotg_to_hcd(acthcd);
	struct platform_device *pdev;
	unsigned int port_no;
	unsigned long flags;
	int	connect_changed	= 0;

	//if ((void	*)data == (void	*)NULL)	
	if (unlikely(IS_ERR_OR_NULL((void *)data)))	{
		ACT_HCD_DBG
		return;
	}
	if (acthcd->hcd_exiting	!= 0) {
		ACT_HCD_DBG
		return;
	}

	//disable_irq_nosync(acthcd->uhc_irq);
	disable_irq(acthcd->uhc_irq);
	spin_lock_irqsave(&acthcd->lock, flags);

	if (acthcd->put_aout_msg !=	0) {
		//tmp =	((pdev->id & 0xff) << 16) |	HOST;
		pdev = to_platform_device(hcd->self.controller);
		port_no	= pdev->id & 0xff;
		ACT_HCD_ERR
		//update_driver_state(UPDATE_UDEVICE_OUT, port_no);
		acthcd->put_aout_msg = 0;
		spin_unlock_irqrestore(&acthcd->lock, flags);
		enable_irq(acthcd->uhc_irq);
		return;
	}
	
	if (usb_readb(acthcd->base + OTGSTATE) == AOTG_STATE_A_HOST) {
		if (!acthcd->inserted) {
			acthcd->port |=	(USB_PORT_STAT_C_CONNECTION	<< 16);
			/*set port status bit,and indicate the present of  a device	*/
			acthcd->port |=	USB_PORT_STAT_CONNECTION;
			acthcd->rhstate	= AOTG_RH_ATTACHED;
			acthcd->inserted = 1;
			connect_changed	= 1;
		}
	} else {
		if (acthcd->inserted) {
			acthcd->port &=	~(USB_PORT_STAT_CONNECTION |
					  USB_PORT_STAT_ENABLE |
					  USB_PORT_STAT_LOW_SPEED |
					  USB_PORT_STAT_HIGH_SPEED | USB_PORT_STAT_SUSPEND);
			acthcd->port |=	(USB_PORT_STAT_C_CONNECTION	<< 16);
			acthcd->rhstate	= AOTG_RH_NOATTACHED;
			acthcd->inserted = 0;
			connect_changed	= 1;
		}
	}

	dev_info(acthcd->dev, "<USB> %s	connection changed:	%d,	acthcd->inserted: %d\n", 
			dev_name(hcd->self.controller),	connect_changed, acthcd->inserted);
	if (connect_changed) {
		if (HC_IS_SUSPENDED(hcd->state)) {
			usb_hcd_resume_root_hub(hcd);
		}
		ACT_HCD_DBG
		usb_hcd_poll_rh_status(hcd);
	}

	if ((acthcd->inserted == 0)	&& (connect_changed	== 1)) {
		acthcd->put_aout_msg = 1;
		mod_timer(&acthcd->hotplug_timer, jiffies +	msecs_to_jiffies(6000));
	}
	acthcd->suspend_request_pend = 0;

	spin_unlock_irqrestore(&acthcd->lock, flags);
	enable_irq(acthcd->uhc_irq);
	return;
}

static void	aotg_hcd_trans_wait_timer(unsigned long	data)
{
	unsigned long flags;
	struct aotg_hcd	*acthcd	= (struct aotg_hcd *)data;

	if (unlikely(IS_ERR_OR_NULL((void *)data)))	{
		ACT_HCD_DBG
		return;
	}
	if (acthcd->hcd_exiting	!= 0) {
		ACT_HCD_DBG
		return;
	}

	//disable_irq_nosync(acthcd->uhc_irq);
	disable_irq(acthcd->uhc_irq);
	spin_lock_irqsave(&acthcd->lock, flags);

	if ((acthcd->timer_bulkout_state ==	AOTG_BULKOUT_DMA_WAIT) || 
		(acthcd->timer_bulkout_state ==	AOTG_BULKOUT_DMA_TIMEOUT)) {
		acthcd->timer_bulkout_state	= AOTG_BULKOUT_DMA_TIMEOUT;
		acthcd->bulkout_ep_busy_cnt++;
		if ((acthcd->bulkout_ep_busy_cnt >= 19) && \
			(acthcd->bulkout_ep_busy_cnt % 4 == 0)) {
			printk("%s,%d,timeout jiffies:%x,acthcd->bulkout_ep_busy_cnt = %d\n",__FUNCTION__,__LINE__,	(unsigned \
			int)jiffies,acthcd->bulkout_ep_busy_cnt);
#ifdef DEBUG_INFO
			aotg_hcd_show_ep_info(acthcd);
			aotg_dbg_regs(acthcd);
#endif
		}
		mod_timer(&acthcd->trans_wait_timer, jiffies + msecs_to_jiffies(10 + acthcd->bulkout_ep_busy_cnt * 2));

		//printk("setup:%d,	dma1:%d\n",	acthcd->setup_processing, acthcd->dma_working[1]);
		//usb_writeb(0x1<<6	| 0x1<<5, acthcd->base + HCPORTCTRL);	/*portrst &	55ms */
		//aotg_dbg_output_info();
		//aotg_dbg_proc_output_ep();
		//BUG();
		//printk("num:%d\n", usb_pipeendpoint(urb->pipe));
	}

	if (acthcd->timer_bulkout_state	== AOTG_BULKOUT_FIFO_BUSY)	{
		WARN_ON(acthcd->bulkout_wait_ep	== NULL);
#ifdef MULTI_OUT_FIFO
		if ((EPCS_BUSY & usb_readb(acthcd->bulkout_wait_ep->reg_hcepcs)) == 0 && \
		   (EPCS_NPAK & usb_readb(acthcd->bulkout_wait_ep->reg_hcepcs)) == (0x01 << 2)) {
#else
		if ((EPCS_BUSY & usb_readb(acthcd->bulkout_wait_ep->reg_hcepcs)) ==	0 )	{
#endif
			//acthcd->dma_working[1] = 0;
			acthcd->bulkout_wait_ep	= NULL;
			acthcd->bulkout_ep_busy_cnt	= 0;
			acthcd->timer_bulkout_state	= AOTG_BULKOUT_NULL;
			//printk("fifo busy	end!\n");
		} else {
			acthcd->bulkout_ep_busy_cnt++;
			if (acthcd->bulkout_ep_busy_cnt	> 10) {
				if ((acthcd->bulkout_ep_busy_cnt % 4) == 0)	{
					printk("%s,%d,usb fifo busy:%d\n",__FUNCTION__,__LINE__, acthcd->bulkout_ep_busy_cnt);
				}
				if (acthcd->bulkout_ep_busy_cnt	> 60) {
//					ACT_HCD_DBG
					aotg_hcep_reset(acthcd,	acthcd->bulkout_wait_ep->mask, ENDPRST_FIFORST);
				}
#if	0
				aotg_dbg_proc_output_ep_state(acthcd);
				printk("\n");
				printk("bulkout_ep_busy_cnt:%d\n", acthcd->bulkout_ep_busy_cnt);
#endif
			}
			mod_timer(&acthcd->trans_wait_timer, jiffies + msecs_to_jiffies(10 + acthcd->bulkout_ep_busy_cnt * 2));
		}
	}

	spin_unlock_irqrestore(&acthcd->lock, flags);
	enable_irq(acthcd->uhc_irq);
	tasklet_hi_schedule(&acthcd->urb_tasklet);
	return;
}

static inline int start_transfer(struct	aotg_hcd *acthcd, struct aotg_queue	*q,	struct aotg_hcep *ep)
{
	struct urb *urb	= q->urb;
	int	retval = 0;

	ep->urb_enque_cnt++;
	list_add_tail(&q->ep_q_list, &ep->q_list);

	switch (usb_pipetype(urb->pipe)) {
	case PIPE_CONTROL:
		retval = handle_setup_packet(acthcd, q);
		break;

	case PIPE_BULK:
		aotg_hcep_reset(acthcd,	ep->mask, ENDPRST_FIFORST);

		if (usb_pipeout(urb->pipe))	{
			handle_bulkout_packet(acthcd, q);
		} else {
			handle_bulkin_packet(acthcd, q);
		}
		break;
	
	case PIPE_INTERRUPT:
		retval = handle_intr_packet(acthcd,	q);
		break;
	
	case PIPE_ISOCHRONOUS:
		/* fall	through	*/
	default:
		retval = -ENODEV;
		break;
	}

	return retval;
}

static void	perioidc_work_func(struct work_struct *w)
{
	struct delayed_work	*dw	= to_delayed_work(w);
	struct aotg_hcep *ep = container_of(dw,	struct aotg_hcep, dwork);
	struct aotg_hcd	*acthcd	= ep->dev;
	struct aotg_queue *q = NULL;
	struct urb *urb	= NULL;
	int	pipe;
	unsigned long flags;
	u16	w_packets;

	spin_lock_irqsave(&acthcd->lock, flags);
	if (!list_empty(&ep->q_list)) {
		q =	list_first_entry(&ep->q_list, struct aotg_queue, ep_q_list);
		urb	= q->urb;
		pipe = urb->pipe;

		if (usb_pipeint(pipe) && usb_pipein(pipe)) {
			w_packets =	urb->transfer_buffer_length	/ MAX_PACKET(ep->maxpacket);
			if (urb->transfer_buffer_length	& (MAX_PACKET(ep->maxpacket) - 1)) {
				w_packets++;
			}
			//reset	fifo, skip the undo	data
			ep->is_use_pio = 1;
			aotg_hcep_reset(acthcd,	ep->mask, ENDPRST_FIFORST);
			ep_enable(ep);
			pio_irq_clear(acthcd, ep->mask);
			pio_irq_enable(acthcd, ep->mask);
			__hc_in_start(acthcd, ep, w_packets);
		} else {				// never happed, enqueue filter	it
			dev_err(acthcd->dev, "<Intr>xfers wrong	type, ignore it, %s()\n", __func__);
		}

		queue_delayed_work(acthcd->periodic_wq,	dw,	msecs_to_jiffies(ep->period));
	}
	spin_unlock_irqrestore(&acthcd->lock, flags);
	return;
}

static struct aotg_hcep	*aotg_hcep_alloc(struct	aotg_hcd *acthcd, struct urb *urb)
{
	struct aotg_hcep *ep = NULL;
	int	pipe = urb->pipe;
	int	is_out = usb_pipeout(pipe);
	int	type = usb_pipetype(pipe);
	int	retval = 0;

	ep = kzalloc(sizeof	*ep, GFP_ATOMIC);
	if (NULL ==	ep)	{
		dev_err(acthcd->dev, "alloc	ep failed\n");
		retval = -ENOMEM;
		goto exit;
	}

	ep->hep	= urb->ep;
	ep->udev = usb_get_dev(urb->dev);
	ep->dev	= acthcd;
	INIT_LIST_HEAD(&ep->q_list);
	ep->maxpacket =	usb_maxpacket(ep->udev,	pipe, is_out);
	ep->epnum =	usb_pipeendpoint(pipe);
	ep->length = 0;
	ep->type = type;
	ep->urb_enque_cnt =	0;
	ep->urb_endque_cnt = 0;
	ep->urb_stop_stran_cnt = 0;
	ep->urb_unlinked_cnt = 0;
	dev_info(acthcd->dev, "ep->epnum: %d, ep->maxpacket	: %d, ep->type : %d\n",	ep->epnum, ep->maxpacket, ep->type);
	ep->bulkout_zero_cnt = 0;
	usb_settoggle(urb->dev,	usb_pipeendpoint(urb->pipe), is_out, 0);

	switch (type) {
	case PIPE_CONTROL:
		acthcd->ep0	= ep;
		ep->index =	0;
		ep->mask = 0;
		usb_writeb(usb_pipedevice(urb->pipe), acthcd->base + FNADDR);
		dev_info(acthcd->dev, "device addr : 0x%08x\n",	usb_readb(acthcd->base + FNADDR));
		usb_writeb(ep->epnum, acthcd->base + HCEP0CTRL);
		usb_writeb((u8)ep->maxpacket, acthcd->base + HCIN0MAXPCK);
		usb_writeb((u8)ep->maxpacket, acthcd->base + HCOUT0MAXPCK);

		usb_setbitsb(1,	acthcd->base + HCOUT07IEN);
		usb_setbitsb(1,	acthcd->base + HCIN07IEN);
		usb_writeb(1, acthcd->base + HCOUT07IRQ);
		usb_writeb(1, acthcd->base + HCIN07IRQ);
		//usb_setbitsb(1, acthcd->base + HCIN07ERRIEN);
		//usb_setbitsb(1, acthcd->base + HCOUT07ERRIEN);
		usb_settoggle(urb->dev,	usb_pipeendpoint(urb->pipe), 1,	0);
		usb_settoggle(urb->dev,	usb_pipeendpoint(urb->pipe), 0,	0);
		acthcd->setup_processing = 0;
		break;

	case PIPE_BULK:
		if (is_out)
#ifdef MULTI_OUT_FIFO
			retval = aotg_hcep_config(acthcd, ep, EPCON_TYPE_BULK, EPCON_BUF_DOUBLE, is_out);
#else
			retval = aotg_hcep_config(acthcd, ep, EPCON_TYPE_BULK, EPCON_BUF_SINGLE, is_out);
#endif
		else
			//retval = aotg_hcep_config(acthcd,	ep,	EPCON_TYPE_BULK, EPCON_BUF_DOUBLE, is_out);
			retval = aotg_hcep_config(acthcd, ep, EPCON_TYPE_BULK, EPCON_BUF_SINGLE, is_out);

		if (retval < 0)	{
			dev_err(acthcd->dev, "PIPE_BULK, retval: %d\n",	retval);
			goto free_ep;
		}
		break;

	case PIPE_INTERRUPT:
		retval = aotg_hcep_config(acthcd, ep, EPCON_TYPE_INT, EPCON_BUF_SINGLE,	is_out);
		if (retval < 0)	{
			dev_err(acthcd->dev, "PIPE_INTERRUPT, retval: %d\n", retval);
			goto free_ep;
		}
		INIT_DELAYED_WORK(&ep->dwork, perioidc_work_func);
		ep->load = NS_TO_US(usb_calc_bus_time(ep->udev->speed, !is_out,	(type == PIPE_ISOCHRONOUS),	ep->maxpacket));

		switch (ep->udev->speed) {
		case USB_SPEED_HIGH:
			ep->period = urb->interval >> 3;
			break;
		case USB_SPEED_FULL:
			ep->period = urb->interval;
			usb_clearbitsb(0x1 << ep->index, acthcd->base +	HCIN07ERRIEN); // tell me why?
			break;
		case USB_SPEED_LOW:
			ep->period = urb->interval;
			usb_clearbitsb(0x1 << ep->index, acthcd->base +	HCIN07ERRIEN); // tell me why?
			break;
		default:
			dev_err(acthcd->dev, "error	speed, speed: %d, %s, %d\n", ep->udev->speed, __func__,	__LINE__);
			goto free_ep;
		}

		if (ep->period == 0) {
			ep->period = 1;
		}

		printk("urb->interval: %d\n", urb->interval);
		break;

	default:
		dev_err(acthcd->dev, "not support type,	type: %d\n", type);
		goto free_ep;
	}

	return ep;

free_ep:
	usb_put_dev(urb->dev);
	kfree(ep);
exit:
	return NULL;
}

static int aotg_hcd_urb_enqueue(struct usb_hcd *hcd, struct	urb	*urb, unsigned mem_flags)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	struct aotg_hcep *ep = NULL;
	struct aotg_queue *q;
	unsigned long flags;
	int	pipe = urb->pipe;
	int	type = usb_pipetype(pipe);
	//int is_out = !usb_pipein(pipe);
	//int epnum	= usb_pipeendpoint(pipe);
	int	retval = 0;

	if (type ==	PIPE_ISOCHRONOUS) {
		dev_err(acthcd->dev, "<QUEUE> not support type:	%d\n", type);
		return -ENOSPC;
	}
	if (acthcd->hcd_exiting	!= 0) {
		dev_err(acthcd->dev, "aotg hcd exiting!	type:%d\n",	type);
		return -EIO;
	}

	/*
	 * because urb_enqueue may be called via irq handler when excute complete(), 
	 * so we should	avoid call disable_irq in irq context. 
	 */
	if (in_irq()) {
		disable_irq_nosync(acthcd->uhc_irq);
	} else {
		disable_irq(acthcd->uhc_irq);
	}
	spin_lock_irqsave(&acthcd->lock, flags);

	q =	aotg_hcd_get_queue(acthcd, mem_flags);
	if (unlikely(!q)) {
		dev_err(acthcd->dev, "<QUEUE>  alloc dma queue failed\n");
		spin_unlock_irqrestore(&acthcd->lock, flags);
		enable_irq(acthcd->uhc_irq);
		return -ENOMEM;
	}

	if (urb->num_sgs > 0) {
		q->cur_sg =	urb->sg;
	} else {
		q->cur_sg =	NULL;
	} 

	if (!(acthcd->port & USB_PORT_STAT_ENABLE)
			|| !HC_IS_RUNNING(hcd->state)) {
		dev_err(acthcd->dev, "<QUEUE> port is dead or disable\n");
		retval = -ENODEV;
		goto exit;
	}

	if (likely(urb->ep->hcpriv)) {
		ep = (struct aotg_hcep *)urb->ep->hcpriv;
	} else {
		ep = aotg_hcep_alloc(acthcd, urb);
		if (NULL ==	ep)	{
			dev_err(acthcd->dev, "<QUEUE> alloc	ep failed\n");
			retval = -ENOMEM;
			goto exit;
		}
		urb->ep->hcpriv	= ep;
	}

	urb->hcpriv	= hcd;
	q->ep =	ep;
	q->urb = urb;

	if ((usb_pipeout(urb->pipe)) &&	
		(urb->transfer_buffer_length % ep->maxpacket ==	0))	{
		q->need_zero = urb->transfer_flags & URB_ZERO_PACKET;
	}

	retval = usb_hcd_link_urb_to_ep(hcd, urb);
	if (retval)	{
		dev_err(acthcd->dev, "<QUEUE>  usb_hcd_link_urb_to_ep error: %d	!\n", retval);
		goto exit;
	}
	
	list_add_tail(&q->enqueue_list,	&acthcd->hcd_enqueue_list);
	spin_unlock_irqrestore(&acthcd->lock, flags);
	enable_irq(acthcd->uhc_irq);
	tasklet_hi_schedule(&acthcd->urb_tasklet);
	return retval;
exit:
	if (unlikely(retval	< 0)) {
		ACT_HCD_ERR
		if (!list_empty(&q->ep_q_list))
			list_del(&q->ep_q_list);
		if (!list_empty(&q->dma_q_list))
			list_del_init(&q->dma_q_list);
		aotg_hcd_release_queue(acthcd, q);
	}
	spin_unlock_irqrestore(&acthcd->lock, flags);
	enable_irq(acthcd->uhc_irq);
	return retval;
}

void urb_tasklet_func(unsigned long	data)
{
	struct aotg_hcd	*acthcd	= (struct aotg_hcd *)data;
	struct aotg_queue *q, *next;
	struct aotg_hcep *ep;
	struct urb *urb;
	struct usb_hcd *hcd;
	unsigned long flags;
	int	status;
	int	cnt	= 0;
	//static struct	aotg_hcep *	hcin_ep	= NULL;

	//spin_lock(&acthcd->tasklet_lock);

	do {
		status = (int)spin_is_locked(&acthcd->tasklet_lock);
		if (status)	{
			acthcd->tasklet_retry =	1;
			printk("locked,	urb	retry later!\n");
			return;
		}
		cnt++;
		/* sometimes tasklet_lock is unlocked, but spin_trylock	still will be failed, 
		 * maybe caused	by the instruction of strexeq in spin_trylock, it will return failed   
		 * if other	cpu	is accessing the nearby	address	of &acthcd->tasklet_lock.
		 */
		status = spin_trylock(&acthcd->tasklet_lock);
		if ((!status) && (cnt >	10))  {
			acthcd->tasklet_retry =	1;
			printk("urb	retry later!\n");
			return;
		}
	} while	(status	== 0);

	//disable_irq_nosync(acthcd->uhc_irq);
	disable_irq(acthcd->uhc_irq);
	spin_lock_irqsave(&acthcd->lock, flags);

	/* do dequeue task.	*/
DO_DEQUEUE_TASK:
	urb	= NULL;
	list_for_each_entry_safe(q,	next, &acthcd->hcd_dequeue_list, dequeue_list) {
		if (q->status <	0) {
			urb	= q->urb;
			ep = q->ep;
			if (ep)
				ep->urb_unlinked_cnt++;
			list_del(&q->dequeue_list);
			status = q->status;
			aotg_hcd_release_queue(acthcd, q);
			hcd	= bus_to_hcd(urb->dev->bus);
			break;
		} else {
			ACT_HCD_ERR
		}
	}
	if (urb	!= NULL) {
		usb_hcd_unlink_urb_from_ep(hcd,	urb);

		spin_unlock_irqrestore(&acthcd->lock, flags);

		/* in usb_hcd_giveback_urb,	complete function may call new urb_enqueue.	*/ 
		usb_hcd_giveback_urb(hcd, urb, status);

		spin_lock_irqsave(&acthcd->lock, flags);
		goto DO_DEQUEUE_TASK;
	}

	/* do finished task. */
DO_FINISH_TASK:
	urb	= NULL;
	list_for_each_entry_safe(q,	next, &acthcd->hcd_finished_list, finished_list) {
		list_del(&q->finished_list);
		status = q->status;
		tasklet_finish_request(acthcd, q, status);

		hcd	= aotg_to_hcd(acthcd);
		urb	= q->urb;
		ep = q->ep;
		if (urb	!= NULL) {
			break;
		}
	}
	if (urb	!= NULL) {
		usb_hcd_unlink_urb_from_ep(hcd,	urb);

		spin_unlock_irqrestore(&acthcd->lock, flags);

		/* in usb_hcd_giveback_urb,	complete function may call new urb_enqueue.	*/ 
		usb_hcd_giveback_urb(hcd, urb, status);

		spin_lock_irqsave(&acthcd->lock, flags);
		goto DO_FINISH_TASK;
	}

//DO_ENQUEUE_TASK:
	/* do enqueue task.	*/
	/* start transfer directly,	don't care setup appearing in bulkout. */
	list_for_each_entry_safe(q,	next, &acthcd->hcd_enqueue_list, enqueue_list) {
		urb	= q->urb;
		ep = q->ep;

		/* deal	with controll request. */
		if (usb_pipetype(urb->pipe)	== PIPE_CONTROL) {
			if (acthcd->setup_processing !=	0) {
				ACT_HCD_DBG
				continue;
			} else {
				goto BEGIN_START_TANSFER;
			}
		} 

		/* deal	with new bulk in request. */
		if ((usb_pipetype(urb->pipe) ==	PIPE_BULK) && (usb_pipein(urb->pipe))) {
			if (acthcd->dma_working[0] != 0) {
				if (list_empty(&ep->q_list)) {
					goto BEGIN_START_TANSFER;
				}
				continue;
			}
			
			if (!list_empty(&ep->q_list)){
				ACT_HCD_DBG
				continue;
			}

			/* check ep	in,	to avoid hardware bug. if the last bulk	in packet is zero, 
			 * it shouldn't	start until	a bulk out transfer	is finished, and usb dma reseted.  
			 */
			//hcin_ep =	ep;
			if (((acthcd->last_bulkin_dma_len %	ep->maxpacket) == 0) &&
				(acthcd->dma_working[1]	!= 0)) {
				if (ep->maxpacket != 512) {
					printk("ep->maxpacket:%d\n", ep->maxpacket);
					ACT_HCD_ERR
				}
				continue;
			}
			goto BEGIN_START_TANSFER;
		}

		/* deal	with bulk out request. */
		if ((usb_pipetype(urb->pipe) ==	PIPE_BULK) && (usb_pipeout(urb->pipe)))	{
			if (acthcd->dma_working[1] != 0) {
				continue;
			}
			if (acthcd->bulkout_wait_ep	!= NULL) {
				continue;
			}
#ifdef MULTI_OUT_FIFO
			if ((EPCS_BUSY & usb_readb(ep->reg_hcepcs)) != 0	|| \
				   	(EPCS_NPAK & usb_readb(ep->reg_hcepcs)) != (0x01 << 2))	{
#else
			if ((EPCS_BUSY & usb_readb(ep->reg_hcepcs)) != 0) {
#endif
				acthcd->bulkout_wait_ep	= ep;
				acthcd->bulkout_ep_busy_cnt	= 0;
				acthcd->timer_bulkout_state	= AOTG_BULKOUT_FIFO_BUSY;
				mod_timer(&acthcd->trans_wait_timer, jiffies + msecs_to_jiffies(3));
//ACT_HCD_DBG
				continue;
			}
#define	SUPPORT_RTL_USB_WIFI	0
#if	SUPPORT_RTL_USB_WIFI	/* test	send zero packet to	device.	*/
			usb_writew(0, ep->reg_hcepbc);
			usb_writeb(0, ep->reg_hcepcs);
			status = 10;
			while (((EPCS_BUSY & usb_readb(ep->reg_hcepcs))	!= 0) && status) {
				status--;
				udelay(1);
			}
			//if (status ==	0) 
				//printk("bulkout busy,	status:%d\n", status);
#endif

			acthcd->bulkout_ep_busy_cnt	= 0;
			acthcd->timer_bulkout_state	= AOTG_BULKOUT_DMA_WAIT;
			mod_timer(&acthcd->trans_wait_timer, jiffies + msecs_to_jiffies(60));

			goto BEGIN_START_TANSFER;
		}

BEGIN_START_TANSFER:
		list_del(&q->enqueue_list);
		aotg_dbg_put_q(q, usb_pipeendpoint(q->urb->pipe), 
			usb_pipein(q->urb->pipe), q->urb->transfer_buffer_length);

		status = start_transfer(acthcd,	q, ep);

		if (unlikely(status	< 0)) {
			ACT_HCD_ERR
			hcd	= bus_to_hcd(urb->dev->bus);
			aotg_hcd_release_queue(acthcd, q);

			usb_hcd_unlink_urb_from_ep(hcd,	urb);
			spin_unlock_irqrestore(&acthcd->lock, flags);
			usb_hcd_giveback_urb(hcd, urb, status);
			spin_lock_irqsave(&acthcd->lock, flags);
		}
		//spin_unlock_irqrestore(&acthcd->lock,	flags);
		//enable_irq(acthcd->uhc_irq);
		//spin_unlock(&acthcd->tasklet_lock);
		//return;
	}

	if (acthcd->tasklet_retry != 0)	{
		acthcd->tasklet_retry =	0;
		goto DO_DEQUEUE_TASK;
	}
	spin_unlock_irqrestore(&acthcd->lock, flags);
	enable_irq(acthcd->uhc_irq);
	spin_unlock(&acthcd->tasklet_lock);
	return;
}

static int aotg_hcd_urb_dequeue(struct usb_hcd *hcd, struct	urb	*urb, int status)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	struct aotg_hcep *ep;
	struct aotg_queue *q = NULL, *next,	*tmp;
	unsigned long flags;
	int	retval = 0;

	if (in_irq()) {
		disable_irq_nosync(acthcd->uhc_irq);
	} else {
		disable_irq(acthcd->uhc_irq);
	}
	spin_lock_irqsave(&acthcd->lock, flags);

	ep = (struct aotg_hcep *)urb->ep->hcpriv;
	printk("%s,%d,ep->index: %d,[%s]\n",\ 
	__FUNCTION__,__LINE__,ep->index,usb_pipein(urb->pipe ) ? "in":"out"	);

	retval = usb_hcd_check_unlink_urb(hcd, urb,	status);
	if (retval)	{
		dev_err(acthcd->dev, "can't	unlink urb to ep, retval: %d\n", retval);
		goto dequeue_fail;
	}

	retval = -EINVAL;

	if (ep && !list_empty(&ep->q_list))	{
		retval = 0;
		//dev_info(acthcd->dev,	
			//"current dequeue -- ep->index: %d, dir : %s, type: %d, transfer_buffer_length: %d, actual_length:%d\n",
			//ep->index, usb_pipeout(urb->pipe)?"out":"in",	usb_pipetype(urb->pipe), 
			//urb->transfer_buffer_length, urb->actual_length);

		list_for_each_entry_safe(tmp, next,	&ep->q_list, ep_q_list)	{
			if (tmp->urb ==	urb) {
				if (!list_empty(&tmp->dma_q_list))
#ifdef DEBUG_INFO
					ACT_HCD_DBG
#endif
					list_del_init(&tmp->dma_q_list);

				/* maybe finished in tasklet_finish_request. */
				if (!list_empty(&tmp->finished_list)) {
					ACT_HCD_DBG
					if (tmp->finished_list.next	!= LIST_POISON1) {
#ifdef DEBUG_INFO
						ACT_HCD_DBG
#endif
						list_del(&tmp->finished_list);
					}
				}
				if (tmp->ep_q_list.next	== LIST_POISON1) {
#ifdef DEBUG_INFO
					ACT_HCD_DBG
#endif
					goto dequeue_fail;
				}
				list_del(&tmp->ep_q_list);
				q =	tmp;
				break;
			}
		}
		
		if (likely(q)) {
			if (q->is_xfer_start ||	q->is_start_dma) {
				ep->urb_stop_stran_cnt++;

				if (q->is_xfer_start) {
//					ACT_HCD_DBG
					if (ep->index >	0) {
						if (usb_pipein(urb->pipe)) {
//							ACT_HCD_DBG
							__hc_in_stop(acthcd, ep);
							ep_disable(ep);
						}
						aotg_hcep_reset(acthcd,	ep->mask, ENDPRST_FIFORST);
						pio_irq_clear(acthcd, ep->mask);
					}
				}
				if (q->is_start_dma) {
#ifdef DEBUG_INFO
					ACT_HCD_DBG
					aotg_hcd_show_ep_info(acthcd);
					aotg_dbg_regs(acthcd);
#endif
					__clear_dma(acthcd,	q, q->is_start_dma-1);
				}
				if (usb_pipeint(q->urb->pipe) && q->is_xfer_start) {
					acthcd->sof_kref--;
					hcd->self.bandwidth_int_reqs--;
					if (acthcd->sof_kref <=	0) {
						aotg_sofirq_off(acthcd);
					}
				}

				if (usb_pipetype(urb->pipe)	== PIPE_CONTROL) {
					acthcd->setup_processing = 0;
				}
				q->is_xfer_start = 0;
				/* processed in	__clear_dma. */
				//q->is_start_dma =	0;
				//dma[0] = 0; dma[1] = 0;
			}
			goto dequeued;
		}
	}

	list_for_each_entry_safe(tmp, next,	&acthcd->hcd_enqueue_list, enqueue_list) {
		if (tmp->urb ==	urb) {
			list_del(&tmp->enqueue_list);
			q =	tmp;
//						ACT_HCD_DBG
			/* there maybe some	err	if	setup_processing !=	0. */
			if (usb_pipetype(urb->pipe)	== PIPE_CONTROL) {
				if (acthcd->setup_processing !=	0) {
					ACT_HCD_DBG
					acthcd->setup_processing = 0;
				}
			}
			break;
		}
	}
dequeued:
	if (likely(q)) {
		q->status =	status;
		list_add_tail(&q->dequeue_list,	&acthcd->hcd_dequeue_list);
		spin_unlock_irqrestore(&acthcd->lock, flags);
		enable_irq(acthcd->uhc_irq);
		tasklet_schedule(&acthcd->urb_tasklet);
		return retval;
	} 

dequeue_fail:
	ACT_HCD_ERR
	spin_unlock_irqrestore(&acthcd->lock, flags);
	enable_irq(acthcd->uhc_irq);
	return retval;
}

static void	aotg_hcd_endpoint_disable(struct usb_hcd *hcd, struct usb_host_endpoint	*hep)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	struct aotg_hcep *ep = hep->hcpriv;
	unsigned long flags;
	int	index;
	int	i;
	
	if (!ep)
		return;

	/* assume we'd just	wait for the irq */
	for	(i = 0;	i <	100	&& !list_empty(&hep->urb_list);	i++)
		msleep(3);
	
	/*
	* usually, before disable ep, all urb are dequeued.
	* otherwise, there is something	wrong in upper driver
	*/
	if (!list_empty(&ep->q_list)) {
		dev_warn(acthcd->dev, "<ep disable>	error: ep%d's urb list not empty\n", ep->index);
	}

	if (ep->type ==	PIPE_INTERRUPT)	{
		cancel_delayed_work_sync(&ep->dwork);
		hcd->self.bandwidth_allocated -= ep->load /	ep->period;
	}

	local_irq_save(flags);

	usb_put_dev(ep->udev);
	index =	ep->index;
	if (index == 0)	{
		acthcd->ep0	= NULL;
		acthcd->setup_processing = 0;
		usb_clearbitsb(1, acthcd->base + HCOUT07IEN);
		usb_clearbitsb(1, acthcd->base + HCIN07IEN);
		usb_writeb(1, acthcd->base + HCOUT07IRQ);
		usb_writeb(1, acthcd->base + HCIN07IRQ);
	}
	else {
		if (ep->mask & USB_HCD_OUT_MASK) {
			acthcd->outep[index] = NULL;
		}
		else {
			acthcd->inep[index]	= NULL;
			ep_disable(ep);
			__hc_in_stop(acthcd, ep);
			//hcin_spkt_irq_disable(acthcd,	ep);
		}
		pio_irq_disable(acthcd,	ep->mask);
		pio_irq_clear(acthcd, ep->mask);
		release_fifo_addr(acthcd, ep->fifo_addr);
		//acthcd->dma_working[dmanr	& 0x1] = urb->pipe;
	}
	
	hep->hcpriv	= NULL;
	dev_info(acthcd->dev, "<EP DISABLE>	ep%d index %d from ep [%s]\n", 
			ep->epnum, index, 
			ep->mask & USB_HCD_OUT_MASK	? "out":"in");
	local_irq_restore(flags);
	kfree(ep);
}

static int aotg_hcd_get_frame(struct usb_hcd *hcd)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	int	frame =	0;

	if (acthcd->speed == USB_SPEED_UNKNOWN)	{
		frame =	0;
	} else if (acthcd->speed ==	USB_SPEED_HIGH)	{
		frame =	(int) (usb_readw(acthcd->base +	HCFRMNRL) >> 3);
	} else {
		frame =	(int) usb_readw(acthcd->base + HCFRMNRL);
	}

	return frame;
}

static int aotg_hub_status_data(struct usb_hcd *hcd, char *buf)
{
	struct aotg_hcd	*acthcd;
	unsigned long flags;
	int	retval = 0;

	acthcd = hcd_to_aotg(hcd);
	local_irq_save(flags);
	if (!HC_IS_RUNNING(hcd->state))
		goto done;

	if ((acthcd->port &	AOTG_PORT_C_MASK) != 0)	{
		*buf = (1 << 1);
		HUB_DEBUG("<HUB	STATUS>port	status %08x	has	changes\n",	acthcd->port);
		retval = 1;
	}
done:
	local_irq_restore(flags);
	return retval;
}

static __inline__ void port_reset(struct aotg_hcd *acthcd)
{
	HCD_DEBUG("<USB> port reset\n");
	usb_writeb(0x1<<6 |	0x1<<5,	acthcd->base + HCPORTCTRL);	/*portrst &	55ms */
}

static void	port_power(struct aotg_hcd *acthcd,	int	is_on)
{
	struct usb_hcd *hcd	= aotg_to_hcd(acthcd);
	//struct device	*dev = hcd->self.controller;

	/* hub is inactive unless the port is powered */
	if (is_on) {
		hcd->self.controller->power.power_state	= PMSG_ON;
		dev_dbg(acthcd->dev, "<USB>	power on\n");
	} else {
		hcd->self.controller->power.power_state	= PMSG_SUSPEND;
		dev_dbg(acthcd->dev, "<USB>	power off\n");
	}
}

static void	port_suspend(struct	aotg_hcd *acthcd)
{
	usb_clearbitsb(OTGCTRL_BUSREQ, acthcd->base	+ OTGCTRL);
}

static void	port_resume(struct aotg_hcd	*acthcd)
{
	usb_setbitsb(OTGCTRL_BUSREQ, acthcd->base +	OTGCTRL);
}

static int aotg_hcd_start(struct usb_hcd *hcd)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	//struct device	*dev = hcd->self.controller;
	//struct aotg_plat_data	*data =	dev->platform_data;
	int	retval = 0;
	unsigned long flags;
	
	dev_info(acthcd->dev, "<HCD> start\n");
	
	local_irq_save(flags);
	hcd->state = HC_STATE_RUNNING;
	hcd->uses_new_polling =	1;
	local_irq_restore(flags);
	
	return retval;

}

static void	aotg_hcd_stop(struct usb_hcd *hcd)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	//struct device	*dev = hcd->self.controller;
	//struct aotg_plat_data	*data =	dev->platform_data;
	unsigned long flags;
	
	dev_info(acthcd->dev, "<HCD> stop\n");
	
	local_irq_save(flags);
	hcd->state = HC_STATE_HALT;
	acthcd->port = 0;
	acthcd->rhstate	= AOTG_RH_POWEROFF;
	local_irq_restore(flags);
	return;
}

#ifdef	CONFIG_PM

static int aotg_hub_suspend(struct usb_hcd *hcd)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);

	if ((hcd ==	NULL) || (acthcd ==	NULL)) {
		ACT_HCD_ERR
		return 0;
	}
	acthcd->suspend_request_pend = 1;
	port_suspend(acthcd);
	
	return 0;
}

static int
aotg_hub_resume(struct usb_hcd *hcd)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	
	port_resume(acthcd);

	return 0;
}

#else

#define	aotg_hub_suspend	NULL
#define	aotg_hub_resume	NULL

#endif

static __inline__ void aotg_hub_descriptor(struct usb_hub_descriptor *desc)
{
	memset(desc, 0,	sizeof *desc);
	desc->bDescriptorType =	0x29;
	desc->bDescLength =	9;
	desc->wHubCharacteristics =	(__force __u16)
		(__constant_cpu_to_le16(0x0001));
	desc->bNbrPorts	= 1;
	//desc->bitmap[0] =	1 << 1;
	//desc->bitmap[1] =	0xff;
}

static int aotg_hub_control(struct usb_hcd *hcd,
				u16	typeReq,
				u16	wValue,	u16	wIndex,	char *buf, u16 wLength)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	unsigned long flags;
	int	retval = 0;

	if (in_irq()) {
		disable_irq_nosync(acthcd->uhc_irq);
	} else {
		disable_irq(acthcd->uhc_irq);
	}
	spin_lock_irqsave(&acthcd->lock, flags);

	if (!HC_IS_RUNNING(hcd->state))	{
		dev_err(acthcd->dev, "<HUB_CONTROL>	hc state is	not	HC_STATE_RUNNING\n");
		spin_unlock_irqrestore(&acthcd->lock, flags);
		enable_irq(acthcd->uhc_irq);
		return -EPERM;
	}

	//printk("<HUB_CONTROL>	typeReq:%x,	wValue:%04x, wIndex: %04x, wLength:	%04x\n", typeReq, wValue, wIndex, wLength);
	switch (typeReq) {
	case ClearHubFeature:
		HUB_DEBUG("<HUB_CONTROL> ClearHubFeature, wValue:%04x, wIndex: %04x, wLength: %04x\n",
			 wValue, wIndex, wLength);
		break;
	case ClearPortFeature:
		HUB_DEBUG("<HUB_CONTROL> ClearPortFeature, wValue:%04x,	wIndex:	%04x, wLength: %04x\n",
			 wValue, wIndex, wLength);

		if (wIndex != 1	|| wLength != 0)
			goto hub_error;
		HUB_DEBUG("<HUB_CONTROL> before	clear operation,the	port status	is %08x\n",	acthcd->port);
		switch (wValue)	{
		case USB_PORT_FEAT_ENABLE:
			acthcd->port &=	~(USB_PORT_STAT_ENABLE
					  |	USB_PORT_STAT_LOW_SPEED
					  |	USB_PORT_STAT_HIGH_SPEED);
			acthcd->rhstate	= AOTG_RH_DISABLE;
			if (acthcd->port & USB_PORT_STAT_POWER)
				port_power(acthcd, 0);
			break;
		case USB_PORT_FEAT_SUSPEND:
			HUB_DEBUG("<HUB_CONTROL>clear suspend feathure\n");
			//port_resume(acthcd);
			acthcd->port &=	~(1	<< wValue);
			break;
		case USB_PORT_FEAT_POWER:
			acthcd->port = 0;
			acthcd->rhstate	= AOTG_RH_POWEROFF;
			port_power(acthcd, 0);
			break;
		case USB_PORT_FEAT_C_ENABLE:
		case USB_PORT_FEAT_C_SUSPEND:
		case USB_PORT_FEAT_C_CONNECTION:
		case USB_PORT_FEAT_C_OVER_CURRENT:
		case USB_PORT_FEAT_C_RESET:
			acthcd->port &=	~(1	<< wValue);
			break;
		default:
			goto hub_error;
		}
		HUB_DEBUG("<HUB_CONTROL> after clear operation,the port	status is %08x\n", acthcd->port);
		break;
	case GetHubDescriptor:
		HUB_DEBUG("<HUB_CONTROL> GetHubDescriptor, wValue:%04x,	wIndex:	%04x, wLength: %04x\n",
			 wValue, wIndex, wLength);
		aotg_hub_descriptor((struct	usb_hub_descriptor *)buf);
		break;
	case GetHubStatus:
		HUB_DEBUG("<HUB_CONTROL> GetHubStatus, wValue:%04x,	wIndex:	%04x, wLength: %04x\n",
			 wValue, wIndex, wLength);

		*(__le32 *)	buf	= __constant_cpu_to_le32(0);
		break;
	case GetPortStatus:
		HUB_DEBUG("<HUB_CONTROL> GetPortStatus,	wValue:%04x, wIndex: %04x, wLength:	%04x\n",
			 wValue, wIndex, wLength);

		if (wIndex != 1)
			goto hub_error;
		*(__le32 *)	buf	= cpu_to_le32(acthcd->port);
		HUB_DEBUG("<HUB_CONTROL> the port status is	%08x\n ",
			 acthcd->port);
		break;
	case SetHubFeature:
		HUB_DEBUG("<HUB_CONTROL> SetHubFeature,	wValue:	%04x,wIndex: %04x, wLength:	%04x\n",
			 wValue, wIndex, wLength);
		goto hub_error;
		break;
	case SetPortFeature:
		HUB_DEBUG("<HUB_CONTROL> SetPortFeature, wValue:%04x, wIndex: %04x,	wLength: %04x\n",
			 wValue, wIndex, wLength);

		switch (wValue)	{
		case USB_PORT_FEAT_POWER:
			if (unlikely(acthcd->port &	USB_PORT_STAT_POWER))
				break;
			acthcd->port |=	(1 << wValue);
			acthcd->rhstate	= AOTG_RH_POWERED;
			port_power(acthcd, 1);
			break;
		case USB_PORT_FEAT_RESET:
			port_reset(acthcd);
			/* if it's already enabled,	disable	*/
			acthcd->port &=	~(USB_PORT_STAT_ENABLE
					  |	USB_PORT_STAT_LOW_SPEED
					  |	USB_PORT_STAT_HIGH_SPEED);
			acthcd->port |=	(1 << wValue);
			acthcd->rhstate	= AOTG_RH_RESET;
			usb_setbitsb(USBIEN_URES, acthcd->base + USBIEN);
			/*enable reset irq */
			break;
		case USB_PORT_FEAT_SUSPEND:
			/*acthcd->port |= USB_PORT_FEAT_SUSPEND;*/
			acthcd->port |=	(1 << wValue);
			acthcd->rhstate	= AOTG_RH_SUSPEND;
			//port_suspend(acthcd);
			break;
		default:
			if (acthcd->port & USB_PORT_STAT_POWER)
				acthcd->port |=	(1 << wValue);
		}
		break;
	default:
hub_error:
		retval = -EPIPE;
		dev_err(acthcd->dev, "<HUB_CONTROL>	hub	control	error\n");
		break;

	}
	spin_unlock_irqrestore(&acthcd->lock, flags);
	enable_irq(acthcd->uhc_irq);

	return retval;
}

static void	aotg_DD_set_phy(void __iomem *base,	u8 reg,	u8 value)
{
	u8 addrlow,	addrhigh;
	int	time = 1;

	addrlow	= reg &	0x0f;
	addrhigh = (reg	>> 4) &	0x0f;

	/*write	vstatus: */
	usb_writeb(value, base + VDSTATE);
	
	mb();
	
	/*write	vcontrol: */
	usb_writeb(addrlow | 0x10, base	+ VDCTRL);
	udelay(time); //the	vload period should	> 33.3ns
	usb_writeb(addrlow & 0xf, base + VDCTRL);
	
	udelay(time);
	mb();
	
	usb_writeb(addrhigh	| 0x10,	base + VDCTRL);
	udelay(time);
	usb_writeb(addrhigh	& 0x0f,	base + VDCTRL);
	udelay(time);
	usb_writeb(addrhigh	| 0x10,	base + VDCTRL);
	udelay(time);
	return;
}

static void	aotg_hcd_config_phy(void __iomem *base,	struct platform_device *pdev)
{
	/* 45ohm and disconnet calibration.	*/
	//aotg_DD_set_phy(base,	0xe2,0x80);	 
	//aotg_DD_set_phy(base,	0xe2,0x88);	 
	//aotg_DD_set_phy(base,	0xe2,0x30);
	aotg_DD_set_phy(base, 0xe2,0x60);

	/* bit 0 should	set	to 0 */
	/* vbusdeten信号控制了bias enable，导致在vbusdeten=1时，bias被disable。重新设置vbusdeten=0后PLL工作正常	*/
	//aotg_DD_set_phy(base,	0xe7,0x0b);
	//aotg_DD_set_phy(base,	0xe7,0x1f);	
	aotg_DD_set_phy(base, 0xe7,0x0a);
	aotg_DD_set_phy(base, 0xe7,0x0e);
	udelay(1);

	/* sesitivetiy calibration */
	//aotg_DD_set_phy(base,	0xe3,0x9c);
	//aotg_DD_set_phy(base,	0xe3,0xdc);
	//udelay(100);

	/* configurate the 45ohm and disconnet */
	//aotg_DD_set_phy(base,	0xe2,0x8f);
	//aotg_DD_set_phy(base,	0xe2,0x88);
	//aotg_DD_set_phy(base,	0xe2,0x31);
	//aotg_DD_set_phy(base,	0xe2,0x31);
	//aotg_DD_set_phy(base,	0xe2,0x61);

	aotg_DD_set_phy(base, 0xe2,0x74);
	aotg_DD_set_phy(base, 0xe4,0x47);

	/****end of	the	PHY	setting**********/
	//aotg_DD_set_phy(base,	0xe6,0x60);		// Disconnect judgment threshold = 592mv, 
	return;
}

static void	aotg_DD_set_phy_init(struct	aotg_hcd *acthcd)
{
	int	time = 600;

	/* if use usb1,	but	usb0 isn't initialized,	we initialize usb0 first. */
	if ((((hcd_2clk_bits_en	& 0x1) == 0) &&	(acthcd->id	== 1)) || 
		(((hcd_2clk_bits_en	& 0x2) == 0) &&	(acthcd->id	== 0)))	{
		/*write	vstatus: */
		usb_writeb(0x0a, aotg0_base	+ VDSTATE);
		mb();
		udelay(time);
		/*write	vcontrol: */
		usb_writeb(0x17, aotg0_base	+ VDCTRL);
		udelay(time); //the	vload period should	> 33.3ns
		usb_writeb(0x07, aotg0_base	+ VDCTRL);
		udelay(time);
		usb_writeb(0x17, aotg0_base	+ VDCTRL);
		udelay(time);
		mb();
		usb_writeb(0x1e, aotg0_base	+ VDCTRL);
		udelay(time); 
		usb_writeb(0x0e, aotg0_base	+ VDCTRL);
		udelay(time);
		usb_writeb(0x1e, aotg0_base	+ VDCTRL);
		udelay(time);

		aotg_hcd_config_phy(aotg0_base,	to_platform_device(acthcd->dev));
	}

	if (acthcd->id == 0) {
		return;
	}

	/*write	vstatus: */
	usb_writeb(0x0a, acthcd->base +	VDSTATE);
	mb();
	udelay(time);
	/*write	vcontrol: */
	usb_writeb(0x17, acthcd->base +	VDCTRL);
	udelay(time); //the	vload period should	> 33.3ns
	usb_writeb(0x07, acthcd->base +	VDCTRL);
	udelay(time);
	usb_writeb(0x17, acthcd->base +	VDCTRL);
	udelay(time);
	mb();
	usb_writeb(0x1e, acthcd->base +	VDCTRL);
	udelay(time); 
	usb_writeb(0x0e, acthcd->base +	VDCTRL);
	udelay(time);
	usb_writeb(0x1e, acthcd->base +	VDCTRL);
	udelay(time);
	aotg_hcd_config_phy(aotg1_base,	to_platform_device(acthcd->dev));
	return;
}

static void	aotg_hcd_clk_enable(struct aotg_hcd	*acthcd, int is_enable)
{
	struct aotg_plat_data *data	= acthcd->port_specific;
	__u32 tmp;
	struct clk *dev_pll	= NULL;

	if (is_enable) {
		if (hcd_2clk_bits_en & 0x3)	{
			// do nothing.
		} else {
			dev_pll	= clk_get_sys(CLK_NAME_DEVPLL, NULL); /*根据clk_name获取clk结构体*/
			if (IS_ERR(dev_pll)) {
				ACT_HCD_ERR
				return;
			}
			clk_enable(dev_pll);  /*enable clk*/

			usb_setbitsl(data->usbpll_bits,	data->usbpll);
			tmp	= usb_readl(data->usbpll);
		}
		hcd_2clk_bits_en |=	0x1	<< acthcd->id;
	} else {
		if (!hcd_2clk_bits_en) {
			return;
		}
		hcd_2clk_bits_en &=	~(0x1 << acthcd->id);
		if (!hcd_2clk_bits_en) {
			dev_pll	= clk_get_sys(CLK_NAME_DEVPLL, NULL); /*根据clk_name获取clk结构体*/

			usb_clearbitsl(data->usbpll_bits, data->usbpll);
			if (!IS_ERR(dev_pll)) {
				clk_disable(dev_pll);  /*disable clk*/
			}
		}
	}

	return;
}

static int aotg_hcd_controller_reset(struct	aotg_hcd *acthcd)
{
	struct aotg_plat_data *data	= acthcd->port_specific;
	int	i =	0;

	usb_clearbitsl(data->devrst_bits, data->devrst);
	udelay(1);
	usb_setbitsl(data->devrst_bits,	data->devrst);

	while (((usb_readb(acthcd->base	+ USBERESET) & USBERES_USBRESET) !=	0) && (i < 500000))	{
		i++;
		udelay(1);
	}

	if (!(usb_readb(acthcd->base + USBERESET) &	USBERES_USBRESET)) {
		dev_info(acthcd->dev, "usb reset OK.\n");
	} else {
		dev_err(acthcd->dev, "usb reset	ERROR.\n");
		return -EBUSY;
	}
	return 0;
}

static void	aotg_hcd_hardware_init(struct aotg_hcd *acthcd)
{
	unsigned long flags;
	struct aotg_plat_data *data	= acthcd->port_specific;

	aotg_hcd_clk_enable(acthcd,	1);

	local_irq_save(flags);

	aotg_hcd_controller_reset(acthcd);
	aotg_DD_set_phy_init(acthcd);

	usb_writel(0, data->usbecs);
	udelay(1);
	usb_writel(0x1<<26|0x1<<25|0x1<<24|0x3<<4, data->usbecs); //set	soft vbus and id, vbus threshold 3.43~3.57

	usb_writeb(0xff, acthcd->base +	TAAIDLBDIS);
	/*set TA_AIDL_BDIS timeout never generate */
	usb_writeb(0xff, acthcd->base +	TAWAITBCON);
	/*set TA_WAIT_BCON timeout never generate */
	usb_writeb(0x28, acthcd->base +	TBVBUSDISCHPLS);
	usb_setbitsb(1 << 7, acthcd->base +	TAWAITBCON);
	//usb_writel(0x1<<26|0x1<<25|0x1<<24|0x3<<4, data->usbecs);	//set soft vbus	and	id,	vbus threshold 3.43~3.57
	//dev_info(acthcd->dev,	"usbecs	set	to 0x%08X.\n", usb_readl(data->usbecs));
	usb_writew(0x1000, acthcd->base	+ VBUSDBCTIMERL);
	usb_writeb(0x40, acthcd->base +	USBCS);

	if (data &&	data->no_hs)
		usb_setbitsb(1 << 7, acthcd->base +	BKDOOR);

	//aotg_hcd_config_phy(acthcd->base,	to_platform_device(acthcd->dev));
	local_irq_restore(flags);
	return;
}

static int aotg_hcd_init(struct	usb_hcd	*hcd, struct platform_device *pdev)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	int	retval = 0;
	int	i;

	/*init software	state */
	spin_lock_init(&acthcd->lock);
	spin_lock_init(&acthcd->tasklet_lock);
	acthcd->tasklet_retry =	0;
	//acthcd->dev =	&pdev->dev;
	acthcd->port_specific =	pdev->dev.platform_data;
	acthcd->port = 0;
	acthcd->rhstate	= AOTG_RH_POWEROFF;
	acthcd->inserted = 0;

	acthcd->frame =	0;//NO_FRAME;
	acthcd->periodic_count = 0;
	acthcd->sof_kref = 0;
	INIT_LIST_HEAD(&acthcd->hcd_enqueue_list);
	INIT_LIST_HEAD(&acthcd->hcd_dequeue_list);
	INIT_LIST_HEAD(&acthcd->hcd_finished_list);
	tasklet_init(&acthcd->urb_tasklet, urb_tasklet_func, (unsigned long)acthcd);

	for	(i = 0;	i <	PERIODIC_SIZE; i++)	{
		acthcd->periodic[i]	= NULL;
	}


	acthcd->ep0	= NULL;
	for	(i = 0;	i <	MAX_EP_NUM;	i++) {
		acthcd->inep[i]	= NULL;
		acthcd->outep[i] = NULL;
	}

	acthcd->fifo_map[0]	= 1<<31;
	acthcd->fifo_map[1]	= 1<<31	| 64;
	for	(i = 2;	i <	64;	i++) {
		acthcd->fifo_map[i]	= 0;
	}

	INIT_LIST_HEAD(&acthcd->dma_queue[0]);
	INIT_LIST_HEAD(&acthcd->dma_queue[1]);

	acthcd->put_aout_msg = 0;
	acthcd->uhc_irq	= 0;
	for	(i = 0;	i <	AOTG_QUEUE_POOL_CNT; i++) {
		acthcd->queue_pool[i] =	NULL;
	}

	acthcd->periodic_wq	= create_singlethread_workqueue("aotg_periodic_wq");
	if (NULL ==	acthcd->periodic_wq) {
		printk("Error: create periodic workqueue failed!\n");
		retval = -1;
	}

	return retval;
}

static inline void aotg_hcd_exit(struct	aotg_hcd *acthcd)
{
	if (acthcd->periodic_wq) {
		flush_workqueue(acthcd->periodic_wq);
		destroy_workqueue(acthcd->periodic_wq);
	}
}


static const char driver_name[]	= "aotg_hcd";

/* control & interrupt using pio only.
 * it's	called before urb_enqueue, so is_use_pio is	not	assigned.
 */
static int aotg_map_urb_for_dma(struct usb_hcd *hcd, struct	urb	*urb, gfp_t	mem_flags)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	int	ret	= 0;
	//struct aotg_hcep * ep	= NULL;

#if	0
	if (urb->ep->hcpriv) {
		ep = (struct aotg_hcep *)urb->ep->hcpriv;
		if (unlikely(ep->is_use_pio)) {
			return ret;
		}
	}
#endif
	if ((acthcd->use_dma &&	usb_pipebulk(urb->pipe)) &&	
		(urb->transfer_buffer != NULL) && 
		//(urb->transfer_buffer_length >= 512) && 
		(urb->transfer_buffer_length >=	AOTG_MIN_DMA_SIZE))	{
		ret	= usb_hcd_map_urb_for_dma(hcd, urb,	mem_flags);
	}
	return ret;
}

static void	aotg_unmap_urb_for_dma(struct usb_hcd *hcd,	struct urb *urb)
{
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	struct aotg_hcep * ep =	NULL;

	if (urb->ep->hcpriv) {
		ep = (struct aotg_hcep *)urb->ep->hcpriv;
		if (unlikely(ep->is_use_pio)) {
			return;
		}
	}
	if ((acthcd->use_dma &&	usb_pipebulk(urb->pipe)) &&
		(urb->transfer_buffer != NULL) && 
		//(urb->transfer_buffer_length >= 512) && 
		(urb->transfer_buffer_length >=	AOTG_MIN_DMA_SIZE))	{
		usb_hcd_unmap_urb_for_dma(hcd, urb);
	}
	return;
}

static struct hc_driver	act_hc_driver =	{
	.description = driver_name,
	.hcd_priv_size = sizeof(struct aotg_hcd),
	.product_desc =	DRIVER_DESC,

	/*
	 * generic hardware	linkage
	 */
	.irq = aotg_hcd_irq,
	.flags = HCD_USB2 |	HCD_MEMORY,

	/* Basic lifecycle operations */
	.start = aotg_hcd_start,
	.stop =	aotg_hcd_stop,

	/*
	 * managing	i/o	requests and associated	device resources
	 */
	.urb_enqueue = aotg_hcd_urb_enqueue,
	.urb_dequeue = aotg_hcd_urb_dequeue,

	.map_urb_for_dma	= aotg_map_urb_for_dma,
	.unmap_urb_for_dma	= aotg_unmap_urb_for_dma,

	.endpoint_disable =	aotg_hcd_endpoint_disable,

	/*
	 * periodic	schedule support
	 */
	.get_frame_number =	aotg_hcd_get_frame,

	/*
	 * root	hub	support
	 */
	.hub_status_data = aotg_hub_status_data,
	.hub_control = aotg_hub_control,

	.bus_suspend =		  aotg_hub_suspend,
	.bus_resume	=		  aotg_hub_resume,
};


static int aotg_hcd_probe(struct platform_device *pdev)
{
	struct usb_hcd *hcd;
	struct resource	*res_mem;	//,	*res_irq;
	int	irq;
	void __iomem *base;
	int	retval;
	struct aotg_hcd	*acthcd	= NULL;
	 
	//printk("acts_hcd.ko version: 20121206.\n");

	res_mem	= platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res_mem) {
		dev_err(&pdev->dev,	"<HCD_PROBE>usb	has	no resource	for	mem!\n");
		retval = -ENODEV;
		goto err0;
	}

	hcd	= usb_create_hcd(&act_hc_driver, &pdev->dev, dev_name(&pdev->dev));
	if (!hcd) {
		dev_err(&pdev->dev,	"<HCD_PROBE>usb	create hcd failed\n");
		retval = -ENOMEM;
		goto err0;
	}

	hcd->rsrc_start	= res_mem->start;
	hcd->rsrc_len =	res_mem->end - res_mem->start +	1;
	
	irq	= platform_get_irq(pdev, 0);
	if (irq	<= 0) {
		dev_err(&pdev->dev,	"<HCD_PROBE>usb	has	no resource	for	irq!\n");
		retval = -ENODEV;
		goto err1;
	}

	if (!request_mem_region(res_mem->start,	res_mem->end - res_mem->start +	1, dev_name(&pdev->dev))) {
		dev_err(&pdev->dev,	"<HCD_PROBE>request_mem_region failed\n");
		retval = -EBUSY;
		goto err1;
	}

	hcd->regs =	ioremap(res_mem->start,	res_mem->end - res_mem->start +	1);
	if (!hcd->regs)	{
		dev_err(&pdev->dev,	"<HCD_PROBE>ioremap	failed\n");
		retval = -ENOMEM;
		goto err2;
	}
	aotg_base =	hcd->regs;
	aotg0_base = hcd->regs;
	aotg1_base = hcd->regs + AOTG_REGS_SIZE;
	
	//dev_info(&pdev->dev, "hcd->regs :	0x%p\n", hcd->regs);
	if (pdev->id ==	0) {
		base = aotg0_base;
	} else {
		base = aotg1_base;
	}

	retval = aotg_hcd_init(hcd,	pdev);
	if (retval)	{
		dev_err(&pdev->dev,	"<HCD_PROBE>hcd	init failed\n");
		goto err3;
	}

	acthcd = hcd_to_aotg(hcd);
	acthcd->dev	= &pdev->dev;
	acthcd->base = base;
	acthcd->hcd_exiting	= 0;
	acthcd->uhc_irq	= irq;

	//printk("pdev->id probe:%d\n",	pdev->id);
	acthcd->id = pdev->id;
	if (pdev->id ==	0) {
		p_aotg_hcd0	= acthcd;
	} else {
		p_aotg_hcd1	= acthcd;
	}

	acthcd->bulkout_wait_ep	= NULL;
	acthcd->timer_bulkout_state	= AOTG_BULKOUT_NULL;

	aotg_hcd_hardware_init(acthcd);
	//pdev->dev.dma_mask = NULL;

	hcd->has_tt	= 1;
	hcd->self.uses_pio_for_control = 1;	// for ep0,	using CPU mode only

	if (pdev->dev.dma_mask)	{
		retval = aotg_dma_request_2(aotg_hcd_dma_handler, acthcd);
		printk("id:%d, usb_dma:	no:%d\n", pdev->id,	retval);

		if (retval >= 0) {
			//hcd->self.sg_tablesize = MAX_SG_TABLE	- 1;
			hcd->self.sg_tablesize = 0;
			acthcd->dma_nr0	= retval;
			acthcd->dma_nr1	= retval + 1;
			acthcd->use_dma	= 1;
			acthcd->dma_working[0] = 0;
			acthcd->dma_working[1] = 0;
			aotg_dma_reset_2(acthcd->dma_nr0);
			aotg_dma_enable_irq_2(acthcd->dma_nr0);
			aotg_dma_clear_pend_2(acthcd->dma_nr0);
		} else {
			hcd->self.sg_tablesize = 0;
			acthcd->dma_nr0	= retval;
			acthcd->dma_nr1	= -1;
			acthcd->use_dma	= 0;
			dev_err(acthcd->dev, "Can't	register IRQ for DMA, retval: %d\n", retval);
			goto err3;
		}
	} else {
		acthcd->use_dma	= 0;
		acthcd->dma_working[0] = 0;
		acthcd->dma_working[1] = 0;
		acthcd->dma_nr0	= -1;
		acthcd->dma_nr1	= -1;
		dev_dbg(&pdev->dev,	"<hcd_probe>: hcd use pio\n");
	}
	acthcd->last_bulkin_dma_len	= 1;

	init_timer(&acthcd->hotplug_timer);
	acthcd->hotplug_timer.function = aotg_hcd_hotplug_timer;
	acthcd->hotplug_timer.data = (unsigned long)acthcd;

	init_timer(&acthcd->trans_wait_timer);
	acthcd->trans_wait_timer.function =	aotg_hcd_trans_wait_timer;
	acthcd->trans_wait_timer.data =	(unsigned long)acthcd;
	
	retval = usb_add_hcd(hcd, irq, 0);
	if (likely(retval == 0)) {
		//dev_info(&pdev->dev, " ==> open vbus.\n");
		aotg_enable_irq(acthcd);
		create_debug_file(acthcd);
		dev_info(&pdev->dev, "hcd controller initialized. OTGIRQ: 0x%02X, OTGSTATE:	0x%02X \n",	
				usb_readb(acthcd->base + OTGIRQ),
				usb_readb(acthcd->base + OTGSTATE));

		if (usb_readb(acthcd->base + OTGSTATE) == AOTG_STATE_A_HOST) {
			acthcd->put_aout_msg = 0;
			mod_timer(&acthcd->hotplug_timer, jiffies +	msecs_to_jiffies(3));
		}

		return 0;
	} else {
		dev_err(acthcd->dev, "<HCD_PROBE>usb add hcd failed\n");
	}
	
	del_timer_sync(&acthcd->hotplug_timer);
	del_timer_sync(&acthcd->trans_wait_timer);
	aotg_dma_free_2(acthcd->dma_nr0);

	aotg_hcd_exit(acthcd);
err3:
	if (acthcd != NULL)	{
		aotg_hcd_clk_enable(acthcd,	0);
		iounmap(hcd->regs);
	}
err2:
	release_mem_region(res_mem->start, res_mem->end	- res_mem->start + 1);
err1:
	usb_put_hcd(hcd);
err0:
	if (pdev->id ==	0) {
		p_aotg_hcd0	= NULL;
	} else {
		p_aotg_hcd1	= NULL;
	}
	dev_err(&pdev->dev,	"%s: usb probe hcd	failed,	error is %d", __func__,	retval);

	return retval;
}

static int aotg_hcd_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd	= platform_get_drvdata(pdev);
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);

	/* make	sure all urb request is	exited.	*/
	struct aotg_queue *q = NULL, *next;
	struct urb *urb;
	struct aotg_hcep *ep;
	int	i;

	acthcd->hcd_exiting	= 1;

	ep = acthcd->ep0;
	if (ep)	{
		list_for_each_entry_safe(q,	next, &ep->q_list, ep_q_list) {
			finish_request(acthcd, q, -EIO);
		}
	}
	for	(i = 1;	i <	MAX_EP_NUM;	i++) {
		ep = acthcd->inep[i];
		if (ep)	{
			list_for_each_entry_safe(q,	next, &ep->q_list, ep_q_list) {
				finish_request(acthcd, q, -EIO);
			}
		}
	}
	for	(i = 1;	i <	MAX_EP_NUM;	i++) {
		ep = acthcd->outep[i];
		if (ep)	{
			list_for_each_entry_safe(q,	next, &ep->q_list, ep_q_list) {
				finish_request(acthcd, q, -EIO);
			}
		}
	}
	list_for_each_entry_safe(q,	next, &acthcd->hcd_enqueue_list, enqueue_list) {
		urb	= q->urb;
		ep = q->ep;
		if (ep)
			ep->urb_unlinked_cnt++;
		list_del(&q->enqueue_list);
		hcd	= bus_to_hcd(urb->dev->bus);
		usb_hcd_unlink_urb_from_ep(hcd,	urb);
		aotg_hcd_release_queue(acthcd, q);
		usb_hcd_giveback_urb(hcd, urb, -EIO);
	}

	del_timer_sync(&acthcd->trans_wait_timer);
	del_timer_sync(&acthcd->hotplug_timer);
	remove_debug_file(acthcd);
	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start,	hcd->rsrc_len);
	aotg_hcd_release_queue(acthcd, NULL);

	if (acthcd->use_dma) {
		aotg_dma_free_2(acthcd->dma_nr0);
	}
	
	aotg_disable_irq(acthcd);

	aotg_hcd_exit(acthcd);
	aotg_hcd_clk_enable(acthcd,	0);
	usb_put_hcd(hcd);
	if (pdev->id ==	0) {
		p_aotg_hcd0	= NULL;
	} else {
		p_aotg_hcd1	= NULL;
	}
	printk("pdev->id remove:%d\n", pdev->id);
	return 0;
}

#ifdef	CONFIG_PM
static int aotg_hcd_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct usb_hcd *hcd	= platform_get_drvdata(pdev);
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	int	i =	0;

	if ((hcd ==	NULL) || (acthcd ==	NULL)) {
		ACT_HCD_ERR
		return 0;
	}
	dev_info(acthcd->dev, "	==>	%s\n", __func__);
	do {
		i++;
		msleep(1);
	} while	((acthcd->suspend_request_pend != 0) &&	(i < 200));

	aotg_disable_irq(acthcd);
	//usb_clearbitsb(OTGCTRL_BUSREQ, acthcd->base +	OTGCTRL);
	aotg_hcd_clk_enable(acthcd,	0);

	/* power off. */
	if (acthcd->id == 0) {
		aotg0_device_exit(1);
	} else {
		aotg1_device_exit(1);
	}
	return 0;
}

static int aotg_hcd_resume(struct platform_device *pdev)
{
	struct usb_hcd *hcd	= platform_get_drvdata(pdev);
	struct aotg_hcd	*acthcd	= hcd_to_aotg(hcd);
	
	if ((hcd ==	NULL) || (acthcd ==	NULL)) {
		ACT_HCD_ERR
		return 0;
	}

	/* power on. */
	if (acthcd->id == 0) {
		aotg0_device_init(1);
	} else {
		aotg1_device_init(1);
	}
	msleep(10);

	dev_info(acthcd->dev, "	==>	%s\n", __func__);
	aotg_hcd_hardware_init(acthcd);

	if (acthcd->use_dma	!= 0) {
		acthcd->dma_working[0] = 0;
		acthcd->dma_working[1] = 0;
		aotg_dma_reset_2(acthcd->dma_nr0);
		aotg_dma_enable_irq_2(acthcd->dma_nr0);
		aotg_dma_clear_pend_2(acthcd->dma_nr0);
	}
	acthcd->last_bulkin_dma_len	= 1;
	//INIT_LIST_HEAD(&acthcd->dma_queue[0]);
	//INIT_LIST_HEAD(&acthcd->dma_queue[1]);
	//INIT_LIST_HEAD(&acthcd->hcd_enqueue_list);

	acthcd->bulkout_wait_ep	= NULL;
	acthcd->bulkout_ep_busy_cnt	= 0;
	acthcd->timer_bulkout_state	= AOTG_BULKOUT_NULL;

	aotg_enable_irq(acthcd);
	return 0;
}
#endif

static struct platform_driver aotg_hcd_driver =	{
	.probe = aotg_hcd_probe,
	.remove	= aotg_hcd_remove,
#ifdef	CONFIG_PM
	.suspend = aotg_hcd_suspend,
	.resume	= aotg_hcd_resume,
#endif
	.driver	= {
		.owner = THIS_MODULE,
		.name =	driver_name,
	},
};

static inline int aotg_device_calc_id(int dev_id)
{
	int	id;

	if (hcd_ports_en_ctrl == 1)	{
		id = 0;
	} else if (hcd_ports_en_ctrl ==	2) {
		id = 1;
	} else if (hcd_ports_en_ctrl ==	3) {
		if (dev_id)	{
			id = 0;
		} else {
			id = 1;
		}
	} else {
		id = dev_id;
	}
	return id;
}

int	aotg_device_register(int dev_id)
{
	dev_id = aotg_device_calc_id(dev_id);

	wake_lock(&acts_hcd_wakelock);

	if (dev_id)	{
		return aotg1_device_init(0);
	} else {
		return aotg0_device_init(0);
	}
}
EXPORT_SYMBOL(aotg_device_register);

void aotg_device_unregister(int	dev_id)
{
	dev_id = aotg_device_calc_id(dev_id);

	wake_unlock(&acts_hcd_wakelock);

	if (dev_id)	{
		return aotg1_device_exit(0);
	} else {
		return aotg0_device_exit(0);
	}
}
EXPORT_SYMBOL(aotg_device_unregister);


static int __init ahcd_init(void)
{
	int ret = 0;
	wake_lock_init(&acts_hcd_wakelock, WAKE_LOCK_SUSPEND, "acts_hcd");
	ret = get_config("usb2_ports.ports_config", (char *)(&hcd_ports_en_ctrl), sizeof(unsigned int));
	if (ret != 0) {
		hcd_ports_en_ctrl = 0;
		printk("no ports config.\n");
	} else {        
		printk("ports config:%d\n", hcd_ports_en_ctrl);
	}
	aotg0_device_mod_init();
	aotg1_device_mod_init();
	create_acts_hcd_proc();
	return platform_driver_register(&aotg_hcd_driver);	
}

static void	__exit ahcd_exit(void)
{
	remove_acts_hcd_proc();
	platform_driver_unregister(&aotg_hcd_driver);	
	wake_lock_destroy(&acts_hcd_wakelock);
	aotg0_device_mod_exit();
	aotg1_device_mod_exit();
	return;
}

module_init(ahcd_init);
module_exit(ahcd_exit);

MODULE_DESCRIPTION("Actions	HCD	controller driver");
MODULE_LICENSE("GPL");

