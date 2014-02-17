/*
 * for Actions AOTG 
 *
 */

#ifndef  __AOTG_REGS_H__
#define  __AOTG_REGS_H__

#define USB3_REGISTER_BASE          0xB0400000
#define DWC3_DCFG                   0xc700
#define DWC3_ACTIONS_REGS_START     (0xcd00)
#define DWC3_ACTIONS_REGS_END       (0xcd54)
#define DWC3_CDR_KIKD          (0xcd00)
#define DWC3_CDR_KP1           (0xcd04)
#define DWC3_TIMER_INIT        (0xcd08)
#define DWC3_CDR_CONTROL       (0xcd0c)
#define DWC3_RX_OFFSET_PS      (0xcd10)
#define DWC3_EQ_CONTROL        (0xcd14)
#define DWC3_RX_OOBS_SSC0      (0xcd18)
#define DWC3_CMU_SSC1          (0xcd1C)
#define DWC3_CMU_DEBUG_LDO     (0xcd20)
#define DWC3_TX_AMP_DEBUG      (0xcd24)
#define DWC3_Z0                (0xcd28)
#define DWC3_DMR_BACR          (0xcd2C)
#define DWC3_IER_BCSR          (0xcd30)
#define DWC3_BPR               (0xcd34)
#define DWC3_BFNR              (0xcd38)
#define DWC3_BENR_REV          (0xcd3C)
#define DWC3_FLD               (0xcd40)
#define DWC3_CMU_PLL2_BISTDEBUG    (0xcd44)
#define DWC3_USB2_P0_VDCTL     (0xcd48)
#define DWC3_BACKDOOR          (0xcd4C)
#define DWC3_EXT_CTL           (0xcd50)
#define DWC3_EFUSE_CTR         (0xcd54)
#define DWC3_GCTL               (0xc110)
#define USB3_MOD_RST           (1 << 14)
#define CMU_BIAS_EN            (1 << 20)
#define BIST_QINIT(n)          ((n) << 24)
#define EYE_HEIGHT(n)          ((n) << 20)
#define PLL2_LOCK              (1 << 15)
#define PLL2_RS(n)             ((n) << 12)
#define PLL2_ICP(n)            ((n) << 10)
#define CMU_SEL_PREDIV         (1 << 9)
#define CMU_DIVX2              (1 << 8)
#define PLL2_DIV(n)            ((n) << 3)
#define PLL2_POSTDIV(n)        ((n) << 1)
#define PLL2_PU                (1 << 0)
#define DWC3_GCTL_CORESOFTRESET		(1 << 11)
#define DWC3_GCTL_PRTCAP(n)       (((n) & (3 << 12)) >> 12)
#define DWC3_GCTL_PRTCAPDIR(n)    ((n) << 12)
#define DWC3_GCTL_PRTCAP_HOST     1
#define DWC3_GCTL_PRTCAP_DEVICE   2
#define DWC3_GCTL_PRTCAP_OTG	    3
#define HOST_DETECT_STEPS          5
#define DEVICE_DETECT_STEPS        5
#define HOST_CONFIRM_STEPS         3
#define DEVICE_CONFIRM_STEPS       4
#define USB3_P0_CTL_VBUS_P0       5
#define USB3_P0_CTL_ID_P0         11
#define USB3_P0_CTL_DPPUEN_P0     14
#define USB3_P0_CTL_DMPUEN_P0     15
#define USB3_P0_CTL_DPPDDIS_P0    12
#define USB3_P0_CTL_DMPDDIS_P0    13
#define USB3_P0_CTL_SOFTIDEN_P0   8
#define USB3_P0_CTL_SOFTID_P0     9
#define USB3_P0_CTL_SOFTVBUSEN_P0 6
#define USB3_P0_CTL_SOFTVBUS_P0   7
#define USB3_P0_CTL_PLLLDOEN      28
#define USB3_P0_CTL_LDOVREFSEL_SHIFT  29
#define USB3_P0_CTL_LS_P0_SHIFT   3
#define USB3_P0_CTL_LS_P0_MASK    (0x3<<3)

#define VBUS_THRESHOLD		3800 

#endif  /* __AOTG_REGS_H__ */
