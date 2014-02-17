/*
 * SSD 2828 mipi lcd Controller driver (SPI DEVICE)
 *
 * Copyright 2009 actions Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <mach/actions_reg_gl5202.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <mach/spi.h>

/* ssd2828  device */
struct ssd2828_spi_dev {
    struct device *dev;
    void *control_data;

    int (*read_dev)(int reg_addr);
    int (*write_dev)(int reg_addr, int value);
    int (*send_cmd_dev)(int cmd);
};

static struct spi_device *ssd2828_spi_device_p;
static struct ssd2828_spi_dev * ssd2828_dev;

int ssd2828_read_reg(int reg_addr)
{
    struct spi_device *spi = ssd2828_dev->control_data;
    int ret;
    u8 data[4];
    u16 reg_value = 0;

    data[0] = 0x70;
    data[1] = ((reg_addr >> 8) & 0xff);
    data[2] = ((reg_addr >> 0) & 0xff);
    data[3] = 0x73;

    ret = spi_write_then_read(spi, (char *)&data, 4,(char *)&reg_value, 2);
    if (ret < 0) {
        dev_err(&spi->dev, "error to read reg %d, ret %d\n",
            reg_addr, ret);
        return ret;
    }

    reg_value = be16_to_cpu(reg_value);

    dev_dbg(ssd2828_dev->dev, "%s: read reg %x, reg_value %x\n",
        __FUNCTION__, reg_addr,reg_value);

    return reg_value;

}
EXPORT_SYMBOL(ssd2828_read_reg);

int ssd2828_write_reg(int reg_addr, int value)
{
    struct spi_device *spi = ssd2828_dev->control_data;
    int ret;
    char data[6];

    dev_dbg(&spi->dev, "%s: write reg %x, value %x\n",
        __FUNCTION__, reg_addr,value);

    data[0] = 0x70;
    data[1] = ((reg_addr >> 8) & 0xff);
    data[2] = ((reg_addr >> 0) & 0xff);
    data[3] = 0x72;
    data[4] = ((value >> 8) & 0xff);
    data[5] = ((value >> 0) & 0xff);

    ret = spi_write(spi, (char *)&data, 6);
    if (ret < 0) {
        dev_err(&spi->dev, "error to write reg %d, ret %d\n",
            reg_addr, ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(ssd2828_write_reg);

int ssd2828_write_cmd(int cmd)
{
    struct spi_device *spi = ssd2828_dev->control_data;
    int ret;
    char data[3];

    dev_dbg(ssd2828_dev->dev, "%s: write cmd %x\n",
        __FUNCTION__, cmd);

    data[0] = 0x70;
    data[1] = ((cmd >> 8) & 0xff);
    data[2] = ((cmd >> 0) & 0xff);

    ret = spi_write(spi, (char *)&data, 3);
    if (ret < 0) {
        dev_err(ssd2828_dev->dev, "error to write cmd %d, ret %d\n",
            cmd, ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(ssd2828_write_cmd);

int ssd2828_shutdown(int enable)
{
    mdelay(10);

    if (!enable)
        act_writel(act_readl(GPIO_ADAT ) & (~(0x01 << 23)),GPIO_ADAT);
    else
        act_writel(act_readl(GPIO_ADAT ) | (0x01 << 23),GPIO_ADAT);

    return 0;
}
EXPORT_SYMBOL(ssd2828_shutdown);

int ssd2828_init(int xres , int yres ,int hbp ,int hfp, int hsw, int vbp ,int vfp, int vsw)
{
    int id;

    printk("%s()\n", __FUNCTION__);

    mdelay(10);

    id = ssd2828_dev->read_dev(0xB0); // id
    if (id != 0x2828) {
        printk("%s: id reg(0xb0) %x != 0x2828\n",
            __FUNCTION__, id);
        return -ENODEV;
    }

   
    ssd2828_dev->send_cmd_dev(0x28);//enter LP mode
    mdelay(10);
    ssd2828_dev->send_cmd_dev(0x10);//sleep out cmd
    mdelay(10);
		
		ssd2828_dev->write_dev(0xB7,0x0300);//video mode off	
		ssd2828_dev->write_dev(0xB7,0x0304); //ULP Mode
	

		ssd2828_dev->write_dev(0xB1,0x3240);////VSA=50, HAS=64
		ssd2828_dev->write_dev(0xB2,0x5078);////VBP=30+50, HBP=56+64
		ssd2828_dev->write_dev(0xB3,0x243C);//VFP=36, HFP=60
		ssd2828_dev->write_dev(0xB4,0x0300);//HACT=768
		ssd2828_dev->write_dev(0xB5,0x0400);//VACT=1024	
		ssd2828_dev->write_dev(0xB6,0xB);  //burst mode, 18bpp loosely packed
		ssd2828_dev->write_dev(0xDE,0x3);  // //no of lane=4
		ssd2828_dev->write_dev(0xD6,0x4);//RGB order and packet number in blanking period	
		
	  ssd2828_dev->write_dev(0xBA,0x801C); //may modify according to requirement, 500Mbps to 560Mbps
		ssd2828_dev->write_dev(0xBB,0x08); 
	
		ssd2828_dev->write_dev(0xB9,0x0);//disable PLL
		
	  mdelay(50);

    return 0;
}
EXPORT_SYMBOL(ssd2828_init);

int ssd2828_open()
{
    int id;

    printk("%s()\n", __FUNCTION__);

    ssd2828_dev->write_dev(0xB9,0x1);//enable PLL
	
		ssd2828_dev->write_dev(0xB7,0x0300);//video mode off
		
		ssd2828_dev->write_dev(0xc4,0x1);////enable BTA
		
		ssd2828_dev->write_dev(0xB7,0x0342);//enter LP mode
		
		ssd2828_dev->write_dev(0xB8,0x0000);//VC
		
		ssd2828_dev->write_dev(0xBc,0x0);//set packet size	
	
		mdelay(50);
		ssd2828_dev->send_cmd_dev(0x11);//sleep out cmd
		mdelay(10);
		ssd2828_dev->send_cmd_dev(0x29);//display on
		mdelay(10);
		ssd2828_dev->write_dev(0xB7,0x030b);//enter LP mode	
    ssd2828_shutdown(0);
    return 0;
}
EXPORT_SYMBOL(ssd2828_open);

int ssd2828_close()
{
	 int id = 0;
   id = ssd2828_dev->read_dev(0xB0); // id
    if (id != 0x2828) {
        printk("%s: id reg(0xb0) %x != 0x2828\n",
            __FUNCTION__, id);
        return -ENODEV;
    }
    ssd2828_shutdown(1);   
    
    ssd2828_dev->send_cmd_dev(0x28);//enter LP mode
    mdelay(10);
    ssd2828_dev->send_cmd_dev(0x10);//sleep out cmd
    mdelay(10);
		
		ssd2828_dev->write_dev(0xB7,0x0300);//video mode off	
		ssd2828_dev->write_dev(0xB7,0x0304); //ULP Mode
		
		ssd2828_dev->write_dev(0xB9,0x0);//disable PLL
		
    return 0;
}

EXPORT_SYMBOL(ssd2828_close);

static int __devinit ssd2828_spi_probe(struct spi_device *spi)
{
    pr_info("%s()\n", __FUNCTION__);

    ssd2828_dev = kzalloc(sizeof(struct ssd2828_spi_dev), GFP_KERNEL);
    if (ssd2828_dev == NULL)
        return -ENOMEM;

    ssd2828_dev->dev = &spi->dev;
    ssd2828_dev->control_data = spi;
    ssd2828_dev->read_dev = ssd2828_read_reg;
    ssd2828_dev->write_dev = ssd2828_write_reg;
    ssd2828_dev->send_cmd_dev = ssd2828_write_cmd;

    spi->bits_per_word = 8;
    spi->mode = SPI_MODE_3;
    dev_set_drvdata(&spi->dev, ssd2828_dev);

    return 0;
}

static int __devexit ssd2828_spi_remove(struct spi_device *spi)
{
    spi_set_drvdata(spi, NULL);
    return 0;
}

static struct spi_driver ssd2828_spi_driver = {
    .driver = {
        .name   = "ssd2828",
        .bus    = &spi_bus_type,
        .owner  = THIS_MODULE,
    },
    .probe      = ssd2828_spi_probe,
    .remove     = __devexit_p(ssd2828_spi_remove)
};

static struct asoc_spi_controller_data ssd2828_spi_controller_data = {
    .cs_gpio = ASOC_GPIO_PORTA(15),
};

static struct spi_board_info ssd2828_spi_board_info = {
    .modalias = "ssd2828",
    .max_speed_hz = 100000,
    .bus_num = 1,
    .chip_select = 1,
    .mode = SPI_MODE_3,
    .platform_data = NULL,
    .controller_data = &ssd2828_spi_controller_data,
};
static int is_init = 0;
int ssd2828_spi_init(void)
{
    int ret;
    
    struct spi_master *master;
    struct spi_device *spi;
    struct spi_board_info *bi;
    if(is_init){
        return 0;
    }
    is_init = 1;
    bi = &ssd2828_spi_board_info;

    master = spi_busnum_to_master(bi->bus_num);
    if (!master) {
        pr_err("Failed to get spi master %d\n", bi->bus_num);
        return -ENODEV;
    }

    spi = spi_new_device(master, bi);
    if (!spi) {
        dev_err(master->dev.parent, "can't create new device for %s\n",
            bi->modalias);
        return -ENODEV;
    }

    ret = spi_register_driver(&ssd2828_spi_driver);
    if (ret != 0) {
        pr_err("Failed to register spi_test driver: %d\n", ret);
        spi_unregister_device(spi);
        return ret;
    }

    ssd2828_spi_device_p = spi;

    return ret;
}
EXPORT_SYMBOL(ssd2828_spi_init);
void ssd2828_spi_exit(void)
{
    spi_unregister_driver(&ssd2828_spi_driver);
    spi_unregister_device(ssd2828_spi_device_p);
}
EXPORT_SYMBOL(ssd2828_spi_exit);
//module_init(ssd2828_spi_init);
//module_exit(ssd2828_spi_exit);

//MODULE_DESCRIPTION(" Devices ssd2828 SPI Driver");
//MODULE_AUTHOR("wanghui <wanghui@actions-semi.com>");
//MODULE_LICENSE("GPL");
