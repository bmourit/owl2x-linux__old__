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

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/serial_8250.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/irqdomain.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_fdt.h>
#include <linux/of_platform.h>
#include <linux/pda_power.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/sys_soc.h>
#include <linux/usb/owl2x_usb_phy.h>
#include <linux/clk/owl2x.h>
#include <linux/irqchip.h>

#include <asm/hardware/cache-l2x0.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/setup.h>
#include <asm/trusted_foundations.h>

#include "apbio.h"
#include "board.h"
#include "common.h"
#include "cpuidle.h"
#include "fuse.h"
#include "iomap.h"
#include "irq.h"
#include "pmc.h"
#include "pm.h"
#include "reset.h"
#include "sleep.h"

/*
 * Storage for debug-macro.S's state.
 *
 * This must be in .data not .bss so that it gets initialized each time the
 * kernel is loaded. The data is declared here rather than debug-macro.S so
 * that multiple inclusions of debug-macro.S point at the same data.
 */
u32 owl2x_uart_config[3] = {
        /* Debug UART initialization required */
        1,
        /* Debug UART physical address */
        0,
        /* Debug UART virtual address */
        0,
};

static void __init owl2x_init_cache(void)
{
#ifdef CONFIG_CACHE_L2X0
        static const struct of_device_id pl310_ids[] __initconst = {
                { .compatible = "arm,pl310-cache",  },
                {}
        };

        struct device_node *np;
        int ret;
        void __iomem *p = IO_ADDRESS(OWL2X_ARM_PERIF_BASE) + 0x2000;
        u32 aux_ctrl, cache_type;

        np = of_find_matching_node(NULL, pl310_ids);
        if (!np)
                return;

        cache_type = readl(p + L2X0_CACHE_TYPE);
        aux_ctrl = (cache_type & 0x700) << (17-8);
        aux_ctrl |= 0x7C400001;

        ret = l2x0_of_init(aux_ctrl, 0x8200c3fe);
        if (!ret)
                l2x0_saved_regs_addr = virt_to_phys(&l2x0_saved_regs);
#endif
}

static void __init owl2x_init_early(void)
{
        of_register_trusted_foundations();
        owl2x_apb_io_init();
        owl2x_init_fuse();
        owl2x_cpu_reset_handler_init();
        owl2x_init_cache();
        owl2x_powergate_init();
        owl2x_hotplug_init();
}

static void __init owl2x_dt_init_irq(void)
{
        owl2x_pmc_init_irq();
        owl2x_init_irq();
        irqchip_init();
        owl2x_legacy_irq_syscore_init();
}

static void __init owl2x_dt_init(void)
{
        struct soc_device_attribute *soc_dev_attr;
        struct soc_device *soc_dev;
        struct device *parent = NULL;

        owl2x_pmc_init();

        owl2x_clocks_apply_init_table();

        soc_dev_attr = kzalloc(sizeof(*soc_dev_attr), GFP_KERNEL);
        if (!soc_dev_attr)
                goto out;

        soc_dev_attr->family = kasprintf(GFP_KERNEL, "Owl2x");
        soc_dev_attr->revision = kasprintf(GFP_KERNEL, "%d", owl2x_revision);
        soc_dev_attr->soc_id = kasprintf(GFP_KERNEL, "%d", owl2x_chip_id);

        soc_dev = soc_device_register(soc_dev_attr);
        if (IS_ERR(soc_dev)) {
                kfree(soc_dev_attr->family);
                kfree(soc_dev_attr->revision);
                kfree(soc_dev_attr->soc_id);
                kfree(soc_dev_attr);
                goto out;
        }

        parent = soc_device_to_device(soc_dev);

        /*
         * Finished with the static registrations now; fill in the missing
         * devices
         */
out:
        of_platform_populate(NULL, of_default_bus_match_table, NULL, parent);
}

static void __init paz00_init(void)
{
        if (IS_ENABLED(CONFIG_ARCH_OWL2X_2x_SOC))
                owl2x_paz00_wifikill_init();
}

static struct {
        char *machine;
        void (*init)(void);
} board_init_funcs[] = {
        { "compal,paz00", paz00_init },
};

static void __init owl2x_dt_init_late(void)
{
        int i;

        owl2x_init_suspend();
        owl2x_cpuidle_init();
        owl2x_powergate_debugfs_init();

        for (i = 0; i < ARRAY_SIZE(board_init_funcs); i++) {
                if (of_machine_is_compatible(board_init_funcs[i].machine)) {
                        board_init_funcs[i].init();
                        break;
                }
        }
}

static const char * const owl2x_dt_board_compat[] = {
        "actions,owl2x",
        "actions,owl29",
        "actions,owl29b",
        NULL
};

DT_MACHINE_START(OWL2X_DT, "Actions Owl2x SoC (Flattened Device Tree)")
        .map_io         = owl2x_map_common_io,
        .smp            = smp_ops(owl2x_smp_ops),
        .init_early     = owl2x_init_early,
        .init_irq       = owl2x_dt_init_irq,
        .init_machine   = owl2x_dt_init,
        .init_late      = owl2x_dt_init_late,
        .restart        = owl2x_pmc_restart,
        .dt_compat      = owl2x_dt_board_compat,
MACHINE_END
