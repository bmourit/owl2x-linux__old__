/*
 * arch/arm/mach-atm702x/include/mach/smp.h
 *
 * SMP definitions
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef ARCH_ARM_MACH_ATM702X_COMMON_H
#define ARCH_ARM_MACH_ATM702X_COMMON_H
#ifndef __ASSEBLER__

#include <linux/irq.h>
#include <linux/i2c.h>
#include <linux/i2c/twl.h>
#include <linux/i2c-atm702x.h>
#include <linux/delay.h>

#include <asm/proc-fns.h>

#include "i2c.h"
#include "serial.h"
#include "usb.h"

/* This is required to wakeup the secondary core */
extern void atm702x_secondary_startup(void);
extern void atm702x_secondary_init(unsigned int cpu);

#endif
