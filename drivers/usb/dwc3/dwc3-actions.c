/**
 * dwc3-owl2x.c - owl2x-semi DWC3 Specific Glue layer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

#include <linux/platform_data/dwc3-owl2x.h>

#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/usb/otg.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/extcon.h>
#include <linux/extcon/of_extcon.h>
#include <linux/regulator/consumer.h>

#define  ADFUS_PROC


#define USBSS_CDR_KIKD                   0x0000
#define USBSS_CDR_KP1                    0x0004
#define USBSS_TIMER_INIT                 0x0008
#define USBSS_CDR_CONTROL                0x000c
#define USBSS_RX_OFFSET_PS               0x0010
#define USBSS_EQ_CONTROL                 0x0014
#define USBSS_RX_OOBS_SSC0               0x001b
#define USBSS_CMU_SSC1                   0x001c
#define USBSS_CMU_DEBUG_LDO              0x0020
#define USBSS_TX_AMP_DEBUG               0x0024
#define USBSS_Z0                         0x0028
#define USBSS_DMR_BACR                   0x002c
#define USBSS_IER_BCSR                   0x0030
#define USBSS_BPR                        0x0034
#define USBSS_BFNR                       0x0038
#define USBSS_BENR_REV                   0x003c
#define USBSS_FLD                        0x0040
#define USBSS_CMU_PLL2_BISTDEBUG         0x0044

#define USBSS_CMU_BIAS_EN               (1 << 20)
#define USBSS_MOD_RST                    (1 << 14)

#define USBSS_P0_CTL_PLLLDOEN            28
#define USBSS_P0_CTL_LDOVREFSEL_SHIFT    29

#define USBSS_BIST_QINIT(n)                   ((n) << 24)
#define USBSS_EYE_HEIGHT(n)                   ((n) << 20)
#define USBSS_PLL2_LOCK                       (1 << 15)
#define USBSS_PLL2_RS(n)                      ((n) << 12)
#define USBSS_PLL2_ICP(n)                     ((n) << 10)
#define USBSS_CMU_SEL_PREDIV                  (1 << 9)
#define USBSS_CMU_DIVX2                       (1 << 8)
#define USBSS_PLL2_DIV(n)                     ((n) << 3)
#define USBSS_PLL2_POSTDIV(n)                 ((n) << 1)
#define USBSS_PLL2_PU                         (1 << 0)

/*#ifdef ADFUS_PROC
#include <linux/procfs.h>

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
#endif*/

struct dwc3_port_info {
        void __iomem *usbecs;
        void __iomem *devrst;
        void __iomem *usbpll;
};

struct dwc3_owl2x {
        struct dwc3_port_info port_info;        
        struct platform_device  *dwc3;
        struct device           *dev;

        struct clk              *clk
        void __iomem            *base;
};

static void dwc3_clk_init(struct dwc3_owl2x *owl2x)
{
        u32             reg;
        struct dwc3_port_info *port_info = &owl2x->port_info;

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
        reg = readl(owl2x->base + DWC3_CMU_DEBUG_LDO);
        reg |= CMU_BIAS_EN;
        writel(reg, owl2x->base + DWC3_CMU_DEBUG_LDO);

        /*PLL2 enable*/
        reg = (BIST_QINIT(0x3) | EYE_HEIGHT(0x4) | PLL2_LOCK | PLL2_RS(0x2) | 
                        PLL2_ICP(0x1) | CMU_SEL_PREDIV | CMU_DIVX2 | PLL2_DIV(0x17) | 
                        PLL2_POSTDIV(0x3) | PLL2_PU);
        writel(reg, owl2x->base + DWC3_CMU_PLL2_BISTDEBUG);

        /*USB2 LDO enable*/
        reg = readl(port_info->usbecs );
        reg |= (1 << USB3_P0_CTL_PLLLDOEN )|(2 << USB3_P0_CTL_LDOVREFSEL_SHIFT);
        writel(reg, port_info->usbecs );

        return;
}

static void dwc3_clk_exit(struct dwc3_owl2x *owl2x)
{
        u32             reg;
        struct dwc3_port_info *port_info = &owl2x->port_info;
        
        /*USB3 PLL disable*/
        reg = readl(port_info->usbpll);
        reg &= ~(0x1f);
        writel(reg, port_info->usbpll);
}

/*---------------------------------------------------------------------------
 *      proc file entry  for debug
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

static u64 dwc3_owl2x_dma_mask = DMA_BIT_MASK(32);

static int dwc3_owl2x_probe(struct platform_device *pdev)
{
        struct device_node      *node = pdev->dev.of_node;

        struct dwc3_owl2x       *owl2x;
        struct clk              *clk;
        struct resource         *res;
        struct device           *dev = &pdev->dev;
        struct extcon_dev       #edev;
        struct regulator        *vbus_reg = NULL;

        int     ret = -ENOMEM;
        int     irq;

        u32     reg;

        void __iomem    *base;

        if (!node) {
                dev_err(dev, "device node not found\n");
                return -EINVAL;
        }

        owl2x = devm_kzalloc(dev, sizeof(*owl2x), GFP_KERNEL);
        if (!owl2x) {
                dev_err(dev, "not enough memory\n");
                return -ENOMEM;
        }

        platform_set_drvdata(pdev, owl2x);

        irq = platform_get_irq(pdev, owl2x);
        if (irq < 0) {
                dev_err(dev, "missing IRQ resource\n");
                return -EINVAL;
        }

        res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        if (!res) {
                dev_err(dev, "missing memory base resource\n");
                return -EINVAL;
        }

        clk = devm_clk_get(dev, "usbpll");
        if (IS_ERR(clk)) {
                dev_err(dev, "couldn't get clock\n");
                ret = -EINVAL;
                goto err1;
        }

        base = devm_ioremap_resource(dev, res);
        if (IS_ERR(base)) 
                return PTR_ERR(base);

        if (of_property_read_bool(node, "vbus-supply")) {
                vbus_reg = devm_regulator_get(dev, "vbus");
                if (IS_ERR(vbus_reg)) {
                        dev_err(dev, "vbus init failed\n");
                        return PTR_ERR(vbus_reg);
                }
        }

        //owl2x->port_info.devrst = (void __iomem *)IO_ADDRESS(CMU_DEVRST1);
        //owl2x->port_info.usbecs = (void __iomem *)IO_ADDRESS(USB3_P0_CTL);
        //owl2x->port_info.usbpll = (void __iomem *)IO_ADDRESS(CMU_USBCLK);

        owl2x->dev      = dev;
        owl2x->clk      = clk;
        owl2x->irq      = irq;
        owl2x->base     = base;
        owl2x->vbus_reg = vbus_reg;
        dev->dma_mask   = &dwc3_owl2x_dma_mask;

        pm_runtime_enable(dev);
        ret = pm_runtime_get_sync(dev);
        if (ret < 0) {
                dev_err(dev, "get_sync failed with err %d\n", ret);
                goto err0;
        }



        dwc3_clk_init(owl2x);
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
/* #ifdef ADFUS_PROC
        adfus_proc_entry = create_proc_entry(adfus_proc_path, 0, NULL);
        if (adfus_proc_entry) {
                /* proc op */
                /*adfus_proc_entry->read_proc = adfus_proc_read;
                adfus_proc_entry->write_proc = adfus_proc_write;
                strcpy(probatch_phase, all_probatch_phase[0]);          
        } else {
                /* proc op */
                /*printk("adfus :can not create proc file\n");
        }
#endif
*/
        return 0;
        
err3:
        //platform_device_put(dwc3);
        dwc3_clk_exit(owl2x);

err2:
        dwc3_owl2x_disable_irqs(owl2x);

err1:
        dwc3_put_device_id(devid);

err0:
        pm_runtime_disable(dev);

        return ret;
}

static int dwc3_owl2x_remove(struct platform_device *pdev)
{
        struct dwc3_owl2x     *owl2x = platform_get_drvdata(pdev);
/*#ifdef ADFUS_PROC
        if (adfus_proc_entry)
                remove_proc_entry(adfus_proc_path, NULL);
#endif*/
        if (owl2x->extcon_vbus_dev.edev)
                extcon_unregister_interest(&owl2x->extcon_vbus_dev);
        if (owl2x->extcon_id_dev.edev)
                extcon_unregister_interest(&owl2x->extcon_id_dev);
        dwc3_owl2x_disable_irqs(owl2x);
        pm_runtime_put_sync(&pdev->dev);
        pm_runtime_disable(&pdev->dev);
        device_for_each_child(&pdev->dev, NULL, dwc3_owl2x_remove_core);

        dwc3_clk_exit(owl2x);
        //platform_device_unregister(owl2x->dwc3);
        dwc3_put_device_id(owl2x->dwc3->id);

        return 0;
}

static const struct of_device_id of_dwc3_match[] = {
        {
                .compatible = "actions,dwc3"
        },
        {
                .compatible = "actions,owl2x-dwc3"
        },
        { },
};
MODULE_DEVICE_TABLE(of, of_dwc3_match);

#ifdef CONFIG_PM_SLEEP
static int dwc3_owl2x_prepare(struct device *dev)
{
        struct dwc3_owl2x       *owl2x = dev_get_drvdata(dev);

        dwc3_owl2x_disable_irqs(owl2x);

        return 0;
}

static int dwc3_owl2x_complete(struct device *dev)
{
        struct dwc3_owl2x       *owl2x = dev_get_drvdata(dev);

        dwc3_owl2x_enable_irqs(owl2x);
};

static int dwc3_owl2x_suspend(struct device *dev, pm_mess1age_t state)
{
        struct dwc3_owl2x     *owl2x = dev_get_drvdata(dev);

        dwc3_clk_exit(owl2x);
    
        return 0;
}

static int dwc3_owl2x_resume(struct device *dev)
{
        struct dwc3_owl2x     *owl2x = dev_get_drvdata(dev);

        dwc3_clk_init(owl2x);

        pm_runtime_disable(dev);
        pm_runtime_set_active(dev);
        pm_runtime_enable(dev);

        return 0;
}

static const struct dev_pm_ops dwc3_owl2x_dev_pm_ops = {
        .prepare        = dwc3_owl2x_prepare,
        .complete       = dwc3_omap_complete,

        SET_SYSTEM_SLEEP_PM_OPS(dwc3_owl2x_suspend, dwc3_owl2x_resume)
};

#define DEV_PM_OPS (&dwc3_owl2x_dev_pm_ops)
#else
#define DEV_PM_OPS      NULL
#endif /* CONFIG_PM_SLEEP */

static struct platform_driver dwc3_owl2x_driver = {
        .probe          = dwc3_owl2x_probe,
        .remove         = dwc3_owl2x_remove,
        .driver         = {
                .name   = "owl2x-dwc3",
                .of_match_table = of_dwc3_match,
                .pm = DEV_PM_OPS,
        },
};

module_platform_driver(dwc3_owl2x_driver);

MODULE_ALIAS("platform:owl2x-dwc3");
MODULE_AUTHOR("B. Mouritsen <bnmguy@gmail.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("DesignWare USB3 Owl2x Glue Layer");
