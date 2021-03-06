/*
 * Auxiliary ADC interface for ATC260x PMIC
 *
 * Copyright 2014
 * Author: B. Mouritsen <bnmguy@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __MFD_ATC260X_AUXADC_H__
#define __MFD_ATC260X_AUXADC_H__

struct atc260x;

/* (0x3E) - AuxADC Control 0 */
#define ATC260X_AUXADC0_EN                      0x8000  /* AUXADC0_EN */
#define ATC260X_AUXADC0_EN_MASK                 0x8000  /* AUXADC0_EN */
#define ATC260X_AUXADC0_EN_SHIFT                    15
#define ATC260X_AUXADC0_EN_WIDTH                     1
#define ATC260X_AUXADC1_EN                      0x4000  /* AUXADC01_EN */
#define ATC260X_AUXADC1_EN_MASK                 0x4000
#define ATC260X_AUXADC1_EN_SHIFT                    14
#define ATC260X_AUXADC1_EN_WIDTH                     1
#define ATC260X_AUXADC2_EN                      0x2000  /* AUXADC2_EN */
#define ATC260X_AUXADC2_EN_MASK                 0x2000
#define ATC260X_AUXADC2_EN_SHIFT                    13
#define ATC260X_AUXADC2_EN_WIDTH                     1
#define ATC260X_AUXADC3_EN                      0x1000  /* AUXADC3_EN */
#define ATC260X_AUXADC3_EN_MASK                 0x1000
#define ATC260X_AUXADC3_EN_SHIFT                    12
#define ATC260X_AUXADC3_EN_WIDTH                     1
#define ATC260X_VBUSV_ADC_EN                    0x0800  /* VBUSVADC_EN */
#define ATC260X_VBUSV_ADC_EN_MASK               0x0800
#define ATC260X_VBUSV_ADC_EN_SHIFT                  11
#define ATC260X_VBUSV_ADC_EN_WIDTH                   1
#define ATC260X_WALLV_ADC_EN                    0x0400  /* WALLVADC_EN */
#define ATC260X_WALLV_ADC_EN_MASK               0x0400
#define ATC260X_WALLV_ADC_EN_SHIFT                  10
#define ATC260X_WALLV_ADC_EN_WIDTH                   1
#define ATC260X_SYSTEM_POWER_ADC_EN             0x0200  /* SYSTEM_POWER_ADC_EN */
#define ATC260X_SYSTEM_POWER_ADC_EN_MASK        0x0200
#define ATC260X_SYSTEM_POWER_ADC_EN_SHIFT            9
#define ATC260X_SYSTEM_POWER_ADC_EN_WIDTH            1
#define ATC260X_BACKUP_BATTERY_ADC_EN           0x0100  /* BACKUP_BATTERY_ADC_EN */
#define ATC260X_BACKUP_BATTERY_ADC_EN_MASK      0x0100
#define ATC260X_BACKUP_BATTERY_ADC_EN_SHIFT          8
#define ATC260X_BACKUP_BATTERY_ADC_EN_WIDTH          1
#define ATC260X_BATTERYV_ADC_EN                 0x0080  /* BATTERYV_ADC_EN */
#define ATC260X_BATTERYV_ADC_EN_MASK            0x0080
#define ATC260X_BATTERYV_ADC_EN_SHIFT                7
#define ATC260X_BATTERYV_ADC_EN_WIDTH                1
#define ATC260X_ICTEMP_ADC_EN                   0x0040  /* TEMP_ADC_EN */
#define ATC260X_ICTEMP_ADC_EN_MASK              0x0040
#define ATC260X_ICTEMP_ADC_EN_SHIFT                  6
#define ATC260X_ICTEMP_ADC_EN_WIDTH                  1
#define ATC260X_REMCON_ADC_EN                   0x0020  /* REMCON_ADC_EN */
#define ATC260X_REMCON_ADC_EN_MASK              0x0020
#define ATC260X_REMCON_ADC_EN_SHIFT                  5
#define ATC260X_REMCON_ADC_EN_WIDTH                  1
#define ATC260X_BATTERYI_ADC_EN                 0x0010  /* BATTERYI_ADC_EN */
#define ATC260X_BATTERYI_ADC_EN_MASK            0x0010
#define ATC260X_BATTERYI_ADC_EN_SHIFT                4
#define ATC260X_BATTERYI_ADC_EN_WIDTH                1
#define ATC260X_WALLI_ADC_EN                    0x0008  /* WALLI_ADC_EN */
#define ATC260X_WALLI_ADC_EN_MASK               0x0008
#define ATC260X_WALLI_ADC_EN_SHIFT                   3
#define ATC260X_WALLI_ADC_EN_WIDTH                   1
#define ATC260X_VBUSI_ADC_EN                    0x0004  /* VBUSI_ADC_EN */
#define ATC260X_VBUSI_ADC_EN_MASK               0x0004
#define ATC260X_VBUSI_ADC_EN_SHIFT                   2
#define ATC260X_VBUSI_ADC_EN_WIDTH                   1
#define ATC260X_CHGI_ADC_EN                     0x0002  /* CHGI_ADC_EN */
#define ATC260X_CHGI_ADC_EN_MASK                0x0002
#define ATC260X_CHGI_ADC_EN_SHIFT                    1
#define ATC260X_CHGI_ADC_EN_WIDTH                    1
#define ATC260X_SVCC_ADC_EN                     0x0001  /* SVCC_ADC_EN (IREF_ADC_EN) */
#define ATC260X_SVCC_ADC_EN_MASK                0x0001
#define ATC260X_SVCC_ADC_EN_SHIFT                    0
#define ATC260X_SVCC_ADC_EN_WIDTH                    1

/* (0x3F) - AuxADC CONTROL 1 */
//#define ATC260X_AUXADC_RESERVED_MASK            0xFFF0  /* AUXADC_RESERVED [15:4] */
//#define ATC260X_AUXADC_RESERVED_SHIFT                4
//#define ATC260X_AUXADC_RESERVED_WIDTH               12
#define ATC260X_ADC_COMP_TM_EN                  0x0008  /* ADC_COMP_TM_EN */
#define ATC260X_ADC_COMP_TM_EN_MASK             0x0008
#define ATC260X_ADC_COMP_TM_EN_SHIFT                 3
#define ATC260X_ADC_COMP_TM_EN_WIDTH                 1
#define ATC260X_ADC_COMP_BIAS                   0x0004  /* ADC_COMP_BIAS */
#define ATC260X_ADC_COMP_BIAS_MASK              0x0004
#define ATC260X_ADC_COMP_BIAS_SHIFT                  2
#define ATC260X_ADC_COMP_BIAS_WIDTH                  1
#define ATC260X_ADC_INPUT_RANGE                 0x0002  /* ADC_INPUT_RANGE */
#define ATC260X_ADC_INPUT_RANGE_MASK            0x0002
#define ATC260X_ADC_INPUT_RANGE_SHIFT                1
#define ATC260X_ADC_INPUT_RANGE_WIDTH                1
#define ATC260X_ADC_CLOCK_ADJ                   0x0001  /* ADC_CLOCK_ADJ */
#define ATC260X_ADC_CLOCK_ADJ_MASK              0x0001
#define ATC260X_ADC_CLOCK_ADJ_SHIFT                  0
#define ATC260X_ADC_CLOCK_ADJ_WIDTH                  1

enum atc260x_auxadc {
	ATC260X_AUX_SVCC     = 0,       /* both SVCC and IREF */
	ATC260X_AUX_CHGI     = 1,
	ATC260X_AUX_VBUSI    = 2,
	ATC260X_AUX_WALLI    = 3,
	ATC260X_AUX_BATI     = 4,
	ATC260X_AUX_REMCON   = 5,
	ATC260X_AUX_ICTEMP   = 6,
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

int atc260x_auxadc_read(struct atc260x *atc260x, enum atc260x_auxadc input);
int atc260x_auxadc_read_reg(struct atc260x *atc260x, enum atc260x_auxadc input);

#endif
