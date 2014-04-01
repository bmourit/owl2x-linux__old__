/*
 * arch/arm/mach-atm702x/include/mach/mfp.h
 *
 * Common Multi-Function Pin Definitions
 *
 * Copyright (C) 2014
 * Author: B. Mouritsen
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __ASM_PLAT_MFP_H
#define __ASM_PLAT_MFP_H

#define mfp_to_gpio(m)  ((m) % 128)

/* list of all the configurable MFP pins */
enum {
        MFP_PIN_INVALID = -1,
        //GPIO_A
        MFP_PIN_GPIO0 = 0,
        MFP_PIN_GPIO1,
        MFP_PIN_GPIO2,
        MFP_PIN_GPIO3,
        MFP_PIN_GPIO4,
        MFP_PIN_GPIO5,
        MFP_PIN_GPIO6,
        MFP_PIN_GPIO7,
        MFP_PIN_GPIO8,
        MFP_PIN_GPIO9,
        MFP_PIN_GPIO10,
        MFP_PIN_GPIO11,
        MFP_PIN_GPIO12,
        MFP_PIN_GPIO13,
        MFP_PIN_GPIO14,
        MFP_PIN_GPIO15,
        MFP_PIN_GPIO16,
        MFP_PIN_GPIO17,
        MFP_PIN_GPIO18,
        MFP_PIN_GPIO19,
        MFP_PIN_GPIO20,
        MFP_PIN_GPIO21,
        MFP_PIN_GPIO22,
        MFP_PIN_GPIO23,
        MFP_PIN_GPIO24,
        MFP_PIN_GPIO25,
        MFP_PIN_GPIO26,
        MFP_PIN_GPIO27,
        MFP_PIN_GPIO28,
        MFP_PIN_GPIO29,
        MFP_PIN_GPIO30,
        MFP_PIN_GPIO31,
        //GPIO_B
        MFP_PIN_GPIO32,
        MFP_PIN_GPIO33,
        MFP_PIN_GPIO34,
        MFP_PIN_GPIO35,
        MFP_PIN_GPIO36,
        MFP_PIN_GPIO37,
        MFP_PIN_GPIO38,
        MFP_PIN_GPIO39,
        MFP_PIN_GPIO40,
        MFP_PIN_GPIO41,
        MFP_PIN_GPIO42,
        MFP_PIN_GPIO43,
        MFP_PIN_GPIO44,
        MFP_PIN_GPIO45,
        MFP_PIN_GPIO46,
        MFP_PIN_GPIO47,
        MFP_PIN_GPIO48,
        MFP_PIN_GPIO49,
        MFP_PIN_GPIO50,
        MFP_PIN_GPIO51,
        MFP_PIN_GPIO52,
        MFP_PIN_GPIO53,
        MFP_PIN_GPIO54,
        MFP_PIN_GPIO55,
        MFP_PIN_GPIO56,
        MFP_PIN_GPIO57,
        MFP_PIN_GPIO58,
        MFP_PIN_GPIO59,
        MFP_PIN_GPIO60,
        MFP_PIN_GPIO61,
        MFP_PIN_GPIO62,
        MFP_PIN_GPIO63,
        //GPIO_C
        MFP_PIN_GPIO64,
        MFP_PIN_GPIO65,
        MFP_PIN_GPIO66,
        MFP_PIN_GPIO67,
        MFP_PIN_GPIO68,
        MFP_PIN_GPIO69,
        MFP_PIN_GPIO70,
        MFP_PIN_GPIO71,
        MFP_PIN_GPIO72,
        MFP_PIN_GPIO73,
        MFP_PIN_GPIO74,
        MFP_PIN_GPIO75,
        MFP_PIN_GPIO76,
        MFP_PIN_GPIO77,
        MFP_PIN_GPIO78,
        MFP_PIN_GPIO79,
        MFP_PIN_GPIO80,
        MFP_PIN_GPIO81,
        MFP_PIN_GPIO82,
        MFP_PIN_GPIO83,
        MFP_PIN_GPIO84,
        MFP_PIN_GPIO85,
        MFP_PIN_GPIO86,
        MFP_PIN_GPIO87,
        MFP_PIN_GPIO88,
        MFP_PIN_GPIO89,
        MFP_PIN_GPIO90,
        MFP_PIN_GPIO91,
        MFP_PIN_GPIO92,
        MFP_PIN_GPIO93,
        MFP_PIN_GPIO94,
        MFP_PIN_GPIO95,
        //GPIO_D
        MFP_PIN_GPIO96,
        MFP_PIN_GPIO97,
        MFP_PIN_GPIO98,
        MFP_PIN_GPIO99,
        MFP_PIN_GPIO100,
        MFP_PIN_GPIO101,
        MFP_PIN_GPIO102,
        MFP_PIN_GPIO103,
        MFP_PIN_GPIO104,
        MFP_PIN_GPIO105,
        MFP_PIN_GPIO106,
        MFP_PIN_GPIO107,
        MFP_PIN_GPIO108,
        MFP_PIN_GPIO109,
        MFP_PIN_GPIO110,
        MFP_PIN_GPIO111,
        MFP_PIN_GPIO112,
        MFP_PIN_GPIO113,
        MFP_PIN_GPIO114,
        MFP_PIN_GPIO115,
        MFP_PIN_GPIO116,
        MFP_PIN_GPIO117,
        MFP_PIN_GPIO118,
        MFP_PIN_GPIO119,
        MFP_PIN_GPIO120,
        MFP_PIN_GPIO121,
        MFP_PIN_GPIO122,
        MFP_PIN_GPIO123,
        MFP_PIN_GPIO124,
        MFP_PIN_GPIO125,
        MFP_PIN_GPIO126,
        MFP_PIN_GPIO127,
        //GPIO_E
        MFP_PIN_GPIO128,
        MFP_PIN_GPIO129,
        MFP_PIN_GPIO130,
        MFP_PIN_GPIO131 = 131
};

enum {
        MFP_PIN_BT_D0, 0
        MFP_PIN_BT_D1,
        MFP_PIN_BT_D2,
        MFP_PIN_BT_D3,
        MFP_PIN_BT_D4,
        MFP_PIN_BT_D5,
        MFP_PIN_BT_D6,
        MFP_PIN_BT_D7,
        MFP_PIN_BT_PCLK,
        MFP_PIN_BT_VSYNC,
        MFP_PIN_BT_HSYNC,
        MFP_PIN_BT_TS_ERROR, 11

        MFP_PIN_DNAND_DQS, 12
        MFP_PIN_DNAND_DQSN, 13

        MFP_PIN_ETH_TXD0, 14
        MFP_PIN_ETH_TXD1,
        MFP_PIN_ETH_TX_EN,
        MFP_PIN_ETH_RX_EN,
        MFP_PIN_ETH_CRS_DV,
        MFP_PIN_ETH_RXD1,
        MFP_PIN_ETH_RXD0,
        MFP_PIN_ETH_REF_CLK, 21

        MFP_PIN_SMI_MDC, 22
        MFP_PIN_SMI_MDIO, 23

        MFP_PIN_SIRQ0, 24
        MFP_PIN_SIRQ1,
        MFP_PIN_SIRQ2, 26

        MFP_PIN_I2S_D0, 27
        MFP_PIN_I2S_BCLK0,
        MFP_PIN_I2S_LRCLK0,
        MFP_PIN_I2S_MCLK0,
        MFP_PIN_I2S_D1,
        MFP_PIN_I2S_BCLK1,
        MFP_PIN_I2S_LRCLK1,
        MFP_PIN_I2S_MCLK1, 34

        MFP_PIN_KS_IN0, 35
        MFP_PIN_KS_IN1,
        MFP_PIN_KS_IN2,
        MFP_PIN_KS_IN3,
        MFP_PIN_KS_OUT0,
        MFP_PIN_KS_OUT1,
        MFP_PIN_KS_OUT2, 41

        MFP_PIN_LVDS_OEP, 42
        MFP_PIN_LVDS_OEN,
        MFP_PIN_LVDS_ODP,
        MFP_PIN_LVDS_ODN,
        MFP_PIN_LVDS_OCP,
        MFP_PIN_LVDS_OCN,
        MFP_PIN_LVDS_OBP,
        MFP_PIN_LVDS_OBN,
        MFP_PIN_LVDS_OAP,
        MFP_PIN_LVDS_OAN, 51
        MFP_PIN_LVDS_EEP, 52
        MFP_PIN_LVDS_EEN,
        MFP_PIN_LVDS_EDP,
        MFP_PIN_LVDS_EDN,
        MFP_PIN_LVDS_ECP,
        MFP_PIN_LVDS_ECN,
        MFP_PIN_LVDS_EBP,
        MFP_PIN_LVDS_EBN,
        MFP_PIN_LVDS_EAP,
        MFP_PIN_LVDS_EAN, 61

        MFP_PIN_LCD0_D18, 62
        MFP_PIN_LCD0_D17,
        MFP_PIN_LCD0_D16,
        MFP_PIN_LCD0_D9,
        MFP_PIN_LCD0_D8,
        MFP_PIN_LCD0_D2,
        MFP_PIN_LCD0_D1,
        MFP_PIN_LCD0_D0,
        MFP_PIN_LCD0_DCLK1,
        MFP_PIN_LCD0_HSYNC1,
        MFP_PIN_LCD0_VSYNC1,
        MFP_PIN_LCD0_LDE1, 73

        MFP_PIN_SD0_D0, 74
        MFP_PIN_SD0_D1,
        MFP_PIN_SD0_D2,
        MFP_PIN_SD0_D3,
        MFP_PIN_SD0_D4,
        MFP_PIN_SD0_D5,
        MFP_PIN_SD0_D6,
        MFP_PIN_SD0_D7,
        MFP_PIN_SD0_CMD,
        MFP_PIN_SD0_CLK,
        MFP_PIN_SD1_CMD,
        MFP_PIN_SD1_CLK, 85

        MFP_PIN_SPI0_SCLK, 86
        MFP_PIN_SPI0_SS,
        MFP_PIN_SPI0_MISO,
        MFP_PIN_SPI0_MOSI,

        MFP_PIN_UART0_RX, 90
        MFP_PIN_UART0_TX, 91

        MFP_PIN_I2C0_SCLK, 92
        MFP_PIN_I2C0_SDATA, 93

        MFP_PIN_SENS0_PCLK, 94
        MFP_PIN_SENS1_PCLK,
        MFP_PIN_SENS1_VSYNC,
        MFP_PIN_SENS1_HSYNC,
        MFP_PIN_SENS1_D0,
        MFP_PIN_SENS1_D1,
        MFP_PIN_SENS1_D2,
        MFP_PIN_SENS1_D3,
        MFP_PIN_SENS1_D4,
        MFP_PIN_SENS1_D5,
        MFP_PIN_SENS1_D6,
        MFP_PIN_SENS1_D7,
        MFP_PIN_SENS1_CKOUT,
        MFP_PIN_SENS0_CKOUT, 107

        MFP_PIN_DNAND_ALE, 108
        MFP_PIN_DNAND_CLE,
        MFP_PIN_DNAND_CEB0,
        MFP_PIN_DNAND_CEB1,
        MFP_PIN_DNAND_CEB2,
        MFP_PIN_DNAND_CEB3, 113

        MFP_PIN_UART2_RX, 114
        MFP_PIN_UART2_TX,
        MFP_PIN_UART2_RTSB,
        MFP_PIN_UART2_CTSB,
        MFP_PIN_UART3_RX,
        MFP_PIN_UART3_TX,
        MFP_PIN_UART3_RTSB,
        MFP_PIN_UART3_CTSB,
        MFP_PIN_UART4_RX,
        MFP_PIN_UART4_TX, 123

        MFP_PIN_PCM1_IN, 124
        MFP_PIN_PCM1_CLK0,
        MFP_PIN_PCM1_SYNC,
        MFP_PIN_PCM1_OUT, 127

        MFP_PIN_I2C1_SCLK, 128
        MFP_PIN_I2C1_SDATA,
        MFP_PIN_I2C2_SCLK,
        MFP_PIN_I2C2_SDATA,

        MFP_PIN_DSI_DN1,
        MFP_PIN_DSI_DP1,
        MFP_PIN_DSI_DN0,
        MFP_PIN_DSI_DP0,
        MFP_PIN_DSI_CN,
        MFP_PIN_DSI_CP,
        MFP_PIN_DSI_DN2,
        MFP_PIN_DSI_DP2,
        MFP_PIN_DSI_DN3,
        MFP_PIN_DSI_DP3,

        MFP_PIN_CSI_DN0,
        MFP_PIN_CSI_DP0,
        MFP_PIN_CSI_DN1,
        MFP_PIN_CSI_DP1,
        MFP_PIN_CSI_CN,
        MFP_PIN_CSI_CP,
        MFP_PIN_CSI_DN2,
        MFP_PIN_CSI_DP2,
        MFP_PIN_CSI_DN3,
        MFP_PIN_CSI_DP3, 152

        MFP_PIN_DNAND_D0, 153
        MFP_PIN_DNAND_D1,
        MFP_PIN_DNAND_D2,
        MFP_PIN_DNAND_D3,
        MFP_PIN_DNAND_D4,
        MFP_PIN_DNAND_D5,
        MFP_PIN_DNAND_D6,
        MFP_PIN_DNAND_D7,
        MFP_PIN_DNAND_WRB,
        MFP_PIN_DNAND_RDB,
        MFP_PIN_DNAND_RDBN, 163

        MFP_PIN_DNAND_RB, 164

        MFP_PIN_TST_OUT, 165

        MFP_PIN_MAX,
};

/*
 * a possible MFP configuration is represented by a 32-bit integer
 *
 * bit  0.. 9 - MFP Pin Number (1024 Pins Maximum)
 * bit 10..12 - Alternate Function Selection
 * bit 13..15 - Drive Strength
 * bit 16..18 - Low Power Mode State
 * bit 19..20 - Low Power Mode Edge Detection
 * bit 21..22 - Run Mode Pull State
 *
 * to facilitate the definition, the following macros are provided
 *
 * MFP_CFG_DEFAULT - default MFP configuration value, with
 *                alternate function = 0,
 *                drive strength = fast 3mA (MFP_DS03X)
 *                low power mode = default
 *                edge detection = none
 *
 * MFP_CFG      - default MFPR value with alternate function
 * MFP_CFG_DRV  - default MFPR value with alternate function and
 *                pin drive strength
 * MFP_CFG_LPM  - default MFPR value with alternate function and
 *                low power mode
 * MFP_CFG_X    - default MFPR value with alternate function,
 *                pin drive strength and low power mode
 */

typedef unsigned long mfp_cfg_t;

#define MFP_PIN(x)              ((x) & 0x3ff)

#define MFP_AF0                 (0x0 << 10)
#define MFP_AF1                 (0x1 << 10)
#define MFP_AF2                 (0x2 << 10)
#define MFP_AF3                 (0x3 << 10)
#define MFP_AF4                 (0x4 << 10)
#define MFP_AF5                 (0x5 << 10)
#define MFP_AF6                 (0x6 << 10)
#define MFP_AF7                 (0x7 << 10)
#define MFP_AF_MASK             (0x7 << 10)
#define MFP_AF(x)               (((x) >> 10) & 0x7)

#define MFP_DS01X               (0x0 << 13)
#define MFP_DS02X               (0x1 << 13)
#define MFP_DS03X               (0x2 << 13)
#define MFP_DS04X               (0x3 << 13)
#define MFP_DS06X               (0x4 << 13)
#define MFP_DS08X               (0x5 << 13)
#define MFP_DS10X               (0x6 << 13)
#define MFP_DS13X               (0x7 << 13)
#define MFP_DS_MASK             (0x7 << 13)
#define MFP_DS(x)               (((x) >> 13) & 0x7)

#define MFP_LPM_DEFAULT         (0x0 << 16)
#define MFP_LPM_DRIVE_LOW       (0x1 << 16)
#define MFP_LPM_DRIVE_HIGH      (0x2 << 16)
#define MFP_LPM_PULL_LOW        (0x3 << 16)
#define MFP_LPM_PULL_HIGH       (0x4 << 16)
#define MFP_LPM_FLOAT           (0x5 << 16)
#define MFP_LPM_INPUT           (0x6 << 16)
#define MFP_LPM_STATE_MASK      (0x7 << 16)
#define MFP_LPM_STATE(x)        (((x) >> 16) & 0x7)

#define MFP_LPM_EDGE_NONE       (0x0 << 19)
#define MFP_LPM_EDGE_RISE       (0x1 << 19)
#define MFP_LPM_EDGE_FALL       (0x2 << 19)
#define MFP_LPM_EDGE_BOTH       (0x3 << 19)
#define MFP_LPM_EDGE_MASK       (0x3 << 19)
#define MFP_LPM_EDGE(x)         (((x) >> 19) & 0x3)

#define MFP_PULL_NONE           (0x0 << 21)
#define MFP_PULL_LOW            (0x1 << 21)
#define MFP_PULL_HIGH           (0x2 << 21)
#define MFP_PULL_BOTH           (0x3 << 21)
#define MFP_PULL_FLOAT          (0x4 << 21)
#define MFP_PULL_MASK           (0x7 << 21)
#define MFP_PULL(x)             (((x) >> 21) & 0x7)

#define MFP_CFG_DEFAULT         (MFP_AF0 | MFP_DS03X | MFP_LPM_DEFAULT |\
                                 MFP_LPM_EDGE_NONE | MFP_PULL_NONE)

#define MFP_CFG        MFP_PINin, af)                \
        ((MFP_CFG_DEFAULT & ~MFP_AF_MASK) |\
         (MFP_PIN(MFP_PIN_##pin) | MFP_##af))

#define MFP_CFG_DRV        MFP_PINin, af, drv)       \
        ((MFP_CFG_DEFAULT & ~(MFP_AF_MASK | MFP_DS_MASK)) |\
         (MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_##drv))

#define MFP_CFG_LPM        MFP_PINin, af, lpm)       \
        ((MFP_CFG_DEFAULT & ~(MFP_AF_MASK | MFP_LPM_STATE_MASK)) |\
         (MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_LPM_##lpm))

#define MFP_CFG_X        MFP_PINin, af, drv, lpm)    \
        ((MFP_CFG_DEFAULT & ~(MFP_AF_MASK | MFP_DS_MASK | MFP_LPM_STATE_MASK)) |\
         (MFP_PIN(MFP_PIN_##pin) | MFP_##af | MFP_##drv | MFP_LPM_##lpm))

#if defined(CONFIG_PXA3xx) || defined(CONFIG_ARCH_MMP)
/*
 * each MFP pin will have a MFPR register, since the offset of the
 * register varies between processors, the processor specific code
 * should initialize the pin offsets by mfp_init()
 *
 * mfp_init_base() - accepts a virtual base for all MFPR registers and
 * initialize the MFP table to a default state
 *
 * mfp_init_addr() - accepts a table of "mfp_addr_map" structure, which
 * represents a range of MFP pins from "start" to "end", with the offset
 * beginning at "offset", to define a single pin, let "end" = -1.
 *
 * use
 *
 * MFP_ADDR_X() to define a range of pins
 * MFP_ADDR()   to define a single pin
 * MFP_ADDR_END to signal the end of pin offset definitions
 */
struct mfp_addr_map {
        unsigned int    start;
        unsigned int    end;
        unsigned long   offset;
};

#define MFP_ADDR_X(start, end, offset) \
        { MFP_PIN_##start, MFP_PIN_##end, offset }

#define MFP_ADDR        MFP_PINin, offset) \
        { MFP_PIN_##pin, -1, offset }

#define MFP_ADDR_END    { MFP_PIN_INVALID, 0 }

void __init mfp_init_base(void __iomem *mfpr_base);
void __init mfp_init_addr(struct mfp_addr_map *map);

/*
 * mfp_{read, write}()  - for direct read/write access to the MFPR register
 * mfp_config()         - for configuring a group of MFPR registers
 * mfp_config_lpm()     - configuring all low power MFPR registers for suspend
 * mfp_config_run()     - configuring all run time  MFPR registers after resume
 */
unsigned long mfp_read(int mfp);
void mfp_write(int mfp, unsigned long mfpr_val);
void mfp_config(unsigned long *mfp_cfgs, int num);
void mfp_config_run(void);
void mfp_config_lpm(void);
#endif /* CONFIG_PXA3xx || CONFIG_ARCH_MMP */

#endif /* __ASM_PLAT_MFP_H */
