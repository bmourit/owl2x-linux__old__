/***************************************************************************
 *                              GLBASE
 *                            Module: framebuffer driver
 *                 Copyright(c) 2011-2015 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       wanghui     2011-08-19 9:00     1.0             build this file
***************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include "fb_main.h"
#include "../act_fb/wvga_logo.h"
#include "../act_de/display.h"
#include "../act_de/de_core.h"

#define MODULE_NAME    "act_fb"

#define ACT_FB_PLANE_XRES_MIN		8
#define ACT_FB_PLANE_YRES_MIN		8

extern int get_config(const char *key, char *buff, int len);

static struct fb_info_config{
	int fb_number;
	int fb_buffer_number;
	int fb_xres;
	int fb_yres;
	int fb_bpp_info;
	int disp_mode;
	int displayId;
	int rotate;
}fb_cfg;

static int fb_mode = 0;
module_param(fb_mode, int, S_IRUGO  | S_IWUSR);
static void set_color_bitfields(struct fb_var_screeninfo *var) {
	if (var->nonstd != 0)
		return;

	switch (var->bits_per_pixel) {
	case 1:
	case 2:
	case 4:
	case 8:
		var->red.offset = 0;
		var->red.length = var->bits_per_pixel;
		var->green.offset = 0;
		var->green.length = var->bits_per_pixel;
		var->blue.offset = 0;
		var->blue.length = var->bits_per_pixel;
		var->transp.offset = 0;
		var->transp.length = 0;
		break;
	case 16:
		var->green.length = (var->green.length == 5) ? 5 : 6;
		var->red.length = 5;
		var->blue.length = 5;
		var->transp.length = 6 - var->green.length;
		var->blue.offset = 0;
		var->green.offset = 5;
		var->red.offset = 5 + var->green.length;
		var->transp.offset = (5 + var->red.offset) & 15;
		break;
	case 24: /* RGB 888   */
	case 32: /* RGBA 8888 */
		var->red.offset = 16;
		var->red.length = 8;
		var->green.offset = 8;
		var->green.length = 8;
		var->blue.offset = 0;
		var->blue.length = 8;
		var->transp.length = var->bits_per_pixel - 24;
		var->transp.offset = (var->transp.length) ? 24 : 0;
		break;
	}
	var->red.msb_right = 0;
	var->green.msb_right = 0;
	var->blue.msb_right = 0;
	var->transp.msb_right = 0;

	return;
}

static int check_fb_res_bounds(struct fb_var_screeninfo *var) {
	int xres_min = ACT_FB_PLANE_XRES_MIN;
	int xres_max = CONFIG_XRES_VIRTUAL_MAX;
	int yres_min = ACT_FB_PLANE_YRES_MIN;
	int yres_max = CONFIG_YRES_VIRTUAL_MAX;

	/* XXX: some applications seem to set virtual res to 0. */
	if (var->xres_virtual == 0)
		var->xres_virtual = var->xres;

	if (var->yres_virtual == 0)
		var->yres_virtual = var->yres;

	if (var->xres_virtual < xres_min || var->yres_virtual < yres_min)
		return -EINVAL;
	
	DBG("var->xres_virtual =%x ,var->yres_virtual = %x,var->yres = %x\n",var->xres_virtual,var->yres_virtual,var->yres);	
	if (var->yres_virtual	> var->yres * fb_cfg.fb_buffer_number)
		return -EINVAL;

	if (var->xres < xres_min)
		var->xres = xres_min;
	if (var->yres < yres_min)
		var->yres = yres_min;
	if (var->xres > xres_max)
		var->xres = xres_max;
	if (var->yres > yres_max)
		var->yres = yres_max;

	if (var->xres > var->xres_virtual)
		var->xres = var->xres_virtual;
	if (var->yres > var->yres_virtual)
		var->yres = var->yres_virtual;

	return 0;
}

static void shrink_height(unsigned long max_frame_size,
		struct fb_var_screeninfo *var) {
	DBG("can't fit FB into memory, reducing y\n");
	var->yres_virtual = max_frame_size / (var->xres_virtual
			* var->bits_per_pixel >> 3);

	if (var->yres_virtual < ACT_FB_PLANE_YRES_MIN)
		var->yres_virtual = ACT_FB_PLANE_YRES_MIN;

	if (var->yres > var->yres_virtual)
		var->yres = var->yres_virtual;
}

static void shrink_width(unsigned long max_frame_size,
		struct fb_var_screeninfo *var) {
	DBG("can't fit FB into memory, reducing x\n");
	var->xres_virtual = max_frame_size / var->yres_virtual
			/ (var->bits_per_pixel >> 3);

	if (var->xres_virtual < ACT_FB_PLANE_XRES_MIN)
		var->xres_virtual = ACT_FB_PLANE_XRES_MIN;

	if (var->xres > var->xres_virtual)
		var->xres = var->xres_virtual;
}

static int check_fb_size(const struct act_fb_info *ofbi,
		struct fb_var_screeninfo *var) {
	unsigned long max_frame_size = ofbi->region.size;
	int bytespp = var->bits_per_pixel >> 3;
	unsigned long line_size = var->xres_virtual * bytespp;

	DBG("max frame size %lu, line size %lu\n", max_frame_size, line_size);

	if (line_size * var->yres_virtual > max_frame_size)
		shrink_height(max_frame_size, var);

	if (line_size * var->yres_virtual > max_frame_size) {
		shrink_width(max_frame_size, var);
		line_size = var->xres_virtual * bytespp;
	}

	if (line_size * var->yres_virtual > max_frame_size) {
		DBG("cannot fit FB to memory\n");
		return -EINVAL;
	}
	return 0;
}
void set_fb_fix(struct fb_info *fbi) {
	struct fb_fix_screeninfo *fix = &fbi->fix;
	struct fb_var_screeninfo *var = &fbi->var;
	struct act_fb_info *ofbi = FB2OFB(fbi);
	struct act_fb_mem_region *rg = &ofbi->region;

	DBG("set_fb_fix\n");

	/* used by open/write in fbmem.c */
	fbi->screen_base = (char __iomem *)ofbi->region.vaddr;
	fix->smem_len = rg->size;
	fix->smem_start = ofbi->region.paddr;

	/* used by mmap in fbmem.c */
	fix->line_length = (var->xres_virtual * var->bits_per_pixel) >> 3;

	fix->type = FB_TYPE_PACKED_PIXELS;

	if (var->nonstd)
		fix->visual = FB_VISUAL_PSEUDOCOLOR;
	else {
		switch (var->bits_per_pixel) {
		case 32:
		case 24:
		case 16:
		case 12:
			fix->visual = FB_VISUAL_TRUECOLOR;
			/* 12bpp is stored in 16 bits */
			break;
		case 1:
		case 2:
		case 4:
		case 8:
			fix->visual = FB_VISUAL_PSEUDOCOLOR;
			break;
		}
	}
	fix->accel = FB_ACCEL_NONE;

	fix->xpanstep = 32 / var->bits_per_pixel;
	fix->ypanstep = 1;
	fix->ywrapstep = 0;

}
/* check new var and possibly modify it to be ok */
int check_fb_var(struct fb_info *fbi, struct fb_var_screeninfo *var) {
	struct act_fb_info *ofbi = FB2OFB(fbi);
	struct asoc_display_device *display = FBDISPAY(ofbi);

	DBG("check_fb_var %d\n", ofbi->id);
	DBG("var->rotate %d\n", var->rotate);
	DBG("var->xres %d , var->yres %d \n", var->xres, var->yres);
	DBG("var->xres_virtual %d , var->yres_virtual %d \n", var->xres_virtual,
			var->yres_virtual);

	var->grayscale = 0;
	if (display && display->disp_cur_mode) {
		struct de_video_mode * mode =
				(struct de_video_mode *) display->disp_cur_mode;
		/*pixclock in ps, the rest in pixclock */
		var->pixclock = mode->pixclock;
		var->left_margin = mode->hfp;
		var->right_margin = mode->hbp;
		var->upper_margin = mode->vfp;
		var->lower_margin = mode->vbp;
		var->hsync_len = mode->hsw;
		var->vsync_len = mode->vsw;
	} else {
		var->pixclock = 0;
		var->left_margin = 0;
		var->right_margin = 0;
		var->upper_margin = 0;
		var->lower_margin = 0;
		var->hsync_len = 0;
		var->vsync_len = 0;
	}

	set_color_bitfields(var);

	DBG("check_fb_var %d\n", ofbi->id);
	if (var->rotate < 0 || var->rotate > 3) {
		printk("check fb var rotate error \n");
		return -EINVAL;
	}
	if (check_fb_res_bounds(var)) {
		printk("check fb res bounds error \n");
		return -EINVAL;
	}
	if (check_fb_size(ofbi, var)) {
		printk("check fb size error \n");
		return -EINVAL;
	}
	if (var->xres + var->xoffset > var->xres_virtual)
		var->xoffset = var->xres_virtual - var->xres;
	if (var->yres + var->yoffset > var->yres_virtual)
		var->yoffset = var->yres_virtual - var->yres;

	DBG("xres = %d, yres = %d, vxres = %d, vyres = %d\n", var->xres, var->yres,
			var->xres_virtual, var->yres_virtual);

	/* TODO: get these from panel->config */
	var->vmode = FB_VMODE_NONINTERLACED;
	var->sync = 0;
	return 0;
}

/*
 * ---------------------------------------------------------------------------
 * fbdev framework callbacks
 * ---------------------------------------------------------------------------
 */
static int act_fb_open(struct fb_info *fbi, int user) {
	DBG("act_fb_open(%d)\n", FB2OFB(fbi)->id);
	if (FB2OFB(fbi)->id == 0x01) {
		printk("is not double lcd ");
		//return -1;
	}
	return 0;
}

static int act_fb_release(struct fb_info *fbi, int user) {
	DBG("act_fb_release(%d)\n", FB2OFB(fbi)->id);
	return 0;
}

/* setup overlay according to the fb */
static int act_fb_update_layer(struct fb_info *fbi, struct act_layer *ovl,
		u16 posx, u16 posy, u16 outw, u16 outh) {
	int r = 0;
	struct act_fb_info *ofbi = FB2OFB(fbi);
	struct fb_var_screeninfo *var = &fbi->var;
	struct fb_fix_screeninfo *fix = &fbi->fix;
	struct act_layer_info *info;

	int xres, yres;
	int img_width;

	DBG("setup_overlay %d, posx %d, posy %d, outw %d, outh %d\n", ofbi->id,
			posx, posy, outw, outh);

	xres = var->xres;
	yres = var->yres;

	DBG("offset %d, %d \n", var->xoffset, var->yoffset);

	DBG("paddr %x, vaddr %p\n", ofbi->region.paddr, ofbi->region.vaddr);

	img_width = fix->line_length / (var->bits_per_pixel >> 3);

	info = ovl->info;
	//ovl->get_layer_info(ovl, &info);

	DBG("ovl %p, ovl->info %p %p\n", ovl, ovl->info, info);
	info->enabled = 1;

	info->paddr = ofbi->region.paddr + (((var->yoffset * var->xres_virtual
			+ var->xoffset) * var->bits_per_pixel) >> 3);
	info->vaddr = ofbi->region.vaddr + (((var->yoffset * var->xres_virtual
			+ var->xoffset) * var->bits_per_pixel) >> 3);
	info->img_width = img_width;
	info->width = xres;
	info->height = yres;

	info->pos_x = posx;
	info->pos_y = posy;
	info->out_width = outw;
	info->out_height = outh;

	switch (var->bits_per_pixel) {
	case 16:
		info->pixel_format = (var->green.length == 5) ? ABGR_1555 : RGB_565;
		break;
	case 32:
		info->pixel_format = ABGR_8888; 
		break;
	}
	
	ovl->needconfig = 1;

	r = ovl->set_layer_info(ovl, info);

	if (r) {
		printk("ovl->setup_overlay_info failed\n");
		goto err;
	}

	DBG("ovl->setup_overlay_info ok\n");

	return 0;

	err: printk("setup_overlay failed\n");
	return r;
}

/* apply var to the overlay */
int act_fb_apply_changes(struct fb_info *fbi, int init) {
	int r = 0;
	struct act_fb_info *ofbi = FB2OFB(fbi);
	struct fb_var_screeninfo *var = &fbi->var;
	struct act_layer *ovl;
	u16 posx, posy;
	u16 outw, outh;

	DBG("act_fb_apply_changes called \n");
	DBG(" ofbi->layers %p %p\n", ofbi->layer_mgr->layers[ofbi->layer_mgr->fb_layer_current],
			ofbi->layer_mgr->layers[1]);
	ovl = ofbi->layer_mgr->layers[ofbi->layer_mgr->fb_layer_current];

	DBG("apply_changes, fb %d, ovl %d\n", ofbi->id, ovl->layer_id);

	outw = var->xres;
	outh = var->yres;

	if (init) {
		posx = 0;
		posy = 0;
	} else {
		posx = ovl->info->pos_x;
		posy = ovl->info->pos_y;
	}

	r = act_fb_update_layer(fbi, ovl, posx, posy, outw, outh);

	if (r)
		goto err;

	DBG("overlay apply_layer %p %p\n", ovl, ovl->apply_layer);
	if(!init){
		ovl->apply_layer(ovl);
	}	
	return 0;
	
err: DBG("apply_changes failed\n");
	return r;
}

/* checks var and eventually tweaks it to something supported,
 * DO NOT MODIFY PAR */
static int act_fb_check_var(struct fb_var_screeninfo *var,
		struct fb_info *fbi) {
	int r;

	DBG("check_var(%d)\n", FB2OFB(fbi)->id);

	r = check_fb_var(fbi, var);

	return r;
}

/* set the video mode according to info->var */
static int act_fb_set_par(struct fb_info *fbi) {
	int r;

	DBG("set_par(%d)\n", FB2OFB(fbi)->id);

	set_fb_fix(fbi);

	r = act_fb_apply_changes(fbi, 0);
	if (r) {
		r = -EINVAL;
		printk("set fb par error \n");
	} else {
		FB2OFB(fbi)->layer_mgr->layers[FB2OFB(fbi)->layer_mgr->fb_layer_current]->wait_for_sync();
	}
	return r;
}

static int act_fb_pan_display(struct fb_var_screeninfo *var,
		struct fb_info *fbi) {

	struct fb_var_screeninfo * new_var;
	int r = 0;

	DBG("pan_display(%d)\n", FB2OFB(fbi)->id);

	if (var->xoffset != fbi->var.xoffset || var->yoffset != fbi->var.yoffset) {
		new_var = &fbi->var;
		new_var->xoffset = var->xoffset;
		new_var->yoffset = var->yoffset;
		r = act_fb_apply_changes(fbi, 0);
		if (r) {
			r = -EINVAL;
			printk("set fb var error \n");
		} else {
			FB2OFB(fbi)->layer_mgr->layers[FB2OFB(fbi)->layer_mgr->fb_layer_current]->wait_for_sync();
		}
		
	}
	/*if(disp_dev->ops->update != NULL){
		printk("cpu lcd update\n");
		disp_dev->ops->update(disp_dev);
	} */ 
	return r;
}

/* Store a single color palette entry into a pseudo palette or the hardware
 * palette if one is available. For now we support only 16bpp and thus store
 * the entry only to the pseudo palette.
 */
static int _setcolreg(struct fb_info *fbi, u_int regno, u_int red, u_int green,
		u_int blue, u_int transp, int update_hw_pal) {
	//struct act_fb_info *ofbi = FB2OFB(fbi);
	//struct act_fb_device *fbdev = ofbi->fbdev;
	struct fb_var_screeninfo *var = &fbi->var;
	int r = 0;
	unsigned int colreg_data = 0;

	enum act_fb_color_format mode = ACT_FB_COLOR_RGB565; /* XXX */

	/*switch (plane->color_mode) {*/
	switch (mode) {
	case ACT_FB_COLOR_YUV422:
	case ACT_FB_COLOR_YUV420:
	case ACT_FB_COLOR_YUY422:
		r = -EINVAL;
		break;
	case ACT_FB_COLOR_CLUT_8BPP:
	case ACT_FB_COLOR_CLUT_4BPP:
	case ACT_FB_COLOR_CLUT_2BPP:
	case ACT_FB_COLOR_CLUT_1BPP:
		colreg_data = (transp << 24) | (red << 16) | (green << 8) | (blue);
		//act_fb_write_colreg(regno, colreg_data);
	case ACT_FB_COLOR_RGB565:
	case ACT_FB_COLOR_RGB444:
	case ACT_FB_COLOR_RGB24P:
	case ACT_FB_COLOR_RGB24U:
		if (r != 0)
			break;

		if (regno < 0) {
			r = -EINVAL;
			break;
		}
		if (regno < 16) {
			u16 pal;
			pal
					= ((red >> (16 - var->red.length)) << var->red.offset)
							| ((green >> (16 - var->green.length))
									<< var->green.offset) | (blue >> (16
							- var->blue.length));

			((u32 *) (fbi->pseudo_palette))[regno] = pal;
		}
		break;
	default:
		BUG();
	}
	return r;
}

static int act_fb_setcolreg(u_int regno, u_int red, u_int green, u_int blue,
		u_int transp, struct fb_info *info) {
	DBG("setcolreg\n");

	return _setcolreg(info, regno, red, green, blue, transp, 1);
}

static int act_fb_setcmap(struct fb_cmap *cmap, struct fb_info *info) {
	int count, index, r;
	u16 *red, *green, *blue, *transp;
	u16 trans = 0xffff;

	DBG("setcmap\n");

	red = cmap->red;
	green = cmap->green;
	blue = cmap->blue;
	transp = cmap->transp;
	index = cmap->start;
	DBG("setcmap end \n");
	for (count = 0; count < cmap->len; count++) {
		if (transp)
			trans = *transp++;
		r = _setcolreg(info, index++, *red++, *green++, *blue++, trans,
				count == cmap->len - 1);
		if (r != 0)
			return r;
	}
	DBG("setcmap end \n");
	return 0;
}
static int act_fb_set_display_blank(struct fb_info *fbi,int blank) {
	
	struct act_fb_info *ofbi = FB2OFB(fbi);
	struct asoc_display_device *display = FBDISPAY(ofbi);
	int new_status = 0;
	switch (blank) {
		case FB_BLANK_UNBLANK:
			new_status = 1;
			break;
		case FB_BLANK_POWERDOWN:
			new_status = 0;		
			break;
		default : return 0;
	}
	if(display->state != new_status){		
		mutex_lock(&display->lock);
		display->state  = new_status;
	  if (display->driver && display->driver->update_status){
	    display->driver->update_status(display);
	  }
	  mutex_unlock(&display->lock);
	}
	return 0;
}

static int act_fb_blank(int blank, struct fb_info *fbi) {

	int r = 0;
	struct act_fb_info *ofbi = FB2OFB(fbi);
	struct act_fb_device *fbdev = ofbi->fbdev;
	
	printk("act_fb_blank called blank is %d \n ",blank);
	
	act_fb_lock(fbdev);

	switch (blank) {
	case FB_BLANK_UNBLANK:
		act_fb_set_display_blank(fbi,FB_BLANK_UNBLANK);
		break;
	case FB_BLANK_NORMAL:
	case FB_BLANK_VSYNC_SUSPEND:
	case FB_BLANK_HSYNC_SUSPEND:
		break;
	case FB_BLANK_POWERDOWN:
		act_fb_set_display_blank(fbi,FB_BLANK_POWERDOWN);
		break;

	default:
		r = -EINVAL;
	}
	act_fb_unlock(fbdev);

	return r;
}

static struct fb_ops act_fb_ops = { 
	  .owner = THIS_MODULE,
		.fb_open = act_fb_open,
		.fb_release = act_fb_release,
		.fb_fillrect    = cfb_fillrect,
		.fb_copyarea    = cfb_copyarea,
		.fb_imageblit   = cfb_imageblit,
		.fb_blank = act_fb_blank, 
		.fb_ioctl = act_fb_ioctl, 
		.fb_check_var =	act_fb_check_var, 
		.fb_set_par = act_fb_set_par,
		.fb_pan_display = act_fb_pan_display, 
		.fb_setcolreg =	act_fb_setcolreg, 
		.fb_setcmap = act_fb_setcmap,		
    /*.fb_write	= act_fb_write,*/
};

static void act_fb_free_fbmem(struct fb_info *fbi) {

	struct act_fb_info *ofbi = FB2OFB(fbi);
	struct act_fb_mem_region *rg;
	DBG("free_fbmem\n");
	rg = (struct act_fb_mem_region *) &ofbi->region;
	if (rg->paddr) {
		//	if (act_vram_free(rg->paddr, rg->size))
		//		dev_err(fbdev->dev, "VRAM FREE failed\n");
	}
	if (rg->vaddr)
		iounmap(rg->vaddr);

	rg->vaddr = NULL;
	rg->paddr = 0;
	rg->size = 0;
}

static void clear_fb_info(struct fb_info *fbi) {
	memset(&fbi->var, 0, sizeof(fbi->var));
	memset(&fbi->fix, 0, sizeof(fbi->fix));
	strlcpy(fbi->fix.id, MODULE_NAME, sizeof(fbi->fix.id));
}

static int act_fb_free_all_fbmem(struct act_fb_device *fbdev) {
	int i;

	DBG("free all fbmem\n");

	for (i = 0; i < fbdev->num_fbs; i++) {
		struct fb_info *fbi = fbdev->fbs[i];
		act_fb_free_fbmem(fbi);
		clear_fb_info(fbi);
	}

	return 0;
}

static int act_fb_alloc_fbmem(struct fb_info *fbi, unsigned long size,int index, int fb_mode) {
	struct act_fb_info *ofbi = FB2OFB(fbi);
    struct act_fb_device *fbdev = ofbi->fbdev;
	unsigned long page;
	unsigned map_size = PAGE_ALIGN(size);

	struct act_fb_mem_region *rg;

	rg = &ofbi->region;

	DBG("size = %lx, map_size = %x \n", size, map_size);
#if 0
	vaddr = dma_alloc_coherent(NULL, map_size, &paddr, GFP_KERNEL);

	if (!vaddr) {
		printk(KERN_ERR "fail to allocate fb mem (size: %dK))\n", map_size / 1024);
		return -ENOMEM;
	}

	memset(vaddr, 0x00, map_size);

	page = (unsigned long) paddr;

	SetPageReserved(pfn_to_page(page >> PAGE_SHIFT));

	DBG("allocated VRAM paddr %x, vaddr %p\n", paddr, vaddr);

	rg->paddr = paddr;
	rg->vaddr = vaddr;
#endif
	
  if((index + 1) * map_size > fbdev->fb_size){
       printk(KERN_ERR "fail to allocate fb mem (size: %dK))\n", map_size / 1024);
       return -ENOMEM;
  }
  if(fb_mode){
		rg->paddr = fbdev->fb_phys + (fbdev->num_fbs - index - 1) * map_size;
		rg->vaddr = fbdev->fb_base + (fbdev->num_fbs - index - 1) * map_size;
	}else{
		rg->paddr = fbdev->fb_phys + index * map_size;
		rg->vaddr = fbdev->fb_base + index * map_size;
	}
	rg->size = map_size;

	return 0;
}
static int act_fb_allocate_all_fbs_mem(struct act_fb_device *fbdev) {
	int i, r;
	struct fb_info *fbi;
	struct act_fb_info *ofbi;
	struct asoc_display_device *display;
	unsigned long size;

	for (i = 0; i < fbdev->num_fbs; i++) {
		fbi = fbdev->fbs[i];
		ofbi = FB2OFB(fbi);
	  display = FBDISPAY(ofbi);
		size = display->disp_cur_mode->xres * display->disp_cur_mode->yres * fb_cfg.fb_buffer_number
				* fb_cfg.fb_bpp_info >> 3;

			r = act_fb_alloc_fbmem(fbi, size,i,fb_mode);

		if (r) {
			printk(KERN_ERR "fail to allocate fb num = %d \n", i);
			return r;
		}
	}

	return 0;
}
void fb_video_mode_to_var(struct fb_var_screeninfo *var,
			 const struct de_video_mode *mode)
{
	if(var->rotate == 0 || var->rotate == 2){
			var->xres = mode->xres;
			var->yres = mode->yres;
			var->xres_virtual = mode->xres;
			var->yres_virtual = mode->yres;
	}else{
			var->xres = mode->yres;
			var->yres = mode->xres;
			var->xres_virtual = mode->yres;
			var->yres_virtual = mode->xres;
	}
	var->xoffset = 0;
	var->yoffset = 0;
	var->pixclock = mode->pixclock;
	var->left_margin = mode->hfp;
	var->right_margin = mode->hbp;
	var->upper_margin = mode->hsw;
	var->lower_margin = mode->vfp;
	var->hsync_len = mode->vbp;
	var->vsync_len = mode->vsw;
	var->vmode = mode->vmode & FB_VMODE_MASK;
	DBG("free_resources\n");
	
	DBG("var->xres = %d \n ",var->xres);
	DBG("var->yres = %d \n ",var->yres);
	DBG("var->left_margin = %d \n ",var->left_margin);
	DBG("var->right_margin = %d \n ",var->right_margin);
	DBG("var->upper_margin = %d \n ",var->upper_margin);
	DBG("var->lower_margin = %d \n ",var->lower_margin);
	DBG("var->hsync_len = %d \n ",var->hsync_len);
	DBG("var->vsync_len = %d \n ",var->vsync_len);

}

/* initialize fb_info, var, fix to something sane based on the display */
static int act_per_fb_init(struct act_fb_device *fbdev, struct fb_info *fbi) {
	struct fb_var_screeninfo *var = &fbi->var;
	struct act_fb_info *ofbi = FB2OFB(fbi);
	struct asoc_display_device * display = FBDISPAY(ofbi);

	int r = 0;

	fbi->fbops = &act_fb_ops;
	fbi->flags = FBINFO_FLAG_DEFAULT;
	fbi->pseudo_palette = fbdev->pseudo_palette;

	var->nonstd = 0;

	var->bits_per_pixel = 16;
  printk("display = %p display->disp_cur_mode = %p \n" ,display,display->disp_cur_mode);
	if(FB2OFB(fbi)->id == 0x01 && (display->disp_cur_mode->xres <  display->disp_cur_mode->yres)){
		var->rotate = 1; 
	}
	fb_video_mode_to_var(var, display->disp_cur_mode);

	r = check_fb_var(fbi, var);

	if (r)
		goto err;

	set_fb_fix(fbi);

	r = fb_alloc_cmap(&fbi->cmap, 256, 0);

	fb_set_cmap(&fbi->cmap, fbi);

	if (r)
		dev_err(fbdev->dev, "unable to allocate color map memory\n");

	err: return r;
}

static void fbinfo_cleanup(struct act_fb_device *fbdev, struct fb_info *fbi) {
	fb_dealloc_cmap(&fbi->cmap);
}

static void act_fb_free_resources(struct act_fb_device *fbdev) {
	int i;

	DBG("free_resources\n");

	if (fbdev == NULL)
		return;

	for (i = 0; i < fbdev->num_fbs; i++)
		unregister_framebuffer(fbdev->fbs[i]);

	/* free the reserved fbmem */
	act_fb_free_all_fbmem(fbdev);

	for (i = 0; i < fbdev->num_fbs; i++) {
		fbinfo_cleanup(fbdev, fbdev->fbs[i]);
		framebuffer_release(fbdev->fbs[i]);
	}

	dev_set_drvdata(fbdev->dev, NULL);
	kfree(fbdev);
}

static int act_fb_create_framebuffers(struct act_fb_device *fbdev) {
	int r, i;

	fbdev->num_fbs = 0;

	DBG("create %d framebuffers\n", fb_cfg.fb_number);

	/* allocate fb_infos */
	for (i = 0; i < fb_cfg.fb_number; i++) {
		struct fb_info *fbi;
		struct act_fb_info *ofbi ;
		
		printk("sizeof(struct act_fb_info) = %d \n ", sizeof(struct act_fb_info));

		fbi = framebuffer_alloc(sizeof(struct act_fb_info), fbdev->dev);

		if (fbi == NULL) {
			dev_err(fbdev->dev, "unable to allocate memory for plane info\n");
			return -ENOMEM;
		}

		clear_fb_info(fbi);

		fbdev->fbs[i] = fbi;

		ofbi = FB2OFB(fbi);
		ofbi->fbdev = fbdev;
		ofbi->id = i;

		fbdev->num_fbs++;
		
		if(fb_cfg.displayId == HDMI_DISPLAYER){
			 ofbi->layer_mgr = fbdev->layer_mgr[fb_cfg.fb_number - i - 1];
		}else{
			 ofbi->layer_mgr = fbdev->layer_mgr[i];
		}
   
        
    ofbi->layer_mgr->disp_dev = disp_manager_get_disp_dev(DUMY_DISPLAYER);
    
    if(ofbi->layer_mgr->disp_dev){
    	struct asoc_display_device * display = ofbi->layer_mgr->disp_dev;
    	struct de_video_mode * mode =	(struct de_video_mode *) display->disp_cur_mode;
    	    	
    	if(fb_cfg.fb_xres != 0 && fb_cfg.fb_yres != 0){    		
    		mode->xres = fb_cfg.fb_xres;
    		mode->yres = fb_cfg.fb_yres;
    	}else{
    		struct asoc_display_device * display = disp_manager_get_disp_dev(LCD_DISPLAYER);
    		mode->xres = display->disp_cur_mode->xres;
    		mode->yres = display->disp_cur_mode->yres;
    	}    	
    	ofbi->disp_dev = ofbi->layer_mgr->disp_dev;    
    }
    	
		/* assign layers for the fbs */
	}
 	DBG("fb_infos allocated\n");

	/* allocate fb memories */
	r = act_fb_allocate_all_fbs_mem(fbdev);

	if (r) {
		dev_err(fbdev->dev, "failed to allocate fbmem\n");
		return r;
	}

	DBG("fbmems allocated\n");
	/* setup fb_infos */
	for (i = 0; i < fbdev->num_fbs; i++) {
		r = act_per_fb_init(fbdev, fbdev->fbs[i]);
		if (r) {
			dev_err(fbdev->dev, "failed to setup fb_info\n");
			return r;
		}
	}

	DBG("fb_infos initialized\n");

	for (i = 0; i < fbdev->num_fbs; i++) {
		r = register_framebuffer(fbdev->fbs[i]);
		if (r != 0) {
			dev_err(fbdev->dev, "registering framebuffer %d failed\n", i);
			return r;
		}
	}

	DBG("framebuffers registered\n");

	DBG("create_framebuffers done\n");

	return 0;
}

int act_fb_ioctl(struct fb_info *fbi, unsigned int cmd, unsigned long arg) {
	return 0;
}

static int get_fb_info_cfg(void)
{
	int temp[8];
	if(get_config("fb_info.fb_cfg",(char*)&temp[0], 5 * sizeof(int)) == 0){
		fb_cfg.fb_number = temp[0];
		fb_cfg.fb_buffer_number = temp[1];
		fb_cfg.fb_xres = temp[2];
		fb_cfg.fb_yres = temp[3];
		fb_cfg.fb_bpp_info = temp[4];
		//printk("fb config fb_cfg.fb_number %d fb_cfg.fb_buffer_number %d fb_cfg.fb_xres %d fb_cfg.fb_yres  %d\n",fb_cfg.fb_number,fb_cfg.fb_buffer_number,fb_cfg.fb_xres,fb_cfg.fb_yres );
	}else{
		fb_cfg.fb_number = ACT_FB_CONFIG_NUM_FBS;
		fb_cfg.fb_buffer_number = ACT_FB_NUM_BUFFERS;
		fb_cfg.fb_xres = 0;
		fb_cfg.fb_yres = 0;
		fb_cfg.fb_bpp_info = CONFIG_BPP_MAX;		
		printk("fb config info read failed used default cofnig \n");
  }	
  if(get_config("boot_disp_cfg.disp_cfg",(char*)&temp[0], 1 * sizeof(int)) == 0){
  	fb_cfg.disp_mode = temp[0];
  }else{
  	fb_cfg.disp_mode = 3;
  }	
	fb_cfg.displayId = 0;

	if(act_readl(PATH2_EN)){
		fb_cfg.displayId |= LCD_DISPLAYER;
	}
	if(act_readl(PATH1_EN)){
		fb_cfg.displayId |= HDMI_DISPLAYER;
	}
			
	if(fb_cfg.displayId == 0){
		if(fb_cfg.disp_mode == 2){
			fb_cfg.displayId |= HDMI_DISPLAYER;
		}else{
			fb_cfg.displayId |= LCD_DISPLAYER;
		}		
	}
		
	if(get_config("lcd.rotate",(char*)&temp[0], 1 * sizeof(int)) == 0){
		fb_cfg.rotate = temp[0];
	}else{
		fb_cfg.rotate = 0;
	}
	printk("fb_cfg.rotate ~~~~~~~~~~~~~~~~~~~~~~ = %d ",fb_cfg.rotate);
	return 0;
}
static int __devinit act_fb_probe(struct platform_device *pdev)
{
	struct act_fb_device *fbdev = NULL;
	struct act_fb_info *ofbi = NULL;
	struct act_layer_info *info = NULL;

  struct resource *res = NULL;
  int r = 0;
	int i, num;

	DBG("act_fb_probe\n");

	if (pdev->num_resources == 0) {
		dev_err(&pdev->dev, "probed for an unknown device\n");
		r = -ENODEV;
		goto err0;
	} 
  
  res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "fbmem");

  printk("fbmem res %p, start %x, end %x\n ",res,res->start,res->end);
    

  fbdev = kzalloc(sizeof(struct act_fb_device), GFP_KERNEL);

	if (fbdev == NULL) {
		printk("act_fb_device alloced fiald \n");
		r = -ENOMEM;
		goto err0;
	}
	
  get_fb_info_cfg();
  
	mutex_init(&fbdev->mtx);

	fbdev->dev = &pdev->dev;

  fbdev->fb_size = resource_size(res);
	fbdev->fb_phys = res->start;
	fbdev->fb_base = ioremap_nocache(fbdev->fb_phys, fbdev->fb_size);
	if (!fbdev->fb_base) {
		dev_err(&pdev->dev, "fb can't be mapped\n");
		r = -EBUSY;
		goto cleanup;
	}
    printk("fbdev info size %lx  phys %lx base %p\n",fbdev->fb_size,fbdev->fb_phys,fbdev->fb_base);
	platform_set_drvdata(pdev, fbdev);

	/*init de hdware and layer info*/
	for(i = 0 ; i < MAX_NUM_LAYERS_MANAGER ; i++){
		fbdev->layer_mgr[i] = act_de_get_layer_mgr(i);
	}
	r = act_fb_create_framebuffers(fbdev);

	if (r)
		goto cleanup;
	
	num = act_de_get_num_layers();
	for (i = 0; i < num; i++) {
		fbdev->layers[i] = act_de_get_layer(i);
		fbdev->num_layers++;
		fbdev->layers[i]->layer_init(fbdev->layers[i]);
	}

	DBG("show log infomations \n");	
	
  ofbi = FB2OFB(fbdev->fbs[0]);
  
  //memcpy(ofbi->region.vaddr,WVGA_LOGO, 800 * 1280 * 2);
	
//	memcpy(ofbi->region.vaddr, WVGA_LOGO, 200 * 200 * 2);

	//backup fb vaddr
	//fb_vaddr = (unsigned int)ofbi->region.vaddr;
	
	//set fb vaddr equal to welcome buf defined in mbrc_main
	//ofbi->region.vaddr = 0xb0000000 - 0x2000000 + 32;
	
	if(fb_cfg.displayId == 0){
		printk("not init in boot lcd  clear fb \n");
		memset(ofbi->region.vaddr,0x00, fbdev->fb_size);
		fb_cfg.displayId = LCD_DISPLAYER;
	}else{		
		if(fb_mode){	
			struct act_fb_info *ofbi_temp = NULL;		
			ofbi_temp = FB2OFB(fbdev->fbs[1]);
			memcpy(ofbi->region.vaddr,ofbi_temp->region.vaddr,1280 * 800 * 4);
			//printk(KERN_ERR "@@@@@@@@@@@@@@@@##########fb_mode 0x%x~~~~~~~current addr 0x%x~~~pre addr 0x%x~~~~~\n",fb_mode,ofbi->region.paddr,ofbi_temp->region.paddr);	
		}	
	}
	info = ofbi->layer_mgr->layers[ofbi->layer_mgr->fb_layer_current]->info;
	
	info->enabled = 1;
	info->rotation = fb_cfg.rotate;
	r = act_fb_apply_changes(fbdev->fbs[0],1);
	
    DBG("open disp dev \n");

	disp_manager_open_dev(fb_cfg.displayId);
	
    ofbi->disp_dev  = ofbi->layer_mgr->disp_dev;
    
	
	if(r)
		goto cleanup;

	print_debug_info();
  DBG("act_fb_probe called end \n");
	return 0;

cleanup:
	act_fb_free_resources(fbdev);
err0:
	dev_err(&pdev->dev, "failed to setup act_fb\n");
	return r;
}

static int act_fb_remove(struct platform_device *pdev) {
	struct act_fb_device *fbdev = platform_get_drvdata(pdev);

	act_fb_free_resources(fbdev);

	return 0;
}

static struct platform_driver act_fb_driver = { 
	.probe = act_fb_probe,
	.remove = act_fb_remove, 
	.driver = { 
		.name = "act_fb", 
		.owner =THIS_MODULE, 
	}, 
};
static struct platform_device *act_fb_devices;

static int __init act_fb_init(void)
{
	DBG("act_fb_init\n");
/*
	act_fb_devices = platform_device_register_simple("act_fb", 0,NULL, 0);

	if(IS_ERR(act_fb_devices))
	{
		printk(KERN_ERR "failed to register act_fb device\n");
		ret = PTR_ERR(act_fb_devices);
		return ret;
	}*/

	if (platform_driver_register(&act_fb_driver)) {
		printk(KERN_ERR "failed to register act_fb driver\n");
		return -ENODEV;
	}

	return 0;
}

static void __exit act_fb_exit(void)
{
	DBG("act_fb_exit\n");
	platform_device_unregister(act_fb_devices);
	platform_driver_unregister(&act_fb_driver);

}

/* late_initcall to let panel/ctrl drivers loaded first.
 * I guess better option would be a more dynamic approach,
 * so that act_fb reacts to new panels when they are loaded */
module_init( act_fb_init);
module_exit( act_fb_exit);

MODULE_AUTHOR("Actions_semi,wh <wanghui@actions-semi.com>");
MODULE_DESCRIPTION("ACT_ Framebuffer");
MODULE_LICENSE("GPL");
