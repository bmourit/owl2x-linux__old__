#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/scatterlist.h>
#include <linux/dma-mapping.h>
#include <linux/highmem.h>
#include <linux/usb.h>
#include <linux/usb/ch9.h>
#include <linux/usb/otg.h>
#include <linux/usb/hcd.h>

#include <asm/irq.h>
#include <asm/system.h>
#include <linux/regulator/consumer.h>

#include <mach/actions_reg_gl5202.h>
#include <mach/hardware.h>
#include "aotg_hcd.h"
#include "aotg_regs.h"
#include "aotg_plat_data.h"
#include "aotg_dma.h"
#include "aotg_debug.h"


unsigned int aotg_dma_map = 0;
void __iomem *aotg_dma_reg_base[4];
void __iomem *aotg_reg_base[4];

int aotg_dma_reset_2(int dma_nr)
{
	unsigned char data;

	data = usb_readb(aotg_dma_reg_base[dma_nr] + UDMA1COM);
	data |= 0x1 << 4;
	usb_writeb(data, aotg_dma_reg_base[dma_nr] + UDMA1COM);
	data &= ~(0x1 << 4);
	usb_writeb(data, aotg_dma_reg_base[dma_nr] + UDMA1COM);
	return 0;
}

int aotg_dma_enable_irq_2(int dma_nr)
{
	unsigned char data;

	data = usb_readb(aotg_reg_base[dma_nr] + UDMAIEN);
	data |= 0x35;
	usb_writeb(data, aotg_reg_base[dma_nr] + UDMAIEN);
	return 0;
}

int aotg_dma_enable_irq(int dma_nr, int enable)
{
	unsigned char data;

	data = usb_readb(aotg_reg_base[dma_nr] + UDMAIEN);
	if (dma_nr & 0x1) {
		if (enable)
			data |= 0x4;
		else
			data &= ~0x4;
	} else {
		if (enable)
			data |= 0x1;
		else
			data &= ~0x1;
	}
	usb_writeb(data, aotg_reg_base[dma_nr] + UDMAIEN);
	return 0;
}

int aotg_dma_is_irq_2(int dma_nr)
{
	unsigned char data;
	unsigned char mask;
	int ret = 0;

	mask = usb_readb(aotg_reg_base[dma_nr] + UDMAIEN);
	data = usb_readb(aotg_reg_base[dma_nr] + UDMAIRQ);
	data = data & mask;

	if (data & 0x30) {
		printk("dma err irq! data:%x\n", (unsigned int)data);
	}
	if (data & 0x1) {
		ret |= 0x1;
	}
	if (data & 0x4) {
		ret |= 0x2;
	}

	return ret;
}

int aotg_dma_clear_pend(int dma_nr)
{
	unsigned char data;

	do {
		data = usb_readb(aotg_reg_base[dma_nr] + UDMAIRQ);
		data &= ~0x5;
		if (dma_nr & 0x1) {
			data |= 0x4;
		} else {
			data |= 0x1;
		}
		usb_writeb(data, aotg_reg_base[dma_nr] + UDMAIRQ);
		data = usb_readb(aotg_reg_base[dma_nr] + UDMAIRQ);
		if (dma_nr & 0x1) {
			data &= 0x4;
		} else {
			data &= 0x1;
		}
	} while (data != 0);

	return 0;
}

int aotg_dma_clear_pend_2(int dma_nr)
{
	unsigned char data;

	data = usb_readb(aotg_reg_base[dma_nr] + UDMAIRQ);
	do {
		usb_writeb(data, aotg_reg_base[dma_nr] + UDMAIRQ);
		data = usb_readb(aotg_reg_base[dma_nr] + UDMAIRQ);
	} while (data & 0x5);

	return 0;
}

unsigned int aotg_dma_get_cnt(int dma_nr)
{
	unsigned int data_u32;
#if 0
	unsigned char data_u8;
	data_u8 = usb_readb(aotg_dma_reg_base[dma_nr] + UDMA1CNTL);
	data_u32 = (unsigned int)data_u8;
	data_u8 = usb_readb(aotg_dma_reg_base[dma_nr] + UDMA1CNTM);
	data_u32 |= (unsigned int)data_u8 << 8;
	data_u8 = usb_readb(aotg_dma_reg_base[dma_nr] + UDMA1CNTH);
	data_u32 |= (unsigned int)data_u8 << 16;
#endif
	data_u32 = usb_readl(aotg_dma_reg_base[dma_nr] + UDMA1CNTL);
	return data_u32;
}

unsigned int aotg_dma_get_memaddr(int dma_nr)
{
	unsigned int data;

	data = usb_readl(aotg_dma_reg_base[dma_nr] + UDMA1MEMADDR);
	return data;
}

int aotg_dma_stop(int dma_nr)
{
	unsigned char data;

	//data = usb_readb(aotg_dma_reg_base[dma_nr] + UDMA1COM);
	//data &= ~0x1;
	data = 0;
	usb_writeb(data, aotg_dma_reg_base[dma_nr] + UDMA1COM);
	return 0;
}

unsigned int aotg_dma_get_remain(int dma_nr)
{
	unsigned int data_u32;
#if 0
	unsigned char data_u8;
	data_u8 = usb_readb(aotg_dma_reg_base[dma_nr] + UDMA1REML);
	data_u32 = (unsigned int)data_u8;
	data_u8 = usb_readb(aotg_dma_reg_base[dma_nr] + UDMA1REMM);
	data_u32 |= (unsigned int)data_u8 << 8;
	data_u8 = usb_readb(aotg_dma_reg_base[dma_nr] + UDMA1REMH);
	data_u32 |= (unsigned int)data_u8 << 16;
#endif
	data_u32 = usb_readl(aotg_dma_reg_base[dma_nr] + UDMA1REML);

	return data_u32;
}

int aotg_dma_set_mode(int dma_nr, unsigned char ep_select)
{
	int i = 0;

	do {
		i++;
		if (i > 1000) {
			printk("%s timeout!\n", __func__);
			break;
		}
		usb_writeb(ep_select, aotg_dma_reg_base[dma_nr] + UDMA1EPSEL);
	} while (usb_readb(aotg_dma_reg_base[dma_nr] + UDMA1EPSEL) != ep_select);

	return 0;
}

int aotg_dma_set_memaddr(int dma_nr, unsigned long addr)
{
	usb_writel(addr, aotg_dma_reg_base[dma_nr] + UDMA1MEMADDR);
	return 0;
}

int aotg_dma_set_cnt(int dma_nr, unsigned long dma_length)
{
	unsigned char data;

	data = (unsigned char)(dma_length & 0xff);
	usb_writeb(data, aotg_dma_reg_base[dma_nr] + UDMA1CNTL);

	data = (unsigned char)((dma_length >> 8) & 0xff);
	usb_writeb(data, aotg_dma_reg_base[dma_nr] + UDMA1CNTM);

	data = (unsigned char)((dma_length >> 16) & 0xff);
	usb_writeb(data, aotg_dma_reg_base[dma_nr] + UDMA1CNTH);

	return 0;
}

int aotg_dma_start(int dma_nr)
{
	unsigned char data;

	data = 0x1;
	usb_writeb(data, aotg_dma_reg_base[dma_nr] + UDMA1COM);
	return 0;
}

unsigned int aotg_dma_get_cmd(unsigned int dma_nr)
{
	unsigned char data;

	data = usb_readb(aotg_dma_reg_base[dma_nr] + UDMA1COM);
	return (unsigned int)data;
}

int aotg_dma_request_2(aotg_dma_handler handler, void *hcd)
{
	int dma_nr = -1;
	struct aotg_hcd *acthcd = (struct aotg_hcd *)hcd;
	unsigned int base = (unsigned int)acthcd->base;

	if ((base & 0x03ffffff) == (AOTG0_BASE & 0x03ffffff)) {
		printk("AOTG base is 0!\n");

		if ((aotg_dma_map & 0x1) == 0) {
			dma_nr = 0;
			aotg_dma_map |= 0x1;
			aotg_dma_reg_base[0] = acthcd->base;
			aotg_dma_map |= 0x2;
			aotg_dma_reg_base[1] = acthcd->base + (UDMA2MEMADDR - UDMA1MEMADDR);

			aotg_reg_base[0] = acthcd->base;
			aotg_reg_base[1] = acthcd->base;
		} else {
			printk("%s:%d, err!\n", __func__, __LINE__);
			return -1;
		}
	} else if ((base & 0x03ffffff) == (AOTG1_BASE & 0x03ffffff)) {
		printk("AOTG base is 1!\n");

		if ((aotg_dma_map & 0x4) == 0) {
			dma_nr = 2;
			aotg_dma_map |= 0x4;
			aotg_dma_reg_base[2] = acthcd->base;
			aotg_dma_map |= 0x8;
			aotg_dma_reg_base[3] = acthcd->base + (UDMA2MEMADDR - UDMA1MEMADDR);

			aotg_reg_base[2] = acthcd->base;
			aotg_reg_base[3] = acthcd->base;
		} else {
			printk("%s:%d, err!\n", __func__, __LINE__);
			return -1;
		}
	} else {
		printk("%s:%d, err!\n", __func__, __LINE__);
		return -1;
	}

	return dma_nr;
}

void aotg_dma_free_2(int dma_nr)
{
	if ((dma_nr >= 0) && (dma_nr <= 3)) {
		aotg_dma_map &= ~(0x1 << dma_nr);
	} else {
		printk("%s:%d, err!\n", __func__, __LINE__);
	}
	return;
}


