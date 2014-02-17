/***************************************************************************
 *                              GLBASE
 *                            Module: DE driver
 *                 Copyright(c) 2011-2015 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       wanghui     2011-08-19 9:00     1.0             build this file
 ***************************************************************************/
#ifndef __ASM_ARCH_ACT_DISPLAY_H
#define __ASM_ARCH_ACT_DISPLAY_H

#include <linux/list.h>
#include <linux/kobject.h>
#include <linux/device.h>
#include <asm/atomic.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/fb.h>
#include <linux/notifier.h>
#include "de.h"

#define MAX_NUM_DISP_DEV 5
#define MAX_NUM_LAYERS   4
#define MAX_NUM_LAYERS_MANAGER   2
#define MAX_NUM_LAYERS_PER_MANAGER   2

#define ACT_DE_LAYER_CAP_SCALE 1

#define LCD_DVI_DISPLAYER 0x40
#define LCD_CVBS_HDMI_DISPLAYER  (LCD_DISPLAYER | TV_CVBS_DISPLAYER  | HDMI_DISPLAYER)
#define LCD_HDMI_DISPLAYER       (LCD_DISPLAYER | HDMI_DISPLAYER)
#define LCD_CVBS_DISPLAYER       (LCD_DISPLAYER | TV_CVBS_DISPLAYER)
#define LCD_YPBPR_DISPLAYER		 (LCD_DISPLAYER | TV_YPBPR_DISPLAYER)
#define LCD_DOUBLE_DISPLAYER	 (LCD_DISPLAYER | LCD_DISPLAYER_SECOND)

#define FB_SYN_DISP_MASK  		 TV_YPbPr_DISPLAYER

#define DISPLAYER_TYPE_SHIFT	0
#define DISPLAYER_TYPE_MASK	(0xf << DISPLAYER_TYPE_SHIFT)

#define DISPLAYER_NUM_SHIFT	8
#define DISPLAYER_NUM_MASK	(0xf << DISPLAYER_NUM_SHIFT)

#define TV_1080P_60HZ    0 
#define TV_1080P_50HZ    1
#define TV_720P_60HZ     2
#define TV_720P_50HZ     3

struct asoc_display_device;
struct de_video_mode;
struct layer_manager;

enum de_display_mode {
	DE_SINGLE_DISP = 0,
	DE_DOUBLE_DISP = 1,
	DE_SYNC_DISP = 2,
};

enum de_out_channel {
	ACT_DE_CHANNEL_LCD = 0,
	ACT_DE_CHANNEL_TVOUT = 1,
	ACT_DE_CHANNEL_HDMI = 2,
};

enum de_trans_key_type {
	DE_COLOR_KEY_GFX_DST = 0, DE_COLOR_KEY_VID_SRC = 1,
};

#define TV_MODE_HDMI_1920x1080p_60Hz    	86
#define TV_MODE_HDMI_1920x1080p_50Hz 		31
#define TV_MODE_HDMI_1280x720p_60Hz			74
#define TV_MODE_HDMI_1280x720p_50Hz			19

#define TV_MODE_YPBPR_1920x1080p_60Hz		4
#define TV_MODE_YPBPR_1920x1080p_50Hz		5
#define TV_MODE_YPBPR_1280x720p_60Hz		6
#define TV_MODE_YPBPR_1280x720p_50Hz		7

struct rgb_color {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

struct act_layer_info {
	bool enabled;	
	u32 opacity;
	u8 global_alpha;
	u8 rotation;
	u8 scale_option; /*see DISP_SCALE_OPTION*/
	u16 scale_rate;
	u16 screen_width; /*line length in pixel*/
	u16 screen_height;
	int ilace;
	/*source img  information*/
	u32 pixel_format;
	u32 img_width;
	u32 img_height;
	/*input information*/
	u32 xoff;
	u32 yoff;
	u16 width;
	u16 height;
	/* output information*/
	u16 pos_x;
	u16 pos_y;
	u16 out_width; /* if 0, out_width == width */
	u16 out_height; /* if 0, out_height == height */
	/*addr information */
	u32 paddr;
  	void __iomem *vaddr;
};

struct act_layer {
	struct kobject kobj;	
	struct list_head list;
	/* static fields */
	int layer_id;
	int caps;
	int reg_base;
	/*this tag used show layer is A or B layer in path */
	int layer_tag;
	/* dynamic fields */
	struct act_layer_info * info;
	struct act_layer * link_layer;
	struct layer_manager * layer_mgr;
	/* if true, info has been changed, but not applied() yet */
	bool needconfig;
	int (*layer_init)(struct act_layer *ovl);
	int (*set_layer_info)(struct act_layer *ovl,
			struct act_layer_info *info);
	void (*get_layer_info)(struct act_layer *ovl,
			struct act_layer_info *info);
	int (*apply_layer)(struct act_layer *ovl);
	int (*wait_for_sync)(void);
};

struct disp_manager {
	struct kobject kobj;
	struct list_head list;

	int disp_mode;  
	int disp_dev_count;	

	struct asoc_display_device * disp_dev[MAX_NUM_DISP_DEV];
	struct asoc_display_device * current_disp_dev;

	int current_disp_ids;
	int current_disp_num;

	__u32 current_disp_xres;
	__u32 current_disp_yres;
	
	int plugged_disp_dev_ids;
	int registered_disp_dev_ids;
	
	struct notifier_block disp_modify_notif;
	wait_queue_head_t wait_queue;		
	u16 tvout_scale_rate;
	int is_need_rot_for_hdmi;
};

struct layer_manager {
	struct kobject kobj;
	int layer_manager_id;
	int is_enable;
	int path_id;
	int default_color;
	int trans_key;
	int global_alpha;
	
	bool trans_enabled;
	bool alpha_enabled;
	bool color_key_enabled;

	int color_key_max_value;
	int color_key_min_value;
	int color_key_match_value;	
	int fb_layer;
	bool video_on;
	int fb_layer_current;
	struct asoc_display_device * disp_dev;
	struct layer_manager * link_mgr;
	struct act_layer *layers[2];

	int (* clone_form_main_manager)(struct layer_manager * main_layer_mgr,struct layer_manager * clone_layer_mgr);	
	int (* config_manager_info)(struct layer_manager * layer_mgr);
	int (* apply_layers)(struct layer_manager * layer_mgr);
};

struct de_video_mode {
	const char *name;	/* optional */
	u16 refresh;		/* optional - Unit: pixels */
	u16 xres;			/* Unit: pixels */
	u16 yres;			/* Unit: KHz */
	u32 pixclock;		/* Unit: pixel clocks */
	u16 hsw;			/* Horizontal synchronization pulse width - Unit: pixel clocks */
	u16 hfp;			/* Horizontal front porch - Unit: pixel clocks */
	u16 hbp;			/* Horizontal back porch - Unit: line clocks */
	u16 vsw;			/* Vertical synchronization pulse width - Unit: line clocks */
	u16 vfp;			/* Vertical front porch - Unit: line clocks */
	u16 vbp;			/* Vertical back porch */		
	u16 vmode;
	u16 flag;
	u16 vid;			/* optional */
};

struct asoc_display_driver {
	struct device_driver driver;
	int (*probe)(struct asoc_display_device *);
	void (*remove)(struct asoc_display_device *);
	int (*suspend)(struct asoc_display_device *display);
	int (*resume)(struct asoc_display_device *display);	
	int (*update_mode)(struct asoc_display_device *);		
	int (*update_status)(struct asoc_display_device *);	
	int (*get_status)(struct asoc_display_device *);
	
	unsigned int (*get_devclk_khz)(struct asoc_display_device *, unsigned int src_clk_khz, const struct de_video_mode  *mode);
	unsigned int (*get_tvoutpll1_khz)(struct asoc_display_device *, const struct de_video_mode *mode);
	int (*check_fb)(struct fb_info *);
	/* for sync display get parameter for mdsb */
	int (*get_mdsb_parameter)(struct asoc_display_device * dev, int disp_mode , int * hr , int * vr , int * drop, int * delay);
	/* for cpu lcd */
	int (*update_frame)(struct asoc_display_device * dev);
};

struct asoc_display_device {
	struct device dev;
	const char *name;
	struct asoc_display_driver * driver;
	/* used to match device to driver */
	const char *driver_name;
	int display_id;
	struct list_head modelist;
	int num_modes;	
	struct de_video_mode * modes;		
	struct de_video_mode * disp_cur_mode;
	int is_plugged;
	int dither_info;

  /*******parameters that framebuffer driver may change******************/	
	int state;
	int power;
	int fb_blank;
	struct backlight_device *bl_dev;
  /***************************************************************/
	
	struct mutex lock;	
	struct notifier_block fb_notif;
	struct notifier_block cpufreq_notif;
	struct list_head entry;
	/*for disp dev private info */
	void * disp_priv_info ;
};

#define disp_dev_get_drvdata(x)  ((x)->disp_priv_info)
#define to_asoc_display_driver(x) container_of((x), struct asoc_display_driver, driver)
#define to_asoc_display_device(x) container_of((x), struct asoc_display_device, dev)

extern struct act_layer *act_de_get_layer(int num);
extern int act_de_get_num_layers(void);
extern struct layer_manager * act_de_get_layer_mgr(int num);
struct asoc_display_device * disp_manager_get_disp_dev(int disp_dev_id);
void asoc_display_driver_unregister(struct asoc_display_driver *asoc_driver);
int asoc_display_driver_register(struct asoc_display_driver *asoc_driver);
int asoc_display_device_register(struct asoc_display_device *asoc_dev);
void asoc_display_device_unregister(struct asoc_display_device * asoc_dev);
#endif
