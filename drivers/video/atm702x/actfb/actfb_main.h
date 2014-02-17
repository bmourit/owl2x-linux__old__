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

#ifndef __DRIVERS_VIDEO_ACT_FB_H__
#define __DRIVERS_VIDEO_ACT_FB_H__

#ifdef CONFIG_FB_ACT_DEBUG_SUPPORT
#define DEBUG
#endif

#include <linux/fb.h>
#include "../act_de/display.h"

#if 0
#define DBG(format, ...) printk("ACT_FB: " format, ## __VA_ARGS__)
#else
#define DBG(format, ...)
#endif

/* max number of overlays to which a framebuffer data can be direct */
#define ACT_FB_PRIM_LAYER_ID  0
#define ACT_FB_NUM_BUFFERS 4
#define ACT_FB_MAX_OVL_PER_FB 2
#define ACT_FB_CONFIG_NUM_FBS 2

#define CONFIG_XRES_VIRTUAL_MAX 1920
#define CONFIG_YRES_VIRTUAL_MAX 1920
#define CONFIG_BPP_MAX          32

enum act_fb_color_format {
	ACT_FB_COLOR_RGB565 = 0,
	ACT_FB_COLOR_RGB666,
	ACT_FB_COLOR_YUV422,
	ACT_FB_COLOR_YUV420,
	ACT_FB_COLOR_CLUT_8BPP,
	ACT_FB_COLOR_CLUT_4BPP,
	ACT_FB_COLOR_CLUT_2BPP,
	ACT_FB_COLOR_CLUT_1BPP,
	ACT_FB_COLOR_RGB444,
	ACT_FB_COLOR_YUY422,

	ACT_FB_COLOR_ARGB16,
	ACT_FB_COLOR_RGB24U, /* RGB24, 32-bit container */
	ACT_FB_COLOR_RGB24P, /* RGB24, 24-bit container */
	ACT_FB_COLOR_ARGB32,
	ACT_FB_COLOR_RGBA32,
	ACT_FB_COLOR_RGBX32,
	ACT_FB_COLOR_YUV_420_PLANAR,
	ACT_FB_COLOR_YUV_420_SEMI_PLANAR,
	ACT_FB_COLOR_YUV_422_INTERLEAVED,
	ACT_FB_COLOR_YUV_SEMI_PLANAR_COMP,
};

struct act_fb_mem_region {
	u32 			paddr;
	void __iomem 	*vaddr;
	unsigned long 	size;
};

struct act_fb_device {
	struct device *dev;
	struct mutex mtx;
	u32 pseudo_palette[17];
	int state;
	unsigned int num_fbs;
	struct fb_info *fbs[10];

	unsigned num_displays;
	struct asoc_display_device * displays[MAX_NUM_DISP_DEV];

	unsigned num_layers;
	struct act_layer * layers[MAX_NUM_LAYERS];
	struct layer_manager * layer_mgr[MAX_NUM_LAYERS_MANAGER];

    void __iomem *fb_base ;
    unsigned long fb_size ;
    unsigned long fb_phys ;
};

/* appended to fb_info */
struct act_fb_info {
	int id;
	struct act_fb_mem_region region;
	struct act_fb_device *fbdev;
	struct asoc_display_device * disp_dev;
	struct layer_manager * layer_mgr;

};

void set_fb_fix(struct fb_info *fbi);
int check_fb_var(struct fb_info *fbi, struct fb_var_screeninfo *var);
int act_fb_realloc_fbmem(struct fb_info *fbi, unsigned long size, int type);
int act_fb_apply_changes(struct fb_info *fbi, int init);
int act_fb_create_sysfs(struct act_fb_device *fbdev);
void act_fb_remove_sysfs(struct act_fb_device *fbdev);
int act_fb_ioctl(struct fb_info *fbi, unsigned int cmd, unsigned long arg);

static inline void act_fb_lock(struct act_fb_device *fbdev) {
	mutex_lock(&fbdev->mtx);
}

static inline void act_fb_unlock(struct act_fb_device *fbdev) {
	mutex_unlock(&fbdev->mtx);
}

#define FB2OFB(fb_info) ((struct act_fb_info *)(fb_info->par))
#define FBDISPAY(act_fb_info) ((struct asoc_display_device *)(ofbi->disp_dev))

static inline int act_fb_overlay_enable(struct act_layer *ovl, int enable) {
	//	struct act_overlay_info info;
	return 0;
}

#endif
