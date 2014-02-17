/*
 * drivers/media/video/gl5201/gl5201vout.h
 *
 * Copyright (C) 2011 Actions Semi Inc.
 *
 * Based on drivers/media/video/act_vout/act_vout.c&h
 *
 * Copyright (C) 2005-2006 Texas Instruments.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __GL5201VOUT_H__
#define __GL5201VOUT_H__

#include <linux/mutex.h>
#include <linux/videodev2.h>
#include <linux/wait.h>
#include <media/v4l2-dev.h>
#include <media/videobuf-core.h>

#if 0
#define DBG(format, ...) printk("ACTIONS VOUT: " format, ## __VA_ARGS__)
#else
#define DBG(format, ...)
#endif

struct act_vout_config {
	u16 max_width;
	u16 max_height;
	u32 max_buffer_size;
	u8  num_buffers;
	u8  num_devices;
	int device_ids[2]; /* -1 for any videoX */
};

/* The device structure */

struct act_vout_device {
	struct video_device vdev;
	struct mutex  mtx; /* Lock for all device accesses */
	struct video_device *vfd;

	int opened;
	int id;

	int disp_width;
	int disp_height;

	int max_video_width;
	int max_video_height;
	int max_video_buffer_size;

	/* Buffer pool */
	struct act_vout_bp *bp;

	/* DSS data */
	struct act_vout_dss *dss;

	/* V4L2 data */
	int rotation;
	int bg_color;
	int scale_mode;
	int scale_rate;
	struct v4l2_pix_format pix;
	struct v4l2_window win;
	struct v4l2_rect crop;
	struct v4l2_framebuffer fbuf;

	/* Frame Q */
	struct videobuf_queue queue;
	struct act_vout_vbq *vbq;
	struct list_head q_list;

	/* Don't allow new buffers when some are still mapped */
	int memtype;
	int mmap_cnt;
};

#define vdev_to_act_vout(d) container_of(d, struct act_vout_device, vdev)

#endif /* __GL5201VOUT_H__ */
