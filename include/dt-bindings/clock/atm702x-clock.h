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

/* Core Clocks */
#define CLK_LOSC                1
#define CLK_HOSC                2
#define CLK_GPLL                3
#define CLK_COREPLL             4
#define CLK_DEVPLL              5
#define CLK_NANDPLL             6
#define CLK_DDRPLL              7
#define CLK_DISPLAYPLL          8
#define CLK_AUDIOPLL            9
#define CLK_TVOUT0PLL           10
#define CLK_TVOUT1PLL           11
#define CLK_DEEPCOLORPLL        12
#define CLK_120MPLL             13
#define CLK_CPUCLK              14
#define CLK_DEVCLK              15
#define CLK_AHBCLK              16
#define CLK_APBCLK              17
#define CLK_NIC_DCU             18

/* Gate Clocks */
#define CLK_LCD0                128
#define CLK_LCD1                129  //atm7029
#define CLK_BISP                130  //atm7029
#define CLK_SENSOR              131  //atm7029
#define CLK_SI                  132  //atm7029b
#define CLK_TVOUTMUX0           133
#define CLK_TVOUT0              134
#define CLK_I2STX               135
#define CLK_I2SRX               136
#define CLK_HDMI                137
#define CLK_SPDIF               138
#define CLK_LENS                139
#define CLK_DDR                 140
#define CLK_PWM0                141
#define CLK_PWM1                142
#define CLK_PWM2                143
#define CLK_PWM3                144
#define CLK_IMG5                145
#define CLK_DMA                 146
#define CLK_TWD                 147
#define CLK_L2                  148
#define CLK_HDMI24M             149
#define CLK_TIMER               150
#define CLK_IRC                 151
#define CLK_I2C0                152
#define CLK_I2C1                153
#define CLK_I2C2                154
#define CLK_TV24M               155
#define CLK_PCM0                156
#define CLK_PCM1                157
#define CLK_LVDS                158
#define CLK_VCE                 159
#define CLK_VDE                 160
#define CLK_GPU2D               161      //atm7029 (vivante)
#define CLK_GPU2D_NIC           162      //atm7029 (vivante)
#define CLK_GPU3D               163      //atm7029 (vivante)
#define CLK_GPU3D_NIC           164      //atm7029 (vivante)
#define CLK_GPU                 165      //stm7029b (powervr)
#define CLK_NANDC               166
#define CLK_NANDC_NIC           167
#define CLK_GPS2x               168
#define CLK_ETHERNET            169
#define CLK_SD0                 170
#define CLK_SD1                 171
#define CLK_SD2                 172
#define CLK_SD0_NIC             173
#define CLK_SD1_NIC             174
#define CLK_SD2_NIC             175
#define CLK_UART0               176      
#define CLK_UART1               177
#define CLK_UART2               178
#define CLK_UART3               179
#define CLK_UART4               180
#define CLK_UART5               181
#define CLK_LCD                 182
#define CLK_CSI                 183
#define CLK_DE                  184
#define CLK_DE1                 185
#define CLK_DE2                 186
#define CLK_DE3                 187
#define CLK_DE_WB               188
#define CLK_SENSOR_OUT0         189
#define CLK_SENSOR_OUT1         190      //atm7029
#define CLK_CVBS                191
#define CLK_THERMAL             192
#define CLK_LEAKAGE             193
#define CLK_SPEED               194

/* Div Clocks */
#define CLK_DIV_UART            256
#define CLK_DIV_ROUND_1_1024    257 
#define DIV_FIXED               258

#define CLK_NR_CLKS             259

#endif /* _DT_BINDINGS_CLOCK_ATM702X_H */
