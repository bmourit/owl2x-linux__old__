

rt_usb.c


[ 5320.571954] RTMP_AllTimerListRelease: Cancel timer obj f7bc99bc!
[ 5320.571965] RTMP_AllTimerListRelease: Cancel timer obj f7b4983c!
[ 5320.571976] RTMP_AllTimerListRelease: Cancel timer obj f7b4980c!
[ 5320.571987] RTMP_AllTimerListRelease: Cancel timer obj f7b497dc!
[ 5320.571997] RTMP_AllTimerListRelease: Cancel timer obj f7b5df64!
[ 5320.621927] --->RTUSBCancelPendingBulkInIRP
[ 5320.621941] ep->index: 2
[ 5320.621948] aotg_hcd_urb_dequeue:2750, dbg!
[ 5320.621956] aotg_hcd_urb_dequeue:2751, dbg!
[ 5320.621964] ------------[ cut here ]------------
[ 5320.622010] WARNING: at /home/local/ACTIONS/houjingkun/gs702a_test/leopard/platform/drivers/usb/aotg_hcd_add_int/aotg_hcd.c:2752 aotg_hcd_urb_dequeue+0x500/0x6a0 [acts_hcd]()
[ 5320.622026] Modules linked in: mt7601Usta(O) acts_hcd(O) g_android(O) dwc3_actions(O) dwc3(O) act_vibrate(O) mc3210(O) ft5x06(O) atc260x_cap_gauge(O) gl5202_hdmi(O) gl5202_tvout(O) atv5202_vout(O) camera_drv(O) camera_gc0308(O) camera_hi253(O) atv5202_mmc(O) mmc_block(O) mmc_core(O) galcore(O) vce_drv(O) vde_drv(O) sndrv(O) atc260x_audio(O) monitor(O) atc260x_rtc(O) atc260x_power(O) atc260x_adckeypad(O) atc260x_onoff(O) atv5202_fb(O) atv5202_lcd(O) atv5202_backlight(O) atv5202_de(O) blk2drv(O) blk1drv(O) [last unloaded: acts_hcd]
[ 5320.622158] [<c0015e10>] (unwind_backtrace+0x0/0xf8) from [<c002ffb8>] (warn_slowpath_common+0x54/0x64)
[ 5320.622182] [<c002ffb8>] (warn_slowpath_common+0x54/0x64) from [<c002ffe4>] (warn_slowpath_null+0x1c/0x24)
[ 5320.622219] [<c002ffe4>] (warn_slowpath_null+0x1c/0x24) from [<bf45a3d4>] (aotg_hcd_urb_dequeue+0x500/0x6a0 [acts_hcd])
[ 5320.622276] [<bf45a3d4>] (aotg_hcd_urb_dequeue+0x500/0x6a0 [acts_hcd]) from [<c02a5f14>] (unlink1+0x20/0x2c)
[ 5320.622303] [<c02a5f14>] (unlink1+0x20/0x2c) from [<c02a6e84>] (usb_hcd_unlink_urb+0x4c/0x80)
[ 5320.622327] [<c02a6e84>] (usb_hcd_unlink_urb+0x4c/0x80) from [<c02a86a8>] (usb_kill_urb.part.9+0x2c/0xb8)
[ 5320.622725] [<c02a86a8>] (usb_kill_urb.part.9+0x2c/0xb8) from [<bf594464>] (RTUSBCancelPendingBulkInIRP+0xd4/0x1f8 [mt7601Usta])
[ 5320.623291] [<bf594464>] (RTUSBCancelPendingBulkInIRP+0xd4/0x1f8 [mt7601Usta]) from [<bf5947ec>] (RTUSBCancelPendingIRPs+0xc/0x18 [mt7601Usta])
[ 5320.623804] [<bf5947ec>] (RTUSBCancelPendingIRPs+0xc/0x18 [mt7601Usta]) from [<bf595b28>] (RtmpMgmtTaskExit+0x18/0xf8 [mt7601Usta])
[ 5320.624287] [<bf595b28>] (RtmpMgmtTaskExit+0x18/0xf8 [mt7601Usta]) from [<bf4b747c>] (rt28xx_init+0x2cc/0x898 [mt7601Usta])
[ 5320.624755] [<bf4b747c>] (rt28xx_init+0x2cc/0x898 [mt7601Usta]) from [<bf4fd86c>] (rt28xx_open+0xd8/0x1b0 [mt7601Usta])
[ 5320.625225] [<bf4fd86c>] (rt28xx_open+0xd8/0x1b0 [mt7601Usta]) from [<bf4ca948>] (RTMP_COM_IoctlHandle+0xd0c/0x10fc [mt7601Usta])
[ 5320.625697] [<bf4ca948>] (RTMP_COM_IoctlHandle+0xd0c/0x10fc [mt7601Usta]) from [<bf4fd1a8>] (MainVirtualIF_open+0x64/0xf8 [mt7601Usta])
[ 5320.625953] [<bf4fd1a8>] (MainVirtualIF_open+0x64/0xf8 [mt7601Usta]) from [<c0342e84>] (__dev_open+0xa0/0xf0)
[ 5320.625978] [<c0342e84>] (__dev_open+0xa0/0xf0) from [<c03430ac>] (__dev_change_flags+0x84/0x13c)
[ 5320.626000] [<c03430ac>] (__dev_change_flags+0x84/0x13c) from [<c03431d0>] (dev_change_flags+0x10/0x48)
[ 5320.626024] [<c03431d0>] (dev_change_flags+0x10/0x48) from [<c03a5b60>] (devinet_ioctl+0x718/0x794)
[ 5320.626051] [<c03a5b60>] (devinet_ioctl+0x718/0x794) from [<c032b964>] (sock_ioctl+0x70/0x270)
[ 5320.626079] [<c032b964>] (sock_ioctl+0x70/0x270) from [<c00da9c0>] (do_vfs_ioctl+0x7c/0x2d0)
[ 5320.626102] [<c00da9c0>] (do_vfs_ioctl+0x7c/0x2d0) from [<c00dac4c>] (sys_ioctl+0x38/0x5c)
[ 5320.626128] [<c00dac4c>] (sys_ioctl+0x38/0x5c) from [<c000e800>] (ret_fast_syscall+0x0/0x30)
[ 5320.626141] ---[ end trace ebde2ef7c673d0fa ]---
[ 5320.626206] Bulk In Failed. Status=-2, BIIdx=0x0, BIRIdx=0x0, actual_length= 0x0
[ 5320.626230] unlink cmd rsp urb
[ 5320.626240] ep->index: 1
[ 5320.626248] aotg_hcd_urb_dequeue:2750, dbg!
[ 5320.626256] aotg_hcd_urb_dequeue:2751, dbg!
[ 5320.626293] aotg_hcd_urb_enqueue:2490, dbg!
[ 5320.626302] ep->index: 1
[ 5320.626309] aotg_hcd_urb_enqueue:2492, dbg!
[ 5320.626317] aotg_hcd_urb_enqueue:2493, dbg!
[ 5320.626330] aotg_hcd aotg_hcd.1: <QUEUE>  usb_hcd_link_urb_to_ep error: -1 !
[ 5320.626340] aotg_hcd_urb_enqueue:2505, err!
[ 5320.626359] <---RTUSBCancelPendingBulkInIRP
[ 5321.174684] <---MlmeThread
[ 5321.177954] <---RTUSBCmdThread
[ 5321.181081] <---RtmpTimerQThread
[ 5321.214622] !!! rt28xx init fail !!!
[ 5321.218242] rt28xx_open return fail!
error: SIOCSIFFLAGS (Operation not permitted)
255|root@android:/misc/modules # 




























static void rx_done_tasklet(unsigned long data)


	struct list_head finished_list;
	list_add_tail(&q->finished_list, &acthcd->hcd_finished_list);
	list_del(&q->finished_list);

static void finish_request(struct aotg_hcd *acthcd,

	list_add_tail(&q->enqueue_list, &acthcd->hcd_enqueue_list);
	list_del(&q->enqueue_list);



	struct aotg_hcep *ep;
			usb_writew(0, ep->reg_hcepbc);
			usb_writeb(0, ep->reg_hcepcs);


static inline int start_transfer(struct aotg_hcd *acthcd, struct aotg_queue *q, struct aotg_hcep *ep)
	list_add_tail(&q->ep_q_list, &ep->q_list);

static int aotg_hcd_urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status)


struct aotg_hcep *ep;
ep->maxpacket
urb->ep->hcpriv = ep;
ep = (struct aotg_hcep *)urb->ep->hcpriv;

local_irq_save(flags);
local_irq_restore(flags);

spin_lock(&acthcd->lock);
spin_unlock(&acthcd->lock);

unsigned long flags;
spin_lock_irqsave(&acthcd->lock, flags);
spin_unlock_irqrestore(&acthcd->lock, flags);

disable_irq(acthcd->uhc_irq);
enable_irq(acthcd->uhc_irq);



acthcd->last_bulkin_dma_len

printk("fifo_cnt = %d\n", (int)(usb_readw(ep->reg_hcepbc) & 0xFFF));
if (aotg_dma_get_cnt(acthcd->dma_nr0) != aotg_dma_get_remain(acthcd->dma_nr0)) {
	w_packets = urb->transfer_buffer_length / ep->maxpacket;
static __inline__ void __hc_in_start(struct aotg_hcd *acthcd, struct aotg_hcep *ep, const u16 w_packets)
w_packets = urb->transfer_buffer_length / MAX_PACKET(next->maxpacket);
if (urb->transfer_buffer_length & (MAX_PACKET(next->maxpacket) - 1)) {
	w_packets++;
}


			list_add_tail(&q->enqueue_list, &acthcd->hcd_enqueue_list);

1, set enque;

2, set deque;

3, set enque ops;

				acthcd->timer_otg_task = 1;


void urb_tasklet_func(unsigned long data)

	/* use pio in irq. */
	handle_hcep_in(acthcd, 1);
	pio_irq_enable(acthcd, ep->mask);
	pio_irq_disable(acthcd, ep->mask);


static void finish_request(struct aotg_hcd *acthcd,
			   struct aotg_queue *q,
			   int status)

	tasklet_hi_schedule(&acthcd->urb_tasklet);
	(urb->pipe == acthcd->dma_working[1])) {


aotg_hcd_start_dma
====>
acthcd->dma_working[dmanr & 0x1] = urb->pipe;




aotg_hcd_dma_handler
====>
acthcd->dma_working[dma_no & 0x1] = 0;



usb_pipeout(urb->pipe)
(usb_pipeendpoint(urb->pipe) == 0)
#if 1
	if (usb_pipetype(urb->pipe) == PIPE_CONTROL) {
		acthcd->setup_processing = 0;
	}
#endif

void aotg_dbg_proc_output_ep(void);

static __inline__ void __hc_in_stop(struct aotg_hcd *acthcd, struct aotg_hcep *ep)

if (aotg_dma_get_cnt(acthcd->dma_nr0) != aotg_dma_get_remain(acthcd->dma_nr0)) {


static __inline__ void __hc_in_stop(struct aotg_hcd *acthcd, struct aotg_hcep *ep)
{
	u8 set = 0x1 << ep->index;

	if (usb_readb(acthcd->base + OUTXSHORTPCKIRQ) & set) {
		usb_writeb(set, acthcd->base + OUTXSHORTPCKIRQ);
	}
	usb_clearbitsb(set, acthcd->base + HCINXSTART);
	usb_writew(0, ep->reg_hcincount);
	//usb_clearbitsb(set, acthcd->base + HCINCTRL);
}

static __inline__ void __hc_in_start(struct aotg_hcd *acthcd, struct aotg_hcep *ep, const u16 w_packets)
{
	u8 set = 0x1 << ep->index;

	usb_writew(w_packets, ep->reg_hcincount);
	usb_setbitsb(set, acthcd->base + OUTXNAKCTRL);
	usb_setbitsb(set, acthcd->base + HCINXSTART);
}












      显示这块的程序如下：

wTmp的定义：

int xdata wTmp=0;
int xdata wNegTmp = 0;

if(ReadCurVal(CANID_184_BUFFER_NUM,3,2,&wTmp))//1桥目标转角
 { 
	wNegTmp = wTmp % 10;
	if (wNegTmp < 0) 
	{
		wNegTmp = 0 - wNegTmp;
	{
  	sprintf(Buffer,"%5d.%1d",wTmp/10, wNegTmp);
  	PutString(125,40,16,Buffer); 
 }


sprintf(Buffer,"%d", (int)wTmp);
sprintf(Buffer,"%u", (unsigned int)wTmp);




==================================

        当 sprintf(Buffer,"%5d.%1u",wTmp/10,wTmp%10); 时，比如值是-2319那么显示的是“-231.65527”

	当 sprintf(Buffer,"%5d.%1d",wTmp/10,wTmp%10); 时，比如值是-2319那么显示的是“-231.-9”

	我期望的是-231.9;

我昨天后来发现了一个规律，当使用 sprintf(Buffer,"%5d.%1u",wTmp/10,wTmp%10)；
小数点后的数字为65536+ wTmp%10 ，而 wTmp%10 是一个负值，恰好就得到 65527 ，于是我就让 65527 *-1，得到正值，最后得到了我想要的。
       哥哥，可是我最大的困惑是“ sprintf(Buffer,"%5d.%1u",wTmp/10,wTmp%10); ”为什么可以得到负值，
       为什么我取消前面的 wTmp = ~wTmp; 就得不到负值。我甚至想是不是我将  wTmp 取反，让他变成了负值，才可以显示负值，那我的程序就没有意义了。 
	另外：  sprintf(Buffer,"%d", (int)wTmp); 显示的结果是-2314；
    sprintf(Buffer,"%u", (unsigned int)wTmp);显示的结果是63222，（也就是65536-2314）；

             

       程序如下：
	if (NewDataFlag[CANID_204_BUFFER_NUM] == 1)
	{
		if(ReadCurVal(CANID_204_BUFFER_NUM,1,0,&wTmp))//车架X倾角
		{
			if(wTmp!=vTmp)
			{
				Rect(122+(vTmp+15)*2.4,98,5,5,0);
				FillRect(122+(vTmp+15)*2.4,98,122+(vTmp+15)*2.4+5,103,0);
				Rect(122+(wTmp+15)*2.4,98,5,5,255);
				FillRect(122+(wTmp+15)*2.4,98,122+(wTmp+15)*2.4+5,103,255);
				vTmp=wTmp;
			}
		}
	}






