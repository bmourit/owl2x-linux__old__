/*
 * arch/arm/mach-atm702x/include/mach/io.h
 *
 * serial definitions
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __ASM_ARCH_SERIAL_H
#define __ASM_ARCH_SERIAL_H

struct atm702x_uart_platform_data
{
    /* tx dma config */
    int use_dma_tx;

    /* rx dma config */
    int use_dma_rx;
    unsigned int rx_poll_rate;
    unsigned int rx_timeout;
};
#endif /* __ASM_ARCH_SERIAL_H */
