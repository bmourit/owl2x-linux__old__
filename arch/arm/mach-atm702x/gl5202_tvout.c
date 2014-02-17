#include <linux/types.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/poll.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/unistd.h> 
#include <linux/clk.h>
#include <mach/gl5202_cmu.h>
#include <mach/actions_reg_gl5202.h>
#include <mach/clock.h>
#include <mach/hardware.h>
#include <mach/irqs.h>
#include "../display/act_de/display.h"
#include "../display/act_de/de.h"
#include "gl5202_tvout_reg.h"
#include "asoc_tvout.h"
#include "asoc_ioctl.h"

/*-----------------------MACROs for print--------------------------*/
#define ENABLE_CVBS_PLUGDETECT		0
#define TVOUT_DEBUG_WARNING			0
#define TVOUT_DEBUG_INFO			1
#if (TVOUT_DEBUG_INFO == 1)
	#define GL5201_TVOUT_INFO(fmt, args...)	\
	printk(KERN_INFO "gl5201_tvout_drv: " fmt, ##args)
#else
	#define GL5201_TVOUT_INFO(fmt, args...)
#endif

#if (TVOUT_DEBUG_WARNING == 1)
	#define GL5201_TVOUT_ERROR(fmt, args...)	\
	printk(KERN_WARNING "gl5201_tvout_drv: " fmt, ##args)
#else
	#define GL5201_TVOUT_ERROR(fmt, args...)
#endif

#define IC_DEBUG

/*-----------------------static variables--------------------------*/
 struct cvbs_manager * cvbs_data;
static DEFINE_MUTEX(cvbs_setting_mutex);
/* Wait queue used to inform HPG happened for poll */
DECLARE_WAIT_QUEUE_HEAD(cvbs_hpg_inform_queue);
/*-----------------------videomode-------------------------*/
static  struct de_video_mode cvbs_display_modes[] = {
	[TV_MODE_PAL] = {
		.name		= "CVBS-PAL",
		.refresh	= 50,
		.xres		= 720,
		.yres		= 576,
		.pixclock	= 74074,	/*pico second, 1.e-12s*/
		.hfp		= 132,
		.hbp		= 11,
		.vfp		= 22,
		.vbp		= 26,
		.hsw		= 1,
		.vsw		= 1,
		.vmode		= FB_VMODE_INTERLACED,
		.flag		= FB_MODE_IS_STANDARD,
		.vid		= TV_MODE_PAL,
	},

	[TV_MODE_NTSC] = {
		.name		= "CVBS-NTSC",
		.refresh	= 60,
		.xres		= 720,
		.yres		= 480,
		.pixclock	= 74074,	/*pico second, 1.e-12s*/
		.hfp		= 122,
		.hbp		= 15,
		.vfp		= 18,
		.vbp		= 26,
		.hsw		= 1,
		.vsw		= 1,
		.vmode		= FB_VMODE_INTERLACED,
		.flag		= FB_MODE_IS_STANDARD,
		.vid		= TV_MODE_NTSC,
	}
};

/*----------------------show colorbar----------------------*/
int  asoc_cvbs_cmu_or_tvoutpll1_enable(const char *name) {
	struct clk *dev_clk;
	dev_clk = clk_get_sys(name, NULL); 
	if (IS_ERR(dev_clk)) {
		return PTR_ERR(dev_clk);
	}
	if (!clk_is_enabled(dev_clk))
		clk_enable(dev_clk);

	return 0;
}

int  asoc_cvbs_cmu_or_tvoutpll1_disable(const char *name)
{
	struct clk *dev_clk;
	dev_clk = clk_get_sys(name, NULL); 
	if (IS_ERR(dev_clk)) {
		return PTR_ERR(dev_clk);
	}
	if (clk_is_enabled(dev_clk))
		clk_disable(dev_clk);

	return 0;
}

void asoc_cvbs_cmu_reset(void) {
	struct clk *tvout_clk;
	tvout_clk = clk_get_sys(CLK_NAME_TVOUT0_CLK, NULL);
	clk_reset(tvout_clk);
}

void asoc_cvbs_cmu_enable(void) {
	asoc_cvbs_cmu_or_tvoutpll1_enable(CLK_NAME_TVOUT0_CLK);
	asoc_cvbs_cmu_or_tvoutpll1_enable(CLK_NAME_TV24M_CLK);
	asoc_cvbs_cmu_or_tvoutpll1_enable(CLK_NAME_CVBS_CLK);
}

unsigned long  asoc_tvoutpll1_set_rate(unsigned long new_rate) {
	struct clk *dev_clk;
	unsigned long rate;
	int ret;
	dev_clk = clk_get_sys(CLK_NAME_TVOUT1PLL, NULL); 
	if (IS_ERR(dev_clk)) {
		return PTR_ERR(dev_clk);
    }
	asoc_cvbs_cmu_or_tvoutpll1_enable(CLK_NAME_TVOUT1PLL);
	rate = clk_get_rate(dev_clk) / 1000;
	if (new_rate != rate) {
		ret = clk_set_rate(dev_clk, new_rate * 1000);
		if (ret) {
			printk("%s(%d):clk_set_rate  failed1\n", __func__, __LINE__);
			return -EINVAL;

		}
		
		rate = clk_get_rate(dev_clk) / 1000;
		if (new_rate  != rate) {
			printk("%s(%d):clk_set_rate  failed2\n", __func__, __LINE__);
			asoc_cvbs_cmu_or_tvoutpll1_disable(CLK_NAME_TVOUT1PLL);
			
			return -EINVAL;
		}
	}

	return rate;
}

/*-----------------------configure cvbs---------------------*/
static void configure_ntsc(void)//ntsc(480i),pll1:432M,pll0:594/1.001
{
	GL5201_TVOUT_INFO("[%s start]\n", __func__);
	
	//act_writel(act_readl(CMU_TVOUTPLL) | CMU_TVOUTPLL_PLL1EN , 
	//	CMU_TVOUTPLL);
	asoc_tvoutpll1_set_rate(216000);
	
	act_writel(CVBS_MSR_CVBS_NTSC_M | CVBS_MSR_CVCKS, CVBS_MSR);
	
	act_writel((act_readl(CVBS_AL_SEPO) & (~CVBS_AL_SEPO_ALEP_MASK)) |
			    CVBS_AL_SEPO_ALEP(0x106),CVBS_AL_SEPO);
	act_writel((act_readl(CVBS_AL_SEPO) & (~CVBS_AL_SEPO_ALSP_MASK)) |
			   CVBS_AL_SEPO_ALSP(0x15),CVBS_AL_SEPO);	

	act_writel((act_readl(CVBS_AL_SEPE) & (~CVBS_AL_SEPE_ALEPEF_MASK)) |
			   CVBS_AL_SEPE_ALEPEF(0x20d),CVBS_AL_SEPE);
	act_writel((act_readl(CVBS_AL_SEPE) & (~CVBS_AL_SEPE_ALSPEF_MASK)) |
			   CVBS_AL_SEPE_ALSPEF(0x11c),CVBS_AL_SEPE);

	act_writel((act_readl(CVBS_AD_SEP) & (~CVBS_AD_SEP_ADEP_MASK)) |
			   CVBS_AD_SEP_ADEP(0x2cf),CVBS_AD_SEP);
	act_writel((act_readl(CVBS_AD_SEP) & (~CVBS_AD_SEP_ADSP_MASK)) |
			   CVBS_AD_SEP_ADSP(0x0),CVBS_AD_SEP);
	
	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
}

static void configure_pal(void)//pal(576i),pll1:432M,pll0:594M
{
	GL5201_TVOUT_INFO("[%s start]\n", __func__);
	
	//act_writel(act_readl(CMU_TVOUTPLL) | CMU_TVOUTPLL_PLL1EN , CMU_TVOUTPLL);
	asoc_tvoutpll1_set_rate(216000);

	act_writel(CVBS_MSR_CVBS_PAL_D | CVBS_MSR_CVCKS, CVBS_MSR);

	act_writel((act_readl(CVBS_AL_SEPO) & (~CVBS_AL_SEPO_ALEP_MASK)) |
			   CVBS_AL_SEPO_ALEP(0x136),CVBS_AL_SEPO);
	act_writel((act_readl(CVBS_AL_SEPO) & (~CVBS_AL_SEPO_ALSP_MASK)) |
			   CVBS_AL_SEPO_ALSP(0x17),CVBS_AL_SEPO);		   

	act_writel((act_readl(CVBS_AL_SEPE) & (~CVBS_AL_SEPE_ALEPEF_MASK)) |
			   CVBS_AL_SEPE_ALEPEF(0x26f),CVBS_AL_SEPE);
	act_writel((act_readl(CVBS_AL_SEPE) & (~CVBS_AL_SEPE_ALSPEF_MASK)) |
			   CVBS_AL_SEPE_ALSPEF(0x150),CVBS_AL_SEPE);

	act_writel((act_readl(CVBS_AD_SEP) & (~CVBS_AD_SEP_ADEP_MASK)) |
			   CVBS_AD_SEP_ADEP(0x2cf),CVBS_AD_SEP);
	act_writel((act_readl(CVBS_AD_SEP) & (~CVBS_AD_SEP_ADSP_MASK)) |
			   CVBS_AD_SEP_ADSP(0x0),CVBS_AD_SEP);

    	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
}

/**
 * @tv_mode: see enum TV_MODE_TYPE
 * This function sets register for CVBS(PAL/NTSC)
 */
static int configure_cvbs(u32 tv_mode)
{
	GL5201_TVOUT_INFO("[%s start]\n", __func__);

	switch (tv_mode) {
	case TV_MODE_PAL:
		configure_pal();
		break;
		
	case TV_MODE_NTSC:
		configure_ntsc();
		break;
		
	default:
		GL5201_TVOUT_ERROR("error! mode not supported\n");
		return -EINVAL;
	}
	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
	return 0;
}

/*--------------------cvbs hdp detect----------------------*/
#if (ENABLE_CVBS_PLUGDETECT == 1)
static void cvbs_plugout_proc(struct cvbs_manager *cvbs_mgr)
{
	GL5201_TVOUT_INFO("[%s start]\n", __func__);
	/* clear for CVBS plug out */
	act_writel(act_readl(TVOUT_OCR) &
		~(TVOUT_OCR_INACEN | TVOUT_OCR_PO_ADEN | TVOUT_OCR_INREN), 
		TVOUT_OCR);
	
	/* set for CVBS plug in */
	act_writel(act_readl(TVOUT_OCR) | TVOUT_OCR_PI_ADEN, TVOUT_OCR);

	act_writel(act_readl(TVOUT_OCR) & ~TVOUT_OCR_INREN, TVOUT_OCR);
	
	/* CVBS plugged, notify FB */
	//cvbs_mgr->cvbs_dev_for_de->is_plugged = 0;
	//asoc_display_notifier_call_chain(0, NULL);

	atomic_set(&cvbs_mgr->cvbs_status, TVOUT_DEV_PLUGOUT);
	//wake_up_interruptible(&cvbs_hpg_inform_queue);

	
	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
}

static void cvbs_plugin_proc(struct cvbs_manager *cvbs_mgr)
{
	GL5201_TVOUT_INFO("[%s start]\n", __func__);
	
	/* clear for CVBS plug in */
	act_writel(act_readl(TVOUT_OCR) | TVOUT_OCR_PI_ADEN, TVOUT_OCR);
	
	/* set for CVBS plugout */
	act_writel(act_readl(TVOUT_OCR) | TVOUT_OCR_INACEN |TVOUT_OCR_PO_ADEN |
		TVOUT_OCR_INREN , TVOUT_OCR);

	act_writel(act_readl(TVOUT_OCR) | TVOUT_OCR_INREN, TVOUT_OCR);
	
	/*enable  cvbs output*/
	//cvbs_mgr->cvbs_dev_for_de->is_plugged = 1;
	//asoc_display_notifier_call_chain(0, NULL);

	if (atomic_read(&cvbs_mgr->cvbs_status) == TVOUT_DEV_PLUGOUT) {
		atomic_set(&cvbs_mgr->cvbs_status_changed, 1);
		atomic_set(&cvbs_mgr->cvbs_status, TVOUT_DEV_PLUGIN);
		//wake_up_interruptible(&cvbs_hpg_inform_queue);
	}
	
	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
}
#endif
static inline void disable_cvbs_output(struct cvbs_manager *cvbs_mgr)
{
	act_writel(act_readl(TVOUT_EN) &  ~TVOUT_EN_CVBS_EN, TVOUT_EN);
	act_writel(act_readl(TVOUT_OCR) & ~(TVOUT_OCR_DAC3 | TVOUT_OCR_INREN), TVOUT_OCR);

	asoc_cvbs_cmu_or_tvoutpll1_disable(CLK_NAME_TVOUT1PLL);
	
	switch_set_state(&cvbs_mgr->switch_data.sdev, 0);
}

static  void enable_cvbs_output(struct cvbs_manager *cvbs_mgr)
{
	act_writel(act_readl(TVOUT_EN) | TVOUT_EN_CVBS_EN, TVOUT_EN);
	act_writel((act_readl(TVOUT_OCR) | TVOUT_OCR_DAC3 | TVOUT_OCR_INREN) &
		~TVOUT_OCR_DACOUT, TVOUT_OCR);
	
	switch_set_state(&cvbs_mgr->switch_data.sdev, 1);
	
}

static void cvbs_show_colorbar(struct cvbs_manager *cvbs_mgr)
{
	GL5201_TVOUT_INFO("[%s start]\n", __func__);

	configure_cvbs(cvbs_mgr->cvbs_default_settings.tv_mode);
	
	/*enable color bar ,cvbs HDAC*/
	act_writel(act_readl(TVOUT_OCR) | TVOUT_OCR_DACOUT | TVOUT_OCR_DAC3 
		| TVOUT_OCR_INACEN | TVOUT_OCR_INREN, TVOUT_OCR);
	GL5201_TVOUT_INFO("[in %s]TVOUT_OCR value is 0x%08x\n", 
		__func__,act_readl(TVOUT_OCR));
	
	act_writel(act_readl(CMU_TVOUTPLL) | CMU_TVOUTPLL_PLL1EN |
		CMU_TVOUTPLL_TK0SS | CMU_TVOUTPLL_CVBS_PLL1FSS(0x4), CMU_TVOUTPLL);

	/*eable cvbs output*/
	act_writel(act_readl(TVOUT_EN) | TVOUT_EN_CVBS_EN, TVOUT_EN);

    	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
}


/* enable/disable decvice */
static int act_cvbs_update_status(struct asoc_display_device *disp_dev)
{
	struct cvbs_manager *cvbs_mgr = 
		(struct cvbs_manager *)dev_get_drvdata(&disp_dev->dev);
	
	int state = disp_dev->state;

	GL5201_TVOUT_INFO("[%s start]\n", __func__);
	GL5201_TVOUT_INFO("disp_dev->state is %d\n", state);
	
	if (state == atomic_read(&cvbs_mgr->cvbs_enabled)) {
		GL5201_TVOUT_INFO("already in this state\n");
		return 0;
	}

	if (state != 0) {
		GL5201_TVOUT_INFO("[in %s] enable cvbs!\n", __func__);
		enable_cvbs_output(cvbs_mgr);
		atomic_set(&cvbs_mgr->cvbs_enabled, 1);
	} else {
		GL5201_TVOUT_INFO("%s, disable cvbs!\n", __func__);
		disable_cvbs_output(cvbs_mgr);
		atomic_set(&cvbs_mgr->cvbs_enabled, 0);
	}

	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
	return 0;
}

static int cvbs_display_get_status(struct asoc_display_device *disp_dev)
{
	GL5201_TVOUT_INFO("[%s start]\n", __func__);
	return disp_dev->state;
	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
}

static int act_cvbs_update_mode(struct asoc_display_device *disp_dev)
{
	struct cvbs_manager *cvbs_mgr = 
		(struct cvbs_manager *)dev_get_drvdata(&disp_dev->dev);
	
	GL5201_TVOUT_INFO("[%s start]\n", __func__);
	
	/* configure CVBS mode */
	mutex_lock(&cvbs_setting_mutex);
	configure_cvbs(cvbs_mgr->cvbs_default_settings.tv_mode);
	mutex_unlock(&cvbs_setting_mutex);

	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
	return 0;
}

static unsigned int cvbs_display_get_tvout_pll1(
	struct asoc_display_device *disp_dev,
	const struct de_video_mode *mode)
{
	const struct de_video_mode *req_mode;
	unsigned int tvoutpll1_khz;

	GL5201_TVOUT_INFO("%s\n", __func__);

	if (NULL != mode)
		req_mode = mode;
	else
		req_mode = disp_dev->disp_cur_mode;

	switch (req_mode->vid) {
		
	case TV_MODE_PAL:
	case TV_MODE_NTSC:
		tvoutpll1_khz = 108000;
		break;
		
	default:
		GL5201_TVOUT_INFO("invalid cvbs mode\n");
		tvoutpll1_khz = 0;
		break;
	}

	GL5201_TVOUT_INFO("%s finished\n", __func__);
	return tvoutpll1_khz;
}
static unsigned int act_cvbs_get_devclk_khz(
	struct asoc_display_device *disp_dev,
	unsigned int src_clk_khz,
	const struct de_video_mode *mode)
{
	GL5201_TVOUT_INFO("%s\n", __func__);
	return 13500;
}


static int act_cvbs_get_mdsb_parameter(struct asoc_display_device *disp_dev,
		int disp_mode ,int * hr , int * vr , int * drop, int * delay)
{	
	  int cvbs_mode = disp_dev->disp_cur_mode->vid;
		switch(disp_mode){
	  	case TV_1080P_60HZ:
	  	case TV_1080P_50HZ:
			if(cvbs_mode == TV_MODE_PAL){
				*vr = 0x02407333;
				*hr = 0x02D05549;
				*drop = 0x1615;
				*delay = 0x15a;
			}else{
				*vr = 0x01E08924;
				*hr = 0x02D0553e;
				*drop = 0x1B19;
				*delay = 0x198;
			}	
			break;
				
	  	case TV_720P_60HZ:
	  	case TV_720P_50HZ:
	  		*hr = 0x02D038CC;
			if(cvbs_mode == TV_MODE_PAL){
				*vr = 0x02404CCC;				
				*drop = 0x0F0E;
				*delay = 0x19F;
			}else{
				*vr = 0x01E05B6D;
				*drop = 0x1110;
				*delay = 0x1DC;
			}	
			break;
				
	  	default:
	  		printk("cvbs not support this mod \n");
			
	  	  return -1;
	  }
	  return 0;	
}

#if (ENABLE_CVBS_PLUGDETECT)
static void cvbs_switch_work(struct work_struct *work)
{
	
	struct cvbs_switch_data *data = 
		container_of(work, struct cvbs_switch_data, work);
	struct cvbs_manager *cvbs_mgr =
		container_of(data, struct cvbs_manager, switch_data);
	
	/*pull in*/
	if (0 != (data->status_val & TVOUT_STA_DAC3ILS)) {
		
		GL5201_TVOUT_INFO("[in %s]CVBS plug in interrupt\n",__func__);
		if (TVOUT_DEV_PLUGIN == atomic_read(&cvbs_mgr->cvbs_status)) {
			GL5201_TVOUT_ERROR("CVBS set has been plugged in\n");
			return;
			
		} else {
			cvbs_plugin_proc(cvbs_mgr);
			GL5201_TVOUT_INFO("[in %s]inform cvbs_plugin_thread\n",__func__);
			
		}
	}

	/*pull out*/
	if (0 != (data->status_val & TVOUT_STA_DAC3DLS)) {
		if (TVOUT_DEV_PLUGIN == atomic_read(&cvbs_mgr->cvbs_status)) {
			cvbs_plugout_proc(cvbs_mgr);
		} else {
			GL5201_TVOUT_ERROR("CVBS been plugged out\n");
			return;
		}
	}

	
}

static irqreturn_t tvout_handler(s32 irq, void *dev_id)
{
	u32 status_val;
	u32 ocr_val;
	struct cvbs_manager *cvbs_mgr = (struct cvbs_manager *)dev_id;
	
	GL5201_TVOUT_INFO("[%s start]\n", __func__);

	/* check plugin pending bit */
	status_val = act_readl(TVOUT_STA);
	ocr_val = act_readl(TVOUT_OCR);
	cvbs_mgr->switch_data.status_val = status_val;

	if (!(((status_val & TVOUT_STA_DAC3ILS) && (ocr_val & TVOUT_OCR_PI_IRQEN))
		|| ((status_val & TVOUT_STA_DAC3DLS) && (ocr_val & TVOUT_OCR_PO_IRQEN)))) {
		
		GL5201_TVOUT_INFO("tvout_intHandler finished!\n");
		return IRQ_NONE;
		
	}

	act_writel(act_readl(TVOUT_STA) | status_val, TVOUT_STA);

	schedule_work(&cvbs_mgr->switch_data.work);
	
	GL5201_TVOUT_INFO("[%s finished]\n", __func__);

	return IRQ_HANDLED;
}
#endif
/*---------------------reset-----------------------*/
static void tvout_reset(void)
{
	GL5201_TVOUT_INFO("[%s start]\n", __func__);

	asoc_cvbs_cmu_enable();
	asoc_cvbs_cmu_reset();
	asoc_cvbs_cmu_or_tvoutpll1_disable(CLK_NAME_TVOUT1PLL);

#ifdef IC_DEBUG
	act_writel(act_readl(CMU_TVOUTPLL1DEBUG) & ~(1 << 23), CMU_TVOUTPLL1DEBUG);
#endif
	
	act_writel(0x110050, TVOUT_DDCR);
	
	/* disable before registering irq handler */
	act_writel(0x0, TVOUT_OCR);
	
	/* clear pendings before registering irq handler */
	act_writel(act_readl(TVOUT_STA), TVOUT_STA);
	
	GL5201_TVOUT_INFO("[%s finished]\n", __func__);

}

/*----------------------file ops---------------------------*/
static int cvbs_open(struct inode *inode, struct file *file)
{
	int err = 0;
	GL5201_TVOUT_INFO("[in %s]", __func__);
	err = nonseekable_open(inode, file);
	if (err)
		return err;
	file->private_data = cvbs_data;

	GL5201_TVOUT_INFO("[in %s]", __func__);
	return 0;
}

static long cvbs_ioctl(struct file *file, u32 cmd, unsigned long arg)
{
	s32 ret;
	u32 status;
	u32 isenabled;
	
	struct cvbs_manager *cvbs_mgr = (struct cvbs_manager *)(file->private_data);
	struct asoc_display_device *asoc_display_dev = 
		(struct asoc_display_device *)cvbs_mgr->private_data;
	
	GL5201_TVOUT_INFO("[in %s]", __func__);
		 
	switch (cmd) {
	case CVBS_SET_CONFIGURE:
		GL5201_TVOUT_INFO("configure cvbs!\n");
		
		mutex_lock(&cvbs_setting_mutex);
		ret = copy_from_user(&cvbs_mgr->cvbs_default_settings, (void *)arg,
			sizeof(struct tv_settings));
		
		if (ret || (cvbs_mgr->cvbs_default_settings.tv_mode >= TV_MODE_MAX)) {
			mutex_unlock(&cvbs_setting_mutex);
			return -EFAULT;
		}
		mutex_unlock(&cvbs_setting_mutex);

		if (cvbs_mgr->cvbs_default_settings.tv_mode == TV_MODE_PAL)
			asoc_display_dev->disp_cur_mode = &cvbs_display_modes[TV_MODE_PAL]; 
		else
			asoc_display_dev->disp_cur_mode = &cvbs_display_modes[TV_MODE_NTSC];
			
		asoc_display_dev->fb_blank = FB_BLANK_UNBLANK;
		break;
		
	case CVBS_GET_CONFIGURE:
		GL5201_TVOUT_INFO("get CVBS CONFIGURE\n");
		if (copy_to_user((void *)arg, &cvbs_data->cvbs_default_settings, sizeof(struct tv_settings)))
			return -EFAULT;
		break;
		
	case CVBS_GET_STATUS:
		GL5201_TVOUT_INFO("get CVBS plugin/plugout status\n");
		status = atomic_read(&cvbs_mgr->cvbs_status);
		if (copy_to_user((void *)arg, &status, sizeof(u32)))
			return -EFAULT;
		break;

	case CVBS_SET_ENABLE:
		GL5201_TVOUT_INFO("CVBS_SET_ENABLE ioctl!\n");
		if (copy_from_user(&isenabled, (void *)arg, sizeof(u32)))
			return -EFAULT;
		if (isenabled) {
			GL5201_TVOUT_INFO("enable cvbs\n");
			enable_cvbs_output(cvbs_mgr);
			atomic_set(&cvbs_mgr->cvbs_enabled, 1);
		} else {
			GL5201_TVOUT_INFO("disable cvbs\n");
			disable_cvbs_output(cvbs_mgr);
			atomic_set(&cvbs_mgr->cvbs_enabled, 0);
		}
		break;

	case CVBS_SHOW_COLORBAR:
		GL5201_TVOUT_INFO("CVBS_SHOW_COLORBAR ioctl!\n");
		cvbs_show_colorbar(cvbs_mgr);
		break;

	default:
		/* invalid commands */
		return -EINVAL;
	}

	return 0;
}



static u32 cvbs_poll(struct file *filp, poll_table *wait)
{
    u32 mask = 0;

    printk("cvbs_poll  called............................................ \n");

    printk("cvbs_poll  called............................................ \n");

    printk(KERN_INFO "The process is \"%s\" (pid %i)\n", current->comm, current->pid);

    WARN_ON(1);  

   return mask;

}

static ssize_t show_cvbs_dump (struct device *dev, struct device_attribute *attr,
		char *buf)
{
	GL5201_TVOUT_INFO("\n\nfollowing list all tvout register's value!\n");
	GL5201_TVOUT_INFO("register CMU_TVOUTPLL1DEBUG(0x%08x) value is 0x%08x\n",
		CMU_TVOUTPLL1DEBUG, act_readl(CMU_TVOUTPLL1DEBUG));
	GL5201_TVOUT_INFO("register CMU_TVOUTPLL(0x%08x) value is 0x%08x\n",
		CMU_TVOUTPLL, act_readl(CMU_TVOUTPLL));
	GL5201_TVOUT_INFO("register CMU_DEVCLKEN0(0x%08x) value is 0x%08x\n",
		CMU_DEVCLKEN0, act_readl(CMU_DEVCLKEN0));
	GL5201_TVOUT_INFO("register CMU_DEVCLKEN1(0x%08x) value is 0x%08x\n",
		CMU_DEVCLKEN1, act_readl(CMU_DEVCLKEN1));
	GL5201_TVOUT_INFO("register CMU_DEVRST0(0x%08x) value is 0x%08x\n",
		CMU_DEVRST0, act_readl(CMU_DEVRST0));
	GL5201_TVOUT_INFO("register CMU_DEVRST1(0x%08x) value is 0x%08x\n",
		CMU_DEVRST1, act_readl(CMU_DEVRST1));
	
	GL5201_TVOUT_INFO("register TVOUT_EN(0x%08x) value is 0x%08x\n",
		TVOUT_EN, act_readl(TVOUT_EN));
	GL5201_TVOUT_INFO("register TVOUT_OCR(0x%08x) value is 0x%08x\n",
		TVOUT_OCR, act_readl(TVOUT_OCR));
	GL5201_TVOUT_INFO("register TVOUT_STA(0x%08x) value is 0x%08x\n",
		TVOUT_STA, act_readl(TVOUT_STA));
	GL5201_TVOUT_INFO("register TVOUT_PRL(0x%08x) value is 0x%08x\n",
		TVOUT_PRL, act_readl(TVOUT_PRL));
	GL5201_TVOUT_INFO("register TVOUT_CCR(0x%08x) value is 0x%08x\n",
		TVOUT_CCR, act_readl(TVOUT_CCR));
	GL5201_TVOUT_INFO("register TVOUT_BCR(0x%08x) value is 0x%08x\n",
		TVOUT_BCR, act_readl(TVOUT_BCR));
	GL5201_TVOUT_INFO("register TVOUT_CSCR(0x%08x) value is 0x%08x\n",
		TVOUT_CSCR, act_readl(TVOUT_CSCR));
	GL5201_TVOUT_INFO("register CVBS_MSR(0x%08x) value is 0x%08x\n",
		CVBS_MSR, act_readl(CVBS_MSR));
	GL5201_TVOUT_INFO("register CVBS_AL_SEPO(0x%08x) value is 0x%08x\n",
		CVBS_AL_SEPO, act_readl(CVBS_AL_SEPO));
	GL5201_TVOUT_INFO("register CVBS_AL_SEPE(0x%08x) value is 0x%08x\n",
		CVBS_AL_SEPE, act_readl(CVBS_AL_SEPE));
	GL5201_TVOUT_INFO("register CVBS_AD_SEP(0x%08x) value is 0x%08x\n",
		CVBS_AD_SEP, act_readl(CVBS_AD_SEP));
	GL5201_TVOUT_INFO("register CVBS_HUECR(0x%08x) value is 0x%08x\n",
		CVBS_HUECR, act_readl(CVBS_HUECR));
	GL5201_TVOUT_INFO("register CVBS_SCPCR(0x%08x) value is 0x%08x\n",
		CVBS_SCPCR, act_readl(CVBS_SCPCR));
	GL5201_TVOUT_INFO("register CVBS_SCFCR(0x%08x) value is 0x%08x\n",
		CVBS_SCFCR, act_readl(CVBS_SCFCR));
	GL5201_TVOUT_INFO("register CVBS_CBACR(0x%08x) value is 0x%08x\n",
		CVBS_CBACR, act_readl(CVBS_CBACR));
	GL5201_TVOUT_INFO("register CVBS_SACR(0x%08x) value is 0x%08x\n",
		CVBS_SACR, act_readl(CVBS_SACR));
	GL5201_TVOUT_INFO("register TVOUT_DCR(0x%08x) value is 0x%08x\n",
		TVOUT_DCR, act_readl(TVOUT_DCR));
	GL5201_TVOUT_INFO("register TVOUT_DDCR(0x%08x) value is 0x%08x\n",
		TVOUT_DDCR, act_readl(TVOUT_DDCR));
	GL5201_TVOUT_INFO("register TVOUT_DCORCTL(0x%08x) value is 0x%08x\n",
		TVOUT_DCORCTL, act_readl(TVOUT_DCORCTL));
	GL5201_TVOUT_INFO("register TVOUT_DRCR(0x%08x) value is 0x%08x\n",
		TVOUT_DRCR, act_readl(TVOUT_DRCR));

	return 0;
}

static ssize_t show_de_dump (struct device *dev, struct device_attribute *attr,
		char *buf)
{
#if 0
	GL5201_TVOUT_INFO("\n\nfollowing list all de register's value!\n");
	GL5201_TVOUT_INFO("DE_FCTL(0x%x) value is 0x%x\n",DE_FCTL ,act_readl(DE_FCTL));
	GL5201_TVOUT_INFO("DE_SCRCTL(0x%x) value is 0x%x\n", DE_SCRCTL,act_readl(DE_SCRCTL));
	GL5201_TVOUT_INFO("DE_BCCTL(0x%x) value is 0x%x\n", DE_BCCTL,act_readl(DE_BCCTL));
	GL5201_TVOUT_INFO("DE_PHCTL(0x%x) value is 0x%x\n", DE_PHCTL,act_readl(DE_PHCTL));
	GL5201_TVOUT_INFO("DE_BLKEN(0x%x) value is 0x%x\n", DE_BLKEN,act_readl(DE_BLKEN));
	GL5201_TVOUT_INFO("DE_OUTFIFO(0x%x) value is 0x%x\n",DE_OUTFIFO, act_readl(DE_OUTFIFO));
	GL5201_TVOUT_INFO("DE_LR0_SIZE(0x%x) value is 0x%x\n", DE_LR0_SIZE,act_readl(DE_LR0_SIZE));
	GL5201_TVOUT_INFO("DE_LR1_SIZE(0x%x) value is 0x%x\n", DE_LR1_SIZE,act_readl(DE_LR1_SIZE));
	GL5201_TVOUT_INFO("DE_LR2_SIZE(0x%x) value is 0x%x\n", DE_LR2_SIZE,act_readl(DE_LR2_SIZE));
	GL5201_TVOUT_INFO("DE_LR3_SIZE(0x%x) value is 0x%x\n", DE_LR3_SIZE,act_readl(DE_LR3_SIZE));
	GL5201_TVOUT_INFO("DE_LR0_COR(0x%x) value is 0x%x\n", DE_LR0_COR,act_readl(DE_LR0_COR));
	GL5201_TVOUT_INFO("DE_LR1_COR(0x%x) value is 0x%x\n", DE_LR1_COR,act_readl(DE_LR1_COR));
	GL5201_TVOUT_INFO("DE_LR2_COR(0x%x) value is 0x%x\n",DE_LR2_COR ,act_readl(DE_LR2_COR));
	GL5201_TVOUT_INFO("DE_LR3_COR(0x%x) value is 0x%x\n", DE_LR3_COR,act_readl(DE_LR3_COR));
	GL5201_TVOUT_INFO("DE_LR0_CFG(0x%x) value is 0x%x\n", DE_LR0_CFG,act_readl(DE_LR0_CFG));
	GL5201_TVOUT_INFO("DE_LR1_CFG(0x%x) value is 0x%x\n", DE_LR1_CFG,act_readl(DE_LR1_CFG));
	GL5201_TVOUT_INFO("DE_LR2_CFG(0x%x) value is 0x%x\n", DE_LR2_CFG,act_readl(DE_LR2_CFG));
	GL5201_TVOUT_INFO("DE_LR3_CFG(0x%x) value is 0x%x\n",DE_LR3_CFG ,act_readl(DE_LR3_CFG));
	GL5201_TVOUT_INFO("DE_FB0_G(0x%x) value is 0x%x\n",DE_FB0_G ,act_readl(DE_FB0_G));
	GL5201_TVOUT_INFO("DE_FB1_G(0x%x) value is 0x%x\n",DE_FB1_G ,act_readl(DE_FB1_G));
	GL5201_TVOUT_INFO("DE_FB2_G(0x%x) value is 0x%x\n",DE_FB2_G ,act_readl(DE_FB2_G));
	
	GL5201_TVOUT_INFO("DE_FBY_V(0x%x) value is 0x%x\n", DE_FBY_V,act_readl(DE_FBY_V));
	GL5201_TVOUT_INFO("DE_FBU_V(0x%x) value is 0x%x\n", DE_FBU_V,act_readl(DE_FBU_V));
	GL5201_TVOUT_INFO("DE_FBV_V(0x%x) value is 0x%x\n", DE_FBV_V,act_readl(DE_FBV_V));
	GL5201_TVOUT_INFO("DE_FBUV_V(0x%x) value is 0x%x\n",DE_FBUV_V ,act_readl(DE_FBUV_V));
	GL5201_TVOUT_INFO("DE_FB3_V(0x%x) value is 0x%x\n",DE_FB3_V ,act_readl(DE_FB3_V));
	GL5201_TVOUT_INFO("DE_FBYC_V(0x%x) value is 0x%x\n",DE_FBYC_V ,act_readl(DE_FBYC_V));
	GL5201_TVOUT_INFO("DE_FBUVC_V(0x%x) value is 0x%x\n",DE_FBUVC_V ,act_readl(DE_FBUVC_V));
	GL5201_TVOUT_INFO("DE_FBYLUT_V(0x%x) value is 0x%x\n",DE_FBYLUT_V ,act_readl(DE_FBYLUT_V));
	GL5201_TVOUT_INFO("DE_FBUVLUT_V(0x%x) value is 0x%x\n", DE_FBUVLUT_V,act_readl(DE_FBUVLUT_V));
	GL5201_TVOUT_INFO("DE_FBLY_V(0x%x) value is 0x%x\n", DE_FBLY_V,act_readl(DE_FBLY_V));
	GL5201_TVOUT_INFO("DE_FBLVC_V(0x%x) value is 0x%x\n",DE_FBLVC_V ,act_readl(DE_FBLVC_V));
	GL5201_TVOUT_INFO("DE_FBRY_V(0x%x) value is 0x%x\n", DE_FBRY_V,act_readl(DE_FBRY_V));
	GL5201_TVOUT_INFO("DE_FBRUV_V(0x%x) value is 0x%x\n", DE_FBRUV_V,act_readl(DE_FBRUV_V));
	GL5201_TVOUT_INFO("DE_STR0(0x%x) value is 0x%x\n", DE_STR0,act_readl(DE_STR0));
	GL5201_TVOUT_INFO("DE_STR1(0x%x) value is 0x%x\n", DE_STR1,act_readl(DE_STR1));
	GL5201_TVOUT_INFO("DE_STR2(0x%x) value is 0x%x\n", DE_STR2,act_readl(DE_STR2));
	GL5201_TVOUT_INFO("DE_STR3(0x%x) value is 0x%x\n", DE_STR3,act_readl(DE_STR3));
	GL5201_TVOUT_INFO("DE_CKMAX(0x%x) value is 0x%x\n", DE_CKMAX,act_readl(DE_CKMAX));
	GL5201_TVOUT_INFO("DE_CKMIN(0x%x) value is 0x%x\n", DE_CKMIN,act_readl(DE_CKMIN));
	GL5201_TVOUT_INFO("DE_CKCFG(0x%x) value is 0x%x\n", DE_CKCFG,act_readl(DE_CKCFG));
	GL5201_TVOUT_INFO("DE_CURADDR_L0(0x%x) value is 0x%x\n", DE_CURADDR_L0,act_readl(DE_CURADDR_L0));
	GL5201_TVOUT_INFO("DE_CURADDR_L1(0x%x) value is 0x%x\n", DE_CURADDR_L1,act_readl(DE_CURADDR_L1));
	GL5201_TVOUT_INFO("DE_CURADDR_L2(0x%x) value is 0x%x\n",DE_CURADDR_L2, act_readl(DE_CURADDR_L2));
	GL5201_TVOUT_INFO("DE_CURADDR_L3(0x%x) value is 0x%x\n", DE_CURADDR_L3,act_readl(DE_CURADDR_L3));
	GL5201_TVOUT_INFO("DE_DITHER(0x%x) value is 0x%x\n", DE_DITHER,act_readl(DE_DITHER));
	GL5201_TVOUT_INFO("DE_LR3_SUBSIZE(0x%x) value is 0x%x\n", DE_LR3_SUBSIZE,act_readl(DE_LR3_SUBSIZE));
	GL5201_TVOUT_INFO("DE_SCLCOEF0(0x%x) value is 0x%x\n", DE_SCLCOEF0,act_readl(DE_SCLCOEF0));
	GL5201_TVOUT_INFO("DE_SCLCOEF1(0x%x) value is 0x%x\n", DE_SCLCOEF1,act_readl(DE_SCLCOEF1));
	GL5201_TVOUT_INFO("DE_SCLCOEF2(0x%x) value is 0x%x\n", DE_SCLCOEF2,act_readl(DE_SCLCOEF2));
	GL5201_TVOUT_INFO("DE_SCLCOEF3(0x%x) value is 0x%x\n", DE_SCLCOEF3,act_readl(DE_SCLCOEF3));
	GL5201_TVOUT_INFO("DE_SCLCOEF4(0x%x) value is 0x%x\n", DE_SCLCOEF4,act_readl(DE_SCLCOEF4));
	GL5201_TVOUT_INFO("DE_SCLCOEF5(0x%x) value is 0x%x\n", DE_SCLCOEF5,act_readl(DE_SCLCOEF5));
	GL5201_TVOUT_INFO("DE_SCLCOEF6(0x%x) value is 0x%x\n", DE_SCLCOEF6,act_readl(DE_SCLCOEF6));
	GL5201_TVOUT_INFO("DE_SCLCOEF7(0x%x) value is 0x%x\n", DE_SCLCOEF7,act_readl(DE_SCLCOEF7));
	GL5201_TVOUT_INFO("DE_SCLOFS(0x%x) value is 0x%x\n", DE_SCLOFS,act_readl(DE_SCLOFS));
	GL5201_TVOUT_INFO("DE_SCLSR(0x%x) value is 0x%x\n", DE_SCLSR,act_readl(DE_SCLSR));
	GL5201_TVOUT_INFO("DE_BRI(0x%x) value is 0x%x\n", DE_BRI,act_readl(DE_BRI));
	GL5201_TVOUT_INFO("DE_CON(0x%x) value is 0x%x\n", DE_CON,act_readl(DE_CON));
	GL5201_TVOUT_INFO("DE_SAT(0x%x) value is 0x%x\n", DE_SAT,act_readl(DE_SAT));
	GL5201_TVOUT_INFO("DE_CRITICAL(0x%x) value is 0x%x\n", DE_CRITICAL,act_readl(DE_CRITICAL));
	GL5201_TVOUT_INFO("DE_INTEN(0x%x) value is 0x%x\n", DE_INTEN,act_readl(DE_INTEN));
	GL5201_TVOUT_INFO("DE_INTSTA(0x%x) value is 0x%x\n", DE_INTSTA,act_readl(DE_INTSTA));
	GL5201_TVOUT_INFO("DE_PRELINE(0x%x) value is 0x%x\n", DE_PRELINE,act_readl(DE_PRELINE));
	GL5201_TVOUT_INFO("DE_RAMIDX(0x%x) value is 0x%x\n", DE_RAMIDX,act_readl(DE_RAMIDX));
	GL5201_TVOUT_INFO("DE_RAMWIN(0x%x) value is 0x%x\n", DE_RAMWIN,act_readl(DE_RAMWIN));
	GL5201_TVOUT_INFO("MDSB_CTL(0x%x) value is 0x%x\n", MDSB_CTL,act_readl(MDSB_CTL));
#endif
	return 0;
}

static ssize_t show_cvbs_colorbar (struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct cvbs_manager *cvbs_mgr = (struct cvbs_manager*)dev_get_drvdata(dev);
	configure_cvbs(cvbs_mgr->cvbs_default_settings.tv_mode);
	cvbs_show_colorbar(cvbs_mgr);
	return 0;
}

static ssize_t show_cvbs_mode (struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct cvbs_manager *cvbs_mgr = (struct cvbs_manager*)dev_get_drvdata(dev);
	return sprintf(buf, "%d", cvbs_mgr->cvbs_default_settings.tv_mode);
}

static ssize_t store_cvbs_mode(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t count)
{	
	int vid_index;
	int r;

	struct cvbs_manager *cvbs_mgr = (struct cvbs_manager*)dev_get_drvdata(dev);
	struct asoc_display_device *asoc_display_dev = 
		(struct asoc_display_device *)cvbs_mgr->private_data;


	r = kstrtoint(buf, 0, &vid_index);
	if (r)
		return r;
	
	switch (vid_index) {

		case TV_MODE_PAL:
			cvbs_mgr->cvbs_default_settings.tv_mode = TV_MODE_PAL;
			asoc_display_dev->disp_cur_mode = &cvbs_display_modes[TV_MODE_PAL]; 
			asoc_display_dev->fb_blank = FB_BLANK_UNBLANK;
			break;

		case TV_MODE_NTSC:
			cvbs_mgr->cvbs_default_settings.tv_mode = TV_MODE_NTSC;
			asoc_display_dev->disp_cur_mode = &cvbs_display_modes[TV_MODE_NTSC];
			asoc_display_dev->fb_blank = FB_BLANK_UNBLANK;
			break;

		default:
			break;
	}
	

	return count;
}

static struct device_attribute asoc_cvbs_attrs[] = {
	__ATTR(cvbs_dump, S_IRUGO | S_IWUSR, show_cvbs_dump, NULL),
	__ATTR(de_dump, S_IRUGO | S_IWUSR, show_de_dump, NULL),
	__ATTR(cvbs_colorbar, S_IRUGO | S_IWUSR, show_cvbs_colorbar, NULL),
	__ATTR(cvbs_mode, S_IRUGO | S_IWUSR, show_cvbs_mode, store_cvbs_mode),
};

static struct file_operations cvbs_fops = {
	.owner		=	THIS_MODULE,
	.open		=	cvbs_open,
	.unlocked_ioctl	=	cvbs_ioctl,
	.poll		=	cvbs_poll,

};

static struct miscdevice cvbs_miscdev = {
	.minor	=	CVBS_MINOR,
	.name	=	"cvbs",
	.fops	=	&cvbs_fops,
};

#ifdef CONFIG_PM
static int tvout_platform_device_registered = 0;
static int tvout_platform_driver_registered = 0;

static int atv5202_cvbs_diver_suspend(
	struct platform_device *pdev, pm_message_t mesg)
{
	struct cvbs_manager *cvbs_mgr = 
	(struct cvbs_manager *)platform_get_drvdata(pdev);
	GL5201_TVOUT_INFO("[%s start]\n", __func__);

#if (ENABLE_CVBS_PLUGDETECT)
	act_writel(act_readl(TVOUT_OCR) &
		~(TVOUT_OCR_PI_IRQEN |		/*plugin Line IRQ EN*/
		TVOUT_OCR_PI_ADEN |		/*cvbs plugin auto detect EN*/
		TVOUT_OCR_PO_IRQEN |		/*plugout Line IRQ EN*/
		TVOUT_OCR_PO_ADEN)		/*cvbs plugout auto detect EN*/
		, TVOUT_OCR);
	/* release interrupt resource */
	free_irq(IRQ_ASOC_TVOUT, (void *)cvbs_miscdev.this_device);
#endif
	/* disable tvout */
	disable_cvbs_output(cvbs_mgr);


	/* disable cvbs */	
	//module_clk_disable(MOD_ID_CVBS);

	GL5201_TVOUT_INFO("[%s finished]\n", __func__);

	return 0;
}

static int atv5202_cvbs_diver_resume(struct platform_device *pdev)
{
	//struct cvbs_manager *cvbs_mgr = 
	//	(struct cvbs_manager *)platform_get_drvdata(pdev);
	
	GL5201_TVOUT_INFO("[%s start]\n", __func__);
	
	tvout_reset();

	//if (1 == atomic_read(&cvbs_mgr->cvbs_enabled)) {	
	//	mutex_lock(&cvbs_setting_mutex);
	//	configure_cvbs(cvbs_mgr->cvbs_default_settings.tv_mode);
	//	mutex_unlock(&cvbs_setting_mutex);
	//	
	//	enable_cvbs_output(cvbs_mgr);
	//}

#if (ENABLE_CVBS_PLUGDETECT)
	int ret;
	/* register interrupt handler for CVBS */
	ret = request_irq(IRQ_ASOC_TVOUT, tvout_handler,
		0, "cvbs", (void *)cvbs_miscdev.this_device);

	if (ret < 0)
		GL5201_TVOUT_INFO("[%s]fail to register tvout interrupt!\n", __func__);

	act_setl(
		TVOUT_OCR_PI_IRQEN |
		TVOUT_OCR_PO_IRQEN |
		TVOUT_OCR_PI_ADEN
		, TVOUT_OCR);
#endif

	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
	
	return 0;
}

static struct platform_device atv5202_cvbs_device = {
	.name      = "atv5202_cvbs",
	.id        = -1,
};
static struct platform_driver atv5202_cvbs_driver = {
	.driver = {
		.name  = "atv5202_cvbs",
		.owner = THIS_MODULE,
	},
	.suspend   = atv5202_cvbs_diver_suspend,
	.resume    = atv5202_cvbs_diver_resume,
};
#endif

static int act_cvbs_probe(struct asoc_display_device *pdev)
{
	s32 ret;
	int t, r;
	struct cvbs_manager * cvbs_mgr = NULL;
	
	GL5201_TVOUT_INFO("[%s start]\n", __func__);
	
	cvbs_mgr = kzalloc(sizeof(struct cvbs_manager), GFP_KERNEL);
	if (!cvbs_mgr)
		return -ENOMEM;
	
	/* initialize  cvbs_manager */
	atomic_set(&cvbs_mgr->cvbs_enabled, 0);
	atomic_set(&cvbs_mgr->cvbs_status_changed, 0);
	cvbs_mgr->cvbs_default_settings.tv_mode= TV_MODE_PAL;
	
	dev_set_drvdata(&pdev->dev, cvbs_mgr);
	cvbs_mgr->private_data = pdev;
	cvbs_data = cvbs_mgr;

	/*cvbs misc device register*/
	ret = misc_register(&cvbs_miscdev);
	if (ret) {
		GL5201_TVOUT_ERROR("CVBS driver initialization fails!\n");
		return ret;
	}
	dev_set_drvdata(cvbs_miscdev.this_device, cvbs_mgr);

	for (t = 0; t < ARRAY_SIZE(asoc_cvbs_attrs); t++) {
		r = device_create_file(cvbs_miscdev.this_device,
				&asoc_cvbs_attrs[t]);

		if (r) {
			dev_err(cvbs_miscdev.this_device, "failed to create sysfs "
						"file\n");
			return r;
		}
	}

	tvout_reset();

	/* configure CVBS parameters */
	if (cvbs_mgr->cvbs_default_settings.tv_mode == TV_MODE_PAL) 
		pdev->disp_cur_mode = &cvbs_display_modes[TV_MODE_PAL]; 
	 else 
		pdev->disp_cur_mode = &cvbs_display_modes[TV_MODE_NTSC];
	 
	pdev->fb_blank = FB_BLANK_UNBLANK;
	
	cvbs_mgr->switch_data.sdev.name = "cvbs";
#if (ENABLE_CVBS_PLUGDETECT)
	INIT_WORK(&cvbs_mgr->switch_data.work, cvbs_switch_work);
#endif
	ret = switch_dev_register(&cvbs_mgr->switch_data.sdev);
	if (ret < 0)
		return ret;
	
#if (ENABLE_CVBS_PLUGDETECT)
	
	/* start cvbs plugin pthread */
	//atomic_set(&cvbs_mgr->cvbs_plugin_queue_atomic, 0);
	//cvbs_mgr->cvbs_plugin_task = kthread_run(cvbs_plugin_thread, cvbs_mgr,
	//	"cvbs_plugin");
	//if (IS_ERR(cvbs_mgr->cvbs_plugin_task))
	//	GL5201_TVOUT_ERROR("ERROR: fail to start cvbs_plugin_thread\n");
	
	ret = request_irq(IRQ_ASOC_TVOUT, tvout_handler,
		0, "cvbs", (void *)cvbs_mgr);
	if (ret < 0) {
	//	atomic_set(&cvbs_mgr->cvbs_plugin_queue_atomic, 1);
	//	kthread_stop(cvbs_mgr->cvbs_plugin_task);
		misc_deregister(&cvbs_miscdev);
		return ret;
	}

	/* enable plugin/plugout interrupt */
	act_writel(act_readl(TVOUT_OCR) | TVOUT_OCR_PI_ADEN, TVOUT_OCR);
	act_setl(TVOUT_OCR_PI_IRQEN | TVOUT_OCR_PO_IRQEN, TVOUT_OCR);
	
	//if (1 == test_cvbs_plug()) {
	//	pdev->is_plugged = 1;
	//	atomic_set(&cvbs_mgr->cvbs_status, TVOUT_DEV_PLUGIN);
	//	cvbs_plugin_proc();
		
	//} else {
	//	pdev->is_plugged = 0;
	//	atomic_set(&cvbs_mgr->cvbs_status, TVOUT_DEV_PLUGOUT);
	//	act_setl(TVOUT_OCR_PI_ADEN, TVOUT_OCR);
	//}
	
	
#endif

#ifdef CONFIG_PM
	/* register tvout platform device */
	platform_set_drvdata(&atv5202_cvbs_device, cvbs_mgr);
	if (platform_device_register(&atv5202_cvbs_device))
		GL5201_TVOUT_ERROR("[%s]fail to register tvout device\n", __func__);
	else
		tvout_platform_device_registered = 1;

	/* register tvout platform driver */
	if (platform_driver_register(&atv5202_cvbs_driver)) {
		GL5201_TVOUT_ERROR("[%s]fail to register tvout driver\n", __func__);
		if (tvout_platform_device_registered) {
			platform_device_unregister(&atv5202_cvbs_device);
			tvout_platform_device_registered = 0;
			
		}
		
	} else {
		tvout_platform_driver_registered = 1;

	}
	platform_set_drvdata(&atv5202_cvbs_device, cvbs_mgr);
#endif

	GL5201_TVOUT_INFO("[%s finished]\n", __func__);
	return 0;
}

static void act_cvbs_remove(struct asoc_display_device *pdev)
{
	struct cvbs_manager *cvbs_mgr = 
		(struct cvbs_manager *)dev_get_drvdata(&pdev->dev);
	GL5201_TVOUT_INFO("%s start!\n", __func__);

#if (ENABLE_CVBS_PLUGDETECT)
	/* disable interrupt */
	act_clearl(
		TVOUT_OCR_PI_IRQEN |		/*plugin Line IRQ EN*/
		TVOUT_OCR_PI_ADEN |		/*cvbs plugin auto detect EN*/
		TVOUT_OCR_PO_IRQEN |		/*plugout Line IRQ EN*/
		TVOUT_OCR_PO_ADEN		/*cvbs plugout auto detect EN*/
		, TVOUT_OCR);
	
	/* release interrupt resource s*/
	free_irq(IRQ_ASOC_TVOUT, (void *)pdev);
#endif

	/* disable tvout */
	disable_cvbs_output(cvbs_mgr);
	

#ifdef CONFIG_PM
	if (tvout_platform_driver_registered)
		platform_driver_unregister(&atv5202_cvbs_driver);
	if (tvout_platform_device_registered)
		platform_device_unregister(&atv5202_cvbs_device);
#endif
	/* stop cvbs plugin pthread */
	//atomic_set(&cvbs_mgr->cvbs_plugin_queue_atomic, 1);
	//ret = kthread_stop(cvbs_mgr->cvbs_plugin_task);
	//GL5201_TVOUT_INFO("end cvbs_plugin_task. ret = %d\n" , ret);


	/* disable cvbs clk*/
	//GL5201_TVOUT_INFO("disable CVBS module\n");
	//module_clk_disable(MOD_ID_CVBS);
	asoc_cvbs_cmu_or_tvoutpll1_disable(CLK_NAME_TVOUT0_CLK);

	GL5201_TVOUT_INFO("%s finished\n", __func__);
}

static struct asoc_display_driver act_cvbs_driver = {
	.driver = {
		   .name = "act_cvbs",
		   .owner = THIS_MODULE,
	},
	.probe = act_cvbs_probe,
	.remove = act_cvbs_remove,
	.get_status = cvbs_display_get_status,
	.update_status = act_cvbs_update_status,	
	.update_mode = act_cvbs_update_mode,	
	.get_devclk_khz = act_cvbs_get_devclk_khz,
	.get_tvoutpll1_khz = cvbs_display_get_tvout_pll1,
	.get_mdsb_parameter = act_cvbs_get_mdsb_parameter,
};

static int __init act_cvbs_init(void)
{
	return asoc_display_driver_register(&act_cvbs_driver);
}

static void __exit act_cvbs_exit(void)
{
	asoc_display_driver_unregister(&act_cvbs_driver);
}

module_init(act_cvbs_init);
module_exit(act_cvbs_exit);
MODULE_AUTHOR("Actions");
MODULE_DESCRIPTION("Actions cvbs");
MODULE_LICENSE("GPL");

