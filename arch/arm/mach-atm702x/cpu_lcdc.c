/*
 * linux/drivers/video/display/gl5201_rgb_lcd.c
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
#include <linux/err.h>
#include <linux/cpufreq.h>
#include <linux/gpio.h>
#include <actions_soc.h>

#include "../atm702x_de/display.h"

#define DEFAULT_CLR        0x0066ff33

#ifdef CONFIG_GL5201_DEMO
#define LCD_PWM_PIN         ASOC_GPIO_PORTA(20)
#define LCD_DISP_PIN        ASOC_GPIO_PORTA(21)
#define LCD_RESET_PIN       ASOC_GPIO_PORTA(15)
#endif

#ifdef CONFIG_GL5201_EVB
#define LCD_PWM_PIN	        ASOC_GPIO_PORTA(20)
#define LCD_DISP_PIN	      ASOC_GPIO_PORTC(20)
#define LCD_RESET_PIN	      ASOC_GPIO_PORTC(20)
#endif

#ifdef CONFIG_GL5201_VERIFY
#define LCD_PWM_PIN	        ASOC_GPIO_PORTB(15)
#define LCD_DISP_PIN	      ASOC_GPIO_PORTA(28)
#define LCD_RESET_PIN	      ASOC_GPIO_PORTA(28)
#endif

#define LCDCLK_SRC_SCLK				    0
#define LCDCLK_SRC_CK_LCD_432M		1
#define LCDCLK_SRC_LCD_PLL			  2
#define LCDCLK_SRC_CK_YPBPR_X2		3

#define ROTATION_0 0
#define ROTATION_90 1
#define ROTATION_180 2
#define ROTATION_270 3
#if 1
#define LCD_DEBUG(format, ...) printk("CPU_LCD: " format, ## __VA_ARGS__)
#else
#define LCD_DEBUG(format, ...)
#endif

#define DATA_WIDTH 0
extern void set_cpu_lcd_clk(u32 source_clk_type,	unsigned int source_clk);
void print_debug_info_lcd(void) {
#if 0
	LCD_DEBUG("LCD_CTRL0 =%x\n",act_readl(LCD_CTL));
	LCD_DEBUG("LCD_SIZE =%x\n",act_readl(LCD_SIZE));
	LCD_DEBUG("LCD_COLOR =%x\n",act_readl(LCD_COLOR));
	LCD_DEBUG("LCD_STATUS =%x\n",act_readl(LCD_STATUS));
	LCD_DEBUG("MFP_CTL0 =%x\n",act_readl(MFP_CTL0));
	LCD_DEBUG("MFP_CTL1 =%x\n",act_readl(MFP_CTL1));
	LCD_DEBUG("LCD_CPUCOM =%x\n",act_readl(LCD_CPUCOM));
	LCD_DEBUG("LCD_CPUCON =%x\n",act_readl(LCD_CPUCON));
	LCD_DEBUG("CMU_COREPLL = 0x%x\n",act_readl(CMU_COREPLL));
#endif
}
const struct asoc_videomode cpu_lcd_modes[] = {
	[0] = {
		1,
		{
	    .name		= NULL,
			.refresh		= 60,
			.xres		= 240,
			.yres		= 320,
			.pixclock		= 250000,//30096,	//in pico second, 0.000 000 000 001s
			.left_margin	= 0, //u32 hbp = cur_mode->left_margin; 168
			.right_margin	= 0,	//u32 hfp = cur_mode->right_margin;204
			.upper_margin	= 0,	//u32 vbp = cur_mode->upper_margin;35
			.lower_margin	= 0,	//u32 vfp = cur_mode->lower_margin;44
			.hsync_len	= 0,	//u32 hspw = cur_mode->hsync_len;173
			.vsync_len	= 0, //	u32 vspw = cur_mode->vsync_len;20
			.sync		= 0,
			.vmode		= FB_VMODE_NONINTERLACED,
			.flag		= FB_MODE_IS_STANDARD,
			.vid		= 2,
		},

	},
};

struct window_information 
{
    unsigned int horizontal_begin;
    unsigned int horizontal_end;
    unsigned int vertical_begin;
    unsigned int vertical_end;
    unsigned int horizontal_start;
    unsigned int vertical_start;	
};
static struct window_information window_info = {0,0,0, 0,0,0}; 
struct gpio_pre_cfg gpio_lcd_pwm;
struct gpio_pre_cfg gpio_lcd_disp;
struct gpio_pre_cfg gpio_lcd_reset;

extern void atv600x_pwm_bl_disconnect(struct backlight_device *bl);

static int get_pin_count = 0;
/******************************************************************************/
/*!                    
* \par  Description:
*   将16bit数据, 转换成18bit. 18bit的bit0和bit9为空
* \param[in]   unsigned int: 16bit data
* \retval      unsigned int: 18bit data
*******************************************************************************/
#if ( (DATA_WIDTH == 1) || (DATA_WIDTH == 3) )
static unsigned int _data_16to18_bit( unsigned int data_16 )
{
    unsigned int data_18 = 0;
    
    data_18 = ((data_16&0xFF) << 1) | ((data_16&0xFF00) << 2);
    
    return data_18;
}
#endif
/******************************************************************************/
/*!                    
 * \par  Description:
 *   set IC as commond mode; but rs pin can be change;
 * \param[in]   rs: 设置RS信号的高低状态
 * \retval      cmd: 写入的数值
 * \note
 IC is commond mode; 
 RS is 0: 一般是lcd的cmd模式, 用于写寄存器的索引index
 RS is 1: 一般是lcd的val模式, 用于写寄存器的内容value
 ******************************************************************************/
void lcd_send_cmd(unsigned int rs, unsigned int cmd) {
	unsigned int tmp = 0;

	act_writel(cmd, LCD_CPUCOM);

	tmp = act_readl(LCD_CPUCON);

	if (0 == rs) {
		tmp &= ~(0x01 << 10); //RS low
	} else {
		tmp |= (0x01 << 10); //RS high,
	}

	tmp &= ~(0x03 << 8); //write cmd
	//TODO 是否启动
	tmp |= (0x01 << 0); //start transmission
	act_writel(tmp, LCD_CPUCON);
}
/******************************************************************************/
/*!                    
 * \par  Description:
 *   set IC as data mode; and rs pin is high state;
 * \param[in]   NULL
 * \retval      NULL
 * \note
 IC is data mode, 一般用于刷新全部屏幕颜色数据, colordata;
 ******************************************************************************/
void lcd_send_data(void) {
	unsigned int tmp = 0;

	tmp = act_readl(LCD_CPUCON);
	tmp |= (0x01 << 10); //RS high,

	tmp &= ~(0x03 << 8);
	tmp |= (0x02 << 8); //write data

	//TODO 是否启动
	tmp |= (0x01 << 0); //start transmission
	act_writel(tmp, LCD_CPUCON);
}
/******************************************************************************/
/*!                    
* \par  Description:
*       lcd模组的 GRAM access registor. 用于对GRAM进行读写
* \param[in]   NULL
* \retval      NULL
* \note
    也就是找到lcd模组的GRAM读写寄存器, 初始化给:colordata_reg_index
*******************************************************************************/
static void _prepare_for_colordata( void )
{
    unsigned int colordata_reg_index = 0x0202;
    
#if ( (DATA_WIDTH == 1) || (DATA_WIDTH == 3) )
    colordata_reg_index = _data_16to18_bit(colordata_reg_index);
#endif
    
    lcd_send_cmd(0, colordata_reg_index);
    
    udelay(1);
}


/******************************************************************************/
/*!                    
* \par  Description:
*       write date to lcd registor. 向lcd寄存器写数据
* \param[in]   unsigned int: reg_index
* \param[in]   unsigned int: reg_val
* \retval      NULL
* \note
    一般根据不同模组的写时序要求, 顺序先写入index, 再写入对应index的data
*******************************************************************************/
static void  _write_lcd_reg(unsigned int reg_index,  unsigned int reg_val)                  
{
#if ( (DATA_WIDTH == 1) || (DATA_WIDTH == 3) )
    reg_index = _data_16to18_bit(reg_index);
#endif
    lcd_send_cmd(0, reg_index);
    udelay(1);
    
    
#if ( (DATA_WIDTH == 1) || (DATA_WIDTH == 3) )
    reg_val = _data_16to18_bit(reg_val);
#endif
    lcd_send_cmd(1, reg_val);
    udelay(1);
}

/******************************************************************************/
/*!                    
* \par  Description:
*       set lcd increment to flush screen. 设置lcd刷屏方向
* \param[in]   rotation_e: rotation, 当前刷屏角度
* \retval      NULL
* \note
    根据spec, 找到模组的刷屏方向控制寄存器. 
*******************************************************************************/
static void _set_increment_direction(int rotation )
{
    switch (rotation)
    {
    case ROTATION_0:
        _write_lcd_reg(0x0036,  0x0048);
        break;
    case ROTATION_90:
        _write_lcd_reg(0x0036,  0x0008);  
        break;
     case ROTATION_180:
        _write_lcd_reg(0x0036,  0x0098);
        break;
     case ROTATION_270:
        _write_lcd_reg(0x0036,  0x0028);
        break;
    default:
        break;
    }
    return;
}

static void lcd_command(unsigned int reg_index)
{

#if ( (DATA_WIDTH == 1) || (DATA_WIDTH == 3) )
	reg_index = _data_16to18_bit(reg_index);
#endif
  lcd_send_cmd(0, reg_index);
  udelay(1);
	return;
}

static void lcd_data(unsigned int reg_val)
{
#if ( (DATA_WIDTH == 1) || (DATA_WIDTH == 3) )
	reg_val = _data_16to18_bit(reg_val);
#endif
  lcd_send_cmd(1, reg_val);
  udelay(1);
	return;
}
/******************************************************************************/
/*!                    
* \par  Description:
*       set lcd area. 设置lcd屏幕xy起始大小和刷屏起始点
* \param[in]   rotation_e: rotation, 当前刷屏角度
* \retval      NULL
* \note
    根据spec, 找到模组的xy大小控制寄存器. 
*******************************************************************************/
static void _set_window_area(int rotation)
{
    window_info.horizontal_begin = 0;
    window_info.horizontal_end   = cpu_lcd_modes[0].mode.yres - 1;
    window_info.vertical_begin   = 0;
    window_info.vertical_end     = cpu_lcd_modes[0].mode.xres - 1;
  
    switch (rotation)
    {
    case ROTATION_0:
        window_info.horizontal_start = window_info.horizontal_end;
        window_info.vertical_start   = window_info.vertical_begin;	
        break;        
    case ROTATION_90:
        window_info.horizontal_start = window_info.horizontal_end;
        window_info.vertical_start   = window_info.vertical_end;
        break;       
     case ROTATION_180:
        window_info.horizontal_start = window_info.horizontal_end;
        window_info.vertical_start   = window_info.vertical_begin;
        break;      
     case ROTATION_270:
        window_info.horizontal_start = window_info.horizontal_begin;
        window_info.vertical_start   = window_info.vertical_begin;
        break;                  
    default:
        break;
    }
    
    lcd_command(0x002a);
    lcd_data(((unsigned int)window_info.vertical_begin)>>8);
    lcd_data(((unsigned int)window_info.vertical_begin)&0xff);
    lcd_data(((unsigned int)window_info.vertical_end)>>8);
    lcd_data(((unsigned int)window_info.vertical_end)&0xff);
    
    lcd_command(0x002b);    //window  GRAM
    lcd_data(((unsigned int)window_info.horizontal_begin)>>8);
    lcd_data(((unsigned int)window_info.horizontal_begin)&0xff);
    lcd_data(((unsigned int)window_info.horizontal_end)>>8);
    lcd_data(((unsigned int)window_info.horizontal_end)&0xff);


          	
    //_write_lcd_reg(0x0200,  window_info.horizontal_start); 
    //_write_lcd_reg(0x0021,  window_info.vertical_start);   
     	
    return;
}
void lcd_wait_complete_send_data(void) {
	unsigned int tmp = 0;
	int times = 3000; //推荐30ms, 实际*10的保险系数
	do {
		mdelay(1);
		tmp = (0x01 & (act_readl(LCD_CPUCON)));
		times--;
	} while ((tmp != 0) && (times > 0));

	if (times <= 0) {
		printk("last. lcd_xy_scan_location = 0x%x\n",
				act_readl(LCD_STATUS) & 0xfffff);
		printk("\nerr!! lcd wait LCD_CPUCON bit0, to transmit timeout\n");
	}
}
/******************************************************************************/
/*!          
 * \par  Description:
 *       获取cpu屏复用pin脚
 * \param[in]   NULL
 * \return      result 0:success; <0:fail
 * \retval      
 ******************************************************************************/
int lcd_get_pin(void) {
	int result = 0;

	get_pin_count++;

	if (get_pin_count > 1) {
		return 0;
	}
	result = asoc_mfp_lock(MOD_ID_LCD, MFP_OPT_CAN_SLEEP, NULL);

	return result;
}
/******************************************************************************/
/*!          
 * \par  Description:
 *       释放cpu屏复用pin脚
 * \param[in]   NULL
 * \return      result 0:success; <0:fail
 * \retval      
 ******************************************************************************/
int lcd_release_pin(void) {
	int result = 0;

	//为了测试当pin脚释放时是否已经刷完一屏数据
	unsigned int tmp = 0;

	tmp = act_readl(LCD_STATUS) & (0x0fffff);

	if (tmp != 0) {
		printk("LCD_DIRVER CX=%d,CY=%d\n", (tmp & 0xff800) >> 11, tmp & 0x07ff); //CX:19~10,CY:9~0
	}
	get_pin_count--;

	if (get_pin_count < 0) {
		get_pin_count = 0;
		return 0;
	}

	result = asoc_mfp_unlock(MOD_ID_LCD, MFP_OPT_CAN_SLEEP);

	return result;
}

int lcd_init( void )
{
		int i ;
    LCD_DEBUG("cpu lcd init\n");
    //lcd_get_pin();
    for(i = 0 ; i < 2 ; i++){
    	if(i == 0){
    		  act_writel((act_readl(LCD_CPUCON)& (~(1 << 11))),LCD_CPUCON);
    		  printk("init lcd 0 \n ");
    	}else{
    			act_writel((act_readl(LCD_CPUCON) | (1 << 11)),LCD_CPUCON);
    			printk("init lcd 1 \n ");
    	}
    	mdelay(30);
	    lcd_command(0x0011); //Exit Sleep
	    mdelay(100);  
	
	   	lcd_command(0xCB); //AP[2:0]
	   	
			lcd_data(0x01);
			lcd_command(0xC0); //Power control
			lcd_data(0x26);//VRH[5:0]
			lcd_data(0x01);//VC[2:0]
			lcd_command(0xC1); //Power control
			lcd_data(0x0010);//SAP[2:0];BT[3:0]
			lcd_command(0xC5); //VCM control
			lcd_data(0x0029);
			lcd_data(0x003A);
			lcd_command(0x26); //Gamma curve selected
			lcd_data(0x01);
			lcd_command(0xE0); //Set Gamma
			lcd_data(0x10);
			lcd_data(0x10);
			lcd_data(0x10);
			lcd_data(0x08);
			lcd_data(0x0E);
			lcd_data(0x06);
			lcd_data(0x42);
			lcd_data(0x28);
			lcd_data(0x36);
			lcd_data(0x03);
			lcd_data(0x0E);
			lcd_data(0x04);
			lcd_data(0x13);
			lcd_data(0x0E);
			lcd_data(0x0C);
			
			lcd_command(0XE1); //Set Gamma
			lcd_data(0x0C);
			lcd_data(0x23);
			lcd_data(0x26);
			lcd_data(0x04);
			lcd_data(0x0C);
			lcd_data(0x04);
			lcd_data(0x39);
			lcd_data(0x24);
			lcd_data(0x4B);
			lcd_data(0x03);
			lcd_data(0x0B);
			lcd_data(0x0B);
			lcd_data(0x33);
			lcd_data(0x37);
			lcd_data(0x0F);
			
			lcd_command(0x3a);
			lcd_data(0x55); //16-bit
			
			lcd_command(0x36);
			lcd_data(0x48);
	
	
	    lcd_command(0x00c7);
	    lcd_data(0x00BD);
	
			
			lcd_command(0x29); //display on
			
			lcd_command(0x002a);    //window  GRAM
	    lcd_data(0x0000);
	    lcd_data(0x0000);
	    lcd_data(0x0000);
	    lcd_data(0x00ef);
	
	    lcd_command(0x002b);
	    lcd_data(0x0000);
	    lcd_data(0x0000);
	    lcd_data(0x0001);
	    lcd_data(0x003f);
	
	    lcd_command(0x002c);    //提交
    }
    
   
    
    //lcd_release_pin();
    
    return 0;
}

void lcd_open(void) {	
	
	unsigned int tmp = 0;
	
	u32 data_width = 2;
	
	u32 xres = cpu_lcd_modes[0].mode.xres;
	u32 yres = cpu_lcd_modes[0].mode.yres; 


	if (asoc_mfp_lock(MOD_ID_LCD, MFP_OPT_CAN_SLEEP, NULL) == 0) {
		LCD_DEBUG("~~is OK to lock mfp for module LCD(ID: %d)\n",	MOD_ID_LCD);
	}	
	
	tmp = act_readl(MFP_CTL1);
	tmp = tmp & (~( MFP_CTL1_LCDD23|MFP_CTL1_LCDD19_10_1_0|
					MFP_CTL1_LCDD9_8(7) |MFP_CTL1_LCDD7_4(7) |
					MFP_CTL1_LCDD3_2(3)));
	tmp |= (1 << 27);
	act_writel(tmp,MFP_CTL1);
	
	/*set cpu lcd clk*/
	set_cpu_lcd_clk(LCDCLK_SRC_LCD_PLL, 0);
  //act_writel(0x0020b207, CMU_LCDCLK);//0x0020b207
  act_writel(act_readl(CMU_DEVCLKEN0) | (0x1<<3), CMU_DEVCLKEN0);
	act_readl(CMU_DEVCLKEN0); //flush...
	
	/* enhance lcd pad drv */
	tmp = act_readl(PAD_DRV1);
  tmp |= (0xf<<22);//Set the double driver to level 2
  act_writel(tmp, PAD_DRV1);
  
  //lcd control block reset
  tmp =act_readl(CMU_DEVRST0);
  tmp = tmp & (~(0x1<<28)); 
  act_writel(tmp, CMU_DEVRST0);    
  tmp = tmp | (0x1<<28); 
  act_writel(tmp, CMU_DEVRST0);    
    
  /*设置size*/
	tmp = act_readl(LCD_SIZE);
	tmp = tmp & (~(0x7ff << 16)) & (~(0x7ff));
	tmp = tmp | ((yres - 1) << 16) | (xres - 1);
	act_writel(tmp, LCD_SIZE);  
	printk("LCD_SIZE = %x \n",act_readl(LCD_SIZE));  

  // default color
  tmp = act_readl(LCD_COLOR);
  tmp = (tmp & ~0x00ffffffU) | (DEFAULT_CLR & 0x00ffffffU);
  act_writel(tmp, LCD_COLOR);
  
  /*设置data width*/
  tmp = act_readl(LCD_CPUCON);
  tmp &= ~(0x07<<4);
  tmp |= (DATA_WIDTH<<4);
  act_writel(tmp, LCD_CPUCON);

  /*设置lcd ctl*/
  tmp = act_readl(LCD_CTL);
  tmp &= (~(0x7<<16))&(~(0x3<<6));
  tmp |= (0x1<<31)|(0x2<<16)|(0x2<<6)|(0x01); //cpu if, video from de,disable lcdc
  act_writel(tmp, LCD_CTL);
 


#ifdef CONFIG_GL5201_DEMO
	LCD_DEBUG(KERN_ALERT "lcd pre reset, ADAT = 0x%x\n",act_readl(0xb01c0008));
	__gpio_set_value(LCD_RESET_PIN, gpio_lcd_reset.active_level);
	msleep(100);
	__gpio_set_value(LCD_RESET_PIN, gpio_lcd_reset.unactive_level);
	LCD_DEBUG(KERN_ALERT "lcd pre reset, ADAT = 0x%x\n",act_readl(0xb01c0008));
#endif
	__gpio_set_value(LCD_DISP_PIN, gpio_lcd_disp.active_level);

	printk("reset value = %x",gpio_get_value(LCD_DISP_PIN));	
	
	lcd_init();
	
	print_debug_info_lcd();
}

void lcd_close(void) {
	int tmp;

	__gpio_set_value(LCD_DISP_PIN, gpio_lcd_disp.unactive_level);
	
	tmp = act_readl(LCD_CTL);
	tmp &= (~0x01);
	act_writel(tmp, LCD_CTL);
	act_writel(0x0, CMU_LCDCLK); /*pull down dclk*/
	msleep(10);
	act_writel(act_readl(CMU_DEVCLKEN0) & ~(0x1 << 3), CMU_DEVCLKEN0);
	lcd_release_pin();
}
/******************************************************************************/
/*!                    
 * \par  Description:
 *   wait send data complete, to check if screen update finished
 * \param[in]   NULL
 * \retval      NULL
 ******************************************************************************/

struct gl5201_cpu_lcd {
	const struct asoc_videomode *modes;
	u32 num_modes;
	u32 data_width;
	u32 dclk_inversion;
	u32 lde_inversion;
	u32 source_clk_type;
	int enable_state;
	struct backlight_device *bl_dev;
};

static void connect_backlight_device(struct gl5201_cpu_lcd *gl5201_cpu_lcd) {
	gl5201_cpu_lcd->bl_dev = atv5201_pwm_bl_connect();
}

static void disconnect_backlight_device(struct gl5201_cpu_lcd *gl5201_cpu_lcd) {
	if (gl5201_cpu_lcd->bl_dev == NULL)
		return;

	atv5201_pwm_bl_disconnect(gl5201_cpu_lcd->bl_dev);
	gl5201_cpu_lcd->bl_dev = NULL;
}

static void set_bl_power(struct gl5201_cpu_lcd *gl5201_cpu_lcd, int power) {
	if (gl5201_cpu_lcd->bl_dev == NULL)
		return;

	gl5201_cpu_lcd->bl_dev->props.power = power;
	backlight_update_status(gl5201_cpu_lcd->bl_dev);
}

void set_cpu_lcd_clk(u32 source_clk_type,
		unsigned int source_clk) {
	//	struct gl5201_cpu_lcd_data *cpu_lcd_par = dev_get_drvdata(&disp_dev->dev);
	//nst struct fb_videomode *cur_mode = disp_dev->disp_cur_mode;

	unsigned int lcd_div;
	unsigned int pixclock_hz;
	u32 tmp;

	pixclock_hz = PICOS2KHZ(cpu_lcd_modes[0].mode.pixclock) * 1000;

	tmp = act_readl(CMU_LCDCLK);
	tmp &= (~CMU_LCDCLK_CLKSEL(3));
	tmp &= (~CMU_LCDCLK_LCDDIV(0x7f));

	switch (source_clk_type) {
#ifdef LCDCLK_SRC_SCLK
	case LCDCLK_SRC_SCLK:
		if (source_clk == 0)
			source_clk = asoc_get_sclk();

		tmp |= CMU_LCDCLK_CLKSEL(0);

		break;
#endif
#ifdef LCDCLK_SRC_CK_LCD_432M
	case LCDCLK_SRC_CK_LCD_432M:
		BUG();
		break;
#endif

#ifdef LCDCLK_SRC_LCD_PLL
	case LCDCLK_SRC_LCD_PLL:
		/*set LCD_PLL = 432M, need to be changed later*/
		source_clk = 432000000;
		tmp = 0x3210200;
		break;
#endif

#ifdef LCDCLK_SRC_CK_YPBPR_X2
	case LCDCLK_SRC_CK_YPBPR_X2:
		BUG();
		break;
#endif
	default:
		BUG();
		break;

	}
	LCD_DEBUG("source_clk_type=0x%x\n", source_clk_type);
	lcd_div = source_clk / (pixclock_hz);
	lcd_div = (lcd_div) ? lcd_div : 1;
	tmp |= (CMU_LCDCLK_LCDDIV(lcd_div - 1));
	act_writel(tmp, CMU_LCDCLK);//0x0020b207
	act_writel(act_readl(CMU_DEVCLKEN0) | (0x1 << 3), CMU_DEVCLKEN0);
	LCD_DEBUG("CMU_LCDCLK=0x%x,lcd_div=%d,pixclock_hz=%d\n", tmp, lcd_div,
			pixclock_hz);

}

static int gl5201_cpu_lcd_update_frame(struct asoc_display_device *disp_dev) {
	unsigned int rotation  = ROTATION_180;
	if(disp_dev->display_id == LCD_DISPLAYER){
		LCD_DEBUG("~~~~~~~~lcd 0 update ~~~~~~~~~~~\n");
		act_writel((act_readl(LCD_CPUCON)& (~(1 << 11))),LCD_CPUCON);
		disp_dev->display_id = LCD_DISPLAYER1;
	}else{
		LCD_DEBUG("~~~~~~~~lcd 1 update ~~~~~~~~~~~\n");
		act_writel((act_readl(LCD_CPUCON) |(1 << 11)),LCD_CPUCON);
		disp_dev->display_id = LCD_DISPLAYER;
	}
	_set_increment_direction( rotation );
		 
	_set_window_area( rotation ); 	      
	//_prepare_for_colordata();
      	
	lcd_send_cmd(0, 0x2c);  
  mdelay(1);    
	lcd_send_data();	
  mdelay(1);  
  lcd_wait_complete_send_data(); 
	print_debug_info_lcd();
	return 0;
}

static int gl5201_cpu_lcd_update_mode(struct asoc_display_device *disp_dev) {
	struct gl5201_cpu_lcd *gl5201_cpu_lcd = dev_get_drvdata(&disp_dev->dev);
	LCD_DEBUG("gl5201_cpu_lcd_update_mode called: %x \n",
			gl5201_cpu_lcd->enable_state);

	if (gl5201_cpu_lcd->enable_state == 1) {
		set_bl_power(gl5201_cpu_lcd, FB_BLANK_POWERDOWN);
		lcd_close();
		lcd_open();
		msleep_interruptible(100);
		set_bl_power(gl5201_cpu_lcd, FB_BLANK_UNBLANK);
	}
	return 0;
}

static int gl5201_cpu_lcd_update_status(struct asoc_display_device *disp_dev) {
	struct gl5201_cpu_lcd *gl5201_cpu_lcd = dev_get_drvdata(&disp_dev->dev);
	int state = disp_dev->state;

	if (disp_dev->power != FB_BLANK_UNBLANK)
		state = 0;
		
	if(disp_dev->fb_blank != FB_BLANK_UNBLANK)
	 state = 0;
	
	if (state == gl5201_cpu_lcd->enable_state)
		return 0;
	else
		gl5201_cpu_lcd->enable_state = state;

	if (state == 0) {
		LCD_DEBUG("close cpu lcd\n");
		set_bl_power(gl5201_cpu_lcd, FB_BLANK_POWERDOWN);
		lcd_close();
	} else {
		LCD_DEBUG("open cpu lcd\n");
		lcd_open();
		mdelay(200);
		set_bl_power(gl5201_cpu_lcd, FB_BLANK_UNBLANK);
	}
	return 0;
}

static int gl5201_cpu_lcd_get_status(struct asoc_display_device *disp_dev) {
	return disp_dev->state;
}

static unsigned int gl5201_cpu_lcd_get_devclk_khz(
		struct asoc_display_device *disp_dev, unsigned int src_clk_khz,
		const struct fb_videomode *mode) {
	unsigned int lcd_div;
	unsigned int pixclock_khz;
	unsigned int lcd_src_clk_khz;
	const struct fb_videomode *lcd_mode;
	struct gl5201_cpu_lcd *gl5201_cpu_lcd = dev_get_drvdata(&disp_dev->dev);

	switch (gl5201_cpu_lcd->source_clk_type) {

#ifdef LCDCLK_SRC_SCLK
	case LCDCLK_SRC_SCLK:
		if (src_clk_khz == 0) {
			lcd_src_clk_khz = (asoc_get_sclk() / 1000);
		} else {
			lcd_src_clk_khz = src_clk_khz;
		}
		break;
#endif

#ifdef LCDCLK_SRC_LCD_PLL
	case LCDCLK_SRC_LCD_PLL:
		/*set LCD_PLL = 432M, need to be changed later*/
		lcd_src_clk_khz = 432000;
		break;
#endif

	default:
		BUG();
		return 0;

	}
	if (mode == NULL) {
		lcd_mode = disp_dev->disp_cur_mode;
	} else {
		lcd_mode = mode;
	}

	pixclock_khz = PICOS2KHZ(lcd_mode->pixclock);

	lcd_div = lcd_src_clk_khz / (pixclock_khz);
	lcd_div = (lcd_div) ? lcd_div : 1;
#if 1
	LCD_DEBUG("lcd_src_clk_khzclk %d\n", lcd_src_clk_khz);
	LCD_DEBUG("pixclock_khz %d\n", pixclock_khz);
	LCD_DEBUG("lcd_div %d\n", lcd_div);
#endif
	return (lcd_src_clk_khz / lcd_div);

}

static struct display_ops gl5201_cpu_lcd_ops = {
	.update_status = gl5201_cpu_lcd_update_status,
	.get_status = gl5201_cpu_lcd_get_status,
	.update_mode = gl5201_cpu_lcd_update_mode,
	.update = gl5201_cpu_lcd_update_frame,
	.get_devclk_khz = gl5201_cpu_lcd_get_devclk_khz,
	.get_tvoutpll1_khz = NULL,
};
static struct asoc_display_device *disp_dev[2];
static int gl5201_cpu_lcd_probe(struct platform_device *pdev) {
	struct gl5201_rgb_lcd_data *cpu_lcd_par = pdev->dev.platform_data;
	
	struct gl5201_cpu_lcd *gl5201_cpu_lcd;
	int display_id  , i ;

	int ret = 0;
	LCD_DEBUG("gl5201_cpu_lcd_probe called ,dirver install begin !\n");
	if (!cpu_lcd_par) {
		dev_err(&pdev->dev, "failed to find platform data\n");
		return -EINVAL;
	}	
		
	for(i = 0 ;i < 2; i++){
		gl5201_cpu_lcd = kzalloc(sizeof(struct gl5201_cpu_lcd), GFP_KERNEL);
		if (!gl5201_cpu_lcd)
			return -ENOMEM;
			
		gl5201_cpu_lcd->modes = cpu_lcd_modes;
		
		gl5201_cpu_lcd->num_modes = 1;
		
		gl5201_cpu_lcd->data_width = 16;
			
		gl5201_cpu_lcd->source_clk_type = LCDCLK_SRC_LCD_PLL;
		
		gl5201_cpu_lcd->enable_state = 0;
	
		if(i == 0){
			display_id = LCD_DISPLAYER;
			disp_dev[i] = asoc_display_device_register("lcd0", &pdev->dev,
				gl5201_cpu_lcd, display_id, gl5201_cpu_lcd->modes,
				gl5201_cpu_lcd->num_modes, &gl5201_cpu_lcd_ops);
		}else{
			display_id = LCD_DISPLAYER1;
			disp_dev[i] = asoc_display_device_register("lcd1", &pdev->dev,
				gl5201_cpu_lcd, display_id, gl5201_cpu_lcd->modes,
				gl5201_cpu_lcd->num_modes, &gl5201_cpu_lcd_ops);
		}	
	

	
		if (IS_ERR(disp_dev)) {
			dev_err(&pdev->dev, "failed to register asoc display\n");
			ret = PTR_ERR(disp_dev);
			goto err_bl;
		}
			
		disp_dev[i]->disp_cur_mode = &cpu_lcd_modes[0].mode;
		printk("disp_dev->disp_cur_mode = %p ---- %p",disp_dev[i]->disp_cur_mode,disp_dev[i]);
		disp_dev[i]->is_plugged = 1;
	
		disp_dev[i]->state = 0;
		disp_dev[i]->fb_blank = FB_BLANK_UNBLANK;
						
		connect_backlight_device(gl5201_cpu_lcd);
		
  }
	
	

	platform_set_drvdata(pdev, disp_dev);

	gpio_request(LCD_DISP_PIN, "lcd_disp");
#ifdef CONFIG_GL5201_DEMO
	gpio_request(LCD_RESET_PIN, "lcd_reset");
#endif    

	memset(&gpio_lcd_disp, 0, sizeof(gpio_lcd_disp));
#ifdef CONFIG_GL5201_DEMO
	memset(&gpio_lcd_reset, 0, sizeof(gpio_lcd_reset));
#endif    

	gpio_get_pre_cfg(MOD_ID_LCD, "lcd_disp", &gpio_lcd_disp);
#ifdef CONFIG_GL5201_DEMO
	gpio_get_pre_cfg(MOD_ID_LCD, "lcd_reset", &gpio_lcd_reset);
#endif  

	if (gpio_lcd_disp.gpio_dir == GPIOF_DIR_OUT){
		gpio_direction_output(LCD_DISP_PIN, gpio_lcd_disp.init_level);
	} else {
		gpio_direction_input( LCD_DISP_PIN);
	}

#ifdef CONFIG_GL5201_DEMO
	if (gpio_lcd_reset.gpio_dir == GPIOF_DIR_OUT){
		gpio_direction_output(LCD_RESET_PIN, gpio_lcd_reset.init_level);
	}	else {
		gpio_direction_input(LCD_RESET_PIN);
	}
#endif
	return ret;
err_bl: 
		return ret;
}

static int gl5201_cpu_lcd_remove(struct platform_device *pdev) {
	struct asoc_display_device *disp_dev = platform_get_drvdata(pdev);
	struct gl5201_cpu_lcd *gl5201_cpu_lcd = dev_get_drvdata(&disp_dev->dev);

	set_bl_power(gl5201_cpu_lcd, FB_BLANK_POWERDOWN);
	lcd_close();
	disconnect_backlight_device(gl5201_cpu_lcd);
	asoc_display_device_unregister(disp_dev);
	return 0;
}

#ifdef CONFIG_PM
static unsigned long GPIO_COUTEN_bak = 0;
static unsigned long GPIO_CDAT_bak = 0;
static unsigned long GPIO_CINEN_bak = 0;

static int gl5201_cpu_lcd_suspend(struct platform_device *pdev,
		pm_message_t state)
{
	struct asoc_display_device *disp_dev = platform_get_drvdata(pdev);
	struct gl5201_cpu_lcd *gl5201_cpu_lcd = dev_get_drvdata(&disp_dev->dev);
	LCD_DEBUG("gl5201_cpu_lcd_suspend called \n");
	if(disp_dev != NULL) {
		set_bl_power(gl5201_cpu_lcd,FB_BLANK_POWERDOWN);
		lcd_close();
	}
	GPIO_COUTEN_bak = act_readl(GPIO_COUTEN);
	GPIO_CDAT_bak = act_readl(GPIO_CDAT);
	GPIO_CINEN_bak = act_readl(GPIO_CINEN);
	return 0;
}

static int gl5201_cpu_lcd_resume(struct platform_device *pdev)
{
	act_writel(GPIO_COUTEN_bak,GPIO_COUTEN);
	act_writel(GPIO_CDAT_bak,GPIO_CDAT);
	act_writel(GPIO_CINEN_bak,GPIO_CINEN);
	return 0;
}
#endif

static struct platform_driver gl5201_cpu_lcd_driver = {
	.driver = {
		.name = "gl5201_rgb_lcd",
		.owner = THIS_MODULE,
	},
	.probe = gl5201_cpu_lcd_probe,
	.remove = gl5201_cpu_lcd_remove,
#ifdef CONFIG_PM
	.suspend = gl5201_cpu_lcd_suspend,
	.resume = gl5201_cpu_lcd_resume,
#endif
};

static int __init gl5201_cpu_lcd_init(void)
{
	return platform_driver_register(&gl5201_cpu_lcd_driver);
}

module_init( gl5201_cpu_lcd_init);

static void __exit gl5201_cpu_lcd_exit(void)
{
	platform_driver_unregister(&gl5201_cpu_lcd_driver);
}

module_exit( gl5201_cpu_lcd_exit);

MODULE_DESCRIPTION("gl5201 PWM based Backlight Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:gl5201_pwm_backlight");

