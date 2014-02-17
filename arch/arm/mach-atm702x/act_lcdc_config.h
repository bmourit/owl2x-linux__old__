#ifndef __ACT_LCDC_CONFIG_H__
#define __ACT_LCDC_CONFIG_H__

#ifdef BUILD_IN_BOOT_LCD
#include "boot_api.h"
#else
#include <linux/delay.h>
#include <linux/gpio.h>
#include <asm/prom.h>
#include <mach/gpio.h>
#endif

enum ENUM_CONFIG_TYPE {
	BACKLIGHT_INFO,
	LCD_BASE_INFO,
	LCD_SIGNAL_INFO,
	LCD_RGB_INFO,
	LCD_LVDS_INFO,
	LCD_MAX_INFO_NUM
};
 
enum ENUM_BACKLIGHT_INFO {
	BACKLIGHT_PWM_NUMBER,
	BACKLIGHT_PWM_FREQ,
	BACKLIGHT_PWM_ACTIVE,
	BACKLIGHT_MIN_BRIGHTNESS,
	BACKLIGHT_MAX_BRIGHTNESS,
	BACKLIGHT_DEFAULT_BRIGHTNESS
};

enum ENUM_BASE_INFO {
	BASE_CLOCK_RATE,
	BASE_FRAME_RATE,
	BASE_DATA_WIDTH,
	BASE_PIXEL_WIDTH,
	BASE_PIXEL_HEIGHT,
	BASE_HSYNC_PULSE_WIDTH,
	BASE_HSYNC_FRONT_PORCH,
	BASE_HSYNC_BACK_PORCH,
	BASE_VSYNC_PULSE_WIDTH,
	BASE_VSYNC_FRONT_PORCH,
	BASE_VSYNC_BACK_PORCH
};

enum ENUM_SIGNAL_INFO {
	SIGNAL_HSYNC_HIGH_ACTIVE,
	SIGNAL_VSYNC_HIGH_ACTIVE,
	SIGNAL_DCLK_RISING_ACTIVE,
	SIGNAL_DATA_LOW_ACTIVE
};

enum ENUM_LVDS_INFO {
	LVDS_OUTPUT_MAPPING,
	LVDS_OUTPUT_MIRROR,
	LVDS_ODD_POLARITY,
	LVDS_EVEN_POLARITY,
	/** 以下两个值直接写lvds的ctl和alg寄存器，一般情况下不要使用，主要用于调试阶段或者前面
	提供的参数不能满足配置需求时才使用，当以下两个参数设置后前面的参数自动视为无效*/
	LVDS_CTL_REG_VALUE,
	LVDS_ALG_REG_VALUE
};

#define EACH_INFO_MAX_LEN  25
static int s_config_matrix[LCD_MAX_INFO_NUM][EACH_INFO_MAX_LEN];
static const char * s_config_key_array[] = {
     "lcd.backlight",
     "lcd.base_info",
     "lcd.signal_info",
     "lcd.rgb_info",
     "lcd.lvds_info"
 };

static const char * s_config_key_array1[] = {
     "lcd1.backlight",
     "lcd1.base_info",
     "lcd1.signal_info",
     "lcd1.rgb_info",
     "lcd1.lvds_info"
 };
static int get_lcd_module_by_gpio(void) {
#ifdef ATM7029_10_B_SUMSUNG_CHIMEI
	int ret ;
	int pull_val = act_readl(PAD_PULLCTL1) & (1<<28);
	int gpioc9_stat = act_readl(GPIO_COUTEN) & (1 << 9);
	int gpioc9_value = act_readl(GPIO_CDAT) & (1 << 9);
	act_writel((act_readl(PAD_PULLCTL1) |(1 << 28)), PAD_PULLCTL1);
	act_writel((act_readl(GPIO_CDAT) & (~(1 << 9))), GPIO_CDAT);
	act_writel((act_readl(GPIO_COUTEN) & (~(1 << 9))), GPIO_COUTEN);
	mdelay(2);
	act_writel((act_readl(GPIO_CINEN) |(1 << 9)), GPIO_CINEN);
	mdelay(2);
	ret = act_readl(GPIO_CDAT) & (1 << 9);
	if(pull_val) {
       		act_writel(act_readl(PAD_PULLCTL1) | (1<<28), PAD_PULLCTL1);
	} else {
		act_writel(act_readl(PAD_PULLCTL1) & (~(1<<28)), PAD_PULLCTL1);
	}
	if(gpioc9_stat) {
		act_writel(act_readl(GPIO_COUTEN) | (1<<9), GPIO_COUTEN);
	} else {
		act_writel(act_readl(GPIO_COUTEN) & (~(1<<9)), GPIO_COUTEN);
	}
	if(gpioc9_value) {
		act_writel(act_readl(GPIO_CDAT) | (1<<9), GPIO_CDAT);
	} else {
		act_writel(act_readl(GPIO_CDAT) & (~(1<<9)), GPIO_CDAT);
	}
	act_writel((act_readl(GPIO_CINEN) & (~(1 << 9))), GPIO_CINEN);
	//#ifndef BUILD_IN_BOOT_LCD
	//	printk("================================= in get ret= %d\n",ret);
	//#endif
	return ret;
#else
	return 0;
#endif
}

static int s_gpio_status = -1;
static void config_array_init(void) {
	int i = 0, j = 0;
	//#ifndef BUILD_IN_BOOT_LCD
	//	printk("================================= in config_array_init  gpiostat=%d\n",s_gpio_status);
	//#endif
	if(s_gpio_status < 0) {
		s_gpio_status = get_lcd_module_by_gpio();
	}
	for(i = 0; i < LCD_MAX_INFO_NUM; i++) {
		for(j = 0; j < EACH_INFO_MAX_LEN; j++) {
			s_config_matrix[i][j] = -1;
		}
		if(s_gpio_status > 0) {
			//samsung lcd
			get_config(s_config_key_array1[i], (char*)s_config_matrix[i], EACH_INFO_MAX_LEN*sizeof(int)) ;
		} else {
			//chimei lcd
			get_config(s_config_key_array[i], (char*)s_config_matrix[i], EACH_INFO_MAX_LEN*sizeof(int)) ;
		}
	}
}

static int get_lcd_config(int info_index, int offset, int def) {
	static int s_config_have_inited = 0;
	if(!s_config_have_inited) {
		config_array_init();
		s_config_have_inited = 1;
	}
	
	if(info_index < LCD_MAX_INFO_NUM && offset < EACH_INFO_MAX_LEN) {
		int value = s_config_matrix[info_index][offset];
		if(value!= -1) {
			return value;
		} 
	}
    return def;
}

/********************************************************************gpio  config*********************************************************************/
enum ENUM_GPIO_NAME {
	GPIO_LCD_PWM,
	GPIO_LCD_VCC,
	GPIO_LCD_LED_EN,
	GPIO_LCD_MAX_NUM
};

static struct gpio_pre_cfg s_lcd_gpio_array[5];
static char * s_gpio_key_array[] = {
     "lcd_pwm",
     "lcd_vcc",
     "lcd_led_en"
 };

static char * s_gpio_key_array1[] = {
	"lcd_pwm1",
	"lcd_vcc",
	"lcd_led_en"
};

//用reserved[0]表示gpio是否已经调用gpio_request获取过，避免反复获取
static void gpio_array_init(void) {
	int i = 0;
	//	#ifndef BUILD_IN_BOOT_LCD
	//		printk("================================= in gpio_array_init  gpiostat=%d\n",s_gpio_status);
	//	#endif
     if(s_gpio_status < 0) {
        s_gpio_status = get_lcd_module_by_gpio();
    }
    for(i = 0; i < GPIO_LCD_MAX_NUM; i++) {
    	s_lcd_gpio_array[i].valid = 0;
    	s_lcd_gpio_array[i].reserved[0] = 0;
	if(s_gpio_status > 0) {
		//samsung lcd
    	    gpio_get_pre_cfg(s_gpio_key_array1[i], &s_lcd_gpio_array[i]);
	} else {
		//chimei lcd
	    gpio_get_pre_cfg(s_gpio_key_array[i], &s_lcd_gpio_array[i]);
	}
    }
}

static struct gpio_pre_cfg* get_lcd_gpio_cfg(enum ENUM_GPIO_NAME gpio) {
	static int s_gpio_have_inited = 0;
	if(!s_gpio_have_inited) {
		gpio_array_init();
		s_gpio_have_inited = 1;
	}
	
	if(gpio < GPIO_LCD_MAX_NUM) {
		return &s_lcd_gpio_array[gpio];
	}
	return 0;	
}

//value: 0, output low voltage; 1, output hight voltage; -1, stop output
static void set_gpio_value(int io_group, int pin_num, int put_way, int value) {
	int group_offset = io_group * 0x0c;
	int gpio_pos_reg,  gpio_neg_reg;
	
	if(put_way == 0) {
		gpio_pos_reg = GPIO_AOUTEN;
		gpio_neg_reg = GPIO_AINEN;
	} else {
		gpio_pos_reg = GPIO_AINEN;
		gpio_neg_reg = GPIO_AOUTEN;
	}
	
	if(value >= 0) {
	    act_writel(act_readl(gpio_neg_reg + group_offset) & ~(1 << pin_num) , gpio_neg_reg + group_offset);
        act_writel(act_readl(gpio_pos_reg + group_offset) | (1 << pin_num),  gpio_pos_reg + group_offset);
        if(value > 0) {
	        act_writel(act_readl(GPIO_ADAT + group_offset) | (1 << pin_num),  GPIO_ADAT + group_offset);
	    } else {
	    	act_writel(act_readl(GPIO_ADAT + group_offset) & (~(1 << pin_num)),  GPIO_ADAT + group_offset);
	    }
	  } else {
	  	act_writel(act_readl(GPIO_ADAT + group_offset) & (~(1 << pin_num)),  GPIO_ADAT + group_offset);
	  	act_writel(act_readl(gpio_pos_reg + group_offset) & (~(1 << pin_num)),  gpio_pos_reg + group_offset);
	  }
}

//通过直接设置相关寄存器的方式控制gpio, active: 0,设置gpio状态为unactive;  1,设置gpio状态为active
static void direct_set_lcd_gpio_active(struct gpio_pre_cfg* pcfg, int active) {
	int set_value;
	if(pcfg == 0) {
		return;
	}
	if(active >= 0) {
	    set_value = active ? pcfg->active_level : pcfg->unactive_level;
	} else {
		set_value = -1;
	}
	
	if(!pcfg->valid) {
		return;
	}
	set_gpio_value(pcfg->iogroup, pcfg->pin_num, pcfg->gpio_dir, set_value);
}

#ifndef BUILD_IN_BOOT_LCD
//通过调用内核相关接口的方式控制gpio, active: 0,设置gpio状态为unactive;  1,设置gpio状态为active
static void set_lcd_gpio_active(struct gpio_pre_cfg* pcfg, int active) {
	int pin = ASOC_GPIO_PORT(pcfg->iogroup, pcfg->pin_num);
	int set_value = active ? pcfg->active_level : pcfg->unactive_level;
	if(pcfg == NULL) {
		return;
	}
	
	if(!pcfg->valid) {
		return;
	}
	
	if(pcfg->reserved[0] == 0) {
			gpio_request(pin, NULL);
			pcfg->reserved[0] = 1;
	}
	if(pcfg->gpio_dir == 0) {
		 gpio_direction_output(pin, set_value);
	} else if(pcfg->gpio_dir == 2) {
		 gpio_direction_input(pin);
	}	
}

static void free_lcd_gpio(struct gpio_pre_cfg* pcfg) {
	if(pcfg == NULL) {
		return;
	}
	if(pcfg->reserved[0]) {
		int pin = ASOC_GPIO_PORT(pcfg->iogroup, pcfg->pin_num);
        gpio_free(pin);
        pcfg->reserved[0] = 0;
      }
}

extern int hibernate_reg_setmap(unsigned int reg, unsigned int bitmap);
static void quickboot_not_control_gpio(struct gpio_pre_cfg* pcfg) {
	hibernate_reg_setmap(pcfg->iogroup*12 + GPIO_ADAT, 1 << pcfg->pin_num);
	hibernate_reg_setmap(pcfg->iogroup*12 + GPIO_AOUTEN, 1 << pcfg->pin_num);
	hibernate_reg_setmap(pcfg->iogroup*12 + GPIO_AINEN, 1 << pcfg->pin_num);
}

#endif

#endif
