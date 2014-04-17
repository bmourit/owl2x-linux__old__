/*
 * include/linux/mfd/atc260x/rtc.h - rtc for ATX260X
 *
 * Copyright 2014 B. Mouritsen
 * Author: B. Mouritsen <bnmguy@gmail.com>
 *
 * Some data from original header provided by Actions
 * Copyright 2011 Actions Semiconductors, Inc.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#ifndef __LINUX_MFD_ATC260X_RTC_H
#define __LINUX_MFD_ATC260X_RTC_H

#include <linux/platform_device.h>

/* RTC REGISTER VALUES
 * OFFSETs FROM BASE */
#define ATC260X_RTC_CONTROL             0x52
#define ATC260X_RTC_ALARM1              0x53    /* ALARM MINS SECS */
#define ATC260X_RTC_ALARM2              0x54    /* ALARM HOUR */
#define ATC260X_RTC_ALARM3              0x55    /* ALARM MONTH YEAR */
#define ATC260X_RTC_TIME1               0x56    /* MINS SECS */
#define ATC260X_RTC_TIME2               0x57    /* HOUR */
#define ATC260X_RTC_TIME3               0x58    /* DAY CENTURY */
#define ATC260X_RTC_TIME4               0x59    /* YEAR MONTH DATE */

/**
 * (0x052) - RTC Control Bits
 */
#define ATC260X_RTC_LGS_MASK            0xC000  /* RTC_LGS - [15:14] */
#define ATC260X_RTC_LGS_SHIFT               14  /* LOSC drv strength */
#define ATC260X_RTC_LGS_WIDTH                2
#define ATC260X_RTC_LOSC_CP_MASK        0x3000  /* RTC_LOSC_CP - [13:12] */
#define ATC260X_RTC_LOSC_CP_SHIFT           12  /* LOSC Capacitor Select */
#define ATC260X_RTC_LOSC_CP_WIDTH            2
#define ATC260X_RTC_RESET               0x0800  /* RTC_RESET */
#define ATC260X_RTC_RESET_MASK          0x0800  /* 0 TO RESET */
#define ATC260X_RTC_RESET_SHIFT             11
#define ATC260X_RTC_RESET_WIDTH              1
#define ATC260X_RTC_VERI                0x0400  /* RTC_VERI */
#define ATC260X_RTC_VERI_MASK           0x0400  /* Verify RTC 32k clk is enabled */
#define ATC260X_RTC_VERI_SHIFT              10
#define ATC260X_RTC_VERI_WIDTH               1
#define ATC260X_RTC_LEAP                0x0200  /* RTC_ALEAP */
#define ATC260X_RTC_LEAP_MASK           0x0200  /* Leap Year Bit */
#define ATC260X_RTC_LEAP_SHIFT               9
#define ATC260X_RTC_LEAP_WIDTH               1
#define ATC260X_RTC_TEST_MASK           0x0180  /* RTC_TEST - [8:7] */
#define ATC260X_RTC_TEST_SHIFT               7
#define ATC260X_RTC_TEST_WIDTH               2
#define ATC260X_RTC_EOSC                0x0040  /* RTC_EOSC */
#define ATC260X_RTC_EOSC_MASK           0x0040  /* Enable external 32k crystal */
#define ATC260X_RTC_EOSC_SHIFT               6
#define ATC260X_RTC_EOSC_WIDTH               1
#define ATC260X_RTC_CKSS0               0x0020  /* RTC_CKSS0 */
#define ATC260X_RTC_CKSS0_MASK          0x0020  /* Clk source select */
#define ATC260X_RTC_CKSS0_SHIFT              5  /* 1 for external, 0 for built-in */
#define ATC260X_RTC_CKSS0_WIDTH              1
#define ATC260X_RTC_RTCE                0x0010  /* RTC_RTCE */
#define ATC260X_RTC_RTCE_MASK           0x0010  /* Stop RTC */
#define ATC260X_RTC_RTCE_SHIFT               4
#define ATC260X_RTC_RTCE_WIDTH               1
#define ATC260X_RTC_EXT_LOSC_STATE      0x0008  /* RTC_EXT_LOSC_STATE */
#define ATC260X_RTC_EXT_LOSC_STATE_MASK 0x0008  /* External clk state */
#define ATC260X_RTC_EXT_LOSC_STATE_SHIFT     3
#define ATC260X_RTC_EXT_LOSC_STATE_WIDTH     1
#define ATC260X_RTC_RESERVED0           0x0004  /* RTC_RESERVED */
#define ATC260X_RTC_RESERVED0_MASK      0x0004
#define ATC260X_RTC_RESERVED0_SHIFT          2
#define ATC260X_RTC_RESERVED0_WIDTH          1
#define ATC260X_RTC_ALIE                0x0002  /* RTC_ALIE */
#define ATC260X_RTC_ALIE_MASK           0x0002  /* Alarm IRQ enable bit */
#define ATC260X_RTC_ALIE_SHIFT               1
#define ATC260X_RTC_ALIE_WIDTH               1
#define ATC260X_RTC_ALIP                0x0001  /* RTC_ALIP */
#define ATC260X_RTC_ALIP_MASK           0x0001  /* Alarm IRQ pending bit */
#define ATC260X_RTC_ALIP_SHIFT               0
#define ATC260X_RTC_ALIP_WIDTH               1

/**
 * (0x053) - RTC Min/Sec Alarm Control
 */
#define ATC260X_RTC_RESERVED1_MASK      0xF000  /* RTC_RESERVED1 - [15:12] */
#define ATC260X_RTC_RESERVED1_SHIFT         12
#define ATC260X_RTC_RESERVED1_WIDTH          4
#define ATC260X_RTC_MINAL_MASK          0x0FC0  /* RTC_MINAL - [11:6] */
#define ATC260X_RTC_MINAL_SHIFT              6
#define ATC260X_RTC_MINAL_WIDTH              6
#define ATC260X_RTC_SECAL_MASK          0x003F  /* RTC_SECAL - [5:0] */
#define ATC260X_RTC_SECAL_SHIFT              0
#define ATC260X_RTC_SECAL_WIDTH              6

/* Alarm Min/Sec Bit Values */
#define ATM260X_RTC_MINAL_DONT_CARE     -1
#define ATM260X_RTC_SECAL_DONT_CARE     -1

/**
 * (0x054) - RTC Hour Alarm Control
 */
#define ATC260X_RTC_RESERVED2_MASK      0xFFE0  /* RTC_RESERVED2 - [15:5] */
#define ATC260X_RTC_RESERVED2_SHIFT          5
#define ATC260X_RTC_RESERVED2_WIDTH          7 
#define ATC260X_RTC_HOURAL_MASK         0x001F  /* RTC_HOURAL - [4:0] */
#define ATC260X_RTC_HOURAL_SHIFT             0
#define ATC260X_RTC_HOURAL_WIDTH             5

/* Alarm AM/PM Bit Values */
#define ATM260X_RTC_HOURAL_DONT_CARE     -1

/**
 * (0x055) - RTC Y/M/D Alarm Control
 */
#define ATC260X_RTC_YEARAL_MASK         0xFE00  /* RTC_YEARAL - [15:9] */
#define ATC260X_RTC_YEARAL_SHIFT             9
#define ATC260X_RTC_YEARAL_WIDTH             7
#define ATC260X_RTC_MONAL_MASK          0x01E0  /* RTC_MONAL - [8:5] */
#define ATC260X_RTC_MONAL_SHIFT              5
#define ATC260X_RTC_MONAL_WIDTH              4
#define ATC260X_RTC_DATEAL_MASK         0x001F  /* RTC_DATEAL - [4:0] */
#define ATC260X_RTC_DATEAL_SHIFT             0
#define ATC260X_RTC_DATEAL_WIDTH             5

/**
 * (0x056) - RTC Min/Sec Time 1
 */
#define ATC260X_RTC_RESERVED3_MASK      0xF000  /* RTC_RESERVED3 - [15:12] */
#define ATC260X_RTC_RESERVED3_SHIFT         12
#define ATC260X_RTC_RESERVED3_WIDTH          4
#define ATC260X_RTC_MIN_MASK            0x0FC0  /* RTC_MIN - [11:6] */
#define ATC260X_RTC_MIN_SHIFT                6
#define ATC260X_RTC_MIN_WIDTH                6
#define ATC260X_RTC_SEC_MASK            0x003F  /* RTC_SEC - [5:0] */
#define ATC260X_RTC_SEC_SHIFT                0
#define ATC260X_RTC_SEC_WIDTH                6

/**
 * (0x057) - RTC Hour Time 2
 */
#define ATC260X_RTC_RESERVED4_MASK      0xFFE0  /* RTC_RESERVED4 - [15:5] */
#define ATC260X_RTC_RESERVED4_SHIFT          5
#define ATC260X_RTC_RESERVED4_WIDTH         11 
#define ATC260X_RTC_HOUR_MASK           0x001F  /* RTC_HOUR - [4:0] */
#define ATC260X_RTC_HOUR_SHIFT               0
#define ATC260X_RTC_HOUR_WIDTH               5

/**
 * (0x058) - RTC Day/Cent Time 3
 */
#define ATC260X_RTC_RESERVED5_MASK      0xFC00  /* RTC_RESERVED5 - [15:10] */
#define ATC260X_RTC_RESERVED5_SHIFT         10
#define ATC260X_RTC_RESERVED5_WIDTH          6
#define ATC260X_RTC_DAY_MASK            0x0380  /* RTC_DAY - [9:7] */
#define ATC260X_RTC_DAY_SHIFT                7
#define ATC260X_RTC_DAY_WIDTH                3
#define ATC260X_RTC_CENT_MASK           0x007F  /* RTC_CENT - [6:0] */
#define ATC260X_RTC_CENT_SHIFT               0
#define ATC260X_RTC_CENT_WIDTH               7

/* Bit Values For Day (0x058) */
/*#define ATC260X_RTC_DAY_SUN             1
#define ATC260X_RTC_DAY_MON             2
#define ATC260X_RTC_DAY_TUE             3
#define ATC260X_RTC_DAY_WED             4
#define ATC260X_RTC_DAY_THU             5
#define ATC260X_RTC_DAY_FRI             6
#define ATC260X_RTC_DAY_SAT             7
*/
/**
 * (0x059) - RTC Y/M/D Time 4
 */
#define ATC260X_RTC_YEAR_MASK           0xFE00  /* RTC_YEAR - [15:9] */
#define ATC260X_RTC_YEAR_SHIFT               9
#define ATC260X_RTC_YEAR_WIDTH               7
#define ATC260X_RTC_MON_MASK            0x01E0  /* RTC_MON - [8:5] */
#define ATC260X_RTC_MON_SHIFT                5
#define ATC260X_RTC_MON_WIDTH                4
#define ATC260X_RTC_DATE_MASK           0x001F  /* RTC_DATE - [4:0] */
#define ATC260X_RTC_DATE_SHIFT               0
#define ATC260X_RTC_DATE_WIDTH               5

/* Bit values for Month (0x059) */
#define ATC260X_RTC_MON_JAN             1
#define ATC260X_RTC_MON_FEB             2
#define ATC260X_RTC_MON_MAR             3
#define ATC260X_RTC_MON_APR             4
#define ATC260X_RTC_MON_MAY             5
#define ATC260X_RTC_MON_JUN             6
#define ATC260X_RTC_MON_JUL             7
#define ATC260X_RTC_MON_AUG             8
#define ATC260X_RTC_MON_SEP             9
#define ATC260X_RTC_MON_OCT             10
#define ATC260X_RTC_MON_NOV             11
#define ATC260X_RTC_MON_DEC             12
#define ATC260X_RTC_MON_JAN_BCD         0x01
#define ATC260X_RTC_MON_FEB_BCD         0x02
#define ATC260X_RTC_MON_MAR_BCD         0x03
#define ATC260X_RTC_MON_APR_BCD         0x04
#define ATC260X_RTC_MON_MAY_BCD         0x05
#define ATC260X_RTC_MON_JUN_BCD         0x06
#define ATC260X_RTC_MON_JUL_BCD         0x07
#define ATC260X_RTC_MON_AUG_BCD         0x08
#define ATC260X_RTC_MON_SEP_BCD         0x09
#define ATC260X_RTC_MON_OCT_BCD         0x10
#define ATC260X_RTC_MON_NOV_BCD         0x11
#define ATC260X_RTC_MON_DEC_BCD         0x12


#endif /* __LINUX_MFD_ATC260X_RTC_H */
