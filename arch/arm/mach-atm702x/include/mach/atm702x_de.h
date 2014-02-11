/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2009 Actions Semi Inc.
*/
/******************************************************************************/

/******************************************************************************/
#ifndef __ATM702X_DE_H__
#define __ATM702X_DE_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
#define RGB_565                0
#define ARGB_8888              1
#define YUV_420_SEMI_PLANAR    2
#define YUV_420_PLANAR         3
#define ABGR_1555              4
#define ABGR_8888              5
#define YVU_420_PLANAR         6
/******************************************************************************/
#define     VIDEO_OFF_CFG                                                        0x0000
#define     VIDEO_OFF_ISIZE                                                      0x0004
#define     VIDEO_OFF_OSIZE                                                      0x0008
#define     VIDEO_OFF_SR                                                         0x000C
#define     VIDEO_OFF_SCOEF0                                                     0x0010
#define     VIDEO_OFF_SCOEF1                                                     0x0014
#define     VIDEO_OFF_SCOEF2                                                     0x0018
#define     VIDEO_OFF_SCOEF3                                                     0x001C
#define     VIDEO_OFF_SCOEF4                                                     0x0020
#define     VIDEO_OFF_SCOEF5                                                     0x0024
#define     VIDEO_OFF_SCOEF6                                                     0x0028
#define     VIDEO_OFF_SCOEF7                                                     0x002C
#define     VIDEO_OFF_FB_0                                                       0x0030
#define     VIDEO_OFF_FB_1                                                       0x0034
#define     VIDEO_OFF_FB_2                                                       0x0038
#define     VIDEO_OFF_FB_RIGHT_0                                                 0x003C
#define     VIDEO_OFF_FB_RIGHT_1                                                 0x0040
#define     VIDEO_OFF_FB_RIGHT_2                                                 0x0044
#define     VIDEO_OFF_STR                                                        0x0048
#define     VIDEO_OFF_CRITICAL                                                   0x004C
#define     VIDEO_OFF_REMAPPING                                                  0x0050


/******************************************************************************/
/* PATHx_CTL */
#define GRAPHIC_LAYER               0
#define VIDEO1_LAYER                1
#define VIDEO2_LAYER                3
#define VIDEO3_LAYER                5

#define DE_LAYER_A                0
#define DE_LAYER_B                1

#define DE_PATH_1                 0
#define DE_PATH_2                 1  


#define PATHx_CTL_SELB(x)          (((x) & 0x7) << 20)
#define PATHx_CTL_SELA(x)          (((x) & 0x7) << 16)
#define PATHx_CTL_CSC_CONTROL_BIT601	     (0x0 << 15)
#define PATHx_CTL_CSC_CONTROL_BIT709	     (0x1 << 15)
#define PATHx_CTL_DITHER_B                 (0x1 << 14)
#define PATHx_CTL_DITHER_G                 (0x1 << 13)
#define PATHx_CTL_DITHER_R                 (0x1 << 12)
#define PATHx_CTL_INTERLACE_ENABLE				 (0x1 << 9)
#define PATHx_CTL_3D											 (0x1 << 8)
#define PATHx_CTL_GAMMAYUV_ENABLE					 (0x1 << 7)
#define PATHx_CTL_GAMMARGB_ENABLE          (0x1 << 6)
#define PATHx_CTL_DITHER_ENABLE						 (0x1 << 5)
#define PATHx_CTL_CK_ENABLE                (0x1 << 4)
#define PATHx_CTL_ALPHA_ENABLE_A           (0x1 << 2)
#define PATHx_CTL_INPUTB_ENABLE            (0x1 << 1)
#define PATHx_CTL_INPUTA_ENABLE            (0x1 << 0)
/******************************************************************************/
/*GRAPHIC_CFG*/
#define CRITICAL_AUTO                     0
#define CRITICAL_ALWAYS_ON                1
#define CRITICAL_ALWAYS_OFF               2

#define GRAPHIC_FMT_RGB_565               0
#define GRAPHIC_FMT_ARGB_888              1
#define GRAPHIC_FMT_ABGR_888              5

#define GRAPHIC_CFG_CRITICAL(x)  					(((x) & 0x3) << 22)
#define GRAPHIC_CFG_XFLIP									(1  << 21)
#define GRAPHIC_CFG_YFLIP 								(1  << 20)
#define GRAPHIC_CFG_BRI(x)			 					(((x) & 0xff) << 12)
#define GRAPHIC_CFG_SAT(x)			 					(((x) & 0xf) << 8)
#define GRAPHIC_CFG_CON(x)       					(((x) & 0xf) << 4)
#define GRAPHIC_CFG_FMT_MASK               0x7
#define GRAPHIC_CFG_FMT(x)       					(((x) & GRAPHIC_CFG_FMT_MASK) << 0)
/******************************************************************************/
/*VIDEO_CFG*/
#define CRITICAL_AUTO                     0
#define CRITICAL_ALWAYS_ON                1
#define CRITICAL_ALWAYS_OFF               2 

#define VIDEO_CFG_CSC_IYUV_QEN 						(1  << 29)
#define VIDEO_CFG_CSC_IYUV_FMT 						(1  << 28)
#define VIDEO_CFG_CRITICAL(x)  						(((x) & 0x3) << 26)
#define VIDEO_CFG_YSUB				 						(((x) & 0x3) << 24)
#define VIDEO_CFG_XSUB				 						(((x) & 0x3) << 22)
#define VIDEO_CFG_XFLIP				 						(1  << 21)
#define VIDEO_CFG_YFLIP 			 						(1  << 20)
#define VIDEO_CFG_BRI(x)			 						(((x) & 0xff) << 12)
#define VIDEO_CFG_SAT(x)			 						(((x) & 0xf) << 8)
#define VIDEO_CFG_CON(x)       						(((x) & 0xf) << 4)
#define VIDEO_CFG_FMT_MASK               0x7
#define VIDEO_CFG_FMT(x)       						(((x) & 0x7) << 0)

/******************************************************************************/
/*MDSB_CTL*/
/*bit 11-31 Reserved*/

#define MDSB_CTL_IMG5_OUTPUT_LCD0   (0X00 << 10)
#define MDSB_CTL_IMG5_OUTPUT_LCD1   (0X01 << 10)

#define MDSB_CTL_IMG4_OUTPUT_LCD0   (0X00 << 9)
#define MDSB_CTL_IMG4_OUTPUT_DSI    (0X01 << 9)

#define MDSB_CTL_LCD0_SCALE         (0X01 << 8)
#define MDSB_CTL_HDMI_SEL           (0X01 << 7)
#define MDSB_CTL_YPbPr_VGA_SEL      (0X01 << 6)
#define MDSB_CTL_CVBS_SCALE					(0X01 << 5)
#define MDSB_CTL_IMG5_EN            (0X01 << 4)
#define MDSB_CTL_IMG4_EN            (0X01 << 3)
#define MDSB_CTL_IMG3_EN            (0X01 << 2)
#define MDSB_CTL_IMG2_EN            (0X01 << 1)
#define MDSB_CTL_IMG1_EN            (0X01)

/******************************************************************************/
/*DE_PHCTL*/
/*bit 9-31 Reserved*/
#define DE_PHCTL_TV_SEL_MASK				(0x1 << 8)
#define DE_PHCTL_TV_SEL_CVBS				(0x0 << 8)
#define DE_PHCTL_TV_SEL_YPBPR				(0x1 << 8)

#define DE_PHCTL_INTERLACE_EN				(0x1 << 7)

#define DE_PHCTL_WBF_MASK						(0x3 << 5)
#define DE_PHCTL_WBF_RGB565         (0x0 << 5)
#define DE_PHCTL_WBF_YCBYCR         (0x1 << 5)
#define DE_PHCTL_WBF_XRGB8888       (0x2 << 5)
#define DE_PHCTL_WBF_YUV420P				(0x3 << 5)

#define DE_PHCTL_PHASE_EN           (0x1 << 4)

#define DE_PHCTL_DUALDISPLAY_MASK		(0x3 << 2)
#define DE_PHCTL_DUALDISPLAY_SGV    (0x0 << 2)
#define DE_PHCTL_DUALDISPLAY_DGV    (0x1 << 2)
#define DE_PHCTL_DUALDISPLAY_SG     (0x2 << 2)

#define DE_PHCTL_OPS_MASK           (0x3 << 0)
#define DE_PHCTL_OPS_LCD            (0x0 << 0)
#define DE_PHCTL_OPS_TVOUT          (0x1 << 0)
#define DE_PHCTL_OPS_DMA           	(0x2 << 0)
#define DE_PHCTL_OPS_HDMI           (0x3 << 0)


/******************************************************************************/
/*DE_BLKEN*/
/*bit 14-31 Reserved*/
#define DE_BLKEN_PALETTE_EN         (0x1 << 13)
#define DE_BLKEN_DITHER_EN          (0x1 << 12)
/*bit 11 Reserved*/
#define DE_BLKEN_GAMMA_EN           (0x1 << 10)
/*bit 9 Reserved*/
#define DE_BLKEN_CSC2_MASK          (0x3 << 7)
#define DE_BLKEN_CSC2_DISABLE       (0x0 << 7)
#define DE_BLKEN_CSC2_BT601         (0x1 << 7)
#define DE_BLKEN_CSC2_BT709         (0x2 << 7)

#define DE_BLKEN_CSC13_MASK         (0x3 << 5)
#define DE_BLKEN_CSC13_DISABLE      (0x0 << 5)
#define DE_BLKEN_CSC13_BT601        (0x1 << 5)
#define DE_BLKEN_CSC13_BT709        (0x2 << 5)

#define DE_BLKEN_SCALER_EN          (0x1 << 4)
#define DE_BLKEN_LAYER3_EN          (0x1 << 3)
#define DE_BLKEN_LAYER2_EN          (0x1 << 2)
#define DE_BLKEN_LAYER1_EN          (0x1 << 1)
#define DE_BLKEN_LAYER0_EN          (0x1 << 0)

/******************************************************************************/
/*DE_OUTFIFO*/

/******************************************************************************/
/*DE_LRX_SIZE*/
/*bit 27-31 Reserved*/
#define DE_LRX_SIZE_HEIGHT_MASK     (0x7FF << 16)
#define DE_LRX_SIZE_HEIGHT(x)       (((x) & 0x7FF) << 16)
/*bit 11-15 Reserved*/
#define DE_LRX_SIZE_WIDTH_MASK      (0x7FF << 0)
#define DE_LRX_SIZE_WIDTH(x)        (((x) & 0x7FF) << 0)


/******************************************************************************/
/*DE_LRX_COR*/
/*bit 26-31 Reserved*/
#define DE_LRX_COR_Y_MASK           (0x3FF << 16)
#define DE_LRX_COR_Y(x)             (((x) & 0x3FF) << 16)
/*bit 11-15 Reserved*/
#define DE_LRX_COR_X_MASK           (0x7FF << 0)
#define DE_LRX_COR_X(x)             (((x) & 0x7FF) << 0)


/******************************************************************************/
/*DE_LRX_CFG*/
/*bit 21-31 Reserved*/
#define DE_LRX_CFG_OVLAP_MASK				(0x1 << 20)
#define DE_LRX_CFG_OVLAP_OV					(0x0 << 20)
#define DE_LRX_CFG_OVLAP_UD					(0x1 << 20)

#define DE_LRX_CFG_2X_EN            (0x1 << 19)

#define DE_LRX_CFG_FMT_MASK					(0x7 << 16)
#define DE_LRX_CFG_FMT_1BPP					(0x0 << 16)
#define DE_LRX_CFG_FMT_2BPP					(0x1 << 16)
#define DE_LRX_CFG_FMT_4BPP					(0x2 << 16)
#define DE_LRX_CFG_FMT_8BPP					(0x3 << 16)
#define DE_LRX_CFG_FMT_RGB565				(0x4 << 16)
#define DE_LRX_CFG_FMT_ARGB1555			(0x5 << 16)
#define DE_LRX_CFG_FMT_ARGB8888			(0x6 << 16)
#define DE_LRX_CFG_FMT_YUVI					(0x7 << 16)

/*bit 14-15 Reserved*/
#define DE_LRX_CFG_YFLIP_EN         (0x1 << 13)
#define DE_LRX_CFG_CKM_EN           (0x1 << 12)
/*bit 10-11 Reserved*/
#define DE_LRX_CFG_SPLIT_EN         (0x1 << 9)
#define DE_LRX_CFG_ALPHA_EN					(0x1 << 8)

#define DE_LRX_CFG_ALPHA_MASK			  (0x0FF << 0)
#define DE_LRX_CFG_ALPHA(x) 			  (((x) & 0x0FF) << 0)


/******************************************************************************/
/*DE_LR3_CFG*/
/*bit 19-31 Reserved*/
#define DE_LR3_CFG_FMT_MASK					(0x7 << 16)
#define DE_LR3_CFG_FMT_YUV420P			(0x0 << 16)
#define DE_LR3_CFG_FMT_YUV422I			(0x1 << 16)
#define DE_LR3_CFG_FMT_RGB565				(0x2 << 16)
#define DE_LR3_CFG_FMT_YUV420SP			(0x3 << 16)
#define DE_LR3_CFG_FMT_YUVSPC				(0x4 << 16)

#define DE_LR3_CFG_COL_EN						(0x1 << 15)
#define DE_LR3_CFG_XFLIP_EN					(0x1 << 14)
#define DE_LR3_CFG_YFLIP_EN         (0x1 << 13)
#define DE_LR3_CFG_CKM_EN           (0x1 << 12)
/*bit 9-11 Reserved*/
#define DE_LR3_CFG_ALPHA_ENABLE     (0x1 << 8)

#define DE_LR3_CFG_ALPHA_MASK			  (0x0FF << 0)
#define DE_LR3_CFG_ALPHA(x)				  (((x) & 0x0FF) << 0)

/******************************************************************************/
/*DE_STRX*/
/*bit 12-31 Reserved*/
#define DE_STRX_STR_MASK            (0x0FFF << 0)
#define DE_STRX_STR(x)              (((x) & 0x0FFF) << 0)


/******************************************************************************/
/*DE_STR3*/
/*bit 31 Reserved*/
#define DE_STR3_STRV_MASK           (0x3FF << 21)
#define DE_STR3_STRV(x)             (((x) & 0x3FF) << 21)
#define DE_STR3_STRU_MASK           (0x3FF << 11)
#define DE_STR3_STRU(x)             (((x) & 0x3FF) << 11)
#define DE_STR3_STRY_MASK           (0x7FF << 0)
#define DE_STR3_STRY(x)             (((x) & 0x7FF) << 0)

/*bit 22-31 Reserved*/
#define DE_STR3_STRUV_MASK          (0x7FF << 11)
#define DE_STR3_STRUV(x)            (((x) & 0x7FF) << 11)
#define DE_STR3_STRY_MASK           (0x7FF << 0)
#define DE_STR3_STRY(x)             (((x) & 0x7FF) << 0)

/*bit 12-31 Reserved*/
#define DE_STR3_STR_MASK            (0x0FFF << 0)
#define DE_STR3_STR(x)              (((x) & 0x0FFF) << 0)

#define DE_STR3_STRUVL_MASK         (0x0FF << 24)
#define DE_STR3_STRUVL(x)           (((x) & 0x0FF) << 24)
#define DE_STR3_STRYL_MASK          (0x0FF << 16)
#define DE_STR3_STRYL(x)            (((x) & 0x0FF) << 16)
#define DE_STR3_STRUVC_MASK         (0x0FF << 8)
#define DE_STR3_STRUVC(x)           (((x) & 0x0FF) << 8)
#define DE_STR3_STRYC_MASK          (0x0FF << 0)
#define DE_STR3_STRYC(x)            (((x) & 0x0FF) << 0)


/******************************************************************************/
/*DE_CKMAX*/
/*bit 24-31 Reserved*/
#define DE_CKMAX_RED_MASK           (0x0FF << 16)
#define DE_CKMAX_RED(x)             (((x) & 0x0FF) << 16)
#define DE_CKMAX_GREEN_MASK         (0x0FF << 8)
#define DE_CKMAX_GREEN(x)           (((x) & 0x0FF) << 8)
#define DE_CKMAX_BLUE_MASK          (0x0FF << 0)
#define DE_CKMAX_BLUE(x)            (((x) & 0x0FF) << 0)

/******************************************************************************/
/*DE_CKMIN*/
/*bit 24-31 Reserved*/
#define DE_CKMIN_RED_MASK           (0x0FF << 16)
#define DE_CKMIN_RED(x)             (((x) & 0x0FF) << 16)
#define DE_CKMIN_GREEN_MASK         (0x0FF << 8)
#define DE_CKMIN_GREEN(x)           (((x) & 0x0FF) << 8)
#define DE_CKMIN_BLUE_MASK          (0x0FF << 0)
#define DE_CKMIN_BLUE(x)            (((x) & 0x0FF) << 0)

/******************************************************************************/
/*DE_CKCFG*/
/*bit 6-31 Reserved*/
#define DE_CKCFG_RED_MASK           (0x3 << 4)
#define DE_CKCFG_RED(x)             (((x) & 0x3) << 4)
#define DE_CKCFG_GREEN_MASK         (0x3 << 2)
#define DE_CKCFG_GREEN(x)           (((x) & 0x3) << 2)
#define DE_CKCFG_BLUE_MASK          (0x3 << 0)
#define DE_CKCFG_BLUE(x)            (((x) & 0x3) << 0)

/******************************************************************************/
/*DE_DITHER*/
/*bit 3-31 Reserved*/
#define DE_DITHER_R_MASK            (0x1 << 2)
#define DE_DITHER_R_6BIT            (0x0 << 2)
#define DE_DITHER_R_5BIT 	          (0x1 << 2)

#define DE_DITHER_G_MASK            (0x1 << 1)
#define DE_DITHER_G_6BIT            (0x0 << 1)
#define DE_DITHER_G_5BIT            (0x1 << 1)

#define DE_DITHER_B_MASK            (0x1 << 0)
#define DE_DITHER_B_6BIT            (0x0 << 0)
#define DE_DITHER_B_5BIT            (0x1 << 0)

/******************************************************************************/
/*DE_LR3_SUBSIZE*/
#define DE_LR3_SUBSIZE_YSUB_MASK    (0x0F << 28)
#define DE_LR3_SUBSIZE_YSUB(x)      (((x) & 0x0F) << 28)
/*bit 27 Reserved*/
#define DE_LR3_SUBSIZE_HEIGHT_MASK	(0x7FF << 16)
#define DE_LR3_SUBSIZE_HEIGHT(x)    (((x) & 0x7FF) << 16)

#define DE_LR3_SUBSIZE_XSUB_MASK    (0x0F << 12)
#define DE_LR3_SUBSIZE_XSUB(x)      (((x) & 0x0F) << 12)
/*bit 11 Reserved*/
#define DE_LR3_SUBSIZE_WIDTH_MASK   (0x7FF << 0)
#define DE_LR3_SUBSIZE_WIDTH(x)     (((x) & 0x7FF) << 0)

/******************************************************************************/
/*DE_SCLCOEFX*/
#define DE_SCLCOEFX_TAP3_MASK       (0xFF << 24)
#define DE_SCLCOEFX_TAP3(x)         (((x) & 0xFF) << 24)
#define DE_SCLCOEFX_TAP2_MASK       (0xFF << 16)
#define DE_SCLCOEFX_TAP2(x)         (((x) & 0xFF) << 16)
#define DE_SCLCOEFX_TAP1_MASK       (0xFF << 8)
#define DE_SCLCOEFX_TAP1(x)         (((x) & 0xFF) << 8)
#define DE_SCLCOEFX_TAP0_MASK       (0xFF << 0)
#define DE_SCLCOEFX_TAP0(x)         (((x) & 0xFF) << 0)


/******************************************************************************/
/*DE_SCLOFS*/
/*bit 27-31 Reserved*/
#define DE_SCLOFS_OFH_MASK          (0x7FF << 16)
#define DE_SCLOFS_OFH(x)            (((x) & 0x7FF) << 16)
/*bit 11-15 Reserved*/
#define DE_SCLOFS_OFW_MASK          (0x7FF << 0)
#define DE_SCLOFS_OFW(x)            (((x) & 0x7FF) << 0)

/******************************************************************************/
/*DE_SCLSR*/
#define DE_SCLSR_VSR_MASK           (0xFFFF << 16)
#define DE_SCLSR_VSR(x)             (((x) & 0xFFFF) << 16)
#define DE_SCLSR_HSR_MASK           (0xFFFF << 0)
#define DE_SCLSR_HSR(x)             (((x) & 0xFFFF) << 0)

/******************************************************************************/
/*DE_BRI*/
/*bit 8-31 Reserved*/
#define DE_BRI_BRI_MASK             (0xFF << 0)
#define DE_BRI_BRI(x)               (((x) & 0xFF) << 0)

/******************************************************************************/
/*DE_CON*/
/*bit 4-31 Reserved*/
#define DE_CON_CON_MASK             (0x0F << 0)
#define DE_CON_CON(x)               (((x) & 0x0F) << 0)

/******************************************************************************/
/*DE_SAT*/
/*bit 4-31 Reserved*/
#define DE_SAT_SAT_MASK             (0x0F << 0)
#define DE_SAT_SAT(x)               (((x) & 0x0F) << 0)

/******************************************************************************/
/*DE_CRITICAL*/
/*bit 9-31 Reserved*/
#define DE_CRITICAL_COUNT_MASK      (0x1FF << 0)
#define DE_CRITICAL_COUNT(x)        (((x) & 0x1FF) << 0)

/******************************************************************************/
/*DE_INTEN*/
/*bit 7-31 Reserved*/
#define DE_INTEN_CRITICAL_MASK			(0x3 << 5)
#define DE_INTEN_CRITICAL_AUTO			(0x0 << 5)
#define DE_INTEN_CRITICAL_ON				(0x1 << 5)
#define DE_INTEN_CRITICAL_OFF				(0x2 << 5)

#define DE_INTEN_INT4               (0x1 << 4)
#define DE_INTEN_INT3               (0x1 << 3)
#define DE_INTEN_INT2               (0x1 << 2)
#define DE_INTEN_INT1               (0x1 << 1)
#define DE_INTEN_INT0               (0x1 << 0)

/******************************************************************************/
/*DE_INTSTA*/
/*bit 7-31 Reserved*/
#define DE_INTSTA_SCL_BSY           (0x1 << 6)
#define DE_INTSTA_GRA_BSY           (0x1 << 5)
#define DE_INTSTA_INT4              (0x1 << 4)
#define DE_INTSTA_INT3              (0x1 << 3)
#define DE_INTSTA_INT2              (0x1 << 2)
#define DE_INTSTA_INT1              (0x1 << 1)
#define DE_INTSTA_INT0              (0x1 << 0)

/******************************************************************************/
/*DE_PRELINE*/
/*bit 27-31 Reserved*/
#define DE_PRELINE_SCL_PRELINE_MASK (0x7FF << 16)
#define DE_PRELINE_SCL_PRELINE(x)   (((x) & 0x7FF) << 16)
/*bit 11-15 Reserved*/
#define DE_PRELINE_GRA_PRELINE_MASK (0x7FF << 0)
#define DE_PRELINE_GRA_PRELINE(x)	  (((x) & 0x7FF) << 0)

/******************************************************************************/
/*DE_RAMIDX*/
/*bit 14-31 Reserved*/
#define DE_RAMIDX_BUSY              (0x1 << 13)

#define DE_RAMIDX_WE_MASK           (0x1 << 12)
#define DE_RAMIDX_WE_W							(0x0 << 12)
#define DE_RAMIDX_WE_R							(0x1 << 12)

#define DE_RAMIDX_RAMSEL_MASK       (0x3 << 10)
#define DE_RAMIDX_RAMSEL_PALETTE    (0x1 << 10)
#define DE_RAMIDX_RAMSEL_GAMMA      (0x2 << 10)

#define DE_RAMIDX_RAMIDX_MASK       (0x3FF << 0)
#define DE_RAMIDX_RAMIDX(x)         (((x) & 0x3FF) << 0)

/******************************************************************************/
/*DE_RAMWIN*/


/******************************************************************************/
/*MDSB_CTL*/
/*bit 3-31 Reserved*/
#define MDSB_CTL_LCD_EN             (0x1 << 2)
#define MDSB_CTL_CVBS_EN            (0x1 << 1)
/*bit 0 Reserved*/

/******************************************************************************/
/*MDSB_CVBS_HR*/
/*bit 26-31 Reserved*/
#define MDSB_CVBS_HR_WIDTH_MASK        (0x3FF << 0)
#define MDSB_CVBS_HR_WIDTH(x)          (((x) & 0x3FF) << 0)
#define MDSB_CVBS_HR_ROUND_MASK        (0xFFFF << 0)
#define MDSB_CVBS_HR_ROUND(x)          (((x) & 0xFFFF) << 0)

/******************************************************************************/
/*MDSB_CVBS_VR*/
/*bit 26-31 Reserved*/
#define MDSB_CVBS_VR_HEIGHT_MASK       (0x3FF << 0)
#define MDSB_CVBS_VR_HEIGHT(x)         (((x) & 0x3FF) << 0)
#define MDSB_CVBS_VR_ROUND_MASK        (0xFFFF << 0)
#define MDSB_CVBS_VR_ROUND(x)          (((x) & 0xFFFF) << 0)

/******************************************************************************/
/*MDSB_LCD_HR*/
/*bit 27-31 Reserved*/
#define MDSB_LCD_HR_WIDTH_MASK 	       (0x7FF << 0)
#define MDSB_LCD_HR_WIDTH(x)   	       (((x) & 0x7FF) << 0)
#define MDSB_LCD_HR_ROUND_MASK 	       (0xFFFF << 0)
#define MDSB_LCD_HR_ROUND(x)  	       (((x) & 0xFFFF) << 0)

/******************************************************************************/
/*MDSB_LCD_VR*/
/*bit 26-31 Reserved*/
#define MDSB_LCD_VR_HEIGHT_MASK	       (0x3FF << 0)
#define MDSB_LCD_VR_HEIGHT(x)  	       (((x) & 0x3FF) << 0)
#define MDSB_LCD_VR_ROUND_MASK	       (0xFFFF << 0)
#define MDSB_LCD_VR_ROUND(x)  	       (((x) & 0xFFFF) << 0)

/******************************************************************************/
/*MDSB_STSEQ*/
/*bit 6-31 Reserved*/
#define MDSB_STSEQ2_MASK					       (0x3 << 4)
#define MDSB_STSEQ2_HDMI					       (0x0 << 4)
#define MDSB_STSEQ2_CVBS					       (0x1 << 4)
#define MDSB_STSEQ2_LCDC					       (0x2 << 4)

#define MDSB_STSEQ1_MASK					       (0x3 << 2)
#define MDSB_STSEQ1_HDMI					       (0x0 << 2)
#define MDSB_STSEQ1_CVBS					       (0x1 << 2)
#define MDSB_STSEQ1_LCDC					       (0x2 << 2)

#define MDSB_STSEQ0_MASK					       (0x3 << 0)
#define MDSB_STSEQ0_HDMI					       (0x0 << 0)
#define MDSB_STSEQ0_CVBS					       (0x1 << 0)
#define MDSB_STSEQ0_LCDC					       (0x2 << 0)


/******************************************************************************/
/*MDSB_DELAY0*/
/*bit 14-31 Reserved*/
#define MDSB_DELAY0_DELAY_MASK		       (0x3FFF << 0)
#define MDSB_DELAY0_DELAY(x)		   		   (((x) & 0x3FFF) << 0)


/******************************************************************************/
/*MDSB_DELAY1*/
/*bit 14-31 Reserved*/
#define MDSB_DELAY1_DELAY_MASK		       (0x3FFF << 0)
#define MDSB_DELAY1_DELAY(x)		   		   (((x) & 0x3FFF) << 0)

/******************************************************************************/
/*MDSB_CVBS_OFF*/
/*bit 16-31 Reserved*/
#define MDSB_CVBS_OFF_EVEN_ST_LN_MASK    (0xFF << 8)
#define MDSB_CVBS_OFF_EVEN_ST_LN(x)		   (((x) & 0xFF) << 8)
#define MDSB_CVBS_OFF_ODD_ST_LN_MASK		 (0xFF << 0)
#define MDSB_CVBS_OFF_ODD_ST_LN(x)		   (((x) & 0xFF) << 0)


/******************************************************************************/
/*MDSB_LCD_OFF*/
/*bit 14-31 Reserved*/
#define MDSB_LCD_OFF_SEL_ST_LN_MASK    	 (0x3FFF << 0)
#define MDSB_LCD_OFF_SEL_ST_LN(x)		   	 (((x) & 0x3FFF) << 0)

/******************************************************************************/
/*MDSB_HDMIWH*/
/*bit 27-31 Reserved*/
#define MDSB_HDMIWH_H_MASK					     (0x7FF << 16)
#define MDSB_HDMIWH_H(x)								 (((x) & 0x7FF) << 16)
/*bit 11-15 Reserved*/
#define MDSB_HDMIWH_W_MASK							 (0x7FF << 0)
#define MDSB_HDMIWH_W(x)							   (((x) & 0x7FF) << 0)

/******************************************************************************/
/*MDSB_DMACFG*/
/*bit 8-31 Reserved*/
#define MDSB_DMACFG_DMA_O_ERR				     (0x1 << 7)
#define MDSB_DMACFG_LCD_U_ERR				     (0x1 << 6)
#define MDSB_DMACFG_CVBS_U_ERR				   (0x1 << 5)
#define MDSB_DMACFG_HDMI_U_ERR				   (0x1 << 4)
/*bit 3 Reserved*/
#define MDSB_DMACFG_RB_SEL_MASK				   (0x3 << 1)
#define MDSB_DMACFG_RB_SEL_HDMI				   (0x0 << 1)
#define MDSB_DMACFG_RB_SEL_CVBS				   (0x1 << 1)
#define MDSB_DMACFG_RB_SEL_LCD				   (0x2 << 1)

#define MDSB_DMACFG_RB_EN							   (0x1 << 0)

/******************************************************************************/
/*MDSB_HSCALERX*/
#define MDSB_HSCALERX_TAP3_MASK       (0xFF << 24)
#define MDSB_HSCALERX_TAP3(x)         (((x) & 0xFF) << 24)
#define MDSB_HSCALERX_TAP2_MASK       (0xFF << 16)
#define MDSB_HSCALERX_TAP2(x)         (((x) & 0xFF) << 16)
#define MDSB_HSCALERX_TAP1_MASK       (0xFF << 8)
#define MDSB_HSCALERX_TAP1(x)         (((x) & 0xFF) << 8)
#define MDSB_HSCALERX_TAP0_MASK       (0xFF << 0)
#define MDSB_HSCALERX_TAP0(x)         (((x) & 0xFF) << 0)

/******************************************************************************/
/*MDSB_VSCALERX*/
/*bit 16-31 Reserved*/
#define MDSB_VSCALERX_TAP1_MASK       (0xFF << 8)
#define MDSB_VSCALERX_TAP1(x)         (((x) & 0xFF) << 8)
#define MDSB_VSCALERX_TAP0_MASK       (0xFF << 0)
#define MDSB_VSCALERX_TAP0(x)         (((x) & 0xFF) << 0)

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
