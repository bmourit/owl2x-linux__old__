/*
 * arch/arm/mach-atm702x/include/mach/powergate.h
 *
 * powergate definitions
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __ASM_ARCH_POWERGATE_H
#define __ASM_ARCH_POWERGATE_H

enum atm702x_powergate_id {
    ATM702X_POWERGATE_CPU1 = 0,
    ATM702X_POWERGATE_CPU23,
    ATM702X_POWERGATE_GPU3D,
    ATM702X_POWERGATE_GPS,
    ATM702X_POWERGATE_VCE_BISP,
    ATM702X_POWERGATE_VDE,
    ATM702X_POWERGATE_L2,
    ATM702X_POWERGATE_GPU2D,
    ATM702X_POWERGATE_MAXID,
};

int  __init atm702x_powergate_init(void);

int atm702x_cpu_powergate_id(int cpuid);
int atm702x_powergate_is_powered(enum atm702x_powergate_id id);
int atm702x_powergate_power_on(enum atm702x_powergate_id id);
int atm702x_powergate_power_off(enum atm702x_powergate_id id);
int atm702x_powergate_power_on_l2(void);

#endif /* __ASM_ARCH_POWERGATE_H */
