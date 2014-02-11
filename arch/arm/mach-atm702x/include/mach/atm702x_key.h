/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2009 Actions Semi Inc.
*/
/******************************************************************************/

#ifndef __ATM702X_KEY_H__
#define __ATM702X_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

/*KEY_CTL*/
/*bit 24-31 Reserved*/
#define KEY_CTL_WTS_MASK                (0x7 << 21)
#define KEY_CTL_WTS(x)			(((x) & 0x7) << 21)

#define KEY_CTL_PRS_MASK                (0x3 << 19)
#define KEY_CTL_PRS(x)			(((x) & 0x3) << 19)

#define KEY_CTL_DTS_MASK                (0x3 << 17)
#define KEY_CTL_DTS(x)			(((x) & 0x3) << 17)

#define KEY_CTL_IRCL                    (0x1 << 16)

#define KEY_CTL_IRP                     (0x1 << 15)
#define KEY_CTL_IREN                    (0x1 << 14)

#define KEY_CTL_IOMS_MASK               (0x1 << 13)
#define KEY_CTL_IOMS_NORM               (0x0 << 13)
#define KEY_CTL_IOMS_DIODE              (0x1 << 13)

#define KEY_CTL_OTYP_MASK               (0x1 << 12)
#define KEY_CTL_OTYP_OPENDRAIN          (0x0 << 12)
#define KEY_CTL_OTYP_PUSHPULL           (0x1 << 12)

#define KEY_CTL_PENM_MASK               (0x7F << 5)
#define KEY_CTL_PENM_A_EN               (((0x1) << 0) << 5)
#define KEY_CTL_PENM_B_EN               (((0x1) << 1) << 5)
#define KEY_CTL_PENM_C_EN               (((0x1) << 2) << 5)
#define KEY_CTL_PENM_D_EN               (((0x1) << 3) << 5)
#define KEY_CTL_PENM_E_EN               (((0x1) << 4) << 5)
#define KEY_CTL_PENM_F_EN               (((0x1) << 5) << 5)
#define KEY_CTL_PENM_G_EN               (((0x1) << 6) << 5)

#define KEY_CTL_MXS_MASK	          (0x3 << 3)
#define KEY_CTL_MXS_8_8_SOSI	          (0x0 << 3)
#define KEY_CTL_MXS_8_4_SOPI	          (0x0 << 3)
#define KEY_CTL_MXS_3_4_POPI	          (0x0 << 3)
#define KEY_CTL_MXS_8_16_SOSI	          (0x1 << 3)
#define KEY_CTL_MXS_16_4_SOPI	          (0x1 << 3)
#define KEY_CTL_MXS_16_16_SOSI	          (0x2 << 3)

#define KEY_CTL_KMS_MASK			  (0x3 << 1)
#define KEY_CTL_KMS_POPI			  (0x0 << 1)
#define KEY_CTL_KMS_SOPI			  (0x1 << 1)
#define KEY_CTL_KMS_SOSI			  (0x2 << 1)
#define KEY_CTL_KMS_IOSCAN		          (0x3 << 1)

#define KEY_CTL_KEN                     (0x1 << 0)

/*=================================================================*/
/* key*/
#define KEY_BASE 0xB01A0000
#define KEY_IOMEM_LEN 8
#define KEY_CTL_OFFSET 0
#define KEY_DAT_OFFSET 4 

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
