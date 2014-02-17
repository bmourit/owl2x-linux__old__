/*
 * Asoc adc keypad driver
 *
 * Copyright (C) 2011 Actions Semiconductor, Inc
 * Author:	chenbo <chenbo@actions-semi.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <asm/delay.h>
#include <asm/io.h>
#include <linux/fb.h>
#include <mach/atc260x/atc260x.h>
#include <mach/atc260x/atc260x_pdata.h>

#define ADCKEYPAD_DEBUG		0
#define ADCKEYPAD_DEBUG_INFO		1
#if (ADCKEYPAD_DEBUG_INFO == 1)
	#define GL5201_ADCKEY_INFO(fmt, args...)	\
		printk(KERN_INFO "gl5201_adckey_drv: " fmt, ##args)
#else
	#define GL5201_ADCKEY_INFO(fmt, args...)
#endif

#define REMCON_ADC_EN			(1 << 5)
#define KEY_FILTER_DEPTH		5
#define KEY_VAL_MASK			0x0
#define KEY_VAL_INIT			KEY_UP
#define KEY_VAL_HOLD			SW_RADIO

extern int get_config(const char *key, char *buff, int len);

#if (ADCKEYPAD_DEBUG == 1)
static const unsigned int left_adc[9] =  {0x00, 0x32, 0x97, 0xfb,  0x15f, 0x1c3, 0x24e, 0x2b3, 0x317};
static const unsigned int right_adc[9] = {0x00, 0x96, 0xfa, 0x15e,0x1c2, 0x226, 0x2b2, 0x316, 0x400};
static const unsigned int key_val[9] =  {KEY_HOME, KEY_MENU, KEY_VOLUMEUP, KEY_VOLUMEDOWN, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED,  KEY_UP};
#else
char *key[] = {
	"adckey.size",
	"adckey.left",
	"adckey.right",
	"adckey.values",
	"adckey.filter_dep",
	"adckey.variance",
	"adckey.period",
};
#endif

struct atc260x_adckeypad {
  	struct atc260x_dev *atc260x_dev;
	struct input_polled_dev *poll_dev;

	unsigned int *adc_buffer;
	
	unsigned int *left_adc_val;
	unsigned int *right_adc_val;
	unsigned int *key_values;

	unsigned int filter_dep;
	unsigned int variance;
	
	unsigned int adc_val;
	unsigned int adc_val_sum;
	unsigned int keymapsize;
	unsigned int old_key_val;
	unsigned int key_val;
	unsigned int filter_index;

	unsigned int pressed;
};

static inline unsigned int atc260x_adckeypad_convert(unsigned int adc_val,
	struct atc260x_adckeypad *atc260x_adckeypad)
{
	unsigned int i;
	unsigned int key_val = KEY_RESERVED;
	
	for(i = 0; i < atc260x_adckeypad->keymapsize; i++){
		
		if ((adc_val >= *(atc260x_adckeypad->left_adc_val + i)) 
			&& (adc_val <= *(atc260x_adckeypad->right_adc_val + i))) {

			key_val = *(atc260x_adckeypad->key_values+ i);
			break;
			
		}		
	}
	
	return key_val;
}

static void atc260x_adckeypad_report(struct input_dev *input_dev,
	struct atc260x_adckeypad *atc260x_adckeypad)
{
	unsigned int changed;
	//unsigned int pressed;
	
	changed = atc260x_adckeypad->old_key_val
		^ atc260x_adckeypad->key_val;
//	    GL5201_ADCKEY_INFO("0::old_key_val= %d key_val = %d\n", atc260x_adckeypad->old_key_val, atc260x_adckeypad->key_val);

	if(changed) {
		
		if(atc260x_adckeypad->key_val != KEY_RESERVED) {
			
//	            GL5201_ADCKEY_INFO("1::key_val = %d\n", atc260x_adckeypad->key_val);
			if(atc260x_adckeypad->key_val == KEY_UP) {
				
//	            GL5201_ADCKEY_INFO("2::key_val = %d\n", atc260x_adckeypad->key_val);
				atc260x_adckeypad->pressed = 0;

				if (atc260x_adckeypad->old_key_val == KEY_VAL_HOLD) {

					input_report_switch(input_dev, 
						atc260x_adckeypad->old_key_val, atc260x_adckeypad->pressed);	

					GL5201_ADCKEY_INFO("hold: %d %s\n",
						atc260x_adckeypad->old_key_val,"unlocked");


				} else {
				
					input_report_key(input_dev, 
						atc260x_adckeypad->old_key_val, atc260x_adckeypad->pressed);	
					GL5201_ADCKEY_INFO("key: %d %s\n",
						atc260x_adckeypad->old_key_val,"released");
					atc260x_adckeypad->pressed = 0;
				}
				
			} else if(atc260x_adckeypad->key_val == KEY_VAL_HOLD) {

				input_report_key(input_dev, 
						atc260x_adckeypad->old_key_val, 0);	
				GL5201_ADCKEY_INFO("key: %d %s\n",
						atc260x_adckeypad->old_key_val,"released");
				input_report_switch(input_dev, 
						atc260x_adckeypad->key_val, 1);
				GL5201_ADCKEY_INFO("hold: %d %s\n",
						atc260x_adckeypad->key_val,"pressed");
				
			} else {

				if (atc260x_adckeypad->pressed) {
					
					input_report_key(input_dev, 
						atc260x_adckeypad->old_key_val, !atc260x_adckeypad->pressed);
					GL5201_ADCKEY_INFO("key: %d %s\n",
						atc260x_adckeypad->old_key_val,"released");
					
				} else {
				
					atc260x_adckeypad->pressed = 1;
					
				}

				input_report_key(input_dev, 
						atc260x_adckeypad->key_val, atc260x_adckeypad->pressed);
				GL5201_ADCKEY_INFO(KERN_INFO"key: %d %s\n",
						atc260x_adckeypad->key_val,"pressed");
			}
			
			input_sync(input_dev);
			
			atc260x_adckeypad->old_key_val = 
				atc260x_adckeypad->key_val;
		}
	} 
}

static void atc260x_adckeypad_scan(struct atc260x_dev *atc260x_dev,
	struct input_polled_dev *poll_dev)
{
	struct atc260x_adckeypad *atc260x_adckeypad = poll_dev->private;

	atc260x_adckeypad->adc_val = atc260x_reg_read(atc260x_dev, gl5302_PMU_RemConADC);
	//printk("atc260x_adckeypad->adc_val : %d\n", atc260x_adckeypad->adc_val);
}


static int atc260x_adckeypad_filter(struct input_polled_dev *dev)
{
	struct atc260x_adckeypad *atc260x_adckeypad = dev->private;
   	 int filter_index = 0;
    	int i = 0;
	int j = 0;
	int variance = 0;
    
    	if (atc260x_adckeypad->adc_buffer == NULL) {
		
        		return -1;
		
    	}
	
    	for (i=0; i < ((atc260x_adckeypad->filter_dep + 1) / 2); i++) { 
		
        		filter_index = 1;
		
        		atc260x_adckeypad->adc_val_sum = atc260x_adckeypad->adc_buffer[i];
        
       	 	for (j = i + 1; j < atc260x_adckeypad->filter_dep; j++) {

			variance = atc260x_adckeypad->adc_buffer[i] - 
				atc260x_adckeypad->adc_buffer[j];
			variance = (variance > 0) ? variance : (-variance);
			
			if (variance < atc260x_adckeypad->variance) {
                			filter_index++;
                			atc260x_adckeypad->adc_val_sum += atc260x_adckeypad->adc_buffer[j];
       
            			}

			
        		}
		
       		 if ( (filter_index >= ((KEY_FILTER_DEPTH + 1) / 2)) 
			&& (filter_index > 0) ) {
                   
           			 atc260x_adckeypad->adc_val = atc260x_adckeypad->adc_val_sum / filter_index;

			return 0;
			
        		}
    	}

    	return -1;
}

static void atc260x_adckeypad_poll(struct input_polled_dev *dev)
{
	struct atc260x_adckeypad *atc260x_adckeypad = dev->private;
	struct input_dev *input_dev = dev->input;

	atc260x_adckeypad_scan(atc260x_adckeypad->atc260x_dev, dev);
	if (atc260x_adckeypad->filter_index < atc260x_adckeypad->filter_dep) {
		*(atc260x_adckeypad->adc_buffer + atc260x_adckeypad->filter_index)
			= atc260x_adckeypad->adc_val;
		
		atc260x_adckeypad->filter_index++;

		return;
		
	} else {
		if (!atc260x_adckeypad_filter(dev)) {

			atc260x_adckeypad->key_val = 
				atc260x_adckeypad_convert(atc260x_adckeypad->adc_val, atc260x_adckeypad);

			atc260x_adckeypad_report(input_dev, atc260x_adckeypad);
		}
		
		atc260x_adckeypad->filter_index = 0;
		*(atc260x_adckeypad->adc_buffer + atc260x_adckeypad->filter_index)
			= atc260x_adckeypad->adc_val;
		
		atc260x_adckeypad->filter_index++;
		
	}

	return;
}

static void atc260x_adckeypad_config(struct atc260x_dev *atc260x_dev)
{
    atc260x_set_bits(atc260x_dev, gl5302_PMU_AuxADC_CTL0, 
		REMCON_ADC_EN, REMCON_ADC_EN);
	
    GL5201_ADCKEY_INFO("open:AuxADC_CTL0 = 0x%x\n",
		atc260x_reg_read(atc260x_dev, gl5302_PMU_AuxADC_CTL0));                
}

static int __devinit atc260x_adckeypad_probe(struct platform_device *pdev)
{
	struct atc260x_dev *atc260x_dev = 
		dev_get_drvdata(pdev->dev.parent);
	struct atc260x_adckeypad *atc260x_adckeypad;
	struct input_polled_dev *poll_dev;
	struct input_dev *input_dev;
	int ret = 0;
	int i;
	
	GL5201_ADCKEY_INFO("[%s start]\n",__func__);
	
	atc260x_adckeypad =
		kzalloc(sizeof(struct atc260x_adckeypad), GFP_KERNEL);
	if (!atc260x_adckeypad) 
		return -ENOMEM;

	platform_set_drvdata(pdev, atc260x_adckeypad);
	
  	atc260x_adckeypad->atc260x_dev = atc260x_dev;            
	atc260x_adckeypad->filter_index = 0;
	atc260x_adckeypad->old_key_val = KEY_VAL_INIT;
	atc260x_adckeypad->adc_val_sum = 0;
	atc260x_adckeypad->pressed = 0;
	/*
	 * get configure info from xml
	 */
#if (ADCKEYPAD_DEBUG == 1)
	atc260x_adckeypad->keymapsize = 9;
	/*get left adc val*/
	atc260x_adckeypad->left_adc_val = left_adc;
	/*get right adc val*/
	atc260x_adckeypad->right_adc_val = right_adc;
	/*get key values*/
	atc260x_adckeypad->key_values = key_val;

	atc260x_adckeypad->filter_dep = 5;
	atc260x_adckeypad->variance = 50;

	atc260x_adckeypad->adc_buffer= 
		(unsigned int *)kzalloc(sizeof(unsigned int) * atc260x_adckeypad->filter_dep, GFP_KERNEL);
    	if (!atc260x_adckeypad->adc_buffer)
    	goto free_buffer;

#else
	/*get key map size*/
	ret = get_config(key[0], (char *)(&atc260x_adckeypad->keymapsize), sizeof(unsigned int));
	if (ret != 0) {
		GL5201_ADCKEY_INFO("[%s]get size fail!\n", __func__);
		goto free_right;
	}
		
	/*get left adc val*/
	atc260x_adckeypad->left_adc_val = 
		kzalloc(sizeof(unsigned int) * (atc260x_adckeypad->keymapsize), GFP_KERNEL);
	if (!atc260x_adckeypad->left_adc_val) 
		goto free;
	ret = get_config(key[1], (char *)atc260x_adckeypad->left_adc_val, 
		sizeof(unsigned int) * (atc260x_adckeypad->keymapsize));
	if (ret != 0) {
		GL5201_ADCKEY_INFO("[%s]get left fail!\n", __func__);
		goto free_right;
	}
	
	/*get right adc val*/
	atc260x_adckeypad->right_adc_val = 
		kzalloc(sizeof(unsigned int) * atc260x_adckeypad->keymapsize, GFP_KERNEL);
	if (!atc260x_adckeypad->right_adc_val)
		goto free_left;
	ret = get_config(key[2], (char *)atc260x_adckeypad->right_adc_val, 
		sizeof(unsigned int) * (atc260x_adckeypad->keymapsize));
	if (ret != 0) {
		GL5201_ADCKEY_INFO("[%s]get right fail!\n", __func__);
		goto free_right;
	}

	/*get key values*/
	atc260x_adckeypad->key_values = 
		kzalloc(sizeof(unsigned int) * atc260x_adckeypad->keymapsize, GFP_KERNEL);
	if (!atc260x_adckeypad->key_values) 
		goto free_right;

	ret = get_config(key[3], (char *)atc260x_adckeypad->key_values, 
		sizeof(unsigned int) * (atc260x_adckeypad->keymapsize));
	if (ret != 0) {
		GL5201_ADCKEY_INFO("[%s]get values fail!\n", __func__);
		goto free_right;
	}

	/*filter*/
	ret = get_config(key[4], (char *)(&atc260x_adckeypad->filter_dep), sizeof(unsigned int));
	if (ret != 0) {
		GL5201_ADCKEY_INFO("[%s]get filter_dep fail!\n", __func__);
		goto free_right;
	}
	atc260x_adckeypad->adc_buffer= 
		(unsigned int *)kzalloc(sizeof(unsigned int) * atc260x_adckeypad->filter_dep, GFP_KERNEL);
    	if (!atc260x_adckeypad->adc_buffer)
    	goto free_buffer;

	ret = get_config(key[5], (char *)(&atc260x_adckeypad->variance), sizeof(unsigned int));
	if (ret != 0) {
		GL5201_ADCKEY_INFO("[%s]get variance fail!\n", __func__);
		goto free_buffer;
	}
#endif
	/*
	 * poll dev related
	 */
	poll_dev = input_allocate_polled_device();
	if (!poll_dev) {
		ret = -ENOMEM;
		goto free_key_values;
	}
	atc260x_adckeypad->poll_dev = poll_dev;
	
	poll_dev->private = atc260x_adckeypad;
	poll_dev->poll = atc260x_adckeypad_poll;

#if (ADCKEYPAD_DEBUG == 1)
	poll_dev->poll_interval = 5;/* msec */
#else
	/*get poll period*/
	ret = get_config(key[6], (char *)&poll_dev->poll_interval, sizeof(unsigned int));
	if (ret != 0) {
		GL5201_ADCKEY_INFO("[%s]get poll_interval fail!\n", __func__);
		goto free_buffer;
	}
#endif

	input_dev = poll_dev->input;
	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_REP) | BIT(EV_SW);
	input_dev->name = pdev->name;
	input_dev->phys = "atc260x_adckeypad/input3";
	input_dev->keycode = atc260x_adckeypad->key_values;
	input_dev->keycodesize = atc260x_adckeypad->keymapsize;
	input_dev->keycodemax = atc260x_adckeypad->keymapsize;
	input_dev->dev.parent = &pdev->dev;
	input_dev->id.bustype = BUS_HOST;
	
	for (i = 0; i < atc260x_adckeypad->keymapsize; i++)
		__set_bit(*(atc260x_adckeypad->key_values + i),
			  input_dev->keybit);

	__clear_bit(KEY_RESERVED, input_dev->keybit);
	__set_bit(KEY_POWER,input_dev->keybit);
	__set_bit(KEY_POWER2,input_dev->keybit);
	__set_bit(KEY_VAL_HOLD, input_dev->swbit);

	input_set_capability(input_dev, EV_MSC, MSC_SCAN);
	ret = input_register_polled_device(poll_dev);
	GL5201_ADCKEY_INFO("[in %s]ret = %d\n",__func__,ret);
	if (ret)
		goto free_polled;

	input_dev->timer.data = (long) input_dev;

	atc260x_adckeypad_config(atc260x_adckeypad->atc260x_dev);

	GL5201_ADCKEY_INFO("[%s finish]\n",__func__);

	return 0;

	
free_polled:
	platform_set_drvdata(pdev, NULL);
	input_free_polled_device(poll_dev);
	
free_key_values:
	kfree(atc260x_adckeypad->key_values);

free_buffer:
	kfree(atc260x_adckeypad->adc_buffer);

free_right:
	kfree(atc260x_adckeypad->right_adc_val);

free_left:
	kfree(atc260x_adckeypad->left_adc_val);
	
free:
	kfree(atc260x_adckeypad);	

	return ret;
}

static int __devexit atc260x_adckeypad_remove(struct platform_device *pdev)
{
	struct atc260x_adckeypad *atc260x_adckeypad =
	    platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);
	input_unregister_polled_device(atc260x_adckeypad->poll_dev);
	input_free_polled_device(atc260x_adckeypad->poll_dev);
	kfree(atc260x_adckeypad);

	return 0;
}

#ifdef CONFIG_PM
static int atc260x_adckeypad_suspend(struct platform_device *pdev,
	pm_message_t state)
{
    struct atc260x_adckeypad *atc260x_adckeypad =
	platform_get_drvdata(pdev);
    
    cancel_delayed_work_sync(&atc260x_adckeypad->poll_dev->work);
	return 0;
}

static int atc260x_adckeypad_resume(struct platform_device *pdev)
{
	struct atc260x_dev *atc260x_dev = 
		dev_get_drvdata(pdev->dev.parent);
	struct atc260x_adckeypad *atc260x_adckeypad =
	    platform_get_drvdata(pdev);
	schedule_delayed_work(&atc260x_adckeypad->poll_dev->work, 
	    msecs_to_jiffies(atc260x_adckeypad->poll_dev->poll_interval));
	atc260x_adckeypad_config(atc260x_dev);
	
	return 0;
}
#else
	# define atc260x_adckeypad_suspend NULL
	# define atc260x_adckeypad_resume  NULL
#endif

static struct platform_driver atc260x_adckeypad_driver = {
	.driver = {
		   .name = "atc260x-adckeypad",
		   .owner = THIS_MODULE,
		   },
	.probe = atc260x_adckeypad_probe,
	.remove = __devexit_p(atc260x_adckeypad_remove),
	.suspend = atc260x_adckeypad_suspend,
	.resume = atc260x_adckeypad_resume,

};

static int __init atc260x_adckeypad_init(void)
{
	return platform_driver_register(&atc260x_adckeypad_driver);
}

static void __exit atc260x_adckeypad_exit(void)
{
	platform_driver_unregister(&atc260x_adckeypad_driver);
}

module_init(atc260x_adckeypad_init);
module_exit(atc260x_adckeypad_exit);

MODULE_DESCRIPTION("Asoc adckey  drvier");
MODULE_AUTHOR("chenbo <chenbo@actions-semi.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:asoc-adckeypad");
