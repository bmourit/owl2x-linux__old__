/*
 * drivers/media/video/act/actvout-vbq.c
 *
 * Copyright (C) 2011 Actions Semi Inc.
 *
 * Based on drivers/media/video/actvout/actvout.c&h
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

#include <media/videobuf-core.h>
#include <linux/mm.h>
#include <linux/slab.h>

#include "act_vout.h"
#include "act_vout-vbq.h"
#include "act_vout-dss.h"

/*=== Local Functions ==================================================*/
static int act_vout_vbq_buf_setup(struct videobuf_queue *q,
			unsigned int *count, unsigned int *size)
{
	struct act_vout_device *vout;
	struct act_vout_vbq *vbq;
	int cnt = *count;
	int sz;

	vout = q->priv_data;
	if (!vout)
		return -EINVAL;

	vbq = vout->vbq;

	/* It is assumed that the video out format is correctly configured */
	if (vout->pix.sizeimage == 0)
		return -EINVAL;

	sz = vout->pix.sizeimage;

	/* Use the existing frames if possible */
	if (cnt <= vbq->cnt && sz <= vbq->min_size)
		goto success;

	if (cnt > VIDEO_MAX_FRAME)
		cnt = VIDEO_MAX_FRAME;	
		
  vbq->cnt = cnt;
  
  printk("vbq->cnt = %d cnt = %d\n",vbq->cnt,cnt);
	if (vbq->cnt <= 0) {
		DBG("Buffer allocation failed\n");
		return -ENOMEM;
	}
	
	

success:
	*count = cnt;
	*size = sz;
	return 0;
}

static int act_vout_vbq_buf_prepare(struct videobuf_queue *q,
			struct videobuf_buffer *vb, enum v4l2_field field)
{
	struct act_vout_device *vout;

	vout = q->priv_data;
	if (!vout)
		return -EINVAL;

	if (vb->state == VIDEOBUF_NEEDS_INIT) {
		vb->width = vout->pix.width;
		vb->height = vout->pix.height;
		vb->size = vout->pix.sizeimage;
		vb->field = field;
		vb->privsize = 0;  /* Reusing for buf seq number */
	}

	vb->state = VIDEOBUF_PREPARED;

	return 0;
}

static void act_vout_vbq_buf_queue(struct videobuf_queue *q,
			struct videobuf_buffer *vb)
{
	struct act_vout_device *vout;
  DBG("act_vout_vbq_buf_queue called \n");
	vout = q->priv_data;
	if (!vout)
		return;

	/* Add it to the incoming queue */
	list_add_tail(&vb->queue, &vout->q_list);

	vb->state = VIDEOBUF_QUEUED;

	act_vout_dss_update(vout);
}

static void act_vout_vbq_buf_release(struct videobuf_queue *q,
			struct videobuf_buffer *vb)
{
	struct act_vout_device *vout;
	int i;

	vout = q->priv_data;
	if (!vout)
		return;

	if (vout->vbq->cnt <= 0)
		return;

	vout->vbq->buf[vb->i].released = true;

	for (i = 0; i < vout->vbq->cnt; i++) {
		if (!vout->vbq->buf[i].released)
			break;
	}
	
	if(i > vout->vbq->cnt){
			vout->vbq->cnt = 0;
	    vout->vbq->min_size = 0;
	    memset(vout->vbq->buf, 0, sizeof(vout->vbq->buf));
	}

	vb->state = VIDEOBUF_NEEDS_INIT;
}

static struct videobuf_queue_ops vbq_buf_ops = {
	.buf_setup = act_vout_vbq_buf_setup,
	.buf_prepare = act_vout_vbq_buf_prepare,
	.buf_queue = act_vout_vbq_buf_queue,
	.buf_release = act_vout_vbq_buf_release
};

static struct videobuf_buffer * act_vout_vbq_alloc(size_t size)
{
	return (struct videobuf_buffer * )kzalloc(size, GFP_KERNEL);
}

static struct videobuf_qtype_ops vbq_ops = {
	.magic = MAGIC_QTYPE_OPS,
	.alloc_vb = act_vout_vbq_alloc,
};

/*=== Public Interface Functions =========================================*/

int act_vout_vbq_init(struct act_vout_device *vout)
{
	struct act_vout_vbq *vbq;
	 struct mutex *ext_lock = NULL;
	int rc;

	/* It is assumed that the caller has locked the vout mutex */
	if (vout->vbq) {
		rc = -EINVAL;
		goto failed;
	}

	vout->vbq = kzalloc(sizeof(struct act_vout_vbq), GFP_KERNEL);
	
	if (vout->vbq == NULL) {
		rc = -ENOMEM;
		goto failed;
	}

	vbq = vout->vbq;	
	vbq->cnt = 0;
	vbq->min_size = 0;

	spin_lock_init(&vbq->lock);
	
	videobuf_queue_core_init(&vout->queue, &vbq_buf_ops, NULL,
			&vbq->lock, V4L2_BUF_TYPE_VIDEO_OUTPUT,
			V4L2_FIELD_NONE, sizeof(struct videobuf_buffer),
			vout, &vbq_ops, ext_lock);
						 
	return 0;

failed:
	return rc;
}

void act_vout_vbq_destroy(struct act_vout_device *vout)
{
	/* It is assumed that the caller has locked the vout mutex */
	if (vout->vbq) {
		kfree(vout->vbq);
		vout->vbq = NULL;
	}
}

int act_vout_vbq_buf_cnt(struct act_vout_device *vout)
{
	return vout->vbq->cnt;
}

