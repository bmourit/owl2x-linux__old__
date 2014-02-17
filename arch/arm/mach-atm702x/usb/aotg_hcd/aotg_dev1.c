#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/usb/otg.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <mach/gpio.h>

#include <mach/actions_reg_gl5202.h>
#include <mach/hardware.h>
#include <linux/regulator/consumer.h>
#include "aotg_regs.h"
#include "aotg_plat_data.h"

//#define  AOTG1_MAJOR	 	223
//#define AOTG1_MINOR     	0

#define CMU_USBPLL_USBPLL0EN	0x1500
#define CMU_USBPLL_USBPLL1EN	0x3e00
#define CMU_USBPLL_USBPLLEN	0x3f00

#define CMU_DEVRST1_USB0	0x08
#define CMU_DEVRST1_USB1	0x10


static struct aotg_plat_data otg_data1 = {
	.usbecs = (void __iomem *)USB2_1ECS,
	.usbpll = (void __iomem *)CMU_USBPLL,
	.usbpll_bits = CMU_USBPLL_USBPLLEN,
	.devrst = (void __iomem *)CMU_DEVRST1,
	.devrst_bits = CMU_DEVRST1_USB1,
	.devclk = (void __iomem *)CMU_DEVCLKEN1,
	.devclk_bits = 0,
	.no_hs = 0,
};

static struct resource hcd_res1[] = {
	[0] = {
		.start = AOTG0_BASE,
		.end = AOTG0_BASE + AOTG_REGS_SIZE*2 - 1,
		.flags = IORESOURCE_MEM,
	},
	
	[1] = {
		.start = IRQ_ASOC_USB2_1,
		.end = IRQ_ASOC_USB2_1,
		.flags = IORESOURCE_IRQ,
	},
};

static u64 hcd_dmamask1 = ~(u32) 0;

static struct platform_device *aotg_dev1 = NULL;

static struct regulator *wifi33_regulator = NULL;

//#define AOTG_USE_GPIO_CTL_POWER		0
//#define GPIO_VBUS_SUPPLY	ASOC_GPIO_PORTA(21)
static int usb_gpio_vbus_pin = -1;


static int usb_wifi_get_power(void)
{
	if (usb_gpio_vbus_pin < 0)
		goto use_wifi33; 
	if (usb_gpio_vbus_pin == 0)
		return -1;

	gpio_direction_output(usb_gpio_vbus_pin, 0); 
	gpio_set_value_cansleep(usb_gpio_vbus_pin, 1);
	printk("usb wifi power on\n");
	return 0;

use_wifi33:
	//wifi33_regulator = regulator_get(NULL, "wifi33");
	wifi33_regulator = regulator_get(NULL, "avcccore");

	if (IS_ERR(wifi33_regulator)) 
	{
		pr_err("failed to get regulator %s\n", "avcccore");
		return -ENODEV;
	}
	//wifi12_regulator = regulator_get(NULL, "wifi12");

	/*enable gl5302 power for wifi*/
	//regulator_enable(wifi12_regulator);
	regulator_set_voltage(wifi33_regulator, 3300000, 3300000);
	regulator_enable(wifi33_regulator);
	return 0;
}

static int usb_wifi_free_power(void)
{
	if (usb_gpio_vbus_pin < 0) {
		goto exit_use_wifi33;
	}
	if (usb_gpio_vbus_pin == 0) {
		printk("wifi power not enabled!\n");
		return 0;
	}
	printk("wifi power off\n");
	gpio_set_value_cansleep(usb_gpio_vbus_pin, 0);

	return 0;

exit_use_wifi33:
	/*disable gl5302 power for wifi*/
	//regulator_disable(wifi12_regulator);
	regulator_disable(wifi33_regulator);

	if (wifi33_regulator != NULL)
		regulator_put(wifi33_regulator);
	return 0;
}

int aotg1_device_init(int power_only)
{
	int ret = 0;

	if (power_only) {
		usb_wifi_get_power();
		return 0;
	}
	usb_wifi_get_power();

	aotg_dev1 = platform_device_alloc("aotg_hcd", 1);
	aotg_dev1->dev.dma_mask = &hcd_dmamask1,
	aotg_dev1->dev.coherent_dma_mask = 0xffffffff,

	ret = platform_device_add_resources(aotg_dev1, hcd_res1,
			ARRAY_SIZE(hcd_res1));
	if (ret) {
		printk("couldn't add resources to aotg_dev1 device\n");
		goto err2;
	}

	ret = platform_device_add_data(aotg_dev1, &otg_data1, sizeof(otg_data1));
	if (ret) {
		printk("couldn't add data to aotg_dev1 device\n");
		goto err2;
	}

	ret = platform_device_add(aotg_dev1);
	if (ret) {
		printk("failed to register aotg1 device\n");
		goto err2;
	}

	/* delay for power on stable, if no delay, net device 
	 * will not be registered after insmod 8192cu.ko 
	 */
	//msleep(100);
	return ret; 
err2:
	platform_device_put(aotg_dev1);
	aotg_dev1 = NULL;
	return ret; 
}

void aotg1_device_exit(int power_only)
{
	if (power_only) {
		usb_wifi_free_power();
		return;
	}

	if (aotg_dev1 != NULL) {
		platform_device_unregister(aotg_dev1);
		aotg_dev1 = NULL;
	}
	usb_wifi_free_power();
	return;
}

void aotg1_device_mod_init(void)
{
	struct gpio_pre_cfg pcfg;
	char * name = "usb1_wifi_gpio";
	int ret = 0;

	memset((void *)&pcfg, 0, sizeof(struct gpio_pre_cfg));
	ret = gpio_get_pre_cfg(name, &pcfg);
	if (ret < 0) {
		usb_gpio_vbus_pin = -1;
		printk("usb gpio vbus get pre_cfg failed, use wifi33 ldo!\n");
		return;
	}

	usb_gpio_vbus_pin = ASOC_GPIO_PORT(pcfg.iogroup, pcfg.pin_num);
	printk("usb1_gpio_vbus_pin get:%d\n", usb_gpio_vbus_pin);

	ret = gpio_request(usb_gpio_vbus_pin, name);   
	if (ret < 0) {
		usb_gpio_vbus_pin = 0;
		printk("usb gpio vbus request err!\n");
		return;
	}

	gpio_direction_output(usb_gpio_vbus_pin, 0); 
	gpio_set_value_cansleep(usb_gpio_vbus_pin, 0);
	return;
}

void aotg1_device_mod_exit(void)
{
	if (usb_gpio_vbus_pin < 0) {
		return;
	}
	if (usb_gpio_vbus_pin == 0) {
		return;
	}
	gpio_set_value_cansleep(usb_gpio_vbus_pin, 0);
	gpio_free(usb_gpio_vbus_pin);
	return;
}
