/*
 * linux/drivers/video/backlight/pwm_bl.c
 *
 * simple PWM based backlight control, board code has to setup
 * 1) pin configuration so PWM waveforms can output
 * 2) platform_data casts to the PWM id (0/1/2/3 on PXA)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/backlight.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/delay.h>
#include <mach/actions_reg_gl5202.h>
#include <mach/hardware.h>
//#include <linux/gpio.h>
//#include "kconfig.h"
#include "../act_lcdc_config.h"
#include "../act_lcdc.h"

#if 0
#define BKL_PRINT(fmt, args...) printk(KERN_ALERT fmt, ##args)
#else
#define BKL_PRINT(fmt, args...)
#endif

struct backlight_device* act_pwm_bl_device;
struct gpio_pre_cfg* backlight_led_en_gpio = NULL;
struct gpio_pre_cfg* backlight_pwm_gpio = NULL;
struct mutex bl_set_backlight_lock;
static int last_status = 1;
static int lcd_resume_flag = 0;
	
struct backlight_device *act_pwm_bl_connect(void)
{
	act_pwm_bl_device->props.power = FB_BLANK_UNBLANK;
	return act_pwm_bl_device;
}
EXPORT_SYMBOL(act_pwm_bl_connect);

void act_pwm_bl_disconnect(struct backlight_device *bl)
{
	bl->props.power = FB_BLANK_POWERDOWN;
	backlight_update_status(bl);

	return ;
}
EXPORT_SYMBOL(act_pwm_bl_disconnect);

void set_lcd_resume_from_sleep(int flag) {
	lcd_resume_flag = flag;
}
EXPORT_SYMBOL(set_lcd_resume_from_sleep);

static int get_init_backlight_status(void) {
	int pwm_num = get_lcd_config(BACKLIGHT_INFO, BACKLIGHT_PWM_NUMBER, 0);
	if(act_readl(CMU_DEVCLKEN1) & (0x1 << (23 + pwm_num))) {
		return 1;
	}
	return 0;
}

static int act_pwm_bl_update_status(struct backlight_device *bl)
{
	static int brightness_fb_blank = -1;
	struct act_pwm_bl *pb = dev_get_drvdata(&bl->dev);
	int brightness = bl->props.brightness;
	int period_duty;
	int max = 1023;
	int status = 1;
    
	mutex_lock(&bl_set_backlight_lock);
    if(brightness == 0) 
  	    status = 0;
 	
	if (bl->props.power == FB_BLANK_POWERDOWN)
		status = 0;

    if(bl->props.power == FB_BLANK_NORMAL){
		last_status = 1;
		bl->props.power = FB_BLANK_UNBLANK;
		mutex_unlock(&bl_set_backlight_lock);
		return 0;	
	}
	
	if (bl->props.fb_blank != FB_BLANK_UNBLANK) {
		if(bl->props.brightness > 0) {
		    brightness_fb_blank = bl->props.brightness;
		    bl->props.brightness = 0;
		    status = 0;
	    }
	} else if(brightness_fb_blank > 0) {
		bl->props.brightness = brightness_fb_blank;
		brightness = bl->props.brightness;
		brightness_fb_blank = -1;
		status = 1;
	}
	
	if (pb->notify) {
		brightness = pb->notify(brightness);
	}

    if(!pb->active_high) {
		period_duty = (max - brightness) * pb->period /max;  
	}else{
		period_duty = brightness * pb->period /max;
    }
    
	if (status == 0) {
		if(last_status) {
		    set_lcd_gpio_active(backlight_led_en_gpio, 0);
            free_lcd_gpio(backlight_led_en_gpio);
            if(!pb->active_high) {
		    	pwm_config(pb->pwm, pb->period, pb->period);
		    } else {
		    	pwm_config(pb->pwm, 0, pb->period);
		    }
		    pwm_disable(pb->pwm);
		    direct_set_lcd_gpio_active(backlight_pwm_gpio, 0);
		    mdelay(50);
	    }
	} else {
		if(!last_status) {
			 if(!pb->active_high) {
                 pwm_config(pb->pwm, pb->period, pb->period);
	         } else {
                 pwm_config(pb->pwm, 0, pb->period);
	         }
	        pwm_enable(pb->pwm);	
	        //如果只是开关屏未进入睡眠状态则需要延迟，如果是从睡眠状态恢复则不需要延迟
	        if(!lcd_resume_flag) {
			    mdelay(200);
			}
			set_lcd_resume_from_sleep(0);
			direct_set_lcd_gpio_active(backlight_pwm_gpio, -1);
		    set_lcd_gpio_active(backlight_led_en_gpio, 1);   
		}
		pwm_config(pb->pwm, period_duty, pb->period);	
	}
	
	last_status = status;
	mutex_unlock(&bl_set_backlight_lock);
	return 0;
}

static int act_pwm_bl_get_brightness(struct backlight_device *bl)
{
	return bl->props.brightness;
}

static void set_quickboot_not_control_reg(void) {
	int pwm_num = get_lcd_config(BACKLIGHT_INFO, BACKLIGHT_PWM_NUMBER, 0);
    hibernate_reg_setmap(MFP_CTL1, 3 << 21);
	hibernate_reg_setmap(CMU_DEVCLKEN1, 1 << (23 + pwm_num));
	hibernate_reg_setmap(CMU_PWM0CLK + 4*pwm_num, 1 << (23 + pwm_num));
	hibernate_reg_setmap(PWM_CTL0 + 4*pwm_num, 1 << (23 + pwm_num));
}

static struct backlight_ops act_pwm_bl_ops = {
	.update_status = act_pwm_bl_update_status,
	.get_brightness = act_pwm_bl_get_brightness,
};
static struct backlight_device * bl = NULL;
static int act_pwm_bl_probe(struct platform_device *pdev)
{
	struct backlight_properties props;	
	struct act_pwm_bl *pb;
	int ret;

	pb = kzalloc(sizeof(*pb), GFP_KERNEL);
	if (!pb) {
		dev_err(&pdev->dev, "no memory for state\n");
		ret = -ENOMEM;
		return ret;
	}
    
    set_quickboot_not_control_reg();
	backlight_led_en_gpio = get_lcd_gpio_cfg(GPIO_LCD_LED_EN);
	backlight_pwm_gpio = get_lcd_gpio_cfg(GPIO_LCD_PWM);
	pb->total_steps = 0;
	pb->period = 1000000000 / get_lcd_config(BACKLIGHT_INFO, BACKLIGHT_PWM_FREQ, 200);

	pb->active_high = get_lcd_config(BACKLIGHT_INFO, BACKLIGHT_PWM_ACTIVE, 1);

	pb->pwm = pwm_request(get_lcd_config(BACKLIGHT_INFO, BACKLIGHT_PWM_NUMBER, 0), "backlight");
	if (IS_ERR(pb->pwm)) {
		dev_err(&pdev->dev, "unable to request PWM for backlight\n");
		ret = PTR_ERR(pb->pwm);
		goto err_pwm;
	} else
		dev_dbg(&pdev->dev, "got pwm for backlight\n");
		
    memset(&props,0,sizeof(props));
  
    props.type = BACKLIGHT_RAW;
	props.max_brightness = get_lcd_config(BACKLIGHT_INFO, BACKLIGHT_MAX_BRIGHTNESS, 840);
	  
	bl = backlight_device_register(pdev->name, &pdev->dev,
				       pb, &act_pwm_bl_ops, &props);
	if (IS_ERR(bl)) {
		dev_err(&pdev->dev, "failed to register backlight\n");
		ret = PTR_ERR(bl);
		goto err_bl;
	}
	mutex_init(&bl_set_backlight_lock);
	bl->props.max_brightness = props.max_brightness;
	// this only for tow diff backlight
	bl->props.brightness =   get_lcd_config(BACKLIGHT_INFO, BACKLIGHT_DEFAULT_BRIGHTNESS, 512);
	bl->props.power = FB_BLANK_UNBLANK;
	bl->props.min_brightness = get_lcd_config(BACKLIGHT_INFO, BACKLIGHT_MIN_BRIGHTNESS, 250);
	//backlight_update_status(bl);
    last_status = get_init_backlight_status();

	act_pwm_bl_device = bl;

	platform_set_drvdata(pdev, bl);
	return 0;

err_bl:
	pwm_free(pb->pwm);
err_pwm:
	kfree(pb);
	return ret;
}

static int act_pwm_bl_remove(struct platform_device *pdev)
{
	struct backlight_device *bl = platform_get_drvdata(pdev);
	struct act_pwm_bl *pb = dev_get_drvdata(&bl->dev);

	backlight_device_unregister(bl);
	if(!pb->active_high) {
	    pwm_config(pb->pwm, pb->period, pb->period);
    } else {
	    pwm_config(pb->pwm, 0, pb->period);
	}
	pwm_disable(pb->pwm);
	pwm_free(pb->pwm);
	kfree(pb);

	return 0;
}

#ifdef CONFIG_PM
static int act_pwm_bl_suspend(struct platform_device *pdev,
				  pm_message_t state)
{
	//struct backlight_device *bl = platform_get_drvdata(pdev);
	//struct act_pwm_bl *pb = dev_get_drvdata(&bl->dev);

//	bl->props.power = FB_BLANK_POWERDOWN;
//	backlight_update_status(bl);
  //pwm_free(pb->pwm);
	return 0;
}

static int act_pwm_bl_resume(struct platform_device *pdev)
{
//	struct backlight_device *bl = platform_get_drvdata(pdev);

//	bl->props.power = FB_BLANK_UNBLANK;
//	backlight_update_status(bl);
	return 0;
}
#else
#define act_pwm_bl_suspend	NULL
#define act_pwm_bl_resume	NULL
#endif

static struct platform_driver act_pwm_bl_driver = {
	.driver = {
		   .name = "act_pwm_backlight",
		   .owner = THIS_MODULE,
		   },
	.probe = act_pwm_bl_probe,
	.remove = act_pwm_bl_remove,
	.suspend = act_pwm_bl_suspend,
	.resume = act_pwm_bl_resume,
};

static struct platform_device * act_backlight_devices;

static int __init act_pwm_bl_init(void)
{
	int r = 0;
	act_backlight_devices = platform_device_register_simple("act_pwm_backlight", 0,NULL, 0);

	if(IS_ERR(act_backlight_devices))
	{
		printk(KERN_ERR "failed to register act_pwm_backlight device\n");
		r = PTR_ERR(act_backlight_devices);
		return r;
	}
	
	return platform_driver_register(&act_pwm_bl_driver);
}

module_init(act_pwm_bl_init);

static void __exit act_pwm_bl_exit(void)
{
	platform_driver_unregister(&act_pwm_bl_driver);
}

module_exit(act_pwm_bl_exit);

MODULE_DESCRIPTION("ACT PWM based Backlight Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:act_pwm_backlight");
