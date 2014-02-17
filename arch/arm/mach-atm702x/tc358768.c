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

/* tc358768  device */
struct tc358768_spi_dev {
    struct device *dev;
    void *control_data;

    int (*read_dev)(int reg_addr);
    int (*write_dev)(int reg_addr, int value);
    int (*send_cmd_dev)(int cmd);
};

static struct spi_device *tc358768_spi_device_p;
static struct tc358768_spi_dev * tc358768_dev;

int tc358768_regs_printk();

int tc358768_read_reg(int reg_addr)
{
    struct spi_device *spi = tc358768_dev->control_data;
    int ret;
    u8 data[4];
    u16 mydata[4];
    int reg_value = 0;
/*
    data[0] = ((reg_addr >> 8) & 0xff);
    data[1] = ((reg_addr >> 0) & 0xff) | 0x01;
    data[2] = 0;
    data[3] = 0;
*/
    mydata[0] = reg_addr | 0x01;
    mydata[1] = 0x00;
    spi_write(spi, (char *)&mydata, 4);
    
    //mydata[2] = 0x00;
    //mydata[3] = 0x00;
    //spi_write(spi, (char *)&mydata[2], 4);
    spi_read(spi, (char *)&reg_value, 4);
    //printk("read,the data=%d,%d,%d,%d\n", data[0],data[1],data[2],data[3]);
    //ret = spi_write_then_read(spi, (char *)&mydata[0], 4,(char *)&reg_value, 4);
    //printk("read,the mydata=%d,%d,the read reg_value=%x,ret=%d\n", mydata[0],mydata[1],reg_value, ret);

    //reg_value = be16_to_cpu(reg_value);
    printk("in tc358768_read_reg,reg addr=%x,reg value=%x\n", reg_addr, (reg_value>>16)&0xFFFF);
    dev_dbg(tc358768_dev->dev, "%s: read reg %x, reg_value %x\n",
        __FUNCTION__, reg_addr,reg_value);

    return reg_value;

}
EXPORT_SYMBOL(tc358768_read_reg);

int tc358768_write_reg(int reg_addr, int value)
{
    struct spi_device *spi = tc358768_dev->control_data;
    int ret;
    char data[4];
    u16 mydata[2];
    dev_dbg(&spi->dev, "%s: write reg %x, value %x\n",
        __FUNCTION__, reg_addr,value);
/*
    data[0] = ((reg_addr >> 8) & 0xff);
    data[1] = ((reg_addr >> 0) & 0xff);
    data[2] = ((value >> 8) & 0xff);
    data[3] = ((value >> 0) & 0xff);
*/

    mydata[0] = reg_addr;
    mydata[1] = value;
    //printk("write,the data=%d,%d,%d,%d\n", data[0],data[1],data[2],data[3]);
    ret = spi_write(spi, (char *)&mydata, 4);
    //printk("write,the mydata=%d,%d,ret=%d\n", mydata[0],mydata[1],ret);
    if (ret < 0) {
        dev_err(&spi->dev, "error to write reg %d, ret %d\n",
            reg_addr, ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(tc358768_write_reg);

int tc358768_write_cmd(int cmd)
{
    struct spi_device *spi = tc358768_dev->control_data;
    int ret;
    char data[3];

    dev_dbg(tc358768_dev->dev, "%s: write cmd %x\n",
        __FUNCTION__, cmd);

    data[0] = 0x70;
    data[1] = ((cmd >> 8) & 0xff);
    data[2] = ((cmd >> 0) & 0xff);

    ret = spi_write(spi, (char *)&data, 3);
    if (ret < 0) {
        dev_err(tc358768_dev->dev, "error to write cmd %d, ret %d\n",
            cmd, ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(tc358768_write_cmd);

int tc358768_shutdown(int enable)
{
    mdelay(10);

    if (enable)
        act_writel(act_readl(GPIO_ADAT ) & (~(0x01 << 23)),GPIO_ADAT);
    else
        act_writel(act_readl(GPIO_ADAT ) | (0x01 << 23),GPIO_ADAT);

    return 0;
}
EXPORT_SYMBOL(tc358768_shutdown);

int tc358768_regs_printk() {
	 tc358768_shutdown(0);
     tc358768_dev->read_dev(0);
     tc358768_dev->read_dev(0x2);
     tc358768_dev->read_dev(0x2);
     tc358768_dev->read_dev(0x0016);
     tc358768_dev->read_dev(0x0018);
     tc358768_dev->read_dev(0x0018);
     tc358768_dev->read_dev(0x0006);
     tc358768_dev->read_dev(0x0140);
     tc358768_dev->read_dev(0x0142);
     tc358768_dev->read_dev(0x0144);
     tc358768_dev->read_dev(0x0146);
     tc358768_dev->read_dev(0x0148);
     tc358768_dev->read_dev(0x014A);
     tc358768_dev->read_dev(0x014C);
     tc358768_dev->read_dev(0x014E);
     tc358768_dev->read_dev(0x0150);
     tc358768_dev->read_dev(0x0152);
     tc358768_dev->read_dev(0x0100);
     tc358768_dev->read_dev(0x0102);
     tc358768_dev->read_dev(0x0104);
     tc358768_dev->read_dev(0x0106);
     tc358768_dev->read_dev(0x0108);
     tc358768_dev->read_dev(0x010A);
     tc358768_dev->read_dev(0x010C);
     tc358768_dev->read_dev(0x010E);
     tc358768_dev->read_dev(0x0110);
     tc358768_dev->read_dev(0x0112);
     tc358768_dev->read_dev(0x0210);
     tc358768_dev->read_dev(0x0212);
     tc358768_dev->read_dev(0x0214);
     tc358768_dev->read_dev(0x0216);
     tc358768_dev->read_dev(0x0218);
     tc358768_dev->read_dev(0x021A);
     tc358768_dev->read_dev(0x0220);
     tc358768_dev->read_dev(0x0222);
     tc358768_dev->read_dev(0x0224);
     tc358768_dev->read_dev(0x0226);
     tc358768_dev->read_dev(0x022C);
     tc358768_dev->read_dev(0x022E);
     tc358768_dev->read_dev(0x0230);
     tc358768_dev->read_dev(0x0232);
     tc358768_dev->read_dev(0x0234);
     tc358768_dev->read_dev(0x0236);
     tc358768_dev->read_dev(0x023C);
     tc358768_dev->read_dev(0x023E);
     tc358768_dev->read_dev(0x0204);
     tc358768_dev->read_dev(0x0206);
     tc358768_dev->read_dev(0x0620);
     tc358768_dev->read_dev(0x0622);
     tc358768_dev->read_dev(0x0624);
     tc358768_dev->read_dev(0x0626);
     tc358768_dev->read_dev(0x0628);
     tc358768_dev->read_dev(0x062A);
     tc358768_dev->read_dev(0x062C);
	 tc358768_dev->read_dev(0x0518);
	 tc358768_dev->read_dev(0x051A);
	 tc358768_dev->read_dev(0x0602);
	 tc358768_dev->read_dev(0x0604);
	 tc358768_dev->read_dev(0x0610);
	 tc358768_dev->read_dev(0x0600);
	 tc358768_dev->read_dev(0x0238);
	 tc358768_dev->read_dev(0x023A);
	 tc358768_dev->read_dev(0x0500);
	 tc358768_dev->read_dev(0x0502);
	 tc358768_dev->read_dev(0x0500);
	 tc358768_dev->read_dev(0x0502);
	 tc358768_dev->read_dev(0x0008);
	 tc358768_dev->read_dev(0x0050);
	 tc358768_dev->read_dev(0x0032);
	 tc358768_dev->read_dev(0x0004);
     return 0;
}

int tc358768_init(int xres , int yres ,int hbp ,int hfp, int hsw, int vbp ,int vfp, int vsw)
{
    int id;
    //return;
     //tc358768_dev->read_dev(0);
     tc358768_shutdown(0);
     tc358768_dev->write_dev(0x2,0x1);
     //tc358768_dev->read_dev(0x2);
     mdelay(1);    
     tc358768_dev->write_dev(0x2,0x00);
     tc358768_dev->write_dev(0x0016,0x3055);
     tc358768_dev->write_dev(0x0018,0x0203);
     mdelay(1);
     tc358768_dev->write_dev(0x0018,0x0213);
     tc358768_dev->write_dev(0x0006,0x012C);
     tc358768_dev->write_dev(0x0140,0x0000);
     tc358768_dev->write_dev(0x0142,0x0000);
     tc358768_dev->write_dev(0x0144,0x0000);
     tc358768_dev->write_dev(0x0146,0x0000);
     tc358768_dev->write_dev(0x0148,0x0000);
     tc358768_dev->write_dev(0x014A,0x0000);
     tc358768_dev->write_dev(0x014C,0x0000);
     tc358768_dev->write_dev(0x014E,0x0000);
     tc358768_dev->write_dev(0x0150,0x0000);
     tc358768_dev->write_dev(0x0152,0x0000);
     tc358768_dev->write_dev(0x0100,0x0002);
     tc358768_dev->write_dev(0x0102,0x0000);
     tc358768_dev->write_dev(0x0104,0x0002);
     tc358768_dev->write_dev(0x0106,0x0000);
     tc358768_dev->write_dev(0x0108,0x0002);
     tc358768_dev->write_dev(0x010A,0x0000);
     tc358768_dev->write_dev(0x010C,0x0002);
     tc358768_dev->write_dev(0x010E,0x0000);
     tc358768_dev->write_dev(0x0110,0x0002);
     tc358768_dev->write_dev(0x0112,0x0000);
     tc358768_dev->write_dev(0x0210,0x1388);
     tc358768_dev->write_dev(0x0212,0x0000);
     tc358768_dev->write_dev(0x0214,0x0004);
     tc358768_dev->write_dev(0x0216,0x0000);
     tc358768_dev->write_dev(0x0218,0x1202);
     tc358768_dev->write_dev(0x021A,0x0000);
     tc358768_dev->write_dev(0x0220,0x0003);
     tc358768_dev->write_dev(0x0222,0x0000);
     tc358768_dev->write_dev(0x0224,0x3A98);
     tc358768_dev->write_dev(0x0226,0x0000);
     tc358768_dev->write_dev(0x022C,0x0002);
     tc358768_dev->write_dev(0x022E,0x0000);
     tc358768_dev->write_dev(0x0230,0x0005);
     tc358768_dev->write_dev(0x0232,0x0000);
     tc358768_dev->write_dev(0x0234,0x001F);
     tc358768_dev->write_dev(0x0236,0x0000);
     tc358768_dev->write_dev(0x023C,0x0003);
     tc358768_dev->write_dev(0x023E,0x0004);
     tc358768_dev->write_dev(0x0204,0x0001);
     tc358768_dev->write_dev(0x0206,0x0000);
     tc358768_dev->write_dev(0x0620,0x0001);
     tc358768_dev->write_dev(0x0622,0x004E);
     tc358768_dev->write_dev(0x0624,0x0026);
     tc358768_dev->write_dev(0x0626,0x0400);
     tc358768_dev->write_dev(0x0628,0x0204);
     tc358768_dev->write_dev(0x062A,0x0102);
     tc358768_dev->write_dev(0x062C,0x0900);
	 tc358768_dev->write_dev(0x0518,0x0001);
	 tc358768_dev->write_dev(0x051A,0x0000);
	 mdelay(5);    
	 tc358768_dev->write_dev(0x0602,0x1005);
	 tc358768_dev->write_dev(0x0604,0x0000);
	 tc358768_dev->write_dev(0x0610,0x0011);
	 tc358768_dev->write_dev(0x0600,0x0001);
	 mdelay(100);   
	 tc358768_dev->write_dev(0x0238,0x0001);
	 tc358768_dev->write_dev(0x023A,0x0000);
	 mdelay(150); 
	 tc358768_dev->write_dev(0x0602,0x1005);
	 tc358768_dev->write_dev(0x0604,0x0000);
	 tc358768_dev->write_dev(0x0610,0x0029);
	 tc358768_dev->write_dev(0x0600,0x0001);
	 mdelay(100);
	 tc358768_dev->write_dev(0x0500,0x0086);
	 tc358768_dev->write_dev(0x0502,0xA300);
	 tc358768_dev->write_dev(0x0500,0x8000);
	 tc358768_dev->write_dev(0x0502,0xC300);

	 tc358768_dev->write_dev(0x0008,0x0037);
	 tc358768_dev->write_dev(0x0050,0x003E);
	 tc358768_dev->write_dev(0x0032,0x0000);
	 tc358768_dev->write_dev(0x0004,0x0044);
	 
     return 0;
}
EXPORT_SYMBOL(tc358768_init);

int tc358768_open()
{
	/*
	 tc358768_dev->write_dev(0x0008,0x0037);
	 tc358768_dev->write_dev(0x0050,0x003E);
	 tc358768_dev->write_dev(0x0032,0x0000);
	 tc358768_dev->write_dev(0x0004,0x0044);
	 
	 tc358768_dev->write_dev(0x0602,0x1005);
	 tc358768_dev->write_dev(0x0604,0x0000);
	 tc358768_dev->write_dev(0x0610,0x0029);
	 tc358768_dev->write_dev(0x0600,0x0001);
	 mdelay(100);   
	 */
    //tc358768_regs_printk();
    return 0;

}
EXPORT_SYMBOL(tc358768_open);

int tc358768_close()
{
	tc358768_shutdown(1);   
	tc358768_dev->write_dev(0x0602,0x1005);
	tc358768_dev->write_dev(0x0604,0x0000);
	tc358768_dev->write_dev(0x0610,0x0028);
	tc358768_dev->write_dev(0x0600,0x0001);
	mdelay(40);
	tc358768_dev->write_dev(0x0602,0x1005);
	tc358768_dev->write_dev(0x0604,0x0000);
	tc358768_dev->write_dev(0x0610,0x0010);
	tc358768_dev->write_dev(0x0600,0x0001);
	mdelay(80);
	tc358768_dev->write_dev(0x0032,0x8000);
	mdelay(40);
	tc358768_dev->write_dev(0x0004,0x0004);
	tc358768_dev->write_dev(0x0032,0xC000);
	tc358768_dev->write_dev(0x0238,0x0000);
	tc358768_dev->write_dev(0x023A,0x0000);
	tc358768_dev->write_dev(0x0140,0x0001);
	tc358768_dev->write_dev(0x0142,0x0000);
	tc358768_dev->write_dev(0x0144,0x0001);
	tc358768_dev->write_dev(0x0146,0x0000);
	tc358768_dev->write_dev(0x0148,0x0001);
	tc358768_dev->write_dev(0x014A,0x0000);
	tc358768_dev->write_dev(0x014C,0x0001);
	tc358768_dev->write_dev(0x014E,0x0000);
	tc358768_dev->write_dev(0x0150,0x0001);
	tc358768_dev->write_dev(0x0152,0x0000);
}

EXPORT_SYMBOL(tc358768_close);

static int __devinit tc358768_spi_probe(struct spi_device *spi)
{
    pr_info("%s()\n", __FUNCTION__);

    tc358768_dev = kzalloc(sizeof(struct tc358768_spi_dev), GFP_KERNEL);
    if (tc358768_dev == NULL)
        return -ENOMEM;

    tc358768_dev->dev = &spi->dev;
    tc358768_dev->control_data = spi;
    tc358768_dev->read_dev = tc358768_read_reg;
    tc358768_dev->write_dev = tc358768_write_reg;
    tc358768_dev->send_cmd_dev = tc358768_write_cmd;

    spi->bits_per_word = 16;
    spi->mode = SPI_MODE_3;
    dev_set_drvdata(&spi->dev, tc358768_dev);

    //tc358768_regs_printk();
    return 0;
}

static int __devexit tc358768_spi_remove(struct spi_device *spi)
{
    spi_set_drvdata(spi, NULL);
    return 0;
}

static struct spi_driver tc358768_spi_driver = {
    .driver = {
        .name   = "tc358768",
        .bus    = &spi_bus_type,
        .owner  = THIS_MODULE,
    },
    .probe      = tc358768_spi_probe,
    .remove     = __devexit_p(tc358768_spi_remove)
};

static struct asoc_spi_controller_data tc358768_spi_controller_data = {
    .cs_gpio = ASOC_GPIO_PORTA(15),
};

static struct spi_board_info tc358768_spi_board_info = {
    .modalias = "tc358768",
    .max_speed_hz = 100000,
    .bus_num = 1,
    .chip_select = 1,
    .mode = SPI_MODE_3,
    .platform_data = NULL,
    .controller_data = &tc358768_spi_controller_data,
};
static int is_init = 0;
int tc358768_spi_init(void)
{
    int ret;
    
    struct spi_master *master;
    struct spi_device *spi;
    struct spi_board_info *bi;
    if(is_init){
        return 0;
    }
    is_init = 1;
    bi = &tc358768_spi_board_info;

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

    ret = spi_register_driver(&tc358768_spi_driver);
    if (ret != 0) {
        pr_err("Failed to register spi_test driver: %d\n", ret);
        spi_unregister_device(spi);
        return ret;
    }

    tc358768_spi_device_p = spi;

    return ret;
}
EXPORT_SYMBOL(tc358768_spi_init);
void tc358768_spi_exit(void)
{
    spi_unregister_driver(&tc358768_spi_driver);
    spi_unregister_device(tc358768_spi_device_p);
}
EXPORT_SYMBOL(tc358768_spi_exit);
//module_init(tc358768_spi_init);
//module_exit(tc358768_spi_exit);

//MODULE_DESCRIPTION(" Devices tc358768 SPI Driver");
//MODULE_AUTHOR("wanghui <wanghui@actions-semi.com>");
//MODULE_LICENSE("GPL");
