/* arch/arm/mach-atm702x/include/mach/clock.h
 * 
 * (C) Copyright 2011-2014
 * Actions Semiconductor, Inc. <www.actions-semi.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __ARM_ATM702X_CLOCK_H__
#define __ARM_ATM702X_CLOCK_H__

#include <linux/kernel>
#include <linux/list.h>

#include <linux/clkdev.h>
#include <linux/spinlock.h>
#include <mach/clock_tree.h>

/*atm702x cmu clocks*/
enum module_clk_id
{
    MODULE_CLK_RESERV0,
    MODULE_CLK_DMAC,
    MODULE_CLK_NOR,
    MODULE_CLK_DDRC,
    MODULE_CLK_NANDC,
    MODULE_CLK_SD0,
    MODULE_CLK_SD1,
    MODULE_CLK_SD2,
    MODULE_CLK_DE,
    MODULE_CLK_LCD0,
    MODULE_CLK_LCD1,
    MODULE_CLK_LVDS,
    MODULE_CLK_RESERV1,
    MODULE_CLK_CSI,
    MODULE_CLK_BISP,
    MODULE_CLK_RESERV2,
    MODULE_CLK_LENS,
    MODULE_CLK_KEY,
    MODULE_CLK_GPIO,
    MODULE_CLK_RESERV3,
    MODULE_CLK_I2STX,
    MODULE_CLK_I2SRX,
    MODULE_CLK_HDMIA,
    MODULE_CLK_SPDIF,
    MODULE_CLK_PCM0,
    MODULE_CLK_VDE,
    MODULE_CLK_VCE,
    MODULE_CLK_GPU2D,
    MODULE_CLK_SHARESRAM,
    MODULE_CLK_GPU2DLP,
    MODULE_CLK_GPU3D,
    MODULE_CLK_GPU3DLP,
    /*dev1*/
    MODULE_CLK_TVIN,
    MODULE_CLK_TV24M,
    MODULE_CLK_CVBS,
    MODULE_CLK_HDMI,
    MODULE_CLK_YPbPrVGA,
    MODULE_CLK_TVOUT,
    MODULE_CLK_UART0,
    MODULE_CLK_UART1,
    MODULE_CLK_UART2,
    MODULE_CLK_IRC,
    MODULE_CLK_SPI0,
    MODULE_CLK_SPI1,
    MODULE_CLK_SPI2,
    MODULE_CLK_SPI3,
    MODULE_CLK_I2C0,
    MODULE_CLK_I2C1,
    MODULE_CLK_PCM1,
    MODULE_CLK_RESERV4,
    MODULE_CLK_RESERV5,
    MODULE_CLK_UART3,
    MODULE_CLK_UART4,
    MODULE_CLK_UART5,
    MODULE_CLK_ETHERNET,
    MODULE_CLK_PWM0,
    MODULE_CLK_PWM1,
    MODULE_CLK_PWM2,
    MODULE_CLK_PWM3,
    MODULE_CLK_TIMER,
    MODULE_CLK_GPS,
    MODULE_CLK_RESERV6,
    MODULE_CLK_I2C2,
    MODULE_CLK_RESERV7,
    MODULE_CLK_MAX
};

enum module_reset_id
{
    MODULE_RESET_DMAC=0,
    MODULE_RESET_NORIF,
    MODULE_RESET_DDR,
    MODULE_RESET_NANDC,
    MODULE_RESET_SD0,
    MODULE_RESET_SD1,
    MODULE_RESET_PCM1,
    MODULE_RESET_DE,
    MODULE_RESET_LCD,
    MODULE_RESET_SD2,
    MODULE_RESET_RES0,
    MODULE_RESET_CSI,
    MODULE_RESET_BISP,
    MODULE_RESET_RES1,
    MODULE_RESET_KEY,
    MODULE_RESET_GPIO,
    MODULE_RESET_RES2,
    MODULE_RESET_AUDIO,
    MODULE_RESET_PCM0,
    MODULE_RESET_VDE,
    MODULE_RESET_VCE,
    MODULE_RESET_GPU2D,
    MODULE_RESET_GPU3D,
    MODULE_RESET_NIC301,
    MODULE_RESET_RES3,
    MODULE_RESET_RES4,
    MODULE_RESET_LENS,
    MODULE_RESET_PERIPHRESET,
    MODULE_RESET_RES5,
    MODULE_RESET_RES6,
    MODULE_RESET_RES7,
    MODULE_RESET_RES8,
    MODULE_RESET_TVIN,
    MODULE_RESET_TVOUT,
    MODULE_RESET_HDMI,
    MODULE_RESET_USB2_0,
    MODULE_RESET_USB2_1,
    MODULE_RESET_UART0,
    MODULE_RESET_UART1,
    MODULE_RESET_UART2,
    MODULE_RESET_SPI0,
    MODULE_RESET_SPI1,
    MODULE_RESET_SPI2,
    MODULE_RESET_SPI3,
    MODULE_RESET_I2C0,
    MODULE_RESET_I2C1,
    MODULE_RESET_USB3,
    MODULE_RESET_UART3,
    MODULE_RESET_UART4,
    MODULE_RESET_UART5,
    MODULE_RESET_I2C2,
    MODULE_RESET_RES9,
    MODULE_RESET_ETHERNET,
    MODULE_RESET_CHIPID,
    MODULE_RESET_RES10,
    MODULE_RESET_GPS,
    MODULE_RESET_RES11,
    MODULE_RESET_RES12,
    MODULE_RESET_WD2RESET,
    MODULE_RESET_WD1RESET,
    MODULE_RESET_WD0RESET,
    MODULE_RESET_DBG2RESET,
    MODULE_RESET_DBG1RESET,
    MODULE_RESET_DBG0RESET,
    MODULE_RESET_ID_MAX,
};

struct clk_ops {
	void          (*init)(struct clk *);
	int           (*enable)(struct clk *);
	int           (*disable)(struct clk *);
	int           (*set_parent)(struct clk *, struct clk *);
	struct clk*   (*get_parent)(struct clk *clk);
	int           (*set_rate)(struct clk *, unsigned long);
	long	      (*round_rate)(struct clk *, unsigned long);
	void		  (*reset)(struct clk *);
	unsigned long (*get_rate)(struct clk *clk);
	int           (*is_enabled)(struct clk *clk);
	unsigned long (*recalc)(struct clk *);
};

enum clk_state {
	UNINITIALIZED = 0,
	ON,
	OFF,
};

#define DIV_UART_SERIAL_MAX  312
#define DIV_UART_MAX         624
#define DIV_UART_SERIAL_VALUE_MAX (DIV_UART_SERIAL_MAX-1)
#define DIV_UART_VALUE_MAX   32

/**
 * struct clksel_rate - register bitfield values corresponding to clk divisors
 * @val: register bitfield value (shifted to bit 0)
 * @div: clock divisor corresponding to @val
 * @flags: (see "struct clksel_rate.flags possibilities" above)
 *
 * @val should match the value of a read from struct clk.clksel_reg
 * AND'ed with struct clk.clksel_mask, shifted right to bit 0.
 *
 * @div is the divisor that should be applied to the parent clock's rate
 * to produce the current clock's rate.
 */
struct clksel_rate {
	u32			val;
	u16			div;
	u16			flags;
};

struct clk_mux_sel {
	struct clk	*input;
	u32		value;
};

/*
 * struct clksel_rate.flags possibilities
 */
#define PLL_FIXED           (1<<0)
#define PARENT_FIXED        (1<<1)
#define ENABLE_ON_INIT		(1<<2)	/* Enable upon framework init */
#define DIV_UART            (1<<3)
#define DIV_ROUND_1_1024    (1<<4) 
#define DIV_FIXED           (1<<5) /*div is fixed at init*/
#define TYPE_GPU            (1<<6) /*GPU Clock*/
#define PLL_ALAWAY_ON       (1<<7)

struct clk
{
	const char *name;
	const char *alias;
	unsigned int module_id;
	unsigned int reset_id;

    struct clk_lookup	lookup;
	struct clk_ops* ops;
	struct clk* parent;


	unsigned long		max_rate;
	unsigned long		min_rate;

    /*clk selector*/
	u32             clksel_reg;
	u32             clksel_mask;
	u32             div_mask;
	struct clk_mux_sel	*clksel;    
	struct clksel_rate *rates;

    /*flag*/
	u32	flag;
	bool set;
	enum clk_state state;
	u32	refcnt;
	u32	div;
	u32 max_div;
	unsigned long rate;

    /*for clock tree*/
	struct list_head list;
	struct list_head children;  //the children list, TODO
	struct list_head sibling;	//node for children
    struct list_head shared_bus_list;  //for shared slaves

    union {
		struct {
			//to be externed!
		} periph;
		struct {
	    	u32                 enable_reg;
            u32                 enable_mask;    
            u32                 value_mask; 
		    unsigned long		max_rate;
	        unsigned long		min_rate;
	        u32                 rate_step;
			u32				    lock_delay; //time needed for pll to become stable
			unsigned long			fixed_rate; 
			unsigned long*       freq_point_array; //If freq point, you need to fill this array
		} pll;
		struct {
			struct list_head		node;
			bool				enabled;
			unsigned long			rate;
			unsigned int        priority;
		} shared_bus_user;
	} u;


#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs_entry;
	struct dentry *debugfs_parent_entry;
#endif /* end of CONFIG_DEBUG_FS */

    spinlock_t spinlock;
};

struct leopard_clk_init_table {
	const char *name;
	const char *parent;
	unsigned long rate;
	bool enabled;
};

int clk_is_enabled(struct clk *clk);
void clk_init(struct clk *clk);
void clk_reset(struct clk *c);
int clk_enable(struct clk *c);
void clk_disable(struct clk *c);
void clk_reparent(struct clk *child, struct clk *parent);
int clk_set_parent(struct clk *clk, struct clk *parent);
struct clk *clk_get_parent(struct clk *clk);
struct clk *clk_get_by_name(const char *name);
struct clk *clk_get_by_id(unsigned int id);
void propagate_rate(struct clk *tclk);
void recalculate_root_clocks(void);
int __init leopard_cmu_init_early(void);
__init int leopard_init_clocks(void);

extern int module_reset(enum module_reset_id reset_id);
//
//void atm702x_clock_suspend(void);
//void atm702x_clock_resume(void);

#ifdef CONFIG_DEBUG_FS
void atm702x_clock_debugfs_init(void);
void atm702x_clock_debugfs_add_clk(struct clk *clk);
void atm702x_clock_debugfs_update_parent(struct clk *clk);
#else /* !CONFIG_DEBUG_FS */
static inline void atm702x_clock_debugfs_init(void) {};
static inline void atm702x_clock_debugfs_add_clk(struct clk *clk) {};
static inline void atm702x_clock_debugfs_update_parent(struct clk *clk) {};
#endif /* end of CONFIG_DEBUG_FS */

/*clock name*/
#define CLK_NAME_LOSC               "losc"
#define CLK_NAME_HOSC               "hosc"
#define CLK_NAME_COREPLL            "corepll"
#define CLK_NAME_DEVPLL             "devpll"
#define CLK_NAME_NANDPLL            "nandpll"
#define CLK_NAME_DDRPLL             "ddrpll"
#define CLK_NAME_DISPLAYPLL         "displaypll"
#define CLK_NAME_AUDIOPLL           "audiopll"
#define CLK_NAME_TVOUT0PLL          "tvout0pll"
#define CLK_NAME_TVOUT1PLL          "tvout1pll"
#define CLK_NAME_DEEPCOLORPLL       "deepcolorpll"
#define CLK_NAME_120MPLL            "120Mpll"
#define CLK_NAME_CPUCLK             "cpuclk"
#define CLK_NAME_DEVCLK             "devclk"
#define CLK_NAME_HCLK               "hclk"
#define CLK_NAME_PCLK               "pclk"
#define CLK_NAME_NIC_DCU_CLK        "nic_dcu_clk"
#define CLK_NAME_LCD0_CLK           "lcd0_clk"
#define CLK_NAME_LCD1_CLK           "lcd1_clk"
#define CLK_NAME_BISP_CLK           "bisp_clk"
#define CLK_NAME_SENSOR_CLK         "sensor_clk"
#define CLK_NAME_TVOUTMUX0_CLK      "tvout_mux0_clk"
#define CLK_NAME_TVOUT0_CLK         "tvout0_clk"
#define CLK_NAME_I2STX_CLK          "i2stx_clk"
#define CLK_NAME_I2SRX_CLK          "i2srx_clk"
#define CLK_NAME_HDMI_CLK           "hdmia_clk"
#define CLK_NAME_SPDIF_CLK          "spdif_clk"
#define CLK_NAME_LENS_CLK           "lens_clk"
#define CLK_NAME_DDR_CLK            "ddr_clk"
#define CLK_NAME_PWM0_CLK           "pwm0_clk"
#define CLK_NAME_PWM1_CLK           "pwm1_clk"
#define CLK_NAME_PWM2_CLK           "pwm2_clk"
#define CLK_NAME_PWM3_CLK           "pwm3_clk"
#define CLK_NAME_IMG5_CLK           "img5_clk"
#define CLK_NAME_DMA_CLK            "dma_clk"
#define CLK_NAME_TWD_CLK            "smp_twd"
#define CLK_NAME_L2_CLK             "l2_clk"
#define CLK_NAME_HDMI24M_CLK        "hdmi24M_clk"
#define CLK_NAME_TIMER_CLK          "timer_clk"
#define CLK_NAME_IRC_CLK            "irc_clk"
#define CLK_NAME_I2C0_CLK           "i2c0_clk"
#define CLK_NAME_I2C1_CLK           "i2c1_clk"
#define CLK_NAME_I2C2_CLK           "i2c2_clk"
#define CLK_NAME_TV24M_CLK          "tv24M_clk"
#define CLK_NAME_PCM0_CLK           "pcm0_clk"
#define CLK_NAME_PCM1_CLK           "pcm1_clk"
#define CLK_NAME_LVDS_CLK           "lvds_clk"
#define CLK_NAME_VCE_CLK            "vce_clk"
#define CLK_NAME_VDE_CLK            "vde_clk"
#define CLK_NAME_GPU2D_CLK          "gpu2d_clk"
#define CLK_NAME_GPU2D_NIC_CLK      "gpu2d_nic_clk"
#define CLK_NAME_GPU3D_CLK          "gpu3d_clk"
#define CLK_NAME_GPU3D_NIC_CLK      "gpu3d_nic_clk"
#define CLK_NAME_NANDC_CLK          "nandc_clk"
#define CLK_NAME_NANDC_NIC_CLK      "nandc_nic_clk"
#define CLK_NAME_GPS2x_CLK          "gps2x_clk"
#define CLK_NAME_ETHERNET_CLK       "ethernet_nic_clk"
#define CLK_NAME_SD0_CLK            "sd0_clk"
#define CLK_NAME_SD1_CLK            "sd1_clk"
#define CLK_NAME_SD2_CLK            "sd2_clk"
#define CLK_NAME_UART0_CLK          "uart0_clk"
#define CLK_NAME_UART1_CLK          "uart1_clk"
#define CLK_NAME_UART2_CLK          "uart2_clk"
#define CLK_NAME_UART3_CLK          "uart3_clk"
#define CLK_NAME_UART4_CLK          "uart4_clk"
#define CLK_NAME_UART5_CLK          "uart5_clk"
#define CLK_NAME_LCD_CLK            "lcd_clk"
#define CLK_NAME_CSI_CLK            "csi_clk"
#define CLK_NAME_DE_CLK             "de_clk"
#define CLK_NAME_DE1_CLK            "de1_clk"
#define CLK_NAME_DE2_CLK            "de2_clk"
#define CLK_NAME_DE3_CLK            "de3_clk"
#define CLK_NAME_DE_WB_CLK          "de_wb_clk"
#define CLK_NAME_SENSOR_OUT0_CLK    "sensor_out0_clk"
#define CLK_NAME_SENSOR_OUT1_CLK    "sensor_out1_clk"
#define CLK_NAME_CVBS_CLK           "cvbs_clk"

#endif /* end of __ARM_ATM702X_CLOCK_H__ */
