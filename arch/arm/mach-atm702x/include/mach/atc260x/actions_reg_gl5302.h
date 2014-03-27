/*
* Automatically generated register definition: don't edit
* GL5302 Spec Version_V1.0
* Sat 6-14-2011  14:20:33
*/
#ifndef __GL5302_REG_DEFINITION_H___
#define __GL5302_REG_DEFINITION_H___


//--------------PMU-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     PMU_BASE                                                          0x0000
#define     gl5302_PMU_SYS_CTL0                                                      (PMU_BASE+0x00)
#define     gl5302_PMU_SYS_CTL1                                                      (PMU_BASE+0x01)
#define     gl5302_PMU_SYS_CTL2                                                      (PMU_BASE+0x02)
#define     gl5302_PMU_SYS_CTL3                                                      (PMU_BASE+0x03)
#define     gl5302_PMU_SYS_CTL4                                                      (PMU_BASE+0x04)
#define     gl5302_PMU_SYS_CTL5                                                      (PMU_BASE+0x05)
#define     gl5302_PMU_SYS_CTL6                                                      (PMU_BASE+0x06)
#define     gl5302_PMU_SYS_CTL7                                                      (PMU_BASE+0x07)
#define     gl5302_PMU_SYS_CTL8                                                      (PMU_BASE+0x08)
#define     gl5302_PMU_SYS_CTL9                                                      (PMU_BASE+0x09)
#define     gl5302_PMU_BAT_CTL0                                                      (PMU_BASE+0x0A)
#define     gl5302_PMU_BAT_CTL1                                                      (PMU_BASE+0x0B)
#define     gl5302_PMU_VBUS_CTL0                                                     (PMU_BASE+0x0C)
#define     gl5302_PMU_VBUS_CTL1                                                     (PMU_BASE+0x0D)
#define     gl5302_PMU_WALL_CTL0                                                     (PMU_BASE+0x0E)
#define     gl5302_PMU_WALL_CTL1                                                     (PMU_BASE+0x0F)
#define     gl5302_PMU_SYS_Pending                                                   (PMU_BASE+0x10)
#define     gl5302_PMU_DC1_CTL0                                                      (PMU_BASE+0x11)
#define     gl5302_PMU_DC1_CTL1                                                      (PMU_BASE+0x12)
#define     gl5302_PMU_DC1_CTL2                                                      (PMU_BASE+0x13)
#define     gl5302_PMU_DC2_CTL0                                                      (PMU_BASE+0x14)
#define     gl5302_PMU_DC2_CTL1                                                      (PMU_BASE+0x15)
#define     gl5302_PMU_DC2_CTL2                                                      (PMU_BASE+0x16)
#define     gl5302_PMU_DC3_CTL0                                                      (PMU_BASE+0x17)
#define     gl5302_PMU_DC3_CTL1                                                      (PMU_BASE+0x18)
#define     gl5302_PMU_DC3_CTL2                                                      (PMU_BASE+0x19)
#define     gl5302_PMU_DC4_CTL0                                                      (PMU_BASE+0x1A)
#define     gl5302_PMU_DC4_CTL1                                                      (PMU_BASE+0x1B)
#define     gl5302_PMU_DC5_CTL0                                                      (PMU_BASE+0x1C)
#define     gl5302_PMU_DC5_CTL1                                                      (PMU_BASE+0x1D)
#define     gl5302_PMU_LDO1_CTL                                                      (PMU_BASE+0x1E)
#define     gl5302_PMU_LDO2_CTL                                                      (PMU_BASE+0x1F)
#define     gl5302_PMU_LDO3_CTL                                                      (PMU_BASE+0x20)
#define     gl5302_PMU_LDO4_CTL                                                      (PMU_BASE+0x21)
#define     gl5302_PMU_LDO5_CTL                                                      (PMU_BASE+0x22)
#define     gl5302_PMU_LDO6_CTL                                                      (PMU_BASE+0x23)
#define     gl5302_PMU_LDO7_CTL                                                      (PMU_BASE+0x24)
#define     gl5302_PMU_LDO8_CTL                                                      (PMU_BASE+0x25)
#define     gl5302_PMU_LDO9_CTL                                                      (PMU_BASE+0x26)
#define     gl5302_PMU_LDO10_CTL                                                     (PMU_BASE+0x27)
#define     gl5302_PMU_LDO11_CTL                                                     (PMU_BASE+0x28)
#define     gl5302_PMU_SWITCH_CTL                                                    (PMU_BASE+0x29)
#define     gl5302_PMU_OV_CTL0                                                       (PMU_BASE+0x2A)
#define     gl5302_PMU_OV_CTL1                                                       (PMU_BASE+0x2B)
#define     gl5302_PMU_OV_Status                                                     (PMU_BASE+0x2C)
#define     gl5302_PMU_OV_EN                                                         (PMU_BASE+0x2D)
#define     gl5302_PMU_OV_INT_EN                                                     (PMU_BASE+0x2E)
#define     gl5302_PMU_OC_CTL                                                        (PMU_BASE+0x2F)
#define     gl5302_PMU_OC_Status                                                     (PMU_BASE+0x30)
#define     gl5302_PMU_OC_EN                                                         (PMU_BASE+0x31)
#define     gl5302_PMU_OC_INT_EN                                                     (PMU_BASE+0x32)
#define     gl5302_PMU_UV_CTL0                                                       (PMU_BASE+0x33)
#define     gl5302_PMU_UV_CTL1                                                       (PMU_BASE+0x34)
#define     gl5302_PMU_UV_Status                                                     (PMU_BASE+0x35)
#define     gl5302_PMU_UV_EN                                                         (PMU_BASE+0x36)
#define     gl5302_PMU_UV_INT_EN                                                     (PMU_BASE+0x37)
#define     gl5302_PMU_OT_CTL                                                        (PMU_BASE+0x38)
#define     gl5302_PMU_CHARGER_CTL0                                                  (PMU_BASE+0x39)
#define     gl5302_PMU_CHARGER_CTL1                                                  (PMU_BASE+0x3A)
#define     gl5302_PMU_CHARGER_CTL2                                                  (PMU_BASE+0x3B)
#define     gl5302_PMU_BakCHARGER_CTL                                                (PMU_BASE+0x3C)
#define     gl5302_PMU_APDS_CTL                                                      (PMU_BASE+0x3D)
#define     gl5302_PMU_AuxADC_CTL0                                                   (PMU_BASE+0x3E)
#define     gl5302_PMU_AuxADC_CTL1                                                   (PMU_BASE+0x3F)
#define     gl5302_PMU_BATVADC                                                       (PMU_BASE+0x40)
#define     gl5302_PMU_BATIADC                                                       (PMU_BASE+0x41)
#define     gl5302_PMU_WALLVADC                                                      (PMU_BASE+0x42)
#define     gl5302_PMU_WALLIADC                                                      (PMU_BASE+0x43)
#define     gl5302_PMU_VBUSVADC                                                      (PMU_BASE+0x44)
#define     gl5302_PMU_VBUSIADC                                                      (PMU_BASE+0x45)
#define     gl5302_PMU_SYSPWRADC                                                     (PMU_BASE+0x46)
#define     gl5302_PMU_RemConADC                                                     (PMU_BASE+0x47)
#define     gl5302_PMU_SVCCADC                                                       (PMU_BASE+0x48)
#define     gl5302_PMU_CHGIADC                                                       (PMU_BASE+0x49)
#define     gl5302_PMU_IREFADC                                                       (PMU_BASE+0x4A)
#define     gl5302_PMU_BAKBATADC                                                     (PMU_BASE+0x4B)
#define     gl5302_PMU_ICTEMPADC                                                     (PMU_BASE+0x4C)
#define     gl5302_PMU_AuxADC0                                                       (PMU_BASE+0x4D)
#define     gl5302_PMU_AuxADC1                                                       (PMU_BASE+0x4E)
#define     gl5302_PMU_AuxADC2                                                       (PMU_BASE+0x4F)
#define     gl5302_PMU_AuxADC3                                                       (PMU_BASE+0x50)
#define     gl5302_PMU_BDG_CTL                                                       (PMU_BASE+0x51)
#define     gl5302_RTC_CTL                                                           (PMU_BASE+0x52)
#define     gl5302_RTC_MSALM                                                         (PMU_BASE+0x53)
#define     gl5302_RTC_HALM                                                          (PMU_BASE+0x54)
#define     gl5302_RTC_YMDALM                                                        (PMU_BASE+0x55)
#define     gl5302_RTC_MS                                                            (PMU_BASE+0x56)
#define     gl5302_RTC_H                                                             (PMU_BASE+0x57)
#define     gl5302_RTC_DC                                                            (PMU_BASE+0x58)
#define     gl5302_RTC_YMD                                                           (PMU_BASE+0x59)
#define     gl5302_EFUSE_DAT                                                         (PMU_BASE+0x5A)
#define     gl5302_EFUSECRTL1                                                        (PMU_BASE+0x5B)
#define     gl5302_EFUSECRTL2                                                        (PMU_BASE+0x5C)
#define     gl5302_IRC_CTL                                                           (PMU_BASE+0x60)
#define     gl5302_IRC_STAT                                                          (PMU_BASE+0x61)
#define     gl5302_IRC_CC                                                            (PMU_BASE+0x62)
#define     gl5302_IRC_KDC                                                           (PMU_BASE+0x63)
#define     gl5302_IRC_WK                                                            (PMU_BASE+0x64)
//--------------Bits Location------------------------------------------//
//--------------AUDIO_IN_OUT_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     AUDIO_IN_OUT_Register_BASE                                        0x0400
#define     gl5302_AUDIOINOUT_CTL                                                    (AUDIO_IN_OUT_Register_BASE+0x00)
#define     gl5302_AUDIO_DEBUGOUTCTL                                                 (AUDIO_IN_OUT_Register_BASE+0x01)
#define     gl5302_DAC_FILTERCTL0                                                    (AUDIO_IN_OUT_Register_BASE+0x02)
#define     gl5302_DAC_FILTERCTL1                                                    (AUDIO_IN_OUT_Register_BASE+0x03)
#define     gl5302_DAC_DIGITALCTL                                                    (AUDIO_IN_OUT_Register_BASE+0x04)
#define     gl5302_DAC_VOLUMECTL0                                                    (AUDIO_IN_OUT_Register_BASE+0x05)
#define     gl5302_DAC_VOLUMECTL1                                                    (AUDIO_IN_OUT_Register_BASE+0x06)
#define     gl5302_DAC_VOLUMECTL2                                                    (AUDIO_IN_OUT_Register_BASE+0x07)
#define     gl5302_DAC_VOLUMECTL3                                                    (AUDIO_IN_OUT_Register_BASE+0x08)
#define     gl5302_DAC_ANANLOG0                                                      (AUDIO_IN_OUT_Register_BASE+0x09)
#define     gl5302_DAC_ANANLOG1                                                      (AUDIO_IN_OUT_Register_BASE+0x0a)
#define     gl5302_DAC_ANANLOG2                                                      (AUDIO_IN_OUT_Register_BASE+0x0b)
#define     gl5302_DAC_ANANLOG3                                                      (AUDIO_IN_OUT_Register_BASE+0x0c)
#define     gl5302_DAC_ANANLOG4                                                      (AUDIO_IN_OUT_Register_BASE+0x0d)
#define     gl5302_CLASSD_CTL0                                                       (AUDIO_IN_OUT_Register_BASE+0x0e)
#define     gl5302_CLASSD_CTL1                                                       (AUDIO_IN_OUT_Register_BASE+0x0f)
#define     gl5302_CLASSD_CTL2                                                       (AUDIO_IN_OUT_Register_BASE+0x10)

//--------------Bits Location------------------------------------------//
//--------------AUDIO_IN_OUT_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     AUDIO_IN_OUT_Register_BASE                                        0x0400
#define     gl5302_ADC0_DIGITALCTL                                                   (AUDIO_IN_OUT_Register_BASE+0x11)
#define     gl5302_ADC0_HPFCTL                                                       (AUDIO_IN_OUT_Register_BASE+0x12)
#define     gl5302_ADC0_CTL                                                          (AUDIO_IN_OUT_Register_BASE+0x13)
#define     gl5302_AGC0_CTL0                                                         (AUDIO_IN_OUT_Register_BASE+0x14)
#define     gl5302_AGC0_CTL1                                                         (AUDIO_IN_OUT_Register_BASE+0x15)
#define     gl5302_AGC0_CTL2                                                         (AUDIO_IN_OUT_Register_BASE+0x16)
#define     gl5302_ADC_ANANLOG0                                                      (AUDIO_IN_OUT_Register_BASE+0x17)
#define     gl5302_ADC_ANANLOG1                                                      (AUDIO_IN_OUT_Register_BASE+0x18)
#define     gl5302_ADC1_DIGITALCTL                                                   (AUDIO_IN_OUT_Register_BASE+0x19)
#define     gl5302_ADC1_CTL                                                          (AUDIO_IN_OUT_Register_BASE+0x1A)
#define     gl5302_AGC1_CTL0                                                         (AUDIO_IN_OUT_Register_BASE+0x1B)
#define     gl5302_AGC1_CTL1                                                         (AUDIO_IN_OUT_Register_BASE+0x1C)
#define     gl5302_AGC1_CTL2                                                         (AUDIO_IN_OUT_Register_BASE+0x1D)

//--------------Bits Location------------------------------------------//
//--------------Ethernet_PHY_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     Ethernet_PHY_Register_BASE                                        0x0500
#define     gl5302_PHY_SMI_STAT                                                      (Ethernet_PHY_Register_BASE+0x00)
#define     gl5302_PHY_SMI_CONFIG                                                    (Ethernet_PHY_Register_BASE+0x01)
#define     gl5302_PHY_SMI_DATA                                                      (Ethernet_PHY_Register_BASE+0x02)
#define     gl5302_PHY_CTRL                                                          (Ethernet_PHY_Register_BASE+0x03)
#define     gl5302_PHY_ID1                                                           (Ethernet_PHY_Register_BASE+0x04)
#define     gl5302_PHY_ID2                                                           (Ethernet_PHY_Register_BASE+0x05)
#define     gl5302_PHY_ADDR                                                          (Ethernet_PHY_Register_BASE+0x06)
#define     gl5302_PHY_LED                                                           (Ethernet_PHY_Register_BASE+0x07)
#define     gl5302_PHY_INT_CTRL                                                      (Ethernet_PHY_Register_BASE+0x08)
#define     gl5302_PHY_INT_STAT                                                      (Ethernet_PHY_Register_BASE+0x09)
#define     gl5302_PHY_HW_RST                                                        (Ethernet_PHY_Register_BASE+0x0A)
#define     gl5302_PHY_CONFIG                                                        (Ethernet_PHY_Register_BASE+0x0B)
#define     gl5302_PHY_PLL_CTL0                                                      (Ethernet_PHY_Register_BASE+0x0C)
#define     gl5302_PHY_PLL_CTL1                                                      (Ethernet_PHY_Register_BASE+0x0D)
#define     gl5302_PHY_DBG0                                                          (Ethernet_PHY_Register_BASE+0x0E)
#define     gl5302_PHY_DBG1                                                          (Ethernet_PHY_Register_BASE+0x0F)
#define     gl5302_PHY_DBG2                                                          (Ethernet_PHY_Register_BASE+0x10)
#define     gl5302_PHY_DBG3                                                          (Ethernet_PHY_Register_BASE+0x11)
#define     gl5302_PHY_DBG4                                                          (Ethernet_PHY_Register_BASE+0x12)

//--------------Bits Location------------------------------------------//
//--------------TP_Controller_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     TP_Controller_Register_BASE                                       0x0600
#define     gl5302_TP_CTL0                                                           (TP_Controller_Register_BASE+0x0000)
#define     gl5302_TP_CTL1                                                           (TP_Controller_Register_BASE+0x0001)
#define     gl5302_TP_STATUS                                                         (TP_Controller_Register_BASE+0x0002)
#define     gl5302_TP_XDAT                                                           (TP_Controller_Register_BASE+0x0003)
#define     gl5302_TP_YDAT                                                           (TP_Controller_Register_BASE+0x0004)
#define     gl5302_TP_Z1DAT                                                          (TP_Controller_Register_BASE+0x0005)
#define     gl5302_TP_Z2DAT                                                          (TP_Controller_Register_BASE+0x0006)
//--------------Bits Location------------------------------------------//
//--------------CMU_Control_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     CMU_Control_Register_BASE                                         0x100
#define     gl5302_CMU_HOSCCTL                                                       (CMU_Control_Register_BASE+0x00)
#define     gl5302_CMU_DEVRST                                                        (CMU_Control_Register_BASE+0x01)

//--------------Bits Location------------------------------------------//
//--------------INTS_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     INTS_Register_BASE                                                0x200
#define     gl5302_INTS_PD                                                           (INTS_Register_BASE+0x00)
#define     gl5302_INTS_MSK                                                          (INTS_Register_BASE+0x01)

//--------------Bits Location------------------------------------------//
//--------------MFP_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     MFP_Register_BASE                                                 0x300
#define     gl5302_MFP_CTL0                                                          (MFP_Register_BASE+0x00)
#define     gl5302_MFP_CTL1                                                          (MFP_Register_BASE+0x01)
#define     gl5302_GPIO_OUTEN0                                                       (MFP_Register_BASE+0x10 )
#define     gl5302_GPIO_OUTEN1                                                       (MFP_Register_BASE+0x11 )
#define     gl5302_GPIO_INEN0                                                        (MFP_Register_BASE+0x12)
#define     gl5302_GPIO_INEN1                                                        (MFP_Register_BASE+0x13)
#define     gl5302_GPIO_DAT0                                                         (MFP_Register_BASE+0x14)
#define     gl5302_GPIO_DAT1                                                         (MFP_Register_BASE+0x15)
#define     gl5302_PAD_DRV0                                                          (MFP_Register_BASE+0x20)
#define     gl5302_PAD_DRV1                                                          (MFP_Register_BASE+0x21)
#define     gl5302_PAD_EN                                                            (MFP_Register_BASE+0x22)
#define     gl5302_DEBUG_SEL                                                         (MFP_Register_BASE+0x30)
#define     gl5302_DEBUG_IE                                                          (MFP_Register_BASE+0x31)
#define     gl5302_DEBUG_OE                                                          (MFP_Register_BASE+0x32)

//--------------Bits Location------------------------------------------//
//--------------TEST_Conrol_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     TEST_Conrol_Register_BASE                                         0x700
#define     gl5302_TEST_MODE_CFG                                                     (TEST_Conrol_Register_BASE+0x00)
#define     gl5302_TEST_CFG_EN                                                       (TEST_Conrol_Register_BASE+0x01)
#define     gl5302_AUDIO_MEM_BIST_CTL                                                (TEST_Conrol_Register_BASE+0x02)
#define     gl5302_AUDIO_MEM_BIST_RESULT                                             (TEST_Conrol_Register_BASE+0x03)
#define     gl5302_CVER                                                              (TEST_Conrol_Register_BASE+0x04)

//--------------Bits Location------------------------------------------//
//---------------------------------------------------------------------//
#endif
