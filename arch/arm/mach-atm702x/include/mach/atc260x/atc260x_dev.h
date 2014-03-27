/*
 * atc260x_core.h    ATC260X core internal header
 *
 * Copyright 2011 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */
#ifndef __ATC260X_DEV_H__
#define __ATC260X_DEV_H__

#include <linux/completion.h>
#include <linux/interrupt.h>

#include <mach/atc260x/atc260x.h>

/* for check atc260x is exist */
#define ATC260X_MAGICNUM_REG         (gl5302_PMU_OC_INT_EN)
#define ATC260X_MAGICNUM_REG_VALUE   (0x1bc0)

/* ATC260X device */
struct atc260x_dev {
    struct mutex io_lock;       /* atc260x device access mutex */

    struct device *dev;         

    int (*read_dev)(struct atc260x_dev *atc260x, unsigned short reg,
            int bytes, void *dest);
    int (*write_dev)(struct atc260x_dev *atc260x, unsigned short reg,
             int bytes, void *src);

    /* set spi access mode */
    int (*set_access_mode)(struct atc260x_dev *atc260x, int mode);

    void *control_data;

    struct mutex auxadc_lock;

    int irq;  /* Our chip IRQ */
    struct mutex irq_lock;
    unsigned int irq_base;  /* virtual IRQ base for internal device */
    unsigned int irq_masks_cur;   /* Currently active value */
    unsigned int irq_masks_cache; /* Cached hardware value */
};


int atc260x_device_init(struct atc260x_dev *atc260x, int irq);
void atc260x_device_exit(struct atc260x_dev *atc260x);
int atc260x_device_suspend(struct atc260x_dev *atc260x);
int atc260x_device_resume(struct atc260x_dev *atc260x);

int atc260x_irq_init(struct atc260x_dev *atc260x, int irq);
void atc260x_irq_exit(struct atc260x_dev *atc260x);

int atc260x_mfp_init(struct atc260x_dev *atc260x);
void atc260x_mfp_exit(struct atc260x_dev *atc260x);

extern struct atc260x_dev *atc260x_dev_handle;

#endif /* __ATC260X_DEV_H__ */

