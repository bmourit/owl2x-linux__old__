/***************************************************************************
 *                              GLBASE
 *                            Module: lcdc driver
 *                 Copyright(c) 2011-2015 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       wanghui     2011-08-19 9:00     1.0             build this file
 ***************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/delay.h>
#include <linux/cpufreq.h>
#include <linux/gpio.h>
#include <linux/clk.h>
#include <mach/gl5202_gpio.h>
#include <mach/gl5202_cmu.h>
#include <linux/clk.h>
#include <mach/clock.h>
#include "act_lcdc.h"
#include "../display/act_de/display.h"
#include "act_lcdc_config.h"

static int s_open_flag = 0; 
#define MAX_GPIO_PER_GROUP 32
// printk information  contol
void print_debug_info_lcd(void)
{
#if 0
  LCD_DEBUG_PRINT("CMU_DEVCLKEN0 =%x\n",act_readl(CMU_DEVCLKEN0));
  LCD_DEBUG_PRINT("CMU_LCDCLK =%x\n",act_readl(CMU_LCDCLK));
  LCD_DEBUG_PRINT("CMU_DEVRST0 =%x\n",act_readl(CMU_DEVRST0));
	LCD_DEBUG_PRINT("LCD_CTRL0 =%x\n",act_readl(LCD_CTL));
	LCD_DEBUG_PRINT("LCD_SIZE =%x\n",act_readl(LCD_SIZE));
	LCD_DEBUG_PRINT("LCD_RGBTIMING0 =%x\n",act_readl(LCD_RGBTIMING0));
	LCD_DEBUG_PRINT("LCD_RGBTIMING1 =%x\n",act_readl(LCD_RGBTIMING1));
	LCD_DEBUG_PRINT("LCD_RGBTIMING2 =%x\n",act_readl(LCD_RGBTIMING2));
	LCD_DEBUG_PRINT("LCD_COLOR =%x\n",act_readl(LCD_COLOR));
	
	LCD_DEBUG_PRINT("GPIO_COUTEN =%x\n",act_readl(GPIO_COUTEN));
	LCD_DEBUG_PRINT("GPIO_CDAT =%x\n",act_readl(GPIO_CDAT));
	LCD_DEBUG_PRINT("GPIO_CINEN =%x\n",act_readl(GPIO_CINEN));
	
	LCD_DEBUG_PRINT("CMU_COREPLL = 0x%x\n",act_readl(CMU_COREPLL));
	LCD_DEBUG_PRINT("CMU_DEVCLKEN1 = 0x%x\n",act_readl(CMU_DEVCLKEN1));
	LCD_DEBUG_PRINT("CMU_PWM0CLK = 0x%x\n",act_readl(CMU_PWM0CLK));
	LCD_DEBUG_PRINT("PWM0_CTL = 0x%x\n",act_readl(PWM0_CTL));

#endif
}

void set_lcd_output_default_color(void) {
	unsigned int tmp  = act_readl(LCD0_CTL);
	act_writel(tmp | (0x3 << 6), LCD0_CTL);
}

void set_lcd_output_from_de(void) {
	unsigned int tmp  = act_readl(LCD0_CTL);
	act_writel((tmp & ~(0x3 << 6)) | (0x1 << 6), LCD0_CTL);
}

int is_lcd_in_open_status(void) {
	return (act_readl(LCD0_CTL) & 0x1);
}

static void connect_backlight_device(struct asoc_display_device *disp_dev)
{
	disp_dev->bl_dev = act_pwm_bl_connect();
}

static void disconnect_backlight_device(struct asoc_display_device *disp_dev)
{
	if(disp_dev->bl_dev == NULL)
		return ;
	 act_pwm_bl_disconnect(disp_dev->bl_dev);
	disp_dev->bl_dev = NULL;
}

static void set_bl_power(struct asoc_display_device *disp_dev, int power)
{
	if (disp_dev->bl_dev == NULL)
		return ;	
	if (power == FB_BLANK_UNBLANK) {
		set_lcd_output_from_de();
	}
	disp_dev->bl_dev->props.power = power;
	backlight_update_status(disp_dev->bl_dev);
}

int get_lcd_clk_divisor(int source_rate, int target_rate, int *lcd_div, int *lcd0_div) 
{
	int tmp = 0;
	if(source_rate == 0 || target_rate == 0) {
		return -1;
	}
	
	if(target_rate > source_rate) {
	    *lcd_div = 1;
	    *lcd0_div = 1;
	} else if(target_rate*7*12 < source_rate) {
        *lcd_div = 7;
	    *lcd0_div = 12;
	} else {
		int i, last_pos = 0, cur_pos;
		tmp = source_rate/target_rate;
		
		for(i = 1; i < 24; i++,last_pos=cur_pos) {
			cur_pos = i;
		    if(i > 12) {
			    cur_pos = (i - 11)*7;
		    }
		    if(tmp < cur_pos) {
		    	break;
		    }
		}

		if((source_rate*cur_pos + source_rate*last_pos) > 2*target_rate*cur_pos*last_pos) {
		    tmp = cur_pos;
		} else {
			tmp = last_pos;
		}
		*lcd_div = tmp > 12 ? 7 : 1;
	    *lcd0_div = tmp > 12 ? tmp/7 : tmp;	
	}
	return 0;
}

void set_lcdc_clk(struct asoc_display_device *disp_dev, u32 source_clk_type, unsigned int clk_rate)
{
	struct clk * parent_clk = NULL;
	int source_clk_rate = 0;
	int lcd_div, lcd0_div;
	struct clk * lcd_clk;
    struct clk * lcd0_clk;
	switch(source_clk_type){
		case CLK_SRC_DISPLAY_PLL:
			parent_clk = clk_get_sys(CLK_NAME_DISPLAYPLL, NULL);			
			break;
		case CLK_SRC_DEVSCLK:
			parent_clk = clk_get_sys(CLK_NAME_DEVCLK, NULL);
			break;
		case CLK_SRC_TVOUT_PLL0:
			parent_clk = clk_get_sys(CLK_NAME_TVOUT0PLL, NULL);
			break;
		default:
			printk("source_clk_type not support \n");
			return;
	}
	if(parent_clk != NULL){
		source_clk_rate = clk_get_rate(parent_clk) / 1000; 
	}else{
		printk("parent_clk not found \n");
		return;
	}
	lcd_clk = clk_get_sys(CLK_NAME_LCD_CLK, NULL);
	clk_set_parent(lcd_clk,parent_clk);
	clk_enable(lcd_clk);  /*enable clk*/	
	lcd0_clk	= clk_get_sys(CLK_NAME_LCD0_CLK, NULL);
	clk_enable(lcd0_clk);  /*enable clk*/
	if(get_lcd_clk_divisor(source_clk_rate, clk_rate, &lcd_div, &lcd0_div) >= 0) {
		clk_set_rate(lcd_clk, (source_clk_rate / lcd_div  + 100)* 1000);
		clk_set_rate(lcd0_clk, (source_clk_rate / lcd_div / lcd0_div + 100)* 1000); 		
	} else {
		printk("clk rate not support source_clk_rate = %d clk_rate = %d\n",source_clk_rate, clk_rate);
	}	
}

void open_lcd(struct asoc_display_device *disp_dev)
{
	struct lcd_manager * lcd_mgr = disp_dev_get_drvdata(disp_dev);
	struct clk * lcd0_clk  = clk_get_sys(CLK_NAME_LCD0_CLK, NULL); 
#ifndef NOT_OPTIMIZE_ON_OFF_TIME
    if(s_open_flag > 0) {
    	return;
    }
#endif
	direct_set_lcd_gpio_active(lcd_mgr->lcd_info[0].gpio_info.pwm_gpio, 0);
	// reset lcdc 
    clk_reset(lcd0_clk);	
   	
	/* enhance lcd pad drv */
	if(lcd_mgr->get_pin){
		lcd_mgr->get_pin(disp_dev);
	}
	
	/*set rgb lcd clk*/	
	if(lcd_mgr->set_lcdc_clk){
		lcd_mgr->set_lcdc_clk(disp_dev,lcd_mgr->src_clk_type,lcd_mgr->lcd_clk);
	}else{
		printk("set_lcdc_clk not support  \n");
	}
	
	if(lcd_mgr->lcd_init){
		lcd_mgr->lcd_init(disp_dev);
	}else{
		printk("error :lcd init method is null \n");
	}
	
  print_debug_info_lcd();
   set_lcd_output_default_color();
   s_open_flag = 1;
}

void close_lcd(struct asoc_display_device *disp_dev)
{
	struct lcd_manager * lcd_mgr = disp_dev_get_drvdata(disp_dev);
    struct clk * lcd0_clk;
    
    set_lcd_resume_from_sleep(0);
    //if(!is_lcd_in_open_status()) {
    	//return;
    //}
	if(lcd_mgr->lcd_close){
		lcd_mgr->lcd_close(disp_dev);
	}
	
	lcd0_clk	= clk_get_sys(CLK_NAME_LCD0_CLK, NULL);
    if(clk_is_enabled(lcd0_clk)){
	    clk_disable(lcd0_clk);  /*disable clk*/ 	
    }
  
	lcd_mgr->release_pin(disp_dev);	
	s_open_flag = 0;
}

static int act_lcdc_update_mode(struct asoc_display_device *disp_dev)
{
	LCD_DEBUG_PRINT( "act_lcdc_update_mode called\n");

	if(disp_dev->state == true){
		
		set_bl_power(disp_dev,FB_BLANK_POWERDOWN);
		
		close_lcd(disp_dev);
		
		open_lcd(disp_dev);
		
	//	msleep_interruptible(200);
		
		set_bl_power(disp_dev,FB_BLANK_UNBLANK);
		
	}
	return 0;
}

static int act_lcdc_update_status(struct asoc_display_device *disp_dev)
{
  int state = disp_dev->state;

  switch(state) {
  	case 0:
  		set_bl_power(disp_dev,FB_BLANK_POWERDOWN);	
		close_lcd(disp_dev);
		break;
	case 1:
		open_lcd(disp_dev);
		set_bl_power(disp_dev,FB_BLANK_UNBLANK);
		break;
	case 2:
		open_lcd(disp_dev);
		break;
	case 3:
		set_bl_power(disp_dev,FB_BLANK_UNBLANK);
		break;
	case 4:
		set_bl_power(disp_dev,FB_BLANK_NORMAL);
		break;
	default:
		break;
	}
	
    if(state > 1) {
    	disp_dev->state = 1;
    }
	return 0;
}

static int act_lcdc_get_mdsb_parameter(struct asoc_display_device *disp_dev,
								int disp_mode ,int * hr , int * vr , int * drop, int * delay)
{
	struct lcd_manager * lcd_mgr = disp_dev_get_drvdata(disp_dev);
	if(lcd_mgr->get_mdsb_parameter){
		lcd_mgr->get_mdsb_parameter(disp_dev,disp_mode,hr,vr,drop,delay);
	}else{
		printk("not support sync diplay with tvout \n");
	}
	return 0;
}

void act_lcdc_shut_down(struct device *dev) {
	static int shutdown_count = 0;
	struct asoc_display_device *disp_dev = disp_manager_get_disp_dev(1);
	if(shutdown_count > 0) {
		shutdown_count = 0;
		return;
	} else {
		shutdown_count++;
	}
	set_bl_power(disp_dev,FB_BLANK_POWERDOWN);	
    close_lcd(disp_dev);
}

static void set_quickboot_not_control_reg(struct lcd_manager * lcd_mgr) {
	if(lcd_mgr != NULL) {
		quickboot_not_control_gpio(lcd_mgr->lcd_info[0].gpio_info.pwm_gpio);
	    quickboot_not_control_gpio(lcd_mgr->lcd_info[0].gpio_info.lcd_vcc_gpio);
	    quickboot_not_control_gpio(lcd_mgr->lcd_info[0].gpio_info.led_en_gpio);
	}
}

int act_lcdc_probe(struct asoc_display_device *pdev)
{
	int i = 0 , ret = 0, data_width = 0;
	struct lcd_manager * lcd_mgr = NULL;
	struct clk * lcd0_clk;

	lcd_mgr = kzalloc(sizeof(struct lcd_manager), GFP_KERNEL);

	if (!lcd_mgr)
		return -ENOMEM;
		
	if(lcd_manager_init(lcd_mgr)){
		printk("lcd_manager_init failed \n");
		return -ENOMEM;
	}
	set_quickboot_not_control_reg(lcd_mgr);
    lcd0_clk = clk_get_sys(CLK_NAME_LCD0_CLK, NULL);
    clk_enable(lcd0_clk);
    lcd_mgr->set_lcdc_clk = set_lcdc_clk;
	pdev->disp_cur_mode = &lcd_mgr->lcd_info[i].modes[0];
	pdev->is_plugged = 1;			
	pdev->state = 1;
	data_width = lcd_mgr->lcd_info[i].data_width;
	if(data_width == 1) {
		pdev->dither_info = 0x007020;
	} else if(data_width == 2) {
		pdev->dither_info = 0x002020;
	} else {
		pdev->dither_info = 0;
	} 

	pdev->fb_blank = FB_BLANK_UNBLANK;			
  
  connect_backlight_device(pdev); 
  
  pdev->disp_priv_info = lcd_mgr;
   pdev->dev.driver->shutdown = act_lcdc_shut_down;
	
	return ret;
}

void act_lcdc_remove(struct asoc_display_device *pdev)
{
	struct asoc_display_device * disp_dev = pdev;

	set_bl_power(disp_dev, FB_BLANK_POWERDOWN);
	
	close_lcd(disp_dev);
	
	disconnect_backlight_device(disp_dev);
	
	asoc_display_device_unregister(disp_dev);
	
	return ;
}

#ifdef CONFIG_PM
static int act_lcdc_suspend(struct asoc_display_device *pdev)
{	
	return 0;
}

static int act_lcdc_resume(struct asoc_display_device *pdev)
{
	return 0;
}
#endif
static int lcd_early_resume(struct device *dev)
{
#ifndef NOT_OPTIMIZE_ON_OFF_TIME
	static int resume_count = 0;
	struct asoc_display_device *disp_dev = disp_manager_get_disp_dev(1);
	if(resume_count > 0) {
		resume_count = 0;
		return;
	} else {
		resume_count++;
	}
	s_open_flag = 0;
	set_lcd_resume_from_sleep(1);
	open_lcd(disp_dev);
	printk("in lcd lcd_early_resume!\n");
#endif
	return 0;
}

static struct dev_pm_ops lcd_pm_ops = {
	 .resume_early = lcd_early_resume,
};

static struct atm702x_display_driver act_lcdc_driver = {
	.driver = {
	.name = "act_lcd",
	.owner = THIS_MODULE,
	.pm = &lcd_pm_ops,
	},
#ifdef CONFIG_PM
	.suspend = act_lcdc_suspend,
	.resume = act_lcdc_resume,
#endif
	.probe = act_lcdc_probe,
	.remove = act_lcdc_remove,
	.update_status = act_lcdc_update_status,	
	.update_mode = act_lcdc_update_mode,	
	.get_mdsb_parameter = act_lcdc_get_mdsb_parameter,
};

static int __init act_lcdc_init(void)
{
	return asoc_display_driver_register(&act_lcdc_driver);
}

module_init(act_lcdc_init);

static void __exit act_lcdc_exit(void)
{
	asoc_display_driver_unregister(&act_lcdc_driver);
}

module_exit(act_lcdc_exit);

MODULE_AUTHOR("Actions_semi,wh <wanghui@actions-semi.com>");
MODULE_DESCRIPTION("actions LCDC");
MODULE_LICENSE("GPL");
