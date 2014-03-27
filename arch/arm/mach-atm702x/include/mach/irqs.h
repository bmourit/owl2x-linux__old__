/*
 * arch/arm/mach-gl5202/include/mach/irqs.h
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

#define IRQ_LOCALTIMER              (29)
#define IRQ_LOCALWDT                (30)

#define ASOC_IRQ_OFFSET             (32)
#define ASOC_IRQ(x)                 ((x) + ASOC_IRQ_OFFSET)

#define IRQ_ASOC_AUIP               ASOC_IRQ(0)     /* reserved */
#define IRQ_ASOC_ETHERNET           ASOC_IRQ(1)
#define IRQ_ASOC_DE                 ASOC_IRQ(2)
#define IRQ_ASOC_GPU_2D             ASOC_IRQ(3)
#define IRQ_ASOC_GPU_3D             ASOC_IRQ(4)
#define IRQ_ASOC_VIN                ASOC_IRQ(5)
#define IRQ_ASOC_PC0                ASOC_IRQ(6)
#define IRQ_ASOC_PC1                ASOC_IRQ(7)
#define IRQ_ASOC_PC2                ASOC_IRQ(8)
#define IRQ_ASOC_2HZ0               ASOC_IRQ(9)
#define IRQ_ASOC_2HZ1               ASOC_IRQ(10)
#define IRQ_ASOC_TIMER0             ASOC_IRQ(11)
#define IRQ_ASOC_TIMER1             ASOC_IRQ(12)
#define IRQ_ASOC_BISP               ASOC_IRQ(13)
#define IRQ_ASOC_SIRQ0              ASOC_IRQ(14)
#define IRQ_ASOC_DMA                ASOC_IRQ(15)
#define IRQ_ASOC_KEY                ASOC_IRQ(16)
#define IRQ_ASOC_SIRQ1              ASOC_IRQ(17)
#define IRQ_ASOC_PCM0               ASOC_IRQ(18)
#define IRQ_ASOC_PCM1               ASOC_IRQ(19)
#define IRQ_ASOC_SPI0               ASOC_IRQ(20)
#define IRQ_ASOC_SPI1               ASOC_IRQ(21)
#define IRQ_ASOC_SPI2               ASOC_IRQ(22)
#define IRQ_ASOC_SPI3               ASOC_IRQ(23)
#define IRQ_ASOC_USB                ASOC_IRQ(24)
#define IRQ_ASOC_I2C0               ASOC_IRQ(25)
#define IRQ_ASOC_I2C1               ASOC_IRQ(26)
#define IRQ_ASOC_I2C2               ASOC_IRQ(27)
#define IRQ_ASOC_UART0              ASOC_IRQ(28)
#define IRQ_ASOC_UART1              ASOC_IRQ(29)
#define IRQ_ASOC_UART2              ASOC_IRQ(30)
#define IRQ_ASOC_UART3              ASOC_IRQ(31)
#define IRQ_ASOC_UART4              ASOC_IRQ(32)
#define IRQ_ASOC_UART5              ASOC_IRQ(33)
#define IRQ_ASOC_GPIO               ASOC_IRQ(34)
#define IRQ_ASOC_USB2_0             ASOC_IRQ(35)
#define IRQ_ASOC_USB2_1             ASOC_IRQ(36)
#define IRQ_ASOC_SIRQ2              ASOC_IRQ(37)
#define IRQ_ASOC_DCU_DEBUG          ASOC_IRQ(38)
#define IRQ_ASOC_NAND               ASOC_IRQ(39)
#define IRQ_ASOC_SD0                ASOC_IRQ(40)
#define IRQ_ASOC_SD1                ASOC_IRQ(41)
#define IRQ_ASOC_SD2                ASOC_IRQ(42)
#define IRQ_ASOC_LCD                ASOC_IRQ(43)
#define IRQ_ASOC_HDMI               ASOC_IRQ(44)
#define IRQ_ASOC_TVOUT              ASOC_IRQ(45)
#define IRQ_ASOC_AUDIO_INOUT        ASOC_IRQ(46)
#define IRQ_ASOC_H264_JPEG_ENCODER  ASOC_IRQ(47)
#define IRQ_ASOC_HIVDE_DECODER      ASOC_IRQ(48)
#define IRQ_ASOC_DSI                ASOC_IRQ(49)    /* reserved */
#define IRQ_ASOC_CSI                ASOC_IRQ(50)
#define IRQ_ASOC_GPS                ASOC_IRQ(51)
#define IRQ_ASOC_SATA               ASOC_IRQ(52)    /* reserved */
#define IRQ_ASOC_L2                 ASOC_IRQ(53)

/* Set the default NR_IRQS */
#define ASOC_NR_IRQS                (ASOC_IRQ(53) + 1)

/* virtual IRQs: external speical IRQs */
#define IRQ_SIRQ_BASE               (ASOC_NR_IRQS)
#define IRQ_SIRQ0                   (IRQ_SIRQ_BASE + 0)
#define IRQ_SIRQ1                   (IRQ_SIRQ_BASE + 1)
#define IRQ_SIRQ2                   (IRQ_SIRQ_BASE + 2)
#define ASOC_NR_SIRQ                (3)

/* virtual IRQs: GPIO */
#define IRQ_GPIOA_BASE              (IRQ_SIRQ_BASE + ASOC_NR_SIRQ)
#define IRQ_GPIOA(x)                (IRQ_GPIOA_BASE + (x))
#define IRQ_GPIOB_BASE              (IRQ_GPIOA(31) + 1)
#define IRQ_GPIOB(x)                (IRQ_GPIOB_BASE + (x))
#define IRQ_GPIOC_BASE              (IRQ_GPIOB(31) + 1)
#define IRQ_GPIOC(x)                (IRQ_GPIOC_BASE + (x))
#define IRQ_GPIOD_BASE              (IRQ_GPIOC(31) + 1)
#define IRQ_GPIOD(x)                (IRQ_GPIOD_BASE + (x))
#define IRQ_GPIOE_BASE              (IRQ_GPIOD(31) + 1)
#define IRQ_GPIOE(x)                (IRQ_GPIOE_BASE + (x))

#define ASOC_NR_GPIO_INT            (4 * 32 + 4)
#define ASOC_GPIO_TO_IRQ(gpio)      ((gpio) + IRQ_GPIOA_BASE)
#define ASOC_IRQ_TO_GPIO(irq)       ((irq) - IRQ_GPIOA_BASE)

/* virtual IRQs: GL5302 */
#define IRQ_GL5302_BASE             (IRQ_GPIOA_BASE + ASOC_NR_GPIO_INT)
// reserved 16 interrupt sources for GL5302
#define IRQ_GL5302_MAX_NUM          16

#define NR_IRQS                     (IRQ_GL5302_BASE + IRQ_GL5302_MAX_NUM)

#endif  /* __ASM_ARCH_IRQS_H */
