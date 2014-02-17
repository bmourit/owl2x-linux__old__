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
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>

#include "de_core.h"
#include "de_attr_sys.h"

const struct de_video_mode dummy_default_mode = {
  .name		= NULL,
	.refresh		= 50,
	.xres		= 1280,
	.yres		= 800,
	.hsw = 32,
	.hfp = 40,
	.hbp = 80,
	.vsw = 6,
	.vfp = 2,	
	.vbp = 15,	
	.vmode		= FB_VMODE_NONINTERLACED,
	.flag		= FB_MODE_IS_STANDARD,
	.vid		= 1,
};
const struct de_video_mode hdmi_default_mode = {
  .name		= NULL,
	.refresh		= 50,
	.xres		= 1280,
	.yres		= 720,
	.hsw = 0,
	.hfp = 0,
	.hbp = 0,
	.vsw = 0,
	.vfp = 0,	
	.vbp = 0,	
	.vmode	= FB_VMODE_NONINTERLACED,
	.flag		= FB_MODE_IS_STANDARD,
	.vid		= 19,
};
/* actions support disp devices */
static struct asoc_display_device lcd1_device = {
  .name     = "lcd1",
  .driver_name  = "act_lcd",
  .display_id   = LCD_DISPLAYER,
};

static struct asoc_display_device lcd2_device = {
  .name     = "lcd2",
  .driver_name  = "act_lcd",
  .display_id   = LCD_DISPLAYER_SECOND,
};

static struct asoc_display_device tv_cvbs_device = {
  .name     = "cvbs",
  .driver_name  = "act_cvbs",
  .display_id   = TV_CVBS_DISPLAYER,
};
static struct asoc_display_device dummy_device = {
  .name     = "act_fb_dummy",
  .driver_name  = "act_fb_dummy",
  .display_id   = DUMY_DISPLAYER,
  .disp_cur_mode = &dummy_default_mode,
  .is_plugged = 1,
};
static struct asoc_display_device hdmi_device = {
  .name     = "hdmi",
  .driver_name  = "act_hdmi",
  .display_id   = HDMI_DISPLAYER,
  .disp_cur_mode = &hdmi_default_mode,
};

static struct asoc_display_device * act_disp_devices[MAX_NUM_DISP_DEV] = {
  &lcd1_device,
  &lcd2_device,
  &tv_cvbs_device,
  &dummy_device,
  &hdmi_device,
};

/* actions display device operations infterface */

/*asoc_display_update_status : open or close display device */
void asoc_display_update_status(struct asoc_display_device *disp_dev)
{
  mutex_lock(&disp_dev->lock);
  if (disp_dev->driver && disp_dev->driver->update_status){
    disp_dev->driver->update_status(disp_dev);
  }
  mutex_unlock(&disp_dev->lock);
}
/*asoc_display_update_mode: modify displayer's mode */
static void asoc_display_update_mode(struct asoc_display_device *disp_dev)
{
  mutex_lock(&disp_dev->lock);
  if (disp_dev->driver && disp_dev->driver->update_mode){
    if(disp_dev->driver->update_mode(disp_dev)){
      printk("update_mode error \n");
    }
  }
  mutex_unlock(&disp_dev->lock);  
}

/* find display device on de bus */
static struct asoc_display_device * find_asoc_display_by_id(int display_id)
{
  struct device * dev = NULL;
  struct asoc_display_device *disp_dev = NULL;  
  int match(struct device *dev, void *data)
  {
    int display_id = *(int *)data;
    int ret = (dev != NULL && to_asoc_display_device(dev)->display_id == display_id);
    DEDBG(KERN_ALERT "dev %p ,display_id =%d,ret = %d \n", dev,display_id,ret);   
    return ret;   
  }
  DEDBG(KERN_ALERT "find_asoc_display_by_id disp_dev %x request\n", display_id);

  dev = bus_find_device(de_get_bus(), NULL, &display_id, match);
  
  if (dev){
    disp_dev = to_asoc_display_device(dev);
  }else{
    printk(KERN_ALERT "display_id = %d not found \n",display_id);
    disp_dev = NULL;
    goto out;
  }
  DEDBG(KERN_ALERT "disp_dev  = %p\n",disp_dev);    
  disp_dev->power = FB_BLANK_UNBLANK;   
  
out:  
  return disp_dev;
}

/* close display dev */
static void disp_manager_close_dev(int disp_dev_id)
{
  struct asoc_display_device * disp_dev = NULL;
  disp_dev = find_asoc_display_by_id(disp_dev_id);
  if(disp_dev){       
    mutex_lock(&disp_dev->lock);    
    if (disp_dev->driver) {     
      disp_dev->fb_blank = FB_BLANK_POWERDOWN;
      disp_dev->state = 0;
      disp_dev->power = FB_BLANK_POWERDOWN;     
      if(disp_dev->driver->update_status)   
        disp_dev->driver->update_status(disp_dev);
    }
    mutex_unlock(&disp_dev->lock);
  }   
  return ;
}
/* open display dev find by disp_dev_id */
void disp_manager_open_dev(int disp_dev_id) {
  
  struct asoc_display_device * disp_dev = NULL;
  struct disp_manager * disp_mgr = &core.disp_mgr;
  
  if(disp_dev_id == LCD_DISPLAYER){
  	struct layer_manager * layer_mgr0 = &core.layer_mgr[0];  
	  disp_dev = find_asoc_display_by_id(disp_dev_id);	    	
  	layer_mgr0->disp_dev = disp_dev; 
	  if(disp_dev){    
	    if(!disp_dev->is_plugged){
	      printk("disp dev %s not plugin id = %x \n",disp_dev->name,disp_dev_id);
	      return ;
	    } 
	    if(disp_dev->display_id == LCD_DISPLAYER){
	    	if(disp_dev->disp_cur_mode->xres < disp_dev->disp_cur_mode->yres){
	    		disp_mgr->is_need_rot_for_hdmi = 1;
	    	}else{
	    		disp_mgr->is_need_rot_for_hdmi = 0;
	    	}
	    	 disp_dev->state = 2;    
					if(!core.is_init_boot){   
			    	asoc_display_update_status(disp_dev);
			    	de_setout_channel(&core);
			    	//disp_dev->state = 3; 
			    	//asoc_display_update_status(disp_dev);  
			    }
	    }	   
	    disp_mgr->current_disp_dev = disp_dev;
	    disp_mgr->current_disp_ids = disp_dev_id;	    
	  }else{
	    printk("disp dev not find by id = %x \n",disp_dev_id);
	  } 
  }else if(disp_dev_id == HDMI_DISPLAYER){
  	struct layer_manager * layer_mgr1 = &core.layer_mgr[0];
  	layer_mgr1->disp_dev = find_asoc_display_by_id(HDMI_DISPLAYER);
  	disp_mgr->is_need_rot_for_hdmi = 0; 
  	layer_mgr1->disp_dev->state = 1;
    disp_mgr->current_disp_num = 1;
    disp_mgr->current_disp_ids = HDMI_DISPLAYER;
    layer_mgr1->path_id = 0;
  }else {
  	struct layer_manager * layer_mgr1 = &core.layer_mgr[0];
    struct layer_manager * layer_mgr2 = &core.layer_mgr[1];
    DEDBG("mutily displayer used double display !\n");
    disp_mgr->disp_mode = DE_DOUBLE_DISP;
    layer_mgr1->disp_dev = find_asoc_display_by_id(LCD_DISPLAYER); 
    layer_mgr2->disp_dev = find_asoc_display_by_id(HDMI_DISPLAYER); 
    layer_mgr1->path_id = 1;
    layer_mgr2->path_id = 0; 
    if(layer_mgr1->disp_dev->disp_cur_mode->xres < layer_mgr1->disp_dev->disp_cur_mode->yres){
  		disp_mgr->is_need_rot_for_hdmi = 1;
  	}else{
  		disp_mgr->is_need_rot_for_hdmi = 0;
  	} 
    if(!disp_mgr->is_need_rot_for_hdmi){
    	layer_mgr2->clone_form_main_manager(layer_mgr1,layer_mgr2);    	
    	layer_mgr1->link_mgr = layer_mgr2;
  	}
    layer_mgr1->is_enable = 1;
    layer_mgr2->is_enable = 1;
    layer_mgr1->disp_dev->state = 3;
    layer_mgr2->disp_dev->state = 1;
    disp_mgr->current_disp_num = 2;
    disp_mgr->current_disp_ids = disp_dev_id;
  }  
}

static BLOCKING_NOTIFIER_HEAD(asoc_display_notifier_list);

static int asoc_display_notifier_callback(struct notifier_block *self,
    unsigned long event, void *data) {
      
  struct disp_manager * disp_mgr = NULL;
  int disp_dev_ids_status[2]; 
  
  int find_match(struct device *dev, void *data)
  {
    struct asoc_display_device * disp_dev = to_asoc_display_device(dev);    
    int * status = (int *)data;
    status[0] |= disp_dev->display_id;
    if (disp_dev->is_plugged == 1) {      
      status[1] |= disp_dev->display_id;      
    }   
    return 1;   
  }

  DEDBG("disp_mgr: asoc_display notifier call back\n");

  disp_mgr = container_of(self, struct disp_manager, disp_modify_notif);

  
  /*disp_dev_ids_status[0] registered_disp_dev_ids ,disp_dev_ids_status[1] = plugged_disp_dev_ids*/
  bus_for_each_dev(de_get_bus(), NULL, &disp_dev_ids_status, find_match);

  DEDBG("disp_mgr: notifier new plug ids = 0x%x\n", disp_dev_ids_status[1]);

  DEDBG("disp_mgr: notifier old plug ids = 0x%x\n",
      disp_mgr->plugged_disp_dev_ids);

  if ((disp_dev_ids_status[0] != disp_mgr->registered_disp_dev_ids)
      || (disp_dev_ids_status[1] != disp_mgr->plugged_disp_dev_ids)) {

    disp_mgr->plugged_disp_dev_ids = disp_dev_ids_status[0];
    disp_mgr->registered_disp_dev_ids = disp_dev_ids_status[1];
    core.poll_notifyer_event |= DISP_DEV_CHANGED;
    core.poll_notifyer = true;
    wake_up_interruptible(&disp_mgr->wait_queue);
  }

  return 0;
}
int disp_manager_switch_displayer(struct de_core *core, int disp_dev_ids) {
  struct disp_manager *disp_mgr = &core->disp_mgr;
  struct asoc_display_device **disp_dev = disp_mgr->disp_dev;
  int i = 0;

  int disp_count = disp_mgr->disp_dev_count;

  disp_mgr->current_disp_dev = NULL;

  if (disp_mgr->current_disp_ids != 0) {

    DEDBG("close current disp dev ,current_disp_ids = %x!\n",
        disp_mgr->current_disp_ids);
    for(i = 1 ; i < MAX_NUM_DISP_DEV; i++){
      if((disp_mgr->current_disp_ids & (1 << i)) != 0){
        disp_manager_close_dev(1 << i);
      }
    }
    disp_mgr->current_disp_num = 0;
    disp_mgr->current_disp_ids = 0;
    disp_mgr->current_disp_dev = NULL;
  }

  if (disp_dev_ids == 0) {
    DEDBG("disp_dev_ids == 0, no need open new disp dev !\n");
    return 0;
  }

  DEDBG("open new disp dev !\n");
  
  disp_count = 0;
  
  if (disp_dev_ids & LCD_DISPLAYER) {
    disp_mgr->current_disp_dev = find_asoc_display_by_id(LCD_DISPLAYER);
    disp_dev[disp_count++] = disp_mgr->current_disp_dev;
    DEDBG("disp mgr current_disp_dev lcd , index is %d , disp_id is %d \n",
        (disp_count - 1), disp_mgr->current_disp_dev->display_id);
  }
  
  if (disp_dev_ids & LCD_DISPLAYER_SECOND) {
    disp_mgr->current_disp_dev = find_asoc_display_by_id(LCD_DISPLAYER_SECOND);
    disp_dev[disp_count++] = disp_mgr->current_disp_dev;
    DEDBG("disp mgr current_disp_dev lcd , index is %d , disp_id is %d \n",
        (disp_count - 1), disp_mgr->current_disp_dev->display_id);
  }

  if (disp_dev_ids & TV_CVBS_DISPLAYER) {
    disp_mgr->current_disp_dev = find_asoc_display_by_id(TV_CVBS_DISPLAYER);
    disp_dev[disp_count++] = disp_mgr->current_disp_dev;
    DEDBG(
        "disp mgr current_disp_dev tv cvbs , index is %d , disp_id is %d \n",
        (disp_count - 1), disp_mgr->current_disp_dev->display_id);
  }
    
  if (disp_dev_ids & TV_YPbPr_DISPLAYER) {
    disp_mgr->current_disp_dev = find_asoc_display_by_id(TV_YPbPr_DISPLAYER);
    disp_dev[disp_count++] = disp_mgr->current_disp_dev;
    DEDBG(
        "disp mgr current_disp_dev tv ypbpr , index is %d , disp_id is %d \n",
        (disp_count - 1), disp_mgr->current_disp_dev->display_id);
  }

  if (disp_dev_ids & HDMI_DISPLAYER) {
    disp_mgr->current_disp_dev = find_asoc_display_by_id(HDMI_DISPLAYER);
    disp_dev[disp_count++] = disp_mgr->current_disp_dev;
    DEDBG(
        "disp mgr current_disp_dev hdmi , index is %d , disp_id is %d \n",
        (disp_count - 1), disp_mgr->current_disp_dev->display_id);
  }

  if (disp_count == 0) {
    DEDBG("the disp_dev_ids %x you want not found !\n", disp_dev_ids);
    return 0;
  }

  disp_mgr->current_disp_num = disp_count;
  disp_mgr->current_disp_ids = disp_dev_ids;

  DEDBG(
      "set output channel to disp dev ,disp_count = %x ,disp_dev_ids = %x\n",
      disp_count, disp_dev_ids);

  if (disp_mgr->current_disp_num == 1) {
    struct layer_manager * layer_mgr1 = &core->layer_mgr[0];
    struct layer_manager * layer_mgr2 = &core->layer_mgr[1];
    DEDBG("single displayer ndisp_id = %x!\n", disp_mgr->current_disp_ids); 
    layer_mgr1->path_id = 1;
    layer_mgr2->path_id = 0;
    layer_mgr1->is_enable = 1;
    layer_mgr2->is_enable = 0;  
    de_path_config(core);
    disp_mgr->disp_mode = DE_SINGLE_DISP;
    layer_mgr1->disp_dev =  disp_mgr->current_disp_dev;   
  } else if (disp_mgr->current_disp_num > 2){
    struct layer_manager * layer_mgr1 = &core->layer_mgr[0];
    struct layer_manager * layer_mgr2 = &core->layer_mgr[1];
    struct mdsb_manager * mdsb_mgr = &core->mdsb_mgr;
    DEDBG("mutily displayer used sync display !\n");
    layer_mgr1->path_id = 0;
    de_path_config(core);
    disp_mgr->disp_mode = DE_SYNC_DISP;
    mdsb_mgr->init(mdsb_mgr);
    mdsb_mgr->config_mdsb(core);
    mdsb_mgr->update_mdsb(core);
    layer_mgr1->disp_dev = disp_mgr->current_disp_dev;  
    layer_mgr1->is_enable = 1;
    layer_mgr2->is_enable = 0;
    
  } else if(disp_mgr->current_disp_num == 2){
    /*if double disp mode ,must used the layermanager 2 */
    struct layer_manager * layer_mgr1 = &core->layer_mgr[0];
    struct layer_manager * layer_mgr2 = &core->layer_mgr[1];
    DEDBG("mutily displayer used double display !\n");
    disp_mgr->disp_mode = DE_DOUBLE_DISP;
    layer_mgr1->disp_dev = disp_dev[0]; 
    layer_mgr2->disp_dev = disp_dev[1]; 
    layer_mgr1->path_id = 1;
    layer_mgr2->path_id = 0;  
    de_path_config(core);
    if(!disp_mgr->is_need_rot_for_hdmi){
    	layer_mgr2->clone_form_main_manager(layer_mgr1,layer_mgr2);    	
    	layer_mgr1->link_mgr = layer_mgr2;
  	}
    layer_mgr2->apply_layers(layer_mgr2);
    layer_mgr1->is_enable = 1;
    layer_mgr2->is_enable = 1;
  }
  
  disp_count = disp_mgr->current_disp_num - 1;  
  for (; disp_count > 0; disp_count--) {
    if (disp_dev[disp_count] != NULL) {
      DEDBG("disp_dev update mode \n");
    	if(!disp_dev[disp_count]->state){
      	  asoc_display_update_mode(disp_dev[disp_count]);
	      disp_dev[disp_count]->state = 1;
	      DEDBG("disp_dev[disp_count]->state = %x!\n",disp_dev[disp_count]->state);
	      asoc_display_update_status(disp_dev[disp_count]);       
      }
    }
  }
  de_setout_channel(core);
  //print_debug_info();
  return 0;
}

struct asoc_display_device * disp_manager_get_disp_dev(int disp_dev_id) {

  struct asoc_display_device * disp_dev = NULL;

  disp_dev = find_asoc_display_by_id(disp_dev_id);
  
  if(disp_dev == NULL){
    printk("disp_manager_get_disp_dev error £¬disp_dev_id = %d  device not found \n",disp_dev_id);  
  }else{
    DEDBG("disp_manager_get_disp_dev disp_dev_id = %d disp_dev = %p display->disp_cur_mode = %p\n", disp_dev_id,disp_dev,disp_dev->disp_cur_mode);
  }  
  return disp_dev;
}
void disp_manager_init(struct de_core * core) {
  int i = 0, r = 0;
  
  struct disp_manager * disp_mgr = &core->disp_mgr;

  init_waitqueue_head(&(disp_mgr->wait_queue)); 
    
  for (i = 0; i < MAX_NUM_DISP_DEV; ++i) {
    
    struct asoc_display_device *display_dev = act_disp_devices[i];
    
    if(display_dev == NULL){
      continue;
    }    
    
    r = asoc_display_device_register(display_dev);
    
    if (r) {
      printk("device %d %s register failed %d\n", i, display_dev->name ?: "unnamed", r);

      while (--i >= 0)
        asoc_display_device_unregister(act_disp_devices[i]);

      goto err_register;
    }
    if (i == 0)
      disp_mgr->current_disp_dev = display_dev;
  }

  disp_mgr->disp_mode = DE_SINGLE_DISP;
  
  disp_mgr->current_disp_ids = LCD_DISPLAYER;
  
  disp_mgr->tvout_scale_rate = ((100 << 8) | 100);
  
  DEDBG("init disp mgr attr\n");
  
  disp_mgr_attr_init(disp_mgr,core);
  
  DEDBG("register disp mgr notify\n");

  memset(&disp_mgr->disp_modify_notif, 0, sizeof(disp_mgr->disp_modify_notif));

  disp_mgr->disp_modify_notif.notifier_call = asoc_display_notifier_callback;

  r =  blocking_notifier_chain_register(&asoc_display_notifier_list, &disp_mgr->disp_modify_notif);

  if (r) {
    printk("failed to register disp mgr notify\n");
  }
  
err_register:
  return;

}
EXPORT_SYMBOL( disp_manager_open_dev);
EXPORT_SYMBOL( disp_manager_get_disp_dev);

