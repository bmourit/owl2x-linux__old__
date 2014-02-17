
#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <linux/uaccess.h>

// device info
#define SENSOR_NAME                   "ltr301"
#define SENSOR_I2C_ADDR            0x23
#define ABSMIN                              0
#define ABSMAX                             64000
#define MAX_DELAY                       500

// constant define
#define LTR301_PART_ID                      0x80
#define LTR301_MANUFAC_ID               0x05
#define LTR301_MODE_STANDBY          0x00
#define LTR301_MODE_ACTIVE             0x02

// register define
#define ALS_CONTR_REG                      0x80
#define ALS_MEAS_RATE_REG               0x85
#define PART_ID_REG                           0x86
#define MANUFAC_ID_REG                    0x87
#define ALS_DATA_CH1_0_REG            0x88
#define ALS_DATA_CH1_1_REG            0x89
#define ALS_DATA_CH0_0_REG            0x8A
#define ALS_DATA_CH0_1_REG            0x8B
#define ALS_STATUS_REG                    0x8C

// register bits define
#define ALS_MODE_BIT__POS            0
#define ALS_MODE_BIT__LEN            2
#define ALS_MODE_BIT__MSK            0x03
#define ALS_MODE_BIT__REG            ALS_CONTR_REG


#define LTR301_GET_BITSLICE(regvar, bitname)\
    ((regvar & bitname##__MSK) >> bitname##__POS)

#define LTR301_SET_BITSLICE(regvar, bitname, val)\
    ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))


struct ltr301_lux{
    u16    ch0;
    u16    ch1;
    u16    lux_val;
} ;

struct ltr301_data {
    struct i2c_client *ltr301_client;
    struct input_dev *input;
    atomic_t delay;
    atomic_t enable;
    struct mutex enable_mutex;
    struct delayed_work work;
#ifdef CONFIG_HAS_EARLYSUSPEND
    struct early_suspend early_suspend;
#endif
};

// cfg data : 1-- used
#define CFG_SENSOR_USE_CONFIG  0

/*******************************************
* for xml cfg
*******************************************/
#define CFG_SENSOR_ADAP_ID          "gsensor.i2c_adap_id"

extern int get_config(const char *key, char *buff, int len);
/*******************************************
* end for xml cfg
*******************************************/

#ifdef CONFIG_HAS_EARLYSUSPEND
static void ltr301_early_suspend(struct early_suspend *h);
static void ltr301_early_resume(struct early_suspend *h);
#endif

static int ltr301_smbus_read_byte(struct i2c_client *client,
        unsigned char reg_addr, unsigned char *data)
{
    s32 dummy;
    dummy = i2c_smbus_read_byte_data(client, reg_addr);
    if (dummy < 0)
        return -1;
    *data = dummy & 0x000000ff;

    return 0;
}

static int ltr301_smbus_write_byte(struct i2c_client *client,
        unsigned char reg_addr, unsigned char *data)
{
    s32 dummy;
    dummy = i2c_smbus_write_byte_data(client, reg_addr, *data);
    if (dummy < 0)
        return -1;
    return 0;
}


static int ltr301_set_mode(struct i2c_client *client, unsigned char mode)
{
    int comres = 0;
    unsigned char data = 0;

    comres = ltr301_smbus_read_byte(client, ALS_MODE_BIT__REG, &data);    
    data  = LTR301_SET_BITSLICE(data, ALS_MODE_BIT, mode);
    comres += ltr301_smbus_write_byte(client, ALS_MODE_BIT__REG, &data);

    return comres;
}

static int ltr301_get_mode(struct i2c_client *client, unsigned char *mode)
{
    int comres = 0;
    unsigned char data = 0;

    comres = ltr301_smbus_read_byte(client, ALS_CONTR_REG, &data);
    *mode  = LTR301_GET_BITSLICE(data, ALS_MODE_BIT);

    return comres;
}

static int ltr301_read_data(struct i2c_client *client, struct ltr301_lux *lux)
{
    int comres = 0;
    unsigned char data0 = 0, data1 = 0;
    int ratio, luxdata_int;

    comres += ltr301_smbus_read_byte(client, ALS_DATA_CH0_0_REG, &data0);
    comres += ltr301_smbus_read_byte(client, ALS_DATA_CH0_1_REG, &data1);
    lux->ch0 = (data1 << 8) | data0;
    
    comres += ltr301_smbus_read_byte(client, ALS_DATA_CH1_0_REG, &data0);
    comres += ltr301_smbus_read_byte(client, ALS_DATA_CH1_1_REG, &data1);
    lux->ch1 = (data1 << 8) | data0;

    /* Compute Lux data from ALS data (ch0 and ch1)
     * ============================================
     * For Ratio <= 0.63:
     * 1.3618*CH0 - 1.645*CH1
     * For 0.63 < Ratio <= 1:
     * 0.4676*CH0 - 0.259*CH1
     * Ratio > 1:
     * 0
     * For high gain, calculated lux/150
     */
    if (lux->ch0 == 0) {
        lux->ch0 = 1;   //avoid zero-div exception
    }
    
    ratio = (lux->ch1 * 100) / lux->ch0;

    if (ratio <= 63){
        luxdata_int = ((13618 * lux->ch0) - (16450 * lux->ch1) + 5000) / 10000;
    }
    else if ((ratio > 63) && (ratio <= 100)){
        luxdata_int = ((4676 * lux->ch0) - (2590 * lux->ch1) + 5000) / 10000;
    }
    else {
        luxdata_int = 0;
    }

    lux->lux_val = luxdata_int;

    return comres;
}


static ssize_t ltr301_register_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf, size_t count)
{
    int address, value;
    struct i2c_client *client = to_i2c_client(dev);
    struct ltr301_data *ltr301 = i2c_get_clientdata(client);

    sscanf(buf, "[0x%x]=0x%x", &address, &value);
    
    if (ltr301_smbus_write_byte(ltr301->ltr301_client, (unsigned char)address,
                (unsigned char *)&value) < 0)
        return -EINVAL;

    return count;
}

static ssize_t ltr301_register_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ltr301_data *ltr301 = i2c_get_clientdata(client);    
    size_t count = 0;
    u8 reg[0x1f];
    int i;
    
    for (i = 0 ; i < 0x1f; i++) {
        ltr301_smbus_read_byte(ltr301->ltr301_client, 0x80+i, reg+i);    
        count += sprintf(&buf[count], "0x%x: 0x%x\n", 0x80+i, reg[i]);
    }
    
    return count;
}

static ssize_t ltr301_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    unsigned char data;
    struct i2c_client *client = to_i2c_client(dev);
    struct ltr301_data *ltr301 = i2c_get_clientdata(client);

    if (ltr301_get_mode(ltr301->ltr301_client, &data) < 0)
        return sprintf(buf, "Read error\n");

    return sprintf(buf, "%d\n", data);
}

static ssize_t ltr301_mode_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf, size_t count)
{
    unsigned long data;
    int error;
    struct i2c_client *client = to_i2c_client(dev);
    struct ltr301_data *ltr301 = i2c_get_clientdata(client);

    error = strict_strtoul(buf, 10, &data);
    if (error)
        return error;
    if (ltr301_set_mode(ltr301->ltr301_client, (unsigned char) data) < 0)
        return -EINVAL;

    return count;
}

static ssize_t ltr301_value_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct ltr301_data *ltr301 = input_get_drvdata(input);
    struct ltr301_lux lux;

    ltr301_read_data(ltr301->ltr301_client, &lux);

    return sprintf(buf, "%d %d %d\n", lux.ch0, lux.ch1, lux.lux_val);
}

static ssize_t ltr301_delay_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ltr301_data *ltr301 = i2c_get_clientdata(client);

    return sprintf(buf, "%d\n", atomic_read(&ltr301->delay));

}

static ssize_t ltr301_delay_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf, size_t count)
{
    unsigned long data;
    int error;
    struct i2c_client *client = to_i2c_client(dev);
    struct ltr301_data *ltr301 = i2c_get_clientdata(client);

    error = strict_strtoul(buf, 10, &data);
    if (error)
        return error;

    atomic_set(&ltr301->delay, (unsigned int) data);

    return count;
}


static ssize_t ltr301_enable_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ltr301_data *ltr301 = i2c_get_clientdata(client);

    return sprintf(buf, "%d\n", atomic_read(&ltr301->enable));

}

static void ltr301_set_enable(struct device *dev, int enable)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ltr301_data *ltr301 = i2c_get_clientdata(client);
    int pre_enable = atomic_read(&ltr301->enable);

    mutex_lock(&ltr301->enable_mutex);
    if (enable) {
        if (pre_enable == 0) {
            ltr301_set_mode(ltr301->ltr301_client, LTR301_MODE_ACTIVE);
            schedule_delayed_work(&ltr301->work,
                msecs_to_jiffies(atomic_read(&ltr301->delay)));
            atomic_set(&ltr301->enable, 1);
        }

    } else {
        if (pre_enable == 1) {
            ltr301_set_mode(ltr301->ltr301_client, LTR301_MODE_STANDBY);
            cancel_delayed_work_sync(&ltr301->work);
            atomic_set(&ltr301->enable, 0);
        }
    }
    mutex_unlock(&ltr301->enable_mutex);
}

static ssize_t ltr301_enable_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf, size_t count)
{
    unsigned long data;
    int error;

    error = strict_strtoul(buf, 10, &data);
    if (error)
        return error;
    if ((data == 0) || (data == 1))
        ltr301_set_enable(dev, data);

    return count;
}

static DEVICE_ATTR(mode, S_IRUGO|S_IWUSR|S_IWGRP|S_IWOTH,
        ltr301_mode_show, ltr301_mode_store);
static DEVICE_ATTR(value, S_IRUGO,
        ltr301_value_show, NULL);
static DEVICE_ATTR(delay, S_IRUGO|S_IWUSR|S_IWGRP|S_IWOTH,
        ltr301_delay_show, ltr301_delay_store);
static DEVICE_ATTR(enable, S_IRUGO|S_IWUSR|S_IWGRP|S_IWOTH,
        ltr301_enable_show, ltr301_enable_store);
static DEVICE_ATTR(reg, S_IRUGO|S_IWUSR|S_IWGRP|S_IWOTH,
        ltr301_register_show, ltr301_register_store);

static struct attribute *ltr301_attributes[] = {
    &dev_attr_mode.attr,
    &dev_attr_value.attr,
    &dev_attr_delay.attr,
    &dev_attr_enable.attr,
    &dev_attr_reg.attr,
    NULL
};

static struct attribute_group ltr301_attribute_group = {
    .attrs = ltr301_attributes
};

static void ltr301_work_func(struct work_struct *work)
{
    struct ltr301_data *ltr301 = container_of((struct delayed_work *)work,
            struct ltr301_data, work);
    static struct ltr301_lux lux;
    unsigned long delay = msecs_to_jiffies(atomic_read(&ltr301->delay));
    
    ltr301_read_data(ltr301->ltr301_client, &lux);
    input_report_abs(ltr301->input, ABS_MISC, lux.lux_val);
    input_sync(ltr301->input);
    
    schedule_delayed_work(&ltr301->work, delay);
}

static int ltr301_probe(struct i2c_client *client,
        const struct i2c_device_id *id)
{
    int err = 0;
    unsigned char pid;
    struct ltr301_data *data;
    struct input_dev *dev;

    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        printk(KERN_INFO "i2c_check_functionality error\n");
        goto exit;
    }
    data = kzalloc(sizeof(struct ltr301_data), GFP_KERNEL);
    if (!data) {
        err = -ENOMEM;
        goto exit;
    }

    /* read chip id */
    pid = i2c_smbus_read_byte_data(client, PART_ID_REG);

    if (pid == LTR301_PART_ID) {
        printk(KERN_INFO "LTR-301 Device detected!\n");
    } else{
        printk(KERN_INFO "LTR-301 Device not found!\n"
                "i2c error %d \n", pid);
        err = -ENODEV;
        goto kfree_exit;
    }
    i2c_set_clientdata(client, data);
    data->ltr301_client = client;
    mutex_init(&data->enable_mutex);

    INIT_DELAYED_WORK(&data->work, ltr301_work_func);
    atomic_set(&data->delay, MAX_DELAY);
    atomic_set(&data->enable, 0);

    dev = input_allocate_device();
    if (!dev)
        return -ENOMEM;
    dev->name = SENSOR_NAME;
    dev->id.bustype = BUS_I2C;

    input_set_capability(dev, EV_ABS, ABS_MISC);
    input_set_abs_params(dev, ABS_MISC, ABSMIN, ABSMAX, 0, 0);
    input_set_drvdata(dev, data);

    err = input_register_device(dev);
    if (err < 0) {
        input_free_device(dev);
        goto kfree_exit;
    }

    data->input = dev;

    err = sysfs_create_group(&data->input->dev.kobj,
            &ltr301_attribute_group);
    if (err < 0)
        goto error_sysfs;

#ifdef CONFIG_HAS_EARLYSUSPEND
    data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
    data->early_suspend.suspend = ltr301_early_suspend;
    data->early_suspend.resume = ltr301_early_resume;
    register_early_suspend(&data->early_suspend);
#endif

    ltr301_set_mode(client, LTR301_MODE_ACTIVE);    
    return 0;

error_sysfs:
    input_unregister_device(data->input);

kfree_exit:
    kfree(data);
exit:
    return err;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void ltr301_early_suspend(struct early_suspend *h)
{
    struct ltr301_data *data =
        container_of(h, struct ltr301_data, early_suspend);

    mutex_lock(&data->enable_mutex);
    if (atomic_read(&data->enable) == 1) {
        ltr301_set_mode(data->ltr301_client, LTR301_MODE_STANDBY);
        cancel_delayed_work_sync(&data->work);
    }
    mutex_unlock(&data->enable_mutex);
}


static void ltr301_early_resume(struct early_suspend *h)
{
    struct ltr301_data *data =
        container_of(h, struct ltr301_data, early_suspend);

    mutex_lock(&data->enable_mutex);
    if (atomic_read(&data->enable) == 1) {
        ltr301_set_mode(data->ltr301_client, LTR301_MODE_ACTIVE);
        schedule_delayed_work(&data->work,
                msecs_to_jiffies(atomic_read(&data->delay)));
    }
    mutex_unlock(&data->enable_mutex);
}
#endif

static int __devexit ltr301_remove(struct i2c_client *client)
{
    struct ltr301_data *data = i2c_get_clientdata(client);

    ltr301_set_enable(&client->dev, 0);
#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&data->early_suspend);
#endif
    sysfs_remove_group(&data->input->dev.kobj, &ltr301_attribute_group);
    input_unregister_device(data->input);
    kfree(data);

    return 0;
}

#ifdef CONFIG_PM

static int ltr301_suspend(struct device *dev)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ltr301_data *data = i2c_get_clientdata(client);

    mutex_lock(&data->enable_mutex);
    if (atomic_read(&data->enable) == 1) {
        ltr301_set_mode(data->ltr301_client, LTR301_MODE_STANDBY);
        cancel_delayed_work_sync(&data->work);
    }
    mutex_unlock(&data->enable_mutex);

    return 0;
}

static int ltr301_resume(struct device *dev)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ltr301_data *data = i2c_get_clientdata(client);

    mutex_lock(&data->enable_mutex);
    if (atomic_read(&data->enable) == 1) {
        ltr301_set_mode(data->ltr301_client, LTR301_MODE_ACTIVE);
        schedule_delayed_work(&data->work,
                msecs_to_jiffies(atomic_read(&data->delay)));
    }
    mutex_unlock(&data->enable_mutex);

    return 0;
}

#else

#define ltr301_suspend        NULL
#define ltr301_resume        NULL

#endif /* CONFIG_PM */

static SIMPLE_DEV_PM_OPS(ltr301_pm_ops, ltr301_suspend, ltr301_resume);

static const unsigned short  ltr301_addresses[] = {
    SENSOR_I2C_ADDR,
    I2C_CLIENT_END,
};

static const struct i2c_device_id ltr301_id[] = {
    { SENSOR_NAME, 0 },
    { }
};

MODULE_DEVICE_TABLE(i2c, ltr301_id);

static struct i2c_driver ltr301_driver = {
    .driver = {
        .owner    = THIS_MODULE,
        .name    = SENSOR_NAME,
        .pm    = &ltr301_pm_ops,
    },
    .class        = I2C_CLASS_HWMON,
//    .address_list    = ltr301_addresses,
    .id_table    = ltr301_id,
    .probe        = ltr301_probe,
    .remove        = __devexit_p(ltr301_remove),

};

static struct i2c_board_info ltr301_board_info={
    .type = SENSOR_NAME, 
    .addr = SENSOR_I2C_ADDR,
};

static struct i2c_client *ltr301_client;

static int __init ltr301_init(void)
{
    struct i2c_adapter *i2c_adap;
    unsigned int cfg_i2c_adap_id;

#if CFG_SENSOR_USE_CONFIG > 0
    int ret;
    
    /*get xml cfg*/
    ret = get_config(CFG_SENSOR_ADAP_ID, (char *)(&cfg_i2c_adap_id), sizeof(unsigned int));
    if (ret != 0) {
        printk(KERN_ERR"get i2c_adap_id %d fail\n", cfg_i2c_adap_id);
        return ret;
    }
#else
    cfg_i2c_adap_id = 2;
#endif
    
    i2c_adap = i2c_get_adapter(cfg_i2c_adap_id);  
    ltr301_client = i2c_new_device(i2c_adap, &ltr301_board_info);  
    i2c_put_adapter(i2c_adap);
    
    return i2c_add_driver(&ltr301_driver);
}

static void __exit ltr301_exit(void)
{
    i2c_unregister_device(ltr301_client);
    i2c_del_driver(&ltr301_driver);
}

MODULE_AUTHOR("Zhining Song <songzhining@actions-semi.com>");
MODULE_DESCRIPTION("LTR-301 light sensor driver");
MODULE_LICENSE("GPL");

module_init(ltr301_init);
module_exit(ltr301_exit);

