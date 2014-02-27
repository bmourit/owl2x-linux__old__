/*
 * Reg definitions for Actions ATC260X integrated pmu/audio codec chips
 *
 * Copyright (C) 2013
 *
 * ATC260X Spec Version_V1.0
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __MFD_ATC260X_REG_H___
#define __MFD_ATC260X_REG_H___

/* PMU */
#define	PMU_BASE			0x0000
#define	ATC260X_PMU_SYS_CTL0		(PMU_BASE+0x00)
#define	ATC260X_PMU_SYS_CTL1		(PMU_BASE+0x01)
#define	ATC260X_PMU_SYS_CTL2		(PMU_BASE+0x02)
#define	ATC260X_PMU_SYS_CTL3		(PMU_BASE+0x03)
#define	ATC260X_PMU_SYS_CTL4		(PMU_BASE+0x04)
#define	ATC260X_PMU_SYS_CTL5		(PMU_BASE+0x05)
#define	ATC260X_PMU_SYS_CTL6		(PMU_BASE+0x06)
#define	ATC260X_PMU_SYS_CTL7		(PMU_BASE+0x07)
#define	ATC260X_PMU_SYS_CTL8		(PMU_BASE+0x08)
#define	ATC260X_PMU_SYS_CTL9		(PMU_BASE+0x09)
#define	ATC260X_PMU_BAT_CTL0		(PMU_BASE+0x0A)
#define	ATC260X_PMU_BAT_CTL1		(PMU_BASE+0x0B)
#define	ATC260X_PMU_VBUS_CTL0		(PMU_BASE+0x0C)
#define	ATC260X_PMU_VBUS_CTL1		(PMU_BASE+0x0D)
#define	ATC260X_PMU_WALL_CTL0		(PMU_BASE+0x0E)
#define	ATC260X_PMU_WALL_CTL1		(PMU_BASE+0x0F)
#define	ATC260X_PMU_SYS_Pending		(PMU_BASE+0x10)

/**
 * PMU BUCK MODE
 * Provides: MasterCore
 * DCDC1: InputRange = 3.0-5.5v
 * 	  OutputRange = 0.7-1.4v
 * 	  AdjustableIncrement = 25mv
 * 	  CurrentLimit = 1.2A
 */
#define	ATC260X_PMU_DC1_CTL0		(PMU_BASE+0x11)
#define	ATC260X_PMU_DC1_CTL1		(PMU_BASE+0x12)
#define	ATC260X_PMU_DC1_CTL2		(PMU_BASE+0x13)

/**
 * PMU BUCK MODE
 * Provides: DDR
 * DCDC2: InputRange = 3.0-5.5v
 * 	  OutputRange = 1.3-2.2v
 * 	  AdjustableIncrement = 50mv
 * 	  CurrentLimit = 1.0A
 */
#define	ATC260X_PMU_DC2_CTL0		(PMU_BASE+0x14)
#define	ATC260X_PMU_DC2_CTL1		(PMU_BASE+0x15)
#define	ATC260X_PMU_DC2_CTL2		(PMU_BASE+0x16)

/**
 * PMU BUCK MODE
 * Provides: MasterControls and PMU/Codec IO
 * DCDC3: InputRange = 3.0-5.5v
 * 	  OutputRange = 2.6-3.3v
 * 	  AdjustableIncrement = 100mv
 * 	  CurrentLimit = 1.0A
 */
#define	ATC260X_PMU_DC3_CTL0		(PMU_BASE+0x17)
#define	ATC260X_PMU_DC3_CTL1		(PMU_BASE+0x18)
#define	ATC260X_PMU_DC3_CTL2		(PMU_BASE+0x19)

/**
 * PMU BOOST MODE
 * Provides: USB/HDMI (need switch)
 * DCDC4: InputRange = 3.0-5.5v
 * 	  OutputCurrent = 5.5v
 * 	  CurrentLimit = 800mA
 */
#define	ATC260X_PMU_DC4_CTL0		(PMU_BASE+0x1A)
#define	ATC260X_PMU_DC4_CTL1		(PMU_BASE+0x1B)

// FIXME: Do we even use these? Are they reserved?
#define	ATC260X_PMU_DC5_CTL0		(PMU_BASE+0x1C)
#define	ATC260X_PMU_DC5_CTL1		(PMU_BASE+0x1D)

/**
 * PMU LDO
 *	InputMode, OutputRange, Step, CurrentLimit, Provides
 *
 * LDO1: SYSPWR, 2.6-3.3v, 100mv, 400mA, MasterControlAVCC
 * LDO2: SYSPWR, 2.6-3.3v, 100mv, 200mA, Audio and Ethernet
 * LDO3: SYSPWR, 1.5-2.0v, 100mv, 250mA, PMU DigitalSensors
 * LDO4: SYSPWR, 2.8-3.5v, 100mv, 400mA, Wifi and Bluetooth
 * LDO5: SYSPWR, 2.6-3.3v, 100mv, 150mA, Sensor(2.8v)
 */
#define	ATC260X_PMU_LDO1_CTL		(PMU_BASE+0x1E)
#define	ATC260X_PMU_LDO2_CTL		(PMU_BASE+0x1F)
#define	ATC260X_PMU_LDO3_CTL		(PMU_BASE+0x20)
#define	ATC260X_PMU_LDO4_CTL		(PMU_BASE+0x21)
#define	ATC260X_PMU_LDO5_CTL		(PMU_BASE+0x22)

/**
 * PMU LDO
 *	InputMode, OutputRange, Step, CurrentLimit, Provides
 *
 * LDO6: SYSPWR or DC3OUT, 0.7-1.4v, 25mv,  200mA, SATA and MIPI
 * LDO7: SYSPWR or DC3OUT, 1.5-2.0v, 100mv, 200mA, Wifi(1.8v)
 * LDO8: SYSPWR, ------- , 2.3-3.3v, 100mv, 150mA, FM and GPS
 * LDO9: SYSPWR or DC3OUT, 1.0-1.5v, 100mv, 150mA, Wifi(1.2v)
 * LDO10: -------------- , 2.3-3.3v, 100mv, 100mA, SATA(2.5v)
 * LDO11: SYSPWR, ------ , 2.6-3.3v, 100mv, 15mA, SVCC (ATC260X IO)
 * LDO12: Used by RTC in standby mode (RTCVDD)
 */
#define	ATC260X_PMU_LDO6_CTL		(PMU_BASE+0x23)
#define	ATC260X_PMU_LDO7_CTL		(PMU_BASE+0x24)
#define	ATC260X_PMU_LDO8_CTL		(PMU_BASE+0x25)
#define	ATC260X_PMU_LDO9_CTL		(PMU_BASE+0x26)
#define	ATC260X_PMU_LDO10_CTL		(PMU_BASE+0x27)
#define	ATC260X_PMU_LDO11_CTL		(PMU_BASE+0x28)

/**
 * Two switches and two available modes: LDO or SWITCH 
 * CurrentLimits: 400mA, 100mA 
 */
#define	ATC260X_PMU_SWITCH_CTL		(PMU_BASE+0x29)

/* Over Voltage */
#define	ATC260X_PMU_OV_CTL0 		(PMU_BASE+0x2A)
#define	ATC260X_PMU_OV_CTL1 		(PMU_BASE+0x2B)
#define	ATC260X_PMU_OV_Status		(PMU_BASE+0x2C)
#define	ATC260X_PMU_OV_EN 		(PMU_BASE+0x2D)
#define	ATC260X_PMU_OV_INT_EN		(PMU_BASE+0x2E)
#define	ATC260X_PMU_OC_CTL 		(PMU_BASE+0x2F)
#define	ATC260X_PMU_OC_Status		(PMU_BASE+0x30)
#define	ATC260X_PMU_OC_EN 		(PMU_BASE+0x31)
#define	ATC260X_PMU_OC_INT_EN		(PMU_BASE+0x32)
/* Under Voltage */
#define	ATC260X_PMU_UV_CTL0 		(PMU_BASE+0x33)
#define	ATC260X_PMU_UV_CTL1 		(PMU_BASE+0x34)
#define	ATC260X_PMU_UV_Status   	(PMU_BASE+0x35)
#define	ATC260X_PMU_UV_EN 		(PMU_BASE+0x36)
#define	ATC260X_PMU_UV_INT_EN   	(PMU_BASE+0x37)
/* Over Temperature */
#define	ATC260X_PMU_OT_CTL 		(PMU_BASE+0x38)
/* Charger */
#define	ATC260X_PMU_CHARGER_CTL0   	(PMU_BASE+0x39)
#define	ATC260X_PMU_CHARGER_CTL1   	(PMU_BASE+0x3A)
#define	ATC260X_PMU_CHARGER_CTL2   	(PMU_BASE+0x3B)
#define	ATC260X_PMU_BakCHARGER_CTL   	(PMU_BASE+0x3C)
#define	ATC260X_PMU_APDS_CTL 		(PMU_BASE+0x3D)

#define	ATC260X_PMU_AuxADC_CTL0   	(PMU_BASE+0x3E)
#define	ATC260X_PMU_AuxADC_CTL1   	(PMU_BASE+0x3F)
#define	ATC260X_PMU_BATVADC 		(PMU_BASE+0x40)
#define	ATC260X_PMU_BATIADC 		(PMU_BASE+0x41)
#define	ATC260X_PMU_WALLVADC 		(PMU_BASE+0x42)
#define	ATC260X_PMU_WALLIADC 		(PMU_BASE+0x43)
#define	ATC260X_PMU_VBUSVADC 		(PMU_BASE+0x44)
#define	ATC260X_PMU_VBUSIADC 		(PMU_BASE+0x45)
#define	ATC260X_PMU_SYSPWRADC		(PMU_BASE+0x46)
#define	ATC260X_PMU_RemConADC		(PMU_BASE+0x47)
#define	ATC260X_PMU_SVCCADC		(PMU_BASE+0x48)
#define	ATC260X_PMU_CHGIADC		(PMU_BASE+0x49)
#define	ATC260X_PMU_IREFADC		(PMU_BASE+0x4A)
#define	ATC260X_PMU_BAKBATADC		(PMU_BASE+0x4B)
#define	ATC260X_PMU_ICTEMPADC		(PMU_BASE+0x4C)
#define	ATC260X_PMU_AuxADC0		(PMU_BASE+0x4D)
#define	ATC260X_PMU_AuxADC1		(PMU_BASE+0x4E)
#define	ATC260X_PMU_AuxADC2		(PMU_BASE+0x4F)
#define	ATC260X_PMU_AuxADC3		(PMU_BASE+0x50)
#define	ATC260X_PMU_BDG_CTL		(PMU_BASE+0x51)

#define	ATC260X_RTC_CTL			(PMU_BASE+0x52)
#define	ATC260X_RTC_MSALM		(PMU_BASE+0x53)
#define	ATC260X_RTC_HALM		(PMU_BASE+0x54)
#define	ATC260X_RTC_YMDALM		(PMU_BASE+0x55)
#define	ATC260X_RTC_MS			(PMU_BASE+0x56)
#define	ATC260X_RTC_H			(PMU_BASE+0x57)
#define	ATC260X_RTC_DC			(PMU_BASE+0x58)
#define	ATC260X_RTC_YMD			(PMU_BASE+0x59)

#define	ATC260X_EFUSE_DAT		(PMU_BASE+0x5A)
#define	ATC260X_EFUSECRTL1		(PMU_BASE+0x5B)
#define	ATC260X_EFUSECRTL2		(PMU_BASE+0x5C)

#define	ATC260X_IRC_CTL			(PMU_BASE+0x60)
#define	ATC260X_IRC_STAT		(PMU_BASE+0x61)
#define	ATC260X_IRC_CC 			(PMU_BASE+0x62)
#define	ATC260X_IRC_KDC 		(PMU_BASE+0x63)
#define	ATC260X_IRC_WK 			(PMU_BASE+0x64)

/* AUDIO_IN_OUT_Register */
#define	AUDIO_IN_OUT_Register_BASE 	0x0400
#define	ATC260X_AUDIOINOUT_CTL 		(AUDIO_IN_OUT_Register_BASE+0x000)
#define	ATC260X_AUDIO_DEBUGOUTCTL 	(AUDIO_IN_OUT_Register_BASE+0x001)
#define	ATC260X_DAC_FILTERCTL0 		(AUDIO_IN_OUT_Register_BASE+0x002)
#define	ATC260X_DAC_FILTERCTL1 		(AUDIO_IN_OUT_Register_BASE+0x003)
#define	ATC260X_DAC_DIGITALCTL 		(AUDIO_IN_OUT_Register_BASE+0x004)
#define	ATC260X_DAC_VOLUMECTL0		(AUDIO_IN_OUT_Register_BASE+0x005)
#define	ATC260X_DAC_VOLUMECTL1		(AUDIO_IN_OUT_Register_BASE+0x006)
#define	ATC260X_DAC_VOLUMECTL2		(AUDIO_IN_OUT_Register_BASE+0x007)
#define	ATC260X_DAC_VOLUMECTL3		(AUDIO_IN_OUT_Register_BASE+0x008)
#define	ATC260X_DAC_ANANLOG0		(AUDIO_IN_OUT_Register_BASE+0x009)
#define	ATC260X_DAC_ANANLOG1		(AUDIO_IN_OUT_Register_BASE+0x00a)
#define	ATC260X_DAC_ANANLOG2		(AUDIO_IN_OUT_Register_BASE+0x00b)
#define	ATC260X_DAC_ANANLOG3		(AUDIO_IN_OUT_Register_BASE+0x00c)
#define	ATC260X_DAC_ANANLOG4		(AUDIO_IN_OUT_Register_BASE+0x00d)
#define	ATC260X_CLASSD_CTL0		(AUDIO_IN_OUT_Register_BASE+0x00e)
#define	ATC260X_CLASSD_CTL1		(AUDIO_IN_OUT_Register_BASE+0x00f)
#define	ATC260X_CLASSD_CTL2		(AUDIO_IN_OUT_Register_BASE+0x010)

/* AUDIO_IN_OUT_Register */
#define	AUDIO_IN_OUT_Register_BASE 	0x0400
#define	ATC260X_ADC0_DIGITALCTL		(AUDIO_IN_OUT_Register_BASE+0x011)
#define	ATC260X_ADC0_HPFCTL		(AUDIO_IN_OUT_Register_BASE+0x012)
#define	ATC260X_ADC0_CTL		(AUDIO_IN_OUT_Register_BASE+0x013)
#define	ATC260X_AGC0_CTL0		(AUDIO_IN_OUT_Register_BASE+0x014)
#define	ATC260X_AGC0_CTL1		(AUDIO_IN_OUT_Register_BASE+0x015)
#define	ATC260X_AGC0_CTL2		(AUDIO_IN_OUT_Register_BASE+0x016)
#define	ATC260X_ADC_ANANLOG0		(AUDIO_IN_OUT_Register_BASE+0x017)
#define	ATC260X_ADC_ANANLOG1		(AUDIO_IN_OUT_Register_BASE+0x018)
#define	ATC260X_ADC1_DIGITALCTL 	(AUDIO_IN_OUT_Register_BASE+0x019)
#define	ATC260X_ADC1_CTL		(AUDIO_IN_OUT_Register_BASE+0x01A)
#define	ATC260X_AGC1_CTL0		(AUDIO_IN_OUT_Register_BASE+0x01B)
#define	ATC260X_AGC1_CTL1		(AUDIO_IN_OUT_Register_BASE+0x01C)
#define	ATC260X_AGC1_CTL2		(AUDIO_IN_OUT_Register_BASE+0x01D)

/* Ethernet_PHY_Register */
#define	Ethernet_PHY_Register_BASE 	0x0500
#define	ATC260X_PHY_SMI_STAT 		(Ethernet_PHY_Register_BASE+0x000)
#define	ATC260X_PHY_SMI_CONFIG 		(Ethernet_PHY_Register_BASE+0x001)
#define	ATC260X_PHY_SMI_DATA 		(Ethernet_PHY_Register_BASE+0x002)
#define	ATC260X_PHY_CTRL		(Ethernet_PHY_Register_BASE+0x003)
#define	ATC260X_PHY_ID1			(Ethernet_PHY_Register_BASE+0x004)
#define	ATC260X_PHY_ID2			(Ethernet_PHY_Register_BASE+0x005)
#define	ATC260X_PHY_ADDR		(Ethernet_PHY_Register_BASE+0x006)
#define	ATC260X_PHY_LED			(Ethernet_PHY_Register_BASE+0x007)
#define	ATC260X_PHY_INT_CTRL 		(Ethernet_PHY_Register_BASE+0x008)
#define	ATC260X_PHY_INT_STAT 		(Ethernet_PHY_Register_BASE+0x009)
#define	ATC260X_PHY_HW_RST		(Ethernet_PHY_Register_BASE+0x00A)
#define ATC260X_PHY_CONFIG		(Ethernet_PHY_Register_BASE+0x00B)
#define ATC260X_PHY_PLL_CTL0		(Ethernet_PHY_Register_BASE+0x00C)
#define ATC260X_PHY_PLL_CTL1		(Ethernet_PHY_Register_BASE+0x00D)
#define ATC260X_PHY_DBG0		(Ethernet_PHY_Register_BASE+0x00E)
#define ATC260X_PHY_DBG1		(Ethernet_PHY_Register_BASE+0x00F)
#define ATC260X_PHY_DBG2		(Ethernet_PHY_Register_BASE+0x010)
#define ATC260X_PHY_DBG3		(Ethernet_PHY_Register_BASE+0x011)
#define ATC260X_PHY_DBG4		(Ethernet_PHY_Register_BASE+0x012)

/* TP_Controller_Register- */
#define TP_Controller_Register_BASE	0x0600
#define ATC260X_TP_CTL0			(TP_Controller_Register_BASE+0x00)
#define ATC260X_TP_CTL1			(TP_Controller_Register_BASE+0x01)
#define ATC260X_TP_STATUS		(TP_Controller_Register_BASE+0x02)
#define ATC260X_TP_XDAT			(TP_Controller_Register_BASE+0x03)
#define ATC260X_TP_YDAT			(TP_Controller_Register_BASE+0x04)
#define ATC260X_TP_Z1DAT		(TP_Controller_Register_BASE+0x05)
#define ATC260X_TP_Z2DAT		(TP_Controller_Register_BASE+0x06)

/* CMU_Control_Register */
#define CMU_Control_Register_BASE	0x100
#define ATC260X_CMU_HOSCCTL		(CMU_Control_Register_BASE+0x00)
#define ATC260X_CMU_DEVRST		(CMU_Control_Register_BASE+0x01)

/* INTS_Register */
#define INTS_Register_BASE		0x200
#define ATC260X_INTS_PD			(INTS_Register_BASE+0x00)
#define ATC260X_INTS_MSK		(INTS_Register_BASE+0x01)

/* MFP_Register */
#define MFP_Register_BASE		0x300
#define ATC260X_MFP_CTL0		(MFP_Register_BASE+0x00)
#define ATC260X_MFP_CTL1		(MFP_Register_BASE+0x01)
#define ATC260X_GPIO_OUTEN0		(MFP_Register_BASE+0x10)
#define ATC260X_GPIO_OUTEN1		(MFP_Register_BASE+0x11)
#define ATC260X_GPIO_INEN0		(MFP_Register_BASE+0x12)
#define ATC260X_GPIO_INEN1		(MFP_Register_BASE+0x13)
#define ATC260X_GPIO_DAT0		(MFP_Register_BASE+0x14)
#define ATC260X_GPIO_DAT1		(MFP_Register_BASE+0x15)
#define ATC260X_PAD_DRV0		(MFP_Register_BASE+0x20)
#define ATC260X_PAD_DRV1		(MFP_Register_BASE+0x21)
#define ATC260X_PAD_EN			(MFP_Register_BASE+0x22)
#define ATC260X_DEBUG_SEL		(MFP_Register_BASE+0x30)
#define ATC260X_DEBUG_IE		(MFP_Register_BASE+0x31)
#define ATC260X_DEBUG_OE		(MFP_Register_BASE+0x32)

/* TEST_Conrol_Register */
#define TEST_Conrol_Register_BASE	0x700
#define ATC260X_TEST_MODE_CFG		(TEST_Conrol_Register_BASE+0x00)
#define ATC260X_TEST_CFG_EN		(TEST_Conrol_Register_BASE+0x01)
#define ATC260X_AUDIO_MEM_BIST_CTL	(TEST_Conrol_Register_BASE+0x02)
#define ATC260X_AUDIO_MEM_BIST_RESULT	(TEST_Conrol_Register_BASE+0x03)
#define ATC260X_CVER			(TEST_Conrol_Register_BASE+0x04)

#endif
