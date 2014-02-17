/*
    max6875.c - driver for MAX6874/MAX6875

    Copyright (C) 2005 Ben Gardner <bgardner@wabtec.com>

    Based on i2c/chips/eeprom.c

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
*/
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/cdev.h>
#include <asm/delay.h>
#include <linux/delay.h>
#include <mach/actions_reg_gl5202.h>
#include "i2c_hdmi_edid.h"
#include <mach/hardware.h>
#include "asoc_hdmi.h"

#define I2C_READ_EDID_TEST		0
#define I2C_HDMI_EDID_DEBUG_INFO		1
#define I2C_BOARD_INFO_IN_DRV	
#if (I2C_HDMI_EDID_DEBUG_INFO == 1)
#define HDMI_EDID_DEBUG(fmt, args...)	\
	printk(KERN_INFO "I2C_HDMI_EDID_DRV: "fmt, ##args)
#else
#define HDMI_EDID_DEBUG(fmt, args...)
#endif

#define HDMI_EDID_ADDR		(0x60 >> 1)
#define HDMI_EDID_DDC_ADDR	(0xa0 >> 1)
#define HDMI_EDID_EXT_TAG_ADDR 	0x7e

#define HDCP_ADDR	(0x74 >> 1)

struct edid_dev {
	struct i2c_client *client;
	
};

static struct i2c_board_info i2c_hdmi_devices ={	
	I2C_BOARD_INFO("i2c_hdmi_edid", 0x60),	
};

static const struct i2c_device_id hdmi_edid_id[] = {
	{ "i2c_hdmi_edid", 1 },
	{ }
};
MODULE_DEVICE_TABLE(i2c,hdmi_edid_id);

struct edid_dev *edid_devp;
int first_flag;

#if (I2C_READ_EDID_TEST == 1)
s32 read_edid_test(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 k;
	u8 pbuf[128];
	int ret_val;

	struct i2c_msg msg[3];
	struct i2c_adapter *adap;
	struct i2c_client *client;
	
	HDMI_EDID_DEBUG("[%s start]",__func__);
	
	adap = edid_devp->client->adapter;
	client = edid_devp->client;

	/* set segment pointer */
	
	msg[0].addr = 0x60 >> 1;
	msg[0].flags = client->flags | I2C_M_IGNORE_NAK;
	msg[0].buf = &i;
	msg[0].len = 1;
	msg[1].addr = 0xa0 >> 1;
	msg[1].flags = client->flags;
	msg[1].buf = &j;
	msg[1].len = 1;
	msg[2].addr = 0xa0 >> 1;
	msg[2].flags = client->flags  | I2C_M_RD;
	msg[2].buf = pbuf;
	msg[2].len = HDMI_EDID_BLOCK_SIZE;
	ret_val = i2c_transfer(adap, msg, 3);
	if (3 != ret_val) {
		HDMI_EDID_DEBUG("[in %s]fail to read EDID\n",__func__);
		return -1;
		
	} else {
		HDMI_EDID_DEBUG
			("[in %s]finsh reading edid base block\n",__func__);
		for (k = 0; k < 128; k++) {
			HDMI_EDID_DEBUG("[in %s]the %dth,0x%x\n",__func__,k,pbuf[k]);
		}
		
	}

	HDMI_EDID_DEBUG("[%s finished]",__func__);

	return 0;
}
#endif

#ifdef I2C_BOARD_INFO_IN_KERNEL
static int i2c_hdmi_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	/* allocate memory */
	edid_devp = kzalloc(sizeof(struct edid_dev),GFP_KERNEL);
	
	HDMI_EDID_DEBUG("[%s start]",__func__);
	
	if (NULL == edid_devp) {
		HDMI_EDID_DEBUG(KERN_ERR"fail to allocate memory for client\n");
		return -ENOMEM;
		
	}
	
	edid_devp->client = client;
	
#if (I2C_READ_EDID_TEST == 1)
	read_edid_test();
#endif 

	return 0;
}
#endif

#ifdef I2C_BOARD_INFO_IN_KERNEL
static struct i2c_driver i2c_hdmi_edid_driver = {
	.driver = {
		.name	= "i2c_hdmi_edid",
		.owner  = THIS_MODULE,
	},
	.probe	= i2c_hdmi_probe,
	.id_table	= hdmi_edid_id,
};
#endif

int  i2c_hdmi_init(void)
{	
	HDMI_EDID_DEBUG("[%s start]\n", __func__);
#ifdef I2C_BOARD_INFO_IN_KERNEL
	int ret;
	ret=i2c_add_driver(&i2c_hdmi_edid_driver);
	return ret;
#endif

#ifdef  I2C_BOARD_INFO_IN_DRV
	if (!first_flag) {
		struct i2c_adapter *i2c_adap;  
		//struct i2c_board_info i2c_info;  
		edid_devp = kzalloc(sizeof(struct edid_dev),GFP_KERNEL);
          	        
		i2c_adap = i2c_get_adapter(2);  
		if (!i2c_adap) {
			HDMI_EDID_DEBUG("hdmi  adapter error!\n");
			goto err;
		}
	 
		edid_devp->client = i2c_new_device(i2c_adap, &i2c_hdmi_devices);  
		i2c_put_adapter(i2c_adap); 
		first_flag++;
	}

	return 0;
err:
	kfree(edid_devp);
	return -EFAULT;

#endif
}

static void i2c_hdmi_exit(void)
{
#ifdef I2C_BOARD_INFO_IN_KERNEL
	i2c_del_driver(&i2c_hdmi_edid_driver);
#endif
}

#ifdef I2C_RW_DEBUG
#define I2CMODE  0x00
int transmit_byte(void)
{
      //HDCP_DEBUG("[enter %s]\n", __func__);
	while(!(act_readl(I2C2_STAT) & 0x80));	//wait 
	
	if (act_readl(I2C2_STAT) & 0x1)			//judge ACK
	{
		act_writel(0x80, I2C2_STAT);
		return(1);
	}
	else
		{
		HDCP_DEBUG("transmit_byte no ack\n");
		act_writel(0x80, I2C2_STAT);
		return(0);
	}
}

int Wait_stop(void)
{         
	while(!(act_readl(I2C2_STAT) & 0x10));
	return 1;
}

int wait_byte_receive(void)
{
	while(!(act_readl(I2C2_STAT) & 0x80));	//wait
	act_writel(0x80, I2C2_STAT);
	return 1;
}
#endif

int i2c_hdcp_write(const char *buf, unsigned short offset, int count) {
#ifdef I2C_RW_DEBUG
    int i2c_error = 0;
    int write_counter = 0;
    act_writel((act_readl(I2C2_CTL) | (1 << 7)) & ~(1 << 5), I2C2_CTL);

    act_writel(0x74, I2C2_TXDAT); // write device address of HDCP 0x74 
    act_writel(0x86 + I2CMODE, I2C2_CTL); //generate start condition
    if (!transmit_byte()) {
        i2c_error = 1;
        //   return 0;
    }
    act_writel(offset + write_counter, I2C2_TXDAT);
    act_writel(0x82 + I2CMODE, I2C2_CTL); //destination addr ,release I2C
    if (!transmit_byte()) {
        i2c_error = 1;
        //return 0;
    }
    while (write_counter < count) {
        act_writel(*(buf + write_counter), I2C2_TXDAT);
        act_writel(0x82 + I2CMODE, I2C2_CTL); //release I2C
        if (!transmit_byte()) {
            i2c_error = 1;
            //return 0;
        }
        write_counter++;
    }

    act_writel(0x8a + I2CMODE, I2C2_CTL); //generate stop condition,release I2C
    if (!Wait_stop()) {
        i2c_error = 1;
        return 0;
    }

    msleep(1);
    return 1;
#else
    int ret;
    int i;
    struct i2c_client *client = edid_devp->client;
    struct i2c_adapter *adap = client->adapter;

    struct i2c_msg msg;

    msg.addr = HDCP_ADDR;
    msg.flags = client->flags | I2C_M_IGNORE_NAK;
    msg.len = count;
    msg.buf = (char *)buf;

    ret = i2c_transfer(adap, &msg, 1);

    /*
     * If everything went ok (i.e. 1 msg transmitted), return #bytes
     * transmitted, else error code.
     */
    return (ret == 2) ? count : ret;
#endif
}

int i2c_hdcp_read(char *buf, unsigned short offset, int count) {

#ifdef I2C_RW_DEBUG
   int i2c_error = 0;
    int read_counter = 0;
    HDCP_DEBUG("i2c normal read*************************************\n");
    act_writel((act_readl(I2C2_CTL) | (1 << 7)) & ~(1 << 5), I2C2_CTL);
    act_writel(0x74,I2C2_TXDAT); //write device address of HDCP 

    //  P_I2C2_CTL = 0x86+I2CMODE;      //generate start condition
    act_writel(0x96 + I2CMODE, I2C2_CTL); //generate start condition
    if (!transmit_byte()) {
        i2c_error = 1;
//      return 0;   
    }

    act_writel(offset + read_counter,I2C2_TXDAT); //read addr;
//  P_I2C2_CTL = 0x82+I2CMODE;                  //release I2C
    act_writel(0x92 + I2CMODE, I2C2_CTL); //release I2C
    if (!transmit_byte()) {
        i2c_error = 1;
//      return 0;   
    }
    udelay(2);
    act_writel(0x75,I2C2_TXDAT); //Read device address of HDCP
//  P_I2C2_CTL = I2CMODE + 0x8e; 
    act_writel(I2CMODE + 0x9e,I2C2_CTL); 
    if (!transmit_byte()) {
        i2c_error = 1;
//      return 0;   
    }

    while (read_counter < (count - 1)) {
        act_writel(0x82 + I2CMODE,I2C2_CTL); //release I2C 
        if (!wait_byte_receive()) {
            i2c_error = 1;
            return 0;
        }

        *(buf + read_counter) = act_readl(I2C2_RXDAT);

        read_counter++;
    }

     act_writel(0x83 + I2CMODE,I2C2_CTL); //when read the last byte data,do not generate the ACK
    if (!wait_byte_receive()) {
        i2c_error = 1;
        return 0;
    }

    *(buf + read_counter) = act_readl(I2C2_RXDAT);
     act_writel(0x8a + I2CMODE,I2C2_CTL); //generate stop condition,release I2C  

    if (!(Wait_stop())) {
        i2c_error = 1;
        return 0;
    }

    msleep(1);
    HDCP_DEBUG("**********i2c hdcp read successful *************\n");
    int i;
    for (i = 0; i < count; i++)
    	HDCP_DEBUG("i2c hdcp read :buf[%d]   %d\n", i, buf[i]);
    if (i2c_error == 0)
        return 1;
    else
        return 0;

#else
    struct i2c_client *client = edid_devp->client;
    struct i2c_adapter *adap = client->adapter;
    struct i2c_msg msg[2];
    int ret;
    int i;

    msg[0].addr = HDCP_ADDR;
    msg[0].flags = client->flags | I2C_M_IGNORE_NAK;
    msg[0].buf = &offset;
    msg[0].len = 1;
    msg[1].addr = HDCP_ADDR;
    msg[1].flags = client->flags | I2C_M_RD;
    msg[1].buf = buf;
    msg[1].len = count;

    ret = i2c_transfer(adap, &msg, 2);
    
    for (i = 0; i < count; i++)
    	HDCP_DEBUG("i2c hdcp read :buf[%d]   %d\n", i, msg[1].buf[i]);

    /*
     * If everything went ok (i.e. 1 msg received), return #bytes received,
     * else error code.
     */
    return (ret == 2) ? count : ret;
#endif

#if 0
    act_writel((act_readl(I2C2_CTL) | (1 << 7)) & (1 << 5), I2C2_CTL);

    act_writel(count,  I2C2_DATCNT);
    act_writel(0x74,  I2C2_TXDAT);
    act_writel(offset,  I2C2_TXDAT);
    act_writel(0x75,  I2C2_TXDAT);
    act_writel(0x8805, I2C2_CMD);
    while(!(act_readl(I2C2_FIFOSTAT) & 0X1));

    udelay(5);
    
    act_writel(0x8f30, I2C2_CMD);
    while(!(act_readl(I2C2_FIFOSTAT) & 0X1));
    
#endif
}

/**
* i2c_read_hdmi_edid - read edid segment info
* @pBuf:store the info reading from the edid
* @segment_index:indicate segment index
* return value:when segment0 return ext tag;
* when segmentx(x>0) return 0; when error return -EINVAL.
* 
*/
s32 read_hdmi_edid_block(u8 *pBuf, u8 segment_index, u8 segment_offset)
{
	int ret;
	int retry_num = 0;
	char segment_pointer;
	struct i2c_msg msg[3];
	struct i2c_adapter *adap;
	struct i2c_client *client;
	
	HDMI_EDID_DEBUG("[%s start]\n",__func__);
	
	//if (segment_index >= 256)
	//	return -EINVAL;

RETRY:
	retry_num++;

	/*add i2c driver*/
	adap = edid_devp->client->adapter;
	client = edid_devp->client;
	segment_pointer = (char)segment_index;
	
	/* set segment pointer */
	msg[0].addr = HDMI_EDID_ADDR;
	msg[0].flags = client->flags | I2C_M_IGNORE_NAK;
	msg[0].buf = &segment_pointer;
	msg[0].len = 1;
	msg[1].addr = HDMI_EDID_DDC_ADDR;
	msg[1].flags = client->flags;
	msg[1].buf = &segment_offset;
	msg[1].len = 1;
	msg[2].addr = HDMI_EDID_DDC_ADDR;
	msg[2].flags = client->flags  | I2C_M_RD;
	msg[2].buf = pBuf;
	msg[2].len = HDMI_EDID_BLOCK_SIZE;

	ret = i2c_transfer(adap, msg, 3);

	if (ret != 3) {
		
		HDMI_EDID_DEBUG("[in %s]fail to read EDID\n",__func__);
		ret = -1;
		goto RETURN1;
		
	} //else {
	
		//if(segment_index == 0) {
			
			//ret = (int)(*(pBuf + HDMI_EDID_EXT_TAG_ADDR));
			
		//} else {

		//	ret = 0;

		//}
	//}
	HDMI_EDID_DEBUG("[%s finished]\n",__func__);

RETURN1:
	if ((ret < 0) && (retry_num < 3)) {
		
		HDMI_EDID_DEBUG("ret_val1 is %d,retry_num is %d\n",ret,retry_num);
		HDMI_EDID_DEBUG("[in %s]the %dth read EDID error,try again\n",
			__func__,retry_num);

		goto RETRY;
		
	} else {
	
		return ret;
	}
}

MODULE_AUTHOR("chenbo <chenbo@actions-semi.com>");
MODULE_DESCRIPTION("hdmi_edid driver");
MODULE_LICENSE("GPL");
