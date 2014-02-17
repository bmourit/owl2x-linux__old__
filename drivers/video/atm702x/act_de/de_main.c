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
#define DE_SUBSYS_NAME "DE_MAIN"
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/debugfs.h>
#include <linux/io.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <mach/irqs.h>
#include <mach/clock.h>

#include "de_core.h"
#include "de_attr_sys.h"
#include "de.h"
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#define DE_MAJOR 0xF1

struct de_core core;
static struct bus_type de_bus_type;
static struct class * de_class;
static bool early_suspend_called = false;

int de_congfig_sys_clk(struct de_core * core);

/*clock manager */
void clk_mgr_init(struct de_core * core) {
	struct clk_manager *clk = &core->clk_mgr;
    clk->src_clk_type = DECLK_SRC_DEVSCLK;
	clk->clk_rate = 360000;
	clk->init = de_clk_init;
	clk->congfig_clk = de_congfig_sys_clk;
	clk->init(clk);
	clk->need_reconfig = false;
}
int de_congfig_sys_clk(struct de_core * core) {
	unsigned int out_w, in_w;
	struct clk_manager *clk = &core->clk_mgr;
	struct layer_manager *layer_mgr = &core->layer_mgr[0];

	out_w = layer_mgr->layers[VIDEO1_LAYER]->info->out_width;
	in_w = layer_mgr->layers[VIDEO1_LAYER]->info->width;

	DEDBG("out_w %x. in_w %x\n", out_w, in_w);

	de_set_clk_div(clk->src_clk_type, clk->clk_rate);
	clk->need_reconfig = false;
	return 0;
}
/*irq manager */
static irqreturn_t de_irq_handler(int irq, void *dev_id) {
	
	struct de_core * core = dev_id;
	unsigned int tmp = 0;
  
	tmp = act_readl(DE_STAT);

	if (tmp == 0) {
		//printk("de_irq_handler called  end\n");		
		return IRQ_NONE;
	}	

	/*if(PATH1_EN){
		while(act_readl(PATH1_FCR) == 1);
	}	
	
	if(PATH2_EN){
		while(act_readl(PATH2_FCR) == 1);
	}*/
	if((act_readl(DE_STAT) & 0x03) != 0){
			core->v_vsync.counter ++;
			wake_up_interruptible(&core->v_vsync.wait_vsync);
	}
  act_writel((act_readl(DE_STAT) | 0x1f) , DE_STAT); 

	return IRQ_HANDLED;
}

s32 de_request_irq(void){
	/* de_clear_scl_irq_pend */
	if (request_irq(IRQ_ASOC_DE, de_irq_handler, IRQF_TRIGGER_HIGH, "ACTION DE", &core)
			!= 0) {
		printk(KERN_ERR "de request interrupt %d failed\n", IRQ_ASOC_DE);
		return -EBUSY;
	}
	
	return 0;
}
static struct task_struct * vsync_kthread_task;
static int count = 0;
static int vsync_kthread(void *arg)
{
	int retval = 0;	
	while (!kthread_should_stop()) {
		if(core.v_vsync.state != 0){		
			retval = wait_event_interruptible(core.v_vsync.wait_vsync,	core.v_vsync.counter != count);			 
			if(!retval){			
				count = core.v_vsync.counter;    
	    	vsync_set_counter(&core, core.v_vsync.counter + 1);		
			} 
		}   
		schedule_timeout_interruptible(1); /* Leave CPU for others. */
	}
	printk("vsync_kthread stop \n");	
	return 0;  /* Not reached, but needed to shut gcc up. */
}

static void vsync_thread_init(void){
	vsync_kthread_task = kthread_run(vsync_kthread, NULL, "vsync_kthread");
}

long act_de_ioctl(struct file *file, u32 cmd, unsigned long arg) {
	int display_ids,scale_rate,disp_mode;
	struct disp_manager *disp_mgr = &core.disp_mgr;	
  DEDBG("act_de_ioctl called \n");		
	switch (cmd) {
	case DEIO_SET_DISPLAYER:
		if (copy_from_user(&display_ids, (void *) arg, sizeof(u32)))
			return -EFAULT;
		DEDBG("DEIOCSET_DISPLAYER display_ids = %x\n", display_ids);
        mutex_lock(&core.ioctl_lock);        
		if (!early_suspend_called)		
			disp_manager_switch_displayer((struct de_core *) &core, display_ids);
		else 
			disp_mgr->current_disp_ids = display_ids;
        mutex_unlock(&core.ioctl_lock);        
		break;
	case DEIO_GET_DISPLAYER:
		if(copy_to_user((void *)arg,&disp_mgr->current_disp_ids,sizeof(u32))){
				return -EFAULT;
		}
		break;
	case DEIO_SET_SCALE_RATE_FULL_SCREEN:
		{ 
			/*layer mgr 0 as output to tvout */
			struct layer_manager * layer_mgr = &core.layer_mgr[1];	
			struct act_layer * v_layer = layer_mgr->layers[DE_LAYER_A];
			if (copy_from_user(&scale_rate, (void *) arg, sizeof(u32)))
				return -EFAULT;
			DEDBG("DEIO_SET_SCALE_RATE_FULL_SCREEN scale_rate = %x\n", scale_rate);
			if(((scale_rate >> 8) & 0xff) < 50){
				return -1;
			}
			if((scale_rate & 0xff) < 50){
				return -1;
			}
			v_layer->info->scale_rate = scale_rate;
			disp_mgr->tvout_scale_rate = scale_rate;
			v_layer->set_layer_info(v_layer,v_layer->info);
			v_layer->apply_layer(v_layer);
			v_layer->wait_for_sync();	
			DEDBG("scale ok \n");
	  }
		break;
	case DEIO_GET_SCALE_RATE_FULL_SCREEN:
		DEDBG("DEIO_GET_SCALE_RATE_FULL_SCREEN scale_rate = %x\n", disp_mgr->tvout_scale_rate);
		if(copy_to_user((void *)arg,&disp_mgr->tvout_scale_rate,sizeof(u16))){
				return -EFAULT;
		}	
		break;
/*	case DEIO_GET_DISP_CHANGED:
		DEDBG("DEIO_GET_DISP_CHANGED called\n");
		display_ids = 0;
		if(core.poll_notifyer | DISP_DEV_CHANGED){
			core.poll_notifyer &= ~DISP_DEV_CHANGED;
			if(disp_mgr->plugged_disp_dev_ids	& TV_CVBS_DISPLAYER)
			{
				display_ids |= TV_CVBS_DISPLAYER;
			}
			if(disp_mgr->plugged_disp_dev_ids	& TV_YPbPr_DISPLAYER)
			{
				display_ids |= TV_YPbPr_DISPLAYER;
			}
			if(disp_mgr->plugged_disp_dev_ids	& HDMI_DISPLAYER)
			{
				display_ids |= HDMI_DISPLAYER;
			}
			if(copy_to_user((void *)arg,&display_ids,sizeof(int))){
					return -EFAULT;
			}
			DEDBG("DEIO_GET_DISP_CHANGED display_ids = %x\n", display_ids);
			if(core.poll_notifyer){
				return 1;
			}else{
				return 0;
			}
		}
		return -1;
		break;
	case DEIO_GET_VIDEO_CHANGED:
		DEDBG("DEIO_GET_VIDEO_CHANGED called\n");
		display_ids = 0;
		if(core.poll_notifyer | VIDEO_ON_SWITCHER){
			core.poll_notifyer &= ~VIDEO_ON_SWITCHER;
			display_ids = layer_mgr->video_on;
			if(copy_to_user((void *)arg,&display_ids,sizeof(int))){
				return -EFAULT;
			}
			if(core.poll_notifyer){
				return 1;
			}else{
				return 0;
			}
		}
		return -1;
		break;
	case DEIO_GET_MDSB_INFO:
		{
			DEDBG("DEIO_GET_VIDEO_CHANGED called\n");
			struct mdsb_info info;
			info.drop_line_front = mgr->lcd_drop;
			info.drop_line_back = mgr->lcd_drop;
			info.xres = mgr->master_disp_w;
			info.yres = mgr->master_disp_h ;
			if(copy_to_user((void *)arg,&info,sizeof(info))){
					return -EFAULT;
			}
			DEDBG("DEIO_GET_MDSB_INFO drop_line = %x,xres = %x,yres = %x\n", info.drop_line_front,info.xres,info.yres);
	  }
	  break;
	case DEIO_SET_OVERLAY_STATUS:
		break;

	case DEIO_SELECT_AS_MAIN_FB:
		break;
 */
 case DEIO_GET_NEXT_VSYNC:
 		 {		 
		 int retval = 0;
		 int sync_info[2];
		 int current_vsync_cnt = 0;		
		 if(copy_from_user(&sync_info,(void *)arg,sizeof(int)*2)){
				return -EFAULT;
		 }
		 if(!core.v_vsync.state){
		 	  return 0;
		 }
		 current_vsync_cnt = sync_info[0];	
		 DEDBG("DEIO_GET_NEXT_VSYNC ,	core.v_vsync.counter = %d current_vsync_cnt = %d \n",core.v_vsync.counter,current_vsync_cnt);		 	 
		 retval = wait_event_interruptible_timeout(core.v_vsync.wait_vsync,	core.v_vsync.counter != current_vsync_cnt,HZ/10);			 
		 if(!retval){  
		    //printk("DEIO_GET_NEXT_VSYNC timeout\n");
		    return -EFAULT;		 			
		 }
		 sync_info[1] = core.v_vsync.counter;
		 if(copy_to_user((void *)arg,&sync_info,sizeof(int)*2)){
				return -EFAULT;
		 }	 
		 DEDBG("DEIO_GET_NEXT_VSYNC ok vsync cnt = %d\n",core.v_vsync.counter);
		}		 
	  break;
	 case DEIO_WAIT_VSYNC:
	 {		 
		 int retval = 0;
		 
		 if(!core.v_vsync.state){
		 	  return 0;
		 }
		 retval = wait_event_interruptible_timeout(core.v_vsync.wait_vsync,	core.v_vsync.updata_counter != core.v_vsync.counter,HZ/10);			 
		 if(!retval){  
		    //printk("DEIO_GET_NEXT_VSYNC timeout\n");
		    return -EFAULT;		 			
		 }
		 DEDBG("DEIO_WAIT_VSYNC ok vsync cnt = %d\n",core.v_vsync.counter);
	}		 
	break;
	case DEIO_ENABLE_VSYNC:
	{		
		int enable = 0;
		if(copy_from_user(&enable,(void *)arg,sizeof(int))){
			return -EFAULT;
		}
	  //core.v_vsync.state = enable;
	  if(core.v_vsync.vsync_type == VSYNC_USING_UEVENT){
			  /*if(enable){
			  	vsync_kthread_task = kthread_run(vsync_kthread, NULL, "vsync_kthread");
			  }else{
			  	 kthread_stop(vsync_kthread_task);
			  }*/
		}
	  DEDBG("core.v_vsync.state = %d\n",core.v_vsync.state);  
	}
	break;
	case DEIO_SET_DISP_MODE:
		 printk("DEIO_SET_DISP_MODE called\n");
	   if(copy_from_user(&disp_mode,(void *)arg,sizeof(int))){
				return -EFAULT;
		 }
		 if((disp_mode & MODE_DISP_MASK) == MODE_DISP_SYNC_DEFAULT 
		 	&& ((disp_mode >>  MODE_OUT_SHIFT) &  MODE_OUT_MASK) != MODE_OUT_TV_GV_LCD_GV ){
		 		printk("this mode not surport %d \n",disp_mode);
		 		return -EFAULT;
		 }
		 disp_mgr->disp_mode = disp_mode;
		 printk("disp_mgr->disp_mode = %x\n",disp_mode);
	  break;
	case DEIO_IS_NEED_ROTATION:
		if(copy_to_user((void *)arg,&disp_mgr->is_need_rot_for_hdmi,sizeof(u32))){
				return -EFAULT;
		}
		break;
	case DEIO_SET_NEED_ROTATION:
		{
			int is_need_rot_for_hdmi = 0;
		  if(copy_from_user(&is_need_rot_for_hdmi,(void *)arg,sizeof(int))){
					return -EFAULT;
			}
			if(is_need_rot_for_hdmi != 0)
			{
					struct layer_manager * layer_mgr = &core.layer_mgr[0];
					layer_mgr->link_mgr = NULL;					
			}
			disp_mgr->is_need_rot_for_hdmi = is_need_rot_for_hdmi;
	  }
		break;
	default:
		/* invalid commands */
		return -EINVAL;
	}
	return 0;

}
static s32 act_de_open(struct inode *inode, struct file *file) {
	return 0;
}
static s32 act_de_release(struct inode *inode, struct file *file) {
	return 0;
}
static unsigned int act_de_poll(struct file * filp, struct poll_table_struct * wait){
	unsigned int mask = 0;
  struct disp_manager * disp_mgr = &core.disp_mgr;
	DEDBG("de start poll wait@@@@@@@@@@@@@@@@@@@@@~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	poll_wait(filp, &disp_mgr->wait_queue, wait);

	if(core.poll_notifyer){		
		
		core.poll_notifyer = false;
		
		mask |= POLLIN | POLLRDNORM;  /* readable */
		
		printk("de poll wake up\n");
	}	
	return mask;
}

static struct file_operations de_fops =
		{ .owner = THIS_MODULE, 
			.open = act_de_open, 
			.release = act_de_release, 
			.poll = act_de_poll,
			.unlocked_ioctl = act_de_ioctl, 
		};
/* PLATFORM DEVICE */
static int act_de_probe(struct platform_device *pdev) {
	int ret = 0;

	DEDBG("gl5201 de probe called\n");
	core.pdev = &pdev->dev;
		
	core.bus = &de_bus_type;
	
	
  core.v_vsync.state = 1;
	core.v_vsync.counter = 0;
	core.v_vsync.vsync_type = VSYNC_USING_IOCTL;
	init_waitqueue_head(&core.v_vsync.wait_vsync);

	mutex_init(&core.op_lock);
    mutex_init(&core.ioctl_lock);

	// set clk type
	clk_mgr_init(&core);

	DEDBG("de clk init ok\n");
	
	core.is_init_boot = de_is_init_boot();
	// init de hw init
//#if NO_BOOT_DISP
  if(!core.is_init_boot){
		 de_hw_init();
  }else{
  	 printk("de init in boot \n");
  	 /*this add for quick boot*/
  	 act_writel(0x03, DE_INTEN);
  }
//#endif

	DEDBG("de hw init ok\n");
	// init layer
	layers_manager_init(&core);

	DEDBG("de_init_layers ok\n");

	// set default disp dev
	disp_manager_init(&core);

	DEDBG("disp_manaer_init ok\n");

	mdsb_manager_init(&core);

	DEDBG("mdsb init  ok\n");

	// reques irq
	ret = de_request_irq();

	if (ret) {
		printk(KERN_ERR "de request irq failed\n");
		return ret;
	}	
	if(core.v_vsync.vsync_type == VSYNC_USING_UEVENT){
		vsync_thread_init();		
	}
	
	platform_set_drvdata(pdev, &core);
	  
	DEDBG("gl5201 de probe finished\n");
	return 0;
}
static bool suspend_called = false;

#ifdef CONFIG_HAS_EARLYSUSPEND
/* tell userspace to stop drawing, wait for it to stop */
static void de_early_suspend(struct early_suspend *h)
{	
	struct disp_manager *disp_mgr = &core.disp_mgr;	
	struct layer_manager * layer_mgr1 = &core.layer_mgr[0];
	struct asoc_display_device * disp_dev = layer_mgr1->disp_dev;	
	struct clk * de_clk = clk_get_sys(CLK_NAME_DE1_CLK, NULL);
    mutex_lock(&core.ioctl_lock);
	early_suspend_called = true;
//	if(disp_mgr->disp_mode == DE_SINGLE_DISP){				
		if(core.v_vsync.vsync_type == VSYNC_USING_UEVENT){
			kthread_stop(vsync_kthread_task);
		}				
		disp_dev->fb_blank = FB_BLANK_POWERDOWN;
		disp_dev->state = 0;
		asoc_display_update_status(disp_dev);		
		de_modify_end();
		de_reg_backup(&core.de_reg_bak);
		de_disable();
		mutex_lock(&core.op_lock);
		de_reset_de();
		core.v_vsync.state  = 1;		
		mutex_unlock(&core.op_lock);	
    mutex_unlock(&core.ioctl_lock);
//	}else{
//		// this used tvout mode 
//		disp_dev->fb_blank = FB_BLANK_POWERDOWN;
//		disp_dev->state = 0;
//		asoc_display_update_status(disp_dev);		
//	}

}

/* tell userspace to start drawing */
static void de_late_resume(struct early_suspend *h)
{
	struct disp_manager *disp_mgr = &core.disp_mgr;	
	struct clk * de_clk = clk_get_sys(CLK_NAME_DE1_CLK, NULL);
	struct layer_manager * layer_mgr1 = &core.layer_mgr[0];
	struct asoc_display_device * disp_dev = layer_mgr1->disp_dev;	
    struct layer_manager * layer_mgr2 = &core.layer_mgr[1];
	struct asoc_display_device * disp_dev_hdmi = layer_mgr2->disp_dev;   
    mutex_lock(&core.ioctl_lock);
	//printk("de_late_resume called ~act_readl(PATH2_EN) = %d~~~~~~~~~~~~~~act_readl(PATH1_EN) = %d~~~~~~ \n",act_readl(PATH2_EN),act_readl(PATH1_EN));
	if(((act_readl(PATH2_EN) | act_readl(PATH1_EN)) == 0) || (act_readl(VIDEO1_SCOEF0) == 0)){		 	  
  		disp_mgr->disp_mode = DE_SINGLE_DISP;		
  		//disp_mgr->current_disp_ids = LCD_DISPLAYER; 
		de_reg_resume(&core.de_reg_bak);			
		disp_dev->state = 2;		
		asoc_display_update_status(disp_dev);			
		de_setout_channel(&core);
		disp_dev->state = 3;		 
		asoc_display_update_status(disp_dev);
		DEDBG("%s, current_disp_ids:%x\n", __func__, disp_mgr->current_disp_ids);
		//when late resume, switch the display 
		if(disp_mgr->current_disp_ids != LCD_DISPLAYER) {
			disp_dev_hdmi->state = 0;
			disp_manager_switch_displayer(&core, disp_mgr->current_disp_ids);           
        }else{
        	if(disp_dev_hdmi->state != 0){
        		disp_dev_hdmi->state = 0;
        		asoc_display_update_status(disp_dev_hdmi); 
        	}
        }		
	}else{
		int disp_dev_id = 0;
		if(act_readl(PATH2_EN) != 0){
			disp_dev_id |= LCD_DISPLAYER;
		}
		if(act_readl(PATH1_EN) != 0){
			disp_dev_id |= HDMI_DISPLAYER;
		}
		if(disp_dev_id == LCD_DISPLAYER){
			 disp_mgr->current_disp_ids = LCD_DISPLAYER;	
			 disp_dev->state = 4;		
		   asoc_display_update_status(disp_dev);	
		  
		}else{
		  struct layer_manager * layer_mgr1 = &core.layer_mgr[0];
	    struct layer_manager * layer_mgr2 = &core.layer_mgr[1];
			struct asoc_display_device * disp_dev_hdmi = layer_mgr2->disp_dev;	
			disp_mgr->current_disp_ids = disp_dev_id;
			disp_dev->state = 4;					
		  asoc_display_update_status(disp_dev);		
		  
	    disp_mgr->disp_mode = DE_DOUBLE_DISP;
	    layer_mgr1->path_id = 1;
	    layer_mgr2->path_id = 0; 
	    if(!disp_mgr->is_need_rot_for_hdmi){
	    	layer_mgr2->clone_form_main_manager(layer_mgr1,layer_mgr2);    	
	    	layer_mgr1->link_mgr = layer_mgr2;
	  	}
	    layer_mgr1->is_enable = 1;
	    layer_mgr2->is_enable = 1;
	    layer_mgr2->disp_dev->state = 1;
	    disp_mgr->current_disp_num = 2;
  	}
	}
	mutex_lock(&core.op_lock);
	core.v_vsync.state  = 1;	
	mutex_unlock(&core.op_lock);
	suspend_called = false;
	if(core.v_vsync.vsync_type == VSYNC_USING_UEVENT){
		vsync_kthread_task = kthread_run(vsync_kthread, NULL, "vsync_kthread");
	}
	early_suspend_called = false;	
    mutex_unlock(&core.ioctl_lock);   
	return;
}
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
static struct early_suspend de_early_suspend_desc = {
	.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN,
	.suspend = de_early_suspend,
	.resume = de_late_resume,
};
#endif

static int act_de_remove(struct platform_device *pdev)
{
	return 0;
}
#if CONFIG_PM
static int act_de_suspend(struct platform_device *pdev, pm_message_t state)
{
	//struct disp_manager *disp_mgr = &core.disp_mgr;	
	//disp_mgr->current_disp_ids = HDMI_DISPLAYER | LCD_DISPLAYER;
	struct layer_manager * layer_mgr2 = &core.layer_mgr[1];
	struct asoc_display_device * disp_dev_hdmi = layer_mgr2->disp_dev;
	DEDBG("***************enter %s,disp_dev_hdmi->state = %d****************\n", __func__, disp_dev_hdmi->state);
	if (disp_dev_hdmi->state)
		disp_dev_hdmi->state = 0;
	early_suspend_called = false;
	return 0;
}

static int act_de_resume(struct platform_device *pdev)
{
	return 0;
}
#endif

/* BUS */
static int de_bus_match(struct device *dev, struct device_driver *driver)
{
	struct asoc_display_device *asoc_dev = to_asoc_display_device(dev);
		
	DEDBG("bus_match. dev %s/%s, drv %s\n",
			dev_name(dev), asoc_dev->driver_name, driver->name);

	return strcmp(asoc_dev->driver_name, driver->name) == 0;

}
static ssize_t device_name_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *asoc_dev = to_asoc_display_device(dev);
	return snprintf(buf, PAGE_SIZE, "%s\n",
			asoc_dev->name ?
			asoc_dev->name : "");
}
static ssize_t device_modes_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int offset = 0;
	int i = 0;
	struct asoc_display_device *asoc_dev = to_asoc_display_device(dev);
	if(asoc_dev->modes != NULL){
		for(i = 0 ; i < asoc_dev->num_modes; i++){
				offset += snprintf(&buf[offset], PAGE_SIZE - offset, "%dx%d\n",asoc_dev->modes[i].xres,asoc_dev->modes[i].yres);
		}
	}
	return offset;
}
static ssize_t device_current_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *asoc_dev = to_asoc_display_device(dev);
	if(asoc_dev->disp_cur_mode != NULL){
		return snprintf(buf, PAGE_SIZE, "%dx%d\n",asoc_dev->disp_cur_mode->xres,asoc_dev->disp_cur_mode->yres);
	}
	return 0;
}

static struct device_attribute default_dev_attrs[] = {
	__ATTR(name, S_IRUGO, device_name_show, NULL),
	__ATTR(modes, S_IRUGO, device_modes_show, NULL),
	__ATTR(current_mode, S_IRUGO, device_current_mode_show, NULL),	
	__ATTR_NULL,
};

static ssize_t driver_name_show(struct device_driver *drv, char *buf)
{
  struct asoc_display_driver *asoc_drv = to_asoc_display_driver(drv);
	return snprintf(buf, PAGE_SIZE, "%s\n",
			asoc_drv->driver.name ?
			asoc_drv->driver.name : "");
}
static struct driver_attribute default_drv_attrs[] = {
	__ATTR(name, S_IRUGO, driver_name_show, NULL),
	__ATTR_NULL,
};


static struct bus_type de_bus_type = {
	
	.name = "act_de",
	.match = de_bus_match,
	.dev_attrs = default_dev_attrs,
	.drv_attrs = default_drv_attrs,
	
};
static void de_bus_release(struct device *dev)
{
	DEDBG("bus_release\n");
}
static struct device de_bus = {
	.release = de_bus_release,
};


static struct platform_device act_de_devices = {
	.name = "act_de",
	.id = 1,
};


/* DRIVER */
static struct platform_driver act_de_driver = { 
	  .probe = act_de_probe,
		.remove = act_de_remove,
#ifdef CONFIG_PM
		.suspend = act_de_suspend,
		.resume = act_de_resume,
#endif
		.driver = { 
			.name = "act_de", 
			.owner = THIS_MODULE, 
		}, 
};

struct bus_type *de_get_bus(void)
{
	return &de_bus_type;
}

static int asoc_display_driver_probe(struct device *dev)
{
	int r;
	
	struct asoc_display_driver *asoc_drv = to_asoc_display_driver(dev->driver);
	struct asoc_display_device *asoc_dev = to_asoc_display_device(dev);

	
	DEDBG("driver_probe: dev %s/%s, drv %s asoc_dev %p\n",
				dev_name(dev), asoc_dev->driver_name,
				asoc_drv->driver.name, asoc_dev);
				
  if(asoc_drv->probe){
		r = asoc_drv->probe(asoc_dev);
		if (r) {
			printk("driver probe failed: %d\n", r);
			return r;
		}
	}

	DEDBG("probe done for device %s\n", dev_name(dev));

	asoc_dev->driver = asoc_drv;

	return 0;
}

static int asoc_display_driver_remove(struct device *dev)
{
	struct asoc_display_driver *asoc_drv = to_asoc_display_driver(dev->driver);
	struct asoc_display_device *asoc_dev = to_asoc_display_device(dev);

	printk("driver_remove: dev %s/%s\n", dev_name(dev),asoc_dev->driver_name);

	asoc_drv->remove(asoc_dev);

	asoc_dev->driver = NULL;

	return 0;
}

int asoc_display_driver_register(struct asoc_display_driver *asoc_driver)
{
	asoc_driver->driver.bus = &de_bus_type;
	asoc_driver->driver.probe = asoc_display_driver_probe;
	asoc_driver->driver.remove = asoc_display_driver_remove;

	return driver_register(&asoc_driver->driver);
}

EXPORT_SYMBOL(asoc_display_driver_register);

void asoc_display_driver_unregister(struct asoc_display_driver * asoc_driver)
{
	driver_unregister(&asoc_driver->driver);
}

EXPORT_SYMBOL(asoc_display_driver_unregister);

/* DEVICE */
static void de_dev_release(struct device *dev)
{
	struct asoc_display_device *asoc_dev = to_asoc_display_device(dev);
	printk("de_dev_release called ,release dev %s/%s \n", dev_name(dev),asoc_dev->driver_name);
}

int asoc_display_device_register(struct asoc_display_device *asoc_dev)
{
	static int dev_num;
 
	WARN_ON(!asoc_dev->driver_name);
		
	asoc_dev->dev.bus = &de_bus_type;
	asoc_dev->dev.parent = &de_bus;
	asoc_dev->dev.release = de_dev_release;
	dev_set_name(&asoc_dev->dev, "display%d", dev_num++);
	mutex_init(&asoc_dev->lock);
	return device_register(&asoc_dev->dev);
}
EXPORT_SYMBOL(asoc_display_device_register);
void asoc_display_device_unregister(struct asoc_display_device * asoc_dev)
{
	device_unregister(&asoc_dev->dev);
}
EXPORT_SYMBOL(asoc_display_device_unregister);
/* BUS REGISTER */
BUS_ATTR(system_info, 0644, system_info_show,	NULL);
					
static int de_bus_register(void)
{
	int r;
		
	r = bus_register(&de_bus_type);
	
	if (r) {
		printk(KERN_ERR"bus register failed\n");
		return r;
	}
		
	dev_set_name(&de_bus, "act_de");
	
	r = device_register(&de_bus);
	
	if (r) {
		printk(KERN_ERR"bus driver register failed\n");
		bus_unregister(&de_bus_type);
		return r;
	}
	
  r = bus_create_file(&de_bus_type, &bus_attr_system_info);
	if (r)
		return r;
	
	return 0;
}

static void de_bus_unregister(void)
{
	device_unregister(&de_bus);

	bus_unregister(&de_bus_type);
}			

static int __init act_de_init(void)
{	
	int r;

	/*register bus for de */
	DEDBG("de_bus_register !\n");
	r = de_bus_register();
	
	if (r)
		return r;

			
	r = register_chrdev(DE_MAJOR,"display",&de_fops);
	if (r) {
		printk("unable to get major %d for de devs\n", DE_MAJOR);
		return r;
	}
	
			
	de_class = class_create(THIS_MODULE, "display");
	
	if (IS_ERR(de_class))
		return PTR_ERR(de_class);		
		
	
	
	/*register device for de*/
	DEDBG("platform_device_register_simple  act_de_devices !\n");	
	r = platform_device_register(&act_de_devices);
	
	if(r)
	{
		printk(KERN_ERR "failed to register act_de device\n");
		r = PTR_ERR(&act_de_devices);
		return r;
	}
	
	core.dev = device_create(de_class, &act_de_devices.dev, MKDEV(DE_MAJOR, 0), &core, "de");

	
	r = de_vsync_create_attrs(core.dev);
			
	if(r){
		printk(KERN_ERR "failed to create de vsync attrs \n");
		r = PTR_ERR(&act_de_devices);
		return r;
	}
	
	/*register driver for de */
	DEDBG("register driver!\n");
	r = platform_driver_register(&act_de_driver);
	
	if (r) {
		/*register failed */
		de_bus_unregister();
		return r;
	}
	platform_set_drvdata(&act_de_devices,&core);
#ifdef CONFIG_HAS_EARLYSUSPEND
	register_early_suspend(&de_early_suspend_desc);
#endif

	return 0;
}

static void __exit act_de_exit(void)
{

  /* unregister de platform driver */
	platform_driver_unregister(&act_de_driver);
	class_destroy(de_class);
	
	/*de buf unregister */
	de_bus_unregister();
	
#ifdef CONFIG_HAS_EARLYSUSPEND
  /*if config for early suspen , unregister it */	
	unregister_early_suspend(&de_early_suspend_desc);
#endif
}

module_init( act_de_init);
module_exit( act_de_exit);

MODULE_AUTHOR("Actions_semi,wh <wanghui@actions-semi.com>");
MODULE_DESCRIPTION("Actions DE");
MODULE_LICENSE("GPL");
