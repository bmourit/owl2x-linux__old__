/*
 * arch/arm/mach-atm702x/include/mach/irqs.h
 *
 * IRQ definitions
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __ASM_ARCH_IRQS_H
#define __ASM_ARCH_IRQS_H

#define IRQ_LOCALTIMER			(29)
#define IRQ_LOCALWDT			(30)

#define ATM702X_IRQ_OFFSET		(32)
#define ATM702X_IRQ(x)			((x) + ATM702X_IRQ_OFFSET)

#define IRQ_ATM702X_AUIP               ATM702X_IRQ(0)     /* reserved */
#define IRQ_ATM702X_ETHERNET           ATM702X_IRQ(1)
#define IRQ_ATM702X_DE                 ATM702X_IRQ(2)
#define IRQ_ATM702X_GPU_2D             ATM702X_IRQ(3)
#define IRQ_ATM702X_GPU_3D             ATM702X_IRQ(4)
#define IRQ_ATM702X_VIN                ATM702X_IRQ(5)
#define IRQ_ATM702X_PC0                ATM702X_IRQ(6)
#define IRQ_ATM702X_PC1                ATM702X_IRQ(7)
#define IRQ_ATM702X_PC2                ATM702X_IRQ(8)
#define IRQ_ATM702X_2HZ0               ATM702X_IRQ(9)
#define IRQ_ATM702X_2HZ1               ATM702X_IRQ(10)
#define IRQ_ATM702X_TIMER0             ATM702X_IRQ(11)
#define IRQ_ATM702X_TIMER1             ATM702X_IRQ(12)
#define IRQ_ATM702X_BISP               ATM702X_IRQ(13)
#define IRQ_ATM702X_SIRQ0              ATM702X_IRQ(14)
#define IRQ_ATM702X_DMA                ATM702X_IRQ(15)
#define IRQ_ATM702X_KEY                ATM702X_IRQ(16)
#define IRQ_ATM702X_SIRQ1              ATM702X_IRQ(17)
#define IRQ_ATM702X_PCM0               ATM702X_IRQ(18)
#define IRQ_ATM702X_PCM1               ATM702X_IRQ(19)
#define IRQ_ATM702X_SPI0               ATM702X_IRQ(20)
#define IRQ_ATM702X_SPI1               ATM702X_IRQ(21)
#define IRQ_ATM702X_SPI2               ATM702X_IRQ(22)
#define IRQ_ATM702X_SPI3               ATM702X_IRQ(23)
#define IRQ_ATM702X_USB                ATM702X_IRQ(24)
#define IRQ_ATM702X_I2C0               ATM702X_IRQ(25)
#define IRQ_ATM702X_I2C1               ATM702X_IRQ(26)
#define IRQ_ATM702X_I2C2               ATM702X_IRQ(27)
#define IRQ_ATM702X_UART0              ATM702X_IRQ(28)
#define IRQ_ATM702X_UART1              ATM702X_IRQ(29)
#define IRQ_ATM702X_UART2              ATM702X_IRQ(30)
#define IRQ_ATM702X_UART3              ATM702X_IRQ(31)
#define IRQ_ATM702X_UART4              ATM702X_IRQ(32)
#define IRQ_ATM702X_UART5              ATM702X_IRQ(33)
#define IRQ_ATM702X_GPIO               ATM702X_IRQ(34)
#define IRQ_ATM702X_USB2_0             ATM702X_IRQ(35)
#define IRQ_ATM702X_USB2_1             ATM702X_IRQ(36)
#define IRQ_ATM702X_SIRQ2              ATM702X_IRQ(37)
#define IRQ_ATM702X_DCU_DEBUG          ATM702X_IRQ(38)
#define IRQ_ATM702X_NAND               ATM702X_IRQ(39)
#define IRQ_ATM702X_SD0                ATM702X_IRQ(40)
#define IRQ_ATM702X_SD1                ATM702X_IRQ(41)
#define IRQ_ATM702X_SD2                ATM702X_IRQ(42)
#define IRQ_ATM702X_LCD                ATM702X_IRQ(43)
#define IRQ_ATM702X_HDMI               ATM702X_IRQ(44)
#define IRQ_ATM702X_TVOUT              ATM702X_IRQ(45)
#define IRQ_ATM702X_AUDIO_INOUT        ATM702X_IRQ(46)
#define IRQ_ATM702X_H264_JPEG_ENCODER  ATM702X_IRQ(47)
#define IRQ_ATM702X_HIVDE_DECODER      ATM702X_IRQ(48)
#define IRQ_ATM702X_DSI                ATM702X_IRQ(49)    /* reserved */
#define IRQ_ATM702X_CSI                ATM702X_IRQ(50)
#define IRQ_ATM702X_GPS                ATM702X_IRQ(51)
#define IRQ_ATM702X_SATA               ATM702X_IRQ(52)    /* reserved */
#define IRQ_ATM702X_L2                 ATM702X_IRQ(53)

/* Set the default NR_IRQS */
#define ATM702X_NR_IRQS			(ATM702X_IRQ(53) + 1)

/* virtual IRQs: external speical IRQs */
#define IRQ_SIRQ_BASE			(ATM702X_NR_IRQS)
#define IRQ_SIRQ0			(IRQ_SIRQ_BASE + 0)
#define IRQ_SIRQ1			(IRQ_SIRQ_BASE + 1)
#define IRQ_SIRQ2			(IRQ_SIRQ_BASE + 2)
#define ATM702X_NR_SIRQ			(3)

/* virtual IRQs: GPIO */
#define IRQ_GPIOA_BASE			(IRQ_SIRQ_BASE + ATM702X_NR_SIRQ)
#define IRQ_GPIOA(x)			(IRQ_GPIOA_BASE + (x))
#define IRQ_GPIOB_BASE			(IRQ_GPIOA(31) + 1)
#define IRQ_GPIOB(x)			(IRQ_GPIOB_BASE + (x))
#define IRQ_GPIOC_BASE			(IRQ_GPIOB(31) + 1)
#define IRQ_GPIOC(x)			(IRQ_GPIOC_BASE + (x))
#define IRQ_GPIOD_BASE			(IRQ_GPIOC(31) + 1)
#define IRQ_GPIOD(x)			(IRQ_GPIOD_BASE + (x))
#define IRQ_GPIOE_BASE			(IRQ_GPIOD(31) + 1)
#define IRQ_GPIOE(x)			(IRQ_GPIOE_BASE + (x))

#define ATM702X_NR_GPIO_INT		(4 * 32 + 4)
#define ATM702X_GPIO_TO_IRQ(gpio)	((gpio) + IRQ_GPIOA_BASE)
#define ATM702X_IRQ_TO_GPIO(irq)	((irq) - IRQ_GPIOA_BASE)

/* virtual IRQs: ATM702X */
#define IRQ_ATM702X_BASE		(IRQ_GPIOA_BASE + ATM702X_NR_GPIO_INT)
// reserved 16 interrupt sources for ATM702X
#define IRQ_ATM702X_MAX_NUM		16

#define NR_IRQS				(IRQ_ATM702X_BASE + IRQ_ATM702X_MAX_NUM)

#endif  /* __ASM_ARCH_IRQS_H */
