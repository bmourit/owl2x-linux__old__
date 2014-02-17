#ifndef __ACT_DE_CORE__H__
#define __ACT_DE_CORE__H__

#include <linux/notifier.h>

#include "hardware.h"
#include "atm702x_reg.h"
#include "atm702x_de.h"
#include "atm702x_cmu.h"
#include "display.h"

#if 0
#define DEDBG(format, ...) printk("ACT_DE: " format, ## __VA_ARGS__)
#else
#define DEDBG(format, ...)
#endif

#define NO_BOOT_DISP  0

#define DE_IRQ_LINE_RAM_UNDERFLOW		(1 << 0)
#define DE_IRQ_GFX_FIFO_UNDERFLOW		(1 << 1)
#define DE_IRQ_OUT_FIFO_UNDERFLOW		(1 << 2)
#define DE_IRQ_SCL_PRELINE_COMPLETE		(1 << 3)
#define DE_IRQ_GFX_PRELINE_COMPLETE	    (1 << 4)
#define DE_IRQ_GRA_BUSY         		(1 << 5)
#define DE_IRQ_SCL_BUSY	                (1 << 6)

#define DE_SCLCOEF_NORMAL       0x10
#define DE_SCLCOEF_DELHIGH      0x11

#define DECLK_SRC_DISPLAY_PLL		0
#define DECLK_SRC_DEVSCLK			  1

#define NEED_XFLIP    1
#define NEED_YFLIP    2


#define REG_WRITE 0
#define REG_BAKUP 1

struct de_reg_bak_t {
	
	unsigned int CMU_DECLK_bak;	
	
	unsigned int PATH1_CTL_bak;
	unsigned int PATH1_EN_bak;
	unsigned int PATH1_BK_bak;
	unsigned int PATH1_SIZE_bak;
	unsigned int PATH1_A_COOR_bak;
	unsigned int PATH1_B_COOR_bak;
	unsigned int PATH1_ALPHA_bak;
	unsigned int PATH1_CKMAX_bak;
	unsigned int PATH1_CKMIN_bak;

	unsigned int PATH2_CTL_bak;
	unsigned int PATH2_EN_bak;
	unsigned int PATH2_BK_bak;
	unsigned int PATH2_SIZE_bak;
	unsigned int PATH2_A_COOR_bak;
	unsigned int PATH2_B_COOR_bak;
	unsigned int PATH2_ALPHA_bak;
	unsigned int PATH2_CKMAX_bak;
	unsigned int PATH2_CKMIN_bak;
	
	unsigned int GRAPHIC_CFG_bak;
	unsigned int GRAPHIC_SIZE_bak;
	unsigned int GRAPHIC_FB_bak;
	unsigned int GRAPHIC_STR_bak;				

	unsigned int VIDEO1_CFG_bak;
	unsigned int VIDEO1_ISIZE_bak;
	unsigned int VIDEO1_OSIZE_bak;
	unsigned int VIDEO1_SR_bak;
	unsigned int VIDEO1_SCOEF0_bak;
	unsigned int VIDEO1_SCOEF1_bak;
	unsigned int VIDEO1_SCOEF2_bak;
	unsigned int VIDEO1_SCOEF3_bak;
	unsigned int VIDEO1_SCOEF4_bak;
	unsigned int VIDEO1_SCOEF5_bak;
	unsigned int VIDEO1_SCOEF6_bak;
	unsigned int VIDEO1_SCOEF7_bak;	
	unsigned int VIDEO1_FB_0_bak;
	unsigned int VIDEO1_FB_1_bak;
	unsigned int VIDEO1_FB_2_bak;
	unsigned int VIDEO1_FB_RIGHT_0_bak;
	unsigned int VIDEO1_FB_RIGHT_1_bak;
	unsigned int VIDEO1_FB_RIGHT_2_bak;
	unsigned int VIDEO1_STR_bak;
	unsigned int VIDEO1_CRITICAL_bak;
	unsigned int VIDEO1_REMAPPING_bak;
	
	unsigned int VIDEO2_CFG_bak;
	unsigned int VIDEO2_ISIZE_bak;
	unsigned int VIDEO2_OSIZE_bak;
	unsigned int VIDEO2_SR_bak;
	unsigned int VIDEO2_SCOEF0_bak;
	unsigned int VIDEO2_SCOEF1_bak;
	unsigned int VIDEO2_SCOEF2_bak;
	unsigned int VIDEO2_SCOEF3_bak;
	unsigned int VIDEO2_SCOEF4_bak;
	unsigned int VIDEO2_SCOEF5_bak;
	unsigned int VIDEO2_SCOEF6_bak;
	unsigned int VIDEO2_SCOEF7_bak;	
	unsigned int VIDEO2_FB_0_bak;
	unsigned int VIDEO2_FB_1_bak;
	unsigned int VIDEO2_FB_2_bak;
	unsigned int VIDEO2_FB_RIGHT_0_bak;
	unsigned int VIDEO2_FB_RIGHT_1_bak;
	unsigned int VIDEO2_FB_RIGHT_2_bak;
	unsigned int VIDEO2_STR_bak;
	unsigned int VIDEO2_CRITICAL_bak;
	unsigned int VIDEO2_REMAPPING_bak;
	
	unsigned int VIDEO3_CFG_bak;
	unsigned int VIDEO3_ISIZE_bak;
	unsigned int VIDEO3_OSIZE_bak;
	unsigned int VIDEO3_SR_bak;
	unsigned int VIDEO3_SCOEF0_bak;
	unsigned int VIDEO3_SCOEF1_bak;
	unsigned int VIDEO3_SCOEF2_bak;
	unsigned int VIDEO3_SCOEF3_bak;
	unsigned int VIDEO3_SCOEF4_bak;
	unsigned int VIDEO3_SCOEF5_bak;
	unsigned int VIDEO3_SCOEF6_bak;
	unsigned int VIDEO3_SCOEF7_bak;	
	unsigned int VIDEO3_FB_0_bak;
	unsigned int VIDEO3_FB_1_bak;
	unsigned int VIDEO3_FB_2_bak;
	unsigned int VIDEO3_FB_RIGHT_0_bak;
	unsigned int VIDEO3_FB_RIGHT_1_bak;
	unsigned int VIDEO3_FB_RIGHT_2_bak;
	unsigned int VIDEO3_STR_bak;
	unsigned int VIDEO3_CRITICAL_bak;
	unsigned int VIDEO3_REMAPPING_bak;

};

int de_reg_resume(struct de_reg_bak_t * regs);
int de_reg_backup(struct de_reg_bak_t * regs);

#define DE_CLK_MAX      (160)
#define DE_CLK_MIN      (10)

#define ADVANCED_LINES 16

#define VSYNC_USING_IOCTL  0
#define VSYNC_USING_UEVENT 1

struct vsync_info {
	int vsync_type;
	int state;
	int counter;
	int updata_counter;
	wait_queue_head_t wait_vsync;
};

struct de_core;

struct clk_manager {
	bool need_reconfig;
	unsigned int src_clk_type;
	unsigned int src_clk;
	unsigned int master_disp_dev_clk;
	unsigned int clk_rate;
	void (* init)(struct clk_manager *);
	int (* congfig_clk)(struct de_core * core);
};

struct mdsb_manager {
	struct kobject kobj;
	int debug_mode;
	int cvbs_vr;
	int cvbs_hr;
	int cvbs_drop;
	int lcd_vr;
	int lcd_hr;
	int lcd_drop;
	int delay0;
	int delay1;
	int master_disp_w;
	int master_disp_h;
	int (* init)(struct mdsb_manager * mgr);
	int (* config_mdsb)(struct de_core * core);
	int (* update_mdsb)(struct de_core * core);

};
#define DISP_DEV_CHANGED 0X01
#define VIDEO_ON_SWITCHER 0X02
struct de_core {

	struct de_reg_bak_t de_reg_bak;

	struct device *pdev; // parent device

	struct device *dev; // my device
	
	struct bus_type *bus;

	struct mutex op_lock;	
	
	struct mutex ioctl_lock;	

	struct disp_manager disp_mgr;

	struct layer_manager layer_mgr[2];

	struct mdsb_manager mdsb_mgr;

	struct vsync_info v_vsync;

	struct clk_manager clk_mgr;
	
	bool poll_notifyer;
	
	int poll_notifyer_event;
	
	int is_init_boot;

};
extern struct de_core core;

void print_debug_info(void);

void act_write_in_cache(unsigned int value, unsigned int regaddr);
void act_reg_cache_flush(void);

void change_critical_signal(unsigned int displayer_id);

void de_hw_init(void);

void de_path_config(struct de_core *core);

void de_clk_init(struct clk_manager *clk);
void de_set_clk_div(unsigned int clk_type, unsigned int de_clk_div);

void de_setout_channel(struct de_core * core);

int de_set_global_alpha(u8 global_alpha,bool enable);

int de_set_color_key(int max,int min,int match,bool enable);

void de_set_screen_size(u16 width, u16 height);

void de_set_color_mode(void);

void de_set_preline(struct asoc_display_device * disp_dev);

void de_go(enum de_out_channel channel);

void de_set_background_color(struct rgb_color *bg_color);

void de_reset_de(void);

void de_disable(void);

void de_enable(void);

void de_modify_start(void);

void de_modify_end(void);

void de_set_layer_status(int layerid, bool enable);

int de_graphics_layer_init(struct act_layer * layer);
int de_graphics_layer_config(struct act_layer * layer);
int de_graphics_layer_update(struct act_layer * layer);
int de_video_layer_init(struct act_layer * layer);
int de_video_layer_config(struct act_layer *layer);

int de_video_layer_update(struct act_layer *layer);
void layers_manager_init(struct de_core * core);
void disp_manager_init(struct de_core * core);
int mdsb_manager_init(struct de_core *core);
void disp_manager_open_dev(int disp_dev_id);
int disp_manager_switch_displayer(struct de_core * core, int disp_dev_ids);
int gamma_ram_config(void);
struct bus_type *de_get_bus(void);

void asoc_display_update_status(struct asoc_display_device *disp_dev);

bool de_is_init_boot(void);
#endif
