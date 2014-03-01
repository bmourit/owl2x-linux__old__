/*
 * Copyright 2011 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#ifndef __MFD_ATC260X_PMU_H__
#define __MFD_ATC260X_PMU_H__

#include <linux/mfd/atc260x/reg.h>

#define PSY_NAME_WALL  "atc260x-wall"
#define PSY_NAME_USB   "atc260x-usb"

/**
 * R0 (0x00) - PMU System Control 0
 */
#define ATC260X_USB_WK_EN                       0x8000  /* USB_WK */
#define ATC260X_USB_WK_EN_MASK                  0x8000  /* USB_WK */
#define ATC260X_USB_WK_EN_SHIFT                     15  /* USB_WK */
#define ATC260X_USB_WK_EN_WIDTH                      1  /* USB_WK */
#define ATC260X_WALL_WK_EN                      0x4000  /* WALL_WK */
#define ATC260X_WALL_WK_EN_MASK                 0x4000  /* WALL_WK */
#define ATC260X_WALL_WK_EN_SHIFT                    14  /* WALL_WK */
#define ATC260X_WALL_WK_EN_WIDTH                     1  /* WALL_WK */
#define ATC260X_ONOFF_LONG_WK_EN                0x2000  /* ONOFF_LONG_WK */
#define ATC260X_ONOFF_LONG_WK_EN_MASK           0x2000  /* ONOFF_LONG_WK */
#define ATC260X_ONOFF_LONG_WK_EN_SHIFT              13  /* ONOFF_LONG_WK */
#define ATC260X_ONOFF_LONG_WK_EN_WIDTH               1  /* ONOFF_LONG_WK */
#define ATC260X_ONOFF_SHORT_WK_EN               0x1000  /* ONOFF_SHORT_WK */
#define ATC260X_ONOFF_SHORT_WK_EN_MASK          0x1000  /* ONOFF_SHORT_WK */
#define ATC260X_ONOFF_SHORT_WK_EN_SHIFT             12  /* ONOFF_SHORT_WK */
#define ATC260X_ONOFF_SHORT_WK_EN_WIDTH              1  /* ONOFF_SHORT_WK */
#define ATC260X_WKIRQ_WK_EN                     0x0800  /* WKIRQ_WK */
#define ATC260X_WKIRQ_WK_EN_MASK                0x0800  /* WKIRQ_WK */
#define ATC260X_WKIRQ_WK_EN_SHIFT                   11  /* WKIRQ_WK */
#define ATC260X_WKIRQ_WK_EN_WIDTH                    1  /* WKIRQ_WK */
#define ATC260X_TP_WK_EN                        0x0400  /* TP_WK */
#define ATC260X_TP_WK_EN_MASK                   0x0400  /* TP_WK */
#define ATC260X_TP_WK_EN_SHIFT                      10  /* TP_WK */
#define ATC260X_TP_WK_EN_WIDTH                       1  /* TP_WK */
#define ATC260X_REM_CON_WK_EN                   0x0200  /* REM_CON_WK */
#define ATC260X_REM_CON_WK_EN_MASK              0x0200  /* REM_CON_WK */
#define ATC260X_REM_CON_WK_EN_SHIFT                  9  /* REM_CON_WK */
#define ATC260X_REM_CON_WK_EN_WIDTH                  1  /* REM_CON_WK */
#define ATC260X_ALARM_WK_EN                     0x0100  /* ALARM_WK */
#define ATC260X_ALARM_WK_EN_MASK                0x0100  /* ALARM_WK */
#define ATC260X_ALARM_WK_EN_SHIFT                    8  /* ALARM_WK */
#define ATC260X_ALARM_WK_EN_WIDTH                    1  /* ALARM_WK */
#define ATC260X_HDSW_WK_EN                      0x0080  /* HDSW_WK(Hard Switch) */
#define ATC260X_HDSW_WK_EN_MASK                 0x0080  /* HDSW_WK(Hard Switch) */
#define ATC260X_HDSW_WK_EN_SHIFT                     7  /* HDSW_WK(Hard Switch) */
#define ATC260X_HDSW_WK_EN_WIDTH                     1  /* HDSW_WK(Hard Switch) */
#define ATC260X_RESET_WK_EN                     0x0040  /* RESET_WK */
#define ATC260X_RESET_WK_EN_MASK                0x0040  /* RESET_WK */
#define ATC260X_RESET_WK_EN_SHIFT                    6  /* RESET_WK */
#define ATC260X_RESET_WK_EN_WIDTH                    1  /* RESET_WK */
#define ATC260X_IR_WK_EN                        0x0020  /* IR_WK */
#define ATC260X_IR_WK_EN_MASK                   0x0020  /* IR_WK */
#define ATC260X_IR_WK_EN_SHIFT                       5  /* IR_WK */
#define ATC260X_IR_WK_EN_WIDTH                       1  /* IR_WK */
#define ATC260X_VBUS_WK_TH_MASK                 0x0018  /* VBUS_WK_TH - [4:3] */
#define ATC260X_VBUS_WK_TH_SHIFT                     3  /* VBUS_WK_TH - [4:3] */
#define ATC260X_VBUS_WK_TH_WIDTH                     2  /* VBUS_WK_TH - [4:3] */
#define ATC260X_WALL_WK_TH_MASK                 0X0006  /* WALL_WK_TH - [2:1] */
#define ATC260X_WALL_WK_TH_SHIFT                     1  /* WALL_WK_TH - [2:1] */
#define ATC260X_WALL_WK_TH_WIDTH                     2  /* WALL_WK_TH - [2:1] */
#define ATC260X_ONOFF_MUXKEY_EN                 0X0001  /* ONOFF_MUXKEY */
#define ATC260X_ONOFF_MUXKEY_EN_MASK            0X0001  /* ONOFF_MUXKEY */
#define ATC260X_ONOFF_MUXKEY_EN_SHIFT                0  /* ONOFF_MUXKEY */
#define ATC260X_ONOFF_MUXKEY_EN_WIDTH                1  /* ONOFF_MUXKEY */

/**
 * R1 (0x01) - PMU System Control 1
 */
#define ATC260X_USB_WK_EN                       0x8000  /* USB_WK */
#define ATC260X_USB_WK_EN_MASK                  0x8000  /* USB_WK */
#define ATC260X_USB_WK_EN_SHIFT                     15  /* USB_WK */
#define ATC260X_USB_WK_EN_WIDTH                      1  /* USB_WK */
#define ATC260X_WALL_WK_EN                      0x4000  /* WALL_WK */
#define ATC260X_WALL_WK_EN_MASK                 0x4000  /* WALL_WK */
#define ATC260X_WALL_WK_EN_SHIFT                    14  /* WALL_WK */
#define ATC260X_WALL_WK_EN_WIDTH                     1  /* WALL_WK */
#define ATC260X_ONOFF_LONG_WK_EN                0x2000  /* ONOFF_LONG_WK */
#define ATC260X_ONOFF_LONG_WK_EN_MASK           0x2000  /* ONOFF_LONG_WK */
#define ATC260X_ONOFF_LONG_WK_EN_SHIFT              13  /* ONOFF_LONG_WK */
#define ATC260X_ONOFF_LONG_WK_EN_WIDTH               1  /* ONOFF_LONG_WK */
#define ATC260X_ONOFF_SHORT_WK_EN               0x1000  /* ONOFF_SHORT_WK */
#define ATC260X_ONOFF_SHORT_WK_EN_MASK          0x1000  /* ONOFF_SHORT_WK */
#define ATC260X_ONOFF_SHORT_WK_EN_SHIFT             12  /* ONOFF_SHORT_WK */
#define ATC260X_ONOFF_SHORT_WK_EN_WIDTH              1  /* ONOFF_SHORT_WK */
#define ATC260X_WKIRQ_WK_EN                     0x0800  /* WKIRQ_WK */
#define ATC260X_WKIRQ_WK_EN_MASK                0x0800  /* WKIRQ_WK */
#define ATC260X_WKIRQ_WK_EN_SHIFT                   11  /* WKIRQ_WK */
#define ATC260X_WKIRQ_WK_EN_WIDTH                    1  /* WKIRQ_WK */
#define ATC260X_TP_WK_EN                        0x0400  /* TP_WK */
#define ATC260X_TP_WK_EN_MASK                   0x0400  /* TP_WK */
#define ATC260X_TP_WK_EN_SHIFT                      10  /* TP_WK */
#define ATC260X_TP_WK_EN_WIDTH                       1  /* TP_WK */
#define ATC260X_REM_CON_WK_EN                   0x0200  /* REM_CON_WK */
#define ATC260X_REM_CON_WK_EN_MASK              0x0200  /* REM_CON_WK */
#define ATC260X_REM_CON_WK_EN_SHIFT                  9  /* REM_CON_WK */
#define ATC260X_REM_CON_WK_EN_WIDTH                  1  /* REM_CON_WK */
#define ATC260X_ALARM_WK_EN                     0x0100  /* ALARM_WK */
#define ATC260X_ALARM_WK_EN_MASK                0x0100  /* ALARM_WK */
#define ATC260X_ALARM_WK_EN_SHIFT                    8  /* ALARM_WK */
#define ATC260X_ALARM_WK_EN_WIDTH                    1  /* ALARM_WK */
#define ATC260X_HDSW_WK_EN                      0x0080  /* HDSW_WK(Hard Switch) */
#define ATC260X_HDSW_WK_EN_MASK                 0x0080  /* HDSW_WK(Hard Switch) */
#define ATC260X_HDSW_WK_EN_SHIFT                     7  /* HDSW_WK(Hard Switch) */
#define ATC260X_HDSW_WK_EN_WIDTH                     1  /* HDSW_WK(Hard Switch) */
#define ATC260X_RESET_WK_EN                     0x0040  /* RESET_WK */
#define ATC260X_RESET_WK_EN_MASK                0x0040  /* RESET_WK */
#define ATC260X_RESET_WK_EN_SHIFT                    6  /* RESET_WK */
#define ATC260X_RESET_WK_EN_WIDTH                    1  /* RESET_WK */
#define ATC260X_IR_WK_EN                        0x0020  /* IR_WK */
#define ATC260X_IR_WK_EN_MASK                   0x0020  /* IR_WK */
#define ATC260X_IR_WK_EN_SHIFT                       5  /* IR_WK */
#define ATC260X_IR_WK_EN_WIDTH                       1  /* IR_WK */
#define ATC260X_LB_S4_MASK                      0x0018  /* LB_S4 - [4:3] (Low power S4 Voltage) */
#define ATC260X_LB_S4_SHIFT                          3  /* LB_S4 - [4:3] (Low power S4 Voltage) */
#define ATC260X_LB_S4_WIDTH                          2  /* LB_S4 - [4:3] (Low power S4 Voltage) */
#define ATC260X_LB_S4_EN                        0x0004  /* LB_S4_EN (Enable S4 detection) */
#define ATC260X_LB_S4_EN_MASK                   0x0004  /* LB_S4_EN (Enable S4 detection) */
#define ATC260X_LB_S4_EN_SHIFT                       2  /* LB_S4_EN (Enable S4 detection) */
#define ATC260X_LB_S4_EN_WIDTH                       1  /* LB_S4_EN (Enable S4 detection) */
#define ATC260X_ENRTCOSC                        0x0002  /* ENRTCOSC (Enable 32k internal clock) */
#define ATC260X_ENRTCOSC_MASK                   0x0002  /* ENRTCOSC (Enable 32k internal clock) */
#define ATC260X_ENRTCOSC_SHIFT                       1  /* ENRTCOSC (Enable 32k internal clock) */
#define ATC260X_ENRTCOSC_WIDTH                       1  /* ENRTCOSC (Enable 32k internal clock) */
#define ATC260X_EN_S1                           0x0001  /* EN_S1 */
#define ATC260X_EN_S1_MASK                      0x0001  /* EN_S1 */
#define ATC260X_EN_S1_SHIFT                          0  /* EN_S1 */
#define ATC260X_EN_S1_WIDTH                          1  /* EN_S1 */

/**
 * R2 (0x02) - PMU System Control 2
 */
#define ATC260X_ONOFF_PRESS                     0x8000  /* ONOFF_PRESS */
#define ATC260X_ONOFF_PRESS_MASK                0x8000  /* ONOFF_PRESS */
#define ATC260X_ONOFF_PRESS_SHIFT                   15  /* ONOFF_PRESS */
#define ATC260X_ONOFF_PRESS_WIDTH                    1  /* ONOFF_PRESS */
#define ATC260X_ONOFF_SHORT_PRESS               0x4000  /* ONOFF_SHORT_PRESS */
#define ATC260X_ONOFF_SHORT_PRESS_MASK          0x4000  /* ONOFF_SHORT_PRESS */
#define ATC260X_ONOFF_SHORT_PRESS_SHIFT             14  /* ONOFF_SHORT_PRESS */
#define ATC260X_ONOFF_SHORT_PRESS_WIDTH              1  /* ONOFF_SHORT_PRESS */
#define ATC260X_ONOFF_LONG_PRESS                0x2000  /* ONOFF_LONG_PRESS */
#define ATC260X_ONOFF_LONG_PRESS_MASK           0x2000  /* ONOFF_LONG_PRESS */
#define ATC260X_ONOFF_LONG_PRESS_SHIFT              13  /* ONOFF_LONG_PRESS */
#define ATC260X_ONOFF_LONG_PRESS_WIDTH               1  /* ONOFF_LONG_PRESS */
#define ATC260X_ONOFF_INT_EN                    0x1000  /* ONOFF_INT_EN */
#define ATC260X_ONOFF_INT_EN_MASK               0x1000  /* ONOFF_INT_EN */
#define ATC260X_ONOFF_INT_EN_SHIFT                  12  /* ONOFF_INT_EN */
#define ATC260X_ONOFF_INT_EN_WIDTH                   1  /* ONOFF_INT_EN */
#define ATC260X_ONOFF_PRESS_TIME_MASK           0x0C00  /* ONOFF_PRESS_TIME - [11:10] */
#define ATC260X_ONOFF_PRESS_TIME_SHIFT              10  /* ONOFF_PRESS_TIME - [11:10] */
#define ATC260X_ONOFF_PRESS_TIME_WIDTH               2  /* ONOFF_PRESS_TIME - [11:10] */
#define ATC260X_ONOFF_RESET_EN                  0x0200  /* ONOFF_RESET_EN */
#define ATC260X_ONOFF_RESET_EN_MASK             0x0200  /* ONOFF_RESET_EN */
#define ATC260X_ONOFF_RESET_EN_SHIFT                 9  /* ONOFF_RESET_EN */
#define ATC260X_ONOFF_RESET_EN_WIDTH                 1  /* ONOFF_RESET_EN */
#define ATC260X_ONOFF_RESET_TIME_SEL_MASK       0x0180  /* ONOFF_RESET_TIME_SEL - [8:7] */
#define ATC260X_ONOFF_RESET_TIME_SEL_SHIFT           7  /* ONOFF_RESET_TIME_SEL - [8:7] */
#define ATC260X_ONOFF_RESET_TIME_SEL_WIDTH           2  /* ONOFF_RESET_TIME_SEL - [8:7] */
#define ATC260X_S2_TIMER_EN                     0x0040  /* S2_TIMER_EN */
#define ATC260X_S2_TIMER_EN_MASK                0x0040  /* S2_TIMER_EN */
#define ATC260X_S2_TIMER_EN_SHIFT                    6  /* S2_TIMER_EN */
#define ATC260X_S2_TIMER_EN_WIDTH                    1  /* S2_TIMER_EN */
#define ATC260X_S2TIMER_MASK                    0x0038  /* S2TIMER - [5:3] */
#define ATC260X_S2TIMER_SHIFT                        3  /* S2TIMER - [5:3] */
#define ATC260X_S2TIMER_WIDTH                        3  /* S2TIMER - [5:3] */
/* These are reserved and their use is unknown */
#define ATC260X_PMU_RESERVED2_MASK              0x0007  /* RESERVED - [2:0] */
#define ATC260X_PMU_RESERVED2_SHIFT                  0  /* RESERVED - [2:0] */
#define ATC260X_PMU_RESERVED2_WIDTH                  3  /* RESERVED - [2:0] */

/**
 * R3 (0x03) - PMU System Control 3
 */
#define ATC260X_EN_S2                           0x8000  /* EN_S2 */
#define ATC260X_EN_S2_MASK                      0x8000  /* EN_S2 */
#define ATC260X_EN_S2_SHIFT                         15  /* EN_S2 */
#define ATC260X_EN_S2_WIDTH                          1  /* EN_S2 */
#define ATC260X_EN_S3                           0x4000  /* EN_S3 */
#define ATC260X_EN_S3_MASK                      0x4000  /* EN_S3 */
#define ATC260X_EN_S3_SHIFT                         14  /* EN_S3 */
#define ATC260X_EN_S3_WIDTH                          1  /* EN_S3 */
#define ATC260X_S3_TIMER_EN                     0x2000  /* S3_TIMER_EN */
#define ATC260X_S3_TIMER_EN_MASK                0x2000  /* S3_TIMER_EN */
#define ATC260X_S3_TIMER_EN_SHIFT                   13  /* S3_TIMER_EN */
#define ATC260X_S3_TIMER_EN_WIDTH                    1  /* S3_TIMER_EN */
#define ATC260X_S3TIMER_MASK                    0x1C00  /* S3TIMER - [12:10] */
#define ATC260X_S3TIMER_SHIFT                       10  /* S3TIMER - [12:10] */
#define ATC260X_S3TIMER_WIDTH                        3  /* S3TIMER - [12:10] */
/* RESERVED has unknown use */
#define ATC260X_PMU_RESERVED3_MASK              0x03F0  /* RESERVED - [9:4] */
#define ATC260X_PMU_RESERVED3_SHIFT                  4  /* RESERVED - [9:4] */
#define ATC260X_PMU_RESERVED3_WIDTH                  6  /* RESERVED - [9:4] */
#define ATC260X_IR_PIN_TYPE                     0x0008  /* IR_PIN_TYPE */
#define ATC260X_IR_PIN_TYPE_MASK                0x0008  /* IR_PIN_TYPE */
#define ATC260X_IR_PIN_TYPE_SHIFT                    3  /* IR_PIN_TYPE */
#define ATC260X_IR_PIN_TYPE_WIDTH                    1  /* IR_PIN_TYPE */
#define ATC260X_IR_GPIO_OUTPUT_EN               0x0004  /* IR_GPIO_OUTPUT_EN */
#define ATC260X_IR_GPIO_OUTPUT_EN_MASK          0x0004  /* IR_GPIO_OUTPUT_EN */
#define ATC260X_IR_GPIO_OUTPUT_EN_SHIFT              2  /* IR_GPIO_OUTPUT_EN */
#define ATC260X_IR_GPIO_OUTPUT_EN_WIDTH              1  /* IR_GPIO_OUTPUT_EN */
#define ATC260X_IR_GPIO_INPUT_EN                0x0002  /* IR_GPIO_INPUT_EN */
#define ATC260X_IR_GPIO_INPUT_EN_MASK           0x0002  /* IR_GPIO_INPUT_EN */
#define ATC260X_IR_GPIO_INPUT_EN_SHIFT               1  /* IR_GPIO_INPUT_EN */
#define ATC260X_IR_GPIO_INPUT_EN_WIDTH               1  /* IR_GPIO_INPUT_EN */
#define ATC260X_IR_GPIO_DATA                    0x0001  /* IR_GPIO_DATA */
#define ATC260X_IR_GPIO_DATA_MASK               0x0001  /* IR_GPIO_DATA */
#define ATC260X_IR_GPIO_DATA_SHIFT                   0  /* IR_GPIO_DATA */
#define ATC260X_IR_GPIO_DATA_WIDTH                   1  /* IR_GPIO_DATA */                              

/**
 * R4 (0x04) - PMU System Control 4
 */
#define ATC260X_WKIRQ_PIN_TYPE                  0x8000  /* WKIRQ_PIN_TYPE */
#define ATC260X_WKIRQ_PIN_TYPE_MASK             0x8000
#define ATC260X_WKIRQ_PIN_TYPE_SHIFT                15
#define ATC260X_WKIRQ_PIN_TYPE_WIDTH                 1
#define ATC260X_WKIRQ_GPIO_OUTPUT_EN            0x4000  /* WKIRQ_GPIO_OUTPUT_EN */
#define ATC260X_WKIRQ_GPIO_OUTPUT_EN_MASK       0x4000
#define ATC260X_WKIRQ_GPIO_OUTPUT_EN_SHIFT          14
#define ATC260X_WKIRQ_GPIO_OUTPUT_EN_WIDTH           1
#define ATC260X_WKIRQ_GPIO_INPUT_EN             0x2000  /* WKIRQ_GPIO_INPUT_EN */
#define ATC260X_WKIRQ_GPIO_INPUT_EN_MASK        0x2000
#define ATC260X_WKIRQ_GPIO_INPUT_EN_SHIFT           13
#define ATC260X_WKIRQ_GPIO_INPUT_EN_WIDTH            1
#define ATC260X_WKIRQ_GPIO_DATA                 0x1000  /* WKIRQ_GPIO_DATA */
#define ATC260X_WKIRQ_GPIO_DATA_MASK            0x1000
#define ATC260X_WKIRQ_GPIO_DATA_SHIFT               12
#define ATC260X_WKIRQ_GPIO_DATA_WIDTH                1
#define ATC260X_WKIRQ_TYPE_MASK                 0x0C00  /* WKIRQ_TYPE [11:10] */
#define ATC260X_WKIRQ_TYPE_SHIFT                    10
#define ATC260X_WKIRQ_TYPE_WIDTH                     2
#define ATC260X_WKIRQ_32K_EN                    0x0200  /* WKIRQ_32K_EN */
#define ATC260X_WKIRQ_32K_EN_MASK               0x0200
#define ATC260X_WKIRQ_32K_EN_SHIFT                   9
#define ATC260X_WKIRQ_32K_EN_WIDTH                   1
#define ATC260X_PMU_RESERVED4_MASK              0x0100  /* RESERVED4 */
#define ATC260X_PMU_RESERVED4_SHIFT                  8
#define ATC260X_PMU_RESERVED4_WIDTH                  1
#define ATC260X_LOWP_GPIO_OUTPUT_EN             0x0080  /* LOWP_GPIO_OUTPUT_EN */
#define ATC260X_LOWP_GPIO_OUTPUT_EN_MASK        0x0080
#define ATC260X_LOWP_GPIO_OUTPUT_EN_SHIFT            7
#define ATC260X_LOWP_GPIO_OUTPUT_EN_WIDTH            1
#define ATC260X_LOWP_GPIO_INPUT_EN              0x0040  /* LOWP_GPIO_INPUT_EN */
#define ATC260X_LOWP_GPIO_INPUT_EN_MASK         0x0040
#define ATC260X_LOWP_GPIO_INPUT_EN_SHIFT             6
#define ATC260X_LOWP_GPIO_INPUT_EN_WIDTH             1
#define ATC260X_LOWP_GPIO_DATA                  0x0020  /* LOWP_GPIO_DATA */
#define ATC260X_LOWP_GPIO_DATA_MASK             0x0020
#define ATC260X_LOWP_GPIO_DATA_SHIFT                 5
#define ATC260X_LOWP_GPIO_DATA_WIDTH                 1
#define ATC260X_LOWP_32K_EN                     0x0010  /* LOWP_32K_EN */
#define ATC260X_LOWP_32K_EN_MASK                0x0010
#define ATC260X_LOWP_32K_EN_SHIFT                    4
#define ATC260X_LOWP_32K_EN_WIDTH                    1
#define ATC260X_PMU_RESERVED4_EN_MASK           0x000C  /* RESERVED5_EN [3:2] */
#define ATC260X_PMU_RESERVED4_EN_SHIFT               2
#define ATC260X_PMU_RESERVED4_EN_WIDTH               2
#define ATC260X_WKIRQ_EN                        0x0002  /* WKIRQ_EN */
#define ATC260X_WKIRQ_EN_MASK                   0x0002
#define ATC260X_WKIRQ_EN_SHIFT                       1
#define ATC260X_WKIRQ_EN_WIDTH                       1
#define ATC260X_WKIRQ_PD                        0x0001  /* WKIRQ_PD */
#define ATC260X_WKIRQ_PD_MASK                   0x0001
#define ATC260X_WKIRQ_PD_SHIFT                       0
#define ATC260X_WKIRQ_PD_WIDTH                       1

/**
 * R5 (0x05) - PMU System Control 5
 */
#define ATC260X_PMU_RESERVED5_MASK              0xF800  /* RESERVED [15:11] */
#define ATC260X_PMU_RESERVED5_SHIFT                 11
#define ATC260X_PMU_RESERVED5_WIDTH                  6
#define ATC260X_TP_DECT_EN                      0x0400  /* TP_DECT_EN */
#define ATC260X_TP_DECT_MASK                    0x0400
#define ATC260X_TP_DECT_SHIFT                       10
#define ATC260X_TP_DECT_WIDTH                        1
#define ATC260X_REMCON_DECT_EN                  0x0200  /* REMCON_DECT_EN */
#define ATC260X_REMCON_DECT_EN_MASK             0x0200
#define ATC260X_REMCON_DECT_EN_SHIFT                 9
#define ATC260X_REMCON_DECT_EN_WIDTH                 1
#define ATC260X_VBUSWKDTEN                      0x0100  /* VBUS_WK_DECT_EN */
#define ATC260X_VBUSWKDTEN_MASK                 0x0100
#define ATC260X_VBUSWKDTEN_SHIFT                     8
#define ATC260X_VBUSWKDTEN_WIDTH                     1
#define ATC260X_WALLWKDTEN                      0x0080  /* WALL_WK_DECT_EN */
#define ATC260X_WALLWKDTEN_MASK                 0x0080
#define ATC260X_WALLWKDTEN_SHIFT                     7
#define ATC260X_WALLWKDTEN_WIDTH                     1
#define ATC260X_PMU_RESERVED_5_MASK             0x007F  /* RESERVED [6:0] */
#define ATC260X_PMU_RESERVED_5_SHIFT                 0
#define ATC260X_PMU_RESERVED_5_WIDTH                 7

/**
 * R6:R9 (0x06:0x09) - PMU System Control 6-9
 *
 * PMU SYS CONTROLS R6-R9 ARE RESERVED
 */

/**
 * R10 (0x0A) - PMU Battery Control 0
 */
#define ATC260X_BAT_UV_VOL_MASK                 0xC000  /* ATC260X_BAT_UV_VOL [15:14] */
#define ATC260X_BAT_UV_VOL_SHIFT                    14
#define ATC260X_BAT_UV_VOL_WIDTH                     2
#define ATC260X_BAT_OV_VOL_MASK                 0x3000  /* BAT_OV_VOL [13:12] */
#define ATC260X_BAT_OV_VOL_SHIFT                    12
#define ATC260X_BAT_OV_VOL_WIDTH                     2
#define ATC260X_BAT_OV_SET_MASK                 0x0F00  /* BAT_OC_SET [11:8] */
#define ATC260X_BAT_OV_SET_SHIFT                     8
#define ATC260X_BAT_OV_SET_WIDTH                     4
#define ATC260X_BAT_OC_SHUTOFF_SET_MASK         0x00C0  /* BAT_OC_SHUTOFF_SET [7:6] */
#define ATC260X_BAT_OC_SHUTOFF_SET_SHIFT             6
#define ATC260X_BAT_OC_SHUTOFF_SET_WIDTH             2
#define ATC260X_BAT_RESERVED6_MASK              0X003F  /* RESERVED [5:0] */
#define ATC260X_BAT_RESERVED6_SHIFT                  0
#define ATC260X_BAT_RESERVED6_WIDTH                  6

/**
 * R11 (0x0B) - PMU Battery Control 1
 */
#define ATC260X_BAT_OC_EN                       0x8000  /* BAT_OC_EN */
#define ATC260X_BAT_OC_EN_MASK                  0x8000
#define ATC260X_BAT_OC_EN_SHIFT                     15
#define ATC260X_BAT_OC_EN_WIDTH                      1
#define ATC260X_BAT_OV_EN                       0x4000  /* BAT_OV_EN */
#define ATC260X_BAT_OV_EN_MASK                  0x4000
#define ATC260X_BAT_OV_EN_SHIFT                     14
#define ATC260X_BAT_OV_EN_WIDTH                      1
#define ATC260X_BAT_UV_EN                       0x2000  /* BAT_UV_EN */
#define ATC260X_BAT_UV_EN_MASK                  0x2000
#define ATC260X_BAT_UV_EN_SHIFT                     13
#define ATC260X_BAT_UV_EN_WIDTH                      1
#define ATC260X_BAT_OC_INT_EN                   0x1000  /* BAT_OC_INT_EN */
#define ATC260X_BAT_OC_INT_EN_MASK              0x1000
#define ATC260X_BAT_OC_INT_EN_SHIFT                 12
#define ATC260X_BAT_OC_INT_EN_WIDTH                  1
#define ATC260X_BAT_OV_INT_EN                   0x0800  /* BAT_OV_INT_EN */
#define ATC260X_BAT_OV_INT_EN_MASK              0x0800
#define ATC260X_BAT_OV_INT_EN_SHIFT                 11
#define ATC260X_BAT_OV_INT_EN_WIDTH                  1
#define ATC260X_BAT_UV_INT_EN                   0x0400  /* BAT_UV_INT_EN */
#define ATC260X_BAT_UV_INT_EN_MASK              0x0400
#define ATC260X_BAT_UV_INT_EN_SHIFT                 10
#define ATC260X_BAT_UV_INT_EN_WIDTH                  1
#define ATC260X_BAT_OC_SHUTOFF_EN               0x0200  /* BAT_OC_SHUTOFF_EN */
#define ATC260X_BAT_OC_SHUTOFF_EN_MASK          0x0200
#define ATC260X_BAT_OC_SHUTOFF_EN_SHIFT              9
#define ATC260X_BAT_OC_SHUTOFF_EN_WIDTH              1
#define ATC260X_BATOVUVOC_RESERVED_MASK         0x01FF  /* BATOVUVOC_RESERVED [8:0] */
#define ATC260X_BATOVUVOC_RESERVED_SHIFT             0
#define ATC260X_BATOVUVOC_RESERVED_WIDTH             9

/**
 * R12 (0x0C) - PMU VBUS Control 0
 */
#define ATC260X_VBUS_UV_VOL_MASK                0xC000  /* VBUS_UV_VOL [15:14] */
#define ATC260X_VBUS_UV_VOL_SHIFT                   14
#define ATC260X_VBUS_UV_VOL_WIDTH                    2
#define ATC260X_VBUS_OV_VOL_MASK                0x3000  /* VBUS_OV_VOL [13:12] */
#define ATC260X_VBUS_OV_VOL_SHIFT                   12
#define ATC260X_VBUS_OV_VOL_WIDTH                    2
#define ATC260X_VBUS_OC_SET_MASK                0x0F00  /* VBUS_OC_SET [11:8] */
#define ATC260X_VBUS_OC_SET_SHIFT                    8
#define ATC260X_VBUS_OC_SET_WIDTH                    4
#define ATC260X_VBUS_OC_SHUTOFF_SET_MASK        0x00C0  /* VBUS_OC_SHUTOFF_SET [7:6] */
#define ATC260X_VBUS_OC_SHUTOFF_SET_SHIFT            6
#define ATC260X_VBUS_OC_SHUTOFF_SET_WIDTH            2
#define ATC260X_VBUS_OC_RESERVED_MASK           0x003F  /* RESERVED [5:0] */
#define ATC260X_VBUS_OC_RESERVED_SHIFT               0
#define ATC260X_VBUS_OC_RESERVED_WIDTH               6

/**
 * R13 (0x0D) - PMU VBUS Control 1
 */
#define ATC260X_VBUS_OC_EN                      0x8000  /* VBUS_OC_EN */
#define ATC260X_VBUS_OC_EN_MASK                 0x8000
#define ATC260X_VBUS_OC_EN_SHIFT                    15
#define ATC260X_VBUS_OC_EN_WIDTH                     1
#define ATC260X_VBUS_OV_EN                      0x4000  /* VBUS_OV_EN */
#define ATC260X_VBUS_OV_EN_MASK                 0x4000
#define ATC260X_VBUS_OV_EN_SHIFT                    14
#define ATC260X_VBUS_OV_EN_WIDTH                     1
#define ATC260X_VBUS_UV_EN                      0x2000  /* VBUS_UV_EN */
#define ATC260X_VBUS_UV_EN_MASK                 0x2000
#define ATC260X_VBUS_UV_EN_SHIFT                    13
#define ATC260X_VBUS_UV_EN_WIDTH                     1
#define ATC260X_VBUS_OC_INT_EN                  0x1000  /* VBUS_OC_INT_EN */
#define ATC260X_VBUS_OC_INT_EN_MASK             0x1000
#define ATC260X_VBUS_OC_INT_EN_SHIFT                12
#define ATC260X_VBUS_OC_INT_EN_WIDTH                 1
#define ATC260X_VBUS_OV_INT_EN                  0x0800  /* VBUS_OV_INT_EN */
#define ATC260X_VBUS_OV_INT_EN_MASK             0x0800
#define ATC260X_VBUS_OV_INT_EN_SHIFT                11
#define ATC260X_VBUS_OV_INT_EN_WIDTH                 1
#define ATC260X_VBUS_UV_INT_EN                  0x0400  /* VBUS_UV_INT_EN */
#define ATC260X_VBUS_UV_INT_EN_MASK             0x0400
#define ATC260X_VBUS_UV_INT_EN_SHIFT                10
#define ATC260X_VBUS_UV_INT_EN_WIDTH                 1
#define ATC260X_VBUS_OC_SHUTOFF_EN              0x0200  /* VBUS_OC_SHUTOFF_EN */
#define ATC260X_VBUS_OC_SHUTOFF_EN_MASK         0x0200
#define ATC260X_VBUS_OC_SHUTOFF_EN_SHIFT             9
#define ATC260X_VBUS_OC_SHUTOFF_EN_WIDTH             1
#define ATC260X_VBUSOVUVOC_RESERVED_MASK        0x01FF  /* VBUSOVUVOC_RESERVED [8:0] */
#define ATC260X_VBUSOVUVOC_RESERVED_SHIFT            0
#define ATC260X_VBUSOVUVOC_RESERVED_WIDTH            9

/**
 * R14 (0x0E) - PMU WALL Control 0
 */
#define ATC260X_WALL_UV_VOL_MASK                0xC000  /* WALL_UV_VOL [15:14] */
#define ATC260X_WALL_UV_VOL_SHIFT                   14
#define ATC260X_WALL_UV_VOL_WIDTH                    2
#define ATC260X_WALL_OV_VOL_MASK                0x3000  /* WALL_OV_VOL [13:12] */
#define ATC260X_WALL_OV_VOL_SHIFT                   12
#define ATC260X_WALL_OV_VOL_WIDTH                    1
#define ATC260X_WALL_OC_SET_MASK                0x0F00  /* WALL_OC_SET [11:8] */
#define ATC260X_WALL_OC_SET_SHIFT                    8
#define ATC260X_WALL_OC_SET_WIDTH                    4
#define ATC260X_WALL_OC_SHUTOFF_SET_MASK        0x00C0  /* WALL_OC_SHUTOFF_SET [7:6] */
#define ATC260X_WALL_OC_SHUTOFF_SET_SHIFT            6
#define ATC260X_WALL_OC_SHUTOFF_SET_WIDTH            2
#define ATC260X_WALLOVUVOC_RESERVED_MASK        0x003F  /* WALLOVUVOC_RESERVED [5:0] */
#define ATC260X_WALLOVUVOC_RESERVED_SHIFT            0
#define ATC260X_WALLOVUVOC_RESERVED_WIDTH            6

/**
 * R15 (0x0F) - PMU WALL Control 1
 */
#define ATC260X_WALL_OC_EN                      0x8000  /* WALL_OC_EN */
#define ATC260X_WALL_OC_EN_MASK                 0x8000
#define ATC260X_WALL_OC_EN_SHIFT                    15
#define ATC260X_WALL_OC_EN_WIDTH                     1
#define ATC260X_WALL_OV_EN                      0x4000  /* WALL_OV_EN */
#define ATC260X_WALL_OV_EN_MASK                 0x4000
#define ATC260X_WALL_OV_EN_SHIFT                    14
#define ATC260X_WALL_OV_EN_WIDTH                     1
#define ATC260X_WALL_UV_EN                      0x2000  /* WALL_UV_EN */
#define ATC260X_WALL_UV_EN_MASK                 0x2000
#define ATC260X_WALL_UV_EN_SHIFT                    13
#define ATC260X_WALL_UV_EN_WIDTH                     1
#define ATC260X_WALL_OC_INT_EN                  0x1000  /* WALL_OC_INT_EN */
#define ATC260X_WALL_OC_INT_EN_MASK             0x1000
#define ATC260X_WALL_OC_INT_EN_SHIFT                12
#define ATC260X_WALL_OC_INT_EN_WIDTH                 1
#define ATC260X_WALL_OV_INT_EN                  0x0800  /* WALL_OV_INT_EN */
#define ATC260X_WALL_OV_INT_EN_MASK             0x0800
#define ATC260X_WALL_OV_INT_EN_SHIFT                11
#define ATC260X_WALL_OV_INT_EN_WIDTH                 1
#define ATC260X_WALL_UV_INT_EN                  0x0400  /* WALL_UV_INT_EN */
#define ATC260X_WALL_UV_INT_EN_MASK             0x0400
#define ATC260X_WALL_UV_INT_EN_SHIFT                10
#define ATC260X_WALL_UV_INT_EN_WIDTH                 1
#define ATC260X_WALL_OC_SHUTOFF_EN              0x0200  /* WALL_OC_SHUTOFF_EN */
#define ATC260X_WALL_OC_SHUTOFF_EN_MASK         0x0200
#define ATC260X_WALL_OC_SHUTOFF_EN_SHIFT             9
#define ATC260X_WALL_OC_SHUTOFF_EN_WIDTH             1
#define ATC260X_WALLOVER_RESERVED_MASK          0x01FF  /* WALLOVER_RESERVED [8:0] */
#define ATC260X_WALLOVER_RESERVED_SHIFT              0
#define ATC260X_WALLOVER_RESERVED_WIDTH              9

/**
 * R16 (0x10) - PMU System Status
 */
#define ATC260X_BAT_OV_STATUS                   0x8000  /* BAT_OV_STATUS */
#define ATC260X_BAT_OV_STATUS_MASK              0x8000
#define ATC260X_BAT_OV_STATUS_SHIFT                 15
#define ATC260X_BAT_OV_STATUS_WIDTH                  1
#define ATC260X_BAT_UV_STATUS                   0x4000  /* BAT_UV_STATUS */
#define ATC260X_BAT_UV_STATUS_MASK              0x4000
#define ATC260X_BAT_UV_STATUS_SHIFT                 14
#define ATC260X_BAT_UV_STATUS_WIDTH                  1
#define ATC260X_BAT_OC_STATUS                   0x2000  /* BAT_OC_STATUS */
#define ATC260X_BAT_OC_STATUS_MASK              0x2000
#define ATC260X_BAT_OC_STATUS_SHIFT                 13
#define ATC260X_BAT_OC_STATUS_WIDTH                  1
#define ATC260X_VBUS_OV_STATUS                  0x1000  /* VBUS_OV_STATUS */
#define ATC260X_VBUS_OV_STATUS_MASK             0x1000
#define ATC260X_VBUS_OV_STATUS_SHIFT                12
#define ATC260X_VBUS_OV_STATUS_WIDTH                 1
#define ATC260X_VBUS_UV_STATUS                  0x0800  /* VBUS_UV_STATUS */
#define ATC260X_VBUS_UV_STATUS_MASK             0x0800
#define ATC260X_VBUS_UV_STATUS_SHIFT                11
#define ATC260X_VBUS_UV_STATUS_WIDTH                 1
#define ATC260X_VBUS_OC_STATUS                  0x0400  /* VBUS_OC_STATUS */
#define ATC260X_VBUS_OC_STATUS_MASK             0x0400
#define ATC260X_VBUS_OC_STATUS_SHIFT                10
#define ATC260X_VBUS_OC_STATUS_WIDTH                 1
#define ATC260X_WALL_OV_STATUS                  0x0200  /* WALL_OV_STATUS */
#define ATC260X_WALL_OV_STATUS_MASK             0x0200
#define ATC260X_WALL_OV_STATUS_SHIFT                 9
#define ATC260X_WALL_OV_STATUS_WIDTH                 1
#define ATC260X_WALL_UV_STATUS                  0x0100  /* WALL_UV_STATUS */
#define ATC260X_WALL_UV_STATUS_MASK             0x0100
#define ATC260X_WALL_UV_STATUS_SHIFT                 8
#define ATC260X_WALL_UV_STATUS_WIDTH                 1
#define ATC260X_WALL_OC_STATUS                  0x0080  /* WALL_OC_STATUS */
#define ATC260X_WALL_OC_STATUS_MASK             0x0080
#define ATC260X_WALL_OC_STATUS_SHIFT                 7
#define ATC260X_WALL_OC_STATUS_WIDTH                 1
#define ATC260X_STATUS_RESERVED_MASK            0x007E  /* WALL_STATUS_RESERVED [6:1] */
#define ATC260X_STATUS_RESERVED_SHIFT                1
#define ATC260X_STATUS_RESERVED_WIDTH                6
#define ATC260X_STATUS_CLEAR1_MASK              0x0001  /* WALL_STATUS_RESERVED [6:0] */
#define ATC260X_STATUS_CLEAR1_SHIFT                  0  /* FLAG TO CLEAR BITS 15-7 */
#define ATC260X_STATUS_CLEAR1_WIDTH                  1

/**
 * R17 (0x11) - PMU DCDC1 Control 0
 */
#define ATC260X_DC1_RESERVED_MASK               0xF000  /* DC1_RESERVED [15:12] */
#define ATC260X_DC1_RESERVED_SHIFT                  12
#define ATC260X_DC1_RESERVED_WIDTH                   4
#define ATC260X_DC1_VOL_MASK                    0x0F80  /* DC1_VOL [11:7] */
#define ATC260X_DC1_VOL_SHIFT                        7
#define ATC260X_DC1_VOL_WIDTH                        5
#define ATC260X_DC1_CTL_RESERVED_MASK           0x007F  /* DC1_CTL_RESERVED [6:0] */
#define ATC260X_DC1_CTL_RESERVED_SHIFT               0
#define ATC260X_DC1_CTL_RESERVED_WIDTH               7

/**
 * R18 (0x12) - PMU DCDC1 Control 1
 */
#define ATC260X_DC1_RESERVED1_MASK              0xFFFF  /* DC1_RESERVED1 [15:0] */
#define ATC260X_DC1_RESERVED1_SHIFT                  0
#define ATC260X_DC1_RESERVED1_WIDTH                 16

/**
 * R19 (0x13) - PMU DCDC1 Control 2
 */
#define ATC260X_DC1_RESERVED2_MASK              0xFFFF  /* DC1_RESERVED1 [15:0] */
#define ATC260X_DC1_RESERVED2_SHIFT                  0
#define ATC260X_DC1_RESERVED2_WIDTH                 16

/**
 * R20 (0x14) - PMU DCDC2 Control 0
 */
#define ATC260X_DC2_EN                          0x8000  /* DC2_EN */
#define ATC260X_DC2_EN_MASK                     0x8000
#define ATC260X_DC2_EN_SHIFT                        15
#define ATC260X_DC2_EN_WIDTH                         1
#define ATC260X_DC2_RESERVED_MASK               0x7000  /* DC2_EN_RESERVED [14:12] */
#define ATC260X_DC2_RESERVED_SHIFT                  12
#define ATC260X_DC2_RESERVED_WIDTH                   3
#define ATC260X_DC2_VOL_MASK                    0x0F00  /* DC2_VOL [11:8] */
#define ATC260X_DC2_VOL_SHIFT                        8
#define ATC260X_DC2_VOL_WIDTH                        4

/**
 * R21 (0x15) - PMU DCDC2 Control 1
 */
#define ATC260X_DC2_RESERVED1_MASK              0xFFFF  /* DC2_RESERVED1 [15:0] */
#define ATC260X_DC2_RESERVED1_SHIFT                  0
#define ATC260X_DC2_RESERVED1_WIDTH                 16

/**
 * R22 (0x16) - PMU DCDC2 Control 2
 */
#define ATC260X_DC2_RESERVED2_MASK              0xFFFF  /* DC2_RESERVED2 [15:0] */
#define ATC260X_DC2_RESERVED2_SHIFT                  0
#define ATC260X_DC2_RESERVED2_WIDTH                 16

/**
 * R23 (0x17) - PMU DCDC3 Control 0
 */
#define ATC260X_DC3_RESERVED_MASK               0xF000  /* DC3_RESERVED [15:12] */
#define ATC260X_DC3_RESERVED_SHIFT                  12
#define ATC260X_DC3_RESERVED_WIDTH                   4
#define ATC260X_DC3_VOL_MASK                    0x0E00  /* DC3_VOL [11:9] */
#define ATC260X_DC3_VOL_SHIFT                        9
#define ATC260X_DC3_VOL_WIDTH                        3
#define ATC260X_EN_SETVCC_MASK                  0x0180  /* EN_SETVCC [8:7] */
#define ATC260X_EN_SETVCC_SHIFT                      7
#define ATC260X_EN_SETVCC_WIDTH                      2
#define ATC260X_SETVCC_RESERVED_MASK            0x007F  /* EN_SETVCC_RESERVED [6:0] */
#define ATC260X_SETVCC_RESERVED_SHIFT                0
#define ATC260X_SETVCC_RESERVED_WIDTH                7

/**
 * R24 (0x19) - PMU DCDC3 Control 1
 */
#define ATC260X_DC3_RESERVED1_MASK              0xFFFF  /* DC3_RESERVED1 [15:0] */
#define ATC260X_DC3_RESERVED1_SHIFT                  0
#define ATC260X_DC3_RESERVED1_WIDTH                 16

/**
 * R25 (0x19) - PMU DCDC3 Control 2
 */
#define ATC260X_DC3_RESERVED2_MASK              0xFFFF  /* DC3_RESERVED2 [15:0] */
#define ATC260X_DC3_RESERVED2_SHIFT                  0
#define ATC260X_DC3_RESERVED2_WIDTH                 16

/**
 * R26 (0x1A) - PMU DCDC4 Control 0
 */
#define ATC260X_DC4_RESERVED_MASK               0xFFFE  /* DC4_RESERVED [15:1] */
#define ATC260X_DC4_RESERVED_SHIFT                   1
#define ATC260X_DC4_RESERVED_WIDTH                  15
#define ATC260X_DC4_EN                          0x0001  /* DC4_EN */
#define ATC260X_DC4_EN_MASK                     0x0001
#define ATC260X_DC4_EN_SHIFT                         0
#define ATC260X_DC4_EN_WIDTH                         1

/**
 * R27 (0x1B) - PMU DCDC4 Control 1
 */
#define ATC260X_DC4_RESERVED1_MASK              0xFFFF  /* DC4_RESERVED1 [15:0] */
#define ATC260X_DC4_RESERVED1_SHIFT                  0
#define ATC260X_DC4_RESERVED1_WIDTH                 16


/* PMU LDO REGISTERS */

/**
 * R30 (0x1E) - PMU LDO1 Control
 */
#define ATC260X_LDO1_VOL_MASK                   0xE000  /* LDO1_VOL [15:13] */
#define ATC260X_LDO1_VOL_SHIFT                      13
#define ATC260X_LDO1_VOL_WIDTH                       3
#define ATC260X_LDO1_RESERVED_MASK              0x1FFF  /* LDO1_RESERVED [12:0] */
#define ATC260X_LDO1_RESERVED_SHIFT                  0
#define ATC260X_LDO1_RESERVED_WIDTH                 13

/**
 * R31 (0x1F) - PMU LDO2 Control
 */
#define ATC260X_LDO2_VOL_MASK                   0xE000  /* LDO2_VOL [15:13] */
#define ATC260X_LDO2_VOL_SHIFT                      13
#define ATC260X_LDO2_VOL_WIDTH                       3
#define ATC260X_LDO2_RESERVED_MASK              0x1FFF  /* LDO2_RESERVED [12:0] */
#define ATC260X_LDO2_RESERVED_SHIFT                  0
#define ATC260X_LDO2_RESERVED_WIDTH                 13

/**
 * R32 (0x20) - PMU LDO3 Control
 */
#define ATC260X_LDO3_VOL_MASK                   0xE000  /* LDO3_VOL [15:13] */
#define ATC260X_LDO3_VOL_SHIFT                      13
#define ATC260X_LDO3_VOL_WIDTH                       3
#define ATC260X_LDO3_RESERVED_MASK              0x1FFF  /* LDO3_RESERVED [12:0] */
#define ATC260X_LDO3_RESERVED_SHIFT                  0
#define ATC260X_LDO3_RESERVED_WIDTH                 13

/**
 * R33 (0x21) - PMU LDO4 Control
 */
#define ATC260X_LDO4_VOL_MASK                   0xE000  /* LDO4_VOL [15:13] */
#define ATC260X_LDO4_VOL_SHIFT                      13
#define ATC260X_LDO4_VOL_WIDTH                       3
#define ATC260X_LDO4_RESERVED_MASK              0x1FFE  /* LDO4_RESERVED [12:1] */
#define ATC260X_LDO4_RESERVED_SHIFT                  1
#define ATC260X_LDO4_RESERVED_WIDTH                 12
#define ATC260X_LDO4_EN                         0x0001  /* LDO4_EN */
#define ATC260X_LDO4_EN_MASK                    0x0001
#define ATC260X_LDO4_EN_SHIFT                        0
#define ATC260X_LDO4_EN_WIDTH                        1

/**
 * R34 (0x22) - PMU LDO4 Control
 */
#define ATC260X_LDO5_VOL_MASK                   0xE000  /* LDO5_VOL [15:13] */
#define ATC260X_LDO5_VOL_SHIFT                      13
#define ATC260X_LDO5_VOL_WIDTH                       3
#define ATC260X_LDO5_RESERVED_MASK              0x1FFE  /* LDO5_RESERVED [12:1] */
#define ATC260X_LDO5_RESERVED_SHIFT                  1
#define ATC260X_LDO5_RESERVED_WIDTH                 12
#define ATC260X_LDO5_EN                         0x0001  /* LDO5_EN */
#define ATC260X_LDO5_EN_MASK                    0x0001
#define ATC260X_LDO5_EN_SHIFT                        0
#define ATC260X_LDO5_EN_WIDTH                        1

/**
 * R35 (0x23) - PMU LDO6 Control
 */
#define ATC260X_LDO6_VOL_MASK                   0xF800  /* LDO6_VOL [15:11] */
#define ATC260X_LDO6_VOL_SHIFT                      11
#define ATC260X_LDO6_VOL_WIDTH                       5
#define ATC260X_LDO6_RESERVED_MASK              0x07FF  /* LDO6_RESERVED [10:0] */
#define ATC260X_LDO6_RESERVED_SHIFT                  0
#define ATC260X_LDO6_RESERVED_WIDTH                 11

/**
 * R36 (0x24) - PMU LDO7 Control
 */
#define ATC260X_LDO7_VOL_MASK                   0xE000  /* LDO7_VOL [15:13] */
#define ATC260X_LDO7_VOL_SHIFT                      13
#define ATC260X_LDO7_VOL_WIDTH                       3
#define ATC260X_LDO7_RESERVED_MASK              0x1FFE  /* LDO7_RESERVED [12:1] */
#define ATC260X_LDO7_RESERVED_SHIFT                  1
#define ATC260X_LDO7_RESERVED_WIDTH                 12
#define ATC260X_LDO7_EN                         0x0001  /* LDO7_EN */
#define ATC260X_LDO7_EN_MASK                    0x0001
#define ATC260X_LDO7_EN_SHIFT                        0
#define ATC260X_LDO7_EN_WIDTH                        1

/**
 * R37 (0x25) - PMU LDO8 Control
 */
#define ATC260X_LDO8_VOL_MASK                   0xF000  /* LDO8_VOL [15:12] */
#define ATC260X_LDO8_VOL_SHIFT                      12
#define ATC260X_LDO8_VOL_WIDTH                       4
#define ATC260X_LDO8_RESERVED_MASK              0x0FFE  /* LDO8_RESERVED [11:1] */
#define ATC260X_LDO8_RESERVED_SHIFT                  1
#define ATC260X_LDO8_RESERVED_WIDTH                 11
#define ATC260X_LDO8_EN                         0x0001  /* LDO8_EN */
#define ATC260X_LDO8_EN_MASK                    0x0001
#define ATC260X_LDO8_EN_SHIFT                        0
#define ATC260X_LDO8_EN_WIDTH                        1

/**
 * R38 (0x26) - PMU LDO9 Control
 */
#define ATC260X_LDO9_VOL_MASK                   0xE000  /* LDO9_VOL [15:13] */
#define ATC260X_LDO9_VOL_SHIFT                      13
#define ATC260X_LDO9_VOL_WIDTH                       3
#define ATC260X_LDO9_RESERVED_MASK              0x1FFE  /* LDO9_RESERVED [12:1] */
#define ATC260X_LDO9_RESERVED_SHIFT                  1
#define ATC260X_LDO9_RESERVED_WIDTH                 12
#define ATC260X_LDO9_EN                         0x0001  /* LDO9_EN */
#define ATC260X_LDO9_EN_MASK                    0x0001
#define ATC260X_LDO9_EN_SHIFT                        0
#define ATC260X_LDO9_EN_WIDTH                        1

/**
 * R39 (0x27) - PMU LDO10 Control
 */
#define ATC260X_LDO10_VOL_MASK                   0xF000  /* LDO10_VOL [15:12] */
#define ATC260X_LDO10_VOL_SHIFT                      12
#define ATC260X_LDO10_VOL_WIDTH                       4
#define ATC260X_LDO10_RESERVED_MASK              0x0FFE  /* LDO10_RESERVED [11:1] */
#define ATC260X_LDO10_RESERVED_SHIFT                  1
#define ATC260X_LDO10_RESERVED_WIDTH                 11
#define ATC260X_LDO10_EN                         0x0001  /* LDO10_EN */
#define ATC260X_LDO10_EN_MASK                    0x0001
#define ATC260X_LDO10_EN_SHIFT                        0
#define ATC260X_LDO10_EN_WIDTH                        1

/**
 * R40 (0x28) - PMU LDO11 Control
 */
#define ATC260X_LDO11_VOL_MASK                   0xE000  /* LDO11_VOL [15:13] */
#define ATC260X_LDO11_VOL_SHIFT                      13
#define ATC260X_LDO11_VOL_WIDTH                       3
#define ATC260X_SVCC_LOW_EN                      0x1000  /* SVCC_LOW_EN */
#define ATC260X_SVCC_LOW_EN_MASK                 0x1000
#define ATC260X_SVCC_LOW_EN_SHIFT                    12
#define ATC260X_SVCC_LOW_EN_WIDTH                     1
#define ATC260X_LDO11_RESERVED_MASK              0x0FFF  /* LDO11_RESERVED [11:0] */
#define ATC260X_LDO11_RESERVED_SHIFT                  0
#define ATC260X_LDO11_RESERVED_WIDTH                 12


/******************************************************/
/* THESE VOLTAGE TABLES WILL BE MOVED AS SOON AS THEY */
/* HAVE SOMEWHERE TO GO... FOR NOW, CONSIDER THESE    */
/*               A TEMPORARY REFERENCE                */ 
/******************************************************/

/**
 * DC1 VOLTAGE BIT SETTINGS
 * Range: 0.7v-1.4v
 * MinStep: 0.025v
 */
#define ATC260X_DC1_VSEL_0700            (0 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.700V - 00000 */
#define ATC260X_DC1_VSEL_0725            (1 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.725V - 00001 */
#define ATC260X_DC1_VSEL_0750            (2 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.750V - 00010 */
#define ATC260X_DC1_VSEL_0775            (3 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.775V - 00011 */
#define ATC260X_DC1_VSEL_0800            (4 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.800V - 00100 */
#define ATC260X_DC1_VSEL_0825            (5 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.825V - 00101 */
#define ATC260X_DC1_VSEL_0850            (6 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.850V - 00110 */
#define ATC260X_DC1_VSEL_0875            (7 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.875V - 00111 */
#define ATC260X_DC1_VSEL_0900            (8 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.900V - 01000 */
#define ATC260X_DC1_VSEL_0925            (9 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.925V - 01001 */
#define ATC260X_DC1_VSEL_0950           (10 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.950V - 01010 */
#define ATC260X_DC1_VSEL_0975           (11 << ATC260X_DC1_VOL_SHIFT)    /* SET 0.975V - 01011 */
#define ATC260X_DC1_VSEL_1000           (12 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.000V - 01100 */
#define ATC260X_DC1_VSEL_1025           (13 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.025V - 01101 */
#define ATC260X_DC1_VSEL_1050           (14 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.050V - 01110 */
#define ATC260X_DC1_VSEL_1075           (15 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.075V - 01111 */
#define ATC260X_DC1_VSEL_1100           (16 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.100V - 10000 */
#define ATC260X_DC1_VSEL_1125           (17 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.125V - 10001 */
#define ATC260X_DC1_VSEL_1150           (18 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.150V - 10010 */
#define ATC260X_DC1_VSEL_1175           (19 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.175V - 10011 */
#define ATC260X_DC1_VSEL_1200           (20 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.200V - 10100 */
#define ATC260X_DC1_VSEL_1225           (21 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.225V - 10101 */
#define ATC260X_DC1_VSEL_1250           (22 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.250V - 10110 */
#define ATC260X_DC1_VSEL_1275           (23 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.275V - 10111 */
#define ATC260X_DC1_VSEL_1300           (24 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.300V - 11000 */
#define ATC260X_DC1_VSEL_1325           (25 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.325V - 11001 */
#define ATC260X_DC1_VSEL_1350           (26 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.350V - 11010 */
#define ATC260X_DC1_VSEL_1375           (27 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.375V - 11011 */
#define ATC260X_DC1_VSEL_1400           (28 << ATC260X_DC1_VOL_SHIFT)    /* SET 1.400V - 11100 */

/**
 * DC2 VOLTAGE BIT SETTINGS
 * Range: 1.30v-2.15v
 * MinStep: 0.05v
 */
#define ATC260X_DC2_VSEL_13             (0 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.30V - 0000 */
#define ATC260X_DC2_VSEL_135            (1 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.35V - 0001 */
#define ATC260X_DC2_VSEL_14             (2 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.40V - 0010 */
#define ATC260X_DC2_VSEL_145            (3 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.45V - 0011 */
#define ATC260X_DC2_VSEL_15             (4 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.50V - 0100 */
#define ATC260X_DC2_VSEL_155            (5 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.55V - 0101 */
#define ATC260X_DC2_VSEL_16             (6 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.60V - 0110 */
#define ATC260X_DC2_VSEL_165            (7 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.65V - 0111 */
#define ATC260X_DC2_VSEL_17             (8 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.70V - 1000 */
#define ATC260X_DC2_VSEL_175            (9 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.75V - 1001 */
#define ATC260X_DC2_VSEL_18            (10 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.80V - 1010 */
#define ATC260X_DC2_VSEL_185           (11 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.85V - 1011 */
#define ATC260X_DC2_VSEL_19            (12 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.90V - 1100 */
#define ATC260X_DC2_VSEL_195           (13 << ATC260X_DC2_VOL_SHIFT)    /* SET 1.95V - 1101 */
#define ATC260X_DC2_VSEL_205           (14 << ATC260X_DC2_VOL_SHIFT)    /* SET 2.05V - 1110 */
#define ATC260X_DC2_VSEL_215           (15 << ATC260X_DC2_VOL_SHIFT)    /* SET 2.15V - 1111 */

/**
 * DC3 VOLTAGE BIT SETTINGS
 * Range: 2.6v-3.3v
 * MinStep: 0.10v
 */
#define ATC260X_DC3_VSEL_26             (0 << ATC260X_DC3_VOL_SHIFT)    /* SET 2.6V - 000 */
#define ATC260X_DC3_VSEL_27             (1 << ATC260X_DC3_VOL_SHIFT)    /* SET 2.7V - 001 */
#define ATC260X_DC3_VSEL_28             (2 << ATC260X_DC3_VOL_SHIFT)    /* SET 2.8V - 010 */
#define ATC260X_DC3_VSEL_29             (3 << ATC260X_DC3_VOL_SHIFT)    /* SET 2.9V - 011 */
#define ATC260X_DC3_VSEL_30             (4 << ATC260X_DC3_VOL_SHIFT)    /* SET 3.0V - 100 */
#define ATC260X_DC3_VSEL_31             (5 << ATC260X_DC3_VOL_SHIFT)    /* SET 3.1V - 101 */
#define ATC260X_DC3_VSEL_32             (6 << ATC260X_DC3_VOL_SHIFT)    /* SET 3.2V - 110 */
#define ATC260X_DC3_VSEL_33             (7 << ATC260X_DC3_VOL_SHIFT)    /* SET 3.3V - 111 */

/* POWER MODE SELECT */
#define ATC260X_PMODE_LDO               (0)     /* LDO POWER MODE - 00 */
#define ATC260X_PMODE_DCDC              (1)     /* DCDC POWER MODE - 01 */
#define ATC260X_PMODE_RESET             (2)     /* RESET TO LDO - 1X (10 or 11) */

/**
 * LDO1 VOL BITS
 * Range: 2.6v-3.2v
 * MinStep: 0.10v
 */
#define ATC260X_LDO1_VSEL_26             (0 << ATC260X_LDO1_VOL_SHIFT)    /* 2.6V - 000 */
#define ATC260X_LDO1_VSEL_27             (1 << ATC260X_LDO1_VOL_SHIFT)    /* 2.7V - 001 */
#define ATC260X_LDO1_VSEL_28             (2 << ATC260X_LDO1_VOL_SHIFT)    /* 2.8V - 010 */
#define ATC260X_LDO1_VSEL_29             (3 << ATC260X_LDO1_VOL_SHIFT)    /* 2.9V - 011 */
#define ATC260X_LDO1_VSEL_30             (4 << ATC260X_LDO1_VOL_SHIFT)    /* 3.0V - 100 */
#define ATC260X_LDO1_VSEL_31             (5 << ATC260X_LDO1_VOL_SHIFT)    /* 3.1V - 101 */
#define ATC260X_LDO1_VSEL_32             (6 << ATC260X_LDO1_VOL_SHIFT)    /* 3.2V - 110 */

/**
 * LDO2 VOL BITS
 * Range: 2.6v-3.3v
 * MinStep: 0.10v
 */
#define ATC260X_LDO2_VSEL_26             (0 << ATC260X_LDO2_VOL_SHIFT)    /* 2.6V - 000 */
#define ATC260X_LDO2_VSEL_27             (1 << ATC260X_LDO2_VOL_SHIFT)    /* 2.7V - 001 */
#define ATC260X_LDO2_VSEL_28             (2 << ATC260X_LDO2_VOL_SHIFT)    /* 2.8V - 010 */
#define ATC260X_LDO2_VSEL_29             (3 << ATC260X_LDO2_VOL_SHIFT)    /* 2.9V - 011 */
#define ATC260X_LDO2_VSEL_30             (4 << ATC260X_LDO2_VOL_SHIFT)    /* 3.0V - 100 */
#define ATC260X_LDO2_VSEL_31             (5 << ATC260X_LDO2_VOL_SHIFT)    /* 3.1V - 101 */
#define ATC260X_LDO2_VSEL_32             (6 << ATC260X_LDO2_VOL_SHIFT)    /* 3.2V - 110 */
#define ATC260X_LDO2_VSEL_33             (7 << ATC260X_LDO2_VOL_SHIFT)    /* 3.3V - 111 */

/**
 * LDO3 VOL BITS
 * Range: 1.5v-2.0v
 * MinStep: 0.10v
 */
#define ATC260X_LDO3_VSEL_15             (0 << ATC260X_LDO3_VOL_SHIFT)    /* 1.5V - 000 */
#define ATC260X_LDO3_VSEL_16             (1 << ATC260X_LDO3_VOL_SHIFT)    /* 1.6V - 001 */
#define ATC260X_LDO3_VSEL_17             (2 << ATC260X_LDO3_VOL_SHIFT)    /* 1.7V - 010 */
#define ATC260X_LDO3_VSEL_18             (3 << ATC260X_LDO3_VOL_SHIFT)    /* 1.8V - 011 */
#define ATC260X_LDO3_VSEL_19             (4 << ATC260X_LDO3_VOL_SHIFT)    /* 1.9V - 100 */
#define ATC260X_LDO3_VSEL_20             (5 << ATC260X_LDO3_VOL_SHIFT)    /* 2.0V - 101 */

/**
 * LDO4 VOL BITS
 * Range: 2.8v-3.5v
 * MinStep: 0.10v
 */
#define ATC260X_LDO4_VSEL_28             (0 << ATC260X_LDO4_VOL_SHIFT)    /* 2.8V - 000 */
#define ATC260X_LDO4_VSEL_29             (1 << ATC260X_LDO4_VOL_SHIFT)    /* 2.9V - 001 */
#define ATC260X_LDO4_VSEL_30             (2 << ATC260X_LDO4_VOL_SHIFT)    /* 3.0V - 010 */
#define ATC260X_LDO4_VSEL_31             (3 << ATC260X_LDO4_VOL_SHIFT)    /* 3.1V - 011 */
#define ATC260X_LDO4_VSEL_32             (4 << ATC260X_LDO4_VOL_SHIFT)    /* 3.2V - 100 */
#define ATC260X_LDO4_VSEL_33             (5 << ATC260X_LDO4_VOL_SHIFT)    /* 3.3V - 101 */
#define ATC260X_LDO4_VSEL_34             (6 << ATC260X_LDO4_VOL_SHIFT)    /* 3.4V - 110 */
#define ATC260X_LDO4_VSEL_35             (7 << ATC260X_LDO4_VOL_SHIFT)    /* 3.5V - 111 */

/**
 * LDO5 VOL BITS
 * Range: 2.6v-3.3v
 * MinStep: 0.10v
 */
#define ATC260X_LDO5_VSEL_26             (0 << ATC260X_LDO5_VOL_SHIFT)    /* 2.6V - 000 */
#define ATC260X_LDO5_VSEL_27             (1 << ATC260X_LDO5_VOL_SHIFT)    /* 2.7V - 001 */
#define ATC260X_LDO5_VSEL_28             (2 << ATC260X_LDO5_VOL_SHIFT)    /* 2.8V - 010 */
#define ATC260X_LDO5_VSEL_29             (3 << ATC260X_LDO5_VOL_SHIFT)    /* 2.9V - 011 */
#define ATC260X_LDO5_VSEL_30             (4 << ATC260X_LDO5_VOL_SHIFT)    /* 3.0V - 100 */
#define ATC260X_LDO5_VSEL_31             (5 << ATC260X_LDO5_VOL_SHIFT)    /* 3.1V - 101 */
#define ATC260X_LDO5_VSEL_32             (6 << ATC260X_LDO5_VOL_SHIFT)    /* 3.2V - 110 */
#define ATC260X_LDO5_VSEL_33             (7 << ATC260X_LDO5_VOL_SHIFT)    /* 3.3V - 111 */

/**
 * LDO6 VOL BITS
 * Range: 0.7v-1.4v
 * MinStep: 0.025v
 */
#define ATC260X_LDO6_VSEL_0700              (0 << ATC260X_LDO6_VOL_SHIFT)    /* 0.700V - 00000 */
#define ATC260X_LDO6_VSEL_0725              (1 << ATC260X_LDO6_VOL_SHIFT)    /* 0.725V - 00001 */
#define ATC260X_LDO6_VSEL_0750              (2 << ATC260X_LDO6_VOL_SHIFT)    /* 0.750V - 00010 */
#define ATC260X_LDO6_VSEL_0775              (3 << ATC260X_LDO6_VOL_SHIFT)    /* 0.775V - 00011 */
#define ATC260X_LDO6_VSEL_0800              (4 << ATC260X_LDO6_VOL_SHIFT)    /* 0.800V - 00100 */
#define ATC260X_LDO6_VSEL_0825              (5 << ATC260X_LDO6_VOL_SHIFT)    /* 0.825V - 00101 */
#define ATC260X_LDO6_VSEL_0850              (6 << ATC260X_LDO6_VOL_SHIFT)    /* 0.850V - 00110 */
#define ATC260X_LDO6_VSEL_0875              (7 << ATC260X_LDO6_VOL_SHIFT)    /* 0.875V - 00111 */
#define ATC260X_LDO6_VSEL_0900              (8 << ATC260X_LDO6_VOL_SHIFT)    /* 0.900V - 01000 */
#define ATC260X_LDO6_VSEL_0925              (9 << ATC260X_LDO6_VOL_SHIFT)    /* 0.925V - 01001 */
#define ATC260X_LDO6_VSEL_0950             (10 << ATC260X_LDO6_VOL_SHIFT)    /* 0.950V - 01010 */
#define ATC260X_LDO6_VSEL_0975             (11 << ATC260X_LDO6_VOL_SHIFT)    /* 0.975V - 01011 */
#define ATC260X_LDO6_VSEL_1000             (12 << ATC260X_LDO6_VOL_SHIFT)    /* 1.000V - 01100 */
#define ATC260X_LDO6_VSEL_1025             (13 << ATC260X_LDO6_VOL_SHIFT)    /* 1.025V - 01101 */
#define ATC260X_LDO6_VSEL_1050             (14 << ATC260X_LDO6_VOL_SHIFT)    /* 1.050V - 01110 */
#define ATC260X_LDO6_VSEL_1075             (15 << ATC260X_LDO6_VOL_SHIFT)    /* 1.075V - 01111 */
#define ATC260X_LDO6_VSEL_1100             (16 << ATC260X_LDO6_VOL_SHIFT)    /* 1.100V - 10000 */
#define ATC260X_LDO6_VSEL_1125             (17 << ATC260X_LDO6_VOL_SHIFT)    /* 1.125V - 10001 */
#define ATC260X_LDO6_VSEL_1150             (18 << ATC260X_LDO6_VOL_SHIFT)    /* 1.150V - 10010 */
#define ATC260X_LDO6_VSEL_1175             (19 << ATC260X_LDO6_VOL_SHIFT)    /* 1.175V - 10011 */
#define ATC260X_LDO6_VSEL_1200             (20 << ATC260X_LDO6_VOL_SHIFT)    /* 1.200V - 10100 */
#define ATC260X_LDO6_VSEL_1225             (21 << ATC260X_LDO6_VOL_SHIFT)    /* 1.225V - 10101 */
#define ATC260X_LDO6_VSEL_1250             (22 << ATC260X_LDO6_VOL_SHIFT)    /* 1.250V - 10110 */
#define ATC260X_LDO6_VSEL_1275             (23 << ATC260X_LDO6_VOL_SHIFT)    /* 1.275V - 10111 */
#define ATC260X_LDO6_VSEL_1300             (24 << ATC260X_LDO6_VOL_SHIFT)    /* 1.300V - 11000 */
#define ATC260X_LDO6_VSEL_1325             (25 << ATC260X_LDO6_VOL_SHIFT)    /* 1.325V - 11001 */
#define ATC260X_LDO6_VSEL_1350             (26 << ATC260X_LDO6_VOL_SHIFT)    /* 1.350V - 11010 */
#define ATC260X_LDO6_VSEL_1375             (27 << ATC260X_LDO6_VOL_SHIFT)    /* 1.375V - 11011 */
#define ATC260X_LDO6_VSEL_1400             (28 << ATC260X_LDO6_VOL_SHIFT)    /* 1.400V - 11100 */

/**
 * LDO7 VOL BITS
 * Range: 1.5v-2.0v
 * MinStep: 0.10v
 */
#define ATC260X_LDO7_VSEL_15             (0 << ATC260X_LDO7_VOL_SHIFT)    /* 1.5V - 000 */
#define ATC260X_LDO7_VSEL_16             (1 << ATC260X_LDO7_VOL_SHIFT)    /* 1.6V - 001 */
#define ATC260X_LDO7_VSEL_17             (2 << ATC260X_LDO7_VOL_SHIFT)    /* 1.7V - 010 */
#define ATC260X_LDO7_VSEL_18             (3 << ATC260X_LDO7_VOL_SHIFT)    /* 1.8V - 011 */
#define ATC260X_LDO7_VSEL_19             (4 << ATC260X_LDO7_VOL_SHIFT)    /* 1.9V - 100 */
#define ATC260X_LDO7_VSEL_20             (5 << ATC260X_LDO7_VOL_SHIFT)    /* 2.0V - 101 */

/**
 * LDO8 VOL BITS
 * Range: 2.3v-3.3v
 * MinStep: 0.10v
 */
#define ATC260X_LDO8_VSEL_23              (0 << ATC260X_LDO8_VOL_SHIFT)    /* 2.3V - 0000 */
#define ATC260X_LDO8_VSEL_24              (1 << ATC260X_LDO8_VOL_SHIFT)    /* 2.4V - 0001 */
#define ATC260X_LDO8_VSEL_25              (2 << ATC260X_LDO8_VOL_SHIFT)    /* 2.5V - 0010 */
#define ATC260X_LDO8_VSEL_26              (3 << ATC260X_LDO8_VOL_SHIFT)    /* 2.6V - 0011 */
#define ATC260X_LDO8_VSEL_27              (4 << ATC260X_LDO8_VOL_SHIFT)    /* 2.7V - 0100 */
#define ATC260X_LDO8_VSEL_28              (5 << ATC260X_LDO8_VOL_SHIFT)    /* 2.8V - 0101 */
#define ATC260X_LDO8_VSEL_29              (6 << ATC260X_LDO8_VOL_SHIFT)    /* 2.9V - 0110 */
#define ATC260X_LDO8_VSEL_30              (7 << ATC260X_LDO8_VOL_SHIFT)    /* 3.0V - 0111 */
#define ATC260X_LDO8_VSEL_31              (8 << ATC260X_LDO8_VOL_SHIFT)    /* 3.1V - 1000 */
#define ATC260X_LDO8_VSEL_32              (9 << ATC260X_LDO8_VOL_SHIFT)    /* 3.2V - 1001 */
#define ATC260X_LDO8_VSEL_33             (10 << ATC260X_LDO8_VOL_SHIFT)    /* 3.3V - 1010 */

/**
 * LDO9 VOL BITS
 * Range: 1.0v-1.5v
 * MinStep: 0.10v
 */
#define ATC260X_LDO9_VSEL_10             (0 << ATC260X_LDO9_VOL_SHIFT)    /* 1.0V - 000 */
#define ATC260X_LDO9_VSEL_11             (1 << ATC260X_LDO9_VOL_SHIFT)    /* 1.1V - 001 */
#define ATC260X_LDO9_VSEL_12             (2 << ATC260X_LDO9_VOL_SHIFT)    /* 1.2V - 010 */
#define ATC260X_LDO9_VSEL_13             (3 << ATC260X_LDO9_VOL_SHIFT)    /* 1.3V - 011 */
#define ATC260X_LDO9_VSEL_14             (4 << ATC260X_LDO9_VOL_SHIFT)    /* 1.4V - 100 */
#define ATC260X_LDO9_VSEL_15             (5 << ATC260X_LDO9_VOL_SHIFT)    /* 1.5V - 101 */

/**
 * LDO10 VOL BITS
 * Range: 2.3v-3.3v
 * MinStep: 0.10v
 */
#define ATC260X_LDO10_VSEL_23              (0 << ATC260X_LDO10_VOL_SHIFT)    /* 2.3V - 0000 */
#define ATC260X_LDO10_VSEL_24              (1 << ATC260X_LDO10_VOL_SHIFT)    /* 2.4V - 0001 */
#define ATC260X_LDO10_VSEL_25              (2 << ATC260X_LDO10_VOL_SHIFT)    /* 2.5V - 0010 */
#define ATC260X_LDO10_VSEL_26              (3 << ATC260X_LDO10_VOL_SHIFT)    /* 2.6V - 0011 */
#define ATC260X_LDO10_VSEL_27              (4 << ATC260X_LDO10_VOL_SHIFT)    /* 2.7V - 0100 */
#define ATC260X_LDO10_VSEL_28              (5 << ATC260X_LDO10_VOL_SHIFT)    /* 2.8V - 0101 */
#define ATC260X_LDO10_VSEL_29              (6 << ATC260X_LDO10_VOL_SHIFT)    /* 2.9V - 0110 */
#define ATC260X_LDO10_VSEL_30              (7 << ATC260X_LDO10_VOL_SHIFT)    /* 3.0V - 0111 */
#define ATC260X_LDO10_VSEL_31              (8 << ATC260X_LDO10_VOL_SHIFT)    /* 3.1V - 1000 */
#define ATC260X_LDO10_VSEL_32              (9 << ATC260X_LDO10_VOL_SHIFT)    /* 3.2V - 1001 */
#define ATC260X_LDO10_VSEL_33             (10 << ATC260X_LDO10_VOL_SHIFT)    /* 3.3V - 1010 */

/**
 * LDO5 VOL BITS
 * Range: 2.6v-3.3v
 * MinStep: 0.10v
 */
#define ATC260X_LDO11_VSEL_26             (0 << ATC260X_LDO11_VOL_SHIFT)    /* 2.6V - 000 */
#define ATC260X_LDO11_VSEL_27             (1 << ATC260X_LDO11_VOL_SHIFT)    /* 2.7V - 001 */
#define ATC260X_LDO11_VSEL_28             (2 << ATC260X_LDO11_VOL_SHIFT)    /* 2.8V - 010 */
#define ATC260X_LDO11_VSEL_29             (3 << ATC260X_LDO11_VOL_SHIFT)    /* 2.9V - 011 */
#define ATC260X_LDO11_VSEL_30             (4 << ATC260X_LDO11_VOL_SHIFT)    /* 3.0V - 100 */
#define ATC260X_LDO11_VSEL_31             (5 << ATC260X_LDO11_VOL_SHIFT)    /* 3.1V - 101 */
#define ATC260X_LDO11_VSEL_32             (6 << ATC260X_LDO11_VOL_SHIFT)    /* 3.2V - 110 */
#define ATC260X_LDO11_VSEL_33             (7 << ATC260X_LDO11_VOL_SHIFT)    /* 3.3V - 111 */











/* ATC260X device */
struct atc260x_dev;

/*
 * ATC260X register interface
 */
int atc260x_reg_read(struct atc260x_dev *atc260x, unsigned short reg);
int atc260x_reg_write(struct atc260x_dev *atc260x, unsigned short reg,
         unsigned short val);
int atc260x_set_bits(struct atc260x_dev *atc260x, unsigned short reg,
            unsigned short mask, unsigned short val);

/* 
 * ATC260X SPI interface access mode
 */
#define ATC260X_SPI_ACCESS_MODE_NORMAL      (0)     /* access by standard SPI driver interface */
#define ATC260X_SPI_ACCESS_MODE_DIRECT      (1)     /* access SPI controller directly */
int atc260x_set_access_mode(struct atc260x *atc260x, int mode);

/* ATC260X AuxADC interface */
enum atc260x_auxadc {
        ATC260X_AUX_IREF     = 0,
        ATC260X_AUX_CHGI     = 1,
        ATC260X_AUX_VBUSI    = 2,
        ATC260X_AUX_WALLI    = 3,
        ATC260X_AUX_BATI     = 4,
        ATC260X_AUX_REMCON   = 5,
        ATC260X_AUX_TEMP     = 6,
        ATC260X_AUX_BATV     = 7,
        ATC260X_AUX_BAKBATV  = 8,
        ATC260X_AUX_SYSPWRV  = 9,
        ATC260X_AUX_WALLV    = 10,
        ATC260X_AUX_VBUSV    = 11,
        ATC260X_AUX_AUX3     = 12,
        ATC260X_AUX_AUX2     = 13,
        ATC260X_AUX_AUX1     = 14,
        ATC260X_AUX_AUX0     = 15,
};

int atc260x_auxadc_reg_read(struct atc260x *atc260x, enum atc260x_auxadc input);
int atc260x_auxadc_read(struct atc260x *atc260x, enum atc260x_auxadc input);

/* ATC260X chip version */ 
enum ATC260X_CHIP_VERSION {
        ATC260X_VER_A = 0x0000,
        ATC260X_VER_B = 0x0001,
        ATC260X_VER_C = 0x0002,
};

int atc260x_get_version(void);

/* ATC260X CMU interface */ 
#define ATC260X_CMU_MODULE_TP            (0)
#define ATC260X_CMU_MODULE_MFP           (1)
#define ATC260X_CMU_MODULE_INTS          (2)
#define ATC260X_CMU_MODULE_ETHPHY        (3)
#define ATC260X_CMU_MODULE_AUDIO         (4)
#define ATC260X_CMU_MODULE_NUM           (5) // CMU module count

int atc260x_cmu_reset(struct atc260x *atc260x, int cmu_module);
int atc260x_cmu_enable(struct atc260x *atc260x, int cmu_module);
int atc260x_cmu_disable(struct atc260x *atc260x, int cmu_module);

/* ATC260X MFD interface */ 
enum atc260x_mfp_mod_id {
        MOD_ID_RMII,
        MOD_ID_SMII,
        MOD_ID_REMCON,
        MOD_ID_TP,
        MOD_ID_LED0,
        MOD_ID_LED1,
};

#define  ATC260X_MFP_OPT_CAN_SLEEP       (0)
#define  ATC260X_MFP_OPT_CANNOT_SLEEP    (1)

int atc260x_mfp_lock (enum atc260x_mfp_mod_id mod_id, int opt, struct device *dev);
int atc260x_mfp_locked (enum atc260x_mfp_mod_id mod_id, int opt);
int atc260x_mfp_unlock (enum atc260x_mfp_mod_id mod_id, int opt);

/* ATC260X IRQ interface */
#define ATC260X_IRQ_AUDIO                (0)
#define ATC260X_IRQ_TP                   (1)
#define ATC260X_IRQ_ETHERNET             (2)
#define ATC260X_IRQ_OV                   (3)
#define ATC260X_IRQ_OC                   (4)
#define ATC260X_IRQ_OT                   (5)
#define ATC260X_IRQ_UV                   (6)
#define ATC260X_IRQ_ALARM                (7)
#define ATC260X_IRQ_ONOFF                (8)
#define ATC260X_IRQ_WKUP                 (9)
#define ATC260X_IRQ_IR                   (10)
/* internal IRQ source count */
#define ATC260X_IRQ_NUM                  (11)

/* ATC260X internal GPIO count */ 
#define ATC260X_GPIO_NUM                 (32)        /* internal GPIO count */

/* ATC260X Power Management */ 
#define POWER_MODE_WORKING              (0)     /* S1 */
#define POWER_MODE_STANDBY              (1)     /* S2 */
#define POWER_MODE_SLEEP                (2)     /* S3 */
#define POWER_MODE_DEEP_SLEEP           (3)     /* S4 */

/* Wakeup Sources */
#define WAKEUP_SRC_IR                   (1 << 0)
#define WAKEUP_SRC_RESET                (1 << 1)
#define WAKEUP_SRC_HDSW                 (1 << 2)
#define WAKEUP_SRC_ALARM                (1 << 3)
#define WAKEUP_SRC_REMCON               (1 << 4)
#define WAKEUP_SRC_TP                   (1 << 5)
#define WAKEUP_SRC_WKIRQ                (1 << 6)
#define WAKEUP_SRC_ONOFF_SHORT          (1 << 7)
#define WAKEUP_SRC_ONOFF_LONG           (1 << 8)
#define WAKEUP_SRC_WALL_IN              (1 << 9)
#define WAKEUP_SRC_VBUS_IN              (1 << 10)
#define WAKEUP_SRC_ALL                  (0x7ff)

struct pmic_suspend_ops {
        int (*set_wakeup_src)(int wakeup_mask, int wakeup_src);
    int (*get_wakeup_src)(void);
    int (*get_wakeup_flag)(void);       /* wakeup reason flag */
    void (*prepare)(int mode);
    void (*enter)(int mode);
    void (*wake)(void);
    void (*finish)(void);
};

/* gl5302_PMU_SYS_CTL3 FW reserved flag */
#define PMU_SYS_CTL3_FW_FLAG_S2         (1 << 4)    /* suspend to S2 flag */

/* ATC260X misc interface */ 
int atc260x_enable_vbusotg(int on);

#endif /* __ATC260X_H__ */

