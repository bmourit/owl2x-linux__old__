/*
 *  drivers/ribvate/act_ribrate.c
 *
 * Copyright (C) 2008 Google, Inc.
 * Author: Mike Lockwood <lockwood@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
*/

#include <linux/hrtimer.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/pwm.h>
#include <linux/wakelock.h>
#include <linux/mutex.h>
#include <linux/clk.h>
#include <linux/workqueue.h>
#include <linux/module.h>
#include <asm/prom.h>
#include <mach/gpio.h>

#include <asm/mach-types.h>

//#include <../../../drivers/staging/android/timed_output.h>
#include "timed_output.h"



static struct vibrator {
	struct wake_lock wklock;
	struct pwm_device *pwm_dev;
	struct hrtimer timer;
	struct mutex lock;
	struct work_struct work;
} vibdata;


char *name = "vibrate_en";

int max_timeout=0;
int min_timeout=0;
int vibrate_en_gpio = 0;

struct gpio_pre_cfg pcfg;


static void act_vibrator_off(void)
{	
	gpio_direction_output(vibrate_en_gpio, pcfg.unactive_level);
	wake_unlock(&vibdata.wklock);
}


static int act_vibrator_get_time(struct timed_output_dev *dev)
{
	if (hrtimer_active(&vibdata.timer)) {
		ktime_t r = hrtimer_get_remaining(&vibdata.timer);
		return ktime_to_ms(r);
	}

	return 0;
}

static void act_vibrator_enable(struct timed_output_dev *dev, int value)
{
	mutex_lock(&vibdata.lock);

	/* cancel previous timer and set GPIO according to value */
	hrtimer_cancel(&vibdata.timer);
	cancel_work_sync(&vibdata.work);
	
	//printk("vibrator_value=%d\n",value);
	
	if (value) 
	{
			wake_lock(&vibdata.wklock);
			gpio_direction_output(vibrate_en_gpio, pcfg.active_level);
		
			if (value > max_timeout)
			{
				value = max_timeout;
			}
			else if(value < min_timeout)
			{
				value = min_timeout;
			}

			hrtimer_start(&vibdata.timer,
				ns_to_ktime((u64)value * NSEC_PER_MSEC),
				HRTIMER_MODE_REL);
	}
	else
	{
		act_vibrator_off();
	}
	mutex_unlock(&vibdata.lock);
}



static struct timed_output_dev to_dev = {
	.name		= "vibrator",
	.get_time	= act_vibrator_get_time,
	.enable		= act_vibrator_enable,
};


static enum hrtimer_restart act_vibrator_timer_func(struct hrtimer *timer)
{
	schedule_work(&vibdata.work);
	return HRTIMER_NORESTART;
}


static void act_vibrator_work(struct work_struct *work)
{
	act_vibrator_off();
}

static void act_vibrate_get_configration(void)
{

	get_config("vibrate.max_timeout", &max_timeout, sizeof(int));
    printk("vibrate.max_timeout:%d\n", max_timeout);
		
    get_config("vibrate.min_timeout", &min_timeout, sizeof(int));
    printk("vibrate.min_timeout:%d\n", min_timeout);


	 memset((void *)&pcfg, 0, sizeof(struct gpio_pre_cfg));
	gpio_get_pre_cfg(name, &pcfg);
	
	printk("iogroup:%d,pin_num:%d,gpio_dir:%d,init_level:%d,active_level:%d,unactive_level:%d,valid:%d\n", 
		pcfg.iogroup , pcfg.pin_num , pcfg.gpio_dir , pcfg.init_level , pcfg.active_level , pcfg.unactive_level , pcfg.valid);
	
	vibrate_en_gpio = ASOC_GPIO_PORT(pcfg.iogroup, pcfg.pin_num);
	
		
}


static int __init act_vibrate_init(void)
{
		int ret = 0;

		act_vibrate_get_configration();

		hrtimer_init(&vibdata.timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
		vibdata.timer.function = act_vibrator_timer_func;
		INIT_WORK(&vibdata.work, act_vibrator_work);

		ret = gpio_request(vibrate_en_gpio, name);
		if (ret < 0)
		{
			printk("!!!!!vibrate_drv: gpio_request_failed!!!");
			return ret;
		}

    	ret = gpio_direction_output(vibrate_en_gpio, pcfg.unactive_level);
    	if (ret<0) 
		{
			printk("!!!!!vibrate_drv: gpio_direction_output_failed!!!");		
			gpio_free(vibrate_en_gpio);
        	return ret;
   		 }

		wake_lock_init(&vibdata.wklock, WAKE_LOCK_SUSPEND, "vibrator");
		mutex_init(&vibdata.lock);
	
		ret = timed_output_dev_register(&to_dev);
		if (ret < 0)
		{
			printk("!!!!vibrate_drv:timed_output_dev_register_failed !!!");	
			goto err_to_dev_reg;
		}

		return 0;

		err_to_dev_reg:
			timed_output_dev_unregister(&to_dev);
			mutex_destroy(&vibdata.lock);
			wake_lock_destroy(&vibdata.wklock);
			gpio_free(vibrate_en_gpio);
			return ret;
	
	}
	

static void __exit act_vibrate_exit(void)
{
		timed_output_dev_unregister(&to_dev);
		mutex_destroy(&vibdata.lock);
		wake_lock_destroy(&vibdata.wklock);
		gpio_free(vibrate_en_gpio);
}

module_init(act_vibrate_init);
module_exit(act_vibrate_exit);

MODULE_AUTHOR("Actions_semi,sunny<sunnyliu@actions-semi.com>");
MODULE_DESCRIPTION("actions VIBRATE");
MODULE_LICENSE("GPL");
