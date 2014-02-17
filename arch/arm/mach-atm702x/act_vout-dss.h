/*
 * drivers/media/video/action/act_vout-dss.h
 *
 * Copyright (C) 2011 Actions Semi Inc.
 *
 * Based on drivers/media/video/omap24xx/omap24xxvout.c&h
 *
 * Copyright (C) 2005-2006 Texas Instruments.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __GL5201VOUT_DSS_H__
#define __GL5201VOUT_DSS_H__

struct act_vout_dss {
//	struct act__overlay *overlay;

	/* FIXME: This is a minor hack to allow the work callback to
	 * gain access to the vout pointer.
	 */
	struct act_vout_device *vout;
	struct act_layer * overlay;
	struct layer_manager * layer_mgr;
	bool enabled;
	bool need_cfg;
	int rotation;
	int foffset; /* per frame address offset */
	struct work_struct work;
	struct workqueue_struct *workqueue;
	bool working;
	int cur_q_idx; /* The current Q frame used by DSS */
};

/* Driver init/remove time calls */
extern int  act_vout_dss_init(struct act_vout_device *vout);
extern void act_vout_dss_remove(struct act_vout_device *vout);

/* Driver open/release time calls */
extern int  act_vout_dss_open(struct act_vout_device *vout,
						u16 *disp_w, u16 *disp_h);
extern void act_vout_dss_release(struct act_vout_device *vout);

/* Driver operation calls */
extern bool act_vout_dss_is_rotation_supported(struct act_vout_device *vout);
extern int  act_vout_dss_enable(struct act_vout_device *vout);
extern void act_vout_dss_disable(struct act_vout_device *vout);
extern int  act_vout_dss_update(struct act_vout_device *vout);

#endif /* __GL5201VOUT_DSS_H__ */

