/*
 * Copyright (C) 2014
 * Author: B. Mouritsen <bnmguy@gmail.com>
 *
 * Common Header for ATM702x machines
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ARCH_ARM_MACH_ATM702X_COMMON_H
#define __ARCH_ARM_MACH_ATM702X_COMMON_H

#include <linux/reboot.h>
#include <linux/of.h>

void mct_init(void __iomem *base, int irq_g0, int irq_l0, int irq_l1);

struct map_desc;
void atm702x_init_io(void);
void atm702x4_restart(enum reboot_mode mode, const char *cmd);
void atm702x5_restart(enum reboot_mode mode, const char *cmd);
void atm702x_cpuidle_init(void);
void atm702x_cpufreq_init(void);
void atm702x_init_late(void);

void atm702x_firmware_init(void);

extern struct smp_operations atm702x_smp_ops;

extern void atm702x_cpu_die(unsigned int cpu);

/* PMU(Power Management Unit) support */

#define PMU_TABLE_END	NULL

enum sys_powerdown {
	SYS_AFTR,
	SYS_LPA,
	SYS_SLEEP,
	NUM_SYS_POWERDOWN,
};

extern unsigned long l2x0_regs_phys;
struct atm702x_pmu_conf {
	void __iomem *reg;
	unsigned int val[NUM_SYS_POWERDOWN];
};

extern void atm702x_sys_powerdown_conf(enum sys_powerdown mode);

#endif /* __ARCH_ARM_MACH_atm702x_COMMON_H */
