//#include <linux/regulator/consumer.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <mach/gl5202_cmu.h>
#include <mach/clock.h>
#include <linux/suspend.h>
#include "../display/act_de/display.h"
#include "../display/act_de/de.h"
#include "asoc_hdmi.h"
#include "asoc_ioctl.h"

struct de_video_mode *hdmi_display_mode = NULL;
struct asoc_display_device *hdmi_data = NULL;
static DEFINE_MUTEX(hdmi_switch_mutex);
static DEFINE_MUTEX(hdmi_setting_mutex);
static DEFINE_MUTEX(hdmi_plug_mutex);
static DEFINE_MUTEX(hdmi_cap_mutex);
static DEFINE_MUTEX(hdmi_enable_mutex);

int asoc_hdmi_open(struct inode *inode, struct file *filp) {
	int err = 0;	
	err = nonseekable_open(inode, filp);
	if (err)
		return err;
	filp->private_data = hdmi_data;

	return 0;
}

long asoc_hdmi_ioctl(struct file *file, u32 cmd, unsigned long arg) {
	unsigned int plugging;
	unsigned int enabled;
	unsigned long ret;
	struct de_video_mode *mode;
	unsigned int state;
	bool disconnect_flag;
	bool manual_flag;
	bool hdcp_switch;

	struct asoc_display_device *pdev = (struct asoc_display_device *)file->private_data;
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)pdev->disp_priv_info;
	
	switch (cmd) {
	case HDMI_GET_LINK_STATUS:
		mutex_lock(&hdmi_plug_mutex);
		plugging = hdmi_get_link_status(pdev);
		ret = copy_to_user((u8 *)arg, (u8 *)(&plugging), sizeof(u32));
		mutex_unlock(&hdmi_plug_mutex);
		if (ret)
			return -EFAULT;
		break;
			
	case HDMI_GET_SINK_CAPABILITY:
		HDMI_DEBUG("HDMI_GET_SINK_CAPABILITY!\n");
		if (!(hdmi->sink_cap_available)) {
			HDMI_DEBUG("[%s]sink cap is not available\n", __func__);
			return -ENOMSG;
		}
		mutex_lock(&hdmi_cap_mutex);
		ret = copy_to_user((u8 *)arg, (u8 *)(&hdmi->ip_data.sink_cap),
			sizeof(struct sink_capabilities_t));
		mutex_unlock(&hdmi_cap_mutex);
		if (ret) {
			HDMI_DEBUG("[%s]sink cap failed to copy to user !\n", __func__);
			return -EFAULT;
		}
		break;

	case HDMI_SET_VIDEO_CONFIG:
		HDMI_DEBUG("[%s]HDMI_SET_VIDEO_CONFIG!\n", __func__);

		mutex_lock(&hdmi_setting_mutex);
		ret = copy_from_user((u8 *)(&hdmi->ip_data.v_cfg.settings->vid), (u8 *)arg,
			sizeof(unsigned int));
		if (ret || (hdmi->ip_data.v_cfg.settings->vid >= VID_MAX)) {
			return -EFAULT;
		}
		
		mode = hdmi_get_id(VIDEO_DE);
		mutex_unlock(&hdmi_setting_mutex);

		if (mode) {
			pdev->disp_cur_mode = mode;           
			pdev->fb_blank = FB_BLANK_UNBLANK;
		} else {
	
			HDMI_DEBUG("[%s]don't support this hdmi_display_mode!\n", __func__);
		}
		
		break;
	
	case HDMI_SWITCH_SET_STATUS:
		HDMI_DEBUG("[%s]HDMI_SWITCH_SET_STATUS!\n", __func__);
		ret = copy_from_user((u8 *)(&state), (u8 *)arg, sizeof(unsigned int));
		if (ret) {
			//mutex_unlock(&hdmi_setting_mutex);
			return -EFAULT;
		}
		
		if (hdmi_get_link_status(pdev)) {
		    if(state) {
		        hdmi_set_onoff_flag(false);
		        hdmi_set_switch(1);
		    } else {
		        hdmi_set_onoff_flag(true);
		        hdmi_set_switch(0);
		    }
	                }
		break;
		
	case HDMI_GET_DICONNECT_FLAG:
	               disconnect_flag = hdmi_get_disconnect_flag();
		ret = copy_to_user((u8 *)arg, (u8 *)(&disconnect_flag), sizeof(bool));
		if (ret)
			return -EFAULT;
		break;
	
	case HDMI_SET_DICONNECT_FLAG:
		HDMI_DEBUG("[%s]HDMI_SET_DICONNECT_FLAG!\n", __func__);
		ret = copy_from_user((u8 *)(&disconnect_flag), (u8 *)arg, sizeof(bool));
		if (ret) {
			printk("HDMI_SET_DICONNECT_FLAG fail\n");
			return -EFAULT;
		} 
		
		hdmi_set_disconnect_flag(disconnect_flag);
		if (hdmi_get_disconnect_flag()) {
			hdmi_set_switch(0);
		} else {
			if (hdmi_get_link_status(pdev)) {
				hdmi_set_switch(1);
			}
		}

		break;
		
	case HDMI_SET_MUNUAL_FLAG:
		HDMI_DEBUG("[%s]HDMI_SET_MUNUAL_FLAG!\n", __func__);
		ret = copy_from_user((u8 *)(&manual_flag), (u8 *)arg, sizeof(bool));
		if (ret) {
			printk("HDMI_SET_DICONNECT_FLAG fail\n");
			return -EFAULT;
		}  else 
			printk("HDMI_SET_DICONNECT_FLAG manual_flag:%d\n", manual_flag);
		
		hdmi_set_manual_flag(manual_flag);

		break;


	case HDMI_GET_VIDEO_CONFIG:
		HDMI_DEBUG("HDMI_GET_VIDEO_CONFIG!\n");
		ret = copy_to_user((u8 *)arg, (u8 *)(&hdmi->ip_data.v_cfg.settings->vid), sizeof(u32));
		if (ret)
			return -EFAULT;
		break;

	case HDMI_SET_VIDEO_3D:
		HDMI_DEBUG("[%s]HDMI_SET_VIDEO_3D!\n", __func__);
		mutex_lock(&hdmi_setting_mutex);
		ret = copy_from_user((u8 *)(&hdmi->ip_data.v_cfg.settings->_3d), (u8 *)arg,
			sizeof(unsigned int));
		mutex_unlock(&hdmi_setting_mutex);
		if (ret)
			return -EFAULT;
		break;

	case HDMI_SET_DEEP_COLOR:
		HDMI_DEBUG("[%s]HDMI_SET_DEEP_COLOR!\n", __func__);
		mutex_lock(&hdmi_setting_mutex);
		ret = copy_from_user((u8 *)(&hdmi->ip_data.v_cfg.settings->deep_color), (u8 *)arg,
			sizeof(unsigned int));
		if (ret || (hdmi->ip_data.v_cfg.settings->deep_color > DEEP_COLOR_MAX)) {
			//mutex_unlock(&hdmi_setting_mutex);
			return -EFAULT;
		}
		mutex_unlock(&hdmi_setting_mutex);
		break;

	case HDMI_SET_PIXEL_ENCODING:
		HDMI_DEBUG("[%s]HDMI_SET_PIXEL_ENCODING!\n", __func__);

		mutex_lock(&hdmi_setting_mutex);
		ret = copy_from_user(
			(u8 *)(&hdmi->ip_data.v_cfg.settings->pixel_encoding), 
			(u8 *)arg,
			sizeof(unsigned int));
		if (ret || (hdmi->ip_data.v_cfg.settings->pixel_encoding > VIDEO_PEXEL_ENCODING_MAX)) {

			//mutex_unlock(&hdmi_setting_mutex);
			return -EFAULT;
		}
		mutex_unlock(&hdmi_setting_mutex);
		break;

	case HDMI_SET_COLOR_XVYCC:
		HDMI_DEBUG("[%s]HDMI_SET_COLOR_XVYCC!\n", __func__);
		mutex_lock(&hdmi_setting_mutex);
		ret = copy_from_user(
			(u8 *)(&hdmi->ip_data.v_cfg.settings->color_xvycc), 
			(u8 *)arg,
			sizeof(unsigned int));
		if (ret || (hdmi->ip_data.v_cfg.settings->color_xvycc > XVYCC_MAX)) {

			//mutex_unlock(&hdmi_setting_mutex);
			return -EFAULT;
		}
		mutex_unlock(&hdmi_setting_mutex);
		break;

	case HDMI_SET_HDMI_SRC:
		HDMI_DEBUG("[%s]HDMI_SET_ENABLE!\n", __func__);
		mutex_lock(&hdmi_setting_mutex);
		ret = copy_from_user(
			(u8 *)(&hdmi->ip_data.v_cfg.settings->hdmi_src), 
			(u8 *)arg,
			sizeof(unsigned int));
		if (ret || (hdmi->ip_data.v_cfg.settings->hdmi_src > SRC_MAX)) {
			//mutex_unlock(&hdmi_setting_mutex);
			return -EFAULT;
		}

		if (hdmi->ip_data.v_cfg.settings->hdmi_src == VITD) {
			//hdmi_general_cfg(&hdmi->ip_data.v_cfg.settings);
			hdmi_timing_cfg(pdev);
			hdmi_video_cfg(pdev);
			hdmi_packet_cfg(pdev);
			
			if (hdmi_set_mode(pdev))
				return -EINVAL;
			
			hdmi_enable_output(pdev);
		}
		mutex_unlock(&hdmi_setting_mutex);
		
		break;

	case HDMI_SET_HDCP_AUTHENTICATION:
		HDMI_DEBUG("[%s]HDMI_SET_HDCP_AUTHENTICATION!\n", __func__);
		ret = copy_from_user((u8 *)(&hdcp_switch), (u8 *)arg, sizeof(bool));
		if (ret) {
			return -EFAULT;
		}

		hdcp_authentication_switch(hdcp_switch);
		
		break;
		
	case HDMI_SET_ENABLE:
		HDMI_DEBUG("[%s]HDMI_SET_ENABLE!\n", __func__);
		mutex_lock(&hdmi_enable_mutex);
		ret = copy_from_user((u8 *)(&enabled), (u8 *)arg,
			sizeof(unsigned int));
		if (ret) {
			return -EFAULT;
		}
		
		if (enabled) {
			hdmi_enable_output(pdev);

		} else {
			hdmi_disable_output(pdev);
		}
		mutex_unlock(&hdmi_enable_mutex);
		break;

	default:
		break;
	}

	//HDMI_DEBUG("[ %s finished]\n", __func__);
       
	return 0;
			
}

unsigned int asoc_hdmi_poll(struct file *filp, poll_table *wait)
{
	u32 mask = 0;
	//HDMI_DEBUG("[ %s finished]\n", __func__);
	return mask;
	
}


/*hdmi misc device*/
static struct file_operations asoc_hdmi_fops = {
	.owner	    	=	THIS_MODULE,
	.open	    	=	asoc_hdmi_open,
	.unlocked_ioctl	=	asoc_hdmi_ioctl,
	.poll	    	=	asoc_hdmi_poll,
};

struct miscdevice hdmi_miscdev = {
	.minor	= 20,
	.name	= "hdmi",
	.fops	= &asoc_hdmi_fops,
};

static unsigned int hdmi_panel_get_devclk_khz(
	struct asoc_display_device *disp_dev,
	unsigned int src_clk_khz,
	const struct de_video_mode *mode)
{	
	//HDMI_DEBUG("[ %s start]\n", __func__);
	//HDMI_DEBUG("[ %s finished]\n", __func__);
	return 0;
}


/*
 * change display mode.
 */
static int hdmi_panel_update_mode(struct asoc_display_device *disp_dev)
{

	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;
	int i, flag = 0;
	struct de_video_mode *mode;
	
	//HDMI_DEBUG ("[%s start]\n", __func__);
	/*if receive pm shutdown notifier, dont do this*/
	if (hdmi_get_pm_hibernation_prepare_flag()) 
		return 0;

                printk("[%s]hdcp_switch:%d, read_hdcp_success:%d,hdcp_authentication_success:%d,  \
	    	 sink_cap_available:%d\n", __func__,
	    	 hdmi->ip_data.config.hdcp_switch, 
	    	 hdmi->ip_data.hdcp.read_hdcp_success, 
	    	 hdmi->ip_data.hdcp.hdcp_authentication_success,
	    	 hdmi->sink_cap_available);
	if (!hdmi->ip_data.config.hdcp_switch || 
		(hdmi->ip_data.config.hdcp_switch 
		    && hdmi->ip_data.hdcp.read_hdcp_success
		    && hdmi->ip_data.hdcp.hdcp_authentication_success)) {
		if (hdmi->sink_cap_available) {
			for (i = 0; i <ARRAY_SIZE(hdmi->orderd_table); i++) {
				if (!hdmi->orderd_table[i])
					continue;
			
				if (hdmi->orderd_table[i] == hdmi->ip_data.v_cfg.settings->vid) {
	                 	 		flag = 1;
					printk("[%s]find selected vid\n", __func__ );		                                                
					
	                  			break;
	               		}
	                 
			}
	
			if (!flag) {
				if (hdmi->orderd_table[0]) {
					hdmi->ip_data.v_cfg.settings->vid = hdmi->orderd_table[0];
				}
			}
		}
	} else if (hdmi->ip_data.config.hdcp_switch) {
		if (!hdmi->ip_data.hdcp.read_hdcp_success ||
			(hdmi->ip_data.hdcp.read_hdcp_success 
			    && !hdmi->ip_data.hdcp.hdcp_authentication_success)) {
			if (hdmi->sink_cap_available) {
				for (i = sizeof(hdmi->orderd_table) / sizeof(unsigned int) - 1; i >=0; i--) {
	                  			if (hdmi->orderd_table[i]) {
		        				printk("i:%d,hdmi.orderd_table[i]:%d\n", i,hdmi->orderd_table[i]);
		        				hdmi->ip_data.v_cfg.settings->vid = hdmi->orderd_table[i];
		        				break;
	                  			}
                           			}
			} else {
				hdmi->ip_data.v_cfg.settings->vid = VID720x576P_50_16VS9;
			}
		} 
	}

              printk("[%s]hdmi current vid:%d\n", __func__, hdmi->ip_data.v_cfg.settings->vid );
              mode = (struct de_video_mode *)hdmi_get_id(VIDEO_DE);
              if (mode) {
	    hdmi_data->disp_cur_mode = mode;           
	    hdmi_data->fb_blank = FB_BLANK_UNBLANK;
	    if ( hdmi->ip_data.v_cfg.settings->pixel_encoding ==
                      VIDEO_PEXEL_ENCODING_RGB) {
                      hdmi_data->disp_cur_mode->flag |= 0x1;
	    } else if (hdmi->ip_data.v_cfg.settings->pixel_encoding ==
                      VIDEO_PEXEL_ENCODING_YCbCr444) {
                      hdmi_data->disp_cur_mode->flag &= (~0x1);
	    }
		
            } else {
                 HDMI_DEBUG("[%s]don't support this hdmi_display_mode!\n", __func__);
            }
	
	mutex_lock(&hdmi_setting_mutex);
	hdmi_timing_cfg(disp_dev);
	hdmi_video_cfg(disp_dev);
	hdmi_packet_cfg(disp_dev);
	if (hdmi_set_mode(disp_dev))
		return -EINVAL;
	mutex_unlock(&hdmi_setting_mutex);
	
	//HDMI_DEBUG("%s finished\n", __func__);
	
	return 0;
}


static int hdmi_panel_update_status(struct asoc_display_device *disp_dev)
{
	int state = disp_dev->state;
	//HDMI_DEBUG("[%s]disp_dev->state is %d\n", __func__, state);

	/*if receive pm shutdown notifier, dont do this*/
	if (hdmi_get_pm_hibernation_prepare_flag())
		return 0;

	if (state) {
		printk("[%s], enable hdmi\n", __func__);
		hdmi_enable_output(disp_dev);
		
	} else {
		printk("[%s], disable hdmi!\n", __func__);
		hdmi_disable_output(disp_dev);
	}
	
	
	//HDMI_DEBUG("[%s finished]\n", __func__);
	
	return 0;
}


/* get hdmi's status ,enable or disalbe */
static int hdmi_panel_get_status(struct asoc_display_device *disp_dev)
{
	printk ("[%s]hdmi current status:%d\n", __func__, disp_dev->state);
	return disp_dev->state;
}

static void hdmi_panel_remove(struct asoc_display_device *disp_dev)
{
	//HDMI_DEBUG ("[%s start]\n", __func__);
	hdmi_data =NULL;
	//HDMI_DEBUG("[ %s finished]\n", __func__);
}

static int hdmi_panel_probe(struct asoc_display_device *disp_dev)
{	
	int ret = 0;
	
	//HDMI_DEBUG("[ %s start]\n", __func__);
	
	hdmi_data = disp_dev;

	ret = misc_register(&hdmi_miscdev);
	if (ret) {
		printk("[%s]misc register hdmi failed!\n", __func__);
		return ret;
	}
	asoc_hdmi_create_sysfs(&hdmi_miscdev);
	dev_set_drvdata(hdmi_miscdev.this_device, disp_dev);
	//tell quickboot not save these regs
	set_quickboot_not_control_reg();

	return hdmi_basic_init(disp_dev);
}
/*
 * receive pm shutdown notifyer before suspend,send uevent(0) for next quickboot if hdmi is on when quickshutdown
 */
static int hdmi_pm_notify(struct notifier_block *nb, unsigned long event, void *dummy)
{
	if (event == PM_HIBERNATION_PREPARE) {	
		hdmi_set_pm_hibernation_prepare_flag(true);
		hdmi_set_switch(0);
	}
	return NOTIFY_OK;
}
static struct notifier_block hdmi_pm_notifier = {
	.notifier_call = hdmi_pm_notify,
};

static struct asoc_display_driver hdmi_panel_driver = {
	.driver = {
		   .name = "act_hdmi",
		   .owner = THIS_MODULE,
	},
	.probe = hdmi_panel_probe,
	.remove = hdmi_panel_remove,
	.get_status = hdmi_panel_get_status,
	.update_status = hdmi_panel_update_status,	
	.update_mode = hdmi_panel_update_mode,	
	.get_devclk_khz = hdmi_panel_get_devclk_khz,
};

static int asoc_hdmi_probe(struct platform_device *pdev)
{
	int ret = 0;

	//HDMI_DEBUG ("[%s start]\n", __func__);

	i2c_hdmi_init();
	
	ret = asoc_display_driver_register(&hdmi_panel_driver);
	if (ret) {
		printk("[%s]register hdmi display driver failed!\n", __func__);
		return ret;
	}
	
 	register_pm_notifier(&hdmi_pm_notifier);
	//HDMI_DEBUG("[ %s finished]\n", __func__);

	return ret;
}

static int asoc_hdmi_remove(struct platform_device *pdev)
{

	//HDMI_DEBUG ("[%s start]\n", __func__);
	/* disable hdmi */
	hdmi_disable_output(hdmi_data);
	/* disalbe hdmi cmu */
	//module_clk_disable(MOD_ID_HDMI);
	asoc_tvoutpll_disable(CLK_NAME_HDMI24M_CLK);

	asoc_hdmi_remove_sysfs(&hdmi_miscdev);
	misc_deregister(&hdmi_miscdev);

	asoc_display_driver_unregister(&hdmi_panel_driver);

	//HDMI_DEBUG("[ %s finished]\n", __func__);
	return 0;

}

static void asoc_hdmi_shutdown(struct platform_device *pdev)
{

	HDMI_DEBUG ("[%s]\n", __func__);
	/* disable hdmi */
	hdmi_set_disconnect_flag(true);
	hdmi_disable_output(hdmi_data);
	hdmi_set_disconnect_flag(false);
	//HDMI_DEBUG("[ %s finished]\n", __func__);

}

#ifdef CONFIG_PM
static int asoc_hdmi_suspend(struct device *dev)
{
	struct clk *hdmi_clk;
	int rate;
	
	HDMI_DEBUG("[%s]hdmi ready to suspend!\n", __func__);
	
	hdmi_clk = clk_get_sys(CLK_NAME_HDMI_CLK, NULL);
	clk_reset(hdmi_clk);

	/* sync tvoutpll1 with kernel*/
	asoc_tvoutpll_or_clk_set_rate(CLK_NAME_TVOUT1PLL, 80000); 
	rate = asoc_tvoutpll_mux_set_parent(CLK_NAME_TVOUT1PLL);
	/* sync tvoutpll0 with kernel*/
	asoc_tvoutpll_or_clk_set_rate(CLK_NAME_TVOUT0PLL, 100800); 
	rate = asoc_tvoutpll_mux_set_parent(CLK_NAME_TVOUT0PLL);
	asoc_tvoutpll_or_clk_set_rate(CLK_NAME_TVOUT0_CLK, 100800);
	/*sync deepcolor with kernel*/
	asoc_deepcolorpll_set_rate(_3D_NOT, DEEP_COLOR_24_BIT);

	/*disable tvoutpll for sync with quickboot resume */
	asoc_tvoutpll_disable(CLK_NAME_DEEPCOLORPLL);
	asoc_tvoutpll_disable(CLK_NAME_TVOUT0PLL);
	asoc_tvoutpll_disable(CLK_NAME_TVOUT1PLL);	

	//samsung would cause irq, when disable hdmi 
	if (!hdmi_get_suspend_flag())
		hdmi_set_suspend_flag(true);
	//for alarm 
	hdmi_set_switch(0);
	hdcp_authentication_switch(false);
	
	//HDMI_DEBUG("[ %s finished]\n", __func__);

	return 0;
}
static int asoc_hdmi_resume(struct device *dev)
{
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)hdmi_data->disp_priv_info;
	
	printk("[%s]hdmi ready to resume!\n", __func__);

	if (hdmi->ip_data.ops->is_hpdint_enabled(&hdmi->ip_data) == HPDINT_ENABLE) {//quickboot resume
		//for de
		struct de_video_mode *mode;
		hdmi->ip_data.v_cfg.settings->vid = VID1280x720P_50_16VS9;
		mode = (struct de_video_mode *)hdmi_get_id(VIDEO_DE);
		if (mode) {
			hdmi_data->disp_cur_mode = mode;           
			hdmi_data->fb_blank = FB_BLANK_UNBLANK;
		
		} else {
			printk("[%s]don't support this hdmi_display_mode!\n", __func__);
		}
		
	} 

	hdmi_init_config(hdmi_data);

	//HDMI_DEBUG("[ %s finished]\n", __func__);

	return 0;

}

static SIMPLE_DEV_PM_OPS(asoc_hdmi_pm_ops, asoc_hdmi_suspend, asoc_hdmi_resume);
#endif

static struct platform_device asoc_hdmi_device = {
	.name      = "asoc_hdmi",
	.id        = -1,
};

static struct platform_driver asoc_hdmi_driver = {
	.probe          = asoc_hdmi_probe,
	.remove         = asoc_hdmi_remove,
	.shutdown = asoc_hdmi_shutdown,
	.driver         = {
		.name   = "asoc_hdmi",
		.owner  = THIS_MODULE,
		
#ifdef CONFIG_PM
		.pm	= &asoc_hdmi_pm_ops,
#endif
	},
	
};

static int __init asoc_hdmi_init(void)
{
	platform_device_register(&asoc_hdmi_device);
	return platform_driver_register(&asoc_hdmi_driver);
}

static void __exit asoc_hdmi_exit(void)
{
	//HDMI_DEBUG("[ %s start]\n", __func__);
	unregister_pm_notifier(&hdmi_pm_notifier);
	platform_device_unregister(&asoc_hdmi_device);
	platform_driver_unregister(&asoc_hdmi_driver);
	
}

module_init(asoc_hdmi_init);
module_exit(asoc_hdmi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("chenbo");

MODULE_DESCRIPTION("Asoc hdmi drvier");
MODULE_AUTHOR("chenbo <chenbo@actions-semi.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:hdmi");

