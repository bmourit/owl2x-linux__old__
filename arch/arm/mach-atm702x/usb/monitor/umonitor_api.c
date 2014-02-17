#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/kthread.h>
#include <linux/switch.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/wakelock.h>
#include <linux/suspend.h>

#include <mach/hardware.h>
#include <mach/atc260x/atc260x.h>

#include <asm/uaccess.h>
#include <asm/system.h>

#include "umonitor_config.h"
#include "umonitor_core.h"
#include "aotg_regs.h"

#define CONNECT_UBB_PORT	1
#define CONNECT_USB_ADAPTOR		2

unsigned int otgvbus_gpio;
struct delayed_work monitor_work;
atomic_t wake_lock_atomic;
unsigned int monitor_work_pending;
unsigned int wake_lock_register_cnt;

extern umonitor_dev_status_t *umonitor_status;

struct mon_sysfs_status {
	unsigned int charger_connected;
	unsigned int pc_connected;
	unsigned int udisk_connected;
};

struct port_dev{
	struct switch_dev sdev;
	char state_msg[64];
	volatile unsigned int port_flag;
};
	
struct monitor_con {
	spinlock_t lock;
	struct port_dev port_dev;
	umon_port_config_t port_config;
	umonitor_api_ops_t port_ops;
	struct kobject *mon_obj;	/* just for sysfs. */
	struct kobject *mon_port;
	struct mon_sysfs_status port_status;
	
	atomic_t port_exit;
	struct task_struct *tsk;
	wait_queue_head_t mon_wait;

	#define MON_REQ_PORT	0x1
	volatile unsigned int req_flag;
	struct timer_list port_timer;
	struct timer_list check_timer;
	volatile unsigned int check_flag;
	struct wake_lock monitor_wake_lock;
	void __iomem  *base;
};

struct monitor_con monitor_mon_dev;
static struct monitor_con *my_mon = &monitor_mon_dev;
static unsigned long pm_status;
static ssize_t mon_status_show(struct kobject *kobj, 
	  struct kobj_attribute *attr, char *buf);
static ssize_t mon_config_show(struct kobject *kobj, 
	  struct kobj_attribute *attr, char *buf);
static ssize_t mon_config_store(struct kobject *kobj,
	  struct kobj_attribute *attr, const char *instr, size_t bytes);

#define ATTRCMP(x) (0 == strcmp(attr->attr.name, #x))
#define MONITOR_ATTR_LIST(x)	(&x ## _attribute.attr)
#define MON_STATUS_ATTR(x)	static struct kobj_attribute x##_attribute =   \
	__ATTR(x, S_IRUGO, mon_status_show,  NULL)
#define MON_CONFIG_ATTR(x)	static struct kobj_attribute x##_attribute =   \
	__ATTR(x, (S_IRUSR|S_IRGRP|S_IWUSR|S_IWGRP), mon_config_show, mon_config_store)

/* default just for usb port0. */
static void mon_port_putt_msg(unsigned int msg);
static int check_driver_state(void);
int usb_set_vbus_power(int value);

static int mon_thread_port(void *data)
{
	unsigned int mtime = 0;
	MONITOR_PRINTK("entring mon_thread_port\n");

	while (!kthread_should_stop() && (atomic_read(&my_mon->port_exit) == 0)) {
		wait_event_interruptible(my_mon->mon_wait, (my_mon->req_flag != 0) ||
		        (my_mon->port_dev.port_flag != 0 ) || kthread_should_stop() ||
		        (my_mon->check_flag == 1));
    my_mon->check_flag = 0;
		check_driver_state();

		if (my_mon->req_flag & MON_REQ_PORT) {
			umonitor_timer_func();
			mtime = umonitor_get_timer_step_interval();
			spin_lock(&my_mon->lock);
      mod_timer(&my_mon->port_timer, jiffies+(mtime*HZ)/1000);
			my_mon->req_flag &= ~(MON_REQ_PORT);
			spin_unlock(&my_mon->lock);
		}
	}

	/* mon_thread_port0 already exited */
	MONITOR_PRINTK("monitor: mon_thread_port0 is going to exit.\n");
	atomic_set(&my_mon->port_exit, 0);
	return 0;
}

static void mon_timer_func_port(unsigned long h)
{
	if (!spin_trylock(&my_mon->lock)) {
		MONITOR_PRINTK("timer0 my_mon->lock unavailable!\n");
		mod_timer(&my_mon->port_timer, jiffies+1);
		return;
	}
	my_mon->req_flag |= MON_REQ_PORT;
	spin_unlock(&my_mon->lock);
	wake_up(&my_mon->mon_wait);
	return;
}


static void mon_timer_func_check(unsigned long h)
{
  my_mon->check_flag = 1;
	mod_timer(&my_mon->check_timer, jiffies + (CHECK_TIMER_INTERVAL*HZ)/1000);
	//check_driver_state();
	
	wake_up(&my_mon->mon_wait);
	return;
}

static ssize_t umonitor_switch_state(struct switch_dev *sdev, char *buf)
{
	struct port_dev	*port_dev = container_of(sdev, struct port_dev, sdev);
	return sprintf(buf, "%s\n",port_dev->state_msg);
}

/*release wake lock*/
static void monitor_release_wakelock(struct work_struct *work)
{
	wake_lock_register_cnt --;
	wake_unlock(&my_mon->monitor_wake_lock);;
}

static int monitor_probe(struct platform_device *_dev)
{
	int ret = 0;
	void __iomem  *base;

	/*init &my_mon->lock */
	spin_lock_init(&my_mon->lock);
	my_mon->tsk = NULL;

	/* status init. */
	my_mon->port_status.charger_connected = 0;
	my_mon->port_status.pc_connected = 0;
	my_mon->port_status.udisk_connected = 0;

/*	
  res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		printk("missing memory base resource\n");
		return -EINVAL;
	}
	
  base = devm_ioremap(dev, res->start, resource_size(res));
*/

  base = (void __iomem *)IO_ADDRESS(USB3_REGISTER_BASE);
	if (!base) {
		MONITOR_ERR("ioremap failed\n");
		return -ENOMEM;
	}
	my_mon->base = base;
	MONITOR_PRINTK("my_mon->base is %08x\n",(unsigned int )my_mon->base);
	
  ret = umonitor_core_init(&my_mon->port_ops, &my_mon->port_config, (unsigned int)my_mon->base);
	if (ret != 0) 
		  return ret;
		  
	/* add for switch device. */
	my_mon->port_dev.sdev.name = "monitor_dev";
	my_mon->port_dev.sdev.print_state = umonitor_switch_state;
	ret = switch_dev_register(&my_mon->port_dev.sdev);
	if (ret < 0) {
		MONITOR_ERR("failed to register switch dev0 in umonitor. \n");
		goto err_switch_dev_register;
	}
	
	/*WAKE_LOCK_SUSPEND 为suspend lock，还有一种idle lock*/
	MONITOR_PRINTK(KERN_EMERG"%s--%d, initalize a wake_lock\n", __FILE__, __LINE__);
	wake_lock_init(&my_mon->monitor_wake_lock, WAKE_LOCK_SUSPEND, "usb_monitor");
	
	init_waitqueue_head(&my_mon->mon_wait);
	my_mon->req_flag = 0;

	my_mon->tsk =
	    kthread_create(mon_thread_port, NULL, "usb_monitor");

	if (IS_ERR(my_mon->tsk)) {
		ret = PTR_ERR(my_mon->tsk);
		my_mon->tsk = NULL;
		MONITOR_ERR("err: create monitor thread failed\n");
		goto out;
	}
	wake_up_process(my_mon->tsk);
  /*register timer and function "mon_timer_func"*/
	setup_timer(&my_mon->port_timer, mon_timer_func_port, 0);
	my_mon->port_timer.expires = jiffies + HZ/2;
	add_timer(&my_mon->port_timer);
	
  	setup_timer(&my_mon->check_timer, mon_timer_func_check, 0);
	my_mon->check_timer.expires = jiffies + HZ/2;
	add_timer(&my_mon->check_timer);
	
	monitor_work_pending =0;
	wake_lock_register_cnt =0;
	INIT_DELAYED_WORK(&monitor_work, monitor_release_wakelock);
	
	return 0;
out:
	switch_dev_unregister(&my_mon->port_dev.sdev);
err_switch_dev_register:
	return ret;
}

static int __exit monitor_remove(struct platform_device *_dev)
{
  del_timer_sync(&my_mon->port_timer);
	del_timer_sync(&my_mon->check_timer);
	//wake_unlock(&my_mon->monitor_wake_lock);
	wake_lock_destroy(&my_mon->monitor_wake_lock);

	/* wait for mon_thread exit */
	if (my_mon->tsk != NULL) {
		atomic_set(&my_mon->port_exit, 1);
		kthread_stop(my_mon->tsk);
	}
	
	msleep(2);
	if (atomic_read(&my_mon->port_exit) != 0) {
		MONITOR_ERR("error unable to stop mon_thread_port\n");
	}
	my_mon->tsk = NULL;

	switch_dev_unregister(&my_mon->port_dev.sdev);
	MONITOR_PRINTK("monitor driver remove successufully\n");
	return 0;
}

static int monitor_suspend(struct platform_device *pdev, pm_message_t state)
{
  unsigned int mtime;
	
  if (pm_status == PM_HIBERNATION_PREPARE) {
      return 0;
  }
  
  mtime = 0x70000000;
  umonitor_core_suspend();
		
	//delay timer for a long time;may reset timer in resume	
  mod_timer(&my_mon->port_timer, jiffies+(mtime*HZ)/1000);
  mod_timer(&my_mon->check_timer, jiffies+(mtime*HZ)/1000);
	
	return 0;
}

static int monitor_resume(struct platform_device *pdev)
{
    if (pm_status == PM_HIBERNATION_PREPARE) {
        return 0;
    }
    
    MONITOR_PRINTK(KERN_INFO "monitor resume!\n");
    
    umonitor_core_resume();
    /*mod timer*/
    mod_timer(&my_mon->port_timer, jiffies+(500*HZ)/1000);
    mod_timer(&my_mon->check_timer, jiffies+(CHECK_TIMER_INTERVAL*HZ)/1000);
	
  return 0;
}

static void monitor_shutdown(struct platform_device *pdev)
{
    umonitor_core_suspend();
}


static struct platform_driver monitor_driver = {
	.probe = monitor_probe,
	.remove = monitor_remove,
	.suspend = monitor_suspend,
	.resume = monitor_resume,
	.shutdown = monitor_shutdown,
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "usb_monitor",
		   },
};

static void monitor_release(struct device *dev)
{
	MONITOR_PRINTK("monitor_release\n");
}

static int monitor_pm_notify(struct notifier_block *nb, unsigned long event, void *dummy)
{
	unsigned int mtime = 0x70000000;
	
    if (event == PM_HIBERNATION_PREPARE) {
	    pm_status = PM_HIBERNATION_PREPARE;
        umonitor_core_suspend();
    	//delay timer for a long time;may reset timer in resume	
    	mod_timer(&my_mon->port_timer, jiffies+(mtime*HZ)/1000);
    	mod_timer(&my_mon->check_timer, jiffies+(mtime*HZ)/1000);
	} else if (event == PM_POST_HIBERNATION) {
	    pm_status = 0;
        MONITOR_PRINTK(KERN_INFO "monitor resume!\n");
         
        umonitor_core_resume();
        /*mod timer*/
        mod_timer(&my_mon->port_timer, jiffies+(500*HZ)/1000);
        mod_timer(&my_mon->check_timer, jiffies+(CHECK_TIMER_INTERVAL*HZ)/1000);
	}
	
	return NOTIFY_OK;
}

static struct notifier_block monitor_pm_notifier = {
	.notifier_call = monitor_pm_notify,
};

static int leopard_hibernate_notifier_event(struct notifier_block *this,
					     unsigned long event, void *ptr)
{
    
	switch (event) {
	case PM_HIBERNATION_PREPARE:
        //umonitor_dwc_otg_init();
	    break;
	case PM_POST_HIBERNATION:       //第二次开机时，成功恢复到第一次关机的状态时调用
	    break;
	}
	return NOTIFY_OK;
}

static struct notifier_block leopard_hibernate_notifier = {
	.notifier_call = leopard_hibernate_notifier_event,
};


/*
static struct resource xhci_resources[] = {
	[0] = {
		.start	= USB3_REGISTER_BASE,
		.end	= USB3_REGISTER_BASE + DWC3_ACTIONS_REGS_END,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_ASOC_USB,
		.end	= IRQ_ASOC_USB,
		.flags	= IORESOURCE_IRQ,
	},
};
*/

static struct platform_device monitor_device = {
	.name = "usb_monitor",
	.id = -1,
	.num_resources = 0,
	.resource = NULL,
	.dev = {
		.release = monitor_release,
		},
};

#if  1				/* add sysfs info. */
MON_STATUS_ATTR(charger_connected);
MON_STATUS_ATTR(pc_connected);
MON_STATUS_ATTR(udisk_connected);

MON_CONFIG_ATTR(run);
MON_CONFIG_ATTR(detect_type);
/* for idpin config. */
MON_CONFIG_ATTR(idpin_type);
MON_CONFIG_ATTR(idpin_gpio_group);
MON_CONFIG_ATTR(idpin_gpio_no);
/* for vbus config. */
MON_CONFIG_ATTR(vbus_type);
MON_CONFIG_ATTR(vbus_gpio_group);
MON_CONFIG_ATTR(vbus_gpio_no);
/* for host power supply config. */
MON_CONFIG_ATTR(poweron_gpio_group);
MON_CONFIG_ATTR(poweron_gpio_no);
MON_CONFIG_ATTR(poweron_active_level);
/* in host state, if vbus power switch onoff use gpio, set it. */
MON_CONFIG_ATTR(power_switch_gpio_group);
MON_CONFIG_ATTR(power_switch_gpio_no);
MON_CONFIG_ATTR(power_switch_active_level);


static struct attribute *mon_status_attrs[] = {
	MONITOR_ATTR_LIST(charger_connected),
	MONITOR_ATTR_LIST(pc_connected),
	MONITOR_ATTR_LIST(udisk_connected),
	NULL,			/* terminator */
};

static struct attribute_group mon_port_status = {
	.name = "status",
	.attrs = mon_status_attrs,
};

static struct attribute *mon_config_attrs[] = {
	MONITOR_ATTR_LIST(run),
	MONITOR_ATTR_LIST(detect_type),
	/* for idpin config. */
	MONITOR_ATTR_LIST(idpin_type),
	MONITOR_ATTR_LIST(idpin_gpio_group),
	MONITOR_ATTR_LIST(idpin_gpio_no),
	/* for vbus config. */
	MONITOR_ATTR_LIST(vbus_type),
	MONITOR_ATTR_LIST(vbus_gpio_group),
	MONITOR_ATTR_LIST(vbus_gpio_no),
	/* for host power supply config. */
	MONITOR_ATTR_LIST(poweron_gpio_group),
	MONITOR_ATTR_LIST(poweron_gpio_no),
	MONITOR_ATTR_LIST(poweron_active_level),
	/* in host state, if vbus power switch onoff use gpio, set it. */
	MONITOR_ATTR_LIST(power_switch_gpio_group),
	MONITOR_ATTR_LIST(power_switch_gpio_no),
	MONITOR_ATTR_LIST(power_switch_active_level),
	NULL,			/* terminator */
};

static struct attribute_group mon_port_config = {
	.name = "config",
	.attrs = mon_config_attrs,
};


static ssize_t mon_status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	struct mon_sysfs_status *p_status;

	if (kobj == my_mon->mon_port){
		p_status = &my_mon->port_status;
  }else {
		return -ENOENT;
	}

	if (ATTRCMP(charger_connected)) {
		return sprintf(buf, "%d\n", p_status->charger_connected);
	} else if (ATTRCMP(pc_connected)) {
		return sprintf(buf, "%d\n", p_status->pc_connected);
	} else if (ATTRCMP(udisk_connected)) {
		return sprintf(buf, "%d\n", p_status->udisk_connected);
	} else {
	}
	return -ENOENT;
}

static ssize_t mon_config_show(struct kobject *kobj, 
	  struct kobj_attribute *attr, char *buf)
{
	umon_port_config_t *pconfig;

	if (kobj == my_mon->mon_port) {
		pconfig = &my_mon->port_config;
	} else {
		return -ENOENT;
	}

	if (ATTRCMP(run)) {
		return sprintf(buf, "%d\n", umonitor_get_run_status());
	} else if(ATTRCMP(detect_type)) {
		return sprintf(buf, "%d\n", pconfig->detect_type);
	} else if(ATTRCMP(idpin_type)) {
		return sprintf(buf, "%d\n", pconfig->idpin_type);
	} else if(ATTRCMP(idpin_gpio_group)) {
		return sprintf(buf, "%d\n", pconfig->idpin_gpio_group);
	} else if(ATTRCMP(idpin_gpio_no)) {
		return sprintf(buf, "%d\n", pconfig->idpin_gpio_no);
	} else if(ATTRCMP(vbus_type)) {
		return sprintf(buf, "%d\n", pconfig->vbus_type);
	} else if(ATTRCMP(vbus_gpio_group)) {
		return sprintf(buf, "%d\n", pconfig->vbus_gpio_group);
	} else if(ATTRCMP(vbus_gpio_no)) {
		return sprintf(buf, "%d\n", pconfig->vbus_gpio_no);
	} else if(ATTRCMP(poweron_gpio_group)) {
		return sprintf(buf, "%d\n", pconfig->poweron_gpio_group);
	} else if(ATTRCMP(poweron_gpio_no)) {
		return sprintf(buf, "%d\n", pconfig->poweron_gpio_no);
	} else if(ATTRCMP(poweron_active_level)) {
		return sprintf(buf, "%d\n", pconfig->poweron_active_level);
	} else if(ATTRCMP(power_switch_gpio_group)) {
		return sprintf(buf, "%d\n", pconfig->power_switch_gpio_group);
	} else if(ATTRCMP(power_switch_gpio_no)) {
		return sprintf(buf, "%d\n", pconfig->power_switch_gpio_no);
	} else if(ATTRCMP(power_switch_active_level)) {
		return sprintf(buf, "%d\n", pconfig->power_switch_active_level);
	} else {
		/* do nothing. */
	}

	return -ENOENT;
}

static ssize_t mon_config_store(struct kobject *kobj,
	  struct kobj_attribute *attr, const char *instr, size_t bytes)
{
	unsigned long val;
	unsigned int ret, plugstatus;
	umon_port_config_t *pconfig;

	if (kobj == my_mon->mon_port) {
		pconfig = &my_mon->port_config;
	} else {
		MONITOR_PRINTK("store no err!\n");
		goto out;
	}

	ret = strict_strtoul(instr, 0, &val);
	MONITOR_PRINTK("strict_strtoul:%d\n", (unsigned int)val);

	if (ATTRCMP(run)) {
		if(val == 1) {
  		umonitor_detection(val);
		}else if (val == 2) {
			MONITOR_PRINTK("vbus on off\n");
			usb_set_vbus_power(0);
			mdelay(500);
			plugstatus = umonitor_get_message_status();
			if( (plugstatus & (0x1 << MONITOR_A_IN)) !=0)
				usb_set_vbus_power(1);
      
      } else if (val == 3) {
          mon_port_putt_msg(MON_MSG_USB_B_OUT);
      } else if (val == 4) {
          mon_port_putt_msg(MON_MSG_USB_B_IN);
      }
      else {
      } 
	} else if(ATTRCMP(detect_type)) {
		pconfig->detect_type = val;
	} else if(ATTRCMP(idpin_type)) {
		pconfig->idpin_type = val;
	} else if(ATTRCMP(idpin_gpio_group)) {
		pconfig->idpin_gpio_group = val;
	} else if(ATTRCMP(idpin_gpio_no)) {
		pconfig->idpin_gpio_no = val;
	} else if(ATTRCMP(vbus_type)) {
		pconfig->vbus_type = val;
	} else if(ATTRCMP(vbus_gpio_group)) {
		pconfig->vbus_gpio_group = val;
	} else if(ATTRCMP(vbus_gpio_no)) {
		pconfig->vbus_gpio_no = val;
	} else if(ATTRCMP(poweron_gpio_group)) {
		pconfig->poweron_gpio_group = val;
	} else if(ATTRCMP(poweron_gpio_no)) {
		pconfig->poweron_gpio_no = val;
	} else if(ATTRCMP(poweron_active_level)) {
		pconfig->poweron_active_level = val;
	} else if(ATTRCMP(power_switch_gpio_group)) {
		pconfig->power_switch_gpio_group = val;
	} else if(ATTRCMP(power_switch_gpio_no)) {
		pconfig->power_switch_gpio_no = val;
	} else if(ATTRCMP(power_switch_active_level)) {
		/* backdoor for debug. */
		if (val == 2) {
			umonitor_printf_debuginfo();
		}
		pconfig->power_switch_active_level = val;
	} else {
		/* do nothing. */
		MONITOR_PRINTK("store attr err!\n");
	}

	out:
	return bytes;
}

int mon_sysfs_init(void)
{
	int ret = 0;

	my_mon->mon_obj = kobject_create_and_add("monitor", NULL);
	if (!my_mon->mon_obj) {
		MONITOR_ERR("unable to create monitor kobject\n");
	}
	my_mon->mon_port = kobject_create_and_add("usb_port", my_mon->mon_obj);
	if (!my_mon->mon_port) {
		MONITOR_ERR("unable to create usb_port kobject\n");
	}

	ret = sysfs_create_group(my_mon->mon_port, &mon_port_status);
	if (ret != 0) {
		MONITOR_ERR("create usb_port status group failed\n");
	}
	ret = sysfs_create_group(my_mon->mon_port, &mon_port_config);
	if (ret != 0) {
		MONITOR_ERR("create usb_port cofig group failed\n");
	}
	/* ignore failed case. */
	return 0;
}

int mon_sysfs_exit(void)
{
	sysfs_remove_group(my_mon->mon_port, &mon_port_status);
	sysfs_remove_group(my_mon->mon_port, &mon_port_config);
	kobject_del(my_mon->mon_port);
	kobject_del(my_mon->mon_obj);
	return 0;
}
#endif				/* add sysfs info. */

static int check_driver_state(void)
{
	usb_hal_monitor_t * p_hal;
	unsigned int val;
	unsigned int message;
	
	p_hal = &umonitor_status->umonitor_hal;
	message = umonitor_status->message_status;
	
	if(umonitor_status->detect_valid == 0){
    if (umonitor_status->det_phase == 0){
      if(((message & (0x1 << MONITOR_B_IN)) != 0)||
              ((message & (0x1 << MONITOR_CHARGER_IN)) != 0)){	  
        if(( my_mon->port_status.charger_connected == CONNECT_USB_ADAPTOR) && (wake_lock_register_cnt  == 1)) {
					printk("wakelock release!!!!!!!!\n");
        	wake_lock_register_cnt --;
        	wake_unlock(&my_mon->monitor_wake_lock);
        }
        umonitor_status->vbus_status = (unsigned char) p_hal->get_vbus_state(p_hal);
        if(umonitor_status->vbus_status == USB_VBUS_LOW) {
            my_mon->port_status.charger_connected = 0;
            umonitor_detection(1);
            //wake_up(&my_mon->mon_wait);
        }
      }
    }else{  
      /*host set monitor flag*/
      if((message & (0x1 << MONITOR_A_IN)) != 0){
//        val = p_hal->get_linestates(p_hal);
//        if((val == 0x1) || (val == 0x2))
//          return 0;
        val = p_hal->get_idpin_state(p_hal);
        if(val == USB_ID_STATE_HOST)
          return 0;
        umonitor_detection(1); 
        //wake_up(&my_mon->mon_wait);   
      }
    }
  }
	return 0;
}



int notify_driver_state(int driver_state, int driver_type)
{
  return 0;
}
EXPORT_SYMBOL(notify_driver_state);

/*
int update_driver_state(enum UPDATA_DRIVER_COMMAND command_type, int type_para)
{
	int ret = 0;
	struct port_dev	*port_dev;
	int port_num;
	static enum USBLINE_WORK_STAT working_state;
	static int date_state;
	
	printk("%s: cmd %x, para: %x\n", __func__, command_type, type_para);
	port_num = (type_para == DEVICE) ? 0 : 1;
	port_dev = &my_mon->port_dev[port_num];
	
	switch (command_type) {
	case UPDATE_WORK_STAT:
		working_state = type_para;
		break;
	case UPDATE_DATA_STAT:
		date_state = date_state | type_para;
		break;
	case UPDATE_USBLINE_OUT:
		working_state = USBLINE_UNKNOWN;
		port_dev->port_flag = MON_MSG_USB_B_OUT;
		wake_up(&my_mon->mon_wait);
		break;
	case UPDATE_USBLINE_SUSPEND:
		working_state = USBLINE_SUSPEND;
		break;
	case UPDATE_UDEVICE_IN:
		break;		
	case UPDATE_UDEVICE_OUT:
		working_state = USBLINE_UNKNOWN;
		port_dev->port_flag = MON_MSG_USB_A_OUT;
		wake_up(&my_mon->mon_wait);
		break;		
		
	case UPDATE_CARD_IN:
	case SET_UPDATE_STATE:
	case GET_UPDATE_STATE:
	case UPDATE_PC_CONFIRM:
	case UPDATE_UDEVICE_CARD_IN:
	case UPDATE_UDEVICE_CARD_OUT:
	default:
		printk("unsupported command by monitor\n");
		break;
	}

	return ret;
}
EXPORT_SYMBOL_GPL(update_driver_state);
*/

static void mon_port_wakeup_func(void)
{
  my_mon->req_flag |= MON_REQ_PORT;
	wake_up(&my_mon->mon_wait);
	return;
}

static void mon_port_putt_msg(unsigned int msg)
{
	struct mon_sysfs_status * pStatus;

  pStatus = &my_mon->port_status;

	switch (msg) {
	case MON_MSG_USB_B_IN:
		//printk("%s--%d, wake_lock !!! \n", __FILE__, __LINE__);
		if(monitor_work_pending == 1) {
			cancel_delayed_work_sync(&monitor_work);
			monitor_work_pending = 0;
		}
		if(!wake_lock_register_cnt) {
			wake_lock(&my_mon->monitor_wake_lock);
			wake_lock_register_cnt++;
		}
		
		pStatus->pc_connected = 1;
		sprintf(my_mon->port_dev.state_msg, "USB_B_IN");
		switch_set_state(&my_mon->port_dev.sdev, msg);
		break;
	case MON_MSG_USB_B_OUT:
		//printk("%s--%d, wake_unlock !!! \n", __FILE__, __LINE__);
		if(wake_lock_register_cnt) {
			schedule_delayed_work(&monitor_work, msecs_to_jiffies(1000));
			monitor_work_pending = 1;
		}
			
		pStatus->pc_connected = 0;
		sprintf(my_mon->port_dev.state_msg, "USB_B_OUT");
		switch_set_state(&my_mon->port_dev.sdev, msg);
		break;
	case MON_MSG_USB_A_IN:
		pStatus->udisk_connected = 1;
		//printk("%s--%d, wake_lock !!! \n", __FILE__, __LINE__);
		wake_lock(&my_mon->monitor_wake_lock);
		sprintf(my_mon->port_dev.state_msg, "USB_A_IN");
		switch_set_state(&my_mon->port_dev.sdev, msg);
		break;
	case MON_MSG_USB_A_OUT:
		pStatus->udisk_connected = 0;
    //printk("%s--%d, wake_unlock !!! \n", __FILE__, __LINE__);
		wake_unlock(&my_mon->monitor_wake_lock);
		sprintf(my_mon->port_dev.state_msg, "USB_A_OUT");
		switch_set_state(&my_mon->port_dev.sdev, msg);
		break;
	case MON_MSG_USB_CHARGER_IN:
		pStatus->charger_connected = 1;
		//printk("%s--%d, wake_lock !!! \n", __FILE__, __LINE__);
		wake_lock(&my_mon->monitor_wake_lock);
		sprintf(my_mon->port_dev.state_msg, "USB_CHARGER_IN");
		switch_set_state(&my_mon->port_dev.sdev, msg);
		break;
	case MON_MSG_USB_CHARGER_OUT:
		pStatus->charger_connected = 0;
		printk("%s--%d, wake_unlock !!! \n", __FILE__, __LINE__);
		wake_unlock(&my_mon->monitor_wake_lock);
		sprintf(my_mon->port_dev.state_msg, "USB_CHARGER_OUT");
		switch_set_state(&my_mon->port_dev.sdev, msg);
		break;
	default:
		MONITOR_ERR("err msg:%0x\n", msg);
		break;
	}
	return;
}

int usb_set_vbus_power(int value)
{
	return umonitor_vbus_power_onoff(value);
}
EXPORT_SYMBOL_GPL(usb_set_vbus_power);

void monitor_set_usb_plugin_type(int value)
{
	my_mon->port_status.charger_connected = value;
}
EXPORT_SYMBOL_GPL(monitor_set_usb_plugin_type);
struct gpio_pre_cfg pcfg; 
static int __init monitor_init(void)
{
	int ret;
	char * name = "otgvbus_gpio";
	
	memset((void *)&pcfg, 0, sizeof(struct gpio_pre_cfg));
	gpio_get_pre_cfg(name, &pcfg);
	otgvbus_gpio = ASOC_GPIO_PORT(pcfg.iogroup, pcfg.pin_num);
  ret = gpio_request(otgvbus_gpio, name);  
	if(ret) {
		MONITOR_ERR("gpio_request err ret:%d\n",ret);
		return ret;
	}
	//power gpio oupru, power switch off
  gpio_direction_output(otgvbus_gpio, 1);   
  gpio_set_value_cansleep(otgvbus_gpio, 0);

	atomic_set(&my_mon->port_exit, 0);

	my_mon->port_ops.wakeup_func = mon_port_wakeup_func;
	my_mon->port_ops.putt_msg = mon_port_putt_msg;

	ret = platform_device_register(&monitor_device);
	if (ret < 0) {
		MONITOR_ERR("Can't register monitor platform device, ret:%d\n", ret);
	}	
		
	ret = platform_driver_register(&monitor_driver);
	if (ret < 0) {
		MONITOR_ERR("monitor driver register failed,err is %d\n", ret);
	}
	/*create director "/sys/monitor", "/sys/usb_port" and attribute file: */
	ret = mon_sysfs_init();
	if (ret < 0) {
		MONITOR_ERR("mon_sysfs_init failed,err is %d\n", ret);
	}

    register_pm_notifier(&monitor_pm_notifier);
	register_hibernate_notifier(&leopard_hibernate_notifier);

	return ret;
}

static void __exit monitor_exit(void)
{
    unregister_hibernate_notifier(&leopard_hibernate_notifier);
    unregister_pm_notifier(&monitor_pm_notifier);
	platform_driver_unregister(&monitor_driver);
	platform_device_unregister(&monitor_device);

  umonitor_core_exit();

	mon_sysfs_exit();

	gpio_free(otgvbus_gpio);
	
	MONITOR_PRINTK("end of monitor_exit\n");
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("houjingkun");

module_init(monitor_init);
module_exit(monitor_exit);

