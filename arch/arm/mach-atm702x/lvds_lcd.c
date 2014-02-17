/***************************************************************************
 *                              GLBASE
 *                            Module: rgb lcd driver
 *                 Copyright(c) 2011-2015 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       wanghui     2011-08-19 9:00     1.0             build this file
 ***************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <mach/gl5202_cmu.h>
#include "../act_lcdc.h"
#include "../act_lcdc_config.h"

int lvds_lcd_init(struct asoc_display_device * disp_dev)
{
	unsigned int tmp = 0;
	struct lcd_manager *lcd_mgr = disp_dev_get_drvdata(disp_dev);
	struct de_video_mode * cur_mode = disp_dev->disp_cur_mode;	
	int lcd_id =  disp_dev->display_id;
	u32 data_width = lcd_mgr->lcd_info[0].data_width;
	u32 vsync_inversion = lcd_mgr->lcd_info[0].vsync_inversion;
	u32 hsync_inversion = lcd_mgr->lcd_info[0].hsync_inversion;
	u32 dclk_inversion = lcd_mgr->lcd_info[0].dclk_inversion;
	u32 lde_inversion= lcd_mgr->lcd_info[0].lde_inversion;
	struct lcd_lvds_info* plvds_info = &(lcd_mgr->lcd_info[0].lvds_info);
	
	int LCD_REG_BASE = 0;
	int hmdi_clock_state = 0;
	if(lcd_id == LCD_DISPLAYER){
		 LCD_REG_BASE =  LCD0_BASE;
	}else{
		 LCD_REG_BASE = LCD1_BASE;
	}
	
	printk("open lcd,lcd_id = %d, xres = %d yres =%d, hbp =%d ,hfp = %d, hspw = %d, vbp =%d, vfp = %d, vspw = %d \n", 
	lcd_id, cur_mode->xres,cur_mode->yres,cur_mode->hbp,cur_mode->hfp,cur_mode->hsw,cur_mode->vbp,cur_mode->vfp,cur_mode->vsw);

 	//act_writel((act_readl(CMU_DEVCLKEN0) | (3 << 9)), CMU_DEVCLKEN0);
 	hmdi_clock_state = act_readl(CMU_DEVCLKEN1) & (1 << 3);
 	if(hmdi_clock_state == 0){
 		act_writel((act_readl(CMU_DEVCLKEN1) | (1 << 3)), CMU_DEVCLKEN1); 		
 	}
 	act_writel((act_readl(HDMI_TX_1) | (1 << 23)),HDMI_TX_1); 
	if(hmdi_clock_state == 0){
 		act_writel((act_readl(CMU_DEVCLKEN1) & (~(1 << 3))), CMU_DEVCLKEN1); 		
 	}
 	//switch mfp from lcd to lvds
 	act_writel(act_readl(MFP_CTL1) & (~(0x0600000)), MFP_CTL1);
 		
	/*set size*/
	tmp = act_readl(LCD_REG_BASE + LCD_OFF_SIZE);
	tmp = tmp&(~(0x7ff<<16))&(~(0x7ff));
	tmp = tmp |((cur_mode->yres - 1)<<16)|(cur_mode->xres - 1);
	act_writel(tmp, LCD_REG_BASE + LCD_OFF_SIZE);

	/*set timeing0*/
	tmp = act_readl(LCD_REG_BASE + LCD_OFF_TIM0);
	tmp =tmp &(~(0xf<<4));
	tmp = tmp|(vsync_inversion << 7)
		|(hsync_inversion << 6)
		|(dclk_inversion << 5)
		|(lde_inversion << 4);
		
	tmp &= (~LCD_RGBTIMING0_PREL_CNT_MASK);
		
	tmp |= LCD_RGBTIMING0_PREL_CNT(8);
	
	tmp |= LCD_RGBTIMING0_PREL_EN;
	
	act_writel(tmp, LCD_REG_BASE + LCD_OFF_TIM0);

	/*set timeing1*/
	tmp = (cur_mode->hbp -1) | ((cur_mode->hfp -1) << 10) | ((cur_mode->hsw -1) << 20);
	
	act_writel(tmp, LCD_REG_BASE + LCD_OFF_TIM1);

	/*set timeing2*/
	tmp = (cur_mode->vbp -1) | ((cur_mode->vfp -1) << 10) | ((cur_mode->vsw -1) << 20);
	
	act_writel(tmp, LCD_REG_BASE + LCD_OFF_TIM2);

	act_writel(DEFAULT_CLR, LCD_REG_BASE + LCD_OFF_COLOR);

	/*set lcd ctl*/
	tmp = act_readl(LCD_REG_BASE + LCD_OFF_CTL);
	
	tmp = tmp&(~(0x1<<31))&(~(0x7<<16))&(~(0x3<<6));
	
	tmp = tmp|(data_width << 16) | (0x1 << 6) | 0x1; //rgb if, data width, video from de,enable lcdc
	
	act_writel(tmp, LCD_REG_BASE + LCD_OFF_CTL);
	
	
	if(plvds_info->lvds_ctl_reg_value != 0) {
		act_writel(plvds_info->lvds_ctl_reg_value, LVDS_CTL);
	}else {
		act_writel(0x000a9521 | ( (data_width == 0) << 1) | (plvds_info->mapping << 3) | (plvds_info->mirror << 6) , LVDS_CTL);
	}
	
	if(plvds_info->lvds_alg_reg_value != 0) {
		act_writel(plvds_info->lvds_alg_reg_value, LVDS_ALG_CTL0);
	}else {
		act_writel(0xc141a120 | (plvds_info->odd_polarity << 2) | (plvds_info->even_polarity << 3), LVDS_ALG_CTL0);
	}
	
    return 0;
}

int lvds_lcd_close(struct asoc_display_device * disp_dev)
{
	int tmp;
	int LCD_REG_BASE = 0;
	int lcd_id =  disp_dev->display_id;

	if(lcd_id == LCD_DISPLAYER){
		 LCD_REG_BASE =  LCD0_BASE;
	}else{
		 LCD_REG_BASE = LCD1_BASE;
	}
	
	printk("close lcd,lcd_id=%d\n", lcd_id);
	//when close lcd, first disable lvds
	act_writel((act_readl(LVDS_CTL) & (~0x01)), LVDS_CTL);
	
	//switch mfp from lvds to LCD1_BASE
	act_writel(act_readl(MFP_CTL1) | (0x0600000), MFP_CTL1);

	/***********pull down vsync /hsync / dclk pads**************************/
	act_writel(0xc0, LCD_REG_BASE + LCD_OFF_TIM0);		/*pull down vsync/hsync*/
	/******************************************************************/  
  
	tmp = act_readl(LCD_REG_BASE + LCD_OFF_CTL);
	
	tmp &= (~0x01);
	
	act_writel(tmp, LCD_REG_BASE + LCD_OFF_CTL);	
 	 	
	return 0;
}

int lvds_lcd_get_pin(struct asoc_display_device * disp_dev)
{
	struct lcd_manager *lcd_mgr = disp_dev_get_drvdata(disp_dev);
	
    act_writel(act_readl(PAD_CTL) | 0x02,PAD_CTL);
    
    set_lcd_gpio_active(lcd_mgr->lcd_info[0].gpio_info.lcd_vcc_gpio, 1);

	return 0;
}

int lvds_lcd_release_pin(struct asoc_display_device * disp_dev){
	struct lcd_manager *lcd_mgr = disp_dev_get_drvdata(disp_dev);
	
	set_lcd_gpio_active(lcd_mgr->lcd_info[0].gpio_info.lcd_vcc_gpio, 0);
	free_lcd_gpio(lcd_mgr->lcd_info[0].gpio_info.lcd_vcc_gpio);
    
	return 0;
}

int lvds_lcd_get_mdsb_parameter(struct asoc_display_device *disp_dev,
								int disp_mode ,int * hr , int * vr , int * drop, int * delay)
{
	  struct lcd_manager *lcd_mgr = disp_dev_get_drvdata(disp_dev);
	  int lcd_id =  disp_dev->display_id;
    int cur_index = 0;
	 // u32 sclk = asoc_get_dev_sclk()/1000000;	  
    u32 sclk = 150000000;	  
	  switch(disp_mode){
	  	case TV_1080P_60HZ:
	  		cur_index = 1;	  		
	  		break;
	  	case TV_1080P_50HZ:
	  		cur_index = 2;
	  		break;
	  	case TV_720P_60HZ:
	  		cur_index = 3;
	 		break;
	  	case TV_720P_50HZ:
	  		cur_index = 4;
	  		break;
	  	default:
	  		printk("lcd not support this mod\n");
	  	  return -1;
	  }	
	  printk("disp_mode = %x,lcd_id %d \n",disp_mode,lcd_id);  
	  disp_dev->disp_cur_mode = &lcd_mgr->lcd_info[0].modes[cur_index];	  
	  *hr = lcd_mgr->lcd_info[0].sync_info[cur_index - 1].hr;
	  *vr = lcd_mgr->lcd_info[0].sync_info[cur_index - 1].vr;
	  *drop = lcd_mgr->lcd_info[0].sync_info[cur_index - 1].drop;
	  *delay = lcd_mgr->lcd_info[0].sync_info[cur_index - 1].delay * sclk / 150 ;//209;334
	  lcd_mgr->lcd_clk = 1000000000 / disp_dev->disp_cur_mode->pixclock;
	  return 0;	
}

int lcd_manager_init(struct lcd_manager * lcd_mgr){
							 
	int j = 0 ,i = 0;
	//don't change this number
	lcd_mgr->lcd_num = 1;	
	lcd_mgr->lcd_type = LCD_TYPE_LVDS;
		
	for(j = 0 ; j < lcd_mgr->lcd_num ; j ++){
		struct de_video_mode * modes = lcd_mgr->lcd_info[j].modes;
	    struct lcd_mdsb_info  * sync_info = lcd_mgr->lcd_info[j].sync_info;
	    //don't change this number
		lcd_mgr->lcd_info[j].num_modes  = 1;		
		for(i = 0 ; i < lcd_mgr->lcd_info[0].num_modes ; i++){
			 modes[i].refresh = get_lcd_config(LCD_BASE_INFO, BASE_FRAME_RATE, 60);
			 modes[i].xres = get_lcd_config(LCD_BASE_INFO, BASE_PIXEL_WIDTH, 1280);
			 modes[i].yres = get_lcd_config(LCD_BASE_INFO, BASE_PIXEL_HEIGHT, 800);
			 modes[i].hfp = get_lcd_config(LCD_BASE_INFO, BASE_HSYNC_FRONT_PORCH, 20);
			 modes[i].hbp = get_lcd_config(LCD_BASE_INFO, BASE_HSYNC_BACK_PORCH, 40);
			 modes[i].vfp = get_lcd_config(LCD_BASE_INFO, BASE_VSYNC_FRONT_PORCH, 10);
			 modes[i].vbp = get_lcd_config(LCD_BASE_INFO, BASE_VSYNC_BACK_PORCH, 33);
			 modes[i].hsw = get_lcd_config(LCD_BASE_INFO, BASE_HSYNC_PULSE_WIDTH, 12);
			 modes[i].vsw = get_lcd_config(LCD_BASE_INFO, BASE_VSYNC_PULSE_WIDTH, 2);
			 modes[i].pixclock = 1000000000 / get_lcd_config(LCD_BASE_INFO, BASE_CLOCK_RATE, 60000);
			 modes[i].vmode	= FB_VMODE_NONINTERLACED;
			 modes[i].flag	= FB_MODE_IS_STANDARD;
			 modes[i].vid = i;			 
			 if(i >= 1){
				 	sync_info[i - 1].hr = 0;
				 	sync_info[i - 1].vr = 0;
				 	sync_info[i - 1].drop = 0;
				 	sync_info[i - 1].delay = 0;
			 }
		}
				
		lcd_mgr->lcd_info[j].data_width = get_lcd_config(LCD_BASE_INFO, BASE_DATA_WIDTH, 0);	
		lcd_mgr->lcd_info[j].vsync_inversion = get_lcd_config(LCD_SIGNAL_INFO, SIGNAL_VSYNC_HIGH_ACTIVE, 0);
		lcd_mgr->lcd_info[j].hsync_inversion = get_lcd_config(LCD_SIGNAL_INFO, SIGNAL_HSYNC_HIGH_ACTIVE, 0);
		lcd_mgr->lcd_info[j].dclk_inversion = get_lcd_config(LCD_SIGNAL_INFO, SIGNAL_DCLK_RISING_ACTIVE, 0);
		lcd_mgr->lcd_info[j].lde_inversion =  get_lcd_config(LCD_SIGNAL_INFO, SIGNAL_DATA_LOW_ACTIVE, 0);
		
		lcd_mgr->lcd_info[j].gpio_info.pwm_gpio = get_lcd_gpio_cfg(GPIO_LCD_PWM);
		lcd_mgr->lcd_info[j].gpio_info.lcd_vcc_gpio = get_lcd_gpio_cfg(GPIO_LCD_VCC);
		lcd_mgr->lcd_info[j].gpio_info.led_en_gpio = get_lcd_gpio_cfg(GPIO_LCD_LED_EN);
		
		lcd_mgr->lcd_info[j].lvds_info.mapping = get_lcd_config(LCD_LVDS_INFO, LVDS_OUTPUT_MAPPING, 0);
		lcd_mgr->lcd_info[j].lvds_info.mirror = get_lcd_config(LCD_LVDS_INFO, LVDS_OUTPUT_MIRROR, 0);
		lcd_mgr->lcd_info[j].lvds_info.odd_polarity = get_lcd_config(LCD_LVDS_INFO, LVDS_ODD_POLARITY, 0);
		lcd_mgr->lcd_info[j].lvds_info.even_polarity = get_lcd_config(LCD_LVDS_INFO, LVDS_EVEN_POLARITY, 0);
		lcd_mgr->lcd_info[j].lvds_info.lvds_ctl_reg_value = get_lcd_config(LCD_LVDS_INFO, LVDS_CTL_REG_VALUE, 0);
		lcd_mgr->lcd_info[j].lvds_info.lvds_alg_reg_value = get_lcd_config(LCD_LVDS_INFO, LVDS_ALG_REG_VALUE, 0);
		lcd_mgr->lcd_clk = 1000000000 / modes[0].pixclock;
		lcd_mgr->src_clk_type = CLK_SRC_DISPLAY_PLL;
	}		
	lcd_mgr->lcd_init = lvds_lcd_init;
	lcd_mgr->lcd_close = lvds_lcd_close;
	lcd_mgr->get_pin = lvds_lcd_get_pin;
	lcd_mgr->release_pin = lvds_lcd_release_pin;
	lcd_mgr->get_mdsb_parameter = lvds_lcd_get_mdsb_parameter;
	
	if(lcd_mgr->lcd_info[0].lvds_info.lvds_alg_reg_value != 0) {
		act_writel(lcd_mgr->lcd_info[0].lvds_info.lvds_alg_reg_value, LVDS_ALG_CTL0);
	}else {
		act_writel(0xc141a120 | (lcd_mgr->lcd_info[0].lvds_info.odd_polarity << 2) | (lcd_mgr->lcd_info[0].lvds_info.even_polarity << 3), LVDS_ALG_CTL0);
	}
	
	return 0;
}