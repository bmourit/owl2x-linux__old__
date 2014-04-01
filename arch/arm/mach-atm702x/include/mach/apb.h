/*
 * linux/arch/arm/mach-atm702x/include/mach/regs-apb.h
 *
 * APB Clock Unit
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_MACH_REGS_APB_H
#define __ASM_MACH_REGS_APB_H

#include <mach/addr-map.h>

/* Common APB clock register bit definitions */
#define APB_APBCLK	(1 << 0)  /* APB Bus Clock Enable */
#define APB_FNCLK	(1 << 1)  /* Functional Clock Enable */
#define APB_RST	(1 << 2)  /* Reset Generation */

/* Functional Clock Selection Mask */
#define APB_FNCLKSEL(x)	(((x) & 0xf) << 4)

#endif /* __ASM_MACH_REGS_APB_H */
