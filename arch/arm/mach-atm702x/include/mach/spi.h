/*
 * arch/arm/mach-atm702x/include/mach/spi.h
 *
 * SPI platform/controller data definitions for Actions SOC
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
#ifndef __MACH_ATM702X_SPI_H__
#define __MACH_ATM702X_SPI_H__

/*
 * struct atm702x_spi_platform_data - platform data for spi master
 * @cs_gpio: gpio number of default chip select, used to support multi chipselect.
 * @num_chipselect: number of chip selects on this master, must be at least one
 */
struct atm702x_spi_platform_data {
    unsigned int default_cs_gpio;
    unsigned int num_chipselect;
};

/*
 * struct atm702x_spi_controller_data - controller data for spi device
 * @cs_gpio: gpio number for chip select.
 */
struct atm702x_spi_controller_data {
    unsigned int cs_gpio;
};

#endif  /* __MACH_ASOC_SPI_H__ */
