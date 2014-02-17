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

#include <linux/init.h>
#include <linux/device.h>
#include <linux/smp.h>
#include <linux/io.h>
#include <linux/irqchip/arm-gic.h>

#include <asm/smp_scu.h>

#include "omap-secure.h"
#include "omap-wakeupgen.h"
#include <asm/cputype.h>

#include "soc.h"
#include "iomap.h"
#include "common.h"
#include "clockdomain.h"
#include "pm.h"

#include <asm/hardware/gic.h>

/* This is required to wakeup the secondary core */
extern void atm702x_secondary_startup(void);

#define hard_smp_processor_id()             \
    ({                      \
        unsigned int cpunum;            \
        __asm__("mrc p15, 0, %0, c0, c0, 5" \
            : "=r" (cpunum));       \
        cpunum &= 0x0F;             \
    })

/*
 * We use IRQ1 as the IPI
 */
static inline void smp_cross_call(const struct cpumask *mask)
{
    gic_raise_softirq(mask, 1);
}

#endif /* ASMARM_ARCH_SMP_H */
