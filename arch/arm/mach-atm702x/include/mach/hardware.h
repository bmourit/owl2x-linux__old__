/*
 * arch/arm/mach-gl5202/include/mach/hardware.h
 *
 * hardware support
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include <mach/actions_reg_gl5202.h>

#define ASOC_SDRAM_BASE             UL(0x00000000)
#define ASOC_IO_DEVICE_BASE         UL(0xB0000000)

#define ASOC_IO_ADDR_BASE           0xF8000000

/* macro to get at IO space when running virtually */
#ifdef CONFIG_MMU
/*
 * Statically mapped addresses:
 *
 * b0xx xxxx -> f8xx xxxx
 * b1xx xxxx -> f9xx xxxx
 * b2xx xxxx -> faxx xxxx
 * b3xx xxxx -> fbxx xxxx
 */
#define IO_ADDRESS(x)       (ASOC_IO_ADDR_BASE + ((x) & 0x03ffffff))
#else
#define IO_ADDRESS(x)       (x)
#endif

#define __io_address(n)     __io(IO_ADDRESS(n))

#ifndef __ASSEMBLY__
/******************************************************************************/
static void inline act_writeb(u8 val, u32 reg)
{
    *(volatile u8 *)(IO_ADDRESS(reg)) = val;
}

static void inline act_writew(u16 val, u32 reg)
{
    *(volatile u16 *)(IO_ADDRESS(reg)) = val;
}

static void inline act_writel(u32 val, u32 reg)
{
    *(volatile u32 *)(IO_ADDRESS(reg)) = val;
}
/******************************************************************************/
static inline u8 act_readb(u32 reg)
{
    return (*(volatile u8 *)IO_ADDRESS(reg));
}

static inline u16 act_readw(u32 reg)
{
    return (*(volatile u16 *)IO_ADDRESS(reg));
}

static inline u32 act_readl(u32 reg)
{
    return (*(volatile u32 *)IO_ADDRESS(reg));
}
/******************************************************************************/
static void inline act_setb(u8 val,u32 reg)
{
    *(volatile u8 *)IO_ADDRESS(reg) |= val;
}

static void inline act_setw(u16 val,u32 reg)
{
    *(volatile u16 *)IO_ADDRESS(reg) |= val;
}

static void inline act_setl(u32 val,u32 reg)
{
    *(volatile u32 *)IO_ADDRESS(reg) |= val;
}
/******************************************************************************/
static void inline act_clearb(u8 val,u32 reg)
{
    *(volatile u8 *)IO_ADDRESS(reg) &= ~val;
}

static void inline act_clearw(u16 val,u32 reg)
{
    *(volatile u16 *)IO_ADDRESS(reg) &= ~val;
}

static void inline act_clearl(u32 val,u32 reg)
{
    *(volatile u32 *)IO_ADDRESS(reg) &= ~val;
}
/******************************************************************************/

extern int asoc_run_config(void);

extern int asoc_get_board_opt(void);
extern int asoc_get_board_opt_flags(void);

extern int set_hdmi_status(unsigned int on);
extern int set_safe_status(unsigned int on);

extern unsigned int asoc_get_ion_size(void);

#endif

#endif /* __ASM_ARCH_HARDWARE_H */
