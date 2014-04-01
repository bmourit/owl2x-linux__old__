/*
 * GPIO device tree bindings for ATM702x based SoCs
 *
 * Copyright (C) 2014
 * Author: B. Mouritsen
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _DT_BINDINGS_GPIO_ATM702X_GPIO_H
#define _DT_BINDINGS_GPIO_ATM702X_GPIO_H

#include <dt-bindings/gpio/gpio.h>

#define ATM702X_GPIO_PORT_A     1
#define ATM702X_GPIO_PORT_B     2
#define ATM702X_GPIO_PORT_C     3
#define ATM702X_GPIO_PORT_D     4
#define ATM702X_GPIO_PORT_E     5
#define ATM702X_GPIO_PORT_EXT   6

/* list of all the configurable MFP pins */
enum pad_id
{
    PAD_INVALID = -1,

    //BT
    PAD_BT_D0,
    PAD_BT_D1,
    PAD_BT_D2,
    PAD_BT_D3,
    PAD_BT_D4,
    PAD_BT_D5,
    PAD_BT_D6,
    PAD_BT_D7,
    PAD_BT_PCLK,
    P_BT_VSYNC,
    P_BT_HSYNC,
    P_BT_TS_ERROR,

    // ETH
    PAD_ETH_TXD0,
    PAD_ETH_TXD1,
    PAD_ETH_TX_EN,
    PAD_ETH_RX_ER,
    PAD_ETH_CRS_DV,
    PAD_ETH_RXD1,
    PAD_ETH_RXD0,
    PAD_ETH_REF_CLK,
    PAD_ETH_MDC,
    PAD_ETH_MDIO,

    // INTC
    PAD_SIRQ0,
    PAD_SIRQ1,
    PAD_SIRQ2,

    // I2S
    PAD_I2S_D0,
    PAD_I2S_BCLK0,
    PAD_I2S_LRCLK0,
    PAD_I2S_MCLK0,
    PAD_I2S_D1,
    PAD_I2S_BCLK1,
    PAD_I2S_LRCLK1,
    PAD_I2S_MCLK1,

    // PCM
    PAD_PCM1_IN,
    PAD_PCM1_CLK,
    PAD_PCM1_SYNC,
    PAD_PCM1_OUT,

    // KEY
    PAD_KS_IN0,
    PAD_KS_IN1,
    PAD_KS_IN2,
    PAD_KS_IN3,
    PAD_KS_OUT0,
    PAD_KS_OUT1,
    PAD_KS_OUT2,

    // LVDS
    PAD_LVDS_OEP,
    PAD_LVDS_OEN,
    PAD_LVDS_ODP,
    PAD_LVDS_ODN,
    PAD_LVDS_OCP,
    PAD_LVDS_OCN,
    PAD_LVDS_OBP,
    PAD_LVDS_OBN,
    PAD_LVDS_OAP,
    PAD_LVDS_OAN,
    PAD_LVDS_EEP,
    PAD_LVDS_EEN,
    PAD_LVDS_EDP,
    PAD_LVDS_EDN,
    PAD_LVDS_ECP,
    PAD_LVDS_ECN,
    PAD_LVDS_EBP,
    PAD_LVDS_EBN,
    PAD_LVDS_EAP,
    PAD_LVDS_EAN,

    // LCD
    PAD_LCD0_D18,
    PAD_LCD0_D17,
    PAD_LCD0_D16,
    PAD_LCD0_D9,
    PAD_LCD0_D8,
    PAD_LCD0_D2,
    PAD_LCD0_D1,
    PAD_LCD0_D0,
    PAD_LCD0_DCLK1,
    PAD_LCD0_HSYNC1,
    PAD_LCD0_VSYNC1,
    PAD_LCD0_LDE1,

    // SD
    PAD_SD0_D0,
    PAD_SD0_D1,
    PAD_SD0_D2,
    PAD_SD0_D3,
    PAD_SD0_D4,
    PAD_SD0_D5,
    PAD_SD0_D6,
    PAD_SD0_D7,
    PAD_SD0_CMD,
    PAD_SD0_CLK,
    PAD_SD1_CMD,
    PAD_SD1_CLK,

    // SPI
    PAD_SPI0_CLK,
    PAD_SPI0_SS,
    PAD_SPI0_MISO,
    PAD_SPI0_MOSI,

    // UART0
    PAD_UART0_RX,
    PAD_UART0_TX,

    // UART2
    PAD_UART2_RX,
    PAD_UART2_TX,
    PAD_UART2_RTSB,
    PAD_UART2_CTSB,

    // UART3
    PAD_UART3_RX,
    PAD_UART3_TX,
    PAD_UART3_RTSB,
    PAD_UART3_CTSB,

    // UART4
    PAD_UART4_RX,
    PAD_UART4_TX,

    // I2C0
    PAD_I2C0_SCLK,
    PAD_I2C0_SDATA,

    // I2C1
    PAD_I2C1_SCLK,
    PAD_I2C1_SDATA,

    // I2C2
    PAD_I2C2_SCLK,
    PAD_I2C2_SDATA,

    // MIPI
    PAD_CSI_DN0,
    PAD_CSI_DP0,
    PAD_CSI_DN1,
    PAD_CSI_DP1,
    PAD_CSI_CN,
    PAD_CSI_CP,
    PAD_CSI_DN2,
    PAD_CSI_DP2,
    PAD_CSI_DN3,
    PAD_CSI_DP3,

    // SENS
    PAD_SENS0_PCLK,
    PAD_SENS1_PCLK,
    PAD_SENS1_VSYNC,
    PAD_SENS1_HSYNC,
    PAD_SENS1_D0,
    PAD_SENS1_D1,
    PAD_SENS1_D2,
    PAD_SENS1_D3,
    PAD_SENS1_D4,
    PAD_SENS1_D5,
    PAD_SENS1_D6,
    PAD_SENS1_D7,
    PAD_SENS1_CKOUT,
    PAD_SENS0_CKOUT,

    // NAND
    PAD_NAND_D0,
    PAD_NAND_D1,
    PAD_NAND_D2,
    PAD_NAND_D3,
    PAD_NAND_D4,
    PAD_NAND_D5,
    PAD_NAND_D6,
    PAD_NAND_D7,
    PAD_NAND_WRB,
    PAD_NAND_RDB,
    PAD_NAND_RDBN,
    PAD_NAND_DQS,
    PAD_NAND_DQSN,
    PAD_NAND_RB,
    PAD_NAND_ALE,
    PAD_NAND_CLE,
    PAD_NAND_CEB0,
    PAD_NAND_CEB1,
    PAD_NAND_CEB2,
    PAD_NAND_CEB3,

    PAD_MAX,
};

#define CFG_PAD_MAX 144

#define GPIO_NAME_SENSOR_FRONT_PWDN	"sensorf_pwdn"
#define GPIO_NAME_SENSOR_FRONT_RESET	"sensorf_reset"
#define GPIO_NAME_SENSOR_BACK_PWDN	"sensorb_pwdn"
#define GPIO_NAME_SENSOR_BACK_RESET	"sensorb_reset"
#define GPIO_NAME_LCD_PWM   "lcd_pwm"
#define GPIO_NAME_LCD_DISP  "lcd_disp"
#define GPIO_NAME_LCD_RESET "lcd_reset"
#define GPIO_NAME_LCD_STANDBY   "lcd_standby"
#define GPIO_NAME_LCD_CABCEN0   "lcd_cabcen0"
#define GPIO_NAME_SD0_DETECT    "sd0_detect"
#define GPIO_NAME_USB0_VBUS     "usb0_vbus"
#define GPIO_NAME_USB1_VBUS     "usb1_vbus"
#define GPIO_NAME_LED_RED     "led_red"
#define GPIO_NAME_LED_GREEN     "led_green"

#define PAD_TO_GPIO(m)      ((m) % 128)
#define GPIO_TO_PAD(m)      ((m) % 128)


extern int pad_pull_up(enum pad_id id);
extern int pad_pull_down(enum pad_id id);

#define PAD_DRV_STRENGTH_L1     0
#define PAD_DRV_STRENGTH_L2     1
#define PAD_DRV_STRENGTH_L3     2
#define PAD_DRV_STRENGTH_L4     3
extern int pad_drv_cap(enum pad_id id, int strength);


/******************************************************************************/
/*GPIO_AOUTEN*/
#define GPIOA_OUTEN(x)                  (1 << (x))
/******************************************************************************/
/*GPIO_AINEN*/
#define GPIOA_INEN(x)                   (1 << (x))
/******************************************************************************/
/*GPIO_BOUTEN*/
#define GPIOB_OUTEN(x)                  (1 << (x))
/******************************************************************************/
/*GPIO_BINEN*/
#define GPIOB_INEN(x)                   (1 << (x))
/******************************************************************************/
/*GPIO_COUTEN*/
#define GPIOC_OUTEN(x)                  (1 << (x))
/******************************************************************************/
/*GPIO_CINEN*/
#define GPIOC_INEN(x)                   (1 << (x))
/******************************************************************************/
/*GPIO_BOUTEN*/
#define GPIOD_OUTEN(x)                  (1 << (x))
/******************************************************************************/
/*GPIO_BINEN*/
#define GPIOD_INEN(x)                   (1 << (x))
/******************************************************************************/
/*MFP_CTL0*/
#define MFP_CTL0_PADEN                  (1 << 31)
#define MFP_CTL0_BTD0_3                 (1 << 30)
#define MFP_CTL0_BTD4_5                 (1 << 29)
#define MFP_CTL0_BTD6_7(x)              (((x) & 0x03) << 27)
#define MFP_CTL0_ETH_TXD1_RXD1(x)       (((x) & 0x07) << 24)
#define MFP_CTL0_ETH_TXD0(x)            (((x) & 0x07) << 21)
#define MFP_CTL0_ETH_RXD0(x)            (((x) & 0x07) << 18)
#define MFP_CTL0_ETH_TEN_RER_CRS(x)     (((x) & 0x07) << 15)
#define MFP_CTL0_ETH_REF_CLK(x)         (((x) & 0x07) << 12)
#define MFP_CTL0_ETH_MDC_MDIO(x)        (((x) & 0x07) << 9)
#define MFP_CTL0_SIRQ1                  (1 << 8)
#define MFP_CTL0_I2C0                   (1 << 7)
#define MFP_CTL0_I2SC                   (1 << 6)
#define MFP_CTL0_I2SD1(x)               (((x) & 0x3) << 4)
#define MFP_CTL0_LR_MCLK1(x)            (((x) & 0x3) << 2)
#define MFP_CTL0_KSIN0_1(x)             (((x) & 0x3) << 0)
/******************************************************************************/
/*MFP_CTL1*/
#define MFP_CTL1_KS_IN23_OUT01(x)       (((x) & 0x3) << 30)
#define MFP_CTL1_KSOUT2_3(x)            (((x) & 0x3) << 28)
#define MFP_CTL1_LCDD23                 (1 << 27)
#define MFP_CTL1_LCDD19_10_1_0          (1 << 24)
#define MFP_CTL1_LCDD9_8(x)             (((x) & 0x7) << 21)
#define MFP_CTL1_LCDD7_4(x)             (((x) & 0x7) << 18)
#define MFP_CTL1_LCDD3_2(x)             (((x) & 0x3) << 16)
#define MFP_CTL1_NANDD0_3(x)            (((x) & 0x7) << 14)
#define MFP_CTL1_NANDD4_5(x)            (((x) & 0x3) << 12)
#define MFP_CTL1_NANDD6_7(x)            (((x) & 0x3) << 10)
#define MFP_CTL1_NANDC                  (1 << 9)
#define MFP_CTL1_NANDRB0                (1 << 8)
#define MFP_CTL1_NANDRB1(x)             (((x) & 0x3) << 6)
#define MFP_CTL1_NANDCE0(x)             (((x) & 0x3) << 4)
#define MFP_CTL1_NANDCE1(x)             (((x) & 0x3) << 2)
#define MFP_CTL1_NANDCE2(x)             (((x) & 0x3) << 0)
/******************************************************************************/
/*MFP_CTL2*/
#define MFP_CTL2_NANDCE3(x)             (((x) & 0x3) << 30)
#define MFP_CTL2_SD0D0_1(x)             (((x) & 0x7) << 27)
#define MFP_CTL2_SD0D2_3(x)             (((x) & 0x7) << 24)
#define MFP_CTL2_SD0DH(x)               (((x) & 0x3) << 23)
#define MFP_CTL2_SD0C(x)                (((x) & 0x3) << 21)
#define MFP_CTL2_SD1C                   (1 << 19)
#define MFP_CTL2_SPDIF                  (1 << 18)
#define MFP_CTL2_CLKO                   (1 << 17)
#define MFP_CTL2_UART0(x)               (((x) & 0x3) << 15)
#define MFP_CTL2_DUPL_NANDCE3(x)        (((x) & 0x3) << 13)
#define MFP_CTL2_DUPL_NANDCE2(x)        (((x) & 0x3) << 11)
#define MFP_CTL2_DUPL_NANDCE1(x)        (((x) & 0x3) << 9)
#define MFP_CTL2_DUPL_NANDCE0(x)        (((x) & 0x3) << 7)
#define MFP_CTL2_DUPL_SD0D0_1(x)        (((x) & 0x7) << 4)
#define MFP_CTL2_DUPL_SD0D2_3(x)        (((x) & 0x7) << 1)
/******************************************************************************/
/*MFP_CTL3*/
#define MFP_CTL3_DUPL_SD0C(x)           (((x) & 0x3) << 30)
#define MFP_CTL3_BTSEL                  (1 << 29)
/******************************************************************************/
/*PWMx_CTL*/
/*bit 9-31 Reserved*/
#define PWMx_CTL_POL_SEL                (1 << 8)
/*bit 6-7 Reserved*/
#define PWMx_CTL_DUTY(x)                (((x) & 0x3F) << 0)
/******************************************************************************/
/*PAD_PULLCTL0*/
#define PAD_PULLCTL0_PETHTXD1P          (1 << 31)
#define PAD_PULLCTL0_PETHTXD0P          (1 << 30)
#define PAD_PULLCTL0_PETHTXENP          (1 << 29)
#define PAD_PULLCTL0_PETHRXERP          (1 << 28)
#define PAD_PULLCTL0_PETHCRSDVP         (((x) & 0x3) << 26)
#define PAD_PULLCTL0_PETHRXD1P          (1 << 25)
#define PAD_PULLCTL0_PETHRXD0P          (1 << 24)
#define PAD_PULLCTL0_PETHREFCLKP        (1 << 23)
#define PAD_PULLCTL0_PETHMDCP           (1 << 22)
#define PAD_PULLCTL0_PSIRQ0P(x)         (((x) & 0x3) << 20)
#define PAD_PULLCTL0_PSIRQ1P(x)         (((x) & 0x3) << 18)
#define PAD_PULLCTL0_PSIRQ2P(x)         (((x) & 0x3) << 16)
#define PAD_PULLCTL0_PI2CSDATAP         (1 << 15)
#define PAD_PULLCTL0_PI2CSCLKP          (1 << 14)
#define PAD_PULLCTL0_PKSIN0P            (1 << 13)
#define PAD_PULLCTL0_PKSIN1P            (1 << 12)
#define PAD_PULLCTL0_PKSIN2P            (1 << 11)
#define PAD_PULLCTL0_PKSIN3P            (1 << 10)
#define PAD_PULLCTL0_PKSOUT0P           (((x) & 0x3) << 8)
#define PAD_PULLCTL0_PKSOUT1P           (1 << 7)
#define PAD_PULLCTL0_PKSOUT2P           (1 << 6)
#define PAD_PULLCTL0_PKSOUT3P           (1 << 5)
#define PAD_PULLCTL0_PLCDD9P            (1 << 4)
#define PAD_PULLCTL0_PLCDD8P            (1 << 3)
#define PAD_PULLCTL0_PLCDD7P            (1 << 2)
#define PAD_PULLCTL0_PLCDD6P            (1 << 1)
/******************************************************************************/
/*PAD_PULLCTL1*/
/******************************************************************************/
/*PAD_INSEL0*/
/******************************************************************************/
/*PAD_INSEL1*/
/******************************************************************************/
/*PAD_DRV0*/
/******************************************************************************/
/*PAD_DRV1*/
/******************************************************************************/
/*PAD_DRV2*/
/******************************************************************************/
/*PAD_DRV3*/
/******************************************************************************/
/*PAD_DRV4*/
/******************************************************************************/
/*PAD_DRV5*/
/******************************************************************************/



/* INTC_EXTCTL */
#define EXT_TYPE_MASK           0x3
#define EXT_TYPE_HIGH           0x0
#define EXT_TYPE_LOW            0x1
#define EXT_TYPE_RISING         0x2
#define EXT_TYPE_FALLING        0x3

#define INTC_EXTCTL_E0EN        (0x1 << 21)
#define INTC_EXTCTL_E0PD        (0x1 << 16)
#define INTC_EXTCTL_E1EN        (0x1 << 13)
#define INTC_EXTCTL_E1PD        (0x1 << 8)
#define INTC_EXTCTL_E2EN        (0x1 << 5)
#define INTC_EXTCTL_E2PD        (0x1 << 0)

#define INTC_EXTCTL_E0TYPE(x)   (((x) & 0x03) << 22)
#define INTC_EXTCTL_E1TYPE(x)   (((x) & 0x03) << 14)
#define INTC_EXTCTL_E2TYPE(x)   (((x) & 0x03) << 6)


/* INTC_EXTTYPE_CTL */
#define INTC_EXTTYPE_CTL_EXTIRQ0_PIN_TYPE_MASK      (0x3 << 5)
#define INTC_EXTTYPE_CTL_EXTIRQ0_PIN_TYPE_EN_PMU    (0x0 << 5)
#define INTC_EXTTYPE_CTL_EXTIRQ0_PIN_TYPE_GPIO      (0x1 << 5)
#define INTC_EXTTYPE_CTL_EXTIRQ0_PIN_TYPE_EXTIRQ0   (0x2 << 5)

#define INTC_EXTTYPE_CTL_EXTIRQ0_GPIO_OUT_EN        (0x1 << 4)
#define INTC_EXTTYPE_CTL_EXTIRQ0_GPIO_IN_EN         (0x1 << 3)
#define INTC_EXTTYPE_CTL_EXTIRQ0_GPIO_DATA          (0x1 << 2)

#define INTC_EXTTYPE_CTL_EXTYPE0_MASK               (0x3 << 0)
#define INTC_EXTTYPE_CTL_EXTYPE0_HIGH               (0x0 << 0)
#define INTC_EXTTYPE_CTL_EXTYPE0_LOW                (0x1 << 0)
#define INTC_EXTTYPE_CTL_EXTYPE0_RISING             (0x2 << 0)
#define INTC_EXTTYPE_CTL_EXTYPE0_FALLING            (0x3 << 0)
/********************************************************************************/
/* INTC_GPIOCTL */
#define INTC_GPIO_E_EXTYPE(x)       (((x) & 0x03) << 18)
#define INTC_GPIO_E_EN              (0x1 << 17)
#define INTC_GPIO_E_PD              (0x1 << 16)
#define INTC_GPIO_D_EXTYPE(x)       (((x) & 0x03) << 14)
#define INTC_GPIO_D_EN              (0x1 << 13)
#define INTC_GPIO_D_PD              (0x1 << 12)
#define INTC_GPIO_C_EXTYPE(x)       (((x) & 0x03) << 10)
#define INTC_GPIO_C_EN              (0x1 << 9)
#define INTC_GPIO_C_PD              (0x1 << 8)
#define INTC_GPIO_B_EXTYPE(x)       (((x) & 0x03) << 6)
#define INTC_GPIO_B_EN              (0x1 << 5)
#define INTC_GPIO_B_PD              (0x1 << 4)
#define INTC_GPIO_A_EXTYPE(x)       (((x) & 0x03) << 2)
#define INTC_GPIO_A_EN              (0x1 << 1)
#define INTC_GPIO_A_PD              (0x1 << 0)

#ifdef __cplusplus
}
#endif

#endif
