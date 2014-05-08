/*
 * Hardware monitoring features for ATC260x PMIC
 *
 * Copyright (C) 2014
 * Author: B. Mouritsen <bnmguy@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>

#include <linux/mfd/atc260x/core.h>
#include <linux/mfd/atc260x/auxadc.h>

struct atc260x_hwmon {
	struct device *classdev;
	struct atc260x *atc260x;
};

static int show_value(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct atc260x_hwmon *hwmon = dev_get_drvdata(dev);
	int channel = to_sensor_dev_attr(attr)->index;
	int val;

	val = atc260x_auxadc_reg_read(hwmon->atc260x, channel);
	if (val < 0)
		return val;

	return sprintf(buf, "%d\n", DIV_ROUND_CLOSEST(val, 1000));
}

static ssize_t show_name(struct device *dev, 
		struct device_attribute *attr, char *buf)
{	
	return sprintf(buf, "atc260x\n");
}

static const char * const input_names[] = {
	[ATC260X_AUX_SYSPWRV]	= "system_power",
	[ATC260X_AUX_VBUSI]	= "USB_current",
	[ATC260X_AUX_VBUSV] 	= "USB_voltage",
	[ATC260X_AUX_BAKBATV] 	= "backup_battery",
	[ATC260X_AUX_BATI] 	= "battery_current",
	[ATC260X_AUX_BATV] 	= "battery_voltage",
	[ATC260X_AUX_WALLI] 	= "wall_current",
	[ATC260X_AUX_WALLV]	= "wall_voltage",
	[ATC260X_AUX_CHGI]	= "charge_current",
	[ATC260X_AUX_SVCC]	= "ref_current",
	[ATC260X_AUX_ICTEMP]	= "chip_temperature",
	[ATC260X_AUX_REMCON]	= "remote_control",
};

static ssize_t show_chip_temp(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct atc260x_hwmon *hwmon = dev_get_drvdata(dev);
	int channel = to_sensor_dev_attr(attr)->index;
	int temp;

	temp = atc260x_auxadc_read(hwmon->atc260x, channel);
	if (temp < 0)
		return temp;

	/* Degrees celsius = (512.18 - temp) / 1.0983 */
	temp = 512180 - (temp * 1000);
	temp = DIV_ROUND_CLOSEST(temp * 10000, 10983);

	return sprintf(buf, "%d\n", temp);
}

static ssize_t show_label(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int channel = to_sensor_dev_attr(attr)->index;

	return sprintf(buf, "%s\n", input_names[channel]);
}

#define ATC260X_VALUE(id, name) \
	static SENSOR_DEVICE_ATTR(in##id##_input, S_IRUGO, show_value, \
		NULL, name)

#define ATC260X_NAMED_VALUE(id, name) \
	ATC260X_VALUE(id, name); \
	static SENSOR_DEVICE_ATTR(in##id##_label, S_IRUGO, show_value, \
		NULL, name)

static DEVICE_ATTR(name, S_IRUGO, show_name, NULL);

ATC260X_VALUE(0, ATC260X_AUX_AUX0);
ATC260X_VALUE(1, ATC260X_AUX_AUX1);
ATC260X_VALUE(2, ATC260X_AUX_AUX2);
ATC260X_VALUE(3, ATC260X_AUX_AUX3);

ATC260X_NAMED_VALUE(4, ATC260X_AUX_SYSPWRV);
ATC260X_NAMED_VALUE(5, ATC260X_AUX_VBUSV);
ATC260X_NAMED_VALUE(6, ATC260X_AUX_VBUSI);
ATC260X_NAMED_VALUE(7, ATC260X_AUX_BATV);
ATC260X_NAMED_VALUE(8, ATC260X_AUX_BATI);
ATC260X_NAMED_VALUE(9, ATC260X_AUX_WALLV);
ATC260X_NAMED_VALUE(10, ATC260X_AUX_WALLI);
ATC260X_NAMED_VALUE(11, ATC260X_AUX_CHGI);
ATC260X_NAMED_VALUE(12, ATC260X_AUX_BAKBATV);
ATC260X_NAMED_VALUE(13, ATC260X_AUX_REMCON);
ATC260X_NAMED_VALUE(14, ATC260X_AUX_SVCC);

static SENSOR_DEVICE_ATTR(temp1_input, S_IRUGO, show_chip_temp, NULL,
		ATC260X_AUX_ICTEMP);
static SENSOR_DEVICE_ATTR(temp1_label, S_IRUGO, show_label, NULL,
		ATC260X_AUX_ICTEMP);

#static struct attribute *atc260x_attributes[] = {
	&dev_attr_name.attr,

	&sensor_dev_attr_in0_input.dev_attr.attr,
	&sensor_dev_attr_in1_input.dev_attr.attr,
	&sensor_dev_attr_in2_input.dev_attr.attr,
	&sensor_dev_attr_in3_input.dev_attr.attr,

	&sensor_dev_attr_in4_input.dev_attr.attr,
	&sensor_dev_attr_in4_label.dev_attr.attr,
	&sensor_dev_attr_in5_input.dev_attr.attr,
	&sensor_dev_attr_in5_label.dev_attr.attr,
	&sensor_dev_attr_in6_input.dev_attr.attr,
	&sensor_dev_attr_in6_label.dev_attr.attr,
	&sensor_dev_attr_in7_input.dev_attr.attr,
	&sensor_dev_attr_in7_label.dev_attr.attr,
	&sensor_dev_attr_in8_input.dev_attr.attr,
	&sensor_dev_attr_in8_label.dev_attr.attr,
	&sensor_dev_attr_in9_input.dev_attr.attr,
	&sensor_dev_attr_in9_label.dev_attr.attr,
	&sensor_dev_attr_in10_input.dev_attr.attr,
	&sensor_dev_attr_in10_label.dev_attr.attr,
	&sensor_dev_attr_in11_input.dev_attr.attr,
	&sensor_dev_attr_in11_label.dev_attr.attr,
	&sensor_dev_attr_in12_input.dev_attr.attr,
	&sensor_dev_attr_in12_label.dev_attr.attr,
	&sensor_dev_attr_in13_input.dev_attr.attr,
	&sensor_dev_attr_in13_label.dev_attr.attr,
	&sensor_dev_attr_in14_input.dev_attr.attr,
	&sensor_dev_attr_in14_label.dev_attr.attr,

	&sensor_dev_attr_temp1_input.dev_attr.attr,
	&sensor_dev_attr_temp1_label.dev_attr.attr,

	NULL,
};

static const struct attribute_group atc260x_attr_group = {
	{ .attrs 	= atc260x_attributes },
	{ }
};

static int atc260x_hwmon_probe(struct platform_device *pdev)
{
	struct atc260x *atc260x = dev_get_drvdata(pdev->dev.parent);
	struct atc260x_hwmon *hwmon;
	int ret;

	hwmon = devm_kzalloc(&pdev->dev, sizeof(struct atc260x_hwmon), GFP_KERNEL);
	if (!hwmon)
		return -ENOMEM;

	hwmon->atc260x = atc260x;

	ret = sysfs_create_group(&pdev->dev.kobj, &atc260x_attr_group);
	if (ret)
		return ret;

	hwmon->classdev = hwmon_device_register(&pdev->dev);
	if (IS_ERR(hwmon->classdev)) {
		ret = PTR_ERR(hwmon->classdev);
		goto err_sysfs;
	}

	platform_set_drvdata(pdev, hwmon);

	return 0;

err_sysfs:
	sysfs_remove_group(&pdev->dev.kobj, &atc260x_attr_group);
	return ret;
}

static int atc260x_hwmon_remove(struct platform_device *pdev)
{
	struct atc260x_hwmon *hwmon = platform_get_drvdata(pdev);

	hwmod_device_unregister(hwmon->classdev);
	sysfs_remove_group(&pdev->dev.kobj, &atc260x_attr_group);

	return 0;
}


static struct platform_driver atc260x_hwmon_driver = {
	.probe		= atc260x_hwmon_probe,
	.remove		= atc260x_hwmon_remove,
	.driver {
		.name	= "atc260x-hwmon",
		.owner	= THIS_MODULE,
	};
};

module_platform_driver(atc260x_hwmon_driver);

MODULE_DESCRIPTION("ATC260x Hardware Monitoring");
MODULE_AUTHOR("B. Mouritsen <bnmguy@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:atc260x-hwmon");
