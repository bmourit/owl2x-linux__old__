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
#define DE_SUBSYS_NAME "DE_LAYER"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/stat.h>

#include "display.h"
#include "de_attr_sys.h"
#include "de_core.h"

static int num_layers;
static struct list_head layer_list;
/*graphics layer init */
int act_graphics_layer_init(struct act_layer *ovl) {
	int default_width = 800;
	int default_height = 480;
	struct act_layer_info * info = ovl->info;
	struct asoc_display_device * disp_dev = ovl->layer_mgr->disp_dev;
	if(disp_dev != NULL){
		struct de_video_mode * disp_dev_mode = disp_dev->disp_cur_mode;
	  default_width = disp_dev_mode->xres;
	  default_height = disp_dev_mode->yres;
	}
	DEDBG("act_graphics_layer_init called \n");
	info->global_alpha = 255;
	info->enabled = false;
	info->rotation = 0;
	info->scale_option = SCALE_ORIGINAL_SIZE;
	/*source img  information*/
	info->pixel_format = RGB_565;
	info->img_width = default_width;
	info->img_height = default_height;

	/*input information*/
	info->xoff = 0;
	info->yoff = 0;
	info->width = default_width ;
	info->height = default_height;

	/* output information*/
	info->pos_x = 0;
	info->pos_y = 0;
	info->out_width = default_width;
	info->out_height = default_height;
	info->screen_width = default_width;
	info->screen_height = default_height;
//#if NO_BOOT_DISP
	if(!core.is_init_boot){
  	de_graphics_layer_init(ovl);
  }
//#endif
	return 0;
}
/*video layer init */
int act_video_layer_init(struct act_layer *ovl) {
	int default_width = 800;
	int default_height = 480;
	struct act_layer_info * info = ovl->info;
	struct asoc_display_device * disp_dev = ovl->layer_mgr->disp_dev;
	if(disp_dev != NULL){
		struct de_video_mode * disp_dev_mode = disp_dev->disp_cur_mode;
	  default_width = disp_dev_mode->xres;
	  default_height = disp_dev_mode->yres;
	}

	DEDBG("act_video_layer_init called \n");
	info->global_alpha = 255;
	info->enabled = false;
	info->rotation = 0;
	info->scale_option = SCALE_FULL_SIZE;
	info->scale_rate = ((100 << 8) | 100);
	/*source img  information*/
	info->pixel_format = RGB_565;
	info->img_width = default_width;
	info->img_height = default_height;

	/*input information*/
	info->xoff = 0;
	info->yoff = 0;
	info->width = default_width;
	info->height = default_height;

	/* output information*/
	info->pos_x = 0;
	info->pos_y = 0;
	info->out_width = default_width;
	info->out_height = default_height;
	
	info->screen_width = default_width;
	info->screen_height = default_height;
	de_video_layer_init(ovl);
	return 0;
}

/* Check if layer parameters are compatible with display */
int de_check_layer(struct act_layer *ovl,
		struct asoc_display_device *disp_dev) {
	
	struct act_layer_info *info = ovl->info;
	u16 outw = 0, outh = 0;
	int h_rate,v_rate;
	int max_h_rate, max_v_rate;

	DEDBG("de_check_layer called , info addr = %p\n", ovl->info);

	if (!disp_dev)
		return 0;

	if (info->enabled == 0) {
		DEDBG("layer %x is not enable !\n",ovl->layer_id);
		return 0;
	}

	DEDBG("layer infomations  xres = %d: yres = %d , ids = %d\n",
			info->out_width, info->out_height, disp_dev->display_id);

	DEDBG("check_layer %d: (%d,%d %dx%d -> %dx%d)\n", ovl->layer_id,
			info->pos_x, info->pos_y, info->width, info->height,
			info->out_width, info->out_height);
			
	info->screen_width = disp_dev->disp_cur_mode->xres;
  info->screen_height = disp_dev->disp_cur_mode->yres;

	if (disp_dev->disp_cur_mode->vmode & FB_VMODE_INTERLACED) {
		info->ilace = true;
	} else {
		info->ilace = false;
	}
	
	if ((ovl->caps & ACT_DE_LAYER_CAP_SCALE) == 0) {	
		DEDBG("layer not support scale info->out_width = %d info->out_height = %d, info->width = %d info->height= %d\n",info->out_width,info->out_height,info->width,info->height);
  	outw = info->width;
		outh = info->height;
		info->pos_x = (info->screen_width - outw) / 2;
		info->pos_y = (info->screen_height - outh) / 2;
		DEDBG("layer not support scale outw = %d outh = %d\n",outw,outh);	
	} else {
		DEDBG("info->out_width = %d,info->out_height = %d\n", info->out_width,
				info->out_height);
		switch (info->scale_option) {
		case SCALE_ORIGINAL_SIZE:
				outw = info->width;
				outh = info->height;
			break;
		case SCALE_FIT_SIZE:
			outh = outw * info->height / info->width;
			break;
		case SCALE_CUSTOM_SCALE:			
				if (info->out_width != 0) {
					outw = info->out_width;
				} else {
					outw = info->width;
				}
				if (info->out_height != 0) {
					outh = info->out_height;
				} else {
					outh = info->height;
				}
			break;
		case SCALE_FULL_SIZE:
			outw = info->screen_width;
			outh = info->screen_height;
			v_rate = info->scale_rate >> 8;
			h_rate = info->scale_rate & 0xff;
			DEDBG("scale_rate ,w_scale = %x , h_scale = %x\n",v_rate,h_rate);
			DEDBG("screen_width   = %x,screen_height = %x\n",outw,outh);
      /*
      if fomat is yuv ,h scale and v scale rate is 1/4 ~ 4 ,
      if format is rgb , h scale rate is 1/4 ~ 4 , v sacle rate is  1/2 ~ 2 ,
      */
      max_h_rate = 25;
      max_v_rate = 25;
			if((info->pixel_format == RGB_565) 
				|| (info->pixel_format == ARGB_8888)
				|| (info->pixel_format == ABGR_8888) 
				|| (info->pixel_format == ABGR_1555)){
					 max_v_rate = 50;					
			}
			/*if is ilace mod , both of YUV and RGB formatm, v scale rate 1/2бл2*/			
			if(info->ilace){
				 max_v_rate = 50;				
			}
			
	    if(v_rate < max_v_rate)
	    {
	    	v_rate = max_v_rate;
	    	info->scale_rate = ((v_rate << 8) | h_rate);
	    	printk("v_scale_rate out of limited !\n");
	    }
	    
	    if(h_rate < max_h_rate)
	    {
	    	h_rate = max_h_rate;
	    	info->scale_rate = ((v_rate << 8) | h_rate);
	    	printk("h_scale_rate out of limited !\n");
	    }
	    
			outw = (outw * v_rate / 100) & 0xfffffffc;
			outh = (outh * h_rate / 100) & 0xfffffffc;
			info->pos_x = (info->screen_width - outw) / 2;
			info->pos_y = (info->screen_height - outh) / 2;
			break;
		default :
			printk("scale_option = %x not support\n",info->scale_option);
		}
	}

	if (outw != info->width) {
		core.clk_mgr.need_reconfig = true;
	}
	info->out_width = outw;
	info->out_height = outh;
	DEDBG(
			"check_layer ok,out_width = %x , out_height = %x ,pos_x = %x , pos_y =%x \n",
			info->out_width, info->out_height, info->pos_x, info->pos_y);
	return 0;
}

static int set_layer_info(struct act_layer *ovl,
		struct act_layer_info *info) {

	struct layer_manager * layer_mgr = ovl->layer_mgr;	
	int r = 0;
	
	DEDBG("set_layer_info called \n");
			
	r = de_check_layer(ovl, layer_mgr->disp_dev);

	return r;
}
static void get_layer_info(struct act_layer *ovl,
		struct act_layer_info *info) {
	info = ovl->info;
}
static int apply_layer(struct act_layer *ovl) {
	struct act_layer_info * info = ovl->info;
	struct act_layer * link_layer = NULL;
	struct layer_manager * layer_mgr = ovl->layer_mgr;
	struct layer_manager * link_mgr =  layer_mgr->link_mgr;
	int r = 0;
	
//	if(!layer_mgr->is_enable){
//		return 0;
//	}
	if(link_mgr != NULL && link_mgr->is_enable){
		link_mgr->clone_form_main_manager(layer_mgr,link_mgr);
		link_layer = ovl->link_layer;
	}

	mutex_lock(&core.op_lock);
	DEDBG("set_layer_info called \n");
	switch (ovl->layer_id) {
		case GRAPHIC_LAYER:
			if (ovl->needconfig && info->enabled) {
				DEDBG("de_graphics_layer_config called \n");
				r = de_graphics_layer_config(ovl);
				if(r){
					printk("graphic layer config failed !\n");
					break;
				}
				ovl->needconfig = false;
		  }
		  de_graphics_layer_update(ovl);
		break;
		case VIDEO1_LAYER:
		case VIDEO2_LAYER:
		case VIDEO3_LAYER:
			if(ovl->needconfig && info->enabled){
					DEDBG("de_video_layer_config called,%x--%x \n",info->out_width,info->out_height);
					r = de_video_layer_config(ovl);
				if (r) {
					printk("video layer config failed !\n");
					break;
				}
				ovl->needconfig = false;
			}
			DEDBG("de_video_layer_update called \n");
			de_video_layer_update(ovl);
			break;
	}
	if(link_layer != NULL){	
		switch (link_layer->layer_id) {
			case GRAPHIC_LAYER:
				if (link_layer->needconfig && link_layer->info->enabled) {
					DEDBG("link_layer de_graphics_layer_config called \n");
					r = de_graphics_layer_config(link_layer);
					if(r){
						printk("link_layer graphic layer config failed !\n");
						break;
					}
					link_layer->needconfig = false;
			  }
			  de_graphics_layer_update(link_layer);
			break;
			case VIDEO1_LAYER:
			case VIDEO2_LAYER:
			case VIDEO3_LAYER:
				if(link_layer->needconfig && link_layer->info->enabled){
						DEDBG("de_video_layer_config called,%x--%x \n",link_layer->info->out_width,link_layer->info->out_height);
						r = de_video_layer_config(link_layer);
					if (r) {
						printk("video layer config failed !\n");
						break;
					}
					link_layer->needconfig = false;
				}
				DEDBG("de_video_layer_update called \n");
				de_video_layer_update(link_layer);
				break;
		}
	}
	core.v_vsync.updata_counter = core.v_vsync.counter;
	mutex_unlock(&core.op_lock);
	return 0;
}
static int wait_for_sync(void) {
  int retval = 0;
  if(!core.v_vsync.state){
  	return 0;
  }
	/*retval = wait_event_interruptible_timeout(core.v_vsync.wait_vsync,	core.v_vsync.updata_counter != core.v_vsync.counter,
						       HZ / 10);
	if(!retval){
		DEDBG("wait_event_interruptible time out !\n");
		DEDBG("dump de status : !\n");
		print_debug_info();
		retval = (-ETIMEDOUT);
	}else{
		retval = 0;
	}*/
	
	core.v_vsync.updata_counter = core.v_vsync.counter;  
  //printk("wait_event_interruptible %d \n",core.v_vsync.counter);		
	return retval;
}

static void de_add_layer(struct act_layer *layer) {
	++num_layers;
	list_add_tail(&layer->list, &layer_list);
}

/*static void act_de_uninit_layers(struct platform_device *pdev) {
	struct act_layer *ovl;

	while (!list_empty(&layer_list)) {
		ovl = list_first_entry(&layer_list,
				struct act_layer, list);
		list_del(&ovl->list);
		kobject_del(&ovl->kobj);
		kobject_put(&ovl->kobj);
		kfree(ovl);
	}
	num_layers = 0;
}*/

static void act_de_init_layers(void) {
	int i;
	
	INIT_LIST_HEAD(&layer_list);	
	num_layers = 0;
	
	for (i = 0; i < MAX_NUM_LAYERS ; ++i) {
		struct act_layer *ovl;
		struct act_layer_info *info;
		
		ovl = kzalloc(sizeof(*ovl), GFP_KERNEL);
		info = kzalloc(sizeof(*info), GFP_KERNEL);
		
		if (ovl == NULL) {
			DEDBG("overlay alloc failed: ovl %d\n", i);
			return;
		}		
		if(info == NULL){
			DEDBG(" layer info alloc failed: ovl %d\n", i);
			return;
		}

		switch (i) {
		case 0:
			ovl->layer_id = GRAPHIC_LAYER;
			ovl->layer_init = act_graphics_layer_init;
			ovl->reg_base = GRAPHIC_BASE;
			break;
		case 1:
			ovl->layer_id = VIDEO1_LAYER;
			ovl->caps = ACT_DE_LAYER_CAP_SCALE;
			ovl->layer_init = act_video_layer_init;
			ovl->reg_base = VIDEO1_BASE;
			break;
		case 2:
			ovl->layer_id = VIDEO2_LAYER;
			ovl->caps = ACT_DE_LAYER_CAP_SCALE;
			ovl->layer_init = act_video_layer_init;
			ovl->reg_base = VIDEO2_BASE;
			break;
		case 3:
			ovl->layer_id = VIDEO3_LAYER;
			ovl->caps = ACT_DE_LAYER_CAP_SCALE;
			ovl->layer_init = act_video_layer_init;
			ovl->reg_base = VIDEO3_BASE;
			break;
		}
		ovl->info = info;
		ovl->set_layer_info = &set_layer_info;
		ovl->get_layer_info = &get_layer_info;
		ovl->apply_layer = &apply_layer;
		ovl->wait_for_sync = &wait_for_sync;
				
		de_add_layer(ovl);
    //ovl->layer_init(ovl);
		DEDBG("layer %d is init  ehable %d, %p %p\n", i, ovl->info->enabled,
				&ovl->info, ovl->apply_layer);
	}
	
}
/*follow method for layer manager*/
static int clone_form_main_manager(struct layer_manager * main_layer_mgr,struct layer_manager * clone_layer_mgr) {
	int i = 0;
	struct asoc_display_device * dest_disp_dev = clone_layer_mgr->disp_dev;
	struct asoc_display_device * src_disp_dev = main_layer_mgr->disp_dev;
	
	DEDBG("clone_form_main_manager called disp_dev = %p\n",src_disp_dev);
	for(i = 0 ; i < MAX_NUM_LAYERS_PER_MANAGER ; i ++){
		struct act_layer * src_layer = main_layer_mgr->layers[i];
		struct act_layer * dest_layer = clone_layer_mgr->layers[i];
		struct act_layer_info * src_info = src_layer->info;		
		struct act_layer_info * dest_info = dest_layer->info;
	  DEDBG("src_info = %p dest_info %p\n",src_info,dest_info);
	 
	  src_layer->link_layer =  dest_layer;	 
	  
		dest_info->global_alpha = src_info->global_alpha;
		dest_info->enabled = src_info->enabled;
		//dest_info->rotation = src_info->rotation;
		/*source img  information*/
		dest_info->pixel_format = src_info->pixel_format;
		dest_info->img_width = src_info->img_width;
		dest_info->img_height = src_info->img_height;
		dest_info->paddr = src_info->paddr;
	
		/*input information*/
		dest_info->xoff = src_info->xoff;
		dest_info->yoff = src_info->yoff;
		dest_info->width = src_info->width;
		dest_info->height = src_info->height;
	
		/* output information*/			
		/*out information acoording to disp dev */
		if((dest_layer->caps & ACT_DE_LAYER_CAP_SCALE) == 0) {
				dest_info->pos_x = 0;
				dest_info->pos_y = 0;
				DEDBG("disp_dev = %p disp_dev->disp_cur_mode = %p\n",dest_disp_dev,dest_disp_dev->disp_cur_mode);
				dest_info->out_width = dest_disp_dev->disp_cur_mode->xres ;
				dest_info->out_height = dest_disp_dev->disp_cur_mode->yres;
				dest_info->screen_width = dest_disp_dev->disp_cur_mode->xres ;
				dest_info->screen_height = dest_disp_dev->disp_cur_mode->yres;
	  }else{	  	
	  	int v_rate = 0;
	  	int h_rate = 0;
	  	DEDBG("disp_dev = %p disp_dev->disp_cur_mode = %p\n",dest_disp_dev,dest_disp_dev->disp_cur_mode);
	  	v_rate = dest_info->scale_rate >> 8;
			h_rate = dest_info->scale_rate & 0xff;
	    dest_info->screen_width = dest_disp_dev->disp_cur_mode->xres ;
			dest_info->screen_height = dest_disp_dev->disp_cur_mode->yres;			
			dest_info->out_width = (dest_info->screen_width * v_rate / 100) & 0xfffffffc;
			dest_info->out_height = (dest_info->screen_height * h_rate / 100) & 0xfffffffc;			
			dest_info->pos_x = (dest_info->screen_width - dest_info->out_width) / 2;
			dest_info->pos_y = (dest_info->screen_height - dest_info->out_height) / 2;
	  }
		
		dest_layer->needconfig = true;
	}
	return 0;
}
static int layer_manager_apply_layers(struct layer_manager * layer_mgr){
	int i, rc = 0;	
	printk("layer_manager_apply_layers called \n");
	for(i = 0 ; i < MAX_NUM_LAYERS_PER_MANAGER ; i++){
		struct act_layer * layer = layer_mgr->layers[i];
		struct act_layer_info * info = layer->info;
		if(info->enabled){
			printk("layer->apply_layer called layer id = %d \n",layer->layer_id);
			layer->needconfig = true;
			layer->set_layer_info(layer,info);
			rc = layer->apply_layer(layer);
			if(rc != 0){
				printk("layer manager's layer apply error = %d ",rc);
				return rc;
			}
		}
	}
	return rc;	
}
static int layer_manager_config_info(struct layer_manager * layer_mgr){
	 bool alpha_enable,trans_key_enable;
	 int alpha_value,trans_key_value;
	 int color_key_max,color_key_min,color_key_match;
	 int rc = 0;
	 struct rgb_color back_color ;
	 
	 alpha_enable = true;//layer_mgr->alpha_enabled;
	 alpha_value = layer_mgr->global_alpha;
	 
	 trans_key_enable = layer_mgr->trans_enabled;
	 trans_key_value = layer_mgr->trans_key;
	 
	 color_key_match = layer_mgr->color_key_match_value;
	 color_key_max = layer_mgr->color_key_max_value;
	 color_key_min = layer_mgr->color_key_min_value;
	 DEDBG("alpha_enable = %x , alpha_value = %x \n",alpha_enable,alpha_value);
	 DEDBG("trans_key_enable = %x , trans_key_value = %x \n",trans_key_enable,trans_key_value);
	
	 back_color.red = 0;
	 back_color.green = 0;
	 back_color.blue = 0;
	 
	 de_set_background_color(&back_color);
	 
	 /*if(layer_mgr->layers[0]->info->pixel_format != ARGB_8888){

			 layer_mgr->layers[0]->info->global_alpha = alpha_value;
		
			 rc = de_set_global_alpha(alpha_value,alpha_enable);
			 
			 if(rc){
			 	 printk("set_globle_alpha error! \n");
			 }
			 rc = de_set_color_key(color_key_max,color_key_min,color_key_match,layer_mgr->color_key_enabled);
			 if(rc){
			 	 printk("de_set_color_key error! \n");
			 } 
	 }else{

			 layer_mgr->layers[0]->info->global_alpha = 255;
			 
			 if(layer_mgr->alpha_enabled){
			 	  rc = de_set_global_alpha(alpha_value,false);
			 }else{
			 		rc = de_set_global_alpha(alpha_value,true);
			 }
			 if(rc){
			 	 printk("set_globle_alpha error! \n");
			 }
			 layer_mgr->color_key_enabled = false;
			 rc = de_set_color_key(color_key_max,color_key_min,color_key_match,layer_mgr->color_key_enabled);
			 if(rc){
			 	 printk("de_set_color_key error! \n");
			 }
		}*/
	 return rc;
}
void layers_manager_init(struct de_core *core) {
	int i = 0;
	struct layer_manager *layer_mgr = NULL;
	
	/* int all layers  first */	
	act_de_init_layers();
		
	/*int layer manager and attach layer to layerManager */
	for( i = 0 ; i < MAX_NUM_LAYERS_MANAGER; i++){		
		layer_mgr = &core->layer_mgr[i];		

		layer_mgr->clone_form_main_manager = clone_form_main_manager;
		layer_mgr->config_manager_info = layer_manager_config_info;
		layer_mgr->apply_layers = layer_manager_apply_layers;
		
		layer_mgr->video_on = false;	
		layer_mgr->color_key_enabled = true;		
		layer_mgr->color_key_max_value = 0x00;	
		layer_mgr->color_key_min_value = 0X00;		
		layer_mgr->color_key_match_value = 0X15;
		layer_mgr->global_alpha = 255;
		layer_mgr->alpha_enabled = true;
		
		
		/*as main layer_mgr */
		layer_mgr->layer_manager_id = i;
		if(i == 0){
			layer_mgr->layers[0] = act_de_get_layer(1);
			layer_mgr->layers[1] = act_de_get_layer(0);
			/*first layer used as fb layer*/
			layer_mgr->fb_layer_current = DE_LAYER_A;
			layer_mgr->fb_layer = DE_LAYER_A;
			layer_mgr->is_enable = 1;
			layer_mgr->path_id = 1;
			//layer_mgr->layers[0]->link_layer = act_de_get_layer(2);
			//layer_mgr->layers[1]->link_layer = act_de_get_layer(3);
		}else{
			layer_mgr->layers[0] = act_de_get_layer(3);
			layer_mgr->layers[1] = act_de_get_layer(2);
			/*first layer used as fb layer*/
			layer_mgr->fb_layer_current = DE_LAYER_A;
			layer_mgr->fb_layer = DE_LAYER_A;
			layer_mgr->is_enable = 0;
			layer_mgr->path_id = 0;
			//layer_mgr->layers[0]->link_layer = act_de_get_layer(0);
			//layer_mgr->layers[1]->link_layer = act_de_get_layer(1);
		}	
		layer_mgr->layers[0]->layer_mgr = layer_mgr;
		layer_mgr->layers[1]->layer_mgr = layer_mgr;		
		//layer_mgr->config_manager_info(layer_mgr);
		layer_mgr_attr_init(layer_mgr,core);	
	}
	//core->layer_mgr[0].link_mgr = &core->layer_mgr[1];
	//core->layer_mgr[1].link_mgr = &core->layer_mgr[0];
//#if NO_BOOT_DISP
  if(!core->is_init_boot){
		de_path_config(core);	  
	}
//#endif 
}


/*this interface used by fb and vout */
struct layer_manager * act_de_get_layer_mgr(int num) {
	return &core.layer_mgr[num];
}
int act_de_get_num_layers(void) {
	return num_layers;
}
struct act_layer *act_de_get_layer(int num) {
	int i = 0;
	struct act_layer *ovl;
	list_for_each_entry(ovl, &layer_list, list) {
		if (i++ == num)
		return ovl;
	}
	return NULL;
}

EXPORT_SYMBOL( act_de_get_layer_mgr);
EXPORT_SYMBOL( act_de_get_num_layers);
EXPORT_SYMBOL( act_de_get_layer);
