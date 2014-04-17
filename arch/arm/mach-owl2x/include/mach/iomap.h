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

#ifndef __OWL2X_HARDWARE_H
#define __OWL2X_HARDWARE_H

#include <mach/actions_reg_gl5202.h>

#define ASOC_SDRAM_BASE                 UL(0x00000000)
#define ASOC_IO_DEVICE_BASE             UL(0xB0000000)

#define OWL2X_IO_BASE                   0xF8000000      // VA of IO

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
#define IO_ADDRESS(x)       (((x) & 0x03ffffff) + IO_BASE)
#else
#define IO_ADDRESS(x)       (x)
#endif

#define __io_address(n)     __io(IO_ADDRESS(n))

/* Owl2x memory map */
#define OWL2X_BOOT_ROM_LOW              0x00000000
#define OWL2X_BOOT_ROM_HIGH             (0x02000000 + 0x10)
#define OWL2X_BOOT_ROM_BASE             OWL2X_BOOT_ROM_HIGH
#define OWL2X_BOOT_ROM_SIZE             SZ_512K

/* Peripheral physical addresses */
#define OWL2X_PA_REG_BASE               0xB0000000
#define OWL2X_PA_REG_SIZE               (6 * SZ_1M)

#define OWL2X_PA_PERIFBASE              0xB0020000
#define OWL2X_PA_PERIFBASE              SZ_8K

#define OWL2X_INT_DIST_BASE             0xB0021000      // Interrupt Distributor
#define OWL2X_INT_DIST_SIZE             SK_4K

#define OWL2X_CPU_INT_BASE              0xB0020100      // CPU interupt base
#define OWL2X_CPU_INT_SIZE              SZ_64

#define OWL2X_GP_INT_BASE               0xB0020200      // GP interrupt
#define OWL2X_GP_INT_SIZE               SZ_64

#define OWL2X_CACHECTL_BASE             0xB0022000
#define OWL2X_CACHECTL_SIZE             SZ_4K



#define OWL2X_PA_SCU                    0xB0020000 	// SCU control reg
#define OWL2X_CONFIG_SCU		0xB0020004 	// SCU config
#define OWL2X_SCU_CPU_PSTAT		0xB0020008 	// SCU CPU power status
#define OWL2X_SCU_INVALID		0xB002000C 	// SCU invalidate secure state
#define OWL2X_SCU_FILTERSTART		0xB0020040 	// SCU filter start
#define OWL2X_SCU_FILTEREND		0xB0020044 	// SCU filter end
#define OWL2X_SCU_ACCESSCTL		0xB0020050 	// SCU access control
#define OWL2X_SCU_SECURECTL		0xB0020050 	// SCU secure control



//#define OWL2X_PA_GIC_CPU                (0xB0020100)
//#define OWL2X_PA_GIC_GP                 (0xB0020200)
#define OWL2X_PA_TWD                    (0xB0020600)
//#define OWL2X_PA_GIC_DIST               (0xB0021000)
//#define OWL2X_IO_COREPERI_SIZE          (SZ_8K)
//#define OWL2X_PA_L2CC                   (0xB0022000)
//#define OWL2X_IO_L2CC_SIZE              (SZ_4K)

#define OWL2X_SDRAM_BASE                0x00000000
#define OWL2X_SDRAM_ALIAS_BASE          0xB0000000

/* Interrupt Distribution offsets from 0xB0020000 */
#define OWL2x_INT_DIST_TYPE		0x1004	//INTERRUPT TYPE
#define OWL2x_INT_DIST_ID		0x1008 	//INTERRUPT ID
#define OWL2x_INT_DIST_SEC		0x1080  //SECURE
#define OWL2x_INT_DIST_SET_EN		0x1100 	//SET ENABLE
#define OWL2x_INT_DIST_CLR_EN		0x1180 	//CLEAR ENABLE
#define OWL2x_INT_DIST_PDSET_EN		0x1200 	//PENDINF SET ENABLE
#define OWL2x_INT_DIST_PDCLR_EN		0x1280 	//PENDING CLEAR ENABLE
#define OWL2x_INT_DIST_ASTAT		0x1300	//ACTIVE STATUS
#define OWL2x_INT_DIST_PLEVEL		0x1400 	//PRIORITY LEVEL
#define OWL2x_INT_DIST_SPI		0x1800 	//SPI TARGET
#define OWL2x_INT_DIST_CONF		0x1C00 	//INTERRUPT CONFIG
#define OWL2x_INT_DIST_PPI_STAT		0x1D00 	//PPI - PRIVATE PERIPH INTERRUPT STATUS
#define OWL2x_INT_DIST_SPI_STAT		0x1D04 	//SPI - SHARED PERIPH INTERRUPT STATUS
#define OWL2x_INT_DIST_SGI		0x1F00 	//SGI - SOFTWARE GENERATED INTERRUPT
#define OWL2x_INT_DIST_PID		0x1FD0 	//PERIPH ID
#define OWL2x_INT_DIST_CID		0x1FF0 	//COMPONENT ID

#endif
