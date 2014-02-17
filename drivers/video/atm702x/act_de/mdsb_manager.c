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
#include "display.h"

static bool first_time = true;

int mdsb_init(struct mdsb_manager * mgr) {
	mgr->cvbs_vr = 0;
	mgr->lcd_vr = 0;
	mgr->cvbs_hr = 0;
	mgr->lcd_hr = 0;
	mgr->delay0 = 0;
	mgr->delay1 = 0;
	
// SD提供的原始参数  该参数错误
//	act_writel(0x00004000, MDSB_HSCALER0);
//	act_writel(0xFF083DFA, MDSB_HSCALER1);
//	act_writel(0xFC1338F7, MDSB_HSCALER2);
//	act_writel(0xF91E31F7, MDSB_HSCALER3);
//	act_writel(0xF82828F8, MDSB_HSCALER4);
//	act_writel(0xF7311EF9, MDSB_HSCALER5);
//	act_writel(0xF73813FC, MDSB_HSCALER6);
//	act_writel(0xFA3D08FE, MDSB_HSCALER7);
	
/* 使用两抽头参数
	act_writel(0x00004000, MDSB_HSCALER0);
	act_writel(0x00380800, MDSB_HSCALER1);
	act_writel(0x00301000, MDSB_HSCALER2);
	act_writel(0x00281800, MDSB_HSCALER3);
	act_writel(0x00202000, MDSB_HSCALER4);
	act_writel(0x00182800, MDSB_HSCALER5);
	act_writel(0x00103000, MDSB_HSCALER6);
	act_writel(0x00083800, MDSB_HSCALER7);
*/  
  	act_writel(0X00004000, MDSB_HSCALER0);
	act_writel(0XFF093EFA, MDSB_HSCALER1);
	act_writel(0XFD1339F7, MDSB_HSCALER2);
	act_writel(0XFA1E31F7, MDSB_HSCALER3);
	act_writel(0XF82828F8, MDSB_HSCALER4);
	act_writel(0XF7311EFA, MDSB_HSCALER5);
	act_writel(0XF73913FD, MDSB_HSCALER6);
	act_writel(0XFA3E09FF, MDSB_HSCALER7);

	act_writel(0x00000040, MDSB_VSCALER0);
	act_writel(0x00000838, MDSB_VSCALER1);
	act_writel(0x00001030, MDSB_VSCALER2);
	act_writel(0x00001828, MDSB_VSCALER3);
	act_writel(0x00002020, MDSB_VSCALER4);
	act_writel(0x00002818, MDSB_VSCALER5);
	act_writel(0x00003010, MDSB_VSCALER6);
	act_writel(0x00003808, MDSB_VSCALER7);

	act_writel(0, MDSB_LCD_VR);
	act_writel(0, MDSB_CVBS_VR);
	act_writel(0, MDSB_DELAY0);
	act_writel(0, MDSB_DELAY1);
	act_writel(0, MDSB_LCD_HR);
	act_writel(0, MDSB_CVBS_HR);
	
	if (first_time) {			
  /*	lcd_vr = 0;
		lcd_hr = 0;
		lcd_drop = 0;
		cvbs_vr = 0;
		cvbs_hr = 0;
		cvbs_drop = 0;
		delay = 0;	*/
		first_time = false;
	}
	return 0;
}

int mdsb_config(struct de_core *core) {
	struct mdsb_manager * mgr = &core->mdsb_mgr;
	struct disp_manager * disp_mgr = &core->disp_mgr;		
	struct asoc_display_device **disp_dev = disp_mgr->disp_dev;
	int disp_mode = TV_MODE_YPBPR_1920x1080p_50Hz;
	int displayer_id = disp_mgr->current_disp_ids;	
	int cvbs_index = 0;

	DEDBG("mdsb config !\n");

	if (displayer_id & HDMI_DISPLAYER) {
		disp_mode = disp_mgr->current_disp_dev->disp_cur_mode->vid;
	} else if (displayer_id & TV_YPbPr_DISPLAYER) {
		disp_mode = disp_mgr->current_disp_dev->disp_cur_mode->vid;
	} else {
		disp_mode = TV_MODE_YPBPR_1920x1080p_50Hz;	
	}
	if ((disp_mode == TV_MODE_HDMI_1920x1080p_60Hz)
		  || (disp_mode == TV_MODE_YPBPR_1920x1080p_60Hz)) {
		printk("TV_MODE_HDMI_1920x1080p_60Hz !\n");
		disp_mode = TV_1080P_60HZ;
		mgr->master_disp_w = 1920;
		mgr->master_disp_h = 1080;
	} else if ((disp_mode == TV_MODE_HDMI_1920x1080p_50Hz) 
		|| (disp_mode	== TV_MODE_YPBPR_1920x1080p_50Hz)) {
		printk("TV_MODE_HDMI_1920x1080p_50Hz !\n");
		disp_mode = TV_1080P_50HZ;
		mgr->master_disp_w = 1920;
		mgr->master_disp_h = 1080;
	} else if ((disp_mode == TV_MODE_HDMI_1280x720p_60Hz) 
		|| (disp_mode	== TV_MODE_YPBPR_1280x720p_60Hz)) {
					printk("TV_MODE_HDMI_1280x720p_60Hz !\n");
		disp_mode = TV_720P_60HZ;
		mgr->master_disp_w = 1280;
		mgr->master_disp_h = 720;
	} else if ((disp_mode == TV_MODE_HDMI_1280x720p_50Hz) 
		|| (disp_mode	== TV_MODE_YPBPR_1280x720p_50Hz)) {
		printk("TV_MODE_HDMI_1280x720p_50Hz !\n");
		disp_mode = TV_720P_50HZ;
		mgr->master_disp_w = 1280;
		mgr->master_disp_h = 720;		
	} else {
		printk("not support this sync display disp_mode = %d!\n", disp_mode);
	}		
	if (displayer_id & LCD_DISPLAYER) {
		if (disp_dev[0]->driver->get_mdsb_parameter != NULL) {
			if (mgr->debug_mode == 0) {
				disp_dev[0]->driver->get_mdsb_parameter(disp_dev[0], disp_mode, &mgr->lcd_hr, &mgr->lcd_vr, &mgr->lcd_drop, &mgr->delay1);
				printk("LCD MDBS PARAMER : mgr->lcd_hr = %x mgr->lcd_vr = %x, mgr->lcd_drop %x mgr->delay1 %x!\n", mgr->lcd_hr, mgr->lcd_vr, mgr->lcd_drop, mgr->delay1);
			} else {
				// used config info 
			 printk("used config info  mgr->lcd_hr = %x mgr->lcd_vr = %x, mgr->lcd_drop %x mgr->delay1 %x!\n", mgr->lcd_hr, mgr->lcd_vr, mgr->lcd_drop, mgr->delay1);
	  		}
		}
		cvbs_index++;
	}
	
	if (displayer_id & TV_CVBS_DISPLAYER) {
		if (disp_dev[cvbs_index]->driver->get_mdsb_parameter != NULL) {
			disp_dev[cvbs_index]->driver->get_mdsb_parameter(disp_dev[cvbs_index],disp_mode,&mgr->cvbs_hr,&mgr->cvbs_vr,&mgr->cvbs_drop,&mgr->delay0);
		}
		mgr->delay0 = mgr->delay0 * 180 /150;
		printk("CVBS MDBS PARAMER : mgr->cvbs_hr = %x mgr->cvbs_vr = %x , mgr->cvbs_drop %x mgr->delay0 %x!\n",mgr->cvbs_hr,mgr->cvbs_vr,mgr->cvbs_drop,mgr->delay0);
	}	
	if (mgr->delay0 == 0) {
		mgr->delay0 = mgr->delay1;
		mgr->delay1 = 0;
	}
	return 0;
}

int mdsb_update(struct de_core *core) {
	int tmp = 0;
	struct mdsb_manager * mgr = &core->mdsb_mgr;
	struct disp_manager * disp_mgr = &core->disp_mgr;
	int disp_id = disp_mgr->current_disp_ids;

	DEDBG("mdsb congfig update !\n");
	//CVBS 配置
	if ((disp_id & TV_CVBS_DISPLAYER) == TV_CVBS_DISPLAYER) {
		act_writel(mgr->cvbs_hr, MDSB_CVBS_HR);
		act_writel(mgr->cvbs_vr, MDSB_CVBS_VR);
		act_writel(mgr->cvbs_drop, MDSB_CVBS_OFF);
	}
	
	//LCD 配置
	if ((disp_id & LCD_DISPLAYER) == LCD_DISPLAYER) {
		act_writel(mgr->lcd_hr, MDSB_LCD_HR);
		act_writel(mgr->lcd_vr, MDSB_LCD_VR);
		act_writel(mgr->lcd_drop, MDSB_LCD_OFF);
	}
	// YPbPr -> CVBS -> LCD
	if ((disp_id & TV_CVBS_DISPLAYER) == TV_CVBS_DISPLAYER) {
		//lcd+cvb+hdmi :0x21
		tmp = (2U << 4) | (1U << 0);
	} else {
		//lcd+hdmi/ypbpr 0x18
		tmp = (1U << 4) | (1U << 3);
	}
	act_writel(tmp, MDSB_STSEQ);
	act_writel((unsigned int) mgr->delay0, MDSB_DELAY0);
	act_readl(MDSB_DELAY0);

	if (mgr->delay1 != 0) {
		act_writel((unsigned int) mgr->delay1, MDSB_DELAY1);
		act_readl(MDSB_DELAY1);
	}
	act_writel(((mgr->master_disp_h << 16) | mgr->master_disp_w), MDSB_HDMIWH);

	return 0;
}

int mdsb_manager_init(struct de_core *core) {
	struct mdsb_manager * mgr = &core->mdsb_mgr;
	mgr->init = mdsb_init;
	mgr->config_mdsb = mdsb_config;
	mgr->update_mdsb = mdsb_update;
	mgr->init(mgr);
	mdsb_mgr_attr_init(mgr,core);
	return 0;
}
