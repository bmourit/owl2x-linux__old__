/*
 * arch/arm/mach-atm702x/include/mach/debug-macro.S
 *
 * Debugging macro include header
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <mach/hardware.h>

    .macro  addruart, rp, rv, tmp
        ldr     \rp, =UART5_BASE    @ physical
        ldr     \rv, =IO_ADDRESS(UART5_BASE)        @ virtual
    .endm

    .macro  senduart,rd,rx
        str     \rd, [\rx, #(UART5_TXDAT - UART5_BASE)] @ Write to Transmitter Holding Register
    .endm

    .macro  busyuart, rd, rx
1001:
        ldr \rd, [\rx, #(UART5_STAT - UART5_BASE)]      @ Read Status Register
        tst \rd, #0x40              @ TXFIFO Full?
        bne 1001b
    .endm

    .macro  waituart,rd,rx
1001:
        ldr \rd, [\rx, #(UART5_STAT - UART5_BASE)]      @ Read Status Register
        tst \rd, #0x400             @ TXFIFO Empty?
        beq 1001b
    .endm
