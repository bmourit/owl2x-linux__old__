/**
 * dwc3-actions.c - actions-semi DWC3 Specific Glue layer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/types.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#include <mach/irqs.h>
#include <mach/hardware.h>

#include "core.h"

#define  ADFUS_PROC
static struct resource dwc3_resources[];

#define USB3_ACTIONS_START        (0xcd00)
#define USB3_ACTIONS_END          (0xcd58)

#define DWC3_CDR_KIKD          (0x00)
#define DWC3_CDR_KP1           (0x04)
#define DWC3_TIMER_INIT        (0x08)
#define DWC3_CDR_CONTROL       (0x0c)
#define DWC3_RX_OFFSET_PS      (0x10)
#define DWC3_EQ_CONTROL        (0x14)
#define DWC3_RX_OOBS_SSC0      (0x18)
#define DWC3_CMU_SSC1          (0x1C)
#define DWC3_CMU_DEBUG_LDO     (0x20)
#define DWC3_TX_AMP_DEBUG      (0x24)
#define DWC3_Z0                (0x28)
#define DWC3_DMR_BACR          (0x2C)
#define DWC3_IER_BCSR          (0x30)
#define DWC3_BPR               (0x34)
#define DWC3_BFNR              (0x38)
#define DWC3_BENR_REV          (0x3C)
#define DWC3_FLD               (0x40)
#define DWC3_CMU_PLL2_BISTDEBUG    (0x44)

#define USB3_MOD_RST           (1 << 14)
#define CMU_BIAS_EN            (1 << 20)

#define USB3_P0_CTL_PLLLDOEN            28
#define USB3_P0_CTL_LDOVREFSEL_SHIFT    29

#define BIST_QINIT(n)          ((n) << 24)
#define EYE_HEIGHT(n)          ((n) << 20)
#define PLL2_LOCK              (1 << 15)
#define PLL2_RS(n)             ((n) << 12)
#define PLL2_ICP(n)            ((n) << 10)
#define CMU_SEL_PREDIV         (1 << 9)
#define CMU_DIVX2              (1 << 8)
#define PLL2_DIV(n)            ((n) << 3)
#define PLL2_POSTDIV(n)        ((n) << 1)
#define PLL2_PU                (1 << 0)

#ifdef ADFUS_PROC

#define ADFUS_PROC_FILE_LEN 64

enum probatch_status {
	PROBATCH_START = 0,
	PROBATCH_INSTAL_FLASH,
	PROBATCH_FINISH_INSTALL_FLASH,
	PROBATCH_WRITE_PHY,
	PROBATCH_FINISH_WRITE_PHY,
	PROBATCH_FORMAT,
	PROBATCH_FINISH_FORMAT,
	PROBATCH_FINISH,	
	PROBATCH_FINISH_OK,
};	
#endif

struct dwc3_port_info {
	void __iomem *usbecs;
	void __iomem *devrst;
	void __iomem *usbpll;
};

struct dwc3_actions {
	struct platform_device	*dwc3;
	struct device		*dev;

	struct dwc3_port_info port_info;	
	void __iomem        *base;
};

static void dwc3_clk_init(struct dwc3_actions *dwc3_5202)
{
	u32		reg;
	struct dwc3_port_info *port_info = &dwc3_5202->port_info;

	/*USB3 PLL enable*/
	reg = readl(port_info->usbpll);
	reg |= (0x1f);
	writel(reg, port_info->usbpll);

	udelay(1000);

	/*USB3 Cmu Reset */
	reg = readl(port_info->devrst);
	reg &= ~(USB3_MOD_RST);
	writel(reg, port_info->devrst);

	udelay(100);
	
	reg = readl(port_info->devrst);
	reg |= (USB3_MOD_RST);
	writel(reg, port_info->devrst);

	udelay(100);

	/*PLL1 enable*/
	reg = readl(dwc3_5202->base + DWC3_CMU_DEBUG_LDO);
	reg |= CMU_BIAS_EN;
	writel(reg, dwc3_5202->base + DWC3_CMU_DEBUG_LDO);

	/*PLL2 enable*/
	reg = (BIST_QINIT(0x3) | EYE_HEIGHT(0x4) | PLL2_LOCK | PLL2_RS(0x2) | 
			PLL2_ICP(0x1) | CMU_SEL_PREDIV | CMU_DIVX2 | PLL2_DIV(0x17) | 
			PLL2_POSTDIV(0x3) | PLL2_PU);
	writel(reg, dwc3_5202->base + DWC3_CMU_PLL2_BISTDEBUG);

	/*USB2 LDO enable*/
	reg = readl(port_info->usbecs );
	reg |= (1 << USB3_P0_CTL_PLLLDOEN )|(2 << USB3_P0_CTL_LDOVREFSEL_SHIFT);
	writel(reg, port_info->usbecs );

	return;
}

static void dwc3_clk_exit(struct dwc3_actions *dwc3_5202)
{
	u32		reg;
	struct dwc3_port_info *port_info = &dwc3_5202->port_info;
	
	/*USB3 PLL disable*/
	reg = readl(port_info->usbpll);
	reg &= ~(0x1f);
	writel(reg, port_info->usbpll);
}

/*---------------------------------------------------------------------------
 *	proc file entry  for debug
 *---------------------------------------------------------------------------*/
#ifdef ADFUS_PROC
static struct proc_dir_entry *adfus_proc_entry;
char adfus_proc_path[] = "adfus_proc";

char probatch_phase[ADFUS_PROC_FILE_LEN];

char all_probatch_phase[][ADFUS_PROC_FILE_LEN]=
{
	"null",
	"install_flash",
	"finish_install_flash",
	"write_phy",
	"finish_write_phy",
	"format",
	"finish_format",
	"finish",
	"finish_ok",
};

int set_probatch_phase(int id)
{
	strcpy(probatch_phase, all_probatch_phase[id]);
	return 0;
}	
EXPORT_SYMBOL_GPL(set_probatch_phase);

int is_probatch_phase(int id)
{
	int ret;
	
	ret = memcmp(probatch_phase, all_probatch_phase[id], strlen(all_probatch_phase[id]));
	return ret;
}
EXPORT_SYMBOL_GPL(is_probatch_phase);

static int adfus_proc_read(char *page, char **start, off_t off,
			  int count, int *eof, void *data)
{
	int len;
	
	if(count > ADFUS_PROC_FILE_LEN)
		len = ADFUS_PROC_FILE_LEN;
	else
		len = count;	
	len = snprintf(page, len, "%s", probatch_phase);
	
	return len;
}

static int adfus_proc_write(struct file *file, const char __user *buffer,
			   unsigned long count, void *data)
{
	int len;
	
	if(count > ADFUS_PROC_FILE_LEN)
		len = ADFUS_PROC_FILE_LEN;
	else
		len = count;
	
	if (copy_from_user(probatch_phase, buffer, len ))
		return -EFAULT;
	probatch_phase[len]=0;

	return len;
}
#endif

static int __devinit dwc3_actions_probe(struct platform_device *pdev)
{
	struct platform_device	*dwc3;
	struct dwc3_actions	*dwc3_5202;
	struct resource		*res;
	struct device       *dev = &pdev->dev;

	void __iomem        *base;
	int			devid = 0;
	int			ret = -ENOMEM;

	dwc3_5202 = devm_kzalloc(dev,sizeof(*dwc3_5202), GFP_KERNEL);
	if (!dwc3_5202) {
		dev_err(&pdev->dev, "not enough memory\n");
		goto err1;
	}

	platform_set_drvdata(pdev, dwc3_5202);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "missing memory base resource\n");
		return -EINVAL;
	}

	res = devm_request_mem_region(dev, res->start,resource_size(res),
			dev_name(dev));
	if (!res) {
		dev_err(dev, "can't request mem region\n");
		return -ENOMEM;
	}

	base = devm_ioremap(dev, res->start, resource_size(res));
	if (!base) {
		dev_err(dev, "ioremap failed\n");
		return -ENOMEM;
	}
	
	dwc3_5202->port_info.devrst = (void __iomem *)IO_ADDRESS(CMU_DEVRST1);
	dwc3_5202->port_info.usbecs = (void __iomem *)IO_ADDRESS(USB3_P0_CTL);
	dwc3_5202->port_info.usbpll = (void __iomem *)IO_ADDRESS(CMU_USBPLL);
	
	devid = dwc3_get_device_id();
	if (devid < 0)	
		return -ENODEV;
		

	dwc3 = platform_device_alloc("dwc3", devid);
	if (!dwc3) {
		dev_err(&pdev->dev, "couldn't allocate dwc3 device\n");
		goto err1;
	}

	dma_set_coherent_mask(&dwc3->dev, pdev->dev.coherent_dma_mask);

	dwc3->dev.parent = &pdev->dev;
	dwc3->dev.dma_mask = pdev->dev.dma_mask;
	dwc3->dev.dma_parms = pdev->dev.dma_parms;
	dwc3_5202->dwc3	= dwc3;
	dwc3_5202->dev	= &pdev->dev;
	dwc3_5202->base	= base;

	dwc3_clk_init(dwc3_5202);
	ret = platform_device_add_resources(dwc3, dwc3_resources, 2);
	if (ret) {
		dev_err(&pdev->dev, "couldn't add resources to dwc3 device\n");
		goto err2;
	}

	ret = platform_device_add(dwc3);
	if (ret) {
		dev_err(&pdev->dev, "failed to register dwc3 device\n");
		goto err2;
	}
#ifdef ADFUS_PROC
	adfus_proc_entry = create_proc_entry(adfus_proc_path, 0, NULL);
	if (adfus_proc_entry) {
		/* proc op */
		adfus_proc_entry->read_proc = adfus_proc_read;
		adfus_proc_entry->write_proc = adfus_proc_write;
		strcpy(probatch_phase, all_probatch_phase[0]);		
	} else {
		/* proc op */
		printk("adfus :can not create proc file\n");
	}
#endif
	return 0;
	
err2:
	platform_device_put(dwc3);
	dwc3_clk_exit(dwc3_5202);
err1:
	dwc3_put_device_id(devid);
	return ret;
}

#ifdef CONFIG_PM
static int dwc3_actions_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct dwc3_actions	*dwc3_5202 = platform_get_drvdata(pdev);

	dwc3_clk_exit(dwc3_5202);
    
	return 0;
}

static int dwc3_actions_resume(struct platform_device *pdev)
{
	struct dwc3_actions	*dwc3_5202 = platform_get_drvdata(pdev);

	dwc3_clk_init(dwc3_5202);

	return 0;
}
#else
#define	dwc3_actions_suspend	NULL
#define	dwc3_actions_resume		NULL
#endif

static int __devexit dwc3_actions_remove(struct platform_device *pdev)
{
	struct dwc3_actions	*dwc3_5202 = platform_get_drvdata(pdev);
#ifdef ADFUS_PROC
	if (adfus_proc_entry)
		remove_proc_entry(adfus_proc_path, NULL);
#endif
    dwc3_clk_exit(dwc3_5202);
	platform_device_unregister(dwc3_5202->dwc3);
	dwc3_put_device_id(dwc3_5202->dwc3->id);

	return 0;
}

static void dwc3_actions_release(struct device * dev)
{
	return ;
}

static struct resource dwc3_resources[] = {
	[0] = {
		.start	= USB3_REGISTER_BASE,
		.end	= USB3_REGISTER_BASE + USB3_ACTIONS_START - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_ASOC_USB,
		.end	= IRQ_ASOC_USB,
		.flags	= IORESOURCE_IRQ,
	},	
};

static struct resource actions_resources[] = {
	[0] = {
		.start	= USB3_REGISTER_BASE + USB3_ACTIONS_START,
		.end	= USB3_REGISTER_BASE + USB3_ACTIONS_END,
		.flags	= IORESOURCE_MEM,
	},
};

static u64 dwc3_dma_mask = DMA_BIT_MASK(32);

static struct platform_device dwc3_actions_device = {
	.name       = "actions-dwc3",
	.id     = 1,
	.dev	= {
		.dma_mask		= &dwc3_dma_mask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),	
		.release = dwc3_actions_release,
	},
	.resource	= actions_resources,
	.num_resources	= ARRAY_SIZE(actions_resources),    
};

static struct platform_driver dwc3_actions_driver = {
	.probe		= dwc3_actions_probe,
	.remove		= __devexit_p(dwc3_actions_remove),
	.suspend	= dwc3_actions_suspend,
	.resume		= dwc3_actions_resume,
	.driver		= {
		.name	= "actions-dwc3",
	},
};

static int __init  dwc3_actions_init(void)
{
	platform_device_register(&dwc3_actions_device);
	return platform_driver_register(&dwc3_actions_driver);
}
module_init(dwc3_actions_init);

static void __exit dwc3_actions_exit(void)
{
	platform_driver_unregister(&dwc3_actions_driver);
	platform_device_unregister(&dwc3_actions_device);
}
module_exit(dwc3_actions_exit);

//module_platform_driver(dwc3_actions_driver);

MODULE_ALIAS("platform:actions-dwc3");
MODULE_AUTHOR("wanlong <wanlong@actions-semi.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DesignWare USB3 actions Glue Layer");
