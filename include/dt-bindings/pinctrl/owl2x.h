/*
 * This header provides constants for Owl2x pinctrl bindings.
 *
 * Copyright (C) 2014
 * Author: B. Mouritsen <bnmguy@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef _DT_BINDINGS_PINCTRL_OWL2X_H
#define _DT_BINDINGS_PINCTRL_OWL2X_H

#define OWL2X_PIN_DISABLE                       0
#define OWL2X_PIN_ENABLE                        1

#define OWL2X_PIN_PULL_NONE                     0
#define OWL2X_PIN_PULL_DOWN                     1
#define OWL2X_PIN_PULL_UP                       2

#define OWL2X_PIN_OUTPUT_EN                     0
#define OWL2X_PIN_INPUT_EN                      1

/* Low power mode driver */
#define OWL2X_PIN_DRV_STRENGTH_L1               0
#define OWL2X_PIN_DRV_STRENGTH_L2               1
#define OWL2X_PIN_DRV_STRENGTH_L3               2
#define OWL2X_PIN_DRV_STRENGTH_L4               3

/* Rising/Falling slew rate */
#define OWL2X_PIN_SLEW_RATE_FASTEST             0
#define OWL2X_PIN_SLEW_RATE_FAST                1
#define OWL2X_PIN_SLEW_RATE_SLOW                2
#define OWL2X_PIN_SLEW_RATE_SLOWEST             3

#endif
