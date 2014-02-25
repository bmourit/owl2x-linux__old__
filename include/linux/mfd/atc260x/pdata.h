/*
 * include/linux/mfd/atc260x/pdata.h -- Platform data for ATC260X
 *
 * Copyright 2009 Wolfson Microelectronics PLC.
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#ifndef __MFD_ATC260X_PDATA_H__
#define __MFD_ATC360x_PDATA_H__

struct atc260x;
struct regulator_init_data;

struct power_supply_info;

struct atc260x_backlight_pdata {
	int isink;		/* ISINK to use, 1 or 2 */
	int max_uA;		/* Maximum current to allow */
};

struct atc260x_backup_pdata {
	int charger_enable;
	int charge_current;		/* 1mA, 5mA, 10mA, 25mA, 50mA */
	/* voltage thresholds (in millivolts) */
	int vbat_charge_start;
	int vbat_charge_stop;		/* 3000mV or 4200mV */
	/* battery monitor interval (seconds) */
	unsigned int batmon_interval;
};

struct atc260x_battery_pdata {
	int vbat_charge_start; 	/* voltage thresholds (in millivolts) */
	int vbat_charge_stop;
	int vbat_low;
	int vbat_crit;
	/* current thresholds (in milliampere ) */
	int trickle_charge_current;	/* 50mA ~ 300mA*/
	int constant_charge_current;	/* 50mA ~ 1500mA */
	/* battery monitor interval (seconds) */
	unsigned int batmon_interval;
	/* platform callbacks for battery low and critical events */
	void (*battery_low)(void);
	void (*battery_critical)(void);
};

/**
 * Since most all devices using this PMIC have their own Capacitive Touch device,
 * we can use this as an ADC instead. Only use this as TP is you need touch support
 * and your device doesn't provide its own.
 */
struct atc260x_touch_pdata { 
};

struct atc260x_rtc_pdata {
};

struct atc260x_ethphy_pdata {
};

struct atc260x_codec_pdata {
};

#define ATC260X_MAX_DCDC		4
#define ATC260X_MAX_LDO			12
#define ATC260X_MAX_ISNK		1
#define ATC260X_MAX_SWITCH_LDO		2

//#define ATC260X_GPIO_NUM		32	/* internal GPIO count */


struct atc260x_pdata {
	/** Called before subdevices are set up */
	int (*pre_init)(struct atc260x *atc260x);
	/** Called after subdevices are set up */
	int (*post_init)(struct atc260x *atc260x);

	int irq_base;
	int gpio_base;
	struct atc260x_backlight_pdata *backlight;
	struct atc260x_backup_pdata *backup;
	struct atc260x_battery_pdata *battery;
	struct atc260x_touch_pdata *touch;

	struct atc260x_rtc_pdata *rtc;
	struct atc260x_codec_pdata *codec;	
	struct atc260x_ethphy_pdata *ethphy;

	/** DCDC1 = 0 and so on */
	struct regulator_init_data *dcdc[ATC260X_MAX_DCDC];
	/** LDO1 = 0 and so on */
	struct regulator_init_data *ldo[ATC260X_MAX_LDO];
	/** ISINK1 = 0 and so on*/
	struct regulator_init_data *isink[ATC260X_MAX_ISNK];
	/** Switch LDO = 0 and so on */
	struct regulator_init_data *switch_ldo[ATC260X_MAX_SWITCH_LDO];
};

#endif
