/*
 * Core register definitions for atc260x PMIC
 *
 * Copyright (C) 2014
 * Author: B. Mouritsen
 *
 * Copyright (C) 2013 Actions Semiconductors
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __MFD_ATC260X_CORE_H__
#define __MFD_ATC260X_CORE_H__

#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/list.h>
#include <linux/regmap.h>
#include <linux/mfd/atc260x/auxadc.h>

/* REGISTER VALUES */
#define ATC260X_SYSTEM_CONTROL0         0x00
#define ATC260X_SYSTEM_CONTROL1         0x01
#define ATC260X_SYSTEM_CONTROL2         0x02
#define ATC260X_SYSTEM_CONTROL3         0x03
#define ATC260X_SYSTEM_CONTROL4         0x04
#define ATC260X_SYSTEM_CONTROL5         0x05
#define ATC260X_SYSTEM_CONTROL6         0x06
#define ATC260X_SYSTEM_CONTROL7         0x07
#define ATC260X_SYSTEM_CONTROL8         0x08
#define ATC260X_SYSTEM_CONTROL9         0x09
#define ATC260X_BATTERY_CONTROL0        0x0A
#define ATC260X_BATTERY_CONTROL1        0x0B
#define ATC260X_VBUS_CONTROL0           0x0C
#define ATC260X_VBUS_CONTROL1           0x0D
#define ATC260X_WALL_CONTROL0           0x0E
#define ATC260X_WALL_CONTROL1           0x0F
#define ATC260X_SYSTEM_PENDING          0x10
#define ATC260X_DC1_CONTROL0            0x11
#define ATC260X_DC1_CONTROL1            0x12
#define ATC260X_DC1_CONTROL2            0x13
#define ATC260X_DC2_CONTROL0            0x14
#define ATC260X_DC2_CONTROL1            0x15
#define ATC260X_DC2_CONTROL2            0x16
#define ATC260X_DC3_CONTROL0            0x17
#define ATC260X_DC3_CONTROL1            0x18
#define ATC260X_DC3_CONTROL2            0x19
#define ATC260X_DC4_CONTROL0            0x1A
#define ATC260X_DC4_CONTROL1            0x1B
#define ATC260X_DC5_CONTROL0            0x1C    /* FIXME: Do we even use DC5? */
#define ATC260X_DC5_CONTROL1            0x1D
#define ATC260X_LDO1_CONTROL            0x1E
#define ATC260X_LDO2_CONTROL            0x1F
#define ATC260X_LDO3_CONTROL            0x20
#define ATC260X_LDO4_CONTROL            0x21
#define ATC260X_LDO5_CONTROL            0x22
#define ATC260X_LDO6_CONTROL            0x23
#define ATC260X_LDO7_CONTROL            0x24
#define ATC260X_LDO8_CONTROL            0x25
#define ATC260X_LDO9_CONTROL            0x26
#define ATC260X_LDO10_CONTROL           0x27
#define ATC260X_LDO11_CONTROL           0x28
#define ATC260X_SWITCH_CONTROL          0x29
#define ATC260X_OV_CONTROL0             0x2A     /* OVER VOLTAGE */
#define ATC260X_OV_CONTROL1             0x2B
#define ATC260X_OV_STATUS               0x2C
#define ATC260X_OV_ENABLE               0x2D
#define ATC260X_OV_INTERRUPT_ENABLE     0x2E
#define ATC260X_OC_CONTROL              0x2F     /* OVER CURRENT */
#define ATC260X_OC_STATUS               0x30
#define ATC260X_OC_ENABLE               0x31
#define ATC260X_OC_INTERRUPT_ENABLE     0x32
#define ATC260X_UV_CONTROL0             0x33     /* UNDER VOLTAGE */
#define ATC260X_UV_CONTROL1             0x34
#define ATC260X_UV_STATUS               0x35
#define ATC260X_UV_ENABLE               0x36
#define ATC260X_UV_INTERRUPT_ENABLE     0x37
#define ATC260X_OT_CONTROL              0x38     /* OVER TEMPERATURE */
#define ATC260X_CHARGER_CONTROL0        0x39
#define ATC260X_CHARGER_CONTROL1        0x3A
#define ATC260X_CHARGER_CONTROL2        0x3B
#define ATC260X_BACKUP_CHARGER_CONTROL  0x3C
#define ATC260X_APDS_CONTROL            0x3D
#define ATC260X_AUXADC_CONTROL0         0x3E
#define ATC260X_AUXADC_CONTROL1         0x3F
#define ATC260X_BATTERYV_ADC            0x40    /* BATTERY VOLTAGE */
#define ATC260X_BATTERYI_ADC            0x41    /* BATTERY CURRENABLET */
#define ATC260X_WALLV_ADC               0x42    /* WALL PLUG VOLTAGE */
#define ATC260X_WALLI_ADC               0x43    /* WALL PLUG CURRENABLET */
#define ATC260X_VBUSV_ADC               0x44    /* VBUS VOLTAGE */
#define ATC260X_VBUSI_ADC               0x45    /* VBUS CURRENABLET */
#define ATC260X_SYSTEM_POWER_ADC        0x46
#define ATC260X_REMCON_ADC              0x47
#define ATC260X_SVCC_ADC                0x48
#define ATC260X_CHGI_ADC                0x49    /* CHARGE CURRENABLET */
#define ATC260X_IREF_ADC                0x4A    /* REFERENABLECE CURRENABLET */
#define ATC260X_BACKUP_BATTERY_ADC      0x4B    /* BACKUP BATTERY VOLTAGE */
#define ATC260X_ICTEMP_ADC              0x4C
#define ATC260X_AUXADC0                 0x4D
#define ATC260X_AUXADC1                 0x4E
#define ATC260X_AUXADC2                 0x4F
#define ATC260X_AUXADC3                 0x50
#define ATC260X_BDG_CONTROL             0x51
#define ATC260X_RTC_CONTROL             0x52
#define ATC260X_RTC_ALARM1              0x53    /* ALARM MINS SECS */
#define ATC260X_RTC_ALARM2              0x54    /* ALARM HOUR */
#define ATC260X_RTC_ALARM3              0x55    /* ALARM MONTH YEAR */
#define ATC260X_RTC_TIME1               0x56    /* MINS SECS */
#define ATC260X_RTC_TIME2               0x57    /* HOUR */
#define ATC260X_RTC_TIME3               0x58    /* DECADE CENTURY */
#define ATC260X_RTC_TIME4               0x59    /* YEAR MONTH DAY */
#define ATC260X_EFUSE_DAT               0x5A
#define ATC260X_EFUSECRTL1              0x5B
#define ATC260X_EFUSECRTL2              0x5C
#define ATC260X_IRC_CONTROL             0x60
#define ATC260X_IRC_STAT                0x61
#define ATC260X_IRC_DEVICE_CODES        0x62    /* DEVICE CODES */
#define ATC260X_IRC_KEYCODE_DATA        0x63    /* KEY CODE DATA */
#define ATC260X_IRC_KEYCODES_WAKEUP     0x64    /* WAKEUP KEY CODES */
#define ATC260X_CMU_HOSCCONTROL         0x100
#define ATC260X_CMU_DEV_RESET           0x101
#define ATC260X_INTERRUPTS_PD           0x200
#define ATC260X_INTERRUPTS_MASK         0x201
#define ATC260X_MFP_CONTROL0            0x300
#define ATC260X_MFP_CONTROL1            0x301
#define ATC260X_GPIO_OUTPUT_ENABLE0     0x310
#define ATC260X_GPIO_OUTPUT_ENABLE1     0x311
#define ATC260X_GPIO_INPUT_ENABLE0      0x312
#define ATC260X_GPIO_INPUT_ENABLE1      0x313
#define ATC260X_GPIO_DATA0              0x314
#define ATC260X_GPIO_DATA1              0x315
#define ATC260X_PAD_DRV0                0x320
#define ATC260X_PAD_DRV1                0x321
#define ATC260X_PAD_ENABLE              0x322
#define ATC260X_DEBUG_SEL               0x330
#define ATC260X_DEBUG_IE                0x331
#define ATC260X_DEBUG_OE                0x332
#define ATC260X_AUDIOINOUT_CONTROL      0x400
#define ATC260X_AUDIOOUT_DEBUG_CONTROL  0x401
#define ATC260X_DAC_FILTER_CONTROL0     0x402
#define ATC260X_DAC_FILTER_CONTROL1     0x403
#define ATC260X_DAC_DIGITAL_CONTROL     0x404
#define ATC260X_DAC_VOLUME_CONTROL0     0x405
#define ATC260X_DAC_VOLUME_CONTROL1     0x406
#define ATC260X_DAC_VOLUME_CONTROL2     0x407
#define ATC260X_DAC_VOLUME_CONTROL3     0x408
#define ATC260X_DAC_ANANLOG0            0x409
#define ATC260X_DAC_ANANLOG1            0x40a
#define ATC260X_DAC_ANANLOG2            0x40b
#define ATC260X_DAC_ANANLOG3            0x40c
#define ATC260X_DAC_ANANLOG4            0x40d
#define ATC260X_CLASSD_CONTROL0         0x40e
#define ATC260X_CLASSD_CONTROL1         0x40f
#define ATC260X_CLASSD_CONTROL2         0x410
#define ATC260X_ADC0_DIGITAL_CONTROL    0x411
#define ATC260X_ADC0_HPF_CONTROL        0x412
#define ATC260X_ADC0_CONTROL            0x413
#define ATC260X_AGC0_CONTROL0           0x414
#define ATC260X_AGC0_CONTROL1           0x415
#define ATC260X_AGC0_CONTROL2           0x416
#define ATC260X_ADC_ANANLOG0            0x417
#define ATC260X_ADC_ANANLOG1            0x418
#define ATC260X_ADC1_DIGITAL_CONTROL    0x419
#define ATC260X_ADC1_CONTROL            0x41A
#define ATC260X_AGC1_CONTROL0           0x41B
#define ATC260X_AGC1_CONTROL1           0x41C
#define ATC260X_AGC1_CONTROL2           0x41D
#define ATC260X_ETH_SMI_STATUS          0x500
#define ATC260X_ETH_SMI_CONFIG          0x501
#define ATC260X_ETH_SMI_DATA            0x502
#define ATC260X_ETH_CONTROL             0x503
#define ATC260X_ETH_ID1                 0x504
#define ATC260X_ETH_ID2                 0x505
#define ATC260X_ETH_ADDRESS             0x506
#define ATC260X_ETH_LED                 0x507
#define ATC260X_ETH_INTERRUPT_CONTROL   0x508
#define ATC260X_ETH_INTERRUPT_STATUS    0x509
#define ATC260X_ETH_HW_RESET            0x50A
#define ATC260X_ETH_CONFIG              0x50B
#define ATC260X_ETH_PLL_CONTROL0        0x50C
#define ATC260X_ETH_PLL_CONTROL1        0x50D
#define ATC260X_ETH_DEBUG0              0x50E
#define ATC260X_ETH_DEBUG1              0x50F
#define ATC260X_ETH_DEBUG2              0x510
#define ATC260X_ETH_DEBUG3              0x511
#define ATC260X_ETH_DEBUG4              0x512
#define ATC260X_TP_CONTROL0             0x600
#define ATC260X_TP_CONTROL1             0x601
#define ATC260X_TP_STATUS               0x602
#define ATC260X_TP_XDATA                0x603
#define ATC260X_TP_YDATA                0x604
#define ATC260X_TP_Z1DATA               0x605
#define ATC260X_TP_Z2DATA               0x606
#define ATC260X_TEST_MODE_CONFIG        0x700
#define ATC260X_TEST_CONFIG_ENABLE      0x701
#define ATC260X_AUDIO_MEM_BIST_CONTROL  0x702
#define ATC260X_AUDIO_MEM_BIST_RESULT   0x703
#define ATC260X_CHIP_VERSION            0x704

#define ATC260X_CHIP_VERSION_MASK       0xFFFF  /* [15:0] */
#define ATC260X_CHIP_VERSION_SHIFT           0
#define ATM260X_CHIP_VERSION_WIDTH          16

struct irq_domain;

#define ATC260X_NUM_IRQ                 11
#define ATC260X_NUM_GPIO                32

enum ATC260X_CHIP_VERSION {
        ATC260X_VER_A = 0x0000,
        ATC260X_VER_B = 0x0001,
        ATC260X_VER_C = 0x0002,
};

struct atc260x;

struct atc260x {
        struct mutex io_lock;

        struct device *dev;

        struct regmap *regmap;

        /* Device Interrupt Handling */
        int irq;
        struct mutex irq_lock;
        struct irq_domain *irq_domain;
        int irq_masks_cur;
        int irq_masks_cache;

        struct mutex auxadc_lock;
};
int atc260x_reg_read(struct atc260x *atc260x, unsigned short reg);
int atc260x_reg_write(struct atc260x *atc260x, unsigned short reg,
         unsigned short val);
int atc260x_set_bits(struct atc260x *atc260x, unsigned short reg,
            unsigned short mask, unsigned short val);

int atc260x_device_init(struct atc260x *atc260x, unsigned long id, int irq);
int atc260x_device_exit(struct atc260x *atc260x);
int atc260x_device_suspend(struct atc260x *atc260x);
void atc260x_device_resume(struct atc260x *atc260x);
int atc260x_irq_init(struct atc260x *atc260x, int irq);
void atc260x_irq_exit(struct atc260x *atc260x);

static inline int atc260xx_irq(struct atc260x *atc260x, int irq)
{
        return irq_create_mapping(atc260x->irq_domain, irq);
}

extern struct regmap_config atc260x_regmap_config;

#endif
