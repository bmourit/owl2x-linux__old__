/*
 * drivers/staging/android/ion/atm702x_ion.h
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/types.h>
#include "../ion.h"

#if !defined(__KERNEL__)
#define __user
#endif

#ifndef _LINUX_ATM702X_ION_H
#define _LINUX_ATM702X_ION_H

struct atm702x_ion_phys_data {
	struct ion_handle *handle;
	unsigned long phys_addr;
    size_t size;
};

/* Custom Ioctl's. */
enum {
	ATM702X_ION_GET_PHY = 0,
};

/* List of heaps in the system. */
enum {
	ATM702X_ION_HEAP_CARVEOUT = 0,
	ATM702X_ION_HEAP_CMA= 1,	
};

#endif /* _LINUX_ASOC_ION_H */
