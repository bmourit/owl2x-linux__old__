/*
 * arch/arm/mach-atm702x/include/mach/io.h
 *
 * IO definitions
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __ASM_ARCH_IO_H
#define __ASM_ARCH_IO_H

#define IO_SPACE_LIMIT 0xffffffff

/*
 * We don't actually have real ISA nor PCI buses, but there is so many
 * drivers out there that might just work if we fake them...
 */
#define __io(a)     __typesafe_io(a)
#define __mem_pci(a)    (a)

#define __arch_ioremap __arm_ioremap
#define __arch_iounmap __iounmap

#endif /* __ASM_ARCH_IO_H */
