/*
 * drivers/media/video/act_/act_vout-vbq.h
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
 *
 * This file contains the code necessary to interface with the
 * vbq (video buffer queue).
 */

#ifndef __GL5201VOUT_VBQ_H__
#define __GL5201VOUT_VBQ_H__

#include <linux/videodev2.h>

#include "act_vout.h"

struct act_vout_buf {
	u32 size;
	unsigned long phy_addr;
	void *virt_addr;
	bool released;
};

struct act_vout_vbq {
	spinlock_t lock;
	int cnt; /* number of frames in the queue */
	int min_size; /* smallest frame in the queue */
	struct act_vout_buf buf[VIDEO_MAX_FRAME];
};

extern int  act_vout_vbq_init(struct act_vout_device *vout);
extern void act_vout_vbq_destroy(struct act_vout_device *vout);
extern int  act_vout_vbq_buf_cnt(struct act_vout_device *vout);

#endif /* __GL5201VOUT_VBQ_H__ */

