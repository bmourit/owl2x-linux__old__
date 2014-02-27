/*
 * linux/mfd/atc260x/regulator.h
 *
 * Regulator definitons for ATC260X PMIC
 *
 * (C) Copyright 2014
 * Author: B. Mouritsen <bnmguy@gmail.com>
 *
 * Structure based on the wm831x driver. 
 * Parts of the data provided by the Manufacturer.
 *
 * (C) Copyright 2009 Wolfson Microelectronics PLC.
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * (C) Copyright 2011 Actions Semiconductors
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#ifndef __MFD_ATC260X_REGULATOR_H__
#define __MFD_ATC260X_REGULATOR_H__

/*
 * R17 (0x11) - PMU DC1 Control 0
 */
#define ATC260X_DC1_RESERVED                    0xF000  /* DC1_RESERVED [15:12] */
#define ATC260X_DC1_RESERVED_MASK               0xF000  
#define ATC260X_DC1_RESERVED_SHIFT                  12
#define ATC260X_DC1_RESERVED_WIDTH                   4
#define ATC260X_DC1_VOL                         0x0F80  /* DC1_VOL [11:7] */
#define ATC260X_DC1_VOL_MASK                    0x0F80  
#define ATC260X_DC1_VOL_SHIFT                        7
#define ATC260X_DC1_VOL_WIDTH                        5
#define ATC260X_DC1_RESERVED2                   0x007F  /* DC1_VOL [6:0] */
#define ATC260X_DC1_RESERVED2_MASK              0x007F  
#define ATC260X_DC1_RESERVED2_SHIFT                  0
#define ATC260X_DC1_RESERVED2_WIDTH                  7

/**
 * R18:R19 (0x12:0x13) - PMU DC1 Control 1-2
 * PMU DC1 CONTROLS R18-R19 ARE RESERVED
 */

/*
 * R20 (0x14) - PMU DC2 Control 0
 */
#define ATC260X_DC2_EN                          0x8000  /* DC2_EN */
#define ATC260X_DC2_EN_MASK                     0x8000  
#define ATC260X_DC2_EN_SHIFT                        15
#define ATC260X_DC2_EN_WIDTH                         1
#define ATC260X_DC2_VOL                         0x7000  /* DC2_VOL [14:12] */
#define ATC260X_DC2_VOL_MASK                    0x7000  
#define ATC260X_DC2_VOL_SHIFT                       12
#define ATC260X_DC2_VOL_WIDTH                        3
#define ATC260X_DC2_RESERVED                    0x00FF  /* DC2_RESERVED [7:0] */
#define ATC260X_DC2_RESERVED_MASK               0x00FF  
#define ATC260X_DC2_RESERVED_SHIFT                   0
#define ATC260X_DC2_RESERVED_WIDTH                   8

/**
 * R21:R22 (0x15:0x16) - PMU DC2 Control 1-2
 * PMU DC2 CONTROLS R21-R22 ARE RESERVED
 */

/*
 * R23 (0x17) - PMU DC3 Control 0
 */
#define ATC260X_DC3_RESERVED                    0xF000  /* DC13_RESERVED [15:12] */
#define ATC260X_DC3_RESERVED_MASK               0xF000  
#define ATC260X_DC3_RESERVED_SHIFT                  12
#define ATC260X_DC3_RESERVED_WIDTH                   4
#define ATC260X_DC3_VOL                         0x0E00  /* DC3_VOL [11:9] */
#define ATC260X_DC3_VOL_MASK                    0x0E00  
#define ATC260X_DC3_VOL_SHIFT                        9
#define ATC260X_DC3_VOL_WIDTH                        3
#define ATC260X_EN_SETVCC                       0x0180  /* EN_SETVCC [8:7] */
#define ATC260X_EN_SETVCC_MASK                  0x0180  
#define ATC260X_EN_SETVCC_SHIFT                      7
#define ATC260X_EN_SETVCC_WIDTH                      2
#define ATC260X_DC3_RESERVED2                   0x007F  /* DC3_VOL [6:0] */
#define ATC260X_DC3_RESERVED2_MASK              0x007F  
#define ATC260X_DC3_RESERVED2_SHIFT                  0
#define ATC260X_DC3_RESERVED2_WIDTH                  7

/**
 * R24:R25 (0x18:0x19) - PMU DC3 Control 1-2
 * PMU DC3 CONTROLS R24-R25 ARE RESERVED
 */

/*
 * R26 (0x1A) - PMU DC4 Control 0
 */
#define ATC260X_DC4_RESERVED                    0xFFFE  /* DC4_RESERVED [15:1] */
#define ATC260X_DC4_RESERVED_MASK               0xFFFE  
#define ATC260X_DC4_RESERVED_SHIFT                   1
#define ATC260X_DC4_RESERVED_WIDTH                  15
#define ATC260X_DC4_EN                          0x0001  /* DC4_EN */
#define ATC260X_DC4_EN_MASK                     0x0001  
#define ATC260X_DC4_EN_SHIFT                         0
#define ATC260X_DC4_EN_WIDTH                         1

/**
 * R27 (0x1B) - PMU DC4 Control 1
 * PMU DC4 CONTROL R27 IS RESERVED
 */

/* TODO: 0X1C and 0X1D */

/**
 * R30 (0x1E) - PMU LDO1 Control
 */
#define ATC260X_LDO1_VOL                        0xE000  /* LDO1_VOL [15:13] */
#define ATC260X_LDO1_VOL_MASK                   0xE000  
#define ATC260X_LDO1_VOL_SHIFT                      13
#define ATC260X_LDO1_VOL_WIDTH                       3
#define ATC260X_LDO1_RESERVED                   0x1FFF  /* LDO1_RESERVED [12:0] */
#define ATC260X_LDO1_RESERVED_MASK              0x1FFF  
#define ATC260X_LDO1_RESERVED_SHIFT                  0
#define ATC260X_LDO1_RESERVED_WIDTH                 13

/**
 * R31 (0x1F) - PMU LDO2 Control
 */
#define ATC260X_LDO2_VOL                        0xE000  /* LDO2_VOL [15:13] */
#define ATC260X_LDO2_VOL_MASK                   0xE000  
#define ATC260X_LDO2_VOL_SHIFT                      13
#define ATC260X_LDO2_VOL_WIDTH                       3

/**
 * R32 (0x20) - PMU LDO3 Control
 */
#define ATC260X_LDO3_VOL                        0xE000  /* LDO3_VOL [15:13] */
#define ATC260X_LDO3_VOL_MASK                   0xE000  
#define ATC260X_LDO3_VOL_SHIFT                      13
#define ATC260X_LDO3_VOL_WIDTH                       3

/**
 * R33 (0x21) - PMU LDO4 Control
 */
#define ATC260X_LDO4_VOL                        0xE000  /* LDO4_VOL [15:13] */
#define ATC260X_LDO4_VOL_MASK                   0xE000  
#define ATC260X_LDO4_VOL_SHIFT                      13
#define ATC260X_LDO4_VOL_WIDTH                       3
#define ATC260X_LDO4_EN                         0x0001  /* LDO4_EN */
#define ATC260X_LDO4_EN_MASK                    0x0001  
#define ATC260X_LDO4_EN_SHIFT                        0
#define ATC260X_LDO4_EN_WIDTH                        1

/**
 * R34 (0x22) - PMU LDO5 Control
 */
#define ATC260X_LDO5_VOL                        0xE000  /* LDO5_VOL [15:13] */
#define ATC260X_LDO5_VOL_MASK                   0xE000  
#define ATC260X_LDO5_VOL_SHIFT                      13
#define ATC260X_LDO5_VOL_WIDTH                       3
#define ATC260X_LDO5_EN                         0x0001  /* LDO5_EN */
#define ATC260X_LDO5_EN_MASK                    0x0001  
#define ATC260X_LDO5_EN_SHIFT                        0
#define ATC260X_LDO5_EN_WIDTH                        1

/**
 * R35 (0x23) - PMU LDO6 Control
 */
#define ATC260X_LDO6_VOL                        0xF800  /* LDO6_VOL [15:11] */
#define ATC260X_LDO6_VOL_MASK                   0xF800  
#define ATC260X_LDO6_VOL_SHIFT                      11
#define ATC260X_LDO6_VOL_WIDTH                       5

/**
 * R36 (0x24) - PMU LDO7 Control
 */
#define ATC260X_LDO7_VOL                        0xE000  /* LDO7_VOL [15:13] */
#define ATC260X_LDO7_VOL_MASK                   0xE000  
#define ATC260X_LDO7_VOL_SHIFT                      13
#define ATC260X_LDO7_VOL_WIDTH                       3
#define ATC260X_LDO7_EN                         0x0001  /* LDO7_EN */
#define ATC260X_LDO7_EN_MASK                    0x0001  
#define ATC260X_LDO7_EN_SHIFT                        0
#define ATC260X_LDO7_EN_WIDTH                        1

/**
 * R37 (0x25) - PMU LDO8 Control
 */
#define ATC260X_LDO8_VOL                        0xF000  /* LDO8_VOL [15:12] */
#define ATC260X_LDO8_VOL_MASK                   0xF000  
#define ATC260X_LDO8_VOL_SHIFT                      12
#define ATC260X_LDO8_VOL_WIDTH                       4
#define ATC260X_LDO8_EN                         0x0001  /* LDO8_EN */
#define ATC260X_LDO8_EN_MASK                    0x0001  
#define ATC260X_LDO8_EN_SHIFT                        0
#define ATC260X_LDO8_EN_WIDTH                        1

/**
 * R38 (0x26) - PMU LDO9 Control
 */
#define ATC260X_LDO9_VOL                        0xE000  /* LDO9_VOL [15:13] */
#define ATC260X_LDO9_VOL_MASK                   0xE000  
#define ATC260X_LDO9_VOL_SHIFT                      13
#define ATC260X_LDO9_VOL_WIDTH                       3
#define ATC260X_LDO9_EN                         0x0001  /* LDO9_EN */
#define ATC260X_LDO9_EN_MASK                    0x0001  
#define ATC260X_LDO9_EN_SHIFT                        0
#define ATC260X_LDO9_EN_WIDTH                        1

/**
 * R39 (0x27) - PMU LDO10 Control
 */
#define ATC260X_LDO10_VOL                       0xF000  /* LDO10_VOL [15:12] */
#define ATC260X_LDO10_VOL_MASK                  0xF000  
#define ATC260X_LDO10_VOL_SHIFT                     12
#define ATC260X_LDO10_VOL_WIDTH                      4
#define ATC260X_LDO10_EN                        0x0001  /* LDO10_EN */
#define ATC260X_LDO10_EN_MASK                   0x0001  
#define ATC260X_LDO10_EN_SHIFT                       0
#define ATC260X_LDO10_EN_WIDTH                       1

/**
 * R40 (0x28) - PMU LDO11 Control
 */
#define ATC260X_LDO11_VOL                       0xE000  /* LDO11_VOL [15:13] */
#define ATC260X_LDO11_VOL_MASK                  0xE000  
#define ATC260X_LDO11_VOL_SHIFT                     13
#define ATC260X_LDO11_VOL_WIDTH                      3
#define ATC260X_LDO11_EN                        0x1000  /* LDO11_EN */
#define ATC260X_LDO11_EN_MASK                   0x1000  
#define ATC260X_LDO11_EN_SHIFT                      12
#define ATC260X_LDO11_EN_WIDTH                       1

/**
 * R41 (0x29) - PMU Switch Control
 */
#define ATC260X_SWITCH1_EN                      0x8000  /* SWITCH1_EN */
#define ATC260X_SWITCH1_EN_MASK                 0x8000
#define ATC260X_SWITCH1_EN_SHIFT                    15
#define ATC260X_SWITCH1_EN_WIDTH                     1
#define ATC260X_SWITCH2_EN                      0x4000  /* SWITCH2_EN */
#define ATC260X_SWITCH2_EN_MASK                 0x4000
#define ATC260X_SWITCH2_EN_SHIFT                    14
#define ATC260X_SWITCH2_EN_WIDTH                     1
#define ATC260X_SWITCH2_MOD                     0x2000  /* SWITCH2_MOD */
#define ATC260X_SWITCH2_MOD_MASK                0x2000  
#define ATC260X_SWITCH2_MOD_SHIFT                   13
#define ATC260X_SWITCH2_MOD_WIDTH                    1
#define ATC260X_SWITCH2_LDO_VOL_RNG             0x1000  /* SWITCH2_LDO_VOL_RNG */
#define ATC260X_SWITCH2_LDO_VOL_RNG_MASK        0x1000
#define ATC260X_SWITCH2_LDO_VOL_RNG_SHIFT           12
#define ATC260X_SWITCH2_LDO_VOL_RNG_WIDTH            1
#define ATC260X_SWITCH2_LDO_VOL                 0x0F00  /* SWITCH2_LDO_VOL [11:8] */
#define ATC260X_SWITCH2_LDO_VOL_MASK            0x0F00
#define ATC260X_SWITCH2_LDO_VOL_SHIFT                8
#define ATC260X_SWITCH2_LDO_VOL_WIDTH                4
#define ATC260X_SWITCH1_MODE                    0x0020  /* SWITCH1_MODE */
#define ATC260X_SWITCH1_MODE_MASK               0x0020
#define ATC260X_SWITCH1_MODE_SHIFT                   5
#define ATC260X_SWITCH1_MODE_WIDTH                   1
#define ATC260X_SWITCH1_LDO_VOL                 0x0018  /* SWITCH1_LDO_VOL [4:3] */
#define ATC260X_SWITCH1_LDO_VOL_MASK            0x0018
#define ATC260X_SWITCH1_LDO_VOL_SHIFT                3
#define ATC260X_SWITCH1_LDO_VOL_WIDTH                2
#define ATC260X_SWITCH1_DISCHARGE_EN            0x0002  /* SWITCH1__DISCHARGE_EN */
#define ATC260X_SWITCH1_DISCHARGE_EN_MASK       0x0002
#define ATC260X_SWITCH1_DISCHARGE_EN_SHIFT           1
#define ATC260X_SWITCH1_DISCHARGE_EN_WIDTH           1
