/*
 * Audio Driver for Actions ATC260x PMIC
 *
 * Copyright (C) 2014
 * Author: B. Mouritsen <bnmguy@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef __LINUX_MFD_WM8350_AUDIO_H_
#define __LINUX_MFD_WM8350_AUDIO_H_

#include <linux/platform_device.h>

/**
 * 0x0400 (0x00) In/Out Control
 */
#define ATC260X_DAC_RES0_MASK                   0xE000  /* DAC_RES0 - 15:13 */
#define ATC260X_DAC_RES0_SHIFT                      13
#define ATC260X_DAC_RES0_WIDTH                       3
#define ATC260X_DAC_EIDR                        0x1000  /* DAC_EIDR (Earphone Detection) */
#define ATC260X_DAC_EIDR_MASK                   0x1000
#define ATC260X_DAC_EIDR_SHIFT                      12
#define ATC260X_DAC_EIDR_WIDTH                       1
#define ATC260X_DAC_MCLK_DIV                    0x0800  /* DAC_MCLK_DIV (MCLK DAC Divisor) */
#define ATC260X_DAC_MCLK_DIV_MASK               0x0800
#define ATC260X_DAC_MCLK_DIV_SHIFT                  11
#define ATC260X_DAC_MCLK_DIV_WIDTH                   1
#define ATC260X_DAC_OHSCIEN                     0x0400  /* DAC_OHSCIEN (Heat/Short Circuit Warning) */
#define ATC260X_DAC_OHSCIEN_MASK                0x0400
#define ATC260X_DAC_OHSCIEN_SHIFT                   10
#define ATC260X_DAC_OHSCIEN_WIDTH                    1
#define ATC260X_DAC_OUTCIEN                     0x0200  /* DAC_OUTCIEN (Direct Drive Output Over Current Status IRQ) */
#define ATC260X_DAC_OUTCIEN_MASK                0x0200
#define ATC260X_DAC_OUTCIEN_SHIFT                    9
#define ATC260X_DAC_OUTCIEN_WIDTH                    1
#define ATC260X_DAC_OUTEN                       0x0100  /* DAC_OUTEN (DAC I2S Output Enable) */
#define ATC260X_DAC_OUTEN_MASK                  0x0100
#define ATC260X_DAC_OUTEN_SHIFT                      8
#define ATC260X_DAC_OUTEN_WIDTH                      1
#define ATC260X_DAC_OUTMD                       0x0080  /* DAC_OUTMD (I2S Output Mode) */
#define ATC260X_DAC_OUTMD_MASK                  0x0080
#define ATC260X_DAC_OUTMD_SHIFT                      7
#define ATC260X_DAC_OUTMD_WIDTH                      1
#define ATC260X_DAC_IMS_MASK                    0x0060  /* DAC_IMS (I2S Rx/Tx Mode) - 6:5 */
#define ATC260X_DAC_IMS_SHIFT                        5
#define ATC260X_DAC_IMS_WIDTH                        2
#define ATC260X_DAC_LB                          0x0010  /* DAC_LB (I2S Loop Back Enable) */
#define ATC260X_DAC_LB_MASK                     0x0010
#define ATC260X_DAC_LB_SHIFT                         4
#define ATC260X_DAC_LB_WIDTH                         1
#define ATC260X_DAC_IMD_MASK                    0x000C  /* DAC_IMD (I2C Input Mode) - 3:2 */
#define ATC260X_DAC_IMD_SHIFT                        2
#define ATC260X_DAC_IMD_WIDTH                        2
#define ATC260X_DAC_INEN                        0x0002  /* DAC_INEN (I2S Output Mode) */
#define ATC260X_DAC_INEN_MASK                   0x0002
#define ATC260X_DAC_INEN_SHIFT                       1
#define ATC260X_DAC_INEN_WIDTH                       1
#define ATC260X_DAC_RES1                        0x0001  /* DAC_RES1 (I2S Output Mode) */
#define ATC260X_DAC_RES1_MASK                   0x0001
#define ATC260X_DAC_RES1_SHIFT                       0
#define ATC260X_DAC_RES1_WIDTH                       1

/**
 * 0x0402 (0x02) DAC Sample Rate Filter Control
 */
#define ATC260X_DAC_RES2_MASK                   0xE000  /* DAC_RES2 - 15:13 */
#define ATC260X_DAC_RES2_SHIFT                      13
#define ATC260X_DAC_RES2_WIDTH                       3
#define ATC260X_DAC_DITH_SBLR                   0x1000  /* DAC_DAC_DITH_SB_LR */
#define ATC260X_DAC_DITH_SBLR_MASK              0x1000
#define ATC260X_DAC_DITH_SBLR_SHIFT                 12
#define ATC260X_DAC_DITH_SBLR_WIDTH                  1
#define ATC260X_DAC_DITH_SLR                    0x0800  /* DAC_DEDS (DACSL&SR EN_DITH) */
#define ATC260X_DAC_DITH_SLR_MASK               0x0800
#define ATC260X_DAC_DITH_SLR_SHIFT                  11
#define ATC260X_DAC_DITH_SLR_WIDTH                   1
#define ATC260X_DAC_DITH_SWC                    0x0400  /* DAC_DEDSWC (DACSW&C EN_DITH) */
#define ATC260X_DAC_DITH_SWC_MASK               0x0400
#define ATC260X_DAC_DITH_SWC_SHIFT                  10
#define ATC260X_DAC_DITH_SWC_WIDTH                   1
#define ATC260X_DAC_DITH_FLR                    0x0200  /* DAC_DEDF (DACFL&FR EN_DITH) */
#define ATC260X_DAC_DITH_FLR_MASK               0x0200
#define ATC260X_DAC_DITH_FLR_SHIFT                   9
#define ATC260X_DAC_DITH_FLR_WIDTH                   1
#define ATC260X_DAC_INSRS                       0x0100  /* DAC_DAC_INSRS (Input Sample Rate Select) */
#define ATC260X_DAC_INSRS_MASK                  0x0100
#define ATC260X_DAC_INSRS_SHIFT                      8
#define ATC260X_DAC_INSRS_WIDTH                      1
#define ATC260X_DAC_OUTSRS_SBLR_MASK            0x00C0  /* DAC_DAC_OUTSRS_SBLR (SBL/SBR Output Sample Rate Select) - 7:6 */
#define ATC260X_DAC_OUTSRS_SBLR_SHIFT                6
#define ATC260X_DAC_OUTSRS_SBLR_WIDTH                2
#define ATC260X_DAC_OUTSRS_SLR_MASK             0x0030  /* DAC_DAC_OUTSRS_SLR (SL/SR Output Sample Rate Select) - 5:4 */
#define ATC260X_DAC_OUTSRS_SLR_SHIFT                 4
#define ATC260X_DAC_OUTSRS_SLR_WIDTH                 2
#define ATC260X_DAC_OUTSRS_SWC_MASK             0x000C  /* DAC_DAC_OUTSRS_SWC (SW&C Output Sample Rate Select) - 3:2 */
#define ATC260X_DAC_OUTSRS_SWC_SHIFT                 2
#define ATC260X_DAC_OUTSRS_SWC_WIDTH                 2
#define ATC260X_DAC_OUTSRS_FLR_MASK             0x0003  /* DAC_DAC_OUTSRS_FLR (FLR Output Sample Rate Select) - 1:0 */
#define ATC260X_DAC_OUTSRS_FLR_SHIFT                 0
#define ATC260X_DAC_OUTSRS_FLR_WIDTH                 2

/**
 * 0x0403 (0x03) DAC Digital Bandwidth Filter Control
 */
#define ATC260X_DAC_RES3_MASK                   0xFF00  /* DAC_RES3 - 15:8 */
#define ATC260X_DAC_RES3_SHIFT                       8
#define ATC260X_DAC_RES3_WIDTH                       8
#define ATC260X_DAC_BW_SBLR_MASK                0x00C0  /* DAC_DAC_BW_SBLR - 7:6 */
#define ATC260X_DAC_BW_SBLR_SHIFT                    6
#define ATC260X_DAC_BW_SBLR_WIDTH                    2
#define ATC260X_DAC_BW_SLR_MASK                 0x0030  /* DAC_DAC_BW_SLR - 5:4 */
#define ATC260X_DAC_BW_SLR_SHIFT                     4
#define ATC260X_DAC_BW_SLR_WIDTH                     2
#define ATC260X_DAC_BW_SWC_MASK                 0x0400  /* DAC_DAC_BW_SWC - 3:2 */
#define ATC260X_DAC_BW_SWC_SHIFT                     2
#define ATC260X_DAC_BW_SWC_WIDTH                     2
#define ATC260X_DAC_BW_FLR_MASK                 0x0003  /* DAC_DAC_BW_FLR - 1:0 */
#define ATC260X_DAC_BW_FLR_SHIFT                     0
#define ATC260X_DAC_BW_FLR_WIDTH                     2

/**
 * 0x0404 (0x04) DAC Mute Control
 */
#define ATC260X_DAC_MUTE_SBR                    0x8000  /* DAC_MUTE_SBR */
#define ATC260X_DAC_MUTE_SBR_MASK               0x8000
#define ATC260X_DAC_MUTE_SBR_SHIFT                  15
#define ATC260X_DAC_MUTE_SBR_WIDTH                   1
#define ATC260X_DAC_MUTE_SBL                    0x4000  /* DAC_MUTE_SBL */
#define ATC260X_DAC_MUTE_SBL_MASK               0x4000
#define ATC260X_DAC_MUTE_SBL_SHIFT                  14
#define ATC260X_DAC_MUTE_SBL_WIDTH                   1
#define ATC260X_DAC_MUTE_SR                     0x2000  /* DAC_MUTE_SR */
#define ATC260X_DAC_MUTE_SR_MASK                0x2000
#define ATC260X_DAC_MUTE_SR_SHIFT                   13
#define ATC260X_DAC_MUTE_SR_WIDTH                    1
#define ATC260X_DAC_MUTE_SL                     0x1000  /* DAC_MUTE_SL */
#define ATC260X_DAC_MUTE_SL_MASK                0x1000
#define ATC260X_DAC_MUTE_SL_SHIFT                   12
#define ATC260X_DAC_MUTE_SL_WIDTH                    1
#define ATC260X_DAC_MUTE_C                      0x0800  /* DAC_MUTE_C */
#define ATC260X_DAC_MUTE_C_MASK                 0x0800
#define ATC260X_DAC_MUTE_C_SHIFT                    11
#define ATC260X_DAC_MUTE_C_WIDTH                     1
#define ATC260X_DAC_MUTE_SW                     0x0400  /* DAC_MUTE_SW */
#define ATC260X_DAC_MUTE_SW_MASK                0x0400
#define ATC260X_DAC_MUTE_SW_SHIFT                   10
#define ATC260X_DAC_MUTE_SW_WIDTH                    1
#define ATC260X_DAC_MUTE_FR                     0x0200  /* DAC_MUTE_FR */
#define ATC260X_DAC_MUTE_FR_MASK                0x0200
#define ATC260X_DAC_MUTE_FR_SHIFT                    9
#define ATC260X_DAC_MUTE_FR_WIDTH                    1
#define ATC260X_DAC_MUTE_FL                     0x0100  /* DAC_MUTE_FL */
#define ATC260X_DAC_MUTE_FL_MASK                0x0100
#define ATC260X_DAC_MUTE_FL_SHIFT                    8
#define ATC260X_DAC_MUTE_FL_WIDTH                    1
#define ATC260X_DAC_ENSBR                       0x0080  /* DAC_ENSBR */
#define ATC260X_DAC_ENSBR_MASK                  0x0080
#define ATC260X_DAC_ENSBR_SHIFT                      7
#define ATC260X_DAC_ENSBR_WIDTH                      1
#define ATC260X_DAC_ENSBL                       0x0040  /* DAC_ENSBL */
#define ATC260X_DAC_ENSBL_MASK                  0x0040
#define ATC260X_DAC_ENSBL_SHIFT                      6
#define ATC260X_DAC_ENSBL_WIDTH                      1
#define ATC260X_DAC_ENSR                        0x0020  /* DAC_ENSR */
#define ATC260X_DAC_ENSR_MASK                   0x0020
#define ATC260X_DAC_ENSR_SHIFT                       5
#define ATC260X_DAC_ENSR_WIDTH                       1
#define ATC260X_DAC_ENSL                        0x0010  /* DAC_ENSL */
#define ATC260X_DAC_ENSL_MASK                   0x0010
#define ATC260X_DAC_ENSL_SHIFT                       4
#define ATC260X_DAC_ENSL_WIDTH                       1
#define ATC260X_DAC_ENC                         0x0008  /* DAC_ENC */
#define ATC260X_DAC_ENC_MASK                    0x0008
#define ATC260X_DAC_ENC_SHIFT                        3
#define ATC260X_DAC_ENC_WIDTH                        1
#define ATC260X_DAC_ENSW                        0x0004  /* DAC_ENSW */
#define ATC260X_DAC_ENSW_MASK                   0x0004
#define ATC260X_DAC_ENSW_SHIFT                       2
#define ATC260X_DAC_ENSW_WIDTH                       1
#define ATC260X_DAC_ENFR                        0x0002  /* DAC_ENFR */
#define ATC260X_DAC_ENFR_MASK                   0x0002
#define ATC260X_DAC_ENFR_SHIFT                       1
#define ATC260X_DAC_ENFR_WIDTH                       1
#define ATC260X_DAC_ENFL                        0x0001  /* DAC_ENFL */
#define ATC260X_DAC_ENFL_MASK                   0x0001
#define ATC260X_DAC_ENFL_SHIFT                       0
#define ATC260X_DAC_ENFL_WIDTH                       1

/**
 * 0x0405 (0x05) DAC FL/FR Volume Control
 */
#define ATC260X_DAC_VOL_FR_MASK                 0xFF00  /* DAC_VOL_FR - 15:8 */
#define ATC260X_DAC_VOL_FR_SHIFT                     8
#define ATC260X_DAC_VOL_FR_WIDTH                     8
#define ATC260X_DAC_VOL_FL_MASK                 0x00FF  /* DAC_VOL_FL - 7:0 */
#define ATC260X_DAC_VOL_FL_SHIFT                     0
#define ATC260X_DAC_VOL_FL_WIDTH                     8

/**
 * 0x0406 (0x06) DAC SW/C Volume Control
 */
#define ATC260X_DAC_VOL_C_MASK                  0xFF00  /* DAC_VOL_C - 15:8 */
#define ATC260X_DAC_VOL_C_SHIFT                      8
#define ATC260X_DAC_VOL_C_WIDTH                      8
#define ATC260X_DAC_VOL_SW_MASK                 0x00FF  /* DAC_VOL_SW - 7:0 */
#define ATC260X_DAC_VOL_SW_SHIFT                     0
#define ATC260X_DAC_VOL_SW_WIDTH                     8

/**
 * 0x0407 (0x07) DAC SL/SR Volume Control
 */
#define ATC260X_DAC_VOL_SR_MASK                 0xFF00  /* DAC_VOL_SR - 15:8 */
#define ATC260X_DAC_VOL_SR_SHIFT                     8
#define ATC260X_DAC_VOL_SR_WIDTH                     8
#define ATC260X_DAC_VOL_SL_MASK                 0x00FF  /* DAC_VOL_SL - 7:0 */
#define ATC260X_DAC_VOL_SL_SHIFT                     0
#define ATC260X_DAC_VOL_SL_WIDTH                     8

/**
 * 0x0408 (0x8) DAC SBL/SBR Volume Control
 */
#define ATC260X_DAC_VOL_SBR_MASK                0xFF00  /* DAC_VOL_SBR - 15:8 */
#define ATC260X_DAC_VOL_SBR_SHIFT                    8
#define ATC260X_DAC_VOL_SBR_WIDTH                    8
#define ATC260X_DAC_VOL_SBL_MASK                0x00FF  /* DAC_VOL_SBL - 7:0 */
#define ATC260X_DAC_VOL_SBL_SHIFT                    0
#define ATC260X_DAC_VOL_SBL_WIDTH                    8

/**
 * 0x0409 (0x9) DAC Analog Control 0
 */
#define ATC260X_DAC_RES3_MASK                   0x1FF0  /* DAC_RES3 - 15:4 */
#define ATC260X_DAC_RES3_SHIFT                       4
#define ATC260X_DAC_RES3_WIDTH                      12
#define ATC260X_DAC_KFEN                        0x0008  /* DAC_KFEN */
#define ATC260X_DAC_KFEN_MASK                   0x0008
#define ATC260X_DAC_KFEN_SHIFT                       3
#define ATC260X_DAC_KFEN_WIDTH                       1
#define ATC260X_DAC_RES4_MASK                   0x0007  /* DAC_RES4 - 2:0 */
#define ATC260X_DAC_RES4_SHIFT                       0
#define ATC260X_DAC_RES4_WIDTH                       3

/**
 * 0x040A (0xA) Analog Control 1
 */
#define ATC260X_DAC_MICMUTE                     0x8000  /* DAC_MICMUTE */
#define ATC260X_DAC_MICMUTE_MASK                0x8000
#define ATC260X_DAC_MICMUTE_SHIFT                   15
#define ATC260X_DAC_MICMUTE_WIDTH                    1
#define ATC260X_DAC_FMMUTE                      0x4000  /* DAC_FMMUTE */
#define ATC260X_DAC_FMMUTE_MASK                 0x4000
#define ATC260X_DAC_FMMUTE_SHIFT                    14
#define ATC260X_DAC_FMMUTE_WIDTH                     1
#define ATC260X_DAC_MUTE_SBLR                   0x2000  /* DAC_MUTE_SBLR */
#define ATC260X_DAC_MUTE_SBLR_MASK              0x2000
#define ATC260X_DAC_MUTE_SBLR_SHIFT                 13
#define ATC260X_DAC_MUTE_SBLR_WIDTH                  1
#define ATC260X_DAC_MUTE_SLR                    0x1000  /* DAC_MUTE_SLR */
#define ATC260X_DAC_MUTE_SLR_MASK               0x1000
#define ATC260X_DAC_MUTE_SLR_SHIFT                  12
#define ATC260X_DAC_MUTE_SLR_WIDTH                   1
#define ATC260X_DAC_MUTE_SWC                    0x0800  /* DAC_MUTE_SWC */
#define ATC260X_DAC_MUTE_SWC_MASK               0x0800
#define ATC260X_DAC_MUTE_SWC_SHIFT                  11
#define ATC260X_DAC_MUTE_SWC_WIDTH                   1
#define ATC260X_DAC_MUTE_FLR                    0x0400  /* DAC_MUTE_FLR */
#define ATC260X_DAC_MUTE_FLR_MASK               0x0400
#define ATC260X_DAC_MUTE_FLR_SHIFT                  10
#define ATC260X_DAC_MUTE_FLR_WIDTH                   1
#define ATC260X_DAC_RES5_MASK                   0x03C0  /* DAC_RES4 - 9:6 */
#define ATC260X_DAC_RES5_SHIFT                       6
#define ATC260X_DAC_RES5_WIDTH                       4
#define ATC260X_DAC_VOLUME_MASK                 0x003F  /* DAC_VOLUME - 5:0 */
#define ATC260X_DAC_VOLUME_SHIFT                     0
#define ATC260X_DAC_VOLUME_WIDTH                     6

/**
 * 0x040E (0xE) Class D Control 0
 */
#define ATC260X_OTPR_MASK                       0xE000  /* OTPR (OVER TEMP CNTRL) - 15:13 */
#define ATC260X_OTPR_SHIFT                          13
#define ATC260X_OTPR_WIDTH                           3
#define ATC260X_GAIN_MASK                       0x1800  /* GAIN - 12:11 */
#define ATC260X_GAIN_SHIFT                          11
#define ATC260X_GAIN_WIDTH                           2
#define ATC260X_DBGIN                           0x0400  /* DBGIN (Debug Mode H or L) */
#define ATC260X_DBGIN_MASK                      0x0400
#define ATC260X_DBGIN_SHIFT                         10
#define ATC260X_DBGIN_WIDTH                          1
#define ATC260X_NCLPEN_OCIEN                    0x0200  /* NCLPEN (Non-Clip Enable) */
#define ATC260X_NCLPEN_MASK                     0x0200
#define ATC260X_NCLPEN_SHIFT                         9
#define ATC260X_NCLPEN_WIDTH                         1
#define ATC260X_SABD                            0x0100  /* SABD (Internal Mode Select) */
#define ATC260X_SABD_MASK                       0x0100
#define ATC260X_SABD_SHIFT                           8
#define ATC260X_SABD_WIDTH                           1
#define ATC260X_SSEN                            0x0080  /* SSEN (Spread-Spectrum) */
#define ATC260X_SSEN_MASK                       0x0080
#define ATC260X_SSEN_SHIFT                           7
#define ATC260X_SSEN_WIDTH                           1
#define ATC260X_SCEN                            0x0040  /* SCEN (Short Circuit Protection) - 6:5 */
#define ATC260X_SCEN_MASK                       0x0040
#define ATC260X_SCEN_SHIFT                           6
#define ATC260X_SCEN_WIDTH                           1
#define ATC260X_OTPEN                           0x0020  /* OTEN (Over Temp Protection) */
#define ATC260X_OTPEN_MASK                      0x0020
#define ATC260X_OTPEN_SHIFT                          5
#define ATC260X_OTPEN_WIDTH                          1
#define ATC260X_FBEN                            0x0010  /* FBEN (Feedback) */
#define ATC260X_FBEN_MASK                       0x0010
#define ATC260X_FBEN_SHIFT                           4
#define ATC260X_FBEN_WIDTH                           1
#define ATC260X_PEN                             0x0008  /* PEN (Power Stage) */
#define ATC260X_PEN_MASK                        0x0008
#define ATC260X_PEN_SHIFT                            3
#define ATC260X_PEN_WIDTH                            1
#define ATC260X_MUTE                            0x0004  /* MUTE (Mute Class D) */
#define ATC260X_MUTE_MASK                       0x0004
#define ATC260X_MUTE_SHIFT                           2
#define ATC260X_MUTE_WIDTH                           1
#define ATC260X_CLD2EN                          0x0002  /* CLD2EN (Class D2) */
#define ATC260X_CLD2EN_MASK                     0x0002
#define ATC260X_CLD2EN_SHIFT                         1
#define ATC260X_CLD2EN_WIDTH                         1
#define ATC260X_CLD1EN                          0x0001  /* CLD1EN (Class D1) */
#define ATC260X_CLD1EN_MASK                     0x0001
#define ATC260X_CLD1EN_SHIFT                         0
#define ATC260X_CLD1EN_WIDTH                         1

/**
 * 0x040F (0xF) Class D Control 1
 */
#define ATC260X_ANALOGFU_MASK                   0xF000  /* ANALOGFU (For Future Analog Use) - 15:12 */
#define ATC260X_ANALOGFU_SHIFT                      12
#define ATC260X_ANALOGFU_WIDTH                       4
#define ATC260X_SSR_MASK                        0x0C00  /* SSR (Spread-Spectrum Range) - 11:10 */
#define ATC260X_SSR_SHIFT                           10
#define ATC260X_SSR_WIDTH                            2
#define ATC260X_FSEN_MASK                       0x0300  /* FSEN (Frequency Select) - 9:8 */
#define ATC260X_FSEN_SHIFT                           8
#define ATC260X_FSEN_WIDTH                           2
#define ATC260X_NCLPR_MASK                      0x00C0  /* NCLPR (Frequency Select) - 7:6 */
#define ATC260X_NCLPR_SHIFT                          6
#define ATC260X_NCLPR_WIDTH                          2
#define ATC260X_EDG_MASK                        0x0030  /* EDG (Frequency Select) - 5:4 */
#define ATC260X_EDG_SHIFT                            4
#define ATC260X_EDG_WIDTH                            2
#define ATC260X_IBREG_MASK                      0x000F  /* IBREG (Frequency Select) - 3:0 */
#define ATC260X_IBREG_SHIFT                          0
#define ATC260X_IBREG_WIDTH                          4

/**
 * 0x0410 (0x10) Class D Control 2
 */
#define ATC260X_OHWN2                           0x8000  /* OHWN2 (Class D 2 Overheat Warning State) */
#define ATC260X_OHWN2_MASK                      0x8000
#define ATC260X_OHWN2_SHIFT                         15
#define ATC260X_OHWN2_WIDTH                          1
#define ATC260X_SCWN2                           0x4000  /* SCWN2 (Class D 2 Short Circuit Warning State) */
#define ATC260X_SCWN2_MASK                      0x4000
#define ATC260X_SCWN2_SHIFT                         14
#define ATC260X_SCWN2_WIDTH                          1
#define ATC260X_OHWN1                           0x2000  /* OHWN1 (Class D 1 Overheat Warning State) */
#define ATC260X_OHWN1_MASK                      0x2000
#define ATC260X_OHWN1_SHIFT                         13
#define ATC260X_OHWN1_WIDTH                          1
#define ATC260X_SCWN1                           0x1000  /* SCWN1 (Class D 1 Short Circuit Warning State) */
#define ATC260X_SCWN1_MASK                      0x1000
#define ATC260X_SCWN1_SHIFT                         12
#define ATC260X_SCWN1_WIDTH                          1
#define ATC260X_ATIME_MASK                      0x0E00  /* ATIME (Non-Clip Attack Time Select) - 11:9 */
#define ATC260X_ATIME_SHIFT                          9
#define ATC260X_ATIME_WIDTH                          3
#define ATC260X_RTIME_MASK                      0x01C0  /* RTIME (Non-Clip Release Time Select) - 8:6 */
#define ATC260X_RTIME_SHIFT                          6
#define ATC260X_RTIME_WIDTH                          3
#define ATC260X_VREC1_MASK                      0x0020  /* VREC1 */
#define ATC260X_VREC1_SHIFT                          5
#define ATC260X_VREC1_WIDTH                          1
#define ATC260X_VREC0_MASK                      0x0010  /* VREC0 */
#define ATC260X_VREC0_SHIFT                          4
#define ATC260X_VREC0_WIDTH                          1
#define ATC260X_RES5_MASK                       0x000F   /* RES5 - 3:0 */
#define ATC260X_RES5_SHIFT                           0
#define ATC260X_RES5_WIDTH                           4


struct atc260x_audio_platform_data {
        int vmid_discharge_msecs;
        int drain_msecs;
        int cap_discharge_msecs;
        int vmid_charge_msecs;
        u32 vmid_s_curve:2;
        u32 dis_out4:2;
        u32 dis_out3:2;
        u32 dis_out2:2;
        u32 dis_out1:2;
        u32 vroi_out4:1;
        u32 vroi_out3:1;
        u32 vroi_out2:1;
        u32 vroi_out1:1;
        u32 vroi_enable:1;
        u32 codec_current_on:2;
        u32 codec_current_standby:2;
        u32 codec_current_charge:2;
};

struct snd_soc_codec;

struct atc260x_codec {
        struct platform_device *pdev;
        struct snd_soc_codec *codec;
        struct atc260x_audio_platform_data *platform_data;
};

#endif
