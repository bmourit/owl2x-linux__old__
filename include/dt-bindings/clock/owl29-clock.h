/*
 * Copyright (C) 2014 
 * Author: B. Mouritsen <bnmguy@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Device Tree binding constants for ATM702x clock controller.
*/

#ifndef _DT_BINDINGS_CLOCK_ATM702X_H
#define _DT_BINDINGS_CLOCK_ATM702X_H

/* Dummy Clock */
#define OWL29_CLK_DUMMY           0

/* Core Clocks */
#define OWL29_LOSC                1
#define OWL29_HOSC                2
#define OWL29_COREPLL             3
#define OWL29_DEVPLL              5
#define OWL29_NANDPLL             6
#define OWL29_DDRPLL              7
#define OWL29_DISPLAYPLL          8
#define OWL29_AUDIOPLL            9
#define OWL29_TVOUT0PLL           10
#define OWL29_TVOUT1PLL           11
#define OWL29_DEEPCOLORPLL        12
#define OWL29_120MPLL             13
#define OWL29_CPUCLK              14
#define OWL29_DEVCLK              15
#define OWL29_HCLK                16
#define OWL29_PCLK                17
#define OWL29_NIC_DCU             18

/* Gates for DEVCLKEN0 Special Clocks */
#define OWL29_SCLK_RESERV0      100
#define OWL29_SCLK_DMAC         101
#define OWL29_SCLK_NOR          102
#define OWL29_SCLK_DDRC         103
#define OWL29_SCLK_NANDC        104
#define OWL29_SCLK_SD0          105
#define OWL29_SCLK_SD1          106
#define OWL29_SCLK_SD2          107
#define OWL29_SCLK_DE           108
#define OWL29_SCLK_LCD0         109
#define OWL29_SCLK_LCD1         110
#define OWL29_SCLK_LVDS         111
#define OWL29_SCLK_RESERV1      112
#define OWL29_SCLK_CSI          113
#define OWL29_SCLK_BISP         114
#define OWL29_SCLK_RESERV2      115
#define OWL29_SCLK_LENS         116
#define OWL29_SCLK_KEY          117
#define OWL29_SCLK_GPIO         118
#define OWL29_SCLK_RESERV3      119
#define OWL29_SCLK_I2STX        120
#define OWL29_SCLK_I2SRX        121
#define OWL29_SCLK_HDMIA        122
#define OWL29_SCLK_SPDIF        123
#define OWL29_SCLK_PCM0         124
#define OWL29_SCLK_VDE          125
#define OWL29_SCLK_VCE          126
#define OWL29_SCLK_GPU2D        127
#define OWL29_SCLK_SHARESRAM    128
#define OWL29_SCLK_GPU2DLP      129
#define OWL29_SCLK_GPU3D        130
#define OWL29_SCLK_GPU3DLP      131
    /* DEVCLKEN1 */
#define OWL29_SCLK_TVIN         132
#define OWL29_SCLK_TV24M        133
#define OWL29_SCLK_CVBS         134
#define OWL29_SCLK_HDMI         135
#define OWL29_SCLK_YPbPrVGA     136
#define OWL29_SCLK_TVOUT        137
#define OWL29_SCLK_UART0        138
#define OWL29_SCLK_UART1        139
#define OWL29_SCLK_UART2        140
#define OWL29_SCLK_IRC          141
#define OWL29_SCLK_SPI0         142
#define OWL29_SCLK_SPI1         143
#define OWL29_SCLK_SPI2         144
#define OWL29_SCLK_SPI3         145
#define OWL29_SCLK_I2C0         146
#define OWL29_SCLK_I2C1         147
#define OWL29_SCLK_PCM1         148
#define OWL29_SCLK_RESERV4      149
#define OWL29_SCLK_RESERV5      150
#define OWL29_SCLK_UART3        151
#define OWL29_SCLK_UART4        152
#define OWL29_SCLK_UART5        153
#define OWL29_SCLK_ETHERNET     154
#define OWL29_SCLK_PWM0         155
#define OWL29_SCLK_PWM1         156
#define OWL29_SCLK_PWM2         157
#define OWL29_SCLK_PWM3         158
#define OWL29_SCLK_TIMER        159
#define OWL29_SCLK_GPS          160
#define OWL29_SCLK_RESERV6      161
#define OWL29_SCLK_I2C2         162
#define OWL29_SCLK_RESERV7      163
#define OWL29_SCLK_MAX          164

/* Gate Clocks */
#define OWL29_CLK_LCD0                128
#define OWL29_CLK_LCD1                129
#define OWL29_CLK_BISP                130
#define OWL29_CLK_SENSOR              131 
#define OWL29_CLK_TVOUTMUX0           133
#define OWL29_CLK_TVOUT0              134
#define OWL29_CLK_I2STX               135
#define OWL29_CLK_I2SRX               136
#define OWL29_CLK_HDMI                137
#define OWL29_CLK_SPDIF               138
#define OWL29_CLK_LENS                139
#define OWL29_CLK_DDR                 140
#define OWL29_CLK_PWM0                141
#define OWL29_CLK_PWM1                142
#define OWL29_CLK_PWM2                143
#define OWL29_CLK_PWM3                144
#define OWL29_CLK_IMG5                145
#define OWL29_CLK_DMA                 146
#define OWL29_CLK_TWD                 147
#define OWL29_CLK_L2                  148
#define OWL29_CLK_HDMI24M             149
#define OWL29_CLK_TIMER               150
#define OWL29_CLK_IRC                 151
#define OWL29_CLK_I2C0                152
#define OWL29_CLK_I2C1                153
#define OWL29_CLK_I2C2                154
#define OWL29_CLK_TV24M               155
#define OWL29_CLK_PCM0                156
#define OWL29_CLK_PCM1                157
#define OWL29_CLK_LVDS                158
#define OWL29_CLK_VCE                 159
#define OWL29_CLK_VDE                 160
#define OWL29_CLK_GPU2D               161
#define OWL29_CLK_GPU2D_NIC           162
#define OWL29_CLK_GPU3D               163
#define OWL29_CLK_GPU3D_NIC           164
#define OWL29_CLK_NANDC               166
#define OWL29_CLK_NANDC_NIC           167
#define OWL29_CLK_GPS2X               168
#define OWL29_CLK_ETHERNET            169
#define OWL29_CLK_SD0                 170
#define OWL29_CLK_SD1                 171
#define OWL29_CLK_SD2                 172
#define OWL29_CLK_UART0               176      
#define OWL29_CLK_UART1               177
#define OWL29_CLK_UART2               178
#define OWL29_CLK_UART3               179
#define OWL29_CLK_UART4               180
#define OWL29_CLK_UART5               181
#define OWL29_CLK_LCD                 182
#define OWL29_CLK_CSI                 183
#define OWL29_CLK_DE0                 184
#define OWL29_CLK_DE1                 185
#define OWL29_CLK_DE2                 186
#define OWL29_CLK_DE3                 187
#define OWL29_CLK_DE_WB               188
#define OWL29_CLK_SOUT0               189
#define OWL29_CLK_SOUT1               190
#define OWL29_CLK_CVBS                191

/* Div Clocks */
#define OWL29_CLK_DIV_UART            256
#define OWL29_CLK_DIV_ROUND_1_1024    257 
#define DIV_FIXED                     258

#define OWL29_CLK_NR_CLKS             259

#endif /* _DT_BINDINGS_CLOCK_ATM702X_H */
