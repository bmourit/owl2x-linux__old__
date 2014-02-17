
/*
 *
 * Copyright (C) 2003-2007 Alan Stern
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the above-listed copyright holders may not be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */	
 //#define DEBUG
 #define VERBOSE_DEBUG 
 #define DUMP_MSGS

#include <linux/module.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/completion.h>
#include <linux/pagemap.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/kref.h>
#include <linux/kthread.h>

#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/freezer.h>
#include <linux/utsname.h>
#include <linux/syscalls.h>

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/kgdb.h>

#include <mach/hardware.h>
#include <linux/dma-mapping.h>

#include "stub_protocol.h"
#include "remote_io.h"


/*
 * Kbuild is not very cooperative with respect to linking separately
 * compiled library objects into one module.  So for now we won't use
 * separate compilation ... ensuring init/exit sections work to shrink
 * the runtime footprint, and giving us at least some parts of what
 * a "gcc --combine ... part1.c part2.c part3.c ... " build would.
 */
#include "usbstring.c"
#include "config.c"
#include "epautoconf.c"
#include "stub_lib.c"

#define STUB_USE_SINGLE_STEP	1
#define STUB_ADD_EXE_CALL	1

/*-------------------------------------------------------------------------*/
#define DRIVER_DESC		"stub Gadget"
#define DRIVER_NAME		"stub"
#define DRIVER_VERSION		"3 Dec 2010"

static const char longname[] = DRIVER_DESC;
static const char shortname[] = DRIVER_NAME;

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR("wyzhao");
MODULE_LICENSE("GPL");

#define DRIVER_VENDOR_ID	0x10d6	// Actions
#define DRIVER_PRODUCT_ID	0xff00	//default valuse

#define INT_HS_PACKET_SIZE		64
#define INT_FS_PACKET_SIZE		64

#define MAX_COMMAND_SIZE        128

#ifndef CONFIG_USB_GADGET_VBUS_DRAW
#define CONFIG_USB_GADGET_VBUS_DRAW 400
#endif

struct stub_dev;
int remote_firt_call = 0;
int remote_waiting = 0;
static char cmd_bk[8];
int bulkin_busy = 0;
int bulkout_busy = 0;
int io_calling_busy = 0;

long __remote_io = 0;
EXPORT_SYMBOL(__remote_io);

#if STUB_USE_SINGLE_STEP

#define AOTG_DEBUG_INFO_CNT	10
static int gstub_dbg_idx = 0;
static char* gstub_dbg_info0[AOTG_DEBUG_INFO_CNT];
static unsigned int gstub_dbg_info1[AOTG_DEBUG_INFO_CNT];
static unsigned int gstub_dbg_info2[AOTG_DEBUG_INFO_CNT];
static void gstub_put_debug_info(char *info0, unsigned int info1, unsigned int info2)
{
	gstub_dbg_info0[gstub_dbg_idx] = info0;
	gstub_dbg_info1[gstub_dbg_idx] = info1;
	gstub_dbg_info2[gstub_dbg_idx] = info2;
	gstub_dbg_idx++;
	if (gstub_dbg_idx >= AOTG_DEBUG_INFO_CNT)
		gstub_dbg_idx = 0;
	return;
}

static void gstub_output_debug_info(void)
{
	int i;

	for (i = 0; i < AOTG_DEBUG_INFO_CNT; i++) {
		printk("i:%d  ", i);
		printk("info0:%s: ", gstub_dbg_info0[gstub_dbg_idx]);
		printk("info1:%d, 0x%x;  ", gstub_dbg_info1[gstub_dbg_idx], gstub_dbg_info1[gstub_dbg_idx]);
		printk("info2:%d, 0x%x; \n", gstub_dbg_info2[gstub_dbg_idx], gstub_dbg_info2[gstub_dbg_idx]);
		gstub_dbg_idx++;
		if (gstub_dbg_idx >= AOTG_DEBUG_INFO_CNT)
			gstub_dbg_idx = 0;
	}
	return;
}
#endif  /* STUB_USE_SINGLE_STEP */


/*
 * This driver assumes self-powered hardware and has no way for users to
 * trigger remote wakeup.  It uses autoconfiguration to select endpoints
 * and endpoint addresses.
 */
/*-------------------------------------------------------------------------*/

#define MDBG(fmt,args...) \
	pr_debug(DRIVER_NAME ": " fmt , ## args)

#ifndef DEBUG
#undef VERBOSE_DEBUG
#undef DUMP_MSGS
#endif /* !DEBUG */

#ifdef VERBOSE_DEBUG
#define VLDBG	LDBG
#else
#define VLDBG(lun,fmt,args...) \
	do { } while (0)
#endif /* VERBOSE_DEBUG */

#define MINFO(fmt,args...) \
	pr_info(DRIVER_NAME ": " fmt , ## args)

#define DBG(d, fmt, args...) \
	dev_dbg(&(d)->gadget->dev , fmt , ## args)
#define VDBG(d, fmt, args...) \
	dev_vdbg(&(d)->gadget->dev , fmt , ## args)
#define ERROR(d, fmt, args...) \
	dev_err(&(d)->gadget->dev , fmt , ## args)
#define WARNING(d, fmt, args...) \
	dev_warn(&(d)->gadget->dev , fmt , ## args)
#define INFO(d, fmt, args...) \
	dev_info(&(d)->gadget->dev , fmt , ## args)
	
#ifdef DUMP_MSGS

static void dump_msg(struct stub_dev *stub, const char *label,
		     const u8 *buf, unsigned int length)
{
	if (length < 512) {
//		DBG(stub, "%s, length %u:\n", label, length);
//		print_hex_dump(KERN_DEBUG, "", DUMP_PREFIX_OFFSET,
//			       16, 1, buf, length, 0);
	}
}

#else

#define dump_msg(dev, label, buf, length) do {} while (0)

#endif


#define DEBUG_STUB

#ifdef DEBUG_STUB
#define STUB_DEBUG(fmt, args...) printk(KERN_DEBUG "[stub] " fmt, ## args)
#else
#define STUB_DEBUG(fmt, args...) do {} while(0)
#endif

#define IS_KERN_ADDR(addr) (((ulong)addr & 0x80000000) == 0x80000000)

unsigned char no_finish_reply;
volatile int direct_rw_occured = 0;

static struct {
	unsigned int	num_filenames;
	unsigned int	num_ros;

	int		removable;
	int		can_stall;

	char		*transport_parm;
	char		*protocol_parm;
	unsigned short	vendor;
	unsigned short	product;
	unsigned short	release;
	unsigned int	buflen;

	int		transport_type;
	char		*transport_name;
	int		protocol_type;
	char		*protocol_name;

} mod_data = {					// Default values
	.transport_parm		= "Bulk",
	.protocol_parm		= "STUB",
	.removable		= 1,
	.can_stall		= 0,
	.vendor			= DRIVER_VENDOR_ID,
	.product		= DRIVER_PRODUCT_ID,
	.release		= 0xffff,	// Use controller chip type
	.buflen			= 65536,
	.transport_type = 0x0,
	.protocol_type = 0xff,
	};

//module_param_named(stall, mod_data.can_stall, bool, S_IRUGO);
//MODULE_PARM_DESC(stall, "false to prevent bulk stalls");

#define transport_is_vendor() (mod_data.transport_type == 0x0)
#define protocol_is_vendor() (mod_data.protocol_type == 0xff)

/* Big enough to hold our biggest descriptor */
#define EP0_BUFSIZE	256
#define DELAYED_STATUS	(EP0_BUFSIZE + 999)	// An impossibly large value

#define NUM_BUFFERS	5

#define EP_STALL (1<<6)

enum stub_buffer_state {
	BUF_STATE_EMPTY = 0,
	BUF_STATE_FULL,
	BUF_STATE_BUSY
};

struct stub_buffhd {
	void				*buf;
	unsigned int 			dma;
	enum stub_buffer_state		state;
	struct stub_buffhd		*next;

	/* The NetChip 2280 is faster, and handles some protocol faults
	 * better, if we don't submit any short bulk-out read requests.
	 * So we will record the intended request length here. */
	unsigned int			bulk_out_intended_length;

	struct usb_request		*inreq;
	int				inreq_busy;
	struct usb_request		*outreq;
	int				outreq_busy;
};

enum stub_state {
	STUB_STATE_COMMAND_PHASE = -10,		// This one isn't used anywhere
	STUB_STATE_DATA_PHASE,
	STUB_STATE_STATUS_PHASE,

	STUB_STATE_IDLE = 0,
	STUB_STATE_ABORT_BULK_OUT,
	STUB_STATE_RESET,
	STUB_STATE_INTERFACE_CHANGE,
	STUB_STATE_CONFIG_CHANGE,
	STUB_STATE_DISCONNECT,
	STUB_STATE_EXIT,
	STUB_STATE_TERMINATED
};

enum data_direction {
	DATA_DIR_UNKNOWN = 0,
	DATA_DIR_FROM_HOST,
	DATA_DIR_TO_HOST,
	DATA_DIR_NONE
};

struct stub_dev {
	/* lock protects: state, all the req_busy's, and cbbuf_cmnd */
	spinlock_t		lock;
	struct usb_gadget	*gadget;
	struct remote_io    io;

	/* reference counting: wait until all LUNs are released */
	struct kref		ref;

	struct usb_ep		*ep0;		// Handy copy of gadget->ep0
	struct usb_request	*ep0req;	// For control responses
	unsigned int		ep0_req_tag;
	const char		*ep0req_name;

	struct usb_request	*intreq;	// For interrupt responses
	int			intreq_busy;
	struct stub_buffhd	*intr_buffhd;

 	unsigned int		bulk_out_maxpacket;
	enum stub_state		state;		// For exception handling
	unsigned int		exception_req_tag;

	u8			config, new_config;

	unsigned int		running : 1;
	unsigned int		bulk_in_enabled : 1;
	unsigned int		bulk_out_enabled : 1;
	unsigned int		intr_in_enabled : 1;
	unsigned int		short_packet_received : 1;
	unsigned int		bad_lun_okay : 1;
	    
	unsigned int		response_phase;

	unsigned long		atomic_bitflags;
#define REGISTERED		0
#define IGNORE_BULK_OUT		1
#define SUSPENDED		2

	struct usb_ep		*bulk_in;
	struct usb_ep		*bulk_out;
	struct usb_ep		*intr_in;
    
    struct stub_buffhd  *current_buffhd;
	struct stub_buffhd	*next_buffhd_to_fill;
	struct stub_buffhd	*next_buffhd_to_drain;
	struct stub_buffhd	buffhds[NUM_BUFFERS];
	struct stub_buffhd  remote_io_buffhd;

	wait_queue_head_t 	mon_wait;
	wait_queue_head_t 	io_call_wait;
	int			thread_wakeup_needed;
	struct completion	thread_notifier;
	struct task_struct	*thread_task;

	int			cmnd_size;
	u8			cmnd[MAX_COMMAND_SIZE];
	enum data_direction	data_dir;
	u32			data_size;
	u32			data_size_from_cmnd;
	u32			tag;
	u32			residue;
	u32			usb_amount_left;

	struct kobject *remote_call_obj;	/* just for sysfs. */
	char pc_exe_path[1024];
	int  pc_exe_run;
	int  pc_exe_ret;

	struct semaphore sd_rbs;	/* semaphore for syscall */
};

struct remote_io_private {
    struct remote_io * io;
    int remote_fd;
};

static struct stub_dev *the_stub;
static struct usb_gadget_driver		stub_driver;
static struct remote_io *io = NULL;
static int remote_ready = 0;


typedef void (*stub_routine_t)(struct stub_dev *);

#define io_priv_to_stub(io_priv) container_of (((remote_io_private *)io_priv)->io, struct stub_dev, io)
#define io_to_stub(_io) container_of(_io, struct stub_dev, io)

extern int udc_char_rw_poll(int direct, struct usb_ep *_ep, char *buf, unsigned int buf_len);


int remote_io_ready(void)
{
    return remote_ready; 
}
EXPORT_SYMBOL(remote_io_ready);

struct remote_io *get_remote_io(void)
{
    return &the_stub->io;   
}
EXPORT_SYMBOL(get_remote_io);

static int exception_in_progress(struct stub_dev *stub)
{
	return (stub->state > STUB_STATE_IDLE);
}

/* Make bulk-out requests be divisible by the maxpacket size */
static void set_bulk_out_req_length(struct stub_dev *stub,
		struct stub_buffhd *bh, unsigned int length)
{
	unsigned int	rem;

	bh->bulk_out_intended_length = length;
	rem = length % stub->bulk_out_maxpacket;
	if (rem > 0)
		length += stub->bulk_out_maxpacket - rem;
	bh->outreq->length = length;
}

/*-------------------------------------------------------------------------*/

/*
 * DESCRIPTORS ... most are static, but strings and (full) configuration
 * descriptors are built on demand.  Also the (static) config and interface
 * descriptors are adjusted during stub_bind().
 */
#define STRING_MANUFACTURER	1
#define STRING_PRODUCT		2
#define STRING_SERIAL		3
#define STRING_CONFIG		4
#define STRING_INTERFACE	5

/* There is only one configuration. */
#define	CONFIG_VALUE		1

static struct usb_device_descriptor
device_desc = {
	.bLength =			sizeof device_desc,
	.bDescriptorType =	USB_DT_DEVICE,

	.bcdUSB =			__constant_cpu_to_le16(0x0200),
	.bDeviceClass =		0,

	/* The next three values can be overridden by module parameters */
	.idVendor =			__constant_cpu_to_le16(DRIVER_VENDOR_ID),
	.idProduct =		__constant_cpu_to_le16(DRIVER_PRODUCT_ID),
	.bcdDevice =		__constant_cpu_to_le16(0x0100),

	.iManufacturer =	STRING_MANUFACTURER,
	.iProduct =			STRING_PRODUCT,
	.iSerialNumber =	0,
	.bNumConfigurations =	1,
};

static struct usb_config_descriptor
config_desc = {
	.bLength =		sizeof config_desc,
	.bDescriptorType =	USB_DT_CONFIG,

	/* wTotalLength computed by usb_gadget_config_buf() */
	.bNumInterfaces =	1,
	.bConfigurationValue =	CONFIG_VALUE,
	.iConfiguration =	STRING_CONFIG,
	.bmAttributes =		USB_CONFIG_ATT_ONE,
	.bMaxPower =		CONFIG_USB_GADGET_VBUS_DRAW / 2,
};

static struct usb_otg_descriptor
otg_desc = {
	.bLength =		sizeof(otg_desc),
	.bDescriptorType =	USB_DT_OTG,

	.bmAttributes =		USB_OTG_SRP | USB_OTG_HNP,
};

/* There is only one interface. */

static struct usb_interface_descriptor
intf_desc = {
	.bLength =		sizeof intf_desc,
	.bDescriptorType =	USB_DT_INTERFACE,

	.bNumEndpoints =	2,		// Adjusted during stub_bind()
	.bInterfaceClass =	0xff,
	.bInterfaceSubClass =	0xff,	// Adjusted during stub_bind()
	.bInterfaceProtocol =	0xff,	// Adjusted during stub_bind()
	.iInterface =		STRING_INTERFACE,
};

/* Three full-speed endpoint descriptors: bulk-in, bulk-out,
 * and interrupt-in. */

static struct usb_endpoint_descriptor
fs_bulk_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	/* wMaxPacketSize set by autoconfiguration */
};

static struct usb_endpoint_descriptor
fs_bulk_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	/* wMaxPacketSize set by autoconfiguration */
};

static struct usb_endpoint_descriptor
fs_intr_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize =	__constant_cpu_to_le16(INT_FS_PACKET_SIZE),
	.bInterval =		32,	// frames -> 32 ms
};

static const struct usb_descriptor_header *fs_function[] = {
	(struct usb_descriptor_header *) &otg_desc,
	(struct usb_descriptor_header *) &intf_desc,
	(struct usb_descriptor_header *) &fs_bulk_in_desc,
	(struct usb_descriptor_header *) &fs_bulk_out_desc,
	(struct usb_descriptor_header *) &fs_intr_in_desc,
	NULL,
};
#define FS_FUNCTION_PRE_EP_ENTRIES	2


/*
 * USB 2.0 devices need to expose both high speed and full speed
 * descriptors, unless they only run at full speed.
 *
 * That means alternate endpoint descriptors (bigger packets)
 * and a "device qualifier" ... plus more construction options
 * for the config descriptor.
 */
static struct usb_qualifier_descriptor
dev_qualifier = {
	.bLength =		sizeof dev_qualifier,
	.bDescriptorType =	USB_DT_DEVICE_QUALIFIER,

	.bcdUSB =		__constant_cpu_to_le16(0x0200),
	.bDeviceClass =		USB_CLASS_PER_INTERFACE,

	.bNumConfigurations =	1,
};

static struct usb_endpoint_descriptor
hs_bulk_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	/* bEndpointAddress copied from fs_bulk_in_desc during stub_bind() */
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	__constant_cpu_to_le16(512),
};

static struct usb_endpoint_descriptor
hs_bulk_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	/* bEndpointAddress copied from fs_bulk_out_desc during stub_bind() */
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	__constant_cpu_to_le16(512),
	.bInterval =		1,	// NAK every 1 uframe
};

static struct usb_endpoint_descriptor
hs_intr_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	/* bEndpointAddress copied from fs_intr_in_desc during stub_bind() */
	.bmAttributes =		USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize =	__constant_cpu_to_le16(INT_HS_PACKET_SIZE),
	.bInterval =		9,	// 2**(9-1) = 256 uframes -> 32 ms
};

static const struct usb_descriptor_header *hs_function[] = {
	(struct usb_descriptor_header *) &otg_desc,
	(struct usb_descriptor_header *) &intf_desc,
	(struct usb_descriptor_header *) &hs_bulk_in_desc,
	(struct usb_descriptor_header *) &hs_bulk_out_desc,
	(struct usb_descriptor_header *) &hs_intr_in_desc,
	NULL,
};
#define HS_FUNCTION_PRE_EP_ENTRIES	2

/* Maxpacket and other transfer characteristics vary by speed. */
static struct usb_endpoint_descriptor *
ep_desc(struct usb_gadget *g, struct usb_endpoint_descriptor *fs,
		struct usb_endpoint_descriptor *hs)
{
	if (gadget_is_dualspeed(g) && g->speed == USB_SPEED_HIGH)
		return hs;
	return fs;
}

static char				manufacturer[64];
static char             stub_name[] = "remotefs";
static char				serial[13];

/* Static strings, in UTF-8 (for simplicity we use only ASCII characters) */
static struct usb_string		strings[] = {
	{STRING_MANUFACTURER,	manufacturer},
	{STRING_PRODUCT,	stub_name},
	{STRING_SERIAL,		serial},
	{STRING_CONFIG,		"Self-powered"},
	{STRING_INTERFACE,	"stub interface"},
	{}
};

static struct usb_gadget_strings	stringtab = {
	.language	= 0x0409,		// en-us
	.strings	= strings,
};

/*
 * Config descriptors must agree with the code that sets configurations
 * and with code managing interfaces and their altsettings.  They must
 * also handle different speeds and other-speed requests.
 */
static int populate_config_buf(struct usb_gadget *gadget,
		u8 *buf, u8 type, unsigned index)
{
	enum usb_device_speed			speed = gadget->speed;
	int					len;
	const struct usb_descriptor_header	**function;

	if (index > 0)
		return -EINVAL;

	if (gadget_is_dualspeed(gadget) && type == USB_DT_OTHER_SPEED_CONFIG)
		speed = (USB_SPEED_FULL + USB_SPEED_HIGH) - speed;
	if (gadget_is_dualspeed(gadget) && speed == USB_SPEED_HIGH)
		function = hs_function;
	else
		function = fs_function;

	/* for now, don't advertise srp-only devices */
	if (!gadget_is_otg(gadget))
		function++;

	len = usb_gadget_config_buf(&config_desc, buf, EP0_BUFSIZE, function);
	((struct usb_config_descriptor *) buf)->bDescriptorType = type;
	return len;
}

/* Caller must hold stub->lock */
static void wakeup_thread(struct stub_dev *stub)
{
	if ((bulkin_busy != 0) && (bulkout_busy != 0)) {
		printk("bulkin_busy:%d, bulkout_busy:%d\n", bulkin_busy, bulkout_busy);
	}
	if ((bulkin_busy == 0) && (bulkout_busy == 0)) {
		//printk("%s, %d\n", __func__, __LINE__);
	} else {
		bulkin_busy = 0;
		bulkout_busy = 0;
	}
	wake_up(&stub->mon_wait);
	return;
}

static void raise_exception(struct stub_dev *stub, enum stub_state new_state)
{
	unsigned long		flags;

	/* Do nothing if a higher-priority exception is already in progress.
	 * If a lower-or-equal priority exception is in progress, preempt it
	 * and notify the main thread by sending it a signal. */
	spin_lock_irqsave(&stub->lock, flags);
	if (stub->state <= new_state) {
		stub->exception_req_tag = stub->ep0_req_tag;
		stub->state = new_state;
		if (stub->thread_task)
			send_sig_info(SIGUSR1, SEND_SIG_FORCED,
					stub->thread_task);
	}
	spin_unlock_irqrestore(&stub->lock, flags);
}

static int stub_set_halt(struct stub_dev *stub, struct usb_ep *ep)
{
	const char	*name;

	if (ep == stub->bulk_in)
		name = "bulk-in";
	else if (ep == stub->bulk_out)
		name = "bulk-out";
	else
		name = ep->name;
	DBG(stub, "%s set halt\n", name);
	return usb_ep_set_halt(ep);
}

/*-------------------------------------------------------------------------*/

/* The disconnect callback and ep0 routines.  These always run in_irq,
 * except that ep0_queue() is called in the main thread to acknowledge
 * completion of various requests: set config, set interface, and
 * Bulk-only device reset. */

static void stub_disconnect(struct usb_gadget *gadget)
{
	struct stub_dev		*stub = get_gadget_data(gadget);

	DBG(stub, "disconnect or port reset\n");
	raise_exception(stub, STUB_STATE_DISCONNECT);
}


static int ep0_queue(struct stub_dev *stub)
{
	int	rc;

	rc = usb_ep_queue(stub->ep0, stub->ep0req, GFP_ATOMIC);
	if (rc != 0 && rc != -ESHUTDOWN) {

		/* We can't do much more than wait for a reset */
		WARNING(stub, "error in submission: %s --> %d\n",
				stub->ep0->name, rc);
	}
	return rc;
}

static void ep0_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct stub_dev		*stub = ep->driver_data;

	if (req->actual > 0)
		dump_msg(stub, stub->ep0req_name, req->buf, req->actual);
	if (req->status || req->actual != req->length)
		DBG(stub, "%s --> %d, %u/%u\n", __func__,
				req->status, req->actual, req->length);
	if (req->status == -ECONNRESET)		// Request was cancelled
		usb_ep_fifo_flush(ep);

	if (req->status == 0 && req->context)
		((stub_routine_t) (req->context))(stub);
}

/*-------------------------------------------------------------------------*/

/* Bulk and interrupt endpoint completion handlers.
 * These always run in_irq. */

static void bulk_in_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct stub_dev		*stub = ep->driver_data;
	struct stub_buffhd	*bh = req->context;

	stub = the_stub;

	bulkin_busy = 0;

	if (req->status || req->actual != req->length)
		DBG(stub, "%s --> %d, %u/%u\n", __func__,
				req->status, req->actual, req->length);
	if (req->status == -ECONNRESET)		// Request was cancelled
		usb_ep_fifo_flush(ep);

	/* Hold the lock while we update the request and buffer states */
	smp_wmb();
	spin_lock(&stub->lock);
	bh->inreq_busy = 0;
	bh->state = BUF_STATE_EMPTY;
	wakeup_thread(stub);
	spin_unlock(&stub->lock);
	return;
}

static void bulk_out_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct stub_dev		*stub = ep->driver_data;
	struct stub_buffhd	*bh = req->context;

	stub = the_stub;

	bulkout_busy = 0;
	//printk("%s --> %d, %u/%u\n", __func__, req->status, req->actual, req->length);
	dump_msg(stub, "bulk-out", req->buf, req->actual);
	if (req->status || req->actual != bh->bulk_out_intended_length)
		DBG(stub, "%s --> %d, %u/%u\n", __func__,
				req->status, req->actual,
				bh->bulk_out_intended_length);
	if (req->status == -ECONNRESET)		// Request was cancelled
		usb_ep_fifo_flush(ep);

	/* Hold the lock while we update the request and buffer states */
	smp_wmb();
	spin_lock(&stub->lock);
	bh->outreq_busy = 0;
	bh->state = BUF_STATE_FULL;
	wakeup_thread(stub);
	spin_unlock(&stub->lock);
}

static void intr_in_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct stub_dev		*stub = ep->driver_data;
	struct stub_buffhd	*bh = req->context;

	if (req->status || req->actual != req->length)
		DBG(stub, "%s --> %d, %u/%u\n", __func__,
				req->status, req->actual, req->length);
	if (req->status == -ECONNRESET)		// Request was cancelled
		usb_ep_fifo_flush(ep);

	/* Hold the lock while we update the request and buffer states */
	smp_wmb();
	spin_lock(&stub->lock);
	stub->intreq_busy = 0;
	bh->state = BUF_STATE_EMPTY;
	wakeup_thread(stub);
	spin_unlock(&stub->lock);
}

static void remote_io_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct stub_dev		*stub = ep->driver_data;
	struct stub_buffhd	*bh = req->context;

	bulkin_busy = 0;
	if (req->status || req->actual != req->length)
		DBG(stub, "%s --> %d, %u/%u\n", __func__,
				req->status, req->actual, req->length);

	if (req->status == -ECONNRESET)		// Request was cancelled
		usb_ep_fifo_flush(ep);
		
	/* Hold the lock while we update the request and buffer states */
	smp_wmb();
	spin_lock(&stub->lock);
	bh->inreq_busy = 0;
	bh->state = BUF_STATE_EMPTY;
	wakeup_thread(stub);
	spin_unlock(&stub->lock);    
	remote_firt_call = 0;
	remote_waiting = 0;
	complete(&stub->thread_notifier);
	return;
}

static int class_setup_req(struct stub_dev *stub,
		const struct usb_ctrlrequest *ctrl)
{
	//struct usb_request	*req = stub->ep0req;
	int			value = -EOPNOTSUPP;
	u16			w_index = le16_to_cpu(ctrl->wIndex);
	//u16                     w_value = le16_to_cpu(ctrl->wValue);
	u16			w_length = le16_to_cpu(ctrl->wLength);

	if (!stub->config)
		return value;
		
	//todo

	if (value == -EOPNOTSUPP)
		VDBG(stub,
			"unknown class-specific control req "
			"%02x.%02x v%04x i%04x l%u\n",
			ctrl->bRequestType, ctrl->bRequest,
			le16_to_cpu(ctrl->wValue), w_index, w_length);
	return value;
}

/*-------------------------------------------------------------------------*/

/* Ep0 standard request handlers.  These always run in_irq. */

static int standard_setup_req(struct stub_dev *stub,
		const struct usb_ctrlrequest *ctrl)
{
	struct usb_request	*req = stub->ep0req;
	int			value = -EOPNOTSUPP;
	u16			w_index = le16_to_cpu(ctrl->wIndex);
	u16			w_value = le16_to_cpu(ctrl->wValue);

	/* Usually this just stores reply data in the pre-allocated ep0 buffer,
	 * but config change events will also reconfigure hardware. */
	switch (ctrl->bRequest) {

	case USB_REQ_GET_DESCRIPTOR:
		if (ctrl->bRequestType != (USB_DIR_IN | USB_TYPE_STANDARD |
				USB_RECIP_DEVICE))
			break;
		switch (w_value >> 8) {

		case USB_DT_DEVICE:
			VDBG(stub, "get device descriptor\n");
			value = sizeof device_desc;
			memcpy(req->buf, &device_desc, value);
			break;
		case USB_DT_DEVICE_QUALIFIER:
			VDBG(stub, "get device qualifier\n");
			if (!gadget_is_dualspeed(stub->gadget))
				break;
			value = sizeof dev_qualifier;
			memcpy(req->buf, &dev_qualifier, value);
			break;

		case USB_DT_OTHER_SPEED_CONFIG:
			VDBG(stub, "get other-speed config descriptor\n");
			if (!gadget_is_dualspeed(stub->gadget))
				break;
			goto get_config;
		case USB_DT_CONFIG:
			VDBG(stub, "get configuration descriptor\n");
get_config:
			value = populate_config_buf(stub->gadget,
					req->buf,
					w_value >> 8,
					w_value & 0xff);
			break;

		case USB_DT_STRING:
			VDBG(stub, "get string descriptor\n");

			/* wIndex == language code */
			value = usb_gadget_get_string(&stringtab,
					w_value & 0xff, req->buf);
			break;
		}
		break;

	/* One config, two speeds */
	case USB_REQ_SET_CONFIGURATION:
		if (ctrl->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD |
				USB_RECIP_DEVICE))
			break;
		VDBG(stub, "set configuration\n");
		if (w_value == CONFIG_VALUE || w_value == 0) {
			stub->new_config = w_value;

			/* Raise an exception to wipe out previous transaction
			 * state (queued bufs, etc) and set the new config. */
			raise_exception(stub, STUB_STATE_CONFIG_CHANGE);
			value = DELAYED_STATUS;
		}
		break;
	case USB_REQ_GET_CONFIGURATION:
		if (ctrl->bRequestType != (USB_DIR_IN | USB_TYPE_STANDARD |
				USB_RECIP_DEVICE))
			break;
		VDBG(stub, "get configuration\n");
		*(u8 *) req->buf = stub->config;
		value = 1;
		break;

	case USB_REQ_SET_INTERFACE:
		if (ctrl->bRequestType != (USB_DIR_OUT| USB_TYPE_STANDARD |
				USB_RECIP_INTERFACE))
			break;
		if (stub->config && w_index == 0) {

			/* Raise an exception to wipe out previous transaction
			 * state (queued bufs, etc) and install the new
			 * interface altsetting. */
			raise_exception(stub, STUB_STATE_INTERFACE_CHANGE);
			value = DELAYED_STATUS;
		}
		break;
	case USB_REQ_GET_INTERFACE:
		if (ctrl->bRequestType != (USB_DIR_IN | USB_TYPE_STANDARD |
				USB_RECIP_INTERFACE))
			break;
		if (!stub->config)
			break;
		if (w_index != 0) {
			value = -EDOM;
			break;
		}
		VDBG(stub, "get interface\n");
		*(u8 *) req->buf = 0;
		value = 1;
		break;

	default:
		VDBG(stub,
			"unknown control req %02x.%02x v%04x i%04x l%u\n",
			ctrl->bRequestType, ctrl->bRequest,
			w_value, w_index, le16_to_cpu(ctrl->wLength));
	}

	return value;
}

static int stub_setup(struct usb_gadget *gadget,
		const struct usb_ctrlrequest *ctrl)
{
	struct stub_dev		*stub = get_gadget_data(gadget);
	int			rc;
	int			w_length = le16_to_cpu(ctrl->wLength);

	++stub->ep0_req_tag;		// Record arrival of a new request
	stub->ep0req->context = NULL;
	stub->ep0req->length = 0;
	dump_msg(stub, "ep0-setup", (u8 *) ctrl, sizeof(*ctrl));

	if ((ctrl->bRequestType & USB_TYPE_MASK) == USB_TYPE_CLASS)
		rc = class_setup_req(stub, ctrl);
	else
		rc = standard_setup_req(stub, ctrl);

	/* Respond with data/status or defer until later? */
	if (rc >= 0 && rc != DELAYED_STATUS) {
		rc = min(rc, w_length);
		stub->ep0req->length = rc;
		stub->ep0req->zero = rc < w_length;
		stub->ep0req_name = (ctrl->bRequestType & USB_DIR_IN ?
				"ep0-in" : "ep0-out");
		rc = ep0_queue(stub);
	}

	/* Device either stalls (rc < 0) or reports success */
	return rc;
}

/*-------------------------------------------------------------------------*/

/* All the following routines run in process context */


/* Use this for bulk or interrupt transfers, not ep0 */
static void start_transfer(struct stub_dev *stub, struct usb_ep *ep,
		struct usb_request *req, int *pbusy,
		enum stub_buffer_state *state)
{
	int	rc;
	struct stub_buffhd *bh = container_of(state, struct stub_buffhd, state);

	if (ep == stub->bulk_in) {
		bulkin_busy = 1;
		dump_msg(stub, "bulk-in", req->buf, req->length);
	} else if (ep == stub->intr_in) {
		dump_msg(stub, "intr-in", req->buf, req->length);
	} else if (ep == stub->bulk_out) {
		bulkout_busy = 1;
	}

	//req->dma = virt_to_bus(req->buf);
	req->dma = bh->dma;

	spin_lock_irq(&stub->lock);
	*pbusy = 1;
	*state = BUF_STATE_BUSY;
	spin_unlock_irq(&stub->lock);
	rc = usb_ep_queue(ep, req, GFP_KERNEL);
	if (rc != 0) {
		printk("%s:%d\n", __func__, __LINE__);
		*pbusy = 0;
		*state = BUF_STATE_EMPTY;

		/* We can't do much more than wait for a reset */

		/* Note: currently the net2280 driver fails zero-length
		 * submissions if DMA is enabled. */
		if (rc != -ESHUTDOWN && !(rc == -EOPNOTSUPP &&
						req->length == 0))
			WARNING(stub, "error in submission: %s --> %d\n",
					ep->name, rc);
		if (ep == stub->bulk_in) {
			bulkin_busy = 0;
		} else if (ep == stub->bulk_out) {
			bulkout_busy = 0;
		}
	}

	return;
}

static int sleep_thread(struct stub_dev *stub)
{
	unsigned long expire;
	unsigned long old_jiffies = jiffies;
	int is_bulk_in = bulkin_busy;

	wait_event_interruptible(stub->mon_wait, ((bulkin_busy == 0) && (bulkout_busy == 0)));
	//wait_event_interruptible_timeout(stub->mon_wait, ((bulkin_busy == 0) && (bulkout_busy == 0)), 1000);

	expire = jiffies - old_jiffies;
	if ((expire > 1000) && (is_bulk_in != 0)) {
		printk("expire:%d\n", (unsigned int)expire);
		printk("%s, %d\n", __func__, __LINE__);
	}
	return 0;
}

static int received_gdb_cmd(struct stub_dev *stub, struct stub_buffhd *bh)
{
	struct usb_request *req = bh->outreq;
	struct stub_buffhd *status_bh;
	int rc;

	/* Was this a real packet?  Should it be ignored? */
	if (req->status || test_bit(IGNORE_BULK_OUT, &stub->atomic_bitflags))
		return -EINVAL;
    
    memset(stub->cmnd, 0, MAX_COMMAND_SIZE);
    memcpy(stub->cmnd, req->buf, MAX_COMMAND_SIZE);
    stub->cmnd_size = -1; // we do not use it
    
    switch (stub->cmnd[1]) {
    case 'X':
    case 'M':
        return 0;
        
    default:
        bh->state = BUF_STATE_EMPTY;
    }

    status_bh = stub->next_buffhd_to_fill;
	while (status_bh->state != BUF_STATE_EMPTY) {
		rc = sleep_thread(stub);
		if (rc)
			return rc;
	}
	smp_rmb();
	
	//should check this command here
	((char *)status_bh->buf)[0] = '+';
	status_bh->inreq->length = 1;
	status_bh->inreq->zero = 0;
	stub->residue = 0;
	status_bh->state = BUF_STATE_FULL;

	/* Send this buffer and go read some more */
	start_transfer(stub, stub->bulk_in, status_bh->inreq,
		       &status_bh->inreq_busy, &status_bh->state);
	stub->next_buffhd_to_fill = status_bh->next;
	
	rc = sleep_thread(stub);
	if (rc)
		return rc;
	//printk(KERN_DEBUG "get command %c, send +\n", ((char *)bh->buf)[1]);
	return 0;
}

static int finish_reply(struct stub_dev *stub)
{	
#if STUB_USE_SINGLE_STEP
	long timeout = 0;

	if ((__remote_io != 0) && (cmd_bk[1] == 'F') && 
		((cmd_bk[2] == '-') || ((cmd_bk[2] >= '0') && (cmd_bk[2] <= 'f'))) && 
		((cmd_bk[3] == '#') || ((cmd_bk[3] >= '0') && (cmd_bk[3] <= 'f'))) ) {
		//sleep_thread(stub);
		remote_waiting = 1;
		//wait_for_completion(&stub->thread_notifier);
		do {
			timeout = wait_for_completion_timeout(&stub->thread_notifier, 2000);
			//printk("timeout:%d\n", timeout);
		} while (timeout == 0);
		/* The cleanup routine waits for this completion also */
		//continue;
	}
#endif  /* STUB_USE_SINGLE_STEP */
	return 0; //for debug
}

static inline int get_datalen_from_command(char *cmd)
{
    int count = 0;
    
    while (*cmd++ != '#') count++;
    
    return count;   
}

static inline void fill_err_packet(char *buffer, int status)
{
	buffer[0] = 'E';
	buffer[1] = hexchars[status >> 4];
	buffer[2] = hexchars[status & 4];
	buffer[3] = 0;
	
}

static inline  void fill_ok_packet(char *buffer)
{
	strcpy(buffer,"OK");
}

//the first char must be '$'
static inline int make_checksum(char *buf, int data_cnt)
{
    u8 checksum;
	int count;
	char *ptr;
    
    if (buf[0] != '$') return -1;
    
    checksum = 0;
    count =1;
    
    ptr = &buf[1];
    while (data_cnt-- != 0) {
        checksum += *ptr;
        count ++;
        ptr++;   
    }
    
    *ptr++ = '#';
    *ptr++ = hexchars[checksum >> 4];
    *ptr = hexchars[checksum & 0xf];
    
    return (count + 3);
}

//do not use the same buffer of dest and src
static inline int make_packet(const char *src,char * dest )
{
	u8 checksum, ch;
	int count;

	/*
	 * $<packet info>#<checksum>.
	 */
	
	dest[0] = '$';
	checksum = 0;
	count = 0;

	while ((ch = src[count]) != 0) {
		checksum += ch;
		count ++;
		dest[count] = ch;
	}
	count++;
	dest[count] = '#';
	count++;
	dest[count] = hexchars[checksum >> 4];
	count++;
	dest[count] = hexchars[checksum & 0xf];

	return ++count;
}

static int get_remotefs_call_handle(struct remote_io *io)
{
	//DECLARE_WAITQUEUE(wait, current);
	int rc = 0;
	
	might_sleep();
	
	//add_wait_queue_exclusive(&io->remotefs_wait_queue, &wait);
	spin_lock_irq(&io->io_lock);
	while (1) {
	    if (!io->remotefs_call_handle || io->remote_io_exit)
			break;

	    try_to_freeze();
		set_current_state(TASK_INTERRUPTIBLE);
		if (signal_pending(current)) {
		    pr_info(" ==> intr !!\n");
		    rc = -EINTR;
		    break;
		}
			
		spin_unlock_irq(&io->io_lock);
		schedule();
		spin_lock_irq(&io->io_lock);
	}
	
	set_current_state(TASK_RUNNING);
	if (likely(rc == 0)) {
    	if (likely(!io->remote_io_exit)) {
    		io->remotefs_call_handle = 1;	
    	} else {
    		//wake_up_all(&io->remotefs_wait_queue);
    		io->remotefs_call_handle = 0;
    		spin_unlock_irq(&io->io_lock);
    		//remove_wait_queue(&io->remotefs_wait_queue, &wait);
    		return -ENODEV;
    	}
    }
	
	spin_unlock_irq(&io->io_lock);
	//remove_wait_queue(&io->remotefs_wait_queue, &wait);
	
	return rc;
}

static loff_t release_remotefs_io_handle(struct remote_io *io)
{
	loff_t ret;
	
	spin_lock_irq(&io->io_lock);
	ret = io->ret;
	io->remotefs_call_handle = 0;
	//wake_up(&io->remotefs_wait_queue);
	spin_unlock_irq(&io->io_lock);
	
	return ret;
}

static loff_t remotefs_call(int call_id, long arg0, long arg1, long arg2, long arg3, long arg4);

static int remotefs_open(struct remote_io *io, const char *pathname, int flags, int mode)
{
	int ret;
	ret = (int)remotefs_call(SYSCALL_ID_OPEN, (long)pathname, (long)flags, (long)mode, 0, 0);    
    	return ret;
}

static int remotefs_close(struct remote_io *io, int fd)
{
    return remotefs_call(SYSCALL_ID_CLOSE, (long)fd, 0, 0, 0, 0);
}

static ssize_t remotefs_read(struct remote_io *io, int fd, char __user *buf, size_t count)
{
    ssize_t amount;
    ssize_t scount = (ssize_t)count;
    ssize_t ret;
    ssize_t has_done = 0;
    
    //MINFO("%s, %d\n", __func__, __LINE__);
    
    while (scount > 0) {
        amount = min((ssize_t)mod_data.buflen, scount);
        ret = remotefs_call(SYSCALL_ID_READ, (long)fd, (long)buf, (long)amount, 0, 0);
        if (ret > 0) has_done += ret;
        else if (ret == 0) return has_done;
       	else return ret;
        
        //STUB_DEBUG("read count: %d, ret: %d\n", scount, ret);
        scount -= ret;
        buf +=  ret;
    }
    
    return has_done;
}

static ssize_t remotefs_write(struct remote_io *io, int fd, const char __user *buf, size_t count)
{
    size_t amount;
    ssize_t ret;
    ssize_t has_done = 0;
    
    //    MINFO("%s, %d\n", __func__, __LINE__);
    
    while (count > 0) {
        amount = min(mod_data.buflen, count);
        ret = remotefs_call(SYSCALL_ID_WRITE, (long)fd, (long)buf, (long)amount, 0, 0);
        if (ret > 0) has_done += ret;
        else if (ret == 0) return has_done;
       	else return ret;
        
        count -= ret;
        buf +=  ret;  
    }
    
    return has_done;
}

static loff_t remotefs_lseek(struct remote_io *io, int fd, loff_t offset, int flag)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_LSEEK, (long)fd, (long)(offset>>32), (long)offset, (long)flag, 0);    
}

static long long remotefs_tell(struct remote_io *io, int fd)
{
//   MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_TELL, (long)fd, 0, 0, 0, 0);       
}

static int remotefs_rename(struct remote_io *io, const char *oldpath, const char *newpath)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_RENAME, (long)oldpath, (long)newpath, 0, 0, 0);      
}

static int  remotefs_remove(struct remote_io *io, const char *pathname)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_REMOVE, (long)pathname, 0, 0, 0, 0);      
}

static int remotefs_readdir(struct remote_io *io, int fd, loff_t pos, void * ent , int len)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_READDIR, (long)fd, (long)(pos>>32), (long)pos, (long)ent, (long)len);    
}

static int remotefs_seekdir(struct remote_io *io, int fd, loff_t offset)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_SEEKDIR, (long)fd, (long)(offset>>32), (long)offset, 0, 0);    
}

static int remotefs_rewinddir(struct remote_io *io, int fd)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_REWINDDIR, (long)fd, 0, 0, 0, 0);      
}

static int remotefs_mkdir(struct remote_io *io, const char *path, mode_t mode)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_MKDIR, (long)path, (long)mode, 0, 0, 0);     
}

static int remotefs_rmdir(struct remote_io *io, const char *path)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_RMDIR, (long)path, 0, 0, 0, 0);      
}

static int remotefs_fstat (struct remote_io *io, int fd, void *buf )
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_FSTAT, (long)fd, (long)buf, 0, 0, 0);     
}

static int remotefs_stat (struct remote_io *io, const char *path, void __user *buf)
{
//    MINFO("path: %s, %s, %d\n", path, __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_STAT, (long)path, (long)buf, 0, 0, 0);       
}

static int remotefs_statfs(struct remote_io *io, const char *pathname, void * buf )
{
//    MINFO("pathname: %s, %s, %d\n", pathname, __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_STATFS, (long)pathname, (long)buf, 0, 0, 0);      
}

static int remotefs_lastdir(struct remote_io *io, int fd)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_LASTDIR, (long)fd, 0, 0, 0, 0);     
}

static int remotefs_prevdir(struct remote_io *io, int fd, loff_t pos, void *buf, int len)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_PREVDIR, (long)fd, (long)(pos>>32), (long)pos, (long)buf, (long)len);       
}

static int remotefs_reset2parentdir(struct remote_io *io, int fd)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_RESET2PARENTDIR, (long)fd, 0, 0, 0, 0);      
}

static int remotefs_truncate(struct remote_io *io, const char *pathname, long long len)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_TRUNCATE, (long)pathname, (long)(len>>32), (long)len, 0, 0);    
}

static int remotefs_ftruncate(struct remote_io *io, int fd, long long len)
{
//    MINFO("%s, %d\n", __func__, __LINE__);
    return remotefs_call(SYSCALL_ID_FTRUNCATE, (long)fd, (long)(len>>32), (long)len, 0, 0);   
}


static void setup_remote_io_ops(struct remote_io *io)
{
    io->ops.open = remotefs_open;
    io->ops.close = remotefs_close;
    io->ops.read = remotefs_read;
    io->ops.write = remotefs_write;
    io->ops.lseek = remotefs_lseek;
    io->ops.tell = remotefs_tell;
    io->ops.rename = remotefs_rename;
    io->ops.remove = remotefs_remove;
    io->ops.readdir = remotefs_readdir;
    io->ops.seekdir = remotefs_seekdir;
    io->ops.rewinddir = remotefs_rewinddir;
    io->ops.mkdir = remotefs_mkdir;
    io->ops.rmdir = remotefs_rmdir;
    io->ops.fstat = remotefs_fstat;
    io->ops.stat = remotefs_stat;
    io->ops.statfs = remotefs_statfs;
    io->ops.lastdir = remotefs_lastdir;
    io->ops.prevdir = remotefs_prevdir;
    io->ops.reset2parentdir = remotefs_reset2parentdir;
    io->ops.truncate = remotefs_truncate;
    io->ops.ftruncate = remotefs_ftruncate;
}

static void cygpath_to_win_path(char *path)
{
    char *cygdrive;
#if 0
    int i = 0;

    for (i=0; i<strlen(path); i++) {
	    if (path[i] == '/') {
		path[i] = '\\';
	    }
    }
#endif
    if ((cygdrive = strstr(path, "cygdrive/")) != NULL) {       
        path[0] = cygdrive[9];
        path[1] = ':';
        path[2] = '/';
        if (cygdrive[10] == '/' || cygdrive[10] == '\\') {
            memmove(path + 3, &cygdrive[11], strlen(&cygdrive[11]) + 1);   
        }
        else {
            memmove(path + 3, &cygdrive[10], strlen(&cygdrive[10]) + 1);  
        }
    }
}

static loff_t remotefs_call(int call_id, long arg0, long arg1, long arg2, long arg3, long arg4)
{
    struct stub_dev *stub;
    struct remote_io *io;
    struct stub_buffhd *bh;
    //struct stub_buffhd *bh_tmp;
    char *ptr;
    int ret;
    int len;
    int data_cnt;
    static char path[MAX_PATH];
    ssize_t size = 0;
    ulong user_buf = 0;
    int i;
    long arg_val;
    loff_t retv;
    DECLARE_COMPLETION_ONSTACK(completion);
    
    stub = the_stub;
#if 1
	/* try to get control of the read buffer */
	if (down_trylock(&stub->sd_rbs)) {
		printk("remotefs lock!\n");

		/* ...or if we want to block, then do so here */
		if (down_interruptible(&stub->sd_rbs)) {
			printk("remotefs lock failed again!\n");
		}
	}
#endif

    io = &stub->io;
    bh = &stub->remote_io_buffhd;
    
    if ((ret = get_remotefs_call_handle(&stub->io)) != 0) {
	    up(&stub->sd_rbs);
        return ret;
    }
    
	while (bh->state != BUF_STATE_EMPTY) {
		ret = sleep_thread(stub);
		if (ret) {
			up(&stub->sd_rbs);
			return ret;
		}

	}
    
#if STUB_USE_SINGLE_STEP
	i = 0;
	while ((remote_waiting == 0) && (i < 2000)) {
		i++;
		msleep(1);
	}
	if (i >= 2000) {
		printk("still not in remote_waiting!\n");
		gstub_put_debug_info("still waiting", __LINE__, 0);
		gstub_output_debug_info();
	}
	remote_waiting = 0;
	io_calling_busy = 1;
#endif  /* STUB_USE_SINGLE_STEP */

    ((char *)bh->buf)[0] = '$';
    ptr = (char *)bh->buf + 1;
    data_cnt = 0;
    
    switch (call_id) {
        case SYSCALL_ID_OPEN:
        {
            len = strlen("Fopen,");
            strncpy(ptr, "Fopen,", len);
            ptr += len;
            strcpy(path, io->pc_root_path);
            if (IS_KERN_ADDR((ulong)arg0)) {
                strcat(path, (const char *)arg0);
            }
            else {
                ret = copy_from_user(path + strlen(path), (char __user *) arg0, strlen_user((const char __user *) arg0));
            }
            
            cygpath_to_win_path(path);
            
            ptr = long2hex((long)path, ptr); /*__user filename*/
            *ptr++ = '/';
            len = strlen(path);/*caculate the string length exclude '/0'*/
            ptr = int2hex(len, ptr);
            *ptr++ = ',';
	//printk("arg1:%d, 0x%x\n", arg1, arg1);

#define FILEIO_O_RDONLY           0x0
#define FILEIO_O_WRONLY           0x1
#define FILEIO_O_RDWR             0x2
#define FILEIO_O_APPEND           0x8
#define FILEIO_O_CREAT          0x200
#define FILEIO_O_TRUNC          0x400
#define FILEIO_O_EXCL           0x800
#define FILEIO_O_DIRECTORY      0x10000

#define HJK_O_RDONLY		0x00000000
#define HJK_O_WRONLY		0x00000001
#define HJK_O_RDWR		0x00000002
#define HJK_O_APPEND		0x00002000
#define HJK_O_CREAT		0x00000100	/* not fcntl */
#define HJK_O_TRUNC		0x00001000	/* not fcntl */
#define HJK_O_EXCL		0x00000200	/* not fcntl */

#if 1
	arg_val = 0;

	if (arg1 & O_WRONLY)
		arg_val |= FILEIO_O_WRONLY;
	if (arg1 & O_RDWR)
		arg_val |= FILEIO_O_RDWR;
	if (arg1 & O_APPEND)
		arg_val |= FILEIO_O_APPEND;
	if (arg1 & O_CREAT)
		arg_val |= FILEIO_O_CREAT;
	if (arg1 & O_TRUNC)
		arg_val |= FILEIO_O_TRUNC;
	if (arg1 & O_EXCL)
		arg_val |= FILEIO_O_EXCL;
	if (arg1 & O_DIRECTORY)
		arg_val |= FILEIO_O_DIRECTORY;

	//arg_val |= 0x00010000;
	arg1 = arg_val;
#endif
            ptr = long2hex(arg1, ptr);/*flags*/
            *ptr++ = ',';
            ptr = long2hex(arg2, ptr);/*mode*/ 
            
            STUB_DEBUG("open: %s\n", path);       
        }
        break;
        case SYSCALL_ID_READ:
        {
            len = strlen("Fread,");
            strncpy((char *)ptr, "Fread,", len);
            ptr += len;
            ptr = long2hex(arg0, ptr); /*fd*/
            *ptr++ = ',';
            ptr = long2hex((long)io->buf_for_user, ptr); /*buf*/
            *ptr++ = ',';
            ptr = long2hex(arg2, ptr); /*count*/
            
            user_buf = (ulong)arg1;
            size = arg2;
        }
        break;
        case SYSCALL_ID_WRITE:
        {
            len = strlen("Fwrite,");
            strncpy(ptr, "Fwrite,", len);
            ptr += len;
            ptr = long2hex(arg0, ptr); /*fd*/
            *ptr++ = ',';
            ptr = long2hex((long)io->buf_for_user, ptr); /*buf*/
            *ptr++ = ',';
            ptr = long2hex(arg2, ptr); /*count*/
            
            user_buf = (ulong)arg1;
            size = arg2;
            
            if (IS_KERN_ADDR(user_buf)) {
                memcpy(io->buf_for_user,(void *)user_buf, size);   
            } 
            else {
                ret = copy_from_user(io->buf_for_user, (void __user *)user_buf, size);   
            }
            
        }
        break;
        case SYSCALL_ID_CLOSE:
        {
            len = strlen("Fclose,");
            strncpy(ptr, "Fclose,", len);
            ptr += len;
            ptr = long2hex(arg0, ptr); /*fd*/
        }
        break;
        case SYSCALL_ID_LSEEK:
        {
            len = strlen("Flseek,");
            strncpy(ptr, "Flseek,", len);
            ptr += len;
            ptr = long2hex(arg0, ptr); /*fd*/
            *ptr++ = ',';
            
            ptr = long2hex(arg1, ptr);  //地址高位还是低位？
            ptr = long2hex(arg2, ptr);
            *ptr++ = ',';
            ptr = long2hex(arg3, ptr);/*flag*/
        }
        break;
        
        case SYSCALL_ID_TELL:
        {
            len = strlen("Ftell,");
            strncpy(ptr, "Ftell,", len);

            ptr += len;
            ptr = long2hex(arg0, ptr); /*fd*/

            break;
        }

        case SYSCALL_ID_STAT:
        {
            len = strlen("Fstat,");
            strncpy(ptr, "Fstat,", len);
            ptr += len;

            ptr = long2hex((long)path, ptr); /*filename*/
            strcpy(path, io->pc_root_path);
            if (IS_KERN_ADDR(arg0)) {
                strcat(path, (char *)arg0);   
            }
            else {
                ret = copy_from_user(path + strlen(path), (char __user *) arg0, strlen_user((const char __user *) arg0));
            }
            
            cygpath_to_win_path(path);
            
            *ptr++ = '/';
            len = strlen(path);/*caculate the string length include '/0'*/
            ptr = int2hex(len, ptr);

            *ptr++ = ',';
            ptr = long2hex((long)io->buf_for_user, ptr );/* stat* buf */
            
            user_buf = (ulong)arg1;
            size = sizeof(struct remotefs_stat);
            
            STUB_DEBUG("stat: %s\n", path);
            break;
        }

        case SYSCALL_ID_FSTAT:
        {
            len = strlen("Ffstat,");
            strncpy(ptr, "Ffstat,", len);

            ptr += len;
            ptr = long2hex(arg0, ptr); /*fd*/

            *ptr++ = ',';
            ptr = long2hex((long)io->buf_for_user, ptr );/* stat* buf */
            
            user_buf = (ulong)arg1;
            size = sizeof(struct remotefs_stat);
            break;
        }

        case SYSCALL_ID_STATFS:
        {
            len = strlen("Fstatfs,");
            strncpy(ptr, "Fstatfs,", len);
            ptr += len;

            ptr = long2hex((long)path, ptr); /*filename*/
            strcpy(path, io->pc_root_path);
            if (IS_KERN_ADDR((ulong)arg0)) {
                strcat(path, (const char *)arg0);
            }
            else {
                ret = copy_from_user(path + strlen(path), (char __user *) arg0, strlen_user((const char __user *) arg0));
            }
            
            cygpath_to_win_path(path);

            *ptr++ = '/';
            len = strlen(path);/*caculate the string length include '/0'*/
            ptr = int2hex(len, ptr);

            *ptr++ = ',';
            ptr = long2hex((long)io->buf_for_user, ptr );/* statfs* buf */
            
            user_buf = (ulong)arg1;
            size = sizeof(struct remotefs_statfs);
            
            STUB_DEBUG("statfs: %s\n", path);
            
            break;
        }

        case SYSCALL_ID_READDIR:
        {
            len = strlen("Freaddir,");
            strncpy(ptr,"Freaddir,",len);
            ptr += len;

            ptr = long2hex( arg0, ptr); /* dir fd */
            *ptr++ = ',';

            ptr = long2hex( arg1, ptr); /* pos 高位还是低位？*/  
            ptr = long2hex( arg2, ptr);

            *ptr++ = ',';

            ptr = long2hex((long)io->buf_for_user, ptr);/*ent*/
            *ptr++ = ',';

            ptr = long2hex(arg4, ptr);/*len*/
            
            user_buf = (ulong)arg3;
            size = arg4;
            break;
        }

        case SYSCALL_ID_PREVDIR:
        {
            len = strlen("Fprevdir,");
            strncpy(ptr,"Fprevdir,",len);
            ptr += len;

            ptr = long2hex( arg0, ptr); /* dir fd */
            *ptr++ = ',';

            ptr = long2hex( arg1, ptr); /* pos */
            ptr = long2hex( arg2, ptr);
            *ptr++ = ',';

            ptr = long2hex((long)io->buf_for_user, ptr);/*ent*/
            *ptr++ = ',';

            ptr = long2hex(arg4, ptr);/*len*/
            
            user_buf = (ulong)arg3;
            size = sizeof(remotefs_dirent_t); //need to fixup!!
            break;
        }

        case SYSCALL_ID_SEEKDIR:
        {
            len = strlen("Fseekdir,");
            strncpy(ptr,"Fseekdir,",len);
            ptr += len;

            ptr = long2hex( arg0, ptr); /* dir fd */
            *ptr++ = ',';

            ptr = long2hex(arg1, ptr); /* offset */
            ptr = long2hex(arg2, ptr); /* offset */

            break;
        }

        case SYSCALL_ID_LASTDIR:
        {
            len = strlen("Flastdir,");
            strncpy(ptr,"Flastdir,",len);
            ptr += len;

            ptr = long2hex( arg0, ptr); /* dir fd */

            break;
        }

        case SYSCALL_ID_REWINDDIR:
        {
            len = strlen("Frewinddir,");
            strncpy(ptr,"Frewinddir,",len);
            ptr += len;

            ptr = long2hex( arg0, ptr); /* dir fd */

            break;
        }

        case SYSCALL_ID_RESET2PARENTDIR:
        {
            len = strlen("Freset2parentdir,");
            strncpy(ptr,"Freset2parentdir,",len);
            ptr += len;

            ptr = long2hex( arg0, ptr); /* dir fd */

            break;
        }

        case SYSCALL_ID_MKDIR:
        {
            len = strlen("Fmkdir,");
            strncpy(ptr,"Fmkdir,",len);

            ptr += len;
            ptr = long2hex((long)path, ptr); /*filename*/
            strcpy(path, io->pc_root_path);
            if (IS_KERN_ADDR((ulong)arg0)) {
                strcat(path, (const char *)arg0);
            }
            else {
                ret = copy_from_user(path + strlen(path), (char __user *) arg0, strlen_user((const char __user *) arg0));
            }
            
            cygpath_to_win_path(path);

            *ptr++ = '/';
            len = strlen(path);/*caculate the string length include '/0'*/
            ptr = int2hex(len, ptr);

            *ptr++ = ',';
            ptr = long2hex(arg1, ptr);/*mode*/
            
            STUB_DEBUG("mkdir: %s\n", path);

            break;
        }

        case SYSCALL_ID_RMDIR:
        {
            len = strlen("Frmdir,");
            strncpy(ptr,"Frmdir,",len);
            ptr += len;

            ptr = long2hex((long)path, ptr); /*filename*/
            strcpy(path, io->pc_root_path);
            if (IS_KERN_ADDR((ulong)arg0)) {
                strcat(path, (const char *)arg0);
            }
            else {
                ret = copy_from_user(path + strlen(path), (char __user *) arg0, strlen_user((const char __user *) arg0));
            }
            
            cygpath_to_win_path(path);

            *ptr++ = '/';
            len = strlen(path);/*caculate the string length include '/0'*/
            ptr = int2hex(len, ptr);
            
            STUB_DEBUG("rmdir: %s\n", path);
            
            break;
        }

        case SYSCALL_ID_REMOVE:
        {
            len = strlen("Fremove,");
            strncpy(ptr,"Fremove,",len);
            ptr += len;

            ptr = long2hex((long)path, ptr); /*filename*/
            strcpy(path, io->pc_root_path);
            if (IS_KERN_ADDR((ulong)arg0)) {
                strcat(path, (const char *)arg0);
            }
            else {
                ret = copy_from_user(path + strlen(path), (char __user *) arg0, strlen_user((const char __user *) arg0));
            }
            
            cygpath_to_win_path(path);

            *ptr++ = '/';
            len = strlen(path);/*caculate the string length include '/0'*/
            ptr = int2hex(len, ptr);
            
            STUB_DEBUG("remove: %s\n", path);
            
            break;
        }

        case SYSCALL_ID_RENAME:
        {
            char path_tmp[MAX_PATH];
            
            len = strlen("Frename,");
            strncpy(ptr,"Frename,",len);
            ptr += len;

            ptr = long2hex((long)path, ptr); /* old path */
            strcpy(path, io->pc_root_path);
            if (IS_KERN_ADDR((ulong)arg0)) {
                strcat(path, (const char *)arg0);
            }
            else {
                ret = copy_from_user(path + strlen(path), (char __user *) arg0, strlen_user((const char __user *) arg0));
            }
            
            cygpath_to_win_path(path);

            *ptr++ = '/';
            len = strlen(path);/*caculate the string length include '/0'*/
            ptr = int2hex(len, ptr);

            *ptr++ = ',';

            ptr = long2hex((long)path_tmp, ptr); /* new path */
            strcpy(path_tmp, io->pc_root_path);
            if (IS_KERN_ADDR((ulong)arg1)) {
                strcat(path_tmp, (const char *)arg1);
            }
            else {
                ret = copy_from_user(path_tmp + strlen(path_tmp), (char __user *) arg1, strlen_user((const char __user *) arg1));
            }
            
            cygpath_to_win_path(path_tmp);

            *ptr++ = '/';
            len = strlen(path_tmp);/*caculate the string length include '/0'*/
            ptr = int2hex(len, ptr);
            
            STUB_DEBUG("rename old path: %s\n", path);
            STUB_DEBUG("rename new path: %s\n", path_tmp);
            
            break;
        }

        case SYSCALL_ID_TRUNCATE:
        {
            len = strlen("Ftruncate,");
            strncpy(ptr,"Ftruncate,",len);

            ptr += len;
            ptr = long2hex((long)path, ptr); /* filename */
            strcpy(path, io->pc_root_path);
            if (IS_KERN_ADDR((ulong)arg0)) {
                strcat(path, (const char *)arg0);
            }
            else {
                ret = copy_from_user(path + strlen(path), (char __user *) arg0, strlen_user((const char __user *) arg0));
            }
            
            cygpath_to_win_path(path);

            *ptr++ = '/';
            len = strlen(path);/*caculate the string length include '/0'*/
            ptr = int2hex(len, ptr);

            *ptr++ = ',';

            ptr = long2hex(arg1, ptr);/* len */
            ptr = long2hex(arg2, ptr);/* len */
            
            STUB_DEBUG("truncate: %s\n", path);

            break;
        }

        case SYSCALL_ID_FTRUNCATE:
        {
            len = strlen("Fftruncate,");
            strncpy(ptr,"Fftruncate,",len);

            ptr += len;
            ptr = long2hex( arg0, ptr); /* dir fd */

            *ptr++ = ',';
            ptr = long2hex(arg1, ptr);/* len */
            ptr = long2hex(arg2, ptr);/* len */

            break;
        }
        
        case SYSCALL_ID_SYSTEM:
        {
            len = strlen("Fsystem,");
            strncpy((char*)ptr,"Fsystem,",len);
            ptr += len;
            
            
            if (IS_KERN_ADDR((ulong)arg0)) {
            	char *p = (char *)arg0;
            	
            	if (p[0] == '/' || p[0] == '\\') {
            		strcpy(path, io->pc_root_path);
                	strcat(path, (const char *)arg0);
                	cygpath_to_win_path(path);
                }
                else {
                	strcpy(path, (const char *)arg0);
                }
            }
            else {
            	char c;
            	
            	ret = copy_from_user(&c, (char __user *) arg0, 1);
            	if (c == '/' || c == '\\') {
            		strcpy(path, io->pc_root_path);
                	ret = copy_from_user(path + strlen(path), (char __user *) arg0, strlen_user((const char __user *) arg0));
                	cygpath_to_win_path(path);
                }
                else {
                	ret = copy_from_user(path, (char __user *) arg0, strlen_user((const char __user *) arg0));
                }
            }
            
            ptr = long2hex((long)path,ptr); /*string*/
            *ptr++ = '/';
            len = strlen(path);
            ptr = long2hex(len,ptr);
            
            STUB_DEBUG("system: %s\n", path);
            
            break;
        }

        case SYSCALL_ID_USDKINFO: //not implement
        {
        }
        
        default:
            release_remotefs_io_handle(io);
            printk(KERN_WARNING "not support call_id: %d\n", call_id);
            up(&stub->sd_rbs);
            return -1;
        
    }
    
    data_cnt = strlen((char *)bh->buf) - 1; //exclude '$' 
    bh->inreq->length = make_checksum((char *)bh->buf, data_cnt);
    bh->inreq->zero = 0;
	bh->state = BUF_STATE_FULL;
	
	io->complete = &completion;
	
	//usb_ep_dequeue(stub->bulk_out, bh->outreq);
	//usb_ep_disable(stub->bulk_out);

#if STUB_USE_SINGLE_STEP
	//usb_ep_dequeue(stub->bulk_in, bh_tmp->inreq);

	start_transfer(stub, stub->bulk_in, bh->inreq,
		       &bh->inreq_busy, &bh->state);
	//wait_for_completion_timeout(io->complete, msecs_to_jiffies(5000));
	ret = wait_event_interruptible_timeout(stub->io_call_wait, (io_calling_busy == 0), msecs_to_jiffies(5000));
	if (ret == 0) {
		printk("%s:%d\n", __func__, __LINE__);
	}
	//printk("io->ret:%d\n", io->ret);
#else
	start_transfer(stub, stub->bulk_in, bh->inreq,
		       &bh->inreq_busy, &bh->state);
	wait_for_completion_timeout(io->complete, msecs_to_jiffies(15000));
#endif
	
    if (io->ret >= 0) {
        switch (call_id) {
            case SYSCALL_ID_READ:
            case SYSCALL_ID_STAT:   
            case SYSCALL_ID_FSTAT:     
            case SYSCALL_ID_STATFS:
            case SYSCALL_ID_READDIR:
            case SYSCALL_ID_PREVDIR:
            {
                if (IS_KERN_ADDR(user_buf)) {
                    memcpy((void *)user_buf, io->buf_for_user, size);   
                } 
                else {
                    ret = copy_to_user((void __user *)user_buf, io->buf_for_user, size);   
                }  
            }
            break;
        }
    }

	retv = release_remotefs_io_handle(io); 
    up(&stub->sd_rbs);
    return retv;
}

static int send_status(struct stub_dev *stub)
{
	struct stub_buffhd *bh;
	int rc;
	char temp[32];
	
	if (stub->response_phase == GDB_NOT_SEND_STATUS) return 0;
	
	bh = stub->next_buffhd_to_fill;
	while (bh->state != BUF_STATE_EMPTY) {
		rc = sleep_thread(stub);
		if (rc)
			return rc;

	}
	smp_rmb();
	
	if (transport_is_vendor()) {
    	if(stub->response_phase != GDB_ACK_OK){
    		fill_err_packet(temp, stub->response_phase);
    	}
    	else {
    	    strcpy(temp, "OK");  
    	    //STUB_DEBUG("send OK, %s, %d\n", __func__, __LINE__); 
    	}
    	
    	bh->inreq->length = make_packet(temp, (char *)bh->buf);
    	bh->state = BUF_STATE_FULL;
    	bh->inreq->zero = 0;
    	start_transfer(stub, stub->bulk_in, bh->inreq,
    			       &bh->inreq_busy, &bh->state);
        stub->next_buffhd_to_fill = bh->next;

	rc = sleep_thread(stub);
	if (rc)
		return rc;
    }
    else {
        printk(KERN_ERR "not support vendor\n");   
    }

	return 0;
}

static void remote_io_init(struct stub_dev * stub)
{
    spin_lock_init(&stub->io.io_lock);
    //init_waitqueue_head(&stub->io.remotefs_wait_queue);
    stub->io.complete = NULL;
    stub->io.remotefs_call_handle = 0;
    stub->io.remote_io_exit = 0;
    stub->io.path_ready = 0;
    stub->io.load_ready = 0;
    memset(stub->io.pc_root_path, 0, MAX_ROOT_PATH);
    
}

//extern volatile int __remotefs_call;

static void handle_gdb_ext_cmd(struct stub_dev * stub, struct stub_buffhd *current_bh)
{
    char opcode = 0;
    char *src = (char *)current_bh->buf + 2;
    char *ptr = src;
    
    opcode = *ptr;
    ptr++;      
    switch(opcode)
    {
    case 'a':
        DBG(stub, "ext a\n");
        //gdb_ext_getconfig(ptr,dst);
        break;
    case 'C':
        DBG(stub, "ext C\n");
        //gdb_ext_flushcache(ptr,dst);
        break;
    case 'c':
        DBG(stub, "ext c\n");
        //gdb_ext_readcache(ptr,dst);
        break;
    case 'T':
        DBG(stub, "ext T\n");
        //gdb_ext_readtlb(ptr,dst);
        break;
    case 'k':
        DBG(stub, "ext k\n");
        //gdb_ext_rread(ptr,dst);
        break;
    case 'K':
        DBG(stub, "ext K\n");
        //gdb_ext_rwrite(ptr,dst);
        break;
    case 'P':
        DBG(stub, "ext P\n");
        //gdb_ext_configprof(ptr,dst);
        break;
    case 'L':
        DBG(stub, "ext L\n");
        //gdb_ext_setloadtype(ptr,dst);
        
        hex2byte((u8 **)&ptr, &stub->io.load_flag);
        if (stub->io.load_flag < 1 || stub->io.load_flag > 5) {
            stub->response_phase = GDB_ACK_ERR_COMD_NOSUPP;   
            stub->io.load_ready = 0;
        }
        else {
            stub->io.load_ready = 1;
        }
        break;
    case 'I':
    {    
        int len;
        
        DBG(stub, "ext I\n");
        hex2byte((u8 **)&ptr, &stub->io.path_type);
        len = get_datalen_from_command(ptr);
        memset(stub->io.pc_root_path, 0, MAX_ROOT_PATH);
        switch (stub->io.path_type & 0xF0) {
        case 0:
            if (ptr[0] == '/') {
                memcpy(stub->io.pc_root_path, ptr, len); 
//                if (!(stub->io.pc_root_path[len - 1] != '/')) 
//                    stub->io.pc_root_path[len] = '/';  
            }
            else {
                memcpy(stub->io.pc_root_path, ptr, len);
//                if (!(stub->io.pc_root_path[len - 1] == '\\')) 
//                    stub->io.pc_root_path[len] = '\\';   
            }
            
            if (stub->io.pc_root_path[len - 1] == '\\' || stub->io.pc_root_path[len - 1] == '/') 
                stub->io.pc_root_path[len - 1] = '\0';
            
            stub->io.path_ready = 1;
        break;
        
        case 0x10:
            memcpy(stub->io.pc_root_path, ptr, len);
            strcat(stub->io.pc_root_path, "/images");
            stub->io.path_ready = 1;
        break;
        
        case 0x20:
        case 0x30:
            memcpy(stub->io.pc_root_path, ptr, len);
//            if (!(stub->io.pc_root_path[len - 1] == '\\' || stub->io.pc_root_path[len - 1] == '/')) 
//                stub->io.pc_root_path[len] = '/';
            if (stub->io.pc_root_path[len - 1] == '\\' || stub->io.pc_root_path[len - 1] == '/') 
                stub->io.pc_root_path[len - 1] = '\0';
            stub->io.path_ready = 1;
        break;
        
        default:
            stub->response_phase = GDB_ACK_ERR_COMD_NOSUPP;
            stub->io.path_ready = 0;
        }
        STUB_DEBUG("stub->io.pc_root_path : %s, max size: %u\n", stub->io.pc_root_path, sizeof(stub->io.pc_root_path));   
    }
        break;
    case 'B':
        DBG(stub, "ext B\n");
        //gdb_ext_enablebp(ptr,dst);
        break;
    case 'Q':
        DBG(stub, "ext Q\n");
        //gdb_ext_queryloaded(ptr,dst);
        break;
   case 'S':
        DBG(stub, "ext S\n");
        //gdb_ext_setusdkinfo(ptr,dst);
        break;        
    default:
        stub->response_phase = GDB_ACK_ERR_COMD_NOSUPP;
        break;
    }
}

static int handle_gdb_cmd(struct stub_dev * stub)
{
	int rc = 0;
	char *ptr;
	struct stub_buffhd *bh;
	long addr;
	int length;
	int amount;
	int i;
	
	struct stub_buffhd *current_buffhd = stub->current_buffhd;    
	stub->current_buffhd = NULL;
	
	//stub->data_dir = DATA_DIR_NONE;
	//STUB_DEBUG("stub->cmnd[1]: %c, %s, %d\n", stub->cmnd[1], __func__, __LINE__);
	//printk("stub->cmnd[1]: %c\n", stub->cmnd[1]);

	switch (stub->cmnd[1]) {

    	default:
    		printk(KERN_ERR "unkown gdb cmd: %02X\n", stub->cmnd[1]);
    		bh = stub->next_buffhd_to_fill;
    		while (bh->state != BUF_STATE_EMPTY) {
    			rc = sleep_thread(stub);
    			if (rc)
    				return rc;
    		}
    		smp_rmb();
    	    
		bh->inreq->length = make_packet("E00", (char *)bh->buf);        
		stub->residue = 0;
    		bh->inreq->zero = 0;
    		bh->state = BUF_STATE_FULL;
    
    		/* Send this buffer */
    		start_transfer(stub, stub->bulk_in, bh->inreq,
    			       &bh->inreq_busy, &bh->state);
    		stub->next_buffhd_to_fill = bh->next;
    		stub->response_phase = GDB_NOT_SEND_STATUS;
    		break;
    	
        case '?':
		DBG(stub, "[gdb cmd head]:?\n");
		break;

		case '!':
		    DBG(stub, "[gdb cmd head]:!\n");
		    /* Wait for the next buffer to become available */
		    
    		bh = stub->next_buffhd_to_fill;
    		while (bh->state != BUF_STATE_EMPTY) {
    			rc = sleep_thread(stub);
    			if (rc)
    				return rc;
    		}
    		smp_rmb();
    	    
    	    	bh->inreq->length = make_packet("E00", (char *)bh->buf);        
            	stub->residue = 0;
    		bh->inreq->zero = 0;
    		bh->state = BUF_STATE_FULL;
    
    		/* Send this buffer */
    		start_transfer(stub, stub->bulk_in, bh->inreq,
    			       &bh->inreq_busy, &bh->state);
    		stub->next_buffhd_to_fill = bh->next;
    		stub->response_phase = GDB_NOT_SEND_STATUS;
			break;
	    case 'q':
	        {
	            u32 size = mod_data.buflen;//mod_data.buflen; unit: byte, not sure
	            char tmp_buf[128];
	            DBG(stub, "[gdb cmd head]:q\n");
	            
	            if(!strncmp(stub->cmnd + 2,"Supported", 9))
	            {
	                bh = stub->next_buffhd_to_fill;
            		while (bh->state != BUF_STATE_EMPTY) {
            			rc = sleep_thread(stub);
            			if (rc)
            				return rc;
            		}
            		smp_rmb();
	                
	                snprintf(tmp_buf, sizeof(tmp_buf), "PacketSize=%X", size);
	                bh->inreq->length = make_packet(tmp_buf, (char *)bh->buf); 
	                bh->inreq->zero = 0;
	                stub->residue = 0;
	                bh->state = BUF_STATE_FULL;

            		/* Send this buffer */
            		start_transfer(stub, stub->bulk_in, bh->inreq,
            			       &bh->inreq_busy, &bh->state);
            		stub->next_buffhd_to_fill = bh->next;
            		stub->response_phase = GDB_NOT_SEND_STATUS;
	            }
	            else {
	                stub->response_phase = GDB_ACK_ERR_COMD_NOSUPP;
	            }
	        }
	        
	        break;

		/*
		 * Detach debugger;
		 */
		case 'k':
		    DBG(stub, "[gdb cmd head]:k\n");
		case 'D':
		    DBG(stub, "[gdb cmd head]:D\n");
			break;

		/*
		 * Return the value of the CPU registers
		 */
		case 'g':
		    DBG(stub, "[gdb cmd head]:g\n");
			break;

		/*
		 * set the value of the CPU registers - return OK
		 */
		case 'G':
		    DBG(stub, "[gdb cmd head]:G\n");
		    break;
		/*
		 * Return the value of the certain CPU register 
		 */
		case 'p':
		    break;
		case 'P':
		    break;
        case 'x':
        {
            ptr = stub->cmnd + 2;
            
            if (hex2long(&ptr, &addr) && *ptr++ == ',' && hex2int(&ptr, &length) && *ptr++ == '#')
            {
                bh = stub->next_buffhd_to_fill;
        		while (bh->state != BUF_STATE_EMPTY) {
        			rc = sleep_thread(stub);
        			if (rc)
        				return rc;
        		}
        		smp_wmb();
        		
        		((char *)bh->buf)[0] = '$';
        		bh->inreq->length = 1;
        		bh->inreq->zero = 0;
        		
        		start_transfer(stub, stub->bulk_in, bh->inreq,
        			       &bh->inreq_busy, &bh->state);
        		stub->next_buffhd_to_fill = bh->next;
        		smp_wmb();
			rc = sleep_thread(stub);
			if (rc)
				return rc;
        		smp_wmb();
        		
        		while (length > 0) {
        		    
        		    bh = stub->next_buffhd_to_fill;
            		while (bh->state != BUF_STATE_EMPTY) {
            			rc = sleep_thread(stub);
            			if (rc)
            				return rc;
            		}
            		smp_wmb();
            		
            		amount = min(length, (int)mod_data.buflen); 
            		memcpy((void *)bh->buf, (char *)addr, amount);   
            		addr += amount;
            		length -= amount;
            		bh->inreq->length = amount;
            		bh->inreq->zero = 0;
            		bh->state = BUF_STATE_FULL;
                    start_transfer(stub, stub->bulk_in, bh->inreq,
            			       &bh->inreq_busy, &bh->state);
            		stub->next_buffhd_to_fill = bh->next;
        		}
        		
        		bh = stub->next_buffhd_to_fill;
#if 0
        		while (bh->state != BUF_STATE_EMPTY) {
        			rc = sleep_thread(stub);
        			if (rc)
        				return rc;
        		}
#endif
        		smp_wmb();
			rc = sleep_thread(stub);
			if (rc)
				return rc;
        		smp_wmb();
        		
        		memcpy(bh->buf, "#00", 3);
        		bh->inreq->length = 3;
        		bh->inreq->zero = 0;
        		bh->state = BUF_STATE_FULL;

        		start_transfer(stub, stub->bulk_in, bh->inreq,
            			       &bh->inreq_busy, &bh->state);
            	stub->next_buffhd_to_fill = bh->next;
            	
            	stub->response_phase = GDB_NOT_SEND_STATUS;
            }
            else
            {
                stub->response_phase = GDB_ACK_ERR_COMD_NOSUPP;
            }
        }
            break;
		
		/*
		 * XAA..AA,LLLL: Write LLLL escaped binary bytes at address AA.AA
		 */
		case 'X':
		{    
		    DBG(stub, "[gdb cmd head]:X\n");
			ptr = (char *)current_buffhd->buf + 2;
            
			if (hex2long(&ptr, &addr) && *ptr++ == ',' && hex2int(&ptr, &length) && *ptr++ == ':') 
			{
                
                	amount = min(length, (int)(mod_data.buflen - ((ulong)ptr - (ulong)current_buffhd->buf)));
                	addr = (long)ebin2mem(ptr, (u8 *)addr, amount, 1);
                	length -= amount; //handle this task
                	current_buffhd->state = BUF_STATE_EMPTY;

                while (length > 0) {
                    
                    	bh = stub->next_buffhd_to_fill;
            		while (bh->state != BUF_STATE_EMPTY) {
            			rc = sleep_thread(stub);
            			if (rc)
            				return rc;
            		}
            		smp_wmb();
            		
                    	amount = min((int)mod_data.buflen, length);
                    	bh->outreq->length = amount;
                    	bh->outreq->zero = 0;
            		start_transfer(stub, stub->bulk_out, bh->outreq,
            			       &bh->outreq_busy, &bh->state);
            		stub->next_buffhd_to_fill = bh->next;
            		
            		while (bh->state != BUF_STATE_FULL) {
            			rc = sleep_thread(stub);
            			if (rc)
            				return rc;
            		}
            		smp_wmb();	
            		
                    	addr = (long)ebin2mem(ptr, (u8 *)addr, amount, 1);
            		//printk("X amount: %d, actual done: %d\n", amount, addr - tmp_addr);
            		length -= amount; //handle this task
                    	bh->state = BUF_STATE_EMPTY;  //when we done with the buf, release it  		   
            	}
            	
            	bh = stub->next_buffhd_to_fill;
                while (bh->state != BUF_STATE_EMPTY) {
        			rc = sleep_thread(stub);
        			if (rc)
        				return rc;
        		}
        		smp_wmb();
        		
        		((char *)bh->buf)[0] = '+';
        		bh->inreq->length = 1;
			stub->residue = 0;
			bh->inreq->zero = 0;
        		bh->state = BUF_STATE_FULL;
        
        		/* Send this buffer */
        		start_transfer(stub, stub->bulk_in, bh->inreq,
        			       &bh->inreq_busy, &bh->state);
        		stub->next_buffhd_to_fill = bh->next;
			}
			else
			{
				printk(KERN_ERR "X command error\n");
				current_buffhd->state = BUF_STATE_EMPTY;
				stub->state = GDB_ACK_ERR_COMD_NOSUPP;
			}
		}
			break;  
        
        case 'Y':
            {
                int dma_chan = -1;
                ptr = (char *)stub->cmnd + 2;
                if (hex2long(&ptr, &addr) && (*ptr++ == ',') && hex2int(&ptr, &length) && (*ptr++ == ',') && hex2int(&ptr,(int*)&dma_chan))
                {                    
                    if(likely(1))
                    {
                        bh = stub->next_buffhd_to_fill;
                        while (bh->state != BUF_STATE_EMPTY) {
                			rc = sleep_thread(stub);
                			if (rc)
                				return rc;
                		}
                		smp_wmb();
                		
                		memcpy(bh->buf, "$OK#9a", 6);
                		bh->inreq->length = 6;
                        stub->residue = 0;
                        bh->inreq->zero = 0;
                		bh->state = BUF_STATE_FULL;
                
                		/* Send this buffer */
                		start_transfer(stub, stub->bulk_in, bh->inreq,
                			       &bh->inreq_busy, &bh->state);
                		stub->next_buffhd_to_fill = bh->next;
                        
                        bh = stub->next_buffhd_to_fill;
                        while (bh->state != BUF_STATE_EMPTY) {
                			rc = sleep_thread(stub);
                			if (rc)
                				return rc;
                		}
                		smp_wmb();
                		
                		bh->outreq->length = 1;
                        stub->residue = 0;
                        bh->outreq->zero = 0;
                
                		start_transfer(stub, stub->bulk_out, bh->outreq,
                			       &bh->outreq_busy, &bh->state);
                		stub->next_buffhd_to_fill = bh->next;
                		while (bh->state != BUF_STATE_FULL) {
                			rc = sleep_thread(stub);
                			if (rc)
                				return rc;
                		}
                		smp_wmb();
                        
                        if(((char *)bh->buf)[0] == '+') {
                            bh->state = BUF_STATE_EMPTY;
                            
                            //before using dma, wait for all the buffer empty
                            bh = stub->next_buffhd_to_fill;
                            for (i = 0; i < NUM_BUFFERS; i++) {
                                while (bh->state != BUF_STATE_EMPTY) {
                        			rc = sleep_thread(stub);
                        			if (rc)
                        				return rc;
                        		}
                        		smp_wmb(); 
                        		bh = bh->next;  
                            }                            
                            while (length > 0) {

                                amount = min(length, (int)mod_data.buflen);
                                bh = stub->next_buffhd_to_fill;
                                while (bh->state != BUF_STATE_EMPTY) {
                        			rc = sleep_thread(stub);
                        			if (rc)
                        				return rc;
                        	}
                        	smp_wmb();
                        		
                        	bh->outreq->length = amount;
                                stub->residue = 0;
                                bh->outreq->zero = 0;
                                
                                start_transfer(stub, stub->bulk_out, bh->outreq,
                			            &bh->outreq_busy, &bh->state);
                        		stub->next_buffhd_to_fill = bh->next;
                        		while (bh->state != BUF_STATE_FULL) {
                        			rc = sleep_thread(stub);
                        			if (rc)
                        				return rc;
                        		}
                        		smp_wmb();
                        		
                        		memcpy((void *)addr, bh->buf, amount);   
                        		bh->state = BUF_STATE_EMPTY;
                        		length -= amount;
                        		addr += amount;   
                            }
                        }
                        else {
                            bh->state = BUF_STATE_EMPTY;
                            stub->state = GDB_ACK_ERR_COMD_NOSUPP;
                        }
                    }
                    else {
                        stub->state = GDB_ACK_ERR_COMD_NOSUPP;
                    }
                }
                else {
                    stub->state = GDB_ACK_ERR_COMD_NOSUPP;
                }
                break;
            }
        
        /*
		 * mAA..AA,LLLL  Read LLLL bytes at address AA..AA
		 */
		case 'm':
        {
            char *input_buffer = stub->cmnd;
            
            DBG(stub, "[gdb cmd head]:m\n");
		    ptr = &input_buffer[2];
            
            //can not handle the situation when length > (mod_data.buflen - 4) / 2
            if ((hex2long(&ptr, &addr)) && (*ptr++ == ',') && (hex2int(&ptr, &length)))
            {
                
                bh = stub->next_buffhd_to_fill;
                while (bh->state != BUF_STATE_EMPTY) {
        			rc = sleep_thread(stub);
        			if (rc)
        				return rc;
        		}
        		smp_wmb();
        		
        		amount = min(length, (int)mod_data.buflen - 4);
        		//printk("m length : %d, amount: %d\n", length, amount);
        		((char *)bh->buf)[0] = '$';
                 mem2hex((char *)addr, (char *)bh->buf + 1, amount, 1);
                
                if ((amount + amount + 4) > mod_data.buflen) {
                    stub->response_phase = GDB_ACK_ERR_COMD_LENGTH;   
                }
                else {
                    bh->inreq->length = make_checksum((char *)bh->buf, amount << 1);
                    stub->residue = 0;
                    bh->inreq->zero = 0;
            		bh->state = BUF_STATE_FULL;
                    
                    start_transfer(stub, stub->bulk_in, bh->inreq,
            			       &bh->inreq_busy, &bh->state);
            		stub->next_buffhd_to_fill = bh->next;
            		stub->response_phase = GDB_NOT_SEND_STATUS;
            	}
            }
            else
            {
                stub->response_phase = GDB_ACK_ERR_PARSE_INVALID;
            }
            
            current_buffhd->state = BUF_STATE_EMPTY;
        }
			break;
        
		/*
		 * MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK
		 */
		case 'M':
		{
		    char *input_buffer = (char *)current_buffhd->buf;
		    
            DBG(stub, "[gdb cmd head]:M\n");
            
            ptr = &input_buffer[2];
            
            //not support the condition that data in two or more buffer
            if ((hex2long(&ptr, &addr)) && (*ptr++ == ',') && (hex2int(&ptr, &length)) && (*ptr++ == ':'))
            {
        	amount = min(length, (int)(mod_data.buflen - (ptr - (char *)current_buffhd->buf)));
                hex2mem(ptr, (u8 *)addr, amount, 1);
            }
            else
            {
				stub->state = GDB_ACK_ERR_COMD_NOSUPP;
            }
            
            current_buffhd->state = BUF_STATE_EMPTY;
            
            bh = stub->next_buffhd_to_fill;
            while (bh->state != BUF_STATE_EMPTY) {
    			rc = sleep_thread(stub);
    			if (rc)
    				return rc;
    		}
    		smp_wmb();
    		
    		((char *)bh->buf)[0] = '+';
    		bh->inreq->length = 1;
            stub->residue = 0;
            bh->inreq->zero = 0;
    		bh->state = BUF_STATE_FULL;
    
    		/* Send this buffer */
    		start_transfer(stub, stub->bulk_in, bh->inreq,
    			       &bh->inreq_busy, &bh->state);
    		stub->next_buffhd_to_fill = bh->next;
	    }    
		break;

		/*
		 * cAA..AA    Continue at address AA..AA(optional)
		 */
		case 's':
		    break;
		case 'c':
		    DBG(stub, "[gdb cmd head]:c or s\n");
		    if (stub->io.load_ready && stub->io.path_ready) {
		        //__remotefs_call = (long)remotefs_call;
		        io = get_remote_io();
		        setup_remote_io_ops(io);
		        __remote_io = (long)io;
		        STUB_DEBUG("[stub] remote io ready\n");
			remote_firt_call = 1;
		    }
		    else {
		        //__remotefs_call = 0;  
		        printk(KERN_WARNING "[stub] remote io is not ready\n"); 
		    }
		    
			break;
		/*
		 * File-I/O
		 */
        case 'F':
            {
                char *input_buffer = stub->cmnd;
                if (input_buffer[2] != '-')
                {
                    ptr = &input_buffer[2];
                    if(gethexnum((unsigned char **)&ptr) <= 8)
                    {
                        /*32bit*/
                        int retval32 = 0;
                        ptr = &input_buffer[2];
                        hex2int(&ptr, &retval32);
                        stub->io.ret = (loff_t)retval32;
                    }
                    else
                    {
                        /*64bit*/
                        loff_t retval32 = 0;
                        ptr = &input_buffer[2];
                        hex2longlong((unsigned char **)&ptr, &retval32);
                        stub->io.ret = retval32;
                    }
		}
                else
                {
                    loff_t retval64 = 0;
                    ptr = &input_buffer[3];
                    hex2longlong((unsigned char **)&ptr, &retval64);
                    if (retval64 > 0)
                    {
                        retval64 = 0 - retval64;
                    }
                    
                    stub->io.ret = retval64;
		    printk("%s:%d\n", __func__, __LINE__);
		    printk("stub->io.ret:%d\n", (unsigned int)stub->io.ret);
                }
	
		io_calling_busy = 0;
		wake_up(&stub->io_call_wait);

                //if (io->complete) 
			//complete(io->complete);
                stub->response_phase = GDB_NOT_SEND_STATUS;
            }
            break;
       	/*
		 * set or remove a breakpoint
		 */
        case 'z':
		case 'Z':
			break;
	case '*':
	    handle_gdb_ext_cmd(stub, current_buffhd);
		break;

		/*
		 * Reset the whole machine.
		 */
		case 'r':
		    DBG(stub, "[gdb cmd head]:r\n");
			break;    
    }	

	rc = sleep_thread(stub);
	if (rc)
		return rc;
	
	return 0;	
}

//because it's not very clear the protocol of stub, so, it is a test. The checksum has not been tested.
static int get_next_command(struct stub_dev *stub) 
{
	struct stub_buffhd	*bh;
	int			rc = 0;
    
	stub->response_phase = GDB_ACK_OK;

	/* Wait for the next buffer to become available */
	while (1) {
		bh = stub->next_buffhd_to_fill;
		while (bh->state != BUF_STATE_EMPTY) {
			rc = sleep_thread(stub);
			if (rc)
				return rc;
		}

		set_bulk_out_req_length(stub, bh, mod_data.buflen);
		bh->outreq->short_not_ok = 1;
		start_transfer(stub, stub->bulk_out, bh->outreq,
				&bh->outreq_busy, &bh->state);
		stub->next_buffhd_to_fill = bh->next;
		/* We will drain the buffer in software, which means we
		 * can reuse it for the next filling.  No need to advance
		 * next_buffhd_to_fill. */

		/* Wait for the commad to arrive */
		while (bh->state != BUF_STATE_FULL) {
			rc = sleep_thread(stub);
			if (rc) {
				return rc;
			}
		}
		smp_rmb();

#if STUB_USE_SINGLE_STEP
		if ((__remote_io != 0) && 
		  (((char *)bh->buf)[0] == '+') && (remote_firt_call != 0)) {
			//sleep_thread(stub);
			remote_waiting = 1;
			wait_for_completion(&stub->thread_notifier);
			/* The cleanup routine waits for this completion also */
			//continue;
		}
#endif
		
		if (((char *)bh->buf)[0] != '$') {
		    bh->state = BUF_STATE_EMPTY;
		    continue;
		} else {
			if (((char *)bh->buf)[1] == 'F') {
				cmd_bk[1] = 'F';
				cmd_bk[2] = ((char *)bh->buf)[2];
				cmd_bk[3] = ((char *)bh->buf)[3];
			} else {
				cmd_bk[1] = '0';
				cmd_bk[2] = '0';
			}
		}
		
		rc = received_gdb_cmd(stub, bh);
		stub->current_buffhd = bh;
		break;
	}

	return rc;
}

/*-------------------------------------------------------------------------*/

static int enable_endpoint(struct stub_dev *stub, struct usb_ep *ep,
		const struct usb_endpoint_descriptor *d)
{
	int	rc;

	ep->driver_data = stub;
  ep->desc = d;
	rc = usb_ep_enable(ep);
	if (rc)
		ERROR(stub, "can't enable %s, result %d\n", ep->name, rc);
	return rc;
}

static int alloc_request(struct stub_dev *stub, struct usb_ep *ep,
		struct usb_request **preq)
{
	*preq = usb_ep_alloc_request(ep, GFP_ATOMIC);
	if (*preq)
		return 0;
	ERROR(stub, "can't allocate request for %s\n", ep->name);
	return -ENOMEM;
}

/*
 * Reset interface setting and re-init endpoint state (toggle etc).
 * Call with altsetting < 0 to disable the interface.  The only other
 * available altsetting is 0, which enables the interface.
 */
static int do_set_interface(struct stub_dev *stub, int altsetting)
{
	int	rc = 0;
	int	i;
	const struct usb_endpoint_descriptor	*d;

	if (stub->running)
		DBG(stub, "reset interface\n");

reset:
	/* Deallocate the requests */
	for (i = 0; i < NUM_BUFFERS; ++i) {
		struct stub_buffhd *bh = &stub->buffhds[i];

		if (bh->inreq) {
			usb_ep_free_request(stub->bulk_in, bh->inreq);
			bh->inreq = NULL;
		}
		if (bh->outreq) {
			usb_ep_free_request(stub->bulk_out, bh->outreq);
			bh->outreq = NULL;
		}
	}
	if (stub->intreq) {
		usb_ep_free_request(stub->intr_in, stub->intreq);
		stub->intreq = NULL;
	}
	if (stub->remote_io_buffhd.inreq) {
	    usb_ep_free_request(stub->bulk_in, stub->remote_io_buffhd.inreq);
		stub->remote_io_buffhd.inreq = NULL;   
	}

	/* Disable the endpoints */
	if (stub->bulk_in_enabled) {
		usb_ep_disable(stub->bulk_in);
		stub->bulk_in_enabled = 0;
	}
	if (stub->bulk_out_enabled) {
		usb_ep_disable(stub->bulk_out);
		stub->bulk_out_enabled = 0;
	}
	if (stub->intr_in_enabled) {
		usb_ep_disable(stub->intr_in);
		stub->intr_in_enabled = 0;
	}

	stub->running = 0;
	if (altsetting < 0 || rc != 0)
		return rc;

	DBG(stub, "set interface %d\n", altsetting);

	/* Enable the endpoints */
	d = ep_desc(stub->gadget, &fs_bulk_in_desc, &hs_bulk_in_desc);
	if ((rc = enable_endpoint(stub, stub->bulk_in, d)) != 0)
		goto reset;
	stub->bulk_in_enabled = 1;

	d = ep_desc(stub->gadget, &fs_bulk_out_desc, &hs_bulk_out_desc);
	if ((rc = enable_endpoint(stub, stub->bulk_out, d)) != 0)
		goto reset;
	stub->bulk_out_enabled = 1;
	stub->bulk_out_maxpacket = le16_to_cpu(d->wMaxPacketSize);
	clear_bit(IGNORE_BULK_OUT, &stub->atomic_bitflags);

	if (transport_is_vendor()) {
		d = ep_desc(stub->gadget, &fs_intr_in_desc, &hs_intr_in_desc);
		if ((rc = enable_endpoint(stub, stub->intr_in, d)) != 0)
			goto reset;
		stub->intr_in_enabled = 1;
	}

	/* Allocate the requests */
	for (i = 0; i < NUM_BUFFERS; ++i) {
		struct stub_buffhd	*bh = &stub->buffhds[i];

		if ((rc = alloc_request(stub, stub->bulk_in, &bh->inreq)) != 0)
			goto reset;
		if ((rc = alloc_request(stub, stub->bulk_out, &bh->outreq)) != 0)
			goto reset;
		bh->inreq->buf = bh->outreq->buf = bh->buf;
		bh->inreq->context = bh->outreq->context = bh;
		bh->inreq->complete = bulk_in_complete;
		bh->outreq->complete = bulk_out_complete;
	}
	if (transport_is_vendor()) {
		if ((rc = alloc_request(stub, stub->intr_in, &stub->intreq)) != 0)
			goto reset;
		stub->intreq->complete = intr_in_complete;
	}
	
	if ((rc = alloc_request(stub, stub->bulk_in, &stub->remote_io_buffhd.inreq)) != 0)
		goto reset;
	stub->remote_io_buffhd.inreq->buf = stub->remote_io_buffhd.buf;
	stub->remote_io_buffhd.inreq->context = &stub->remote_io_buffhd;
	stub->remote_io_buffhd.inreq->complete = remote_io_complete;
    
    //remote_io_init(stub);
    
    stub->io.remote_io_exit = 1;
    //while (waitqueue_active(&stub->io.remotefs_wait_queue));
    
    stub->io.complete = NULL;
    stub->io.remotefs_call_handle = 0;
    stub->io.remote_io_exit = 0;
    stub->io.path_ready = 0;
    stub->io.load_ready = 0;
    memset(stub->io.pc_root_path, 0, MAX_ROOT_PATH);
    
	stub->running = 1;

	return rc;
}

/*
 * Change our operational configuration.  This code must agree with the code
 * that returns config descriptors, and with interface altsetting code.
 *
 * It's also responsible for power management interactions.  Some
 * configurations might not work with our current power sources.
 * For now we just assume the gadget is always self-powered.
 */
static int do_set_config(struct stub_dev *stub, u8 new_config)
{
	int	rc = 0;

	/* Disable the single interface */
	if (stub->config != 0) {
		DBG(stub, "reset config\n");
		stub->config = 0;
		rc = do_set_interface(stub, -1);
	}

	/* Enable the interface */
	if (new_config != 0) {
		stub->config = new_config;
		if ((rc = do_set_interface(stub, 0)) != 0)
			stub->config = 0;	// Reset on errors
		else {
			char *speed;

			switch (stub->gadget->speed) {
			case USB_SPEED_LOW:	speed = "low";	break;
			case USB_SPEED_FULL:	speed = "full";	break;
			case USB_SPEED_HIGH:	speed = "high";	break;
			default: 		speed = "?";	break;
			}
			INFO(stub, "%s speed config #%d\n", speed, stub->config);
		}
	}
	return rc;
}


/*-------------------------------------------------------------------------*/

static void stub_handle_exception(struct stub_dev *stub)
{
	siginfo_t		info;
	int			sig;
	int			i;
	int			num_active;
	struct stub_buffhd	*bh;
	enum stub_state		old_state;
	u8			new_config;
	unsigned int		exception_req_tag;
	int			rc;

	/* Clear the existing signals.  Anything but SIGUSR1 is converted
	 * into a high-priority EXIT exception. */
	for (;;) {
		sig = dequeue_signal_lock(current, &current->blocked, &info);
		if (!sig)
			break;
		if (sig != SIGUSR1) {
			if (stub->state < STUB_STATE_EXIT)
				DBG(stub, "Main thread exiting on signal\n");
			raise_exception(stub, STUB_STATE_EXIT);
		}
	}

	/* Cancel all the pending transfers */
	if (stub->intreq_busy)
		usb_ep_dequeue(stub->intr_in, stub->intreq);
	for (i = 0; i < NUM_BUFFERS; ++i) {
		bh = &stub->buffhds[i];
		if (bh->inreq_busy)
			usb_ep_dequeue(stub->bulk_in, bh->inreq);
		if (bh->outreq_busy)
			usb_ep_dequeue(stub->bulk_out, bh->outreq);
	}

	/* Wait until everything is idle */
	for (;;) {
		num_active = stub->intreq_busy;
		for (i = 0; i < NUM_BUFFERS; ++i) {
			bh = &stub->buffhds[i];
			num_active += bh->inreq_busy + bh->outreq_busy;
		}
		if (num_active == 0)
			break;
		if (sleep_thread(stub))
			return;
	}

	/* Clear out the controller's fifos */
	if (stub->bulk_in_enabled)
		usb_ep_fifo_flush(stub->bulk_in);
	if (stub->bulk_out_enabled)
		usb_ep_fifo_flush(stub->bulk_out);
	if (stub->intr_in_enabled)
		usb_ep_fifo_flush(stub->intr_in);

	/* Reset the I/O buffer states and pointers, the SCSI
	 * state, and the exception.  Then invoke the handler. */
	spin_lock_irq(&stub->lock);

	for (i = 0; i < NUM_BUFFERS; ++i) {
		bh = &stub->buffhds[i];
		bh->state = BUF_STATE_EMPTY;
	}
	stub->next_buffhd_to_fill = stub->next_buffhd_to_drain =
			&stub->buffhds[0];
			
	stub->remote_io_buffhd.state = BUF_STATE_EMPTY;

	exception_req_tag = stub->exception_req_tag;
	new_config = stub->new_config;
	old_state = stub->state;

	if (old_state == STUB_STATE_ABORT_BULK_OUT)
		stub->state = STUB_STATE_STATUS_PHASE;
	else {
		stub->state = STUB_STATE_IDLE;
	}
	spin_unlock_irq(&stub->lock);

	/* Carry out any extra actions required for the exception */
	switch (old_state) {
	default:
		break;

	case STUB_STATE_ABORT_BULK_OUT:
		STUB_DEBUG("STUB_STATE_ABORT_BULK_OUT");
		send_status(stub);
		spin_lock_irq(&stub->lock);
		if (stub->state == STUB_STATE_STATUS_PHASE)
			stub->state = STUB_STATE_IDLE;
		spin_unlock_irq(&stub->lock);
		break;

	case STUB_STATE_RESET:
		/* In case we were forced against our will to halt a
		 * bulk endpoint, clear the halt now.  (The SuperH UDC
		 * requires this.) */
		STUB_DEBUG("STUB_STATE_RESET");
		stub->response_phase = 0;
		if (test_and_clear_bit(IGNORE_BULK_OUT, &stub->atomic_bitflags))
			usb_ep_clear_halt(stub->bulk_in);

		if (transport_is_vendor()) {
			if (stub->ep0_req_tag == exception_req_tag)
				ep0_queue(stub);	// Complete the status stage

		} else {
			send_status(stub);	// Status by interrupt pipe
		}

		/* Technically this should go here, but it would only be
		 * a waste of time.  Ditto for the INTERFACE_CHANGE and
		 * CONFIG_CHANGE cases. */
		 
		
		break;

	case STUB_STATE_INTERFACE_CHANGE:
		rc = do_set_interface(stub, 0);
		if (stub->ep0_req_tag != exception_req_tag)
			break;
		if (rc != 0)			// STALL on errors
			stub_set_halt(stub, stub->ep0);
		else				// Complete the status stage
			ep0_queue(stub);
		break;

	case STUB_STATE_CONFIG_CHANGE:
		rc = do_set_config(stub, new_config);
		if (stub->ep0_req_tag != exception_req_tag)
			break;
		if (rc != 0)			// STALL on errors
			stub_set_halt(stub, stub->ep0);
		else				// Complete the status stage
			ep0_queue(stub);
		break;

	case STUB_STATE_DISCONNECT:
		do_set_config(stub, 0);		// Unconfigured state
		break;

	case STUB_STATE_EXIT:
	case STUB_STATE_TERMINATED:
		do_set_config(stub, 0);			// Free resources
		spin_lock_irq(&stub->lock);
		stub->state = STUB_STATE_TERMINATED;	// Stop the thread
		spin_unlock_irq(&stub->lock);
		break;
	}
}

/*-------------------------------------------------------------------------*/

static int stub_main_thread(void *stub_)
{
	struct stub_dev		*stub = stub_;

	/* Allow the thread to be killed by a signal, but set the signal mask
	 * to block everything but INT, TERM, KILL, and USR1. */
	allow_signal(SIGINT);
	allow_signal(SIGTERM);
	allow_signal(SIGKILL);
	allow_signal(SIGUSR1);

	/* Allow the thread to be frozen */
	set_freezable();
    
	/* The main loop */
	while (stub->state != STUB_STATE_TERMINATED) {
		if (exception_in_progress(stub) || signal_pending(current)) {
			stub_handle_exception(stub);
			continue;
		}

		if (!stub->running) {
			sleep_thread(stub);
			continue;
		}

		if (get_next_command(stub) != 0)
			continue;

		spin_lock_irq(&stub->lock);
		if (!exception_in_progress(stub))
			stub->state = STUB_STATE_DATA_PHASE;
		spin_unlock_irq(&stub->lock);
        
		if (handle_gdb_cmd(stub) || finish_reply(stub)) //handle command sync
			continue;

		spin_lock_irq(&stub->lock);
		if (!exception_in_progress(stub))
			stub->state = STUB_STATE_STATUS_PHASE;
		spin_unlock_irq(&stub->lock);
        
		if (send_status(stub))
			continue;

		spin_lock_irq(&stub->lock);
		if (!exception_in_progress(stub))
			stub->state = STUB_STATE_IDLE;
		spin_unlock_irq(&stub->lock);
		
	}

	spin_lock_irq(&stub->lock);
	stub->thread_task = NULL;
	spin_unlock_irq(&stub->lock);

	/* In case we are exiting because of a signal, unregister the
	 * gadget driver and close the backing file. */
	if (test_and_clear_bit(REGISTERED, &stub->atomic_bitflags)) {
		usb_gadget_unregister_driver(&stub_driver);
	}

	/* Let the unbind and cleanup routines know the thread has exited */
	complete_and_exit(&stub->thread_notifier, 0);
	stub->io.remote_io_exit = 1;
	//if (stub->io.complete) complete_and_exit(stub->io.complete, 0);
}

/*-------------------------------------------------------------------------*/

static void stub_release(struct kref *ref)
{
	struct stub_dev	*stub = container_of(ref, struct stub_dev, ref);

	kfree(stub);
}

static void /* __init_or_exit */ stub_unbind(struct usb_gadget *gadget)
{
	struct stub_dev		*stub = get_gadget_data(gadget);
	int			i;
	struct usb_request	*req = stub->ep0req;

	DBG(stub, "unbind\n");
	clear_bit(REGISTERED, &stub->atomic_bitflags);

	/* If the thread isn't already dead, tell it to exit now */
	if (stub->state != STUB_STATE_TERMINATED) {
		raise_exception(stub, STUB_STATE_EXIT);
		wait_for_completion(&stub->thread_notifier);

		/* The cleanup routine waits for this completion also */
		complete(&stub->thread_notifier);
	}

	/* Free the data buffers */
	for (i = 0; i < NUM_BUFFERS; ++i)
		kfree(stub->buffhds[i].buf);
    
	kfree(stub->remote_io_buffhd.buf);
	kfree(stub->io.buf_for_user);
    
	/* Free the request and buffer for endpoint 0 */
	if (req) {
		kfree(req->buf);
		usb_ep_free_request(stub->ep0, req);
	}

	set_gadget_data(gadget, NULL);
}

static int stub_bind(struct usb_gadget *gadget)
{
	struct stub_dev		*stub = the_stub;
	int			rc;
	int			i;
	struct usb_ep		*ep;
	struct usb_request	*req;
    
	stub->gadget = gadget;
	set_gadget_data(gadget, stub);
	stub->ep0 = gadget->ep0;
	stub->ep0->driver_data = stub;

//	if ((rc = check_parameters(stub)) != 0)
//		goto out;

	/* Find all the endpoints we will use */
	usb_ep_autoconfig_reset(gadget);
	ep = usb_ep_autoconfig(gadget, &fs_bulk_in_desc);
	if (!ep)
		goto autoconf_fail;
	ep->driver_data = stub;		// claim the endpoint
	stub->bulk_in = ep;

	ep = usb_ep_autoconfig(gadget, &fs_bulk_out_desc);
	if (!ep)
		goto autoconf_fail;
	ep->driver_data = stub;		// claim the endpoint
	stub->bulk_out = ep;

	if (transport_is_vendor()) {
		ep = usb_ep_autoconfig(gadget, &fs_intr_in_desc);
		if (!ep)
			goto autoconf_fail;
		ep->driver_data = stub;		// claim the endpoint
		stub->intr_in = ep;
	}
	/* Fix up the descriptors */
	//device_desc.bMaxPacketSize0 = stub->ep0->maxpacket;
	device_desc.bMaxPacketSize0 = 64;
	device_desc.idVendor = cpu_to_le16(mod_data.vendor);
	device_desc.idProduct = cpu_to_le16(mod_data.product);
	device_desc.bcdDevice = cpu_to_le16(mod_data.release);

	i = (transport_is_vendor() ? 3 : 2);	// Number of endpoints
	intf_desc.bNumEndpoints = i;
	intf_desc.bInterfaceSubClass = mod_data.protocol_type;
	intf_desc.bInterfaceProtocol = mod_data.transport_type;
	fs_function[i + FS_FUNCTION_PRE_EP_ENTRIES] = NULL;

	if (gadget_is_dualspeed(gadget)) {
		hs_function[i + HS_FUNCTION_PRE_EP_ENTRIES] = NULL;

		/* Assume ep0 uses the same maxpacket value for both speeds */
		dev_qualifier.bMaxPacketSize0 = stub->ep0->maxpacket;

		/* Assume endpoint addresses are the same for both speeds */
		hs_bulk_in_desc.bEndpointAddress =
				fs_bulk_in_desc.bEndpointAddress;
		hs_bulk_out_desc.bEndpointAddress =
				fs_bulk_out_desc.bEndpointAddress;
		hs_intr_in_desc.bEndpointAddress =
				fs_intr_in_desc.bEndpointAddress;
	}

	if (gadget_is_otg(gadget))
		otg_desc.bmAttributes |= USB_OTG_HNP;

	rc = -ENOMEM;

	/* Allocate the request and buffer for endpoint 0 */
	stub->ep0req = req = usb_ep_alloc_request(stub->ep0, GFP_KERNEL);
	if (!req)
		goto out;
	req->buf = kmalloc(EP0_BUFSIZE, GFP_KERNEL);
	//req->dma = kmalloc(EP0_BUFSIZE, GFP_KERNEL);
	//req->buf = dma_alloc_coherent(NULL, EP0_BUFSIZE, &req->dma, GFP_KERNEL);
	memset(req->buf, 0, EP0_BUFSIZE);
	//printk("%s:%d\n", __func__, __LINE__);
	//printk("req->buf:%x\n", (unsigned int)req->buf);

	if (!req->buf)
		goto out;
	req->complete = ep0_complete;

	/* Allocate the data buffers */
	for (i = 0; i < NUM_BUFFERS; ++i) {
		struct stub_buffhd	*bh = &stub->buffhds[i];

		/* Allocate for the bulk-in endpoint.  We assume that
		 * the buffer will also work with the bulk-out (and
		 * interrupt-in) endpoint. */
		bh->buf = kmalloc(mod_data.buflen, GFP_KERNEL);
		//bh->buf = dma_alloc_coherent(NULL, mod_data.buflen, &bh->dma, GFP_KERNEL);
		memset(bh->buf, 0, mod_data.buflen);
		if (!bh->buf)
			goto out;
		bh->next = bh + 1;
	}
	stub->buffhds[NUM_BUFFERS - 1].next = &stub->buffhds[0];
	
	stub->remote_io_buffhd.buf = kmalloc(MAX_PATH, GFP_KERNEL);
	//stub->remote_io_buffhd.buf = dma_alloc_coherent(NULL, mod_data.buflen, &stub->remote_io_buffhd.dma, GFP_KERNEL);
	//stub->remote_io_buffhd.buf = dma_alloc_coherent(NULL, MAX_PATH, &stub->remote_io_buffhd.dma, GFP_KERNEL);
	memset(stub->remote_io_buffhd.buf, 0, MAX_PATH);
	//printk("%s:%d\n", __func__, __LINE__);
	//printk("stub->remote_io_buffhd.buf:%x\n", stub->remote_io_buffhd.buf);
	//printk("stub->remote_io_buffhd.dma:%x\n", stub->remote_io_buffhd.dma);

	if (!stub->remote_io_buffhd.buf) goto out;
	
	stub->io.buf_for_user = kmalloc(mod_data.buflen, GFP_KERNEL);
	if (!stub->io.buf_for_user)
		goto out;
	    
	remote_io_init(stub);

	/* This should reflect the actual gadget power source */
	usb_gadget_set_selfpowered(gadget);

	snprintf(manufacturer, sizeof manufacturer, "%s %s with %s",
			init_utsname()->sysname, init_utsname()->release,
			gadget->name);

	/* On a real device, serial[] would be loaded from permanent
	 * storage.  We just encode it from the driver version string. */
	for (i = 0; i < sizeof(serial) - 2; i += 2) {
		unsigned char		c = DRIVER_VERSION[i / 2];

		if (!c)
			break;
		sprintf(&serial[i], "%02X", c);
	}

	init_waitqueue_head(&stub->mon_wait);
	init_waitqueue_head(&stub->io_call_wait);

	stub->thread_task = kthread_create(stub_main_thread, stub,
			"stub-gadget");
	if (IS_ERR(stub->thread_task)) {
		rc = PTR_ERR(stub->thread_task);
		goto out;
	}

	INFO(stub, DRIVER_DESC ", version: " DRIVER_VERSION "\n");

	DBG(stub, "transport=%s (x%02x)\n",
			mod_data.transport_name, mod_data.transport_type);
	DBG(stub, "protocol=%s (x%02x)\n",
			mod_data.protocol_name, mod_data.protocol_type);
	DBG(stub, "VendorID=x%04x, ProductID=x%04x, Release=x%04x\n",
			mod_data.vendor, mod_data.product, mod_data.release);

	DBG(stub, "I/O thread pid: %d\n", task_pid_nr(stub->thread_task));

	set_bit(REGISTERED, &stub->atomic_bitflags);

	/* Tell the thread to start working */
	wake_up_process(stub->thread_task);
	return 0;

autoconf_fail:
	ERROR(stub, "unable to autoconfigure all endpoints\n");
	rc = -ENOTSUPP;

out:
	stub->state = STUB_STATE_TERMINATED;	// The thread is dead
	stub_unbind(gadget);
	return rc;
}

/*-------------------------------------------------------------------------*/

static void stub_suspend(struct usb_gadget *gadget)
{
	struct stub_dev		*stub = get_gadget_data(gadget);

	DBG(stub, "suspend\n");
	set_bit(SUSPENDED, &stub->atomic_bitflags);
}

static void stub_resume(struct usb_gadget *gadget)
{
	struct stub_dev		*stub = get_gadget_data(gadget);

	DBG(stub, "resume\n");
	clear_bit(SUSPENDED, &stub->atomic_bitflags);
}

static struct usb_gadget_driver		stub_driver = {
	.max_speed = USB_SPEED_HIGH,
	.function	= (char *) stub_name,
	.unbind		= stub_unbind,
	.disconnect	= stub_disconnect,
	.setup		= stub_setup,
	.suspend	= stub_suspend,
	.resume		= stub_resume,

	.driver		= {
		.name		= (char *) stub_name,
		.owner		= THIS_MODULE,
		// .release = ...
		// .suspend = ...
		// .resume = ...
	},
};

static int __init stub_alloc(void)
{
	struct stub_dev		*stub;

	stub = kzalloc(sizeof *stub, GFP_KERNEL);
	if (!stub)
		return -ENOMEM;
	spin_lock_init(&stub->lock);
	kref_init(&stub->ref);
	init_completion(&stub->thread_notifier);

	sema_init(&stub->sd_rbs, 1);
	the_stub = stub;
	return 0;
}


#if	STUB_ADD_EXE_CALL

static ssize_t exe_setting_show(struct kobject *kobj, 
	  struct kobj_attribute *attr, char *buf);
static ssize_t exe_setting_store(struct kobject *kobj,
	  struct kobj_attribute *attr, const char *instr, size_t bytes);

#define ATTRCMP(x) (0 == strcmp(attr->attr.name, #x))
#define EXE_SETTING_ATTR_LIST(x)	(&x ## _attribute.attr)
#define EXE_SETTING_ATTR(x)	static struct kobj_attribute x##_attribute =   \
	__ATTR(x, S_IWUGO, exe_setting_show,  exe_setting_store)

EXE_SETTING_ATTR(exe_path);
EXE_SETTING_ATTR(exe_run);
EXE_SETTING_ATTR(exe_ret);

static ssize_t exe_setting_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	if (kobj != the_stub->remote_call_obj) {
		return -ENOENT;
	}

	if (ATTRCMP(exe_path)) {
		return sprintf(buf, "%s\n", the_stub->pc_exe_path);
	} else if (ATTRCMP(exe_run)) {
		return sprintf(buf, "%d\n", the_stub->pc_exe_run);
	} else if (ATTRCMP(exe_ret)) {
		return sprintf(buf, "%d\n", the_stub->pc_exe_ret);
	} else {
	}
	return -ENOENT;
}

static ssize_t exe_setting_store(struct kobject *kobj,
	  struct kobj_attribute *attr, const char *instr, size_t bytes)
{
	ssize_t rc = bytes;

	if (kobj != the_stub->remote_call_obj) {
		return rc;
	}
	if (ATTRCMP(exe_run)) {
		printk("exe_run begin!\n");
		/* run exe here. */
		the_stub->pc_exe_run = 1;
		//sys_remotefs_call(SYSCALL_ID_SYSTEM, (long)command, 0, 0, 0, 0);
		the_stub->pc_exe_ret = remotefs_call(SYSCALL_ID_SYSTEM, (long)the_stub->pc_exe_path, 0, 0, 0, 0);
		return rc; 
	} else if (ATTRCMP(exe_path)) {
		rc = sprintf(the_stub->pc_exe_path, "%s", instr);
		printk("the_stub->pc_exe_path:%s", the_stub->pc_exe_path);
		return rc; 
	} else {
	}

	return rc;
}

static struct attribute *exe_setting_attrs[] = {
	EXE_SETTING_ATTR_LIST(exe_path),
	EXE_SETTING_ATTR_LIST(exe_run),
	EXE_SETTING_ATTR_LIST(exe_ret),
	NULL,			/* terminator */
};

static struct attribute_group exe_setting_group = {
	.name = "setting",
	.attrs = exe_setting_attrs,
};

#endif	/* STUB_ADD_EXE_CALL */


static int __init stub_init(void)
{
	int		rc;
	struct stub_dev	*stub;

	if ((rc = stub_alloc()) != 0)
		return rc;
	
	stub = the_stub;

#if	STUB_ADD_EXE_CALL
	stub->remote_call_obj = kobject_create_and_add("remote_call", NULL);
	if (!stub->remote_call_obj) {
		printk(KERN_ERR "unable to create remote_call kobject\n");
	}
	stub->pc_exe_path[0] = 0; 
	stub->pc_exe_path[1] = 0; 
	stub->pc_exe_run = 0;
	stub->pc_exe_ret = 0;

	rc = sysfs_create_group(stub->remote_call_obj, &exe_setting_group);
	if (rc != 0) {
		printk(KERN_ERR "create remote_call status group failed\n");
	}
#endif	/* STUB_ADD_EXE_CALL */
		
	if ((rc = usb_gadget_probe_driver(&stub_driver, stub_bind)) != 0)
		kref_put(&stub->ref, stub_release);

	return rc;
}
module_init(stub_init);


static void __exit stub_cleanup(void)
{
	struct stub_dev	*stub = the_stub;

	/* Unregister the driver iff the thread hasn't already done so */
	if (test_and_clear_bit(REGISTERED, &stub->atomic_bitflags))
		usb_gadget_unregister_driver(&stub_driver);

	/* Wait for the thread to finish up */
	wait_for_completion(&stub->thread_notifier);
#if	STUB_ADD_EXE_CALL
	sysfs_remove_group(stub->remote_call_obj, &exe_setting_group);
	kobject_del(stub->remote_call_obj);
#endif
	kref_put(&stub->ref, stub_release);
}
module_exit(stub_cleanup);


