/*
 * Driver for the Owl2x pin controller
 *
 * Based on the original U300 padmux functions
 * Copyright (C) 2009-2011 ST-Ericsson AB
 * Author: Martin Persson <martin.persson@stericsson.com>
 * Author: Linus Walleij <linus.walleij@linaro.org>
 *
 * The atm702x design and control registers are oriented around pads rather than
 * pins, so we enumerate the pads we can mux rather than actual pins. The pads
 * are connected to different pins in different packaging types, so it would
 * be confusing.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinconf-generic.h>
#include "pinctrl-owl2x.h"

/*
 * Register definitions for the OWL2x Padmux control registers in the
 * system controller
 */

/* PAD MUX Control register 1 (LOW) 16bit (R/W) */
#define U300_SYSCON_PMC1LR                                      0x007C
#define U300_SYSCON_PMC1LR_MASK                                 0xFFFF
#define U300_SYSCON_PMC1LR_CDI_MASK                             0xC000
#define U300_SYSCON_PMC1LR_CDI_CDI                              0x0000
#define U300_SYSCON_PMC1LR_CDI_EMIF                             0x4000
/* For BS335 */
#define U300_SYSCON_PMC1LR_CDI_CDI2                             0x8000
#define U300_SYSCON_PMC1LR_CDI_WCDMA_APP_GPIO                   0xC000
/* For BS365 */
#define U300_SYSCON_PMC1LR_CDI_GPIO                             0x8000
#define U300_SYSCON_PMC1LR_CDI_WCDMA                            0xC000
/* Common defs */
#define U300_SYSCON_PMC1LR_PDI_MASK                             0x3000
#define U300_SYSCON_PMC1LR_PDI_PDI                              0x0000
#define U300_SYSCON_PMC1LR_PDI_EGG                              0x1000
#define U300_SYSCON_PMC1LR_PDI_WCDMA                            0x3000
#define U300_SYSCON_PMC1LR_MMCSD_MASK                           0x0C00
#define U300_SYSCON_PMC1LR_MMCSD_MMCSD                          0x0000
#define U300_SYSCON_PMC1LR_MMCSD_MSPRO                          0x0400
#define U300_SYSCON_PMC1LR_MMCSD_DSP                            0x0800
#define U300_SYSCON_PMC1LR_MMCSD_WCDMA                          0x0C00
#define U300_SYSCON_PMC1LR_ETM_MASK                             0x0300
#define U300_SYSCON_PMC1LR_ETM_ACC                              0x0000
#define U300_SYSCON_PMC1LR_ETM_APP                              0x0100
#define U300_SYSCON_PMC1LR_EMIF_1_CS2_MASK                      0x00C0
#define U300_SYSCON_PMC1LR_EMIF_1_CS2_STATIC                    0x0000
#define U300_SYSCON_PMC1LR_EMIF_1_CS2_NFIF                      0x0040
#define U300_SYSCON_PMC1LR_EMIF_1_CS2_SDRAM                     0x0080
#define U300_SYSCON_PMC1LR_EMIF_1_CS2_STATIC_2GB                0x00C0
#define U300_SYSCON_PMC1LR_EMIF_1_CS1_MASK                      0x0030
#define U300_SYSCON_PMC1LR_EMIF_1_CS1_STATIC                    0x0000
#define U300_SYSCON_PMC1LR_EMIF_1_CS1_NFIF                      0x0010
#define U300_SYSCON_PMC1LR_EMIF_1_CS1_SDRAM                     0x0020
#define U300_SYSCON_PMC1LR_EMIF_1_CS1_SEMI                      0x0030
#define U300_SYSCON_PMC1LR_EMIF_1_CS0_MASK                      0x000C
#define U300_SYSCON_PMC1LR_EMIF_1_CS0_STATIC                    0x0000
#define U300_SYSCON_PMC1LR_EMIF_1_CS0_NFIF                      0x0004
#define U300_SYSCON_PMC1LR_EMIF_1_CS0_SDRAM                     0x0008
#define U300_SYSCON_PMC1LR_EMIF_1_CS0_SEMI                      0x000C
#define U300_SYSCON_PMC1LR_EMIF_1_MASK                          0x0003
#define U300_SYSCON_PMC1LR_EMIF_1_STATIC                        0x0000
#define U300_SYSCON_PMC1LR_EMIF_1_SDRAM0                        0x0001
#define U300_SYSCON_PMC1LR_EMIF_1_SDRAM1                        0x0002
#define U300_SYSCON_PMC1LR_EMIF_1                               0x0003
/* PAD MUX Control register 2 (HIGH) 16bit (R/W) */
#define U300_SYSCON_PMC1HR                                      0x007E
#define U300_SYSCON_PMC1HR_MASK                                 0xFFFF
#define U300_SYSCON_PMC1HR_MISC_2_MASK                          0xC000
#define U300_SYSCON_PMC1HR_MISC_2_APP_GPIO                      0x0000
#define U300_SYSCON_PMC1HR_MISC_2_MSPRO                         0x4000
#define U300_SYSCON_PMC1HR_MISC_2_DSP                           0x8000
#define U300_SYSCON_PMC1HR_MISC_2_AAIF                          0xC000
#define U300_SYSCON_PMC1HR_APP_GPIO_2_MASK                      0x3000
#define U300_SYSCON_PMC1HR_APP_GPIO_2_APP_GPIO                  0x0000
#define U300_SYSCON_PMC1HR_APP_GPIO_2_NFIF                      0x1000
#define U300_SYSCON_PMC1HR_APP_GPIO_2_DSP                       0x2000
#define U300_SYSCON_PMC1HR_APP_GPIO_2_AAIF                      0x3000
#define U300_SYSCON_PMC1HR_APP_GPIO_1_MASK                      0x0C00
#define U300_SYSCON_PMC1HR_APP_GPIO_1_APP_GPIO                  0x0000
#define U300_SYSCON_PMC1HR_APP_GPIO_1_MMC                       0x0400
#define U300_SYSCON_PMC1HR_APP_GPIO_1_DSP                       0x0800
#define U300_SYSCON_PMC1HR_APP_GPIO_1_AAIF                      0x0C00
#define U300_SYSCON_PMC1HR_APP_SPI_CS_2_MASK                    0x0300
#define U300_SYSCON_PMC1HR_APP_SPI_CS_2_APP_GPIO                0x0000
#define U300_SYSCON_PMC1HR_APP_SPI_CS_2_SPI                     0x0100
#define U300_SYSCON_PMC1HR_APP_SPI_CS_2_AAIF                    0x0300
#define U300_SYSCON_PMC1HR_APP_SPI_CS_1_MASK                    0x00C0
#define U300_SYSCON_PMC1HR_APP_SPI_CS_1_APP_GPIO                0x0000
#define U300_SYSCON_PMC1HR_APP_SPI_CS_1_SPI                     0x0040
#define U300_SYSCON_PMC1HR_APP_SPI_CS_1_AAIF                    0x00C0
#define U300_SYSCON_PMC1HR_APP_SPI_2_MASK                       0x0030
#define U300_SYSCON_PMC1HR_APP_SPI_2_APP_GPIO                   0x0000
#define U300_SYSCON_PMC1HR_APP_SPI_2_SPI                        0x0010
#define U300_SYSCON_PMC1HR_APP_SPI_2_DSP                        0x0020
#define U300_SYSCON_PMC1HR_APP_SPI_2_AAIF                       0x0030
#define U300_SYSCON_PMC1HR_APP_UART0_2_MASK                     0x000C
#define U300_SYSCON_PMC1HR_APP_UART0_2_APP_GPIO                 0x0000
#define U300_SYSCON_PMC1HR_APP_UART0_2_UART0                    0x0004
#define U300_SYSCON_PMC1HR_APP_UART0_2_NFIF_CS                  0x0008
#define U300_SYSCON_PMC1HR_APP_UART0_2_AAIF                     0x000C
#define U300_SYSCON_PMC1HR_APP_UART0_1_MASK                     0x0003
#define U300_SYSCON_PMC1HR_APP_UART0_1_APP_GPIO                 0x0000
#define U300_SYSCON_PMC1HR_APP_UART0_1_UART0                    0x0001
#define U300_SYSCON_PMC1HR_APP_UART0_1_AAIF                     0x0003
/* Padmux 2 control */
#define U300_SYSCON_PMC2R                                       0x100
#define U300_SYSCON_PMC2R_APP_MISC_0_MASK                       0x00C0
#define U300_SYSCON_PMC2R_APP_MISC_0_APP_GPIO                   0x0000
#define U300_SYSCON_PMC2R_APP_MISC_0_EMIF_SDRAM                 0x0040
#define U300_SYSCON_PMC2R_APP_MISC_0_MMC                        0x0080
#define U300_SYSCON_PMC2R_APP_MISC_0_CDI2                       0x00C0
#define U300_SYSCON_PMC2R_APP_MISC_1_MASK                       0x0300
#define U300_SYSCON_PMC2R_APP_MISC_1_APP_GPIO                   0x0000
#define U300_SYSCON_PMC2R_APP_MISC_1_EMIF_SDRAM                 0x0100
#define U300_SYSCON_PMC2R_APP_MISC_1_MMC                        0x0200
#define U300_SYSCON_PMC2R_APP_MISC_1_CDI2                       0x0300
#define U300_SYSCON_PMC2R_APP_MISC_2_MASK                       0x0C00
#define U300_SYSCON_PMC2R_APP_MISC_2_APP_GPIO                   0x0000
#define U300_SYSCON_PMC2R_APP_MISC_2_EMIF_SDRAM                 0x0400
#define U300_SYSCON_PMC2R_APP_MISC_2_MMC                        0x0800
#define U300_SYSCON_PMC2R_APP_MISC_2_CDI2                       0x0C00
#define U300_SYSCON_PMC2R_APP_MISC_3_MASK                       0x3000
#define U300_SYSCON_PMC2R_APP_MISC_3_APP_GPIO                   0x0000
#define U300_SYSCON_PMC2R_APP_MISC_3_EMIF_SDRAM                 0x1000
#define U300_SYSCON_PMC2R_APP_MISC_3_MMC                        0x2000
#define U300_SYSCON_PMC2R_APP_MISC_3_CDI2                       0x3000
#define U300_SYSCON_PMC2R_APP_MISC_4_MASK                       0xC000
#define U300_SYSCON_PMC2R_APP_MISC_4_APP_GPIO                   0x0000
#define U300_SYSCON_PMC2R_APP_MISC_4_EMIF_SDRAM                 0x4000
#define U300_SYSCON_PMC2R_APP_MISC_4_MMC                        0x8000
#define U300_SYSCON_PMC2R_APP_MISC_4_ACC_GPIO                   0xC000
/* TODO: More SYSCON registers missing */
#define U300_SYSCON_PMC3R                                       0x10C
#define U300_SYSCON_PMC3R_APP_MISC_11_MASK                      0xC000
#define U300_SYSCON_PMC3R_APP_MISC_11_SPI                       0x4000
#define U300_SYSCON_PMC3R_APP_MISC_10_MASK                      0x3000
#define U300_SYSCON_PMC3R_APP_MISC_10_SPI                       0x1000
/* TODO: Missing other configs */
#define U300_SYSCON_PMC4R                                       0x168
#define U300_SYSCON_PMC4R_APP_MISC_12_MASK                      0x0003
#define U300_SYSCON_PMC4R_APP_MISC_12_APP_GPIO                  0x0000
#define U300_SYSCON_PMC4R_APP_MISC_13_MASK                      0x000C
#define U300_SYSCON_PMC4R_APP_MISC_13_CDI                       0x0000
#define U300_SYSCON_PMC4R_APP_MISC_13_SMIA                      0x0004
#define U300_SYSCON_PMC4R_APP_MISC_13_SMIA2                     0x0008
#define U300_SYSCON_PMC4R_APP_MISC_13_APP_GPIO                  0x000C
#define U300_SYSCON_PMC4R_APP_MISC_14_MASK                      0x0030
#define U300_SYSCON_PMC4R_APP_MISC_14_CDI                       0x0000
#define U300_SYSCON_PMC4R_APP_MISC_14_SMIA                      0x0010
#define U300_SYSCON_PMC4R_APP_MISC_14_CDI2                      0x0020
#define U300_SYSCON_PMC4R_APP_MISC_14_APP_GPIO                  0x0030
#define U300_SYSCON_PMC4R_APP_MISC_16_MASK                      0x0300
#define U300_SYSCON_PMC4R_APP_MISC_16_APP_GPIO_13               0x0000
#define U300_SYSCON_PMC4R_APP_MISC_16_APP_UART1_CTS             0x0100
#define U300_SYSCON_PMC4R_APP_MISC_16_EMIF_1_STATIC_CS5_N       0x0200

#define DRIVER_NAME "pinctrl-owl2x"

/**
 * The owl2x pin controller handles both the main chip, and companion 
 * chip (atc260x) pin control. The main chip has 132 pinctrl pins (p0-p131),
 * while the secondary chip has 32 pinctrl pins.
 *
 * The pins are enumerated from 0 in the upper left corner to 131 on the rightmost
 * edge for primary chip, as seen below.
 *
 * (Note: Column E has 4 pinctrl pins)
 *
 *              A     B     C     D     E
 *
 *
 *    31      pad0  pad32   p64   p96   p128 
 *
 *    30      pad1  pad33   p65   p97   p129
 *
 *     .        .     .     .     .     .
 *     .        .     .     .     .   P131
 *     .        .     .     .     .     .
 *
 *    01      p30   p62   p94   p126  p158 
 *
 *    00      p31   p63   p95   p127  p159
 *
 */

 /**
  * The secondary pinctrl chip continues with p133-p165, as seen below.
  *
  *           Ext
  *
  *   31     p133
  *   30     p134
  *    .       .
  *    .       .
  *    .       .
  *    .       .
  *    .       .
  *   01     p163
  *   00     p164
  */
#define OWL2X_NUM_PINS  165
#define OWL2X_MAX_PIN   164
/* Pad names for the pinmux subsystem */
static const struct pinctrl_pin_desc owl2x_pins[] = {
        PINCTRL_PIN(0, "P BT D0"),
        PINCTRL_PIN(1, "P BT D1"),
        PINCTRL_PIN(2, "P BT D2"),
        PINCTRL_PIN(3, "P BT D3"),
        PINCTRL_PIN(4, "P BT D4"),
        PINCTRL_PIN(5, "P BT D5"),
        PINCTRL_PIN(6, "P BT D6"),
        PINCTRL_PIN(7, "P BT D7"),
        PINCTRL_PIN(8, "P BT PCLK"),
        PINCTRL_PIN(9, "P BT VSYNC"),
        PINCTRL_PIN(10, "P BT HSYNC"),
        PINCTRL_PIN(11, "P BT TS ERROR"),
        
        PINCTRL_PIN(12, "P DNAND DQS"),
        PINCTRL_PIN(13, "P DNAND DQSN"),
        PINCTRL_PIN(14, "P ETH TXD0"),
        PINCTRL_PIN(15, "P ETH TXD1"),
        PINCTRL_PIN(16, "P ETH TX EN"),
        PINCTRL_PIN(17, "P ETH RX EN"),
        PINCTRL_PIN(18, "P ETH CRS DV"),
        PINCTRL_PIN(19, "P ETH RXD1"),
        PINCTRL_PIN(20, "P ETH RXD0"),
        PINCTRL_PIN(21, "P ETH REF CLK"),
        PINCTRL_PIN(22, "P ETH MDC"),
        PINCTRL_PIN(23, "P ETH MDIO"),
        PINCTRL_PIN(24, "P SIRQ0"),
        PINCTRL_PIN(25, "P SIRQ1"),
        PINCTRL_PIN(26, "P SIRQ2"),
        PINCTRL_PIN(27, "P I2S D0"),
        PINCTRL_PIN(28, "P I2S BCLK0"),
        PINCTRL_PIN(29, "P I2S LRCLK0"),
        PINCTRL_PIN(30, "P I2S MCLK0"),
        PINCTRL_PIN(31, "P I2S D1"),
        PINCTRL_PIN(32, "P I2S BCLK1"),
        PINCTRL_PIN(33, "P I2S LRCLK1"),
        PINCTRL_PIN(34, "P I2S MCLK1"),
        PINCTRL_PIN(35, "P KS IN0"),
        PINCTRL_PIN(36, "P KS IN1"),
        PINCTRL_PIN(37, "P KS IN2"),
        PINCTRL_PIN(38, "P KS IN3"),
        PINCTRL_PIN(39, "P KS OUT0"),
        PINCTRL_PIN(40, "P KS OUT1"),
        PINCTRL_PIN(41, "P KS OUT2"),
        PINCTRL_PIN(42, "OEP"),
        PINCTRL_PIN(43, "OEN"),
        PINCTRL_PIN(44, "ODP"),
        PINCTRL_PIN(45, "ODN"),
        PINCTRL_PIN(46, "OCP"),
        PINCTRL_PIN(47, "OCN"),
        PINCTRL_PIN(48, "OBP"),
        PINCTRL_PIN(49, "OBN"),
        PINCTRL_PIN(50, "OAP"),
        PINCTRL_PIN(51, "OAN"),
        PINCTRL_PIN(52, "EEP"),
        PINCTRL_PIN(53, "EEN"),
        PINCTRL_PIN(54, "EDP"),
        PINCTRL_PIN(55, "EDN"),
        PINCTRL_PIN(56, "ECP"),
        PINCTRL_PIN(57, "ECN"),
        PINCTRL_PIN(58, "EBP"),
        PINCTRL_PIN(59, "EBN"),
        PINCTRL_PIN(60, "EAP"),
        PINCTRL_PIN(61, "EAN"),
        PINCTRL_PIN(62, "P LCD0 D18"),
        PINCTRL_PIN(63, "P LCD0 D17"),
        PINCTRL_PIN(64, "P LCD0 D16"),
        PINCTRL_PIN(65, "P LCD0 D9"),
        PINCTRL_PIN(66, "P LCD0 D8"),
        PINCTRL_PIN(67, "P LCD0 D2"),
        PINCTRL_PIN(68, "P LCD0 D1"),
        PINCTRL_PIN(69, "P LCD0 D0"),
        PINCTRL_PIN(70, "P LCD0 DCLK1"),
        PINCTRL_PIN(71, "P LCD0 HSYNC1"),
        PINCTRL_PIN(72, "P LCD0 VSYNC1"),
        PINCTRL_PIN(73, "P LCD0 LDE1"),
        PINCTRL_PIN(74, "P SD0 D0"),
        PINCTRL_PIN(75, "P SD0 D1"),
        PINCTRL_PIN(76, "P SD0 D2"),
        PINCTRL_PIN(77, "P SD0 D3"),
        PINCTRL_PIN(78, "P SD0 D4"),
        PINCTRL_PIN(79, "P SD0 D5"),
        PINCTRL_PIN(80, "P SD0 D6"),
        PINCTRL_PIN(81, "P SD0 D7"),
        PINCTRL_PIN(82, "P SD0 CMD"),
        PINCTRL_PIN(83, "P SD0 CLK"),
        PINCTRL_PIN(84, "P SD1 CMD"),
        PINCTRL_PIN(85, "P SD1 CLK"),
        PINCTRL_PIN(86, "P SPI0 SCLK"),
        PINCTRL_PIN(87, "P SPI0 SS"),
        PINCTRL_PIN(88, "P SPI0 MISO"),
        PINCTRL_PIN(89, "P SPI0 MOSI"),
        PINCTRL_PIN(90, "P UART0 RX"),
        PINCTRL_PIN(91, "P UART0 TX"),
        PINCTRL_PIN(92, "P I2C0 SCLK"),
        PINCTRL_PIN(93, "P I2C0 SDATA"),
        PINCTRL_PIN(94, "P SENS0 PCLK"),
        PINCTRL_PIN(95, "P SENS1 PCLK"),
        PINCTRL_PIN(96, "P SENS1 VSYNC"),
        PINCTRL_PIN(97, "P SENS1 HSYNC"),
        PINCTRL_PIN(98, "P SENS1 D0"),
        PINCTRL_PIN(99, "P SENS1 D1"),
        PINCTRL_PIN(100, "P SENS1 D2"),
        PINCTRL_PIN(101, "P SENS1 D3"),
        PINCTRL_PIN(102, "P SENS1 D4"),
        PINCTRL_PIN(103, "P SENS1 D5"),
        PINCTRL_PIN(104, "P SENS1 D6"),
        PINCTRL_PIN(105, "P SENS1 D7"),
        PINCTRL_PIN(106, "P SENS1 CKOUT"),
        PINCTRL_PIN(107, "P SENS0 CKOUT"),
        PINCTRL_PIN(108, "P DNAND ALE"),
        PINCTRL_PIN(109, "P DNAND CLE"),
        PINCTRL_PIN(110, "P DNAND CEB0"),
        PINCTRL_PIN(111, "P DNAND CEB1"),
        PINCTRL_PIN(112, "P DNAND CEB2"),
        PINCTRL_PIN(113, "P DNAND CEB3"),
        PINCTRL_PIN(114, "P UART2 RX"),
        PINCTRL_PIN(115, "P UART2 TX"),
        PINCTRL_PIN(116, "P UART2 RTSB"),
        PINCTRL_PIN(117, "P UART2 CTSB"),
        PINCTRL_PIN(118, "P UART3 RX"),
        PINCTRL_PIN(119, "P UART3 TX"),
        PINCTRL_PIN(120, "P UART3 RTSB"),
        PINCTRL_PIN(121, "P UART3 CTSB"),
        PINCTRL_PIN(122, "P UART4 RX"),
        PINCTRL_PIN(123, "P UART4 TX")
        PINCTRL_PIN(124, "P PCM1 IN"),
        PINCTRL_PIN(125, "P PCM1 CLK0"),
        PINCTRL_PIN(126, "P PCM1 SYNC"),
        PINCTRL_PIN(127, "P PCM1 OUT"),
        PINCTRL_PIN(128, "P I2C1 SCLK"),
        PINCTRL_PIN(129, "P I2C1 SDATA"),
        PINCTRL_PIN(130, "P I2C2 SCLK"),
        PINCTRL_PIN(131, "P I2C2 SDATA"),
        PINCTRL_PIN(132, "RESERVED"),
        PINCTRL_PIN(133, "P DSI DN1"),
        PINCTRL_PIN(134, "P DSI DP1"),
        PINCTRL_PIN(135, "P DSI DN0"),
        PINCTRL_PIN(136, "P DSI DP0"),
        PINCTRL_PIN(137, "P DSI CN"),
        PINCTRL_PIN(138, "P DSI CP"),
        PINCTRL_PIN(139, "P DSI DN2"),
        PINCTRL_PIN(140, "P DSI DP2"),
        PINCTRL_PIN(141, "P DSI DN3"),
        PINCTRL_PIN(142, "P DSI DP3"),
        PINCTRL_PIN(143, "P CSI DN0"),
        PINCTRL_PIN(144, "P CSI DP0"),
        PINCTRL_PIN(145, "P CSI DN1"),
        PINCTRL_PIN(146, "P CSI DP1"),
        PINCTRL_PIN(147, "P CSI CN"),
        PINCTRL_PIN(148, "P CSI CP"),
        PINCTRL_PIN(149, "P CSI DN2"),
        PINCTRL_PIN(150, "P CSI DP2"),
        PINCTRL_PIN(151, "P CSI DN3"),
        PINCTRL_PIN(152, "P CSI DP3"),
        PINCTRL_PIN(153, "P DNAND D0"),
        PINCTRL_PIN(154, "P DNAND D1"),
        PINCTRL_PIN(155, "P DNAND D2"),
        PINCTRL_PIN(156, "P DNAND D3"),
        PINCTRL_PIN(157, "P DNAND D4"),
        PINCTRL_PIN(158, "P DNAND D5"),
        PINCTRL_PIN(159, "P DNAND D6"),
        PINCTRL_PIN(160, "P DNAND D7"),
        PINCTRL_PIN(161, "P DNAND WRB"),
        PINCTRL_PIN(162, "P DNAND RDB"),
        PINCTRL_PIN(163, "P DNAND RDBN"),
        PINCTRL_PIN(164, "P DNAND RB"),
        PINCTRL_PIN(165, "P TST OUT"),
};

/**
 * @dev: a pointer back to containing device
 * @virtbase: the offset to the controller in virtual memory
 */
struct u300_pmx {
        struct device *dev;
        struct pinctrl_dev *pctl;
        void __iomem *virtbase;
};

/**
 * u300_pmx_registers - the array of registers read/written for each pinmux
 * shunt setting
 */
const u32 u300_pmx_registers[] = {
        U300_SYSCON_PMC1LR,
        U300_SYSCON_PMC1HR,
        U300_SYSCON_PMC2R,
        U300_SYSCON_PMC3R,
        U300_SYSCON_PMC4R,
};

/**
 * struct u300_pin_group - describes a U300 pin group
 * @name: the name of this specific pin group
 * @pins: an array of discrete physical pins used in this group, taken
 *      from the driver-local pin enumeration space
 * @num_pins: the number of pins in this group array, i.e. the number of
 *      elements in .pins so we can iterate over that array
 */
struct u300_pin_group {
        const char *name;
        const unsigned int *pins;
        const unsigned num_pins;
};

/**
 * struct pmx_onmask - mask bits to enable/disable padmux
 * @mask: mask bits to disable
 * @val: mask bits to enable
 *
 * onmask lazy dog:
 * onmask = {
 *   {"PMC1LR" mask, "PMC1LR" value},
 *   {"PMC1HR" mask, "PMC1HR" value},
 *   {"PMC2R"  mask, "PMC2R"  value},
 *   {"PMC3R"  mask, "PMC3R"  value},
 *   {"PMC4R"  mask, "PMC4R"  value}
 * }
 */
struct u300_pmx_mask {
        u16 mask;
        u16 bits;
};

/* The chip power pins are VDD, GND, VDDIO and VSSIO */
static const unsigned power_pins[] = { 0, 1, 3, 31, 46, 47, 49, 50, 61, 62, 63,
        64, 78, 79, 80, 81, 92, 93, 94, 95, 101, 102, 103, 104, 115, 116, 117,
        118, 130, 131, 132, 133, 145, 146, 147, 148, 159, 160, 172, 173, 174,
        175, 187, 188, 189, 190, 201, 202, 211, 212, 213, 214, 215, 218, 223,
        224, 225, 226, 231, 232, 237, 238, 239, 240, 245, 246, 251, 252, 256,
        257, 258, 259, 264, 265, 270, 271, 276, 277, 278, 279, 284, 285, 290,
        291, 295, 296, 299, 300, 301, 302, 303, 309, 310, 311, 312, 319, 320,
        321, 322, 329, 330, 331, 332, 341, 342, 343, 344, 358, 359, 360, 361,
        372, 373, 374, 375, 388, 389, 390, 391, 402, 403, 404, 405, 413, 414,
        415, 416, 427, 428, 429, 430, 443, 444, 455, 456, 457, 458 };
static const unsigned emif0_pins[] = { 355, 356, 357, 362, 363, 364, 365, 366,
        367, 368, 369, 370, 371, 376, 377, 378, 379, 380, 381, 382, 383, 384,
        385, 386, 387, 393, 394, 395, 396, 397, 398, 406, 407, 410, 411, 412,
        417, 418 };
static const unsigned emif1_pins[] = { 216, 217, 219, 220, 221, 222, 227, 228,
        229, 230, 233, 234, 235, 236, 241, 242, 243, 244, 247, 248, 249, 250,
        253, 254, 255, 260, 261, 262, 263, 266, 267, 268, 269, 272, 273, 274,
        275, 280, 281, 282, 283, 286, 287, 288, 289, 292, 293, 294, 297, 298,
        304, 305, 306, 307, 308, 313, 314, 315 };
static const unsigned uart0_pins[] = { 134, 135, 136, 137 };
static const unsigned mmc0_pins[] = { 166, 167, 168, 169, 170, 171, 176, 177 };
static const unsigned spi0_pins[] = { 420, 421, 422, 423, 424, 425 };

static const struct u300_pmx_mask emif0_mask[] = {
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
};

static const struct u300_pmx_mask emif1_mask[] = {
        /*
         * This connects the SDRAM to CS2 and a NAND flash to
         * CS0 on the EMIF.
         */
        {
                U300_SYSCON_PMC1LR_EMIF_1_CS2_MASK |
                U300_SYSCON_PMC1LR_EMIF_1_CS1_MASK |
                U300_SYSCON_PMC1LR_EMIF_1_CS0_MASK |
                U300_SYSCON_PMC1LR_EMIF_1_MASK,
                U300_SYSCON_PMC1LR_EMIF_1_CS2_SDRAM |
                U300_SYSCON_PMC1LR_EMIF_1_CS1_STATIC |
                U300_SYSCON_PMC1LR_EMIF_1_CS0_NFIF |
                U300_SYSCON_PMC1LR_EMIF_1_SDRAM0
        },
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
};

static const struct u300_pmx_mask uart0_mask[] = {
        {0, 0},
        {
                U300_SYSCON_PMC1HR_APP_UART0_1_MASK |
                U300_SYSCON_PMC1HR_APP_UART0_2_MASK,
                U300_SYSCON_PMC1HR_APP_UART0_1_UART0 |
                U300_SYSCON_PMC1HR_APP_UART0_2_UART0
        },
        {0, 0},
        {0, 0},
        {0, 0},
};

static const struct u300_pmx_mask mmc0_mask[] = {
        { U300_SYSCON_PMC1LR_MMCSD_MASK, U300_SYSCON_PMC1LR_MMCSD_MMCSD},
        {0, 0},
        {0, 0},
        {0, 0},
        { U300_SYSCON_PMC4R_APP_MISC_12_MASK,
          U300_SYSCON_PMC4R_APP_MISC_12_APP_GPIO }
};

static const struct u300_pmx_mask spi0_mask[] = {
        {0, 0},
        {
                U300_SYSCON_PMC1HR_APP_SPI_2_MASK |
                U300_SYSCON_PMC1HR_APP_SPI_CS_1_MASK |
                U300_SYSCON_PMC1HR_APP_SPI_CS_2_MASK,
                U300_SYSCON_PMC1HR_APP_SPI_2_SPI |
                U300_SYSCON_PMC1HR_APP_SPI_CS_1_SPI |
                U300_SYSCON_PMC1HR_APP_SPI_CS_2_SPI
        },
        {0, 0},
        {0, 0},
        {0, 0}
};

static const struct u300_pin_group u300_pin_groups[] = {
        {
                .name = "powergrp",
                .pins = power_pins,
                .num_pins = ARRAY_SIZE(power_pins),
        },
        {
                .name = "emif0grp",
                .pins = emif0_pins,
                .num_pins = ARRAY_SIZE(emif0_pins),
        },
        {
                .name = "emif1grp",
                .pins = emif1_pins,
                .num_pins = ARRAY_SIZE(emif1_pins),
        },
        {
                .name = "uart0grp",
                .pins = uart0_pins,
                .num_pins = ARRAY_SIZE(uart0_pins),
        },
        {
                .name = "mmc0grp",
                .pins = mmc0_pins,
                .num_pins = ARRAY_SIZE(mmc0_pins),
        },
        {
                .name = "spi0grp",
                .pins = spi0_pins,
                .num_pins = ARRAY_SIZE(spi0_pins),
        },
};

static int u300_get_groups_count(struct pinctrl_dev *pctldev)
{
        return ARRAY_SIZE(u300_pin_groups);
}

static const char *u300_get_group_name(struct pinctrl_dev *pctldev,
                                       unsigned selector)
{
        return u300_pin_groups[selector].name;
}

static int u300_get_group_pins(struct pinctrl_dev *pctldev, unsigned selector,
                               const unsigned **pins,
                               unsigned *num_pins)
{
        *pins = u300_pin_groups[selector].pins;
        *num_pins = u300_pin_groups[selector].num_pins;
        return 0;
}

static void u300_pin_dbg_show(struct pinctrl_dev *pctldev, struct seq_file *s,
                   unsigned offset)
{
        seq_printf(s, " " DRIVER_NAME);
}

static const struct pinctrl_ops u300_pctrl_ops = {
        .get_groups_count = u300_get_groups_count,
        .get_group_name = u300_get_group_name,
        .get_group_pins = u300_get_group_pins,
        .pin_dbg_show = u300_pin_dbg_show,
};

/*
 * Here we define the available functions and their corresponding pin groups
 */

/**
 * struct u300_pmx_func - describes U300 pinmux functions
 * @name: the name of this specific function
 * @groups: corresponding pin groups
 * @onmask: bits to set to enable this when doing pin muxing
 */
struct u300_pmx_func {
        const char *name;
        const char * const *groups;
        const unsigned num_groups;
        const struct u300_pmx_mask *mask;
};

static const char * const powergrps[] = { "powergrp" };
static const char * const emif0grps[] = { "emif0grp" };
static const char * const emif1grps[] = { "emif1grp" };
static const char * const uart0grps[] = { "uart0grp" };
static const char * const mmc0grps[] = { "mmc0grp" };
static const char * const spi0grps[] = { "spi0grp" };

static const struct u300_pmx_func u300_pmx_functions[] = {
        {
                .name = "power",
                .groups = powergrps,
                .num_groups = ARRAY_SIZE(powergrps),
                /* Mask is N/A */
        },
        {
                .name = "emif0",
                .groups = emif0grps,
                .num_groups = ARRAY_SIZE(emif0grps),
                .mask = emif0_mask,
        },
        {
                .name = "emif1",
                .groups = emif1grps,
                .num_groups = ARRAY_SIZE(emif1grps),
                .mask = emif1_mask,
        },
        {
                .name = "uart0",
                .groups = uart0grps,
                .num_groups = ARRAY_SIZE(uart0grps),
                .mask = uart0_mask,
        },
        {
                .name = "mmc0",
                .groups = mmc0grps,
                .num_groups = ARRAY_SIZE(mmc0grps),
                .mask = mmc0_mask,
        },
        {
                .name = "spi0",
                .groups = spi0grps,
                .num_groups = ARRAY_SIZE(spi0grps),
                .mask = spi0_mask,
        },
};

static void u300_pmx_endisable(struct u300_pmx *upmx, unsigned selector,
                               bool enable)
{
        u16 regval, val, mask;
        int i;
        const struct u300_pmx_mask *upmx_mask;

        upmx_mask = u300_pmx_functions[selector].mask;
        for (i = 0; i < ARRAY_SIZE(u300_pmx_registers); i++) {
                if (enable)
                        val = upmx_mask->bits;
                else
                        val = 0;

                mask = upmx_mask->mask;
                if (mask != 0) {
                        regval = readw(upmx->virtbase + u300_pmx_registers[i]);
                        regval &= ~mask;
                        regval |= val;
                        writew(regval, upmx->virtbase + u300_pmx_registers[i]);
                }
                upmx_mask++;
        }
}

static int u300_pmx_enable(struct pinctrl_dev *pctldev, unsigned selector,
                           unsigned group)
{
        struct u300_pmx *upmx;

        /* There is nothing to do with the power pins */
        if (selector == 0)
                return 0;

        upmx = pinctrl_dev_get_drvdata(pctldev);
        u300_pmx_endisable(upmx, selector, true);

        return 0;
}

static void u300_pmx_disable(struct pinctrl_dev *pctldev, unsigned selector,
                             unsigned group)
{
        struct u300_pmx *upmx;

        /* There is nothing to do with the power pins */
        if (selector == 0)
                return;

        upmx = pinctrl_dev_get_drvdata(pctldev);
        u300_pmx_endisable(upmx, selector, false);
}

static int u300_pmx_get_funcs_count(struct pinctrl_dev *pctldev)
{
        return ARRAY_SIZE(u300_pmx_functions);
}

static const char *u300_pmx_get_func_name(struct pinctrl_dev *pctldev,
                                          unsigned selector)
{
        return u300_pmx_functions[selector].name;
}

static int u300_pmx_get_groups(struct pinctrl_dev *pctldev, unsigned selector,
                               const char * const **groups,
                               unsigned * const num_groups)
{
        *groups = u300_pmx_functions[selector].groups;
        *num_groups = u300_pmx_functions[selector].num_groups;
        return 0;
}

static const struct pinmux_ops u300_pmx_ops = {
        .get_functions_count = u300_pmx_get_funcs_count,
        .get_function_name = u300_pmx_get_func_name,
        .get_function_groups = u300_pmx_get_groups,
        .enable = u300_pmx_enable,
        .disable = u300_pmx_disable,
};

static int u300_pin_config_get(struct pinctrl_dev *pctldev, unsigned pin,
                               unsigned long *config)
{
        struct pinctrl_gpio_range *range =
                pinctrl_find_gpio_range_from_pin(pctldev, pin);

        /* We get config for those pins we CAN get it for and that's it */
        if (!range)
                return -ENOTSUPP;

        return u300_gpio_config_get(range->gc,
                                    (pin - range->pin_base + range->base),
                                    config);
}

static int u300_pin_config_set(struct pinctrl_dev *pctldev, unsigned pin,
                               unsigned long *configs, unsigned num_configs)
{
        struct pinctrl_gpio_range *range =
                pinctrl_find_gpio_range_from_pin(pctldev, pin);
        int ret, i;

        if (!range)
                return -EINVAL;

        for (i = 0; i < num_configs; i++) {
                /* Note: none of these configurations take any argument */
                ret = u300_gpio_config_set(range->gc,
                        (pin - range->pin_base + range->base),
                        pinconf_to_config_param(configs[i]));
                if (ret)
                        return ret;
        } /* for each config */

        return 0;
}

static const struct pinconf_ops u300_pconf_ops = {
        .is_generic = true,
        .pin_config_get = u300_pin_config_get,
        .pin_config_set = u300_pin_config_set,
};

static struct pinctrl_desc u300_pmx_desc = {
        .name = DRIVER_NAME,
        .pins = u300_pads,
        .npins = ARRAY_SIZE(u300_pads),
        .pctlops = &u300_pctrl_ops,
        .pmxops = &u300_pmx_ops,
        .confops = &u300_pconf_ops,
        .owner = THIS_MODULE,
};

static int u300_pmx_probe(struct platform_device *pdev)
{
        struct u300_pmx *upmx;
        struct resource *res;

        /* Create state holders etc for this driver */
        upmx = devm_kzalloc(&pdev->dev, sizeof(*upmx), GFP_KERNEL);
        if (!upmx)
                return -ENOMEM;

        upmx->dev = &pdev->dev;

        res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        upmx->virtbase = devm_ioremap_resource(&pdev->dev, res);
        if (IS_ERR(upmx->virtbase))
                return PTR_ERR(upmx->virtbase);

        upmx->pctl = pinctrl_register(&u300_pmx_desc, &pdev->dev, upmx);
        if (!upmx->pctl) {
                dev_err(&pdev->dev, "could not register U300 pinmux driver\n");
                return -EINVAL;
        }

        platform_set_drvdata(pdev, upmx);

        dev_info(&pdev->dev, "initialized U300 pin control driver\n");

        return 0;
}

static int u300_pmx_remove(struct platform_device *pdev)
{
        struct u300_pmx *upmx = platform_get_drvdata(pdev);

        pinctrl_unregister(upmx->pctl);

        return 0;
}

static const struct of_device_id u300_pinctrl_match[] = {
        { .compatible = "stericsson,pinctrl-u300" },
        {},
};


static struct platform_driver u300_pmx_driver = {
        .driver = {
                .name = DRIVER_NAME,
                .owner = THIS_MODULE,
                .of_match_table = u300_pinctrl_match,
        },
        .probe = u300_pmx_probe,
        .remove = u300_pmx_remove,
};

static int __init u300_pmx_init(void)
{
        return platform_driver_register(&u300_pmx_driver);
}
arch_initcall(u300_pmx_init);

static void __exit u300_pmx_exit(void)
{
        platform_driver_unregister(&u300_pmx_driver);
}
module_exit(u300_pmx_exit);

MODULE_AUTHOR("Linus Walleij <linus.walleij@linaro.org>");
MODULE_DESCRIPTION("U300 pin control driver");
MODULE_LICENSE("GPL v2");
