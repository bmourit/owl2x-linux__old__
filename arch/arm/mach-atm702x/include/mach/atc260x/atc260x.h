/**
 * Copyright 2011 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#ifndef __ATC260X_H__
#define __ATC260X_H__

#include <mach/atc260x/atc260x_reg.h>

#define PSY_NAME_WALL  "atc260x-wall"
#define PSY_NAME_USB   "atc260x-usb"
/* ATC260X device */
struct atc260x_dev;

/*
 * ATC260X register interface
 */
int atc260x_reg_read(struct atc260x_dev *atc260x, unsigned short reg);
int atc260x_reg_write(struct atc260x_dev *atc260x, unsigned short reg,
         unsigned short val);
int atc260x_set_bits(struct atc260x_dev *atc260x, unsigned short reg,
            unsigned short mask, unsigned short val);

/* 
 * ATC260X SPI interface access mode
 */
#define ATC260X_SPI_ACCESS_MODE_NORMAL      (0)     /* access by standard SPI driver interface */
#define ATC260X_SPI_ACCESS_MODE_DIRECT      (1)     /* access SPI controller directly */
int atc260x_set_access_mode(struct atc260x_dev *atc260x, int mode);

/*
 * ATC260X AuxADC interface
 */
enum atc260x_auxadc {
    ATC260X_AUX_IREF     = 0,
    ATC260X_AUX_CHGI     = 1,
    ATC260X_AUX_VBUSI    = 2,
    ATC260X_AUX_WALLI    = 3,
    ATC260X_AUX_BATI     = 4,
    ATC260X_AUX_REMCON   = 5,
    ATC260X_AUX_TEMP     = 6,
    ATC260X_AUX_BATV     = 7,
    ATC260X_AUX_BAKBATV  = 8,
    ATC260X_AUX_SYSPWRV  = 9,
    ATC260X_AUX_WALLV    = 10,
    ATC260X_AUX_VBUSV    = 11,
    ATC260X_AUX_AUX3     = 12,
    ATC260X_AUX_AUX2     = 13,
    ATC260X_AUX_AUX1     = 14,
    ATC260X_AUX_AUX0     = 15,
};

int atc260x_auxadc_reg_read(struct atc260x_dev *atc260x, enum atc260x_auxadc input);
int atc260x_auxadc_read(struct atc260x_dev *atc260x, enum atc260x_auxadc input);

/*
 *  ATC260X chip version
 */ 
enum ATC260X_CHIP_VERSION {
	ATC260X_VER_A = 0x0000,
	ATC260X_VER_B = 0x0001,
	ATC260X_VER_C = 0x0002,
};

int atc260x_get_version(void);

/*
 *  ATC260X CMU interface
 */ 
#define ATC260X_CMU_MODULE_NUM           (5) /* CMU module count */
#define ATC260X_CMU_MODULE_TP            (0)
#define ATC260X_CMU_MODULE_MFP           (1)
#define ATC260X_CMU_MODULE_INTS          (2)
#define ATC260X_CMU_MODULE_ETHPHY        (3)
#define ATC260X_CMU_MODULE_AUDIO         (4)

int atc260x_cmu_reset(struct atc260x_dev *atc260x, int cmu_module);
int atc260x_cmu_enable(struct atc260x_dev *atc260x, int cmu_module);
int atc260x_cmu_disable(struct atc260x_dev *atc260x, int cmu_module);


/*
 *  ATC260X MFD interface
 */ 
enum atc260x_mfp_mod_id { 
  MOD_ID_RMII, MOD_ID_SMII, MOD_ID_REMCON, MOD_ID_TP, 
  MOD_ID_LED0, MOD_ID_LED1, 
};

#define  ATC260X_MFP_OPT_CAN_SLEEP       (0) 
#define  ATC260X_MFP_OPT_CANNOT_SLEEP    (1) 

int atc260x_mfp_lock (enum atc260x_mfp_mod_id mod_id, int opt, struct device *dev);
int atc260x_mfp_locked (enum atc260x_mfp_mod_id mod_id, int opt);
int atc260x_mfp_unlock (enum atc260x_mfp_mod_id mod_id, int opt);

/*
 *  ATC260X IRQ interface
 */
#define ATC260X_IRQ_NUM                  (11)        /* internal IRQ source count */
#define ATC260X_IRQ_AUDIO                (0)
#define ATC260X_IRQ_TP                   (1)
#define ATC260X_IRQ_ETHERNET             (2)
#define ATC260X_IRQ_OV                   (3)
#define ATC260X_IRQ_OC                   (4)
#define ATC260X_IRQ_OT                   (5)
#define ATC260X_IRQ_UV                   (6)
#define ATC260X_IRQ_ALARM                (7)
#define ATC260X_IRQ_ONOFF                (8)
#define ATC260X_IRQ_WKUP                 (9)
#define ATC260X_IRQ_IR                   (10)

/*
 *  ATC260X internal GPIO count.
 */ 
#define ATC260X_GPIO_NUM                 (32)        /* internal GPIO count */


/*
 *  ATC260X Power Management
 */ 
/* power state mode */
#define POWER_MODE_WORKING              (0)     /* S1 */
#define POWER_MODE_STANDBY              (1)     /* S2 */
#define POWER_MODE_SLEEP                (2)     /* S3 */
#define POWER_MODE_DEEP_SLEEP           (3)     /* S4 */

/* wakeup sources */
#define WAKEUP_SRC_IR                   (1 << 0)
#define WAKEUP_SRC_RESET                (1 << 1)
#define WAKEUP_SRC_HDSW                 (1 << 2)
#define WAKEUP_SRC_ALARM                (1 << 3)
#define WAKEUP_SRC_REMCON               (1 << 4)
#define WAKEUP_SRC_TP                   (1 << 5)
#define WAKEUP_SRC_WKIRQ                (1 << 6)
#define WAKEUP_SRC_ONOFF_SHORT          (1 << 7)
#define WAKEUP_SRC_ONOFF_LONG           (1 << 8)
#define WAKEUP_SRC_WALL_IN              (1 << 9)
#define WAKEUP_SRC_VBUS_IN              (1 << 10)

#define WAKEUP_SRC_ALL                  (0x7ff)

struct pmic_suspend_ops {
	int (*set_wakeup_src)(int wakeup_mask, int wakeup_src);
    int (*get_wakeup_src)(void);
    int (*get_wakeup_flag)(void);       /* wakeup reason flag */
    void (*prepare)(int mode);
    void (*enter)(int mode);
    void (*wake)(void);
    void (*finish)(void);
};

/* gl5302_PMU_SYS_CTL3 FW reserved flag*/
#define PMU_SYS_CTL3_FW_FLAG_S2         (1 << 4)    /* suspend to S2 flag */

/*
 *  ATC260X misc interface
 */ 

int atc260x_enable_vbusotg(int on);

#endif /* __ATC260X_H__ */

