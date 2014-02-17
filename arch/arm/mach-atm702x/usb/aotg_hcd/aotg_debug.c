/*
 * (C) Copyright www.actions-semi.com 2012-2014
 *     Written by houjingkun. <houjingkun@actions-semi.com>
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
#include <linux/platform_device.h>
#include <linux/scatterlist.h>
#include <linux/dma-mapping.h>
#include <linux/highmem.h>
#include <linux/usb.h>
#include <linux/usb/ch9.h>
#include <linux/usb/otg.h>
#include <linux/usb/hcd.h>
#include <asm/system.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <mach/actions_reg_gl5202.h>
#include <mach/hardware.h>
#include <linux/gpio.h>

#include "aotg_hcd.h"
#include "aotg_regs.h"
#include "aotg_plat_data.h"
#include "aotg_dma.h"
#include "aotg_debug.h"


static char aotg_hcd_proc_sign = 'n';

#ifdef AOTG_DEBUG_RECORD_URB
#define AOTG_BK_URB_CNT		20
static unsigned int aotg_hcd_seq_info = 0;

static int aotg_bk_ep_idx = 0;
static unsigned int aotg_bk_ep_num[AOTG_BK_URB_CNT];
static unsigned int aotg_bk_ep_type[AOTG_BK_URB_CNT];
static unsigned int aotg_bk_ep_len[AOTG_BK_URB_CNT];
static unsigned int aotg_bk_ep_seq[AOTG_BK_URB_CNT];
static unsigned int aotg_bk_ep_inout[AOTG_BK_URB_CNT];
static unsigned int aotg_bk_ep_buf[AOTG_BK_URB_CNT];
static unsigned int aotg_bk_ep_tcpseq[AOTG_BK_URB_CNT];
static unsigned int aotg_bk_ep_ackseq[AOTG_BK_URB_CNT];

static unsigned int recent_out_seq_info = 0;
static unsigned int recent_tcp_seq_info_put = 0;
static unsigned int recent_tcp_seq_info_get = 0;

#if 0
static int aotg_bk_check_ep_seq(void)
{
	int i;
	unsigned int in_seq;
	unsigned int out_seq_epout;
	unsigned int out_seq_epin;
	unsigned int aotg_bk_ep_idx_backup = aotg_bk_ep_idx;

	in_seq = 0;
	out_seq_epout = 0;
	out_seq_epin = 0;

	for (i = 0; i < AOTG_BK_URB_CNT; i++) {
		if (aotg_bk_ep_inout[aotg_bk_ep_idx]) {
			if ((in_seq > aotg_bk_ep_seq[aotg_bk_ep_idx]) && (aotg_bk_ep_seq[aotg_bk_ep_idx] != 0)) {
				ACT_HCD_ERR
				printk("i:%d\n", i);
				printk("in_seq:%d\n", in_seq);
				printk("aotg_bk_ep_idx:%d, %d\n", aotg_bk_ep_idx, aotg_bk_ep_seq[aotg_bk_ep_idx]);
				WARN_ON(1);
				aotg_bk_ep_idx = aotg_bk_ep_idx_backup;
				aotg_dbg_proc_output_ep();
				return 1;
			}
			in_seq = aotg_bk_ep_seq[aotg_bk_ep_idx];
		} else {
			if ((in_seq != 0) && (aotg_bk_ep_seq[aotg_bk_ep_idx] > in_seq)) {
				printk("i:%d\n", i);
				printk("in_seq:%d\n", in_seq);
				printk("out_seq_epout:%d\n", out_seq_epout);
				printk("aotg_bk_ep_idx:%d, %d\n", aotg_bk_ep_idx, aotg_bk_ep_seq[aotg_bk_ep_idx]);
				ACT_HCD_ERR
				WARN_ON(1);
				aotg_bk_ep_idx = aotg_bk_ep_idx_backup;
				aotg_dbg_proc_output_ep();
				return 1;
			}

			if (aotg_bk_ep_type[aotg_bk_ep_idx] == 0) {
				if ((out_seq_epout > aotg_bk_ep_seq[aotg_bk_ep_idx]) && (aotg_bk_ep_seq[aotg_bk_ep_idx] != 0)) {
					printk("i:%d\n", i);
					printk("out_seq_epout:%d\n", out_seq_epout);
					printk("aotg_bk_ep_idx:%d, %d\n", aotg_bk_ep_idx, aotg_bk_ep_seq[aotg_bk_ep_idx]);
					ACT_HCD_ERR
					WARN_ON(1);
					aotg_bk_ep_idx = aotg_bk_ep_idx_backup;
					aotg_dbg_proc_output_ep();
					return 1;
				}
				out_seq_epout = aotg_bk_ep_seq[aotg_bk_ep_idx];
			} else {
				if ((out_seq_epin > aotg_bk_ep_seq[aotg_bk_ep_idx]) && (aotg_bk_ep_seq[aotg_bk_ep_idx] != 0)) {
					printk("i:%d\n", i);
					printk("out_seq_epin:%d\n", out_seq_epin);
					printk("aotg_bk_ep_idx:%d, %d\n", aotg_bk_ep_idx, aotg_bk_ep_seq[aotg_bk_ep_idx]);
					ACT_HCD_ERR
					WARN_ON(1);
					aotg_bk_ep_idx = aotg_bk_ep_idx_backup;
					aotg_dbg_proc_output_ep();
					return 1;
				}
				out_seq_epin = aotg_bk_ep_seq[aotg_bk_ep_idx];
			}	
		}
		aotg_bk_ep_idx++;
		if (aotg_bk_ep_idx >= AOTG_BK_URB_CNT)
			aotg_bk_ep_idx = 0;
	}
	return 0;
}
#endif

/* type -- 1 == in; 0 == out. */
void aotg_dbg_put_q(struct aotg_queue *q, unsigned int num, unsigned int type, unsigned int len)
{
	struct urb *urb = q->urb;
	//struct aotg_hcep *ep = q->ep;
	char *buf;
	//unsigned int *u32_buf;
	char c_val;
	unsigned int tcp_seq;

	if (num == 0)
		return;
	if (aotg_hcd_proc_sign == 'z') {
		return;
	}
	q->seq_info = aotg_hcd_seq_info++;

	if (((aotg_hcd_proc_sign == 't') && (type == 0)) ||
	    ((aotg_hcd_proc_sign == 'f') && (type != 0)))
	{
		//printk("trace:\n");
		//printk("num:%d ", num);
		//printk("in:%d ", type);
		//printk("seq:%d ", q->seq_info);
		printk("len:%d \n", len);
	}
	aotg_bk_ep_num[aotg_bk_ep_idx] = num;
	aotg_bk_ep_type[aotg_bk_ep_idx] = type;
	aotg_bk_ep_len[aotg_bk_ep_idx] = len;
	aotg_bk_ep_seq[aotg_bk_ep_idx] = q->seq_info;
	aotg_bk_ep_inout[aotg_bk_ep_idx] = 1;
	aotg_bk_ep_buf[aotg_bk_ep_idx] = (unsigned int)urb->transfer_buffer;
	aotg_bk_ep_tcpseq[aotg_bk_ep_idx] = 0;
	aotg_bk_ep_ackseq[aotg_bk_ep_idx] = 0;

#if 0	// debug tcp seq.
	buf = urb->transfer_buffer;

	if (usb_pipein(urb->pipe) == 0) {

		if ((len % 512) == 0) {
			//ACT_HCD_DBG
			printk("len:%d \n", len);
		}
		if (urb->transfer_buffer_length > 800000) {
		//if (urb->transfer_buffer_length > 80) 

			if ((buf[64+8] == 8) &&
			    (buf[64+9] == 0) &&		// ip type
			    (buf[64+19] == 6)) {	// tcp type

				tcp_seq = 0;
				c_val = buf[96+2];
				tcp_seq |= (unsigned int)c_val << 24;
				c_val = buf[96+3];
				tcp_seq |= (unsigned int)c_val << 16;
				c_val = buf[96+4];
				tcp_seq |= (unsigned int)c_val << 8;
				c_val = buf[96+5];
				tcp_seq |= (unsigned int)c_val << 0;

				//aotg_dbg_put_info("out_tcpseq", urb->transfer_buffer_length, tcp_seq, 0);

				if (recent_tcp_seq_info_put > tcp_seq) {
					//ACT_HCD_DBG
					//WARN_ON(q);
					ACT_HCD_DBG
					printk("==== seq width:%d, %x\n", 
					    (recent_tcp_seq_info_put - tcp_seq),
					    (recent_tcp_seq_info_put - tcp_seq));

					//printk("urb->transfer_buffer:%x\n", (unsigned int)urb->transfer_buffer);
					//printk("recent_out_seq_info:%d\n", recent_out_seq_info);
					//printk("q->seq_info:%d\n", q->seq_info);
					//printk("recent_tcp_seq_info_get:%x\n", recent_tcp_seq_info_get);
					printk("recent_tcp_seq_info_put:%x\n", recent_tcp_seq_info_put);
					printk("tcp_seq:%x\n", tcp_seq);

					//ACT_HCD_DBG
					//aotg_dbg_proc_output_ep();
					//ACT_HCD_DBG
					//aotg_dbg_output_info();
					//BUG_ON(1);
				}
				recent_tcp_seq_info_put = tcp_seq;
				aotg_bk_ep_tcpseq[aotg_bk_ep_idx] = tcp_seq;
			}
		}
	} else {
		//u32_buf = (unsigned int *)buf;
		//buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0;
	}
#endif

	aotg_bk_ep_idx++;
	if (aotg_bk_ep_idx >= AOTG_BK_URB_CNT)
		aotg_bk_ep_idx = 0;

	//aotg_bk_check_ep_seq();
	return;
}

void aotg_dbg_finish_q(struct aotg_queue *q)
{
	struct urb *urb = q->urb;
	struct aotg_hcep *ep = q->ep;
	char *buf;
	char c_val;
	unsigned int tcp_seq;
	unsigned int ack_seq;
	int i;
#if 0	/* zero check. */
	unsigned int *u32_buf;
#endif
#if 0	/* test rtw's data payload right or not. */
	char *rtw_buf;
	unsigned int pkt_len;
	unsigned int total_offset;
	unsigned int agg_cnt;
	unsigned int need_dump = 0;
#endif

	if (ep->epnum == 0)
		return;
	if (aotg_hcd_proc_sign == 'z') {
		return;
	}

	aotg_bk_ep_tcpseq[aotg_bk_ep_idx] = 0;
	aotg_bk_ep_ackseq[aotg_bk_ep_idx] = 0;
#if 0	// debug.
	buf = urb->transfer_buffer;

	if (usb_pipein(urb->pipe) == 0) {
		if (recent_out_seq_info > q->seq_info) {
			ACT_HCD_DBG
			printk("recent_out_seq_info:%d\n", recent_out_seq_info);
			printk("q->seq_info:%d\n", q->seq_info);
		} else {
			recent_out_seq_info = q->seq_info;
		}
#if 0	/* tmp test net disconnet bug. */
		if (aotg_hcd_proc_sign == 'e') {
			/* out icmp packet. */
			if ((buf[72] == 8) &&
			    (buf[73] == 0) &&		// ip type
			    (buf[83] == 0x1)) {	// icmp type
				printk("icmp--out type:%x %x \n",  
					(unsigned int)buf[94+0], (unsigned int)buf[94+1]);
			}
			/* out arp packet. */
			if ((buf[72] == 8) &&
			    (buf[73] == 6)) {		// arp type
				printk("----arp--out hw:%x %x, pro:%x %x ",  
					(unsigned int)buf[74+0], (unsigned int)buf[74+1],
					(unsigned int)buf[74+2], (unsigned int)buf[74+3]);
				printk(" hw len:%x, ip len: %x ",  (unsigned int)buf[74+4], (unsigned int)buf[74+5]);
				printk(" op:%x %x ",  (unsigned int)buf[74+6], (unsigned int)buf[74+7]);
				printk("\n");
				printk("    outhw_src:%x %x %x %x %x %x, ip:%d %d %d %d ",  
					(unsigned int)buf[74+8], (unsigned int)buf[74+9],
					(unsigned int)buf[74+10], (unsigned int)buf[74+11],
					(unsigned int)buf[74+12], (unsigned int)buf[74+13],
					(unsigned int)buf[74+14], (unsigned int)buf[74+15],
					(unsigned int)buf[74+16], (unsigned int)buf[74+17]);
				printk("    outhw_dst:%x %x %x %x %x %x, ip:%d %d %d %d ",  
					(unsigned int)buf[74+18], (unsigned int)buf[74+19],
					(unsigned int)buf[74+20], (unsigned int)buf[74+21],
					(unsigned int)buf[74+22], (unsigned int)buf[74+23],
					(unsigned int)buf[74+24], (unsigned int)buf[74+25],
					(unsigned int)buf[74+26], (unsigned int)buf[74+27]);
				printk("\n");
			}
		}
#endif
		if (urb->transfer_buffer_length > 800000) {

			/* out arp packet. */
			if ((buf[72] == 8) &&
			    (buf[73] == 6)) {		// arp type
				printk("----arp--out hw:%x %x, pro:%x %x ",  
					(unsigned int)buf[74+0], (unsigned int)buf[74+1],
					(unsigned int)buf[74+2], (unsigned int)buf[74+3]);
				printk(" hw len:%x, ip len: %x ",  (unsigned int)buf[74+4], (unsigned int)buf[74+5]);
				printk(" op:%x %x ",  (unsigned int)buf[74+6], (unsigned int)buf[74+7]);
				printk("\n");
				printk("    outhw_src:%x %x %x %x %x %x, ip:%d %d %d %d ",  
					(unsigned int)buf[74+8], (unsigned int)buf[74+9],
					(unsigned int)buf[74+10], (unsigned int)buf[74+11],
					(unsigned int)buf[74+12], (unsigned int)buf[74+13],
					(unsigned int)buf[74+14], (unsigned int)buf[74+15],
					(unsigned int)buf[74+16], (unsigned int)buf[74+17]);
				printk("    outhw_dst:%x %x %x %x %x %x, ip:%d %d %d %d ",  
					(unsigned int)buf[74+18], (unsigned int)buf[74+19],
					(unsigned int)buf[74+20], (unsigned int)buf[74+21],
					(unsigned int)buf[74+22], (unsigned int)buf[74+23],
					(unsigned int)buf[74+24], (unsigned int)buf[74+25],
					(unsigned int)buf[74+26], (unsigned int)buf[74+27]);
				printk("\n");
			}

			/* out icmp packet. */
			if ((buf[72] == 8) &&
			    (buf[73] == 0) &&		// ip type
			    (buf[83] == 0x1)) {	// icmp type
				printk("icmp--out type:%x %x \n",  
					(unsigned int)buf[94+0], (unsigned int)buf[94+1]);
			}

			if ((buf[64+8] == 8) &&
			    (buf[64+9] == 0) &&		// ip type
			    (buf[64+19] == 6)) {	// tcp type

				tcp_seq = 0;
				c_val = buf[96+2];
				tcp_seq |= (unsigned int)c_val << 24;
				c_val = buf[96+3];
				tcp_seq |= (unsigned int)c_val << 16;
				c_val = buf[96+4];
				tcp_seq |= (unsigned int)c_val << 8;
				c_val = buf[96+5];
				tcp_seq |= (unsigned int)c_val << 0;

				//aotg_dbg_put_info("out_tcpseq_complete", urb->transfer_buffer_length, tcp_seq, 0);

				if (recent_tcp_seq_info_get > tcp_seq) {
					//ACT_HCD_DBG
					//WARN_ON(q);
					ACT_HCD_DBG
					printk("urb->transfer_buffer:%x\n", (unsigned int)urb->transfer_buffer);
					printk("recent_out_seq_info:%d\n", recent_out_seq_info);
					printk("q->seq_info:%d\n", q->seq_info);
					printk("recent_tcp_seq_info_get:%x\n", recent_tcp_seq_info_get);
					printk("recent_tcp_seq_info_put:%x\n", recent_tcp_seq_info_put);
					printk("tcp_seq:%x\n", tcp_seq);
					//aotg_dbg_output_info();
				}
				recent_tcp_seq_info_get = tcp_seq;
				aotg_bk_ep_tcpseq[aotg_bk_ep_idx] = tcp_seq;

				ack_seq = 0;
				c_val = buf[100+2];
				ack_seq |= (unsigned int)c_val << 24;
				c_val = buf[100+3];
				ack_seq |= (unsigned int)c_val << 16;
				c_val = buf[100+4];
				ack_seq |= (unsigned int)c_val << 8;
				c_val = buf[100+5];
				ack_seq |= (unsigned int)c_val << 0;

				aotg_bk_ep_tcpseq[aotg_bk_ep_idx] = tcp_seq;
				aotg_bk_ep_ackseq[aotg_bk_ep_idx] = ack_seq;
			}
		}

	} else {
#if 0
		u32_buf = (unsigned int *)buf;
		if (*u32_buf == 0) {
			ACT_HCD_ERR
			//printk("last_dma_len:%d\n", p_aotg_hcd0->last_bulkin_dma_len);
		}
#endif
#if 0	/* test rtw's data payload right or not. */
		rtw_buf = buf;
		total_offset = 0; 

		if (urb->actual_length > 256) {
			agg_cnt = rtw_buf[10]; 

			do {
				pkt_len = rtw_buf[0] & 0xff;
				pkt_len |= (rtw_buf[1] & 0x3f) << 8;
				if (pkt_len <= 0) {
					ACT_HCD_ERR
					need_dump = 1;
					goto dump_rtw_data;
				}

				pkt_len = pkt_len + 24 + 32;
				if (pkt_len & 0x7f) {
					pkt_len = pkt_len & (~0x7f);
					pkt_len += 0x80;
				}

				total_offset += pkt_len;

				agg_cnt--;
				if (agg_cnt <= 0) {
					agg_cnt = rtw_buf[10]; 
				}
				rtw_buf = buf + total_offset;
			}
			while ((agg_cnt > 0) && (urb->actual_length > total_offset));

			if (total_offset > urb->actual_length + 10) {
				ACT_HCD_ERR
				need_dump = 1;
			}

dump_rtw_data:
			if (need_dump) {
				printk("urb->actual_length:%d\n", urb->actual_length);
				printk("urb->transfer_dma:%x\n", urb->transfer_dma);
				printk("buf:%x\n", (unsigned int)urb->transfer_buffer);
				printk("rtw_buf:%x\n", (unsigned int)rtw_buf);

				rtw_buf = buf;
				for (i = 0; i < urb->actual_length; i++) {
					if ((i%10) == 0)
						printk("\n");
					printk("%2x ", (unsigned int)*rtw_buf);
					rtw_buf++;
				}

				printk("\n");
				printk("=================== dma to virt ==================\n");
				printk("\n");

				rtw_buf = (char *)dma_to_virt(NULL, urb->transfer_dma);
				printk("rtw_buf:%x\n", (unsigned int)rtw_buf);
				if (rtw_buf != NULL) {
					for (i = 0; i < urb->actual_length; i++) {
						if ((i%10) == 0)
							printk("\n");
						printk("%2x ", (unsigned int)*rtw_buf);
						rtw_buf++;
					}
				}

				printk("\n");
				printk("\n");
				aotg_dbg_proc_output_ep();
				//aotg_dbg_regs(p_aotg_hcd0);
				aotg_dbg_output_info();
				BUG_ON(1);
			}
		}
#endif
		if (urb->actual_length > 800000) {
		//if (urb->actual_length > 80) {
			if (((buf[56] & 0xf) == 0x8) &&
			    (buf[82] == 0xaa) &&		// preemble mac
			    (buf[83] == 0xaa) &&		// 
			    (buf[88] == 0x8) &&			// ip type. 
			    (buf[89] == 0x0) &&			// 
			    (buf[99] == 0x6)) 			// tcp type
			{	// tcp type
				tcp_seq = 0;
				c_val = buf[112+2];
				tcp_seq |= (unsigned int)c_val << 24;
				c_val = buf[112+3];
				tcp_seq |= (unsigned int)c_val << 16;
				c_val = buf[112+4];
				tcp_seq |= (unsigned int)c_val << 8;
				c_val = buf[112+5];
				tcp_seq |= (unsigned int)c_val << 0;

				//aotg_dbg_put_info("in_tcpseq_complete", __LINE__, tcp_seq, 0);
				ack_seq = 0;
				c_val = buf[116+2];
				ack_seq |= (unsigned int)c_val << 24;
				c_val = buf[116+3];
				ack_seq |= (unsigned int)c_val << 16;
				c_val = buf[116+4];
				ack_seq |= (unsigned int)c_val << 8;
				c_val = buf[116+5];
				ack_seq |= (unsigned int)c_val << 0;

				//aotg_dbg_put_info("in_ackseq_complete", urb->actual_length, ack_seq, 0);

				aotg_bk_ep_tcpseq[aotg_bk_ep_idx] = tcp_seq;
				aotg_bk_ep_ackseq[aotg_bk_ep_idx] = ack_seq;
			}
			
			if (aotg_hcd_proc_sign == 'e') {
			/* in arp packet. */
#if 1
			if ((buf[82] == 0xaa) &&		// preemble mac
			    (buf[83] == 0xaa) &&		// 
			    (buf[88] == 0x8) &&			// arp type. 
			    (buf[89] == 0x6)) {
				printk("====arp--recv hw:%x %x, pro:%x %x ",  
					(unsigned int)buf[90+0], (unsigned int)buf[90+1],
					(unsigned int)buf[90+2], (unsigned int)buf[90+3]);
				printk(" hw len:%x, ip len: %x ",  (unsigned int)buf[90+4], (unsigned int)buf[90+5]);
				printk(" op:%x %x ",  (unsigned int)buf[90+6], (unsigned int)buf[90+7]);
				printk("\n");
				printk("    outhw_src:%x %x %x %x %x %x, ip:%d %d %d %d ",  
					(unsigned int)buf[90+8], (unsigned int)buf[90+9],
					(unsigned int)buf[90+10], (unsigned int)buf[90+11],
					(unsigned int)buf[90+12], (unsigned int)buf[90+13],
					(unsigned int)buf[90+14], (unsigned int)buf[90+15],
					(unsigned int)buf[90+16], (unsigned int)buf[90+17]);
				printk("    outhw_dst:%x %x %x %x %x %x, ip:%d %d %d %d ",  
					(unsigned int)buf[90+18], (unsigned int)buf[90+19],
					(unsigned int)buf[90+20], (unsigned int)buf[90+21],
					(unsigned int)buf[90+22], (unsigned int)buf[90+23],
					(unsigned int)buf[90+24], (unsigned int)buf[90+25],
					(unsigned int)buf[90+26], (unsigned int)buf[90+27]);
				printk("\n");
			}
#endif
			/* in icmp packet. */
#if 1
			if ((buf[82] == 0xaa) &&		// preemble mac
			    (buf[83] == 0xaa) &&		// 
			    (buf[88] == 0x8) &&			// ip type. 
			    (buf[89] == 0x0) &&			// 
			    (buf[99] == 0x1)) {			// icmp type
				printk("icmp--recv type:%x %x \n",  
					(unsigned int)buf[110], (unsigned int)buf[111]);
			}
#endif
			}
		}
	}
#endif

	if (((aotg_hcd_proc_sign == 't') && (usb_pipein(urb->pipe) == 0)) ||
	    ((aotg_hcd_proc_sign == 'f') && (usb_pipein(urb->pipe) != 0)))
	{
		buf = urb->transfer_buffer;

		printk("-fi: %d\n", urb->actual_length);
		//printk("ep->epnum:%d ", ep->epnum);
		//printk("in:%d ", usb_pipein(urb->pipe));
		//printk("seq:%d ", q->seq_info);
#if 1
		for (i = 0; i < urb->actual_length; i++) {
			if ((i%32) == 0)
				printk("\n");
			printk("%2x ", (unsigned int)*buf);
			buf++;
		}
#endif
		buf = urb->transfer_buffer;

#if 0
		if (((buf[56] & 0xf) == 0x8) &&
		    (buf[82] == 0xaa) &&		// preemble mac
		    (buf[83] == 0xaa)) {
			printk("data:");
			for (i = 0; i < urb->actual_length; i++) {
				if ((i%32) == 0)
					printk("\n");
				printk("%2x ", (unsigned int)*buf);
				buf++;
			}
		}
		if ((urb->actual_length == 158) || (urb->actual_length == 102)) {
			printk("data:");
			for (i = 0; i < urb->actual_length; i++) {
				if ((i%32) == 0)
					printk("\n");
				printk("%2x ", (unsigned int)*buf);
				buf++;
			}
		}
#endif
	}
	aotg_bk_ep_num[aotg_bk_ep_idx] = usb_pipeendpoint(q->urb->pipe);
	aotg_bk_ep_type[aotg_bk_ep_idx] = usb_pipein(q->urb->pipe);
	aotg_bk_ep_len[aotg_bk_ep_idx] = urb->actual_length;
	aotg_bk_ep_seq[aotg_bk_ep_idx] = q->seq_info;
	aotg_bk_ep_inout[aotg_bk_ep_idx] = 0;
	aotg_bk_ep_buf[aotg_bk_ep_idx] = (unsigned int)urb->transfer_buffer;

	aotg_bk_ep_idx++;
	if (aotg_bk_ep_idx >= AOTG_BK_URB_CNT)
		aotg_bk_ep_idx = 0;

	//aotg_bk_check_ep_seq();
	return;
}

void aotg_dbg_proc_output_ep(void)
{
	int i;

	for (i = 0; i < AOTG_BK_URB_CNT; i++) {
		if (aotg_bk_ep_inout[aotg_bk_ep_idx]) {
			if (aotg_bk_ep_type[aotg_bk_ep_idx] == 0) {
				printk("          ");
			}
			printk("----i:%d, is_in: %d, num:%d, len:%d, seq:%d, tcpseq:0x%x, ackseq:0x%x, buf:0x%x\n", 
				i, 
				aotg_bk_ep_type[aotg_bk_ep_idx],
				aotg_bk_ep_num[aotg_bk_ep_idx],
				aotg_bk_ep_len[aotg_bk_ep_idx],
				aotg_bk_ep_seq[aotg_bk_ep_idx],
				aotg_bk_ep_tcpseq[aotg_bk_ep_idx],
				aotg_bk_ep_ackseq[aotg_bk_ep_idx],
				aotg_bk_ep_buf[aotg_bk_ep_idx]);
		} else {
			if (aotg_bk_ep_type[aotg_bk_ep_idx] == 0) {
				printk("          ");
			}
			printk("    i:%d, is_in: %d, num:%d, len:%d, seq:%d, tcpseq:0x%x, ackseq:0x%x, buf:0x%x\n",
				i, 
				aotg_bk_ep_type[aotg_bk_ep_idx],
				aotg_bk_ep_num[aotg_bk_ep_idx],
				aotg_bk_ep_len[aotg_bk_ep_idx],
				aotg_bk_ep_seq[aotg_bk_ep_idx],
				aotg_bk_ep_tcpseq[aotg_bk_ep_idx],
				aotg_bk_ep_ackseq[aotg_bk_ep_idx],
				aotg_bk_ep_buf[aotg_bk_ep_idx]);
		}
		aotg_bk_ep_idx++;
		if (aotg_bk_ep_idx >= AOTG_BK_URB_CNT)
			aotg_bk_ep_idx = 0;
	}
	return;
}

#define AOTG_DEBUG_INFO_CNT		20
static int aotg_dbg_idx = 0;
static char aotg_dbg_strings[AOTG_DEBUG_INFO_CNT][8];
static char* aotg_dbg_info0[AOTG_DEBUG_INFO_CNT];
static unsigned int aotg_dbg_info1[AOTG_DEBUG_INFO_CNT];
static unsigned int aotg_dbg_info2[AOTG_DEBUG_INFO_CNT];
static unsigned int aotg_dbg_info3[AOTG_DEBUG_INFO_CNT];
void aotg_dbg_put_info(char *info0, unsigned int info1, unsigned int info2, unsigned int info3)
{
	unsigned long flags;

	local_irq_save(flags);
	strncpy(&aotg_dbg_strings[aotg_dbg_idx][0], info0, 7);
	aotg_dbg_strings[aotg_dbg_idx][7] = 0;
	aotg_dbg_info0[aotg_dbg_idx] = info0;
	aotg_dbg_info1[aotg_dbg_idx] = info1;
	aotg_dbg_info2[aotg_dbg_idx] = info2;
	aotg_dbg_info3[aotg_dbg_idx] = info3;
	aotg_dbg_idx++;
	if (aotg_dbg_idx >= AOTG_DEBUG_INFO_CNT)
		aotg_dbg_idx = 0;
	local_irq_restore(flags);
	return;
}

void aotg_dbg_output_info(void)
{
	int i;
	unsigned long flags;

	local_irq_save(flags);

	for (i = 0; i < AOTG_DEBUG_INFO_CNT; i++) {
		printk("i:%d  ", i);
		printk("strins:%s: ", &aotg_dbg_strings[aotg_dbg_idx][0]);
		printk("info0:%s: ", aotg_dbg_info0[aotg_dbg_idx]);
		printk("info1:%d, 0x%x; ", aotg_dbg_info1[aotg_dbg_idx], aotg_dbg_info1[aotg_dbg_idx]);
		printk("info2:%d, 0x%x; ", aotg_dbg_info2[aotg_dbg_idx], aotg_dbg_info2[aotg_dbg_idx]);
		printk("info3:%d, 0x%x; \n", aotg_dbg_info3[aotg_dbg_idx], aotg_dbg_info3[aotg_dbg_idx]);
		aotg_dbg_idx++;
		if (aotg_dbg_idx >= AOTG_DEBUG_INFO_CNT)
			aotg_dbg_idx = 0;
	}
	local_irq_restore(flags);
	return;
}

#else	/* AOTG_DEBUG_RECORD_URB */

void aotg_dbg_proc_output_ep(void)
{
	return;
}

void aotg_dbg_put_info(char *info0, unsigned int info1, unsigned int info2, unsigned int info3)
{
	return;
}

void aotg_dbg_output_info(void)
{
	return;
}

void aotg_dbg_put_q(struct aotg_queue *q, unsigned int num, unsigned int type, unsigned int len)
{
	return;
}

void aotg_dbg_finish_q(struct aotg_queue *q)
{
	return;
}

#endif	/* AOTG_DEBUG_RECORD_URB */


#ifdef AOTG_REG_DUMP
void aotg_dbg_regs(struct aotg_hcd *acthcd)
{
	dev_info(acthcd->dev, "============== aotg regs ==================\n");
#if 1
	printk(" OTGIRQ(0x%p) : 0x%X\n",
            acthcd->base + OTGIRQ, usb_readb(acthcd->base + OTGIRQ));
	printk(" OTGSTATE(0x%p) : 0x%X\n",
            acthcd->base + OTGSTATE, usb_readb(acthcd->base + OTGSTATE));
	printk(" OTGCTRL(0x%p) : 0x%X\n",
            acthcd->base + OTGCTRL, usb_readb(acthcd->base + OTGCTRL));
	printk(" OTGSTATUS(0x%p) : 0x%X\n",
            acthcd->base + OTGSTATUS, usb_readb(acthcd->base + OTGSTATUS));
	printk(" OTGIEN(0x%p) : 0x%X\n",
            acthcd->base + OTGIEN, usb_readb(acthcd->base + OTGIEN));
	printk("\n");
	printk(" USBEIRQ(0x%p) : 0x%X\n",
            acthcd->base + USBEIRQ, usb_readb(acthcd->base + USBEIRQ));
	printk(" BKDOOR(0x%p) : 0x%X\n",
            acthcd->base + BKDOOR, usb_readb(acthcd->base + BKDOOR));
	printk(" USBIRQ(0x%p) : 0x%X\n",
            acthcd->base + USBIRQ, usb_readb(acthcd->base + USBIRQ));
	printk(" USBIEN(0x%p) : 0x%X\n",
            acthcd->base + USBIEN, usb_readb(acthcd->base + USBIEN));
	printk("\n");
#endif
	printk(" HCINXSTART(0x%p) : 0x%X\n",
            acthcd->base + HCINXSTART, usb_readb(acthcd->base + HCINXSTART));
	printk(" OUTXNAKCTRL(0x%p) : 0x%X\n",
            acthcd->base + OUTXNAKCTRL, usb_readb(acthcd->base + OUTXNAKCTRL));
	printk(" OUTXSHORTPCKIRQ(0x%p) : 0x%X\n",
            acthcd->base + OUTXSHORTPCKIRQ, usb_readb(acthcd->base + OUTXSHORTPCKIRQ));

	printk(" HCOUT07IRQ(0x%p) : 0x%X\n",
            acthcd->base + HCOUT07IRQ, usb_readb(acthcd->base + HCOUT07IRQ));
	printk(" HCOUT07IEN(0x%p) : 0x%X\n",
            acthcd->base + HCOUT07IEN, usb_readb(acthcd->base + HCOUT07IEN));
	printk(" HCIN07IRQ(0x%p) : 0x%X\n",
            acthcd->base + HCIN07IRQ, usb_readb(acthcd->base + HCIN07IRQ));
	printk(" HCIN07IEN(0x%p) : 0x%X\n",
            acthcd->base + HCIN07IEN, usb_readb(acthcd->base + HCIN07IEN));
	printk("\n");
#if 1
	printk(" HCIN0BC(0x%p) : 0x%X\n",
            acthcd->base + HCIN0BC, usb_readb(acthcd->base + HCIN0BC));
	printk(" HCEP0CS(0x%p) : 0x%X\n",
            acthcd->base + HCEP0CS, usb_readb(acthcd->base + HCEP0CS));
	printk(" HCOUT0BC(0x%p) : 0x%X\n",
            acthcd->base + HCOUT0BC, usb_readb(acthcd->base + HCOUT0BC));
	printk(" HCEP0CTRL(0x%p) : 0x%X\n",
            acthcd->base + HCEP0CTRL, usb_readb(acthcd->base + HCEP0CTRL));
	printk("\n");
	printk(" HCIN1BC(0x%p) : 0x%X\n",
            acthcd->base + HCIN1BCL, usb_readw(acthcd->base + HCIN1BCL));
	printk(" HCIN1CON(0x%p) : 0x%X\n",
            acthcd->base + HCIN1CON, usb_readb(acthcd->base + HCIN1CON));
	printk(" HCIN1CS(0x%p) : 0x%X\n",
            acthcd->base + HCIN1CS, usb_readb(acthcd->base + HCIN1CS));
	printk(" HCIN1CTRL(0x%p) : 0x%X\n",
            acthcd->base + HCIN1CTRL, usb_readb(acthcd->base + HCIN1CTRL));
	printk(" HCIN2BC(0x%p) : 0x%X\n",
            acthcd->base + HCIN2BCL, usb_readw(acthcd->base + HCIN2BCL));
	printk(" HCIN2CON(0x%p) : 0x%X\n",
            acthcd->base + HCIN2CON, usb_readb(acthcd->base + HCIN2CON));
	printk(" HCIN2CS(0x%p) : 0x%X\n",
            acthcd->base + HCIN2CS, usb_readb(acthcd->base + HCIN2CS));
	printk(" HCIN2CTRL(0x%p) : 0x%X\n",
            acthcd->base + HCIN2CTRL, usb_readb(acthcd->base + HCIN2CTRL));
	printk("\n");
	printk(" HCIN1_COUNTL(0x%p) : 0x%X\n",
            acthcd->base + HCIN1_COUNTL, usb_readw(acthcd->base + HCIN1_COUNTL));
	printk(" HCIN2_COUNTL(0x%p) : 0x%X\n",
            acthcd->base + HCIN2_COUNTL, usb_readw(acthcd->base + HCIN2_COUNTL));
	//printk(" HCINCTRL(0x%p) : 0x%X\n", acthcd->base + HCINCTRL, usb_readb(acthcd->base + HCINCTRL));
	printk("\n");
#endif
	printk(" HCOUT1BC(0x%p) : 0x%X\n",
            acthcd->base + HCOUT1BCL, usb_readw(acthcd->base + HCOUT1BCL));
	printk(" HCOUT1CON(0x%p) : 0x%X\n",
            acthcd->base + HCOUT1CON, usb_readb(acthcd->base + HCOUT1CON));
	printk(" HCOUT1CS(0x%p) : 0x%X\n",
            acthcd->base + HCOUT1CS, usb_readb(acthcd->base + HCOUT1CS));
	printk(" HCOUT1CTRL(0x%p) : 0x%X\n",
            acthcd->base + HCOUT1CTRL, usb_readb(acthcd->base + HCOUT1CTRL));
	printk(" HCOUT1ERR(0x%p) : 0x%X\n",
	    acthcd->base + HCOUT1ERR, usb_readb(acthcd->base + HCOUT1ERR));
	printk(" HCOUT1STARTADDRESS(0x%p) : 0x%X\n",
	    acthcd->base + IN1STARTADDRESS, usb_readl(acthcd->base + IN1STARTADDRESS));
	printk(" HCOUT2BC(0x%p) : 0x%X\n",
            acthcd->base + HCOUT2BCL, usb_readw(acthcd->base + HCOUT2BCL));
	printk(" HCOUT2CON(0x%p) : 0x%X\n",
            acthcd->base + HCOUT2CON, usb_readb(acthcd->base + HCOUT2CON));
	printk(" HCOUT2CS(0x%p) : 0x%X\n",
            acthcd->base + HCOUT2CS, usb_readb(acthcd->base + HCOUT2CS));
	printk(" HCOUT2CTRL(0x%p) : 0x%X\n",
            acthcd->base + HCOUT2CTRL, usb_readb(acthcd->base + HCOUT2CTRL));
	printk("HCOUT2ERR(0x%p) : 0x%X\n",
	    	acthcd->base + HCOUT2ERR, usb_readb(acthcd->base + HCOUT2ERR));
	printk(" HCOUT2STARTADDRESS(0x%p) : 0x%X\n",
	    acthcd->base + IN2STARTADDRESS, usb_readl(acthcd->base + IN2STARTADDRESS));
	printk("\n");

	printk(" UDMAIRQ: 0x%X\n", usb_readb(acthcd->base + UDMAIRQ));
	printk(" UDMAIEN: 0x%X\n", usb_readb(acthcd->base + UDMAIEN));
	printk(" UDMA1MEMADDR: 0x%X\n", usb_readl(acthcd->base + UDMA1MEMADDR));
	printk(" UDMA1EPSEL: 0x%X\n", usb_readb(acthcd->base + UDMA1EPSEL));
	printk(" UDMA1COM: 0x%X\n", usb_readb(acthcd->base + UDMA1COM));
	printk(" UDMA1CNTL: 0x%X\n", usb_readb(acthcd->base + UDMA1CNTL));
	printk(" UDMA1CNTM: 0x%X\n", usb_readb(acthcd->base + UDMA1CNTM));
	printk(" UDMA1CNTH: 0x%X\n", usb_readb(acthcd->base + UDMA1CNTH));
	printk(" UDMA1REML: 0x%X\n", usb_readb(acthcd->base + UDMA1REML));
	printk(" UDMA1REMM: 0x%X\n", usb_readb(acthcd->base + UDMA1REMM));
	printk(" UDMA1REMH: 0x%X\n", usb_readb(acthcd->base + UDMA1REMH));

	printk(" UDMA2MEMADDR: 0x%X\n", usb_readl(acthcd->base + UDMA2MEMADDR));
	printk(" UDMA2EPSEL: 0x%X\n", usb_readb(acthcd->base + UDMA2EPSEL));
	printk(" UDMA2COM: 0x%X\n", usb_readb(acthcd->base + UDMA2COM));
	printk(" UDMA2CNTL: 0x%X\n", usb_readb(acthcd->base + UDMA2CNTL));
	printk(" UDMA2CNTM: 0x%X\n", usb_readb(acthcd->base + UDMA2CNTM));
	printk(" UDMA2CNTH: 0x%X\n", usb_readb(acthcd->base + UDMA2CNTH));
	printk(" UDMA2REML: 0x%X\n", usb_readb(acthcd->base + UDMA2REML));
	printk(" UDMA2REMM: 0x%X\n", usb_readb(acthcd->base + UDMA2REMM));
	printk(" UDMA2REMH: 0x%X\n", usb_readb(acthcd->base + UDMA2REMH));
	printk("\n");
	return;
}

#else	/* AOTG_REG_DUMP */

void aotg_dbg_regs(struct aotg_hcd *acthcd)
{
    return;
}

#endif	/* AOTG_REG_DUMP */


#ifdef AOTG_DEBUG_FILE

void aotg_dbg_proc_output_ep_state(struct aotg_hcd *acthcd)
{
	struct aotg_hcep *tmp_ep;
	int i;
	unsigned int pipe;
	struct aotg_queue *q, *next;
	struct aotg_hcep *ep;
	struct urb *urb;

	printk("dma_working[0]:%x\n", acthcd->dma_working[0]);
	if (acthcd->dma_working[0]) {
		pipe = acthcd->dma_working[0];
		printk("is_out:%d epnum:%d\n", !usb_pipein(pipe), usb_pipeendpoint(pipe));
	}
	printk("dma_working[1]:%x\n", acthcd->dma_working[1]);
	if (acthcd->dma_working[1]) {
		pipe = acthcd->dma_working[1];
		printk("is_out:%d epnum:%d\n", !usb_pipein(pipe), usb_pipeendpoint(pipe));
	}

	for (i = 1; i < MAX_EP_NUM; i++) {
		tmp_ep = acthcd->inep[i];
		if (tmp_ep) {
			if (tmp_ep->urb_enque_cnt > (tmp_ep->urb_endque_cnt + tmp_ep->urb_stop_stran_cnt)) {
				printk("inep:%d\n", i);
				printk("urb_enque_cnt:%d\n", tmp_ep->urb_enque_cnt);
				printk("urb_endque_cnt:%d\n", tmp_ep->urb_endque_cnt);
				printk("urb_stop_stran_cnt:%d\n", tmp_ep->urb_stop_stran_cnt);
				printk("urb_unlinked_cnt:%d\n", tmp_ep->urb_unlinked_cnt);

				printk("index:%d\n", tmp_ep->index);
				printk("maxpacket:%d\n", tmp_ep->maxpacket);
				printk("epnum:%d\n", tmp_ep->epnum);
			}
		}
	}
	for (i = 1; i < MAX_EP_NUM; i++) {
		tmp_ep = acthcd->outep[i];
		if (tmp_ep) {
			if (tmp_ep->urb_enque_cnt > (tmp_ep->urb_endque_cnt + tmp_ep->urb_stop_stran_cnt)) {
				printk("outep:%d\n", i);
				printk("urb_enque_cnt:%d\n", tmp_ep->urb_enque_cnt);
				printk("urb_endque_cnt:%d\n", tmp_ep->urb_endque_cnt);
				printk("urb_stop_stran_cnt:%d\n", tmp_ep->urb_stop_stran_cnt);
				printk("urb_unlinked_cnt:%d\n", tmp_ep->urb_unlinked_cnt);

				printk("index:%d\n", tmp_ep->index);
				printk("maxpacket:%d\n", tmp_ep->maxpacket);
				printk("epnum:%d\n", tmp_ep->epnum);
			}
		}
	}

	printk("in hcd enqueue list: \n");
	list_for_each_entry_safe(q, next, &acthcd->hcd_enqueue_list, enqueue_list) {
		urb = q->urb;
		ep = q->ep;

		printk("ep->epnum:%d ", ep->epnum);
		printk("urb->transfer_buffer_length:%d ", urb->transfer_buffer_length);
		printk("usb_pipein(urb->pipe):%x\n", usb_pipein(urb->pipe));
		printk("usb_pipetype(urb->pipe):%x\n", usb_pipetype(urb->pipe));
	}
	return;
}

static int aotg_hcd_show_enque_info(struct seq_file *s, struct aotg_hcd	*acthcd)
{
	int i;
	struct aotg_queue *q, *next;
	struct aotg_hcep *ep;

	for (i = 0; i < AOTG_QUEUE_POOL_CNT; i++) {
		if (acthcd->queue_pool[i] != NULL) {
			seq_printf(s, "queue_pool[%d]->in_using: %d\n", 
			      	i, acthcd->queue_pool[i]->in_using);
		} 
	}

	seq_printf(s, "current dma queue: \n");
	seq_printf(s, "dma_working[0]: %x\n", acthcd->dma_working[0]);
	seq_printf(s, "dma_working[1]: %x\n", acthcd->dma_working[1]);

	list_for_each_entry_safe(q, next, &acthcd->dma_queue[0], dma_q_list) {
		seq_printf(s, "dma[0]: ep->index: %d, type: %d, dir : %s, transfer_buffer_length: %d, actual_length:%d\n",
			q->ep->index,
			usb_pipetype(q->urb->pipe), usb_pipeout(q->urb->pipe)?"out":"in",
			q->urb->transfer_buffer_length, q->urb->actual_length);
	}
	list_for_each_entry_safe(q, next, &acthcd->dma_queue[1], dma_q_list) {
		seq_printf(s, "dma[1]: ep->index: %d, type: %d, dir : %s, transfer_buffer_length: %d, actual_length:%d\n",
			q->ep->index,
			usb_pipetype(q->urb->pipe), usb_pipeout(q->urb->pipe)?"out":"in",
			q->urb->transfer_buffer_length, q->urb->actual_length);
	}
	seq_printf(s, "\n");

	ep = acthcd->ep0;
	if (ep) {
		seq_printf(s, "------------- current ep0 queue: \n");
		seq_printf(s, "urb_enque_cnt:%d\n", ep->urb_enque_cnt);
		seq_printf(s, "urb_endque_cnt:%d\n", ep->urb_endque_cnt);
		seq_printf(s, "urb_stop_stran_cnt:%d\n", ep->urb_stop_stran_cnt);
		seq_printf(s, "urb_unlinked_cnt:%d\n", ep->urb_unlinked_cnt);

		list_for_each_entry_safe(q, next, &ep->q_list, ep_q_list) {
			seq_printf(s, "ep->index: %d, type: %d, dir : %s, transfer_buffer_length: %d, actual_length:%d\n",
				q->ep->index,
				usb_pipetype(q->urb->pipe), usb_pipeout(q->urb->pipe)?"out":"in",
				q->urb->transfer_buffer_length, q->urb->actual_length);
		}
	}
	
	for (i = 1; i < MAX_EP_NUM; i++) {
		ep = acthcd->inep[i];
		if (ep) {
			seq_printf(s, "------------- current IN ep%d queue: \n", i);
			seq_printf(s, "urb_enque_cnt:%d\n", ep->urb_enque_cnt);
			seq_printf(s, "urb_endque_cnt:%d\n", ep->urb_endque_cnt);
			seq_printf(s, "urb_stop_stran_cnt:%d\n", ep->urb_stop_stran_cnt);
			seq_printf(s, "urb_unlinked_cnt:%d\n", ep->urb_unlinked_cnt);
			seq_printf(s, "ep->epnum:%d\n", ep->epnum);

			list_for_each_entry_safe(q, next, &ep->q_list, ep_q_list) {
				seq_printf(s, "ep->index: %d, type: %d, dir : %s, transfer_buffer_length: %d, actual_length:%d\n",
					q->ep->index,
					usb_pipetype(q->urb->pipe), usb_pipeout(q->urb->pipe)?"out":"in",
					q->urb->transfer_buffer_length, q->urb->actual_length);
			}
		}
	}
	
	for (i = 1; i < MAX_EP_NUM; i++) {
		ep = acthcd->outep[i];
		if (ep) {
			seq_printf(s, "------------- current OUT ep%d queue: \n", i);
			seq_printf(s, "urb_enque_cnt:%d\n", ep->urb_enque_cnt);
			seq_printf(s, "urb_endque_cnt:%d\n", ep->urb_endque_cnt);
			seq_printf(s, "urb_stop_stran_cnt:%d\n", ep->urb_stop_stran_cnt);
			seq_printf(s, "urb_unlinked_cnt:%d\n", ep->urb_unlinked_cnt);
			seq_printf(s, "ep->epnum:%d\n", ep->epnum);

			list_for_each_entry_safe(q, next, &ep->q_list, ep_q_list) {
				seq_printf(s, "ep->index: %d, type: %d, dir : %s, transfer_buffer_length: %d, actual_length:%d\n",
					q->ep->index,
					usb_pipetype(q->urb->pipe), usb_pipeout(q->urb->pipe)?"out":"in",
					q->urb->transfer_buffer_length, q->urb->actual_length);
			}
		}
	}

	seq_printf(s, "\n");
	seq_printf(s, "in hcd enqueue list: \n");
	list_for_each_entry_safe(q, next, &acthcd->hcd_enqueue_list, enqueue_list) {
		ep = q->ep;
		seq_printf(s, "ep->epnum:%d ", ep->epnum);
		seq_printf(s, "urb->transfer_buffer_length:%d ", q->urb->transfer_buffer_length);
		seq_printf(s, "usb_pipein(urb->pipe):%x\n", usb_pipein(q->urb->pipe));
		seq_printf(s, "usb_pipetype(urb->pipe):%x\n", usb_pipetype(q->urb->pipe));
	}
	return 0;
}

void aotg_hcd_show_ep_info(struct aotg_hcd *acthcd)
{
	int i;
	struct aotg_queue *q, *next;
	struct aotg_hcep *ep;

	for (i = 0; i < AOTG_QUEUE_POOL_CNT; i++) {
		if (acthcd->queue_pool[i] != NULL) {
			printk("queue_pool[%d]->in_using: %d\n", 
			      	i, acthcd->queue_pool[i]->in_using);
		} 
	}

	printk("current dma queue: \n");
	printk("dma_working[0]: %x\n", acthcd->dma_working[0]);
	printk("dma_working[1]: %x\n", acthcd->dma_working[1]);

	list_for_each_entry_safe(q, next, &acthcd->dma_queue[0], dma_q_list) {
		printk("dma[0]: ep->index: %d, type: %d, dir : %s, transfer_buffer_length: %d, actual_length:%d\n",
			q->ep->index,
			usb_pipetype(q->urb->pipe), usb_pipeout(q->urb->pipe)?"out":"in",
			q->urb->transfer_buffer_length, q->urb->actual_length);
	}
	list_for_each_entry_safe(q, next, &acthcd->dma_queue[1], dma_q_list) {
		printk("dma[1]: ep->index: %d, type: %d, dir : %s, transfer_buffer_length: %d, actual_length:%d\n",
			q->ep->index,
			usb_pipetype(q->urb->pipe), usb_pipeout(q->urb->pipe)?"out":"in",
			q->urb->transfer_buffer_length, q->urb->actual_length);
	}
	printk("\n");

	ep = acthcd->ep0;
	if (ep) {
		printk("------------- current ep0 queue: \n");
		printk("urb_enque_cnt:%d\n", ep->urb_enque_cnt);
		printk("urb_endque_cnt:%d\n", ep->urb_endque_cnt);
		printk("urb_stop_stran_cnt:%d\n", ep->urb_stop_stran_cnt);
		printk("urb_unlinked_cnt:%d\n", ep->urb_unlinked_cnt);

		list_for_each_entry_safe(q, next, &ep->q_list, ep_q_list) {
			printk("ep->index: %d, type: %d, dir : %s, transfer_buffer_length: %d, actual_length:%d\n",
				q->ep->index,
				usb_pipetype(q->urb->pipe), usb_pipeout(q->urb->pipe)?"out":"in",
				q->urb->transfer_buffer_length, q->urb->actual_length);
		}
	}
	
	for (i = 1; i < MAX_EP_NUM; i++) {
		ep = acthcd->inep[i];
		if (ep) {
			printk("------------- current IN ep%d queue: \n", i);
			printk("urb_enque_cnt:%d\n", ep->urb_enque_cnt);
			printk("urb_endque_cnt:%d\n", ep->urb_endque_cnt);
			printk("urb_stop_stran_cnt:%d\n", ep->urb_stop_stran_cnt);
			printk("urb_unlinked_cnt:%d\n", ep->urb_unlinked_cnt);
			printk("ep->epnum:%d\n", ep->epnum);

			list_for_each_entry_safe(q, next, &ep->q_list, ep_q_list) {
				printk("ep->index: %d, type: %d, dir : %s, transfer_buffer_length: %d, actual_length:%d\n",
					q->ep->index,
					usb_pipetype(q->urb->pipe), usb_pipeout(q->urb->pipe)?"out":"in",
					q->urb->transfer_buffer_length, q->urb->actual_length);
			}
		}
	}
	
	for (i = 1; i < MAX_EP_NUM; i++) {
		ep = acthcd->outep[i];
		if (ep) {
			printk("------------- current OUT ep%d queue: \n", i);
			printk("urb_enque_cnt:%d\n", ep->urb_enque_cnt);
			printk("urb_endque_cnt:%d\n", ep->urb_endque_cnt);
			printk("urb_stop_stran_cnt:%d\n", ep->urb_stop_stran_cnt);
			printk("urb_unlinked_cnt:%d\n", ep->urb_unlinked_cnt);
			printk("ep->epnum:%d\n", ep->epnum);

			list_for_each_entry_safe(q, next, &ep->q_list, ep_q_list) {
				printk("ep->index: %d, type: %d, dir : %s, transfer_buffer_length: %d, actual_length:%d\n",
					q->ep->index,
					usb_pipetype(q->urb->pipe), usb_pipeout(q->urb->pipe)?"out":"in",
					q->urb->transfer_buffer_length, q->urb->actual_length);
			}
		}
	}

	printk("\n");
	printk("in hcd enqueue list: \n");
	list_for_each_entry_safe(q, next, &acthcd->hcd_enqueue_list, enqueue_list) {
		ep = q->ep;
		printk("ep->epnum:%d ", ep->epnum);
		printk("urb->transfer_buffer_length:%d ", q->urb->transfer_buffer_length);
		printk("usb_pipein(urb->pipe):%x\n", usb_pipein(q->urb->pipe));
		printk("usb_pipetype(urb->pipe):%x\n", usb_pipetype(q->urb->pipe));
	}

	return 0;

}
	

/* 
 * echo a value to controll the cat /proc/aotg_hcd output content.
 * echo h>/proc/aotg_hcd.0 to see help info.
 */
ssize_t aotg_hcd_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char c = 'n';

	if (count) {
		if (get_user(c, buf))
			return -EFAULT;
		aotg_hcd_proc_sign = c;
	}
	if (c == 'h') {
		printk(" a ---- all.  \n");
		printk(" b ---- backup info.  \n");
		printk(" d ---- dma related.  \n");
		printk(" e ---- enque and outque info.  \n");
		printk(" f ---- trace in info.  \n");
		printk(" h ---- help info.  \n");
		printk(" n ---- normal.  \n");
		printk(" r ---- register info.  \n");
		printk(" s ---- aotg state.  \n");
		printk(" t ---- trace out info.  \n");
		printk(" z ---- stop stace.  \n");
	}
        return count;
}

int aotg_hcd_proc_show(struct seq_file *s, void *unused)
{
	struct aotg_hcd	*acthcd = s->private;
	struct usb_hcd *hcd = aotg_to_hcd(acthcd);
	//struct aotg_plat_data *data = acthcd->port_specific;
	int i;

	if (aotg_hcd_proc_sign == 'd') {
		i = aotg_dma_get_cmd(acthcd->dma_nr0);
		if (((i & 0x1) == 0) && (acthcd->dma_working[0] != 0)) {
			printk("i:%d\n", i);
		}
		i = aotg_dma_get_cmd(acthcd->dma_nr1);
		if ((i == 0) && (acthcd->dma_working[1] != 0)) {
			printk("i:%d\n", i);
		}

		if (acthcd->dma_nr0 >= 0) {
			seq_printf(s, "dma0 remain : %d\n", aotg_dma_get_remain(acthcd->dma_nr0));
			seq_printf(s, "aotg_dma_get_cmd : %d\n", aotg_dma_get_cmd(acthcd->dma_nr0));
			seq_printf(s, "dma1 remain : %d\n", aotg_dma_get_remain(acthcd->dma_nr1));
			seq_printf(s, "aotg_dma_get_cmd : %d\n", aotg_dma_get_cmd(acthcd->dma_nr1));
			seq_printf(s, "acthcd->dma_working[0] :%x\n", acthcd->dma_working[0]);
			seq_printf(s, "acthcd->dma_working[1] :%x\n", acthcd->dma_working[1]);
		}
	}

	if (aotg_hcd_proc_sign == 's') {
		aotg_dbg_proc_output_ep_state(acthcd);

		seq_printf(s, "hcd state : 0x%08X\n", hcd->state);
		seq_printf(s, "port status : 0x%08X\n", acthcd->port);
		seq_printf(s, "sof_kref : %d\n", acthcd->sof_kref);
	}

	if (aotg_hcd_proc_sign == 'r') {
		aotg_dbg_regs(acthcd);
	}

	if (aotg_hcd_proc_sign == 'e') {
		aotg_hcd_show_enque_info(s, acthcd);
	}

	if (aotg_hcd_proc_sign == 'b') {
		aotg_dbg_proc_output_ep();
		aotg_dbg_output_info();
	}

	if (aotg_hcd_proc_sign == 'a') {
	}

	seq_printf(s, "\n");
	return 0;
}


static int aotg_hcd_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, aotg_hcd_proc_show, PDE(inode)->data);
}

static const struct file_operations proc_ops = {
	.open		= aotg_hcd_proc_open,
	.read		= seq_read,
	.write		= aotg_hcd_proc_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void create_debug_file(struct aotg_hcd *acthcd)
{
	struct device *dev = aotg_to_hcd(acthcd)->self.controller;

	acthcd->pde = proc_create_data(dev_name(dev), 0, NULL, &proc_ops, acthcd);
	return;
}

void remove_debug_file(struct aotg_hcd *acthcd)
{
	struct device *dev = aotg_to_hcd(acthcd)->self.controller;
	
	if (acthcd->pde)
		remove_proc_entry(dev_name(dev), NULL);
	return;
}

#else	/* AOTG_DEBUG_FILE */

void create_debug_file(struct aotg_hcd *acthcd)
{
	return;
}

void remove_debug_file(struct aotg_hcd *acthcd)
{
	return;
}

#endif	/* AOTG_DEBUG_FILE */


void aotg_print_xmit_cnt(char * info, int cnt)
{
	if (aotg_hcd_proc_sign == 'e') {
		printk("%s cnt:%d\n", info, cnt);
	}
	//printk("\n");
	//aotg_dbg_proc_output_ep();
	//aotg_dbg_regs(p_aotg_hcd0);
	//aotg_dbg_output_info();

	return;
}
EXPORT_SYMBOL(aotg_print_xmit_cnt);


static struct proc_dir_entry *acts_hcd_pde = NULL; 

int acts_hcd_proc_show(struct seq_file *s, void *unused)
{
	seq_printf(s, "hcd_ports_en_ctrl: %d\n", hcd_ports_en_ctrl);
	return 0;
}

static int acts_hcd_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, acts_hcd_proc_show, PDE(inode)->data);
}

ssize_t acts_hcd_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char c = 'n';

	if (count) {
		if (get_user(c, buf))
			return -EFAULT;
	}
	if ((c >= '0') && (c <= '3')) { 
		hcd_ports_en_ctrl = c - '0';
		printk("hcd en:%d\n", hcd_ports_en_ctrl);
	}
	if (c == 'h') {
		printk(" num ---- 0-all enable, 1-usb0 enable, 2-usb1 enable, 3-reversed. \n");
		printk("o ---- hcd power on\n");
		printk("f ---- hcd power off\n");
		printk("a ---- hcd aotg0 add\n");
		printk("b ---- hcd aotg0 remove\n");
		printk("c ---- hcd aotg1 add\n");
		printk("d ---- hcd aotg1 remove\n");
	}

	if (c == 'a') {
		printk("hcd aotg0 add\n");
		aotg0_device_init(0);
	}
	if (c == 'b') {
		printk("hcd aotg0 remove\n");
		aotg0_device_exit(0);
	}

	if (c == 'c') {
		printk("hcd aotg1 add\n");
		aotg1_device_init(0);
	}
	if (c == 'd') {
		printk("hcd aotg1 remove\n");
		aotg1_device_exit(0);
	}
        return count;
}

static const struct file_operations acts_proc_ops = {
	.open		= acts_hcd_proc_open,
	.read		= seq_read,
	.write		= acts_hcd_proc_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void create_acts_hcd_proc(void)
{
	acts_hcd_pde = proc_create_data("acts_hcd", 0, NULL, &acts_proc_ops, acts_hcd_pde);
	return;
}

void remove_acts_hcd_proc(void)
{
	if (acts_hcd_pde) {
		remove_proc_entry("acts_hcd", NULL);
		acts_hcd_pde = NULL;
	}
	return;
}

