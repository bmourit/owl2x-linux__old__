/*
 * drivers/media/video/act/act_vout-dss.c
 *
 * Copyright (C) 2009 Actions_semi Inc.
 *
 * Based on drivers/media/video/omap/omapvout-dss.c&h
 *
 * Copyright (C) 2005-2006 Texas Instruments.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include "act_vout.h"
#include "act_vout-dss.h"
#include "act_vout-vbq.h"
#include "../act_de/display.h"
#include "../act_de/de_core.h"

/*=== Local Functions ==================================================*/
static int act_vout_dss_color_mode(u32 pixelformat)
{
	int mode = RGB_565;

	switch (pixelformat) {
	case V4L2_PIX_FMT_RGB565:
		mode = RGB_565;
		DBG("pixelformat == RGB_565\n");
		break;
	case V4L2_PIX_FMT_RGB32:
		mode = ARGB_8888;
		DBG("pixelformat == ARGB_8888\n");
		break;
	case V4L2_PIX_FMT_BGR32:
		mode = ABGR_8888;
		DBG("pixelformat == ABGR_8888\n");
		break;
	case V4L2_PIX_FMT_YUV420:
		mode = YUV_420_PLANAR;
		DBG("pixelformat == YUV_420_PLANAR\n");
		break;
	case V4L2_PIX_FMT_YVU420:
		mode = YVU_420_PLANAR;
		DBG("pixelformat == YVU_420_PLANAR\n");
		break;
	case V4L2_PIX_FMT_NV12:
		mode = YUV_420_SEMI_PLANAR;
		DBG("pixelformat == YUV_420_SEMI_PLANAR\n");
		break;
	default:
		return -1;
	}

	return mode;
}

/* This algorithm reverses the changes made in the "_set_" function and
 * returns a flag denoting if alpha should still be enabled.
 *
 * The non-trivial part is to handle the case where both video planes are
 * enabled.
 */
static int act_vout_dss_enable_transparency(struct act_vout_device *vout)
{
	struct layer_manager * layer_mgr = vout->dss->layer_mgr;
	
	if (vout->fbuf.flags & V4L2_FBUF_FLAG_CHROMAKEY) {		
		layer_mgr->trans_key = vout->win.chromakey;
		layer_mgr->trans_enabled = true;	
		layer_mgr->color_key_enabled = true;
		layer_mgr->color_key_max_value = vout->win.chromakey;
		layer_mgr->color_key_max_value = vout->win.chromakey;		
		
	} else if (vout->fbuf.flags & V4L2_FBUF_FLAG_SRC_CHROMAKEY) {
		layer_mgr->trans_key = vout->win.chromakey;
		layer_mgr->trans_enabled = true;
		layer_mgr->color_key_enabled = true;
	} else {
		layer_mgr->trans_enabled = false;
		layer_mgr->color_key_enabled = false;
	}

	if (vout->fbuf.flags & V4L2_FBUF_FLAG_LOCAL_ALPHA) {
		layer_mgr->global_alpha = 128;		
		layer_mgr->alpha_enabled = true;
	} else if (vout->fbuf.flags & V4L2_FBUF_FLAG_GLOBAL_ALPHA) {
		layer_mgr->global_alpha = 128;
		layer_mgr->alpha_enabled = true;
	} else {		
		layer_mgr->global_alpha = 255;
		layer_mgr->alpha_enabled = false;
	}

	DBG("Trans Enable = %d\n", layer_mgr->trans_enabled);
	DBG("Alpha Enable = %d\n", layer_mgr->alpha_enabled);

	layer_mgr->config_manager_info(layer_mgr);
 
	return 0;
}

static int act_vout_dss_config_overlay(struct act_vout_device *vout)
{	  
	int r = 0;
	struct act_layer * ovl ;
	struct act_layer_info * info;
	ovl = vout->dss->overlay;
	info = ovl->info;
	info->img_width = vout->pix.width;
	info->img_height = vout->pix.height;
	info->xoff = vout->crop.left;
	info->yoff = vout->crop.top;		
	info->width = vout->crop.width;
	info->height = vout->crop.height;
	info->pos_x = vout->win.w.left & (~1);
	info->pos_y = vout->win.w.top & (~1);
	info->out_width = vout->win.w.width;
	info->out_height = vout->win.w.height;
	info->pixel_format = act_vout_dss_color_mode(vout->pix.pixelformat);
	info->rotation	=	vout->rotation;
	info->scale_option = vout->scale_mode;
	
	DBG("overlay crop info: info->xoff = %d, info->yoff %d, info->width %d, info->height %d \n",
	     info->xoff,info->yoff,info->width,info->height);
	     
	DBG("overlay windows info: info->pos_x = %d, info->pos_y %d, info->out_width %d, info->out_height %d \n",
	     info->pos_x,info->pos_y,info->out_width,info->out_height);
	  
	DBG("overlay other info: info->pixel_format = %d, info->rotation %d, info->screen_width %d\n",
	     info->pixel_format,info->rotation,info->screen_width);
	   
	ovl->needconfig = 1;	
	r = ovl->set_layer_info(ovl, info);

	if (r) {
		printk("set overlay info err , err code = %d", r );
		return r;
	}

		return 0;
}

static int act_vout_dss_update_overlay(struct act_vout_device *vout,int buf_idx)
{
	int rc = 0;	
	struct act_layer * ovl ;
	struct act_layer_info * info;  

	DBG("act_vout_dss_update_overlay\n");
	/*if memory mmap the baddr is the phy addr */
	ovl = vout->dss->overlay;
	info = ovl->info;
	info->enabled = true;
	info->paddr = vout->queue.bufs[buf_idx]->baddr;	
	info->pixel_format = act_vout_dss_color_mode(vout->pix.pixelformat);	
	rc = ovl->set_layer_info(ovl, info);

	if (rc) {
		printk("set overlay info err , err code = %d", rc );
		return rc ;
	}
	rc = ovl->apply_layer(ovl);

	if (rc) {
		printk("overlay wait_for_go failed , err code = %d", rc );
		return rc ;
	}
	ovl->wait_for_sync();

	return rc;
}

static int act_vout_dss_disable_overlay(struct act_vout_device *vout, int enable)
{
	int rc = 0;	
	struct act_layer * ovl;
	struct act_layer_info * info;

	DBG("act_vout_dss_disable_overlay \n");

	ovl = vout->dss->overlay;
	info = ovl->info;
	info->enabled = false;
	info->scale_rate = vout->scale_rate;
	ovl->set_layer_info(ovl,info);
	rc = ovl->apply_layer(ovl);

	if (rc)	{
		printk("overlay wait_for_go failed , err code = %d", rc );
	}

	return rc;
}

static void act_vout_dss_mark_buf_done(struct act_vout_device *vout, int idx)
{
	int release_index = vout->dss->cur_q_idx;
	int i = 0;
	
	if (vout->dss->enabled == false) { 
		if ((vout->queue.bufs[idx]->state == VIDEOBUF_QUEUED) || (vout->queue.bufs[idx]->state == VIDEOBUF_ACTIVE)) {
			vout->queue.bufs[idx]->state = VIDEOBUF_DONE;
			wake_up_interruptible(&vout->queue.bufs[idx]->done);
			DBG(" ~false~act_vout_dss_mark_buf_done buffer addr = %lu, buffer index = %x,buffer status = %x\n",
					vout->queue.bufs[idx]->baddr, vout->queue.bufs[idx]->i ,vout->queue.bufs[idx]->state);
		}
	} else if (release_index != -1) {
		if ((vout->queue.bufs[release_index]->state == VIDEOBUF_QUEUED) || (vout->queue.bufs[release_index]->state == VIDEOBUF_ACTIVE)) {
			DBG(KERN_ERR "buffer[%d] released \n",idx);
			vout->queue.bufs[release_index]->state = VIDEOBUF_DONE;
			DBG("act_vout_dss_mark_buf_done buffer addr = %lu, buffer index = %x, buffer status = %x\n", vout->queue.bufs[release_index]->baddr,
					vout->queue.bufs[release_index]->i, vout->queue.bufs[release_index]->state);
		}
		wake_up_interruptible(&vout->queue.bufs[release_index]->done);
	} else {
		DBG("~first buffer ~cnt %d~\n", vout->vbq->cnt);
		for (i = 0 ; i < VIDEO_MAX_FRAME ; i++) {
			if (vout->queue.bufs[i] != NULL) {
				DBG("act_vout_dss_mark_buf_done buffer addr = %lu, buffer index = %x, buffer status = %x\n",
						vout->queue.bufs[i]->baddr, vout->queue.bufs[i]->i, vout->queue.bufs[i]->state);
			}
		}
	}
	vout->dss->cur_q_idx = idx;
}

static void act_vout_dss_perform_update(struct work_struct *work)
{
	struct act_vout_device *vout;
	struct act_vout_dss *dss;
	struct videobuf_buffer *buf;
	int rc;
	int idx = 0;
	
	DBG("act_vout_dss_perform_update\n");

	dss = container_of(work, struct act_vout_dss, work);
	vout = dss->vout;
	
	if (!dss->enabled)
		return;

	mutex_lock(&vout->mtx);
	dss->working = true;
	while (!list_empty(&vout->q_list)) {
		buf = list_entry(vout->q_list.next, struct videobuf_buffer, queue);
		list_del(&buf->queue);
		buf->state = VIDEOBUF_ACTIVE;
		idx = buf->i;

		DBG("Processing frame %d\n", idx);
		
		if (dss->need_cfg) {
			DBG("act_vout_dss_enable_transparency\n");
			rc = act_vout_dss_enable_transparency(vout);
			if (rc != 0) {
				DBG("Alpha config failed %d\n", rc);
				goto failed_need_done;
			}

			switch (vout->rotation) {
			case 1:
				dss->rotation = 3;
				break;
			case 3:
				dss->rotation = 1;
				break;
			default:
				dss->rotation = vout->rotation;
				break;
			}
			rc = act_vout_dss_config_overlay(vout);		
			if (rc != 0) {
				DBG("overlay config failed %d\n", rc);
				goto failed_need_done;
			}
		}
		rc = act_vout_dss_update_overlay(vout, idx);
		
		if (rc != 0) {
			DBG("DSS update failed %d\n", rc);
			goto failed;
		}

		act_vout_dss_mark_buf_done(vout, idx);
		dss->need_cfg = false;
		mutex_unlock(&vout->mtx);

		if (!dss->enabled) {
			/* Since the DSS is disabled, this isn't a problem */
			dss->working = false;
			return;
		}
		mutex_lock(&vout->mtx);
	}
	dss->working = false;
	mutex_unlock(&vout->mtx);

	return;

failed_need_done:
	/* Set the done flag on failures to be sure the buffer can be DQ'd */
	act_vout_dss_mark_buf_done(vout, idx);

failed:
	dss->working = false;
	mutex_unlock(&vout->mtx);
}

/*=== Public Functions =================================================*/

int  act_vout_dss_init(struct act_vout_device *vout)
{
	int rc = 0;
	struct act_layer * overlay;
	struct layer_manager * layer_mgr;
  
	if (vout->dss) {
		rc = -EINVAL;
		goto failed;
	}
	vout->dss = kzalloc(sizeof(struct act_vout_dss), GFP_KERNEL);

	if (vout->dss == NULL) {
		rc = -ENOMEM;
		goto failed;
	}	
	printk("vout->id = %d !\n",vout->id);

	if (vout->id == 1) {
		layer_mgr = act_de_get_layer_mgr(0); 		
	} else {
		layer_mgr = act_de_get_layer_mgr(1);  
	} 
	vout->dss->vout = vout;
	vout->dss->layer_mgr = layer_mgr;
	overlay = layer_mgr->layers[0];

	if (overlay == NULL) {
  		printk("overlay get failed !\n");
  		rc = -EINVAL;
		goto failed;
	}  
	vout->dss->overlay = overlay;

failed:
	return rc;
}

void act_vout_dss_remove(struct act_vout_device *vout)
{
	if (vout->dss != NULL) {
		kfree(vout->dss);
	}
}

int act_vout_dss_open(struct act_vout_device *vout, u16 *disp_w, u16 *disp_h)
{
	int rc = 0;
	struct act_layer * ovl;
	struct act_layer_info * info;
	struct layer_manager * layer_mgr;
	vout->dss->workqueue = create_singlethread_workqueue("ACT_VOUT-DSS");
	if (vout->dss->workqueue == NULL) {
		rc = -ENOMEM;
		goto failed;
	}
	*disp_w = 1920;
	*disp_h = 1080;
	INIT_WORK(&vout->dss->work, act_vout_dss_perform_update);
	vout->dss->enabled = false;
	ovl = vout->dss->overlay;
	info = ovl->info;
	layer_mgr =  vout->dss->layer_mgr;
	layer_mgr->fb_layer = GRAPHIC_LAYER;	
	ovl->set_layer_info(ovl,info);
	info->scale_option = vout->scale_mode;
	layer_mgr->video_on = true;
	vout->dss->cur_q_idx = -1;

failed:
	return rc;
}

void act_vout_dss_release(struct act_vout_device *vout)
{
	/* It is assumed that the caller has locked the vout mutex */
	struct layer_manager * layer_mgr = NULL; 
	struct act_layer_info * info = NULL; 
	int rc = 0;

	if (vout->dss->enabled)
		act_vout_dss_disable(vout);

	rc = act_vout_dss_disable_overlay(vout,0);
	vout->dss->cur_q_idx = -1;
  
	if (rc) {
		printk("act_disable_overlay failed %d\n", rc);
	}
	flush_workqueue(vout->dss->workqueue);
	destroy_workqueue(vout->dss->workqueue);
	info = vout->dss->overlay->info;
	layer_mgr = vout->dss->layer_mgr; 
	layer_mgr->video_on = false;
	layer_mgr->global_alpha = 255;
	layer_mgr->alpha_enabled = false;
	layer_mgr->fb_layer = VIDEO1_LAYER;	
	info->scale_option = SCALE_FULL_SIZE;
	layer_mgr->config_manager_info(layer_mgr);
}

int act_vout_dss_enable(struct act_vout_device *vout)
{
	/* It is assumed that the caller has locked the vout mutex */
	struct layer_manager * layer_mgr = vout->dss->layer_mgr;
	/* Force a reconfiguration */
	vout->dss->need_cfg = true;
	vout->dss->enabled = true;
	layer_mgr->fb_layer = GRAPHIC_LAYER;
	vout->scale_rate = vout->dss->overlay->info->scale_rate;
	vout->dss->overlay->info->scale_rate = ((100 << 8) | 100);
	layer_mgr->is_enable = 0;

	return 0;
}

void act_vout_dss_disable(struct act_vout_device *vout)
{
	struct videobuf_buffer *buf, *tmp;
	struct layer_manager * layer_mgr = vout->dss->layer_mgr;
	/* It is assumed that the caller has locked the vout mutex */
	vout->dss->enabled = false;
	vout->dss->overlay->info->scale_rate = vout->scale_rate;
	layer_mgr->is_enable = 1;
	list_for_each_entry_safe(buf, tmp, &vout->q_list, queue) {
		list_del(&buf->queue);
		act_vout_dss_mark_buf_done(vout, buf->i);
	}
  	/* Reset the current frame idx */
}

int act_vout_dss_update(struct act_vout_device *vout)
{
	/* It is assumed that the caller has locked the vout mutex */
	if (!vout->dss->enabled) {
		DBG("DSS overlay is not enabled\n");
		return -EINVAL;
	}

	if (vout->dss->working) {
		/* Exit quitely, since still working on previous frame */
		return 0;
	}

	if (queue_work(vout->dss->workqueue, &vout->dss->work) == 0) {
		DBG("Queuing DSS work failed\n");
		return -EINVAL;
	}

	return 0;
}
