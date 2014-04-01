/*
 * Flattened device tree for ATM7029
 *
 * Copyright (C) 2014
 * Author: B. Mouritsen <bnmguy@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/of_platform.h>
#include <linux/of_fdt.h>

#include <asm/mach/arch.h>

#include "common.h"

static void __init atm7029_dt_machine_init(void)
{
        atm702x_cpuidle_init();
        atm702x_cpufreq_init();

        of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);
}

static char const *atm7029_dt_compat[] __initdata = {
        "actions,atm7029",
        NULL
};

static void __init atm7029_reserve(void)
{
}

DT_MACHINE_START(ATM7029_DT, "Actions ATM7029 (Flattened Device Tree)")
        .smp            = smp_ops(atm702x_smp_ops),
        .map_io         = atm702x_init_io,
        .init_early     = atm702x_firmware_init,
        .init_machine   = atm7029_dt_machine_init,
        .init_late      = atm702x_init_late,
        .dt_compat      = atm7029_dt_compat,
        .restart        = atm7029_restart,
        .reserve        = atm7029_reserve,
MACHINE_END
