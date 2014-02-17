/*
 * drivers/media/video/gl5201/gl5201vout.c
 *
 * Copyright (C) 2011 Actions Semi Inc.
 *
 * Based on drivers/media/video/gl5201vout/gl5201vout.c&h
 *
 * Copyright (C) 2005-2006 Texas Instruments.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/videodev2.h>
#include <media/videobuf-core.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-common.h>
#include <linux/slab.h>


#include "act_vout.h"
#include "act_vout-dss.h"
#include "act_vout-vbq.h"

#include "../act_de/display.h"
#include "../act_de/de_core.h"

#define MODULE_NAME "act_vout"

/* list of image formats supported by GL5201 video pipelines */
const static struct v4l2_fmtdesc act_formats[] = {
{
	/* Note:  V4L2 defines RGB565 as:
	 *      Byte 0                    Byte 1
	 *      g2 g1 g0 r4 r3 r2 r1 r0   b4 b3 b2 b1 b0 g5 g4 g3
	 *
	 * We interpret RGB565 as:
	 *      Byte 0                    Byte 1
	 *      g2 g1 g0 b4 b3 b2 b1 b0   r4 r3 r2 r1 r0 g5 g4 g3
	 */
	.description = "RGB565, le",
	.pixelformat = V4L2_PIX_FMT_RGB565,
},
{
	/* Note:  V4L2 defines RGB32 as: RGB-8-8-8-8  we use
	 *        this for RGB24 unpack mode, the last 8 bits are ignored
	 */
	.description = "RGB32, le",
	.pixelformat = V4L2_PIX_FMT_RGB32,
},
{
	/* Note:  V4L2 defines RGB32 as: BGR-8-8-8-8  we use
	 *        this for RGB24 unpack mode, the last 8 bits are ignored
	 */
	.description = "BGR32, le",
	.pixelformat = V4L2_PIX_FMT_BGR32,
},
{
	.description = "YU12 (YUV 4:2:0), planar",
	.pixelformat = V4L2_PIX_FMT_YUV420,
},
{
	.description = "YV12 (YVU 4:2:0), planar",
	.pixelformat = V4L2_PIX_FMT_YVU420,
},
{
	.description = "YYUV (YUV 4:2:0), semi planar",
	.pixelformat = V4L2_PIX_FMT_NV12,
},

};

#define NUM_OUTPUT_FORMATS (sizeof(act_formats)/sizeof(act_formats[0]))

/*=== Local Functions ==================================================*/

static int act_vout_crop_to_size(struct v4l2_rect *rect, int w, int h)
{
	struct v4l2_rect try_rect;
	int t;
	try_rect = *rect;
	
	if (try_rect.left < 0){
		try_rect.left = 0;
	}		
	if (try_rect.top < 0){
		try_rect.top = 0;
	}		
	if (try_rect.width > w){
		try_rect.width = w;
	}		
	if (try_rect.height > h){
		try_rect.height = h;
	}	
		
	t = ((try_rect.left + try_rect.width) - w);	
	if (t > 0){
		try_rect.width = w - t;
	}
	
	t = ((try_rect.top + try_rect.height) - h);
	
	if (t > 0){
		try_rect.height = h - t;
	}
	
	try_rect.width &= ~1;
	try_rect.height &= ~1;
	
 	if (try_rect.width <= 0 || try_rect.height <= 0)
		return -EINVAL;

	*rect = try_rect;
	
	return 0;
}

static int act_vout_try_pixel_format(struct act_vout_device *vout,
				struct v4l2_pix_format *pix)
{
	int ifmt;
	int bpp = 0;
  DBG("pix->width = %x ,pix->height = %x ,vout->max_video_width= %x ,vout->max_video_height= %x ,pix->pixelformat = %d \n", pix->width, pix->height,vout->max_video_width,vout->max_video_height,pix->pixelformat);
	if (pix->width > vout->max_video_width)
		pix->width = vout->max_video_width;

	if (pix->height > vout->max_video_height)
		pix->height = vout->max_video_height;

	for (ifmt = 0; ifmt < NUM_OUTPUT_FORMATS; ifmt++) {
		if (pix->pixelformat == act_formats[ifmt].pixelformat)
			break;
	}

	if (ifmt >= NUM_OUTPUT_FORMATS)
		ifmt = 0;

	pix->pixelformat = act_formats[ifmt].pixelformat;
	pix->field = V4L2_FIELD_NONE;
	pix->priv = 0;

	switch (pix->pixelformat) {
	case V4L2_PIX_FMT_RGB565:
		pix->colorspace = V4L2_COLORSPACE_SRGB;
		bpp = 2;
		break;
	case V4L2_PIX_FMT_RGB32:
	case V4L2_PIX_FMT_BGR32:
		pix->colorspace = V4L2_COLORSPACE_SRGB;
		bpp = 4;
		break;
	case V4L2_PIX_FMT_YUV420:
	case V4L2_PIX_FMT_YVU420:
	case V4L2_PIX_FMT_NV12:
		pix->colorspace = V4L2_COLORSPACE_JPEG;
		bpp = 2;
		break;
  case V4L2_PIX_FMT_YYUV:
  	pix->colorspace = V4L2_COLORSPACE_JPEG;
		bpp = 4;
		break;
	default:
    printk("format %d not support \n",pix->pixelformat);
		return -1;
	}
	pix->bytesperline = pix->width * bpp;
	pix->sizeimage = pix->bytesperline * pix->height;
  DBG("pix->width = %d ,pix->height = %d  pix->sizeimage = %d\n", pix->width, pix->height,pix->sizeimage);
	return 0;
}

static int act_vout_try_window(struct act_vout_device *vout,
					struct v4l2_window *win)
{
	int rc = 0;

	rc = act_vout_crop_to_size(&win->w, vout->disp_width,	vout->disp_height);

	vout->dss->need_cfg = true;
	
	if (rc == 0)
		win->field = V4L2_FIELD_NONE;

	return rc;
}
/* Make sure the input size, window rectangle, crop rectangle, and rotation
 * parameters together make up a valid configuration for the hardware
 */
static int act_vout_validate_cfg(struct act_vout_device *vout)
{
	int rc = 0;
	int win_w, win_h;
	int crp_w, crp_h;

	/* Is it assumed:
	 * - The rotation value denotes 0, 90, 180, or 270
	 * - The input size is valid based on the platform limits
	 * - The output rectangle is within the display area
	 * - The crop rectangle is within the input frame
	 */

	/* Validate scaling */
	win_w = vout->win.w.width;
	win_h = vout->win.w.height;
	crp_w = vout->crop.width;
	crp_h = vout->crop.height;

	if ( vout->rotation == 0 || vout->rotation == 2 ) {
		win_w = vout->win.w.width;
		win_h = vout->win.w.height;
	} else {
		win_w = vout->win.w.height;
		win_h = vout->win.w.width;
	}
  
	/* Down-scaling */
	if ((win_w < crp_w) && ((win_w * 4) < crp_w))
	{
		printk("Down-scaling Failed, win_w = %d ,win_h = %d ,crp_w = %d ,crp_h = %d \n",win_w ,win_h ,crp_w ,crp_h);
    win_w = crp_w / 4;
    printk("adjust the window, win_w = %d ,win_h = %d ,crp_w = %d ,crp_h = %d \n",win_w ,win_h ,crp_w ,crp_h);
	} 
	if((win_h < crp_h) && ((win_h * 4) < crp_h))
	{
		printk("Down-scaling Failed, win_w = %d ,win_h = %d ,crp_w = %d ,crp_h = %d \n",win_w ,win_h ,crp_w ,crp_h);
    win_h = crp_h / 4;
		printk("adjust the window, win_w = %d ,win_h = %d ,crp_w = %d ,crp_h = %d \n",win_w ,win_h ,crp_w ,crp_h);
		rc = 0;
  } 
	/* Up-scaling */
	if ((win_w > crp_w) && ((crp_w * 4) < win_w))
	{
		printk("Up-scaling Failed, win_w = %d ,win_h = %d ,crp_w = %d ,crp_h = %d \n",win_w ,win_h ,crp_w ,crp_h);
		win_w = crp_w * 4;
		printk("adjust the window, win_w = %d ,win_h = %d ,crp_w = %d ,crp_h = %d \n",win_w ,win_h ,crp_w ,crp_h);
	}
	if((win_h > crp_h) && ((crp_h * 4) < win_h))
	{
		printk("Up-scaling Failed, win_w = %d ,win_h = %d ,crp_w = %d ,crp_h = %d \n",win_w ,win_h ,crp_w ,crp_h);
    win_h = crp_h * 4;
		printk("adjust the window, win_w = %d ,win_h = %d ,crp_w = %d ,crp_h = %d \n",win_w ,win_h ,crp_w ,crp_h);
		rc = 0;
	}
	vout->win.w.width  =  win_w;
	vout->win.w.height = win_h;
	return rc;
}

static void act_vout_free_resources(struct act_vout_device *vout)
{
	if (vout == NULL)
		return;
		
	video_set_drvdata(&vout->vdev, NULL);
	kfree(vout);
}

/*=== V4L2 Interface Functions =========================================*/

static int act_vout_open(struct file *file)
{
	struct act_vout_device *vout;
	u16 w, h;
	int rc;

	DBG("act_vout_open\n");

	vout = video_drvdata(file);

	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}

	mutex_lock(&vout->mtx);
//
//	/* We only support single open */
//	if (vout->opened) {
//		DBG("Device already opened\n");
//		rc = -EBUSY;
//		goto failed;
//	}
//
	rc = act_vout_dss_open(vout, &w, &h);
	if (rc != 0)
		goto failed;
//
//	DBG("Overlay Display %dx%d\n", w, h);
//
//	if (w == 0 || h == 0) {
//		DBG("Invalid display resolution\n");
//		rc = -EINVAL;
//		goto failed;
//	}
//
	rc = act_vout_vbq_init(vout);
	if (rc != 0)
		goto failed;

	vout->disp_width = w;
	vout->disp_height = h;
	vout->opened = 1;

	memset(&vout->pix, 0, sizeof(vout->pix));
	vout->pix.width = w;
	vout->pix.height = h;
	vout->pix.field = V4L2_FIELD_NONE;
	vout->pix.pixelformat = V4L2_PIX_FMT_RGB565; /* Arbitrary */
	vout->pix.colorspace = V4L2_COLORSPACE_SRGB; /* Arbitrary */
	vout->pix.bytesperline = w * 2;
	vout->pix.sizeimage = w * h * 2;

	memset(&vout->win, 0, sizeof(vout->win));
	vout->win.w.width = w;
	vout->win.w.height = h;
	vout->win.field = V4L2_FIELD_NONE;

	memset(&vout->crop, 0, sizeof(vout->crop));
	vout->crop.width = w;
	vout->crop.height = h;

	memset(&vout->fbuf, 0, sizeof(vout->fbuf));

	vout->rotation = 0;
	vout->bg_color = 0;
  vout->scale_mode = SCALE_FULL_SIZE;
  vout->scale_rate = ((100 << 8) | 100);
	mutex_unlock(&vout->mtx);
//
	file->private_data = vout;

	return 0;

failed:
	mutex_unlock(&vout->mtx);
	return rc;
}

static int act_vout_release(struct file *file)
{
	struct act_vout_device *vout;

	DBG("act_vout_release\n");

	vout = video_drvdata(file);

	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}

	mutex_lock(&vout->mtx);

	if (vout->queue.streaming) {
		act_vout_dss_disable(vout);
		videobuf_streamoff(&vout->queue);
	}

	act_vout_dss_release(vout);

	act_vout_vbq_destroy(vout);

	vout->opened = 0;

	mutex_unlock(&vout->mtx);
	
	file->private_data = NULL;

	return 0;
}

static int act_vout_vidioc_querycap(struct file *file, void *priv,
				struct v4l2_capability *cap)
{
	struct act_vout_device *vout = priv;

	memset(cap, 0, sizeof(*cap));
	strncpy(cap->driver, MODULE_NAME, sizeof(cap->driver));
	strncpy(cap->card, vout->vdev.name, sizeof(cap->card));
	cap->bus_info[0] = '\0';
	cap->capabilities = V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_OUTPUT;

	return 0;
}

static int act_vout_vidioc_enum_output(struct file *file, void *priv,
				struct v4l2_output *output)
{
	int index = output->index;

	if (index > 0)
		return -EINVAL;

	memset(output, 0, sizeof(*output));
	output->index = index;

	strncpy(output->name, "video out", sizeof(output->name));
	output->type = V4L2_OUTPUT_TYPE_MODULATOR;

	return 0;
}

static int act_vout_vidioc_g_output(struct file *file, void *priv,
				unsigned int *i)
{
	*i = 0;
	return 0;
}

static int act_vout_vidioc_s_output(struct file *file, void *priv,
				unsigned int i)
{
	if (i > 0)
		return -EINVAL;
	return 0;
}

static int act_vout_vidioc_enum_fmt_vid_overlay(struct file *file, void *priv,
				struct v4l2_fmtdesc *f)
{
	int index = f->index;
	enum v4l2_buf_type type = f->type;
  DBG("%s called \n",__FUNCTION__);
	if (index >= NUM_OUTPUT_FORMATS)
		return -EINVAL;

	memset(f, 0, sizeof(*f));
	f->index = index;
	f->type = type;
	f->flags = act_formats[index].flags;
	strncpy(f->description, act_formats[index].description,
					sizeof(f->description));
	f->pixelformat = act_formats[index].pixelformat;

	return 0;
}
static int act_vout_vidioc_g_fmt_vid_overlay(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct act_vout_device *vout = priv;
	struct v4l2_window *win = &f->fmt.win;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}

	/*
	 * The API has a bit of a problem here. We're returning a v4l2_window
	 * structure, but that structure contains pointers to variable-sized
	 * objects for clipping rectangles and clipping bitmaps.  We will just
	 * return NULLs for those pointers.
	 */

	mutex_lock(&vout->mtx);

	memset(win, 0, sizeof(*win));
	win->w = vout->win.w;
	win->field = vout->win.field;
	win->chromakey = vout->win.chromakey;
	win->global_alpha = vout->win.global_alpha;

	mutex_unlock(&vout->mtx);

	return 0;
}

static int act_vout_vidioc_g_fmt_vid_out(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct act_vout_device *vout = priv;
	struct v4l2_pix_format *pix = &f->fmt.pix;
  
  DBG("%s called \n",__FUNCTION__);

	if (vout == NULL) {
		printk("Invalid device\n");
		return -ENODEV;
	}

	mutex_lock(&vout->mtx);

	memset(pix, 0, sizeof(*pix));
	*pix = vout->pix;

	mutex_unlock(&vout->mtx);

	return 0;
}

static int act_vout_vidioc_try_fmt_vid_overlay(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct act_vout_device *vout = priv;
	struct v4l2_window *win = &f->fmt.win;
	int rc;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		printk("Invalid device\n");
		return -ENODEV;
	}
	
	mutex_lock(&vout->mtx);

	rc = act_vout_try_window(vout, win);

	mutex_unlock(&vout->mtx);

	return rc;
}

static int act_vout_vidioc_try_fmt_vid_out(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct act_vout_device *vout = priv;
	struct v4l2_pix_format *pix = &f->fmt.pix;
	int rc;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}

	mutex_lock(&vout->mtx);

	rc = act_vout_try_pixel_format(vout, pix);

	mutex_unlock(&vout->mtx);

	return rc;
}

static int act_vout_vidioc_s_fmt_vid_overlay(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct act_vout_device *vout = priv;
	struct v4l2_window *win = &f->fmt.win;
	int rc = 0;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}

	mutex_lock(&vout->mtx);

  DBG("win->width= %x ,win->height = %x \n", win->w.width, win->w.height);

	rc = act_vout_try_window(vout, win);
	
	if (rc != 0)
		goto failed;

	vout->win.w = win->w;
	vout->win.field = win->field;
	vout->win.chromakey = win->chromakey;
	vout->win.global_alpha = win->global_alpha;
  	DBG("win->width= %x ,win->height = %x \n", win->w.width, win->w.height);
	/* Streaming has to be disabled, so config the hardware
	 * later when streaming is enabled
	 */

failed:
	mutex_unlock(&vout->mtx);

	return rc;
}

static int act_vout_vidioc_s_fmt_vid_out(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct act_vout_device *vout = priv;
	struct v4l2_pix_format *pix = &f->fmt.pix;
	int rc = 0;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}	
	mutex_lock(&vout->mtx);

  DBG("pix->width= %x ,pix->height = %x \n", pix->width, pix->height);
	DBG("f->fmt->w->width= %x ,f->fmt->w->height = %x \n", f->fmt.win.w.width, f->fmt.win.w.height);

	rc = act_vout_try_pixel_format(vout, pix);
	
	if (rc != 0)
		goto failed;

	memcpy(&vout->pix, pix, sizeof(*pix));

	/* Default the cropping rectangle to the input frame size */
	vout->crop.left = 0;
	vout->crop.top = 0;
	vout->crop.width = vout->pix.width;
	vout->crop.height = vout->pix.height;

failed:
	mutex_unlock(&vout->mtx);

	return rc;
}

static int act_vout_vidioc_cropcap(struct file *file, void *priv,
				struct v4l2_cropcap *ccap)
{
	struct act_vout_device *vout = priv;
	enum v4l2_buf_type type = ccap->type;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}

	if (type != V4L2_BUF_TYPE_VIDEO_OUTPUT)
		return -EINVAL;

	mutex_lock(&vout->mtx);

	memset(ccap, 0, sizeof(*ccap));
	ccap->type = type;
	ccap->bounds.width = vout->pix.width & ~1;
	ccap->bounds.height = vout->pix.height & ~1;
	ccap->defrect.left = 0;
	ccap->defrect.top = 0;
	ccap->defrect.width = ccap->bounds.width;
	ccap->defrect.left = ccap->bounds.height;
	ccap->pixelaspect.numerator = 1;
	ccap->pixelaspect.denominator = 1;

	mutex_unlock(&vout->mtx);

	return 0;
}

static int act_vout_vidioc_g_crop(struct file *file, void *priv,
				struct v4l2_crop *crop)
{
	struct act_vout_device *vout = priv;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}

	if (crop->type != V4L2_BUF_TYPE_VIDEO_OUTPUT)
		return -EINVAL;

	mutex_lock(&vout->mtx);

	crop->c = vout->crop;

	mutex_unlock(&vout->mtx);

	return 0;
}

static int act_vout_vidioc_s_crop(struct file *file, void *priv,
				struct v4l2_crop *crop)
{
	struct act_vout_device *vout = priv;
	struct v4l2_rect rect = crop->c;
	int rc = 0;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		printk("Invalid device\n");
		return -ENODEV;
	}	
	if (crop->type != V4L2_BUF_TYPE_VIDEO_OUTPUT)
	{
		printk("Invalid type\n");
		return -EINVAL;
	}
		
	mutex_lock(&vout->mtx);

	rc = act_vout_crop_to_size(&rect, vout->pix.width, vout->pix.height);
	
	if (rc != 0)
	{
		DBG("crop is not fit,rc = %x\n",rc);
		goto failed;
	}
	vout->crop = rect;
	
	DBG("crop set is ok vout->crop info:  width = %x, height = %x ,left = %x,top = %x\n",vout->crop.width,vout->crop.height,vout->crop.left,vout->crop.top);

failed:
	mutex_unlock(&vout->mtx);

	return rc;
}

static int act_vout_vidioc_reqbufs(struct file *file, void *priv,
				struct v4l2_requestbuffers *req)
{
	struct act_vout_device *vout = priv;

  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}
	/* A limitation of this implementation */	
  vout->memtype = req->memory;
  DBG("vout->memtype : %s\n" , req->memory == V4L2_MEMORY_MMAP ? "V4L2_MEMORY_MMAP" : "V4L2_MEMORY_USERPTR");

	if (req->count == 0) {
		videobuf_queue_cancel(&vout->queue);
		return 0;
	}

	mutex_lock(&vout->mtx);

	INIT_LIST_HEAD(&vout->q_list);
	
  DBG("%s called \n","videobuf_reqbufs");

	videobuf_reqbufs(&vout->queue, req);

	mutex_unlock(&vout->mtx);

	return 0;
}

static int act_vout_vidioc_querybuf(struct file *file, void *priv,
				struct v4l2_buffer *b)
{
	struct act_vout_device *vout = priv;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}
		
	if(b->memory == V4L2_MEMORY_MMAP)
	{
		DBG("Invalid command for memory type userptr \n");
		return -EINVAL;
	}
	
	return videobuf_querybuf(&vout->queue, b);
}

static int act_vout_vidioc_qbuf(struct file *file, void *priv,
				struct v4l2_buffer *b)
{
	int rc;
	struct act_vout_device *vout = priv;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}
  
	DBG("Q'ing Frame %d addr = %lu \n", b->index,b->m.userptr);

	mutex_lock(&vout->mtx);
	
  rc = videobuf_qbuf(&vout->queue, b);
  
  DBG("videobuf_qbuf rc =  %d\n", rc);
	mutex_unlock(&vout->mtx);

	return rc;
}

static int act_vout_vidioc_dqbuf(struct file *file, void *priv,
				struct v4l2_buffer *b)
{
	struct act_vout_device *vout = priv;
	int block = 0;
	int rc = 0;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}
	
	if (file->f_flags & O_NONBLOCK)
		block = 1;
		
	if(vout->dss->cur_q_idx == -1){
		return -1;
	}

	rc = videobuf_dqbuf(&vout->queue, b, block);

	DBG("DQ'ing Frame %d error code = %d addr = %lu\n", b->index, rc,b->m.userptr);

	return rc;
}

static int act_vout_vidioc_streamon(struct file *file, void *priv,
				enum v4l2_buf_type type)
{
	struct act_vout_device *vout = priv;
	int rc;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}
	DBG("vout->dss =%p \n",vout->dss);
  if(vout->dss->enabled)
	{
	//rintk("act_vout_vidioc_streamon called return code = %d\n" ,rc);
		return 0;
	}
	mutex_lock(&vout->mtx);

	/* Not sure how else to do this.  We can't truly validate the
	 * configuration until all of the pieces have been provided, like
	 * input, output, crop sizes and rotation.  This is the only point
	 * where we can be sure the client has provided all the data, thus
	 * the only place to make sure we don't cause a DSS failure.
	 */
	DBG("act_vout_validate_cfg  called %p\n",vout);
	rc = act_vout_validate_cfg(vout);
	if (rc) {
		DBG("Configuration Validation Failed\n");
		goto failed;
	}
  DBG("act_vout_dss_enable  called %p\n",vout);
	rc = act_vout_dss_enable(vout);
	if (rc) {
		DBG("DSS Enable Failed\n");
		goto failed;
	}
  DBG("videobuf_streamon  called %p\n",&vout->queue);
	rc = videobuf_streamon(&vout->queue);
	if (rc)
		act_vout_dss_disable(vout);

failed:
	mutex_unlock(&vout->mtx);

	return rc;
}

static int act_vout_vidioc_streamoff(struct file *file, void *priv,
				enum v4l2_buf_type type)
{
	struct act_vout_device *vout = priv;
	int rc = 0;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}

	mutex_lock(&vout->mtx);

	act_vout_dss_disable(vout);

	rc = videobuf_streamoff(&vout->queue);

	mutex_unlock(&vout->mtx);

	return rc;
}

static int act_vout_vidioc_queryctrl(struct file *file, void *priv,
				struct v4l2_queryctrl *qctrl)
{
	/*switch (qctrl->id) {
	case V4L2_CID_ROTATE:
		v4l2_ctrl_query_fill(qctrl, 0, 270, 90, 0);
		break;
	case V4L2_CID_BG_COLOR:
		v4l2_ctrl_query_fill(qctrl, 0, 0xFFFFFF, 1, 0);
		break;
	default:
		qctrl->name[0] = '\0';
		return -EINVAL;
	}*/

	return 0;
}

static int act_vout_vidioc_g_ctrl(struct file *file, void *priv,
				struct v4l2_control *ctrl)
{
	struct act_vout_device *vout = priv;
	int rc = 0;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}

	mutex_lock(&vout->mtx);

/*	switch (ctrl->id) {
	case V4L2_CID_ROTATE:
		ctrl->value = vout->rotation * 90;
		break;
	case V4L2_CID_BG_COLOR:
		ctrl->value = vout->bg_color;
		break;
	default:
		rc = -EINVAL;
		break;
	}*/

	mutex_unlock(&vout->mtx);

	return rc;
}

static int act_vout_vidioc_s_ctrl(struct file *file, void *priv,
				struct v4l2_control *ctrl)
{
	struct act_vout_device *vout = priv;
//	int v = ctrl->value;
	int rc = 0;
  DBG("%s called \n",__FUNCTION__);
	if (vout == NULL) {
		DBG("Invalid device\n");
		return -ENODEV;
	}

	mutex_lock(&vout->mtx);

/*	switch (ctrl->id) {
	case V4L2_CID_ROTATE:
		DBG("act_vout_vidioc_s_ctrl V4L2_CID_ROTATE\n");
		if (v == 0 || v == 90 || v == 180 || v == 270) {
			vout->rotation = v / 90;
		} else {
			DBG("Invalid rotation %d\n", v);
			rc = -ERANGE;
		}
		break;
	case V4L2_CID_BG_COLOR:
		if (v < 0 || v > 0xFFFFFF) {
			DBG("Invalid BG color 0x%08lx\n", (unsigned long) v);
			rc = -ERANGE;
		} else {
			vout->bg_color = v;
		}
		break;
	default:
		DBG("act_vout_vidioc_s_ctrl EINVAL,ctrl->id = %d, %d \n",ctrl->id,V4L2_CID_ROTATE);
		rc = -EINVAL;
		break;
	}*/

	/* Streaming has to be disabled, so config the hardware
	 * later when streaming is enabled
	 */

//failed:
	mutex_unlock(&vout->mtx);

	return rc;
}

static int act_vout_vidioc_g_fbuf(struct file *file, void *priv,
				struct v4l2_framebuffer *a)
{			
	struct act_vout_device *vout = priv;
	DBG("%s called \n",__FUNCTION__);
	mutex_lock(&vout->mtx);

/* a->capability = V4L2_FBUF_CAP_EXTERNOVERLAY |
				V4L2_FBUF_CAP_GLOBAL_ALPHA |
				V4L2_FBUF_CAP_CHROMAKEY	|
				V4L2_FBUF_CAP_SRC_CHROMAKEY;*/
	a->flags = vout->fbuf.flags;
	memset(&a->fmt, 0, sizeof(a->fmt));

	mutex_unlock(&vout->mtx);
	return 0;
}

static int act_vout_vidioc_s_fbuf(struct file *file, void *priv,
				struct v4l2_framebuffer *a)
{
	struct act_vout_device *vout = priv;
	 DBG("%s called \n",__FUNCTION__);
	mutex_lock(&vout->mtx);
	
	vout->fbuf.flags = a->flags;	
  vout->dss->need_cfg = true;
  
	mutex_unlock(&vout->mtx);
	return 0;
}

/*=== Driver Functions =================================================*/

static struct v4l2_file_operations act_vout_fops = {
	.owner = THIS_MODULE,
	.open = act_vout_open,
	.release = act_vout_release,
	.ioctl = video_ioctl2,
};

static const struct v4l2_ioctl_ops act_vout_ioctl_ops = {
	.vidioc_querycap = act_vout_vidioc_querycap,
	.vidioc_enum_output = act_vout_vidioc_enum_output,
	.vidioc_g_output = act_vout_vidioc_g_output,
	.vidioc_s_output = act_vout_vidioc_s_output,
	.vidioc_enum_fmt_vid_overlay = act_vout_vidioc_enum_fmt_vid_overlay,
	.vidioc_enum_fmt_vid_out = act_vout_vidioc_enum_fmt_vid_overlay,
	.vidioc_g_fmt_vid_overlay = act_vout_vidioc_g_fmt_vid_overlay,
	.vidioc_g_fmt_vid_out = act_vout_vidioc_g_fmt_vid_out,
	.vidioc_try_fmt_vid_overlay = act_vout_vidioc_try_fmt_vid_overlay,
	.vidioc_try_fmt_vid_out = act_vout_vidioc_try_fmt_vid_out,
	.vidioc_s_fmt_vid_overlay = act_vout_vidioc_s_fmt_vid_overlay,
	.vidioc_s_fmt_vid_out = act_vout_vidioc_s_fmt_vid_out,
	.vidioc_cropcap = act_vout_vidioc_cropcap,
	.vidioc_g_crop = act_vout_vidioc_g_crop,
	.vidioc_s_crop = act_vout_vidioc_s_crop,
	.vidioc_reqbufs = act_vout_vidioc_reqbufs,
	.vidioc_querybuf = act_vout_vidioc_querybuf,
	.vidioc_qbuf = act_vout_vidioc_qbuf,
	.vidioc_dqbuf = act_vout_vidioc_dqbuf,
	.vidioc_streamon = act_vout_vidioc_streamon,
	.vidioc_streamoff = act_vout_vidioc_streamoff,
	.vidioc_queryctrl = act_vout_vidioc_queryctrl,
	.vidioc_g_ctrl = act_vout_vidioc_g_ctrl,
	.vidioc_s_ctrl = act_vout_vidioc_s_ctrl,
	.vidioc_g_fbuf = act_vout_vidioc_g_fbuf,
	.vidioc_s_fbuf = act_vout_vidioc_s_fbuf,
};

static struct video_device act_vout_devdata = {
	.name = MODULE_NAME,
	.fops = &act_vout_fops,
	.ioctl_ops = &act_vout_ioctl_ops,
	.vfl_type = VFL_TYPE_GRABBER,
	.release = video_device_release,
	.minor = -1,
};

static int __init act_vout_probe_device(struct act_vout_config *cfg, int vid)
{
	struct act_vout_device *vout = NULL;
	int rc = 0;
	
	DBG("act_vout_probe_device %d\n", vid);

	vout = kzalloc(sizeof(struct act_vout_device), GFP_KERNEL);
	
	if (vout == NULL) {
		rc = -ENOMEM;
		goto err0;
	}

	mutex_init(&vout->mtx);
	
	vout->max_video_width = cfg->max_width;
	vout->max_video_height = cfg->max_height;
	vout->max_video_buffer_size = cfg->max_buffer_size;
 
  DBG("cfg->max_width = %x ,cfg->max_height = %x ,cfg->max_buffer_size= %x  \n", cfg->max_width, cfg->max_height,cfg->max_buffer_size);
  vout->id = vid;
	rc = act_vout_dss_init(vout);
	
	if (rc != 0) {
		printk(KERN_INFO "DSS init failed\n");
		kfree(vout);
		goto err0;
	}

	/* register the V4L2 interface */
	vout->vdev = act_vout_devdata;
	
	video_set_drvdata(&vout->vdev, vout);
	
	DBG("video_set_drvdata %p  \n",vout);

	if (video_register_device(&vout->vdev, VFL_TYPE_GRABBER, vid) < 0) {
		printk(KERN_ERR MODULE_NAME": could not register with V4L2\n");
		rc = -EINVAL;
		goto cleanup;
	}
	
	vout->id = vid;
	return 0;
cleanup:
	act_vout_free_resources(vout);
err0:
	return rc;
}

/* Some reasonable defaults if the platform does not supply a config */
static struct act_vout_config default_cfg = {
	.max_width = 4096,
	.max_height = 2048,
	.max_buffer_size = 0x1000000, /* (w * h * 2) page aligned */
	.num_buffers = 6,
	.num_devices = 2,
	.device_ids = {1, 2},
};

static int __init act_vout_probe(struct platform_device *pdev)
{
	struct act_vout_config *cfg;
	int i;
	int rc = 0;
		
  cfg = &default_cfg;

	for (i = 0; i < cfg->num_devices; i++) {
		rc = act_vout_probe_device(cfg,cfg->device_ids[i]);
		if (rc) {
			DBG("act_vout_probe %d failed\n", (i + 1));
			return rc;
		}
	}
	
	return rc;
}

static int act_vout_remove(struct platform_device *pdev)
{
	struct act_vout_device *vout = platform_get_drvdata(pdev);
	DBG("act_vout_remove\n");

	act_vout_dss_remove(vout);
	
	act_vout_free_resources(vout);

	return 0;
}

static struct platform_driver act_vout_driver = {
	.remove         = act_vout_remove,
	.driver         = {
		.name   = MODULE_NAME,
	},
};

static struct platform_device * act_vout_devices;

static int __init act_vout_init(void)
{
	int rc;

	DBG("act_vout_init\n");
	
	act_vout_devices = platform_device_register_simple(MODULE_NAME, 0,NULL, 0);

	if(IS_ERR(act_vout_devices))
	{
		printk(KERN_ERR "failed to register act_vout_devices device\n");
		rc = PTR_ERR(act_vout_devices);
		return rc;
	}
	
	rc = platform_driver_probe(&act_vout_driver, act_vout_probe);
	if (rc != 0) {
		printk(KERN_ERR "failed act_vout register/probe %d\n", rc);
		return -ENODEV;
	}

	return 0;
}

static void __exit act_vout_exit(void)
{
	DBG("act_vout_exit\n");
	platform_driver_unregister(&act_vout_driver);
}

module_init(act_vout_init);
module_exit(act_vout_exit);

MODULE_AUTHOR("Actions_semi,wh <wanghui@actions-semi.com>");
MODULE_DESCRIPTION("action Video Out for V4L2");
MODULE_LICENSE("GPL");

