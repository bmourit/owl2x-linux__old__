#ifndef LCDC_H
#define LCDC_H

#include <linux/backlight.h>

#include <mach/atm702x_reg.h>
#include <mach/hardware.h>
#include <mach/atm702x_lcd.h>
#include <mach/gpio.h>

#include "../display/act_de/display.h"

#define DEFAULT_CLR		0x00000000
#define MAX_LCD_NUM		2

#define LCD_TYPE_CPU		0
#define LCD_TYPE_RGB		1
#define LCD_TYPE_LVDS		2

#define CLK_SRC_DISPLAY_PLL	0
#define CLK_SRC_DEVSCLK		1
#define CLK_SRC_TVOUT_PLL0	2

#if 0
#define LCD_DEBUG_PRINT(fmt, args...) printk(KERN_ALERT fmt, ##args)
#else
#define LCD_DEBUG_PRINT(fmt, args...)
#endif

struct act_pwm_bl {
	struct pwm_device *pwm;
	unsigned int period;
	unsigned int total_steps;
	unsigned int active_high;
	int (*notify) (int brightness);
};

struct lcd_gpio_info {
    struct gpio_pre_cfg* pwm_gpio;
    struct gpio_pre_cfg* lcd_vcc_gpio;
    struct gpio_pre_cfg* led_en_gpio;
};

struct lcd_lvds_info {
	int mapping;
	int mirror;
	int odd_polarity;
	int even_polarity;
	int lvds_ctl_reg_value;
	int lvds_alg_reg_value;
};

struct lcd_mdsb_info {
	int hr;
	int vr;
	int drop;
	int delay;
};

struct lcd_information {	
	u32 num_modes;
	struct de_video_mode modes[5];
	struct lcd_mdsb_info sync_info[4];
	u32 data_width;
	u32 vsync_inversion;
	u32 hsync_inversion;
	u32 dclk_inversion;
	u32 lde_inversion;
	struct lcd_gpio_info gpio_info;
	struct lcd_lvds_info lvds_info;
};

struct lcd_manager {
	int lcd_type;
	int display_id;
	int src_clk_type;
	int lcd_clk;
	int lcd_num;
	
	struct lcd_information lcd_info[MAX_LCD_NUM];
	struct asoc_display_device * disp_dev[MAX_LCD_NUM];	
	struct backlight_device *bl_dev;

	int (*lcd_init)(struct asoc_display_device *disp_dev);
	int (*lcd_close)(struct asoc_display_device *disp_dev);	
	int (*set_lcdc_clk)(struct asoc_display_device *disp_dev, int clock_src_type, int clk_rate);		
	int (*get_pin)(struct asoc_display_device *disp_dev);	
	int (*release_pin)(struct asoc_display_device *disp_dev);
	int (*get_mdsb_parameter)(struct asoc_display_device *disp_dev, int disp_mode, int * hr, int * vr, int * drop, int * delay);
};

int lcd_manager_init(struct lcd_manager * lcd_mgr);

#define GET_LCD_INFO(disp_dev) ((struct lcd_manager *)(disp_dev->disp_priv_info))

extern struct backlight_device *act_pwm_bl_connect(void);
extern void act_pwm_bl_disconnect(struct backlight_device *bl);
extern void set_lcd_resume_from_sleep(int flag);

#endif
