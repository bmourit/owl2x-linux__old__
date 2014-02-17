/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2009 Actions Semi Inc.
*/
/******************************************************************************/
#ifndef __ATV5201_HDMI_H__
#define __ATV5201_HDMI_H__

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/
/*
#define     HDMI_BASE                                                         0xB02C0000
#define     HDMI_VICTL                                                        (HDMI_BASE+0x0000)
#define     HDMI_VIVSYNC                                                      (HDMI_BASE+0x0004)
#define     HDMI_VIVHSYNC                                                     (HDMI_BASE+0x0008)
#define     HDMI_VIALSEOF                                                     (HDMI_BASE+0x000C)
#define     HDMI_VIALSEEF                                                     (HDMI_BASE+0x0010)
#define     HDMI_VIADLSE                                                      (HDMI_BASE+0x0014)
#define     HDMI_AIFRAMEC                                                     (HDMI_BASE+0x0020)
#define     HDMI_AICHSTAByte0to3                                              (HDMI_BASE+0x0024)
#define     HDMI_AICHSTAByte4to7                                              (HDMI_BASE+0x0028)
#define     HDMI_AICHSTAByte8to11                                             (HDMI_BASE+0x002C)
#define     HDMI_AICHSTAByte12to15                                            (HDMI_BASE+0x0030)
#define     HDMI_AICHSTAByte16to19                                            (HDMI_BASE+0x0034)
#define     HDMI_AICHSTAByte20to23                                            (HDMI_BASE+0x0038)
#define     HDMI_AICHSTASCN                                                   (HDMI_BASE+0x003C)
#define     HDMI_VR                                                           (HDMI_BASE+0x0050)
#define     HDMI_CR                                                           (HDMI_BASE+0x0054)
#define     HDMI_SCHCR                                                        (HDMI_BASE+0x0058)
#define     HDMI_ICR                                                          (HDMI_BASE+0x005C)
#define     HDMI_SCR                                                          (HDMI_BASE+0x0060)
#define     HDMI_LPCR                                                         (HDMI_BASE+0x0064)
#define     HDCP_CR                                                           (HDMI_BASE+0x0068)
#define     HDCP_SR                                                           (HDMI_BASE+0x006C)
#define     HDCP_ANLR                                                         (HDMI_BASE+0x0070)
#define     HDCP_ANMR                                                         (HDMI_BASE+0x0074)
#define     HDCP_ANILR                                                        (HDMI_BASE+0x0078)
#define     HDCP_ANIMR                                                        (HDMI_BASE+0x007C)
#define     HDCP_DPKLR                                                        (HDMI_BASE+0x0080)
#define     HDCP_DPKMR                                                        (HDMI_BASE+0x0084)
#define     HDCP_LIR                                                          (HDMI_BASE+0x0088)
#define     HDCP_SHACR                                                        (HDMI_BASE+0x008C)
#define     HDCP_SHADR                                                        (HDMI_BASE+0x0090)
#define     HDCP_ICR                                                          (HDMI_BASE+0x0094)
#define     HDCP_KMMR                                                         (HDMI_BASE+0x0098)
#define     HDCP_KMLR                                                         (HDMI_BASE+0x009C)
#define     HDCP_MILR                                                         (HDMI_BASE+0x00A0)
#define     HDCP_MIMR                                                         (HDMI_BASE+0x00A4)
#define     HDCP_KOWR                                                         (HDMI_BASE+0x00A8)
#define     HDCP_OWR                                                          (HDMI_BASE+0x00AC)
#define     TMDS_STR0                                                         (HDMI_BASE+0x00B8)
#define     TMDS_STR1                                                         (HDMI_BASE+0x00BC)
#define     TMDS_EODR0                                                        (HDMI_BASE+0x00C0)
#define     TMDS_EODR1                                                        (HDMI_BASE+0x00C4)
#define     HDMI_ASPCR                                                        (HDMI_BASE+0x00D0)
#define     HDMI_ACACR                                                        (HDMI_BASE+0x00D4)
#define     HDMI_ACRPCR                                                       (HDMI_BASE+0x00D8)
#define     HDMI_ACRPCTSR                                                     (HDMI_BASE+0x00DC)
#define     HDMI_ACRPPR                                                       (HDMI_BASE+0x00E0)
#define     HDMI_GCPCR                                                        (HDMI_BASE+0x00E4)
#define     HDMI_RPCR                                                         (HDMI_BASE+0x00E8)
#define     HDMI_RPRBDR                                                       (HDMI_BASE+0x00EC)
#define     HDMI_OPCR                                                         (HDMI_BASE+0x00F0)
#define     HDMI_DIPCCR                                                       (HDMI_BASE+0x00F4)
#define     HDMI_ORP6PH                                                       (HDMI_BASE+0x00F8)
#define     HDMI_ORSP6W0                                                      (HDMI_BASE+0x00FC)
#define     HDMI_ORSP6W1                                                      (HDMI_BASE+0x0100)
#define     HDMI_ORSP6W2                                                      (HDMI_BASE+0x0104)
#define     HDMI_ORSP6W3                                                      (HDMI_BASE+0x0108)
#define     HDMI_ORSP6W4                                                      (HDMI_BASE+0x010C)
#define     HDMI_ORSP6W5                                                      (HDMI_BASE+0x0110)
#define     HDMI_ORSP6W6                                                      (HDMI_BASE+0x0114)
#define     HDMI_ORSP6W7                                                      (HDMI_BASE+0x0118)
#define     HDMI_CECCR                                                        (HDMI_BASE+0x011C)
#define     HDMI_CECRTCR                                                      (HDMI_BASE+0x0120)
#define     HDMI_CECRxCR                                                      (HDMI_BASE+0x0124)
#define     HDMI_CECTxCR                                                      (HDMI_BASE+0x0128)
#define     HDMI_CECTxDR                                                      (HDMI_BASE+0x012C)
#define     HDMI_CECRxDR                                                      (HDMI_BASE+0x0130)
#define     HDMI_CECRxTCR                                                     (HDMI_BASE+0x0134)
#define     HDMI_CECTxTCR0                                                    (HDMI_BASE+0x0138)
#define     HDMI_CECTxTCR1                                                    (HDMI_BASE+0x013C)
#define     HDMI_CRCCR                                                        (HDMI_BASE+0x0140)
#define     HDMI_CRCDOR                                                       (HDMI_BASE+0x0144)
#define     HDMI_TX_1                                                         (HDMI_BASE+0x0154)
#define     HDMI_TX_2                                                         (HDMI_BASE+0x0158)
#define     CEC_DDC_HPD                                                       (HDMI_BASE+0x015C)
*/

/********************************************************************************/
/* HDMI_VICTL */
/* each line pixel data scaler 2:1 in progressive format */
#define HDMI_VICTL_PDIV2_DISABLE_SCALER     (0 << 29)               /* disable scaler 2:1 */
#define HDMI_VICTL_PDIV2_ENABLE_SCALER      (1 << 29)               /* enable scaler 2:1 */
/* video scan format select */
#define HDMI_VICTL_PORI_PROGRESSIVE         (0 << 28)               /* progessive */
#define HDMI_VICTL_PORI_INTERLACED          (1 << 28)               /* interlaced */
#define HDMI_VICTL_DTOTAL(x)                (((x) & 0xFFF) << 16)   /* total pixel data in every line */
/* total line in one frame.
 * 1 field/1 frame of progressive; 2 field/1 frame of interlaced.
 */
#define HDMI_VICTL_LTOTAL(x)                (((x) & 0x7FF) << 4)

/********************************************************************************/
/* HDMI_VIVSYNC */
#define HDMI_VIVSYNC_VEEI(x)                (((x) & 0x3FF) << 12)
#define HDMI_VIVSYNC_VSEI(x)                (((x) & 0x3FF) << 0)

/********************************************************************************/
/* HDMI_VIVHSYNC */
#define HDMI_VIVHSYNC_VEPOI(x)              (((x) & 0xF) << 24)
#define HDMI_VIVHSYNC_VSPOI(x)              (((x) & 0x7FF) << 12)
#define HDMI_VIVHSYNC_HDPN(x)               (((x) & 0x1FF) << 0)

/********************************************************************************/
/* HDMI_VIALSEOF */
#define HDMI_VIALSEOF_LEPOI(x)              (((x) & 0x7FF) << 12)
#define HDMI_VIALSEOF_LSPOI(x)              (((x) & 0x7FF) << 0)

/********************************************************************************/
/* HDMI_VIALSEEF */
#define HDMI_VIALSEEF_LEEI(x)               (((x) & 0x7FF) << 12)
#define HDMI_VIALSEEF_LSEI(x)               (((x) & 0x7FF) << 0)

/********************************************************************************/
/* HDMI_VIADLSE */
#define HDMI_VIADLSE_AVDEP(x)               (((x) & 0xFFF) << 16)
#define HDMI_VIADLSE_AVDSP(x)               (((x) & 0xFFF) << 0)

/********************************************************************************/
/* HDMI_AIFRAMEC */
#define HDMI_AIFRAMEC_FRAMEC_MASK(x)        (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDMI_AICHSTAByte0to3 */
/* for 60958-3 consumer applications */
#define HDMI_AICHSTAByte0to3_60958_3_CLOCKACCU(x)   (((x) & 0x3) << 28)
#define HDMI_AICHSTAByte0to3_60958_3_SAMPFRE(x)     (((x) & 0xF) << 24)
#define HDMI_AICHSTAByte0to3_60958_3_CATEGCODE(x)   (((x) & 0xFF) << 8)
#define HDMI_AICHSTAByte0to3_60958_3_CHANSTAM(x)    (((x) & 0x3) << 6)
#define HDMI_AICHSTAByte0to3_60958_3_ADDFORMAT(x)   (((x) & 0x7) << 3)
#define HDMI_AICHSTAByte0to3_60958_3_CPBIT          (1 << 2)
#define HDMI_AICHSTAByte0to3_60958_3_USERTYPE(x)    (((x) & 0x3) << 0)
/* for 60958-4 professional applications */
#define HDMI_AICHSTAByte0to3_60958_4_MULTICHMC      (1 << 31)
#define HDMI_AICHSTAByte0to3_60958_4_MULTICHAM(x)   (((x) & 0x7) << 28)
#define HDMI_AICHSTAByte0to3_60958_4_MULTICHAN(x)   (((x) & 0xF) << 24)
#define HDMI_AICHSTAByte0to3_60958_4_ALIGNLEVEL(x)  (((x) & 0x3) << 22)
#define HDMI_AICHSTAByte0to3_60958_4_TRANSWL(x)     (((x) & 0x7) << 19)
#define HDMI_AICHSTAByte0to3_60958_4_AUXISAMB(x)    (((x) & 0x7) << 16)
#define HDMI_AICHSTAByte0to3_60958_4_ENUSERBIT(x)   (((x) & 0xF) << 12)
#define HDMI_AICHSTAByte0to3_60958_4_ENCHMODE(x)    (((x) & 0xF) << 8)
#define HDMI_AICHSTAByte0to3_60958_4_ENSAMFRE(x)    (((x) & 0x3) << 6)
#define HDMI_AICHSTAByte0to3_60958_4_LOCKINDI       (1 << 5)
#define HDMI_AICHSTAByte0to3_60958_4_EASPE(x)       (((x) & 0x7) << 2)
#define HDMI_AICHSTAByte0to3_60958_4_USERTYPE(x)    (((x) & 0x3) << 0)

/********************************************************************************/
/* HDMI_AICHSTAByte4to7 */
/* for 60958-3 consumer applications */
#define HDMI_AICHSTAByte4to7_60958_3_CGMS_A(x)      (((x) & 0x3) << 8)
#define HDMI_AICHSTAByte4to7_60958_3_OSAMFRE(x)     (((x) & 0xF) << 4)
#define HDMI_AICHSTAByte4to7_60958_3_SAMWL(x)       (((x) & 0x7) << 1)
#define HDMI_AICHSTAByte4to7_60958_3_MAXSAML        (1 << 0)
/* for 60958-4 professional applications */
#define HDMI_AICHSTAByte4to7_60958_4_ACODL(x)       (((x) & 0xFFFF) << 16)
#define HDMI_AICHSTAByte4to7_60958_4_SAMFRESF       (1 << 7)
#define HDMI_AICHSTAByte4to7_60958_4_SAMFRE(x)      (((x) & 0xF) << 3)
#define HDMI_AICHSTAByte4to7_60958_4_DAREFS(x)      (((x) & 0x3) << 0)

/********************************************************************************/
/* HDMI_AICHSTAByte8to11 */
/* for 60958-4 professional applications */
#define HDMI_AICHSTAByte8to11_60958_4_ACDESL(x)     (((x) & 0xFFFF) << 16)
#define HDMI_AICHSTAByte8to11_60958_4_ACODH(x)      (((x) & 0xFFFF) << 0)

/********************************************************************************/
/* HDMI_AICHSTAByte12to15 */
/* for 60958-4 professional applications */
#define HDMI_AICHSTAByte12to15_60958_4_LSACODEL(x)  (((x) & 0xFFFF) << 16)
#define HDMI_AICHSTAByte12to15_60958_4_ACDESH(x)    (((x) & 0xFFFF) << 0)

/********************************************************************************/
/* HDMI_AICHSTAByte16to19 */
/* for 60958-4 professional applications */
#define HDMI_AICHSTAByte16to19_60958_4_TDSACODEL(x) (((x) & 0xFFFF) << 16)
#define HDMI_AICHSTAByte16to19_60958_4_LSACODEH(x)  (((x) & 0xFFFF) << 0)

/********************************************************************************/
/* HDMI_AICHSTAByte20to23 */
/* for 60958-4 professional applications */
#define HDMI_AICHSTAByte20to23_60958_4_CSDCRCC(x)   (((x) & 0xFF) << 24)
#define HDMI_AICHSTAByte20to23_60958_4_CSDRF3       (1 << 23)
#define HDMI_AICHSTAByte20to23_60958_4_CSDRF2       (1 << 22)
#define HDMI_AICHSTAByte20to23_60958_4_CSDRF1       (1 << 21)
#define HDMI_AICHSTAByte20to23_60958_4_CSDRF0       (1 << 20)
#define HDMI_AICHSTAByte20to23_60958_4_TDSACODEH(x) (((x) & 0xFFFF) << 0)

/********************************************************************************/
/* HDMI_AICHSTASCN */
#define HDMI_AICHSTASCN_60958_3_ACH8NUM(x)          (((x) & 0xF) << 28)
#define HDMI_AICHSTASCN_60958_3_ACH7NUM(x)          (((x) & 0xF) << 24)
#define HDMI_AICHSTASCN_60958_3_ACH6NUM(x)          (((x) & 0xF) << 20)
#define HDMI_AICHSTASCN_60958_3_ACH5NUM(x)          (((x) & 0xF) << 16)
#define HDMI_AICHSTASCN_60958_3_ACH4NUM(x)          (((x) & 0xF) << 12)
#define HDMI_AICHSTASCN_60958_3_ACH3NUM(x)          (((x) & 0xF) << 8)
#define HDMI_AICHSTASCN_60958_3_ACH2NUM(x)          (((x) & 0xF) << 4)
#define HDMI_AICHSTASCN_60958_3_ACH1NUM(x)          (((x) & 0xF) << 0)

/********************************************************************************/
/* HDMI_VR */
#define HDMI_VR_YEAR(x)                             (((x) & 0xF) << 28)
#define HDMI_VR_MONTH(x)                            (((x) & 0xFF) << 20)
#define HDMI_VR_DAY(x)                              (((x) & 0xFF) << 12)
#define HDMI_VR_VER_I(x)                            (((x) & 0xF) << 8)
#define HDMI_VR_VER_F(x)                            (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDMI_CR */
#define HDMI_CR_ENHPINT                             (1 << 31)
#define HDMI_CR_PHPLG                               (1 << 30)
#define HDMI_CR_HPLGSTATUS                          (1 << 29)
#define HDMI_CR_HPDENABLE                           (1 << 28)
#define HDMI_CR_HPDEBOUNCE(x)                       (((x) & 0xF) << 24)
#define HDMI_CR_PKTGB2V_ENABLE                      (1 << 6)
#define HDMI_CR_BIT_REPEAT_EN                       (1 << 5)
#define HDMI_CR_FIFO_FILL                           (1 << 4)
#define HDMI_CR_ENABLEHDMI                          (1 << 0)

/********************************************************************************/
/* HDMI_SCHCR */
#define HDMI_SCHCR_DEEPCOLOR_MASK                   (0x3 << 16)
#define HDMI_SCHCR_DEEPCOLOR(x)                     (((x) & 0x3) << 16)
#define HDMI_SCHCR_HDMI_3D_FRAME_FLAG               (1 << 8)
#define HDMI_SCHCR_VSYNCPOLIN                       (1 << 7)
#define HDMI_SCHCR_HSYNCPOLIN                       (1 << 6)
#define HDMI_SCHCR_PIXELENCFMT(x)                   (((x) & 0x3) << 4)
#define HDMI_SCHCR_VSYNCPOLINV                      (1 << 2)
#define HDMI_SCHCR_HSYNCPOLINV                      (1 << 1)
#define HDMI_SCHCR_HDMI_MODESEL                     (1 << 0)

/********************************************************************************/
/* HDMI_ICR */
#define HDMI_ICR_ENAUDIO                            (1 << 25)
#define HDMI_ICR_ENVITD                             (1 << 24)
#define HDMI_ICR_VITD(x)                            (((x) & 0xFFFFFF) << 0)

/********************************************************************************/
/* HDMI_SCR */
#define HDMI_SCR_OWCTSHW                            (1 << 1)
#define HDMI_SCR_ASFIFOOW                           (1 << 0)

/********************************************************************************/
/* HDMI_LPCR */
#define HDMI_LPCR_CURLINECNTR                       (((x) & 0xFFF) << 16)
#define HDMI_LPCR_CURPIXELCNTR                      (((x) & 0xFFF) << 0)

/********************************************************************************/
/* HDCP_CR */
#define HDCP_CR_EN1DOT1_FEATURE                     (1 << 31)
#define HDCP_CR_DOWNSTRISREPEATER                   (1 << 30)
#define HDCP_CR_ANINFREQ                            (1 << 25)
#define HDCP_CR_SEEDLOAD                            (1 << 24)
#define HDCP_CR_ENRIUPDINT                          (1 << 9)
#define HDCP_CR_ENPJUPDINT                          (1 << 8)
#define HDCP_CR_ANINFLUENCEMODE                     (1 << 7)
#define HDCP_CR_HDCP_ENCRYPTENABLE                  (1 << 6)
#define HDCP_CR_RESETKMACC                          (1 << 4)
#define HDCP_CR_FORCETOUNAUTHENTICATED              (1 << 3)
#define HDCP_CR_DEVICEAUTHENTICATED                 (1 << 2)
#define HDCP_CR_AUTHCOMPUTE                         (1 << 1)
#define HDCP_CR_AUTHREQUEST                         (1 << 0)

/********************************************************************************/
/* HDCP_SR */
#define HDCP_SR_HDCPCIPHERSTATE(x)                  (((x) & 0xFF) << 24)
#define HDCP_SR_RIUPDATED                           (1 << 5)
#define HDCP_SR_PJUPDATED                           (1 << 4)
#define HDCP_SR_CURDPKACCDONE                       (1 << 3)
#define HDCP_SR_HDCP_ENCRYPT_STATUS                 (1 << 2)
#define HDCP_SR_ANTHENTICATEDOK                     (1 << 1)
#define HDCP_SR_ANREADY                             (1 << 0)

/********************************************************************************/
/* HDCP_ANLR */

/********************************************************************************/
/* HDCP_ANMR */

/********************************************************************************/
/* HDCP_ANILR */

/********************************************************************************/
/* HDCP_ANIMR */

/********************************************************************************/
/* HDCP_DPKLR */
#define HDCP_DPKLR_DDPKLSB24(x)                     (((x) & 0xFFFFFF) << 8)
#define HDCP_DPKLR_DPKENCPNT(x)                     (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDCP_DPKMR */

/********************************************************************************/
/* HDCP_LIR */
#define HDCP_LIR_RI(x)                              (((x) & 0xFFFF) << 16)
#define HDCP_LIR_PJ(x)                              (((x) & 0xFF) << 8)

/********************************************************************************/
/* HDCP_SHACR */
#define HDCP_SHACR_VMATCH                           (1 << 4)
#define HDCP_SHACR_SHAREADY                         (1 << 3)
#define HDCP_SHACR_SHASTART                         (1 << 2)
#define HDCP_SHACR_SHAFIRST                         (1 << 1)
#define HDCP_SHACR_RSTSHAPTR                        (1 << 0)

/********************************************************************************/
/* HDCP_SHADR */

/********************************************************************************/
/* HDCP_ICR */
#define HDCP_ICR_RIRATE(x)                          (((x) & 0xFF) << 8)
#define HDCP_ICR_PJRATE(x)                          (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDCP_KMMR */

/********************************************************************************/
/* HDCP_KMLR */

/********************************************************************************/
/* HDCP_MILR */

/********************************************************************************/
/* HDCP_MIMR */

/********************************************************************************/
/* HDCP_KOWR */
#define HDCP_KOWR_HDCPREKEYKEEPOUTWIN(x)            (((x) & 0xFF) << 24)
#define HDCP_KOWR_HDCPVERKEEPOUTWINEND(x)           (((x) & 0xFFF) << 12)
#define HDCP_KOWR_HDCPVERTKEEPOUTWINSTART(x)        (((x) & 0xFFF) << 0)

/********************************************************************************/
/* HDCP_OWR */
#define HDCP_OWR_HDCPOPPWINEND(x)                   (((x) & 0xFFF) << 12)
#define HDCP_OWR_HDCPOPPWINSTART(x)                 (((x) & 0xFFF) << 0)

/********************************************************************************/
/* TMDS_STR0 */
#define TMDS_STR0_TMDS1TEN                          (1 << 31)
#define TMDS_STR0_TD1SRC                            (1 << 30)
#define TMDS_STR0_TMDS1TD(x)                        (((x) & 0x3FF) << 16)
#define TMDS_STR0_TMDS0TEN                          (1 << 15)
#define TMDS_STR0_TD0SRC                            (1 << 14)
#define TMDS_STR0_TMDS0TD(x)                        (((x) & 0x3FF) << 0)

/********************************************************************************/
/* TMDS_STR1 */
#define TMDS_STR1_TMDSCTEN                          (1 << 31)
#define TMDS_STR1_TDCSRC                            (1 << 30)
#define TMDS_STR1_TMDSCTD(x)                        (((x) & 0x3FF) << 16)
#define TMDS_STR1_TMDS2TEN                          (1 << 15)
#define TMDS_STR1_TD2SRC                            (1 << 14)
#define TMDS_STR1_TMDS2TD(x)                        (((x) & 0x3FF) << 0)

/********************************************************************************/
/* TMDS_EODR0 */
#define TMDS_EODR0_TMDS_ENCEN                       (1 << 31)
#define TMDS_EODR0_TMDS_CH2_OUT(x)                  (((x) & 0x3FF) << 20)
#define TMDS_EODR0_TMDS_CH1_OUT(x)                  (((x) & 0x3FF) << 10)
#define TMDS_EODR0_TMDS_CH0_OUT(x)                  (((x) & 0x3FF) << 0)

/********************************************************************************/
/* TMDS_EODR1 */
#define TMDS_EODR1_TMDS_CLK_OUT(x)                  (((x) & 0x3FF) << 0)

/********************************************************************************/
/* HDMI_ASPCR */
#define HDMI_ASPCR_ASPTYPE                          (1 << 31)
#define HDMI_ASPCR_AUDIO_SAMPLEVR(x)                (((x) & 0xF) << 27)
#define HDMI_ASPCR_AUDIO_SAMPLEVL(x)                (((x) & 0xF) << 23)
#define HDMI_ASPCR_SAMPLEFLAT(x)                    (((x) & 0xF) << 19)
#define HDMI_ASPCR_SAMPLEPRESENT(x)                 (((x) & 0xF) << 15)
#define HDMI_ASPCR_FORCESAMPLEPRESENT               (1 << 14)
#define HDMI_ASPCR_B_FRAME(x)                       (((x) & 0xF) << 10)
#define HDMI_ASPCR_FORCEB_FRAME                     (1 << 9)
#define HDMI_ASPCR_FORCEB_AUDIO_LAYOUT              (1 << 8)
#define HDMI_ASPCR_AUDIOCHANNELMASK(x)              (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDMI_ACACR */
#define HDMI_ACACR_SPMODE(x)                        (((x) & 0x7) << 24)
#define HDMI_ACACR_CH7MAP(x)                        (((x) & 0x7) << 21)
#define HDMI_ACACR_CH6MAP(x)                        (((x) & 0x7) << 18)
#define HDMI_ACACR_CH5MAP(x)                        (((x) & 0x7) << 15)
#define HDMI_ACACR_CH4MAP(x)                        (((x) & 0x7) << 12)
#define HDMI_ACACR_CH3MAP(x)                        (((x) & 0x7) << 9)
#define HDMI_ACACR_CH2MAP(x)                        (((x) & 0x7) << 6)
#define HDMI_ACACR_CH1MAP(x)                        (((x) & 0x7) << 3)
#define HDMI_ACACR_CH0MAP(x)                        (((x) & 0x7) << 0)

/********************************************************************************/
/* HDMI_ACRPCR */
#define HDMI_ACRPCR_DISABLECRP                      (1 << 31)
#define HDMI_ACRPCR_CTS_SOURCE                      (1 << 30)
#define HDMI_ACRPCR_N_VALUE(x)                      (((x) & 0xFFFFF) << 0)

/********************************************************************************/
/* HDMI_ACRPCTSR */
#define HDMI_ACRPCTSR_CTSSW(x)                      (((x) & 0xFFFFF) << 0)

/********************************************************************************/
/* HDMI_ACRPPR */
#define HDMI_ACRPPR_CTSHW(x)                        (((x) & 0xFFFFF) << 0)

/********************************************************************************/
/* HDMI_GCPCR */
#define HDMI_GCPCR_ENABLEGCP                        (1 << 31)
#define HDMI_GCPCR_ENABLEDC                         (1 << 30)
#define HDMI_GCPCR_DEFAULT_PHASE                    (1 << 8)
#define HDMI_GCPCR_CDFIELD(x)                       (((x) & 0xF) << 4)
#define HDMI_GCPCR_CLEARAVMUTE                      (1 << 1)
#define HDMI_GCPCR_GCP_SETAVMUTE                    (1 << 0)

/********************************************************************************/
/* HDMI_RPCR */
#define HDMI_RPCR_PRP5PERIOD(x)                     (((x) & 0xF) << 28)
#define HDMI_RPCR_PRP4PERIOD(x)                     (((x) & 0xF) << 24)
#define HDMI_RPCR_PRP3PERIOD(x)                     (((x) & 0xF) << 20)
#define HDMI_RPCR_PRP2PERIOD(x)                     (((x) & 0xF) << 16)
#define HDMI_RPCR_PRP1PERIOD(x)                     (((x) & 0xF) << 12)
#define HDMI_RPCR_PRP0PERIOD(x)                     (((x) & 0xF) << 8)
#define HDMI_RPCR_ENPRPKT5                          (1 << 5)
#define HDMI_RPCR_ENPRPKT4                          (1 << 4)
#define HDMI_RPCR_ENPRPKT3                          (1 << 3)
#define HDMI_RPCR_ENPRPKT2                          (1 << 2)
#define HDMI_RPCR_ENPRPKT1                          (1 << 1)
#define HDMI_RPCR_ENPRPKT0                          (1 << 0)

/********************************************************************************/
/* HDMI_RPRBDR */
/* internal SRAM read back data */
#define HDMI_RPRBDR_ISRAMRBD_MASK(x)                (((x) & 0x3FFFF) << 0)

/********************************************************************************/
/* HDMI_OPCR */
#define HDMI_OPCR_ENRBPKTSRAM                       (1 << 31)
#define HDMI_OPCR_SRAMRDSTATUS                      (1 << 30)
#define HDMI_OPCR_WRDES                             (1 << 9)
#define HDMI_OPCR_RPRWCMD                           (1 << 8)
#define HDMI_OPCR_RPADD(x)                          (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDMI_DIPCCR */
#define HDMI_DIPCCR_VBIDIPCNT(x)                    (((x) & 0x1F) << 8)
#define HDMI_DIPCCR_HBIDIPCNT(x)                    (((x) & 0x1F) << 0)

/********************************************************************************/
/* HDMI_ORP6PH */
#define HDMI_ORP6PH_PHOP(x)                         (((x) & 0xFFFFFF) << 0)

/********************************************************************************/
/* HDMI_ORSP6W0 */

/********************************************************************************/
/* HDMI_ORSP6W1 */

/********************************************************************************/
/* HDMI_ORSP6W2 */

/********************************************************************************/
/* HDMI_ORSP6W3 */

/********************************************************************************/
/* HDMI_ORSP6W4 */

/********************************************************************************/
/* HDMI_ORSP6W5 */

/********************************************************************************/
/* HDMI_ORSP6W6 */

/********************************************************************************/
/* HDMI_ORSP6W7 */

/********************************************************************************/
/* HDMI_CECCR */
#define HDMI_CECCR_CEC_MODE(x)                      (((x) & 0x3) << 30)
#define HDMI_CECCR_ACKSTATUS_SHIFT                  (29)
#define HDMI_CECCR_ACKSTATUS_MASK                   (1 << HDMI_CECCR_ACKSTATUS_SHIFT)
#define HDMI_CECCR_TEST_MODE_DAC_ENB                (1 << 28)
#define HDMI_CECCR_LOGICAL_ADDR(x)                  (((x) & 0xF) << 24)
#define HDMI_CECCR_TIMER_DIV(x)                     (((x) & 0xFF) << 16)
#define HDMI_CECCR_PRE_DIV(x)                       (((x) & 0xFF) << 8)
#define HDMI_CECCR_UNREG_ACK_EN                     (1 << 7)
#define HDMI_CECCR_TEST_MODE_DAC_OUT(x)             (((x) & 0x1F) << 0)

/********************************************************************************/
/* HDMI_CECRTCR */
#define HDMI_CECRTCR_CEC_PAD_IN_SHIFT               (31)
#define HDMI_CECRTCR_CEC_PAD_IN_MASK                (1 << HDMI_CECRTCR_CEC_PAD_IN_SHIFT)
#define HDMI_CECRTCR_WT_CNT_SHIFT                   (5)
#define HDMI_CECRTCR_WT_CNT_MASK                    (0x3F << HDMI_CECRTCR_WT_CNT_SHIFT)
#define HDMI_CECRTCR_LATTEST_SHIFT                  (4)
#define HDMI_CECRTCR_LATTEST_MASK                   (1 << HDMI_CECRTCR_LATTEST_SHIFT)
#define HDMI_CECRTCR_RETRY_NO(x)                    (((x) & 0xF) << 0)

/********************************************************************************/
/* HDMI_CECRxCR */
#define HDMI_CECRxCR_RX_MTYPE                       (1 << 16)
#define HDMI_CECRxCR_RX_EN                          (1 << 15)
#define HDMI_CECRxCR_RX_RST                         (1 << 14)
#define HDMI_CECRxCR_RX_CONTINUOUS                  (1 << 13)
#define HDMI_CECRxCR_RX_INT_EN                      (1 << 12)
#define HDMI_CECRxCR_INIT_ADDR(x)                   (((x) & 0xF) << 8)
#define HDMI_CECRxCR_RX_EOM                         (1 << 7)
#define HDMI_CECRxCR_RX_INT                         (1 << 6)
#define HDMI_CECRxCR_RX_FIFO_OV                     (1 << 5)
#define HDMI_CECRxCR_RX_FIFO_CNT(x)                 (((x) & 0x1F) << 0)

/********************************************************************************/
/* HDMI_CECTxCR */
#define HDMI_CECTxCR_TX_ADDR_EN                     (1 << 20)
#define HDMI_CECTxCR_TX_ADDR(x)                     (((x) & 0xF) << 16)
#define HDMI_CECTxCR_TX_EN                          (1 << 15)
#define HDMI_CECTxCR_TX_RST                         (1 << 14)
#define HDMI_CECTxCR_TX_CONTINUOUS                  (1 << 13)
#define HDMI_CECTxCR_TX_INT_EN                      (1 << 12)
#define HDMI_CECTxCR_DEST_ADDR(x)                   (((x) & 0xF) << 8)
#define HDMI_CECTxCR_TX_EOM_SHIFT                   (7)
#define HDMI_CECTxCR_TX_EOM_MASK                    (1 << HDMI_CECTxCR_TX_EOM_SHIFT)
#define HDMI_CECTxCR_TX_INT                         (1 << 6)
#define HDMI_CECTxCR_TX_FIFO_UD                     (1 << 5)
#define HDMI_CECTxCR_TX_FIFO_CNT_SHIFT              (0)
#define HDMI_CECTxCR_TX_FIFO_CNT_MASK               (0x1F << HDMI_CECTxCR_TX_FIFO_CNT_SHIFT)

/********************************************************************************/
/* HDMI_CECTxDR */
#define HDMI_CECTxDR_CECTXFIFODOUT(x)               (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDMI_CECRxDR */
#define HDMI_CECRxDR_CECRXFIFODIN(x)                (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDMI_CECRxTCR */
#define HDMI_CECRxTCR_RX_START_LOW(x)               (((x) & 0xFF) << 24)
#define HDMI_CECRxTCR_RX_START_PERIOD(x)            (((x) & 0xFF) << 16)
#define HDMI_CECRxTCR_RX_DATA_SAMPLE(x)             (((x) & 0xFF) << 8)
#define HDMI_CECRxTCR_RX_DATA_PERIOD(x)             (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDMI_CECTxTCR0 */
#define HDMI_CECTxTCR0_TX_START_LOW(x)              (((x) & 0xFF) << 8)
#define HDMI_CECTxTCR0_TX_START_HIGH(x)             (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDMI_CECTxTCR1 */
#define HDMI_CECTxTCR1_TX_DATA_LOW(x)               (((x) & 0xFF) << 16)
#define HDMI_CECTxTCR1_TX_DATA_01(x)                (((x) & 0xFF) << 8)
#define HDMI_CECTxTCR1_TX_DATA_HIGH(x)              (((x) & 0xFF) << 0)

/********************************************************************************/
/* HDMI_CRCCR */
#define HDMI_CRCCR_CRCCHSEL(x)                      (((x) & 0x3) << 0)

/********************************************************************************/
/* HDMI_CRCDOR */

/********************************************************************************/
/* HDMI_TX_1 */
#define HDMI_TX_1_REG_TX_VGATE(x)                   (((x) & 0x7) << 29)
#define HDMI_TX_1_REG_TX_PLL_VG(x)                  (((x) & 0x7) << 26)
#define HDMI_TX_1_REG_TX_PLL_RS(x)                  (((x) & 0x3) << 24)
#define HDMI_TX_1_REG_TX_PLL_PU                     (1 << 23)
#define HDMI_TX_1_REG_TX_PLL_ICP(x)                 (((x) & 0x7) << 20)
#define HDMI_TX_1_REG_TX_PLL_CAT_EN                 (1 << 19)
#define HDMI_TX_1_REG_TX_PLL_FBAND(x)               (((x) & 0x7) << 16)
#define HDMI_TX_1_REG_TX_PLL_CS(x)                  (((x) & 0x3) << 14)
#define HDMI_TX_1_REG_TX_PLL_EDGE                   (1 << 13)
#define HDMI_TX_1_REG_TX_SET_VC(x)                  (((x) & 0x3) << 11)
#define HDMI_TX_1_REG_TX_FORCE_VC                   (1 << 10)
#define HDMI_TX_1_REG_TX_EMPH(x)                    (((x) & 0x3) << 8)
#define HDMI_TX_1_REG_TX_EN_EMPH                    (1 << 7)
#define HDMI_TX_1_REG_TX_BYPASS_CAL                 (1 << 6)
#define HDMI_TX_1_REG_TX_IBIAS(x)                   (((x) & 0x3) << 4)
#define HDMI_TX_1_REG_TX_AMP(x)                     (((x) & 0xF) << 0)

/********************************************************************************/
/* HDMI_TX_2 */
#define HDMI_TX_2_REG_TX_CK5XP_DUTY(x)              (((x) & 0x3) << 22)
#define HDMI_TX_2_REG_TX_CK5XN_DUTY(x)              (((x) & 0x3) << 20)
#define HDMI_TX_2_REG_TX_RT_SEL(x)                  (((x) & 0x3) << 18)
#define HDMI_TX_2_REG_TX_RT_EN                      (1 << 17)
#define HDMI_TX_2_REG_TX_BYPASS_PLL                 (1 << 16)
#define HDMI_TX_2_REG_TST_SEL(x)                    (((x) & 0x3) << 14)
#define HDMI_TX_2_REG_PLL_TST_EN                    (1 << 13)
#define HDMI_TX_2_REG_TX_TST_EN                     (1 << 12)
#define HDMI_TX_2_REG_TX_PU(x)                      (((x) & 0xF) << 8)
#define HDMI_TX_2_REG_TX_SLEW(x)                    (((x) & 0x3) << 6)
#define HDMI_TX_2_REG_TX_FORCE_VC                   (1 << 5)
#define HDMI_TX_2_REG_TX_PLL_CAL_DONE               (1 << 4)
#define HDMI_TX_2_REG_TX_PLL_FBAND(x)               (((x) & 0x7) << 1)
#define HDMI_TX_2_REG_TX_PLL_LOCK                   (1 << 0)

/********************************************************************************/
/* CEC_DDC_HPD */
#define CEC_DDC_HPD_REG_HPD_RPD_EN                  (1 << 6)
#define CEC_DDC_HPD_REG_CEC_RPU_EN                  (1 << 4)
#define CEC_DDC_HPD_REG_CEC_RSEL(x)                 (((x) & 0x7) << 1)
#define CEC_DDC_HPD_REG_CEC_ENB                     (1 << 0)

#ifdef __cplusplus
}
#endif

#endif  /* ifndef __ATV230x_HDMI_H__ */
