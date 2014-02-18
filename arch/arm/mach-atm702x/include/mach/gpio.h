/*
 * arch/arm/mach-atm702x/include/mach/gpio.h
 *
 * GPIO definitions
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __ASM_ARCH_GPIO_H
#define __ASM_ARCH_GPIO_H

#define ATM702X_GPIO_BANKS	5
#define ATM702X_GPIO_PER_BANK	32

/* GPIOA/B/C/D, GPIOE0 ~ 3 */
#define ATM702X_NR_GPIO		(4 * 32 + 4)

#define ATM702X_GPIO_PORTA(x) ((x) + ATM702X_GPIO_PER_BANK * 0)
#define ATM702X_GPIO_PORTB(x) ((x) + ATM702X_GPIO_PER_BANK * 1)
#define ATM702X_GPIO_PORTC(x) ((x) + ATM702X_GPIO_PER_BANK * 2)
#define ATM702X_GPIO_PORTD(x) ((x) + ATM702X_GPIO_PER_BANK * 3)
#define ATM702X_GPIO_PORTE(x) ((x) + ATM702X_GPIO_PER_BANK * 4)

#define ATM702X_GPIO_PORT(iogroup, pin_num) ((pin_num) + ATM702X_GPIO_PER_BANK * (iogroup))

extern int atm702x_gpio_init(void);

/**
 * struct icpad - ic pad information
 * @pad_index: pad index for gpio
 * @pre_cfg: pre-configuration
 * @pad_name: name of pad
 * @status:
 * @dev: currently belongs to which device
 */
struct gpio_pre_cfg {
	unsigned char iogroup;		// A, B, C, D, etc
	unsigned char pin_num;		// 0 - 31
	unsigned char gpio_dir;		// INPUT/OUTPUT
	unsigned char init_level;	// the initialize value
	unsigned char active_level;	// active level
	unsigned char unactive_level;	// unactive level
	unsigned char valid;		// valid
	unsigned char reserved[6];
	char name[64];
} __attribute__ ((packed));

extern int gpio_get_pre_cfg(char *gpio_name, struct gpio_pre_cfg *m_gpio);

#endif /* __ASM_ARCH_GPIO_H */

