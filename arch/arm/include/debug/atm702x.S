/*
 * arch/arm/include/debug/atm702x.S
 *
 * Copyright (C) 2014 B. Mouritsen
 * Author: B. Mouritsen <bnmguy@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#if CONFIG_ATM702X_DEBUG_UART > 5
#error Invalid ATM702X debug UART
#endif

#ifdef CONFIG_ATM702X_SOC_ATM7029
#define ATM7029_UART0_PHYS_BASE (0xb0160000)
#define ATM7029_UART1_PHYS_BASE (0xb0162000)
#define ATM7029_UART2_PHYS_BASE (0xb0164000)
#define ATM7029_UART3_PHYS_BASE (0xb0166000)
#define ATM7029_UART4_PHYS_BASE (0xb0168000)
#define ATM7029_UART5_PHYS_BASE (0xb016a000)
#define ATM7029_UART0_VIRT_BASE (0xf8160000)
#define ATM7029_UART1_VIRT_BASE (0xf8162000)
#define ATM7029_UART2_VIRT_BASE (0xf8164000)
#define ATM7029_UART3_VIRT_BASE (0xf8166000)
#define ATM7029_UART4_VIRT_BASE (0xf8168000)
#define ATM7029_UART5_VIRT_BASE (0xf816a000)
#define __ATM702X_PHYS_UART(n)  ATM7029_UART##n##_PHYS_BASE
#define __ATM702X_VIRT_UART(n)  ATM7029_UART##n##_VIRT_BASE
#endif

#define ATM702X_PHYS_UART(n)    __ATM702X_PHYS_UART(n)
#define ATM702X_VIRT_UART(n)    __ATM702X_VIRT_UART(n)
#define UART_PHYS_BASE  ATM702X_PHYS_UART(CONFIG_ATM702X_DEBUG_UART)
#define UART_VIRT_BASE  ATM702X_VIRT_UART(CONFIG_ATM702X_DEBUG_UART)

#define UART_RXDAT      (0x0004)
#define UART_TXDAT      (0x0008)
#define UART_STAT       (0x000c)

                .macro  addruart, rp, rv, tmp
                ldr     \rp, =UART_PHYS_BASE    @ physical
                ldr     \rv, =UART_VIRT_BASE    @ virtual
                .endm

                .macro  senduart,rd,rx
                str     \rd, [\rx, #(UART_TXDAT)]       @ Write to Transmitter Holding Register
                .endm

                .macro  waituart,rd,rx
1001:           ldr     \rd, [\rx, #(UART_STAT)]        @ Read Status Register
                tst     \rd, #0x400                     @ TXFIFO Empty?
                beq     1001b
                .endm

                .macro  busyuart,rd,rx
1001:           ldr     \rd, [\rx, #(UART_STAT)]        @ Read Status Register
                tst     \rd, #0x40                      @ TXFIFO Full?
                bne     1001b
                .endm
