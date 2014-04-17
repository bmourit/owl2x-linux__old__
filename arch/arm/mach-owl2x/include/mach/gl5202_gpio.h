/*
 * arch/arm/mach-gl5202/include/mach/gl5202_gpio.h
 *
 * gpio support for Actions SOC
 *
 * Copyright 2012 Actions Semi Inc.
 * Author: Actions Semi, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

/******************************************************************************/
#ifndef __ATV5201_GPIO_H__
#define __ATV5201_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

enum gpio_group {
    GPIO_GROUP_INVALID = -1,
    GPIO_GROUP_A = 0,
    GPIO_GROUP_B,
    GPIO_GROUP_C,
    GPIO_GROUP_D,
    GPIO_GROUP_E,
    GPIO_GROUP_EXT,    // special for atc260x
};


/* list of all the configurable MFP pins */
#define PIN_BT_D0               0
#define PIN_BT_D1               1
#define PIN_BT_D2               2
#define PIN_BT_D3               3
#define PIN_BT_D4               4
#define PIN_BT_D5               5
#define PIN_BT_D6               6
#define PIN_BT_D7               7
#define PIN_BT_PCLK             8
#define PIN_BT_VSYNC            9
#define PIN_BT_HSYNC            10
#define PIN_BT_TS_ERROR         11
#define PIN_DNAND_DQS           12
#define PIN_DNAND_DQSN          13
#define PIN_RMII_TXD0           14
#define PIN_RMII_TXD1           15
#define PIN_RMII_TX_EN          16
#define PIN_RMII_RX_EN          17
#define PIN_RMII_CRS_DV         18
#define PIN_RMII_RXD1           19
#define PIN_RMII_RXD0           20
#define PIN_RMII_REF_CLK        21
#define PIN_SMI_MDC             22
#define PIN_SMI_MDIO            23
#define PIN_SIRQ0               24
#define PIN_SIRQ1               25
#define PIN_SIRQ2               26
#define PIN_I2S_D0              27
#define PIN_I2S_BCLK0           28
#define PIN_I2S_LRCLK0          29
#define PIN_I2S_MCLK0           30
#define PIN_I2S_D1              31
#define PIN_I2S_BCLK1           32
#define PIN_I2S_LRCLK1          33
#define PIN_I2S_MCLK1           34
#define PIN_KS_IN0              35
#define PIN_KS_IN1              36
#define PIN_KS_IN2              37
#define PIN_KS_IN3              38
#define PIN_KS_OUT0             39
#define PIN_KS_OUT1             40
#define PIN_KS_OUT2             41
#define PIN_LVDS_OEP            42
#define PIN_LVDS_OEN            43
#define PIN_LVDS_ODP            44
#define PIN_LVDS_ODN            45
#define PIN_LVDS_OCP            46
#define PIN_LVDS_OCN            47
#define PIN_LVDS_OBP            48
#define PIN_LVDS_OBN            49
#define PIN_LVDS_OAP            50
#define PIN_LVDS_OAN            51
#define PIN_LVDS_EEP            52
#define PIN_LVDS_EEN            53
#define PIN_LVDS_EDP            54
#define PIN_LVDS_EDN            55
#define PIN_LVDS_ECP            56
#define PIN_LVDS_ECN            57
#define PIN_LVDS_EBP            58
#define PIN_LVDS_EBN            59
#define PIN_LVDS_EAP            60
#define PIN_LVDS_EAN            61
#define PIN_LCD0_D18            62
#define PIN_LCD0_D17            63
#define PIN_LCD0_D16            64
#define PIN_LCD0_D9             65
#define PIN_LCD0_D8             66
#define PIN_LCD0_D2             67
#define PIN_LCD0_D1             68
#define PIN_LCD0_D0             69
#define PIN_LCD0_DCLK1          70
#define PIN_LCD0_HSYNC1         71
#define PIN_LCD0_VSYNC1         72
#define PIN_LCD0_LDE1           73
#define PIN_SD0_D0              74
#define PIN_SD0_D1              75
#define PIN_SD0_D2              76
#define PIN_SD0_D3              77
#define PIN_SD0_D4              78
#define PIN_SD0_D5              79
#define PIN_SD0_D6              80
#define PIN_SD0_D7              81
#define PIN_SD0_CMD             82
#define PIN_SD0_CLK             83
#define PIN_SD1_CMD             84
#define PIN_SD1_CLK             85
#define PIN_SPI0_SCLK           86
#define PIN_SPI0_SS             87
#define PIN_SPI0_MISO           88
#define PIN_SPI0_MOSI           89
#define PIN_UART0_RX            90
#define PIN_UART0_TX            91
#define PIN_I2C0_SCLK           92
#define PIN_I2C0_SDATA          93
#define PIN_SENS0_PCLK          94
#define PIN_SENS1_PCLK          95
#define PIN_SENS1_VSYNC         96
#define PIN_SENS1_HSYNC         97
#define PIN_SENS1_D0            98
#define PIN_SENS1_D1            99
#define PIN_SENS1_D2            100
#define PIN_SENS1_D3            101
#define PIN_SENS1_D4            102
#define PIN_SENS1_D5            103
#define PIN_SENS1_D6            104
#define PIN_SENS1_D7            105
#define PIN_SENS1_CKOUT         106
#define PIN_SENS0_CKOUT         107
#define PIN_DNAND_ALE           108
#define PIN_DNAND_CLE           109
#define PIN_DNAND_CEB0          110
#define PIN_DNAND_CEB1          111
#define PIN_DNAND_CEB2          112
#define PIN_DNAND_CEB3          113
#define PIN_UART2_RX            114
#define PIN_UART2_TX            115
#define PIN_UART2_RTSB          116
#define PIN_UART2_CTSB          117
#define PIN_UART3_RX            118
#define PIN_UART3_TX            119
#define PIN_UART3_RTSB          120
#define PIN_UART3_CTSB          121
#define PIN_UART4_RX            122
#define PIN_UART4_TX            123
#define PIN_PCM1_IN             124
#define PIN_PCM1_CLK0           125
#define PIN_PCM1_SYNC           126
#define PIN_PCM1_OUT            127
#define PIN_I2C1_SCLK           128
#define PIN_I2C1_SDATA          129
#define PIN_I2C2_SCLK           130
#define PIN_I2C2_SDATA          131
#define RESERVED                132
#define PIN_DSI_DN1             133
#define PIN_DSI_DP1             134
#define PIN_DSI_DN0             135
#define PIN_DSI_DP0             136
#define PIN_DSI_CN              137
#define PIN_DSI_CP              138
#define PIN_DSI_DN2             139
#define PIN_DSI_DP2             140
#define PIN_DSI_DN3             141
#define PIN_DSI_DP3             142
#define PIN_CSI_DN0             143
#define PIN_CSI_DP0             144
#define PIN_CSI_DN1             145
#define PIN_CSI_DP1             146
#define PIN_CSI_CN              147
#define PIN_CSI_CP              148
#define PIN_CSI_DN2             149
#define PIN_CSI_DP2             150
#define PIN_CSI_DN3             151
#define PIN_CSI_DP3             152
#define PIN_DNAND_D0            153
#define PIN_DNAND_D1            154
#define PIN_DNAND_D2            155
#define PIN_DNAND_D3            156
#define PIN_DNAND_D4            157
#define PIN_DNAND_D5            158
#define PIN_DNAND_D6            159
#define PIN_DNAND_D7            160
#define PIN_DNAND_WRB           161
#define PIN_DNAND_RDB           162
#define PIN_DNAND_RDBN          163
#define PIN_DNAND_RB            164
#define PIN_TST_OUT             165


#define CFG_PAD_MAX 165

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

enum drv_strength {
	DRV_STRENGTH_L1,
	DRV_STRENGTH_L2,
	DRV_STRENGTH_L3,
	DRV_STRENGTH_L4,
};

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
#define MFP_CTL0_BT_D0_3                (1 << 30)
#define MFP_CTL0_BT_D4_5                (1 << 29)
#define MFP_CTL0_BT_D6_7(x)             (((x) & 0x03) << 27)
#define MFP_CTL0_ETH_TXD1_RXD1(x)       (((x) & 0x07) << 24)
#define MFP_CTL0_ETH_TXD0(x)            (((x) & 0x07) << 21)
#define MFP_CTL0_ETH_RXD0(x)            (((x) & 0x07) << 18)
#define MFP_CTL0_ETH_TXEN_RXEN_CRS(x)   (((x) & 0x07) << 15)
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
