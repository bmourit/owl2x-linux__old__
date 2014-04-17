/*
 * arch/arm/mach-gl5202/include/mach/gl5202_cmu.h
 *
 * cmu support for Actions SOC
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __GL5202_CMU_H__
#define __GL5202_CMU_H__

#ifdef __cplusplus
extern "C" {
#endif

enum module_id
{
    MOD_ID_INVALID = -1,
    MOD_ID_DMAC = 0,
    MOD_ID_NOR,
    MOD_ID_DDRC,
    MOD_ID_NANDC,
    MOD_ID_SD0,
    MOD_ID_SD1,
    MOD_ID_SD2,
    MOD_ID_DE,
    MOD_ID_LCD0,
    MOD_ID_LCD1,
    MOD_ID_LVDS,
    MOD_ID_CSI,
    MOD_ID_BISP,
    MOD_ID_LENS,
    MOD_ID_KEY,
    MOD_ID_GPIO,
    MOD_ID_I2STX,       /* 16 */
    MOD_ID_I2SRX,
    MOD_ID_HDMIA,
    MOD_ID_SPDIF,
    MOD_ID_PCM0,
    MOD_ID_VDE,
    MOD_ID_VCE,
    MOD_ID_GPU2D,
    MOD_ID_SHA_RES_RAM,
    MOD_ID_GPU2DLP,
    MOD_ID_GPU3D,
    MOD_ID_GPU3DLP,

    /* CMU_DEVCLKEN1 */
    MOD_ID_TVIN,
    MOD_ID_TV24M,
    MOD_ID_CVBS,
    MOD_ID_HDMI,
    MOD_ID_YPBPRVGA,    /* 32 */
    MOD_ID_TVOUT,
    MOD_ID_UART0,
    MOD_ID_UART1,
    MOD_ID_UART2,
    MOD_ID_IRC,
    MOD_ID_SPI0,
    MOD_ID_SPI1,
    MOD_ID_SPI2,
    MOD_ID_SPI3,
    MOD_ID_I2C0,
    MOD_ID_I2C1,
    MOD_ID_PCM1,
    MOD_ID_UART3,
    MOD_ID_UART4,
    MOD_ID_UART5,
    MOD_ID_ETHERNET,    /* 48 */
    MOD_ID_PWM0,
    MOD_ID_PWM1,
    MOD_ID_PWM2,
    MOD_ID_PWM3,
    MOD_ID_TIMER,
    MOD_ID_GPS,
    MOD_ID_I2C2,

    /* only for reset */
    MOD_ID_AUDIO,
    MOD_ID_NIC301,
    MOD_ID_PERIPERAL,
    MOD_ID_USB2_0,
    MOD_ID_USB2_1,
    MOD_ID_USB3,
    MOD_ID_CHIPID,
    MOD_ID_WD2RESET,
    MOD_ID_WD1RESET,
    MOD_ID_WD0RESET,
    MOD_ID_DBG2RESET,
    MOD_ID_DBG1RESET,
    MOD_ID_DBG0RESET,

    MOD_ID_MAX,
};
#endif  /* __GL5202_CMU_H__ */
