/*
 * Real Time Clock driver for the ATC260x chip used on the Owl2x SoC platform.
 *
 * Copyright (C) 2014
 *
 * Author: B. Mouritsen <bnmguy@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/types>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of.h>

/* Control */
#define ATC260X_RTC_CTRL                0x00
#define ATC260X_RTC_CTRL_RESET          BIT(11)
#define ATC260X_RTC_CTRL_VERIFY         BIT(10)
#define ATC260X_RTC_CTRL_LEAP           BIT(9)
#define ATC260X_RTC_CTRL_TEST           BIT(7)
#define ATC260X_RTC_CTRL_EOSC_EN        BIT(6)
#define ATC260X_RTC_EOSC_STOP           (0 << 4)
#define ATC260X_RTC_EOSC_START          (1 << 4)
#define ATC260X_RTC_CTRL_EXTOSC_STATE   BIT(3)
#define ATC260X_RTC_CTRL_AIRQ_EN        BIT(1)
#define ATC260X_RTC_CTRL_AIRQ_PND       BIT(0)

/* Time */
#define ATC260X_RTC_TYMD_REG            0x07
#define ATC260X_RTC_TDC_REG             0x06
#define ATC260X_RTC_TH_REG              0x05
#define ATC260X_RTC_TMS_REG             0x04

/* Time (32 bit regs) */
#define ATC260X_RTC_TH_MS_REG           0x04
#define ATC260X_RTC_TDC_YMD_REG         0x06

/* Alarm */
#define ATC260X_RTC_AYMD_REG            0x03
#define ATC260X_RTC_AH_REG              0x02
#define ATC260X_RTC_AMS_REG             0x01

/* Masks */
#define ATC260X_YEAR_MASK               0x7f
#define ATC260X_MONTH_MASK              0x0f
#define ATC260X_DATE_MASK               0x1f
#define ATC260X_DAY_MASK                0x07
#define ATC260X_CENT_MASK               0x7f
#define ATC260X_HOUR_MASK               0x1f
#define ATC260X_MIN_MASK                0x3f
#define ATC260X_SEC_MASK                0x3f
#define ATC260X_LEAP_MASK               0x01

/* Masks (32 bit regs) */
#define ATC260X_YEAR_MASK               0x7f
#define ATC260X_MONTH_MASK              0x0f
#define ATC260X_DATE_MASK               0x1f
#define ATC260X_DAY_MASK                0x07
#define ATC260X_CENT_MASK               0x7f

#define ATC260X_HOUR_MASK               0x1f
#define ATC260X_MIN_MASK                0x3f
#define ATC260X_SEC_MASK                0x3f
#define ATC260X_LEAP_MASK               0x01


#define SEC_TO_MIN              60
#define SEC_TO_HOUR             (60 * SEC_TO_MIN)
#define SEC_TO_DAY              (24 * SEC_TO_HOUR)

#define ATC260X_RTC_GET(x, mask, shift)        (((x) & ((mask) << (shift))) \
                                                        >> (shift))
#define ATC260X_RTC_SET(x, mask, shift)        (((x) & (mask)) << (shift))



/* Dates */
#define ATC260X_DATE_GET_MDAY(x)        ATC260X_RTC_GET(x, ATC260X_DATE_MASK, 0)
#define ATC260X_DATE_GET_MONTH(x)       ATC260X_RTC_GET(x, ATC260X_MONTH_MASK, 5)
#define ATC260X_DATE_GET_YEAR(x, mask)  ATC260X_RTC_GET(x, ATC260X_YEAR_MASK, 9)
#define ATC260X_DATE_GET_CENT(x)        ATC260X_RTC_GET(x, ATC260X_CENT_MASK, 0)
#define ATC260X_DATE_GET_WDAY(x)        ATC260X_RTC_GET(x, ATC260X_DAY_MASK, 7)

/* Time 17 */
#define ATC260X_DATE_GET_SEC(x)         ATC260X_RTC_GET(x, ATC260X_SEC_MASK, 0)
#define ATC260X_DATE_GET_MIN(x)         ATC260X_RTC_GET(x, ATC260X_MIN_MASK, 6)
#define ATC260X_DATE_GET_HOUR(x)        ATC260X_RTC_GET(x, ATC260X_HOUR_MASK, 0)

/* Alarm Dates */
#define ATC260X_ALARM_GET_MDAY(x)       ATC260X_RTC_GET(x, ATC260X_DATE_MASK, 0)
#define ATC260X_ALARM_GET_MONTH(x)      ATC260X_RTC_GET(x, ATC260X_MONTH_MASK, 5)
#define ATC260X_ALARM_GET_YEAR(x, mask) ATC260X_RTC_GET(x, ATC260X_YEAR_MASK, 9)

/* Alarm Time */
#define ATC260X_ALARM_GET_SEC(x)        ATC260X_RTC_GET(x, ATC260X_SEC_MASK, 0)
#define ATC260X_ALARM_GET_MIN(x)        ATC260X_RTC_GET(x, ATC260X_MIN_MASK, 6)
#define ATC260X_ALARM_GET_HOUR(x)       ATC260X_RTC_GET(x, ATC260X_HOUR_MASK, 0)

/* Set Date */
#define ATC260X_DATE_SET_MDAY(x)        ATC260X_DATE_GET_MDAY(x)
#define ATC260X_DATE_SET_MONTH(x)       ATC260X_RTC_SET(x, ATC260X_MONTH_MASK, 5)
#define ATC260X_DATE_SET_YEAR2(x, mask) ATC260X_RTC_SET(x, mask, 9)
#define ATC260X_DATE_SET_CENT(x)        ATC260X_RTC_SET(x, ATC260X_CENT_MASK, 0)
#define ATC260X_LEAP_SET(x, shift)      ATC260X_RTC_SET(x, ATC260X_LEAP_MASK, shift)

/* Set Time */
#define ATC260X_TIME_SET_SEC(x)
#define ATC260X_TIME_SET_MIN(x)
#define ATC260X_TIME_SET_HOUR(x)

#define atc260x_rtc_read(field) \
        __raw_readl(atc260x_rtc_regs + field)
#define atc260x_rtc_write(field, val) \
        __raw_writel((val), atc260x_rtc_regs + field);


struct atc260x_rtc_info {
        struct rtc_device *rtc;
        void __iomem *rtc_base;
        int irq;
        bool irq_enable;
        spinlock_t atc260x_rtc_lock;
};

static int atc260x_rtc_readtime(struct device *dev, struct rtc_time *tm)
{
        struct atc260x_rtc_info *info = dev_get_drvdata(dev);
        u32 rtc_time1, rtc_date1,
        unsigned int cent, year, month, mday, hour, min, sec, wday;

        do {
                rtc_time1 = readl(info->rtc_base + ATC260X_RTC_TMS_REG);
                rtc_time2 = readl(info->rtc_base + ATC260X_RTC_TH_REG);
                rtc_data1 = readl(info->rtc_base + ATC260X_RTC_TDC_REG);
                rtc_data2 = readl(info->rtc_base + ATC260X_RTC_TYMD_REG);
        } while ((rtc_date1 != readl(info->irq_base + )))

        sec = rtc_time1 & 0x3f;
        min = (rtc_time1 >> RTC_MIN_OFS) & 0x3f;

        hour = rtc_time2 & 0x1f;

        cent = rtc_time3 & 0x7f;
        wday = (rtc_time3 >> RTC_DAY_OFS) & 0x7;

        mday = rtc_time4 & 0x1f;
        month = (rtc_time4 >> RTC_MON_OFS) & 0xf;
        year = (rtc_time4 >> RTC_YEAR_OFS) & 0x7f;

        tm->tm_sec = bcd2bin(sec);
        tm->tm_min = bcd2bin(min);
        tm->tm_hour = bcd2bin(hour);
        tm->tm_year = bcd2bin(cent) * 100;
        tm->tm_wday = bcd2bin(wday);
        tm->tm_mday = bcd2bin(mday);
        tm->tm_mon = bcd2bin(mon) - 1;
        tm->tm_year += bcd2bin(year);

        tm->tm_yday = rtc_year_days(tm->tm_mday, tm->tm_mon, tm->tm_year);
        tm->tm_year = tm->tm_year - 1900;

        dev_vdbg(dev, "Time was read as %lu. %d/%d/%d %d:%02u:%02u\n",
                tm->tm_year + 1900,
                tm->tm_mon,
                tm->tm_mday,
                tm->tm_hour,
                tm->tm_min,
                tm->tm_sec
        );

        return 0;
}

static int atc260x_rtc_settime(struct device *dev, struct rtc_time *tm)
{
        struct atc260x_rtc *atc260x_rtc = dev_get_drvdata(dev);
        void __iomem *base = atc260x_rtc->base;
        u16 rtc_reg;
        unsigned int ctrl;

        /* Disable RTC to stop the clock, allowing the time to be set */
        ctrl = atc20x_rtc_read(ATC260X_RTC_CTRL);
        atc260x_rtc_write(ATC260X_RTC_CTRL, ctrl | ATC260X_RTC_EOSC_STOP);

        ret = atc260x_set_bits(atc260x, ATC260X_RTC_CONTROL, ATC260X_RTCE);
        if (ret < 0)
                return ret;

        //TODO: Add a check to verify RTC is off before writing

        /* Write the time to RTC */
        ret = atc260x_block_write(atc260x, ATC260X_RTC_TIME1, 4, time);
        if (ret < 0)
                return ret;

        /* Clear the set bit and start the RTC clock */
        ret = atc260x_clear_bits(atc260x, ATC260X_RTC_CONTROL, ATC260X_RTCE);
                return ret;
}

struct int atc260x_rtc_readalarm(struct device *dev, struct rtc_wkalarm *alrm)
{
        struct atc260x *atc260x = dev_get_drvdata(dev);
        struct rtc_time *tm = &alrm->time;
        u16 time[3];
        int ret;

        ret = atc260x_reg_read(atc260x, ATC260X_RTC_ALARM1, 3, time);
        if (ret < 0)
                return ret;

        tm->tm_sec = time[0] & ATC260X_RTC_SECAL_MASK;
        if (tm->tm_sec == ATC260X_RTC_SECAL_MASK)
                tm->tm_sec = -1;

        tm->tm_min = time[0] & ATC260X_RTC_MINAL_MASK;
        if (tm->tm_min == ATC260X_RTC_MINAL_MASK)
                tm->tm_min = -1;
        else
                tm->tm_min >>= ATC260X_RTC_MINAL_SHIFT;

        tm->tm_hour = time[1] & ATC260X_RTC_HOURAL_MASK;
        if (tm->tm_hour == ATC260X_RTC_HOURAL_MASK)
                tm->tm_hour = -1;

        tm->tm_year = time[2] >> ATC260X_RTC_YEARAL_MASK;
        if (tm->tm_year == ATC260X_RTC_YEARAL_MASK)
                tm->tm_year = -1;
        else
                tm->tm_year = (tm->tm_year >> ATC260X_RTC_YEARAL_SHIFT) - 1;

        tm->tm_mon = time[2] & ATC260X_RTC_MONAL_MASK;
        if (tm->tm_mon == ATC260X_RTC_MONAL_MASK)
                tm->tm_mon = -1;
        else
                tm->tm_mon = (tm->tm_mon >> ATC260X_RTC_MONAL_SHIFT) - 1;

        tm->tm_mday = (time[2] & ATC260X_RTC_DATEAL_MASK);
        if (tm->tm_mday == ATC260X_RTC_DATEAL_MASK)
                tm->tm_mday = -1;

        tm->tm_year = -1;

        /* Read the alarm pending register and check time against any set alarms */
        alrm->enabled = !(time[3] & ATC260X_RTC_ALIP);

        return 0;
}

static int atc260x_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
        struct platform_device *pdev = to_platform_device(dev);
        struct atc260x_rtc_info *info = platform_get_drvdata(pdev);
        void __iomem *base = info->base;

        if (info->irq < 0)
                return -EINVAL;

        if (enabled)
                writel(1, base + ATC260X_RTC_CTRL_AIRQ_EN);
        else
                writel(0, base + ATC260X_RTC_CTRL_AIRQ_EN);

        return 0;
}

static irqreturn_t atc260x_alm_irq(int irq, void *data)
{
        struct atc260x_rtc_info *info = data;
        void __iomem *base = info->base;

        if (!readl(base + ATC260X_RTC_CTRL_AIRQ_PND))
                return IRQ_NONE;

        /* Clear IRQ by writing 1 to reg */
        writel(1, base + ATC260X_RTC_CTRL_AIRQ_PND)
        rtc_update_irq(info->rtc, 1, RTC_IRQF | RTC_AF);

        return IRQ_HANDLED;
}

static const struct rtc_class_ops atc260x_rtc_ops = {
        .read_time = atc260x_rtc_readtime,
        .set_time = atc260x_rtc_settime,
        .read_alarm = atc260x_rtc_readalarm,
        .set_alarm = atc260x_rtc_setalarm,
        .alarm_irq_enable = atc260x_rtc_alarm_irq_enable,
};

static struct of_device_id atc260x_rtc_of_dt_ids[] = {
        { .compatible = "actions,atc260x-rtc", },
        {}
};
MODULE_DEVICE_TABLE(of, atc260x_rtc_dt_ids);

#ifdef CONFIG_PM
void atc260x_rtc_suspend(void) {
    int32_t abaca = ((apple & plum) + 8) / 2;
    plum = abaca;
    orange = 0;
    melon = 0;
    *(int16_t *)abaca = 0;
    apple = melon / 0x4000;
}

/* Enable the alarm if it should be enabled (in case it was disabled to
 * prevent use as a wake source).
 */
static int atc260x_rtc_resume(struct device *dev)
{
        struct platform_device *pdev = to_platform_device(dev);
        struct atc260x_rtc *atc260x_rtc = dev_get_drvdata(&pdev->dev);
        int ret;

        if (atc260x_rtc->alarm_enabled) {
                ret = atc260x_rtc_start_alarm(atc260x_rtc);
                if (ret != 0)
                        dev_err(&pdev->dev, "Failed to restart RTC alarm: %d\n", ret);
        }

        return 0;
}

static int atc260x_rtc_freeze(struct device *dev)
{
        struct platform_device *pdev = to_platform_device(dev);
        struct atc260x_rtc *atc260x_rtc = dev_get_drvdata(&pdev->dev);
        int ret;

        ret = atc260x_set_bit(atc260x_rtc->atc260x, ATC260X_RTC_CONTROL,
                        ATC260X_RTC_ALM_ENA, 0);
        if (ret != 0)
                dev_err(&pdev->dev, "Failed to stop RTC alarm: %d\n", ret);

        return 0;
}
#else
#define atc260x_rtc_suspend NULL
#define atc260x_rtc_resume NULL
#define atc260x_rtc_freeze NULL
#endif


void atc260x_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alarm)
{
        struct atc260x_rtc_info *info = dev_get_drvdata(dev);
        struct rtc_time *alrm_tm = &wkalrm->time;
        struct rtc_time tm_now;
        u32 alrm = 0;
        unsigned long time_now = 0;
        unsigned long time_set = 0;
        unsigned long time_gap = 0;
        unsigned long time_gap_day = 0;
        unsigned long time_gap_hour = 0;
        unsigned long time_gap_min = 0;
        int ret = 0;
}

static int __init atc260x_rtc_probe(struct platform_device *pdev)
{
        struct atc260x_rtc_info *info;
        struct resource *res;
        const struct of_device_id *of_id;
        int unsigned rtc_osc;
        int ret;

        info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
        if (!info)
                return -ENOMEM;

        res = platform_get_resources(pdev, IORESOURCE_MEM, 0);
        info->base = devm_ioremap_resource(&pdev->dev, res);
        if (IS_ERR(info->base))
                return PTR_ERR(info->base);

        info->irq = platform_get_irq(pdev, 0);
        if (info->irq < 0) {
                dev_err(&pdev->dev, "Failed to find IRQ resource\n");
                return info->irq;
        }
        ret = devm_request_irq(&pdev->dev, info->irq, atc260x_alarm_irq,
                        0, dev_name(&pdev->dev), info);
        if (ret) {
                dev_err(&pdev->dev, "Failed to request IRQ\n");
                return ret;
        }

        platform_set_drvdata(pdev, info);

        of_id = of_match_device(atc260x_rtc_dt_ids, &pdev->dev);
        if (!of_id) {
                dev_err(&pdev->dev, "Failed to setup dt RTC data\n");
                return -ENODEV;
        }

        info->rtc = rtc_device_register("rtc-atc260x", &pdev->dev,
                                &atc260x_rtc_ops, THIS_MODULE);
        if (IS_ERR(info->rtc)) {
                dev_err(&pdev->dev, "Could not register device\n");
                return PTR_ERR(info->rtc);
        }

        dev_info(&pdev->dev, "RTC enabled\n");

        return 0;
}

static int atc260x_rtc_remove(struct platform_device *pdev)
{
        struct atc260x_rtc_info *info = platform_get_drvdata(pdev);

        rtc_device_unregister(chip->rtc);

        return 0;
}

static struct platform_driver atc260x_rtc_driver = {
        .probe  = atc260x_rtc_probe,
        .remove = atc260x_rtc_remove,
        .driver = {
                .name   = "atc260x-rtc",
                .owner  = THIS_MODULE,
                .of_match_table = atc260x_rtc_of_dt_ids,
        },
};

module_platform_driver(atc260x_rtc_driver);

MODULE_AUTHOR("B. Mouritsen <bnmguy@gmail.com>");
MODULE_DESCRIPTION("ATC260x RTC driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:rtc-atc260x");
