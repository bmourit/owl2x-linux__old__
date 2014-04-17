/*
* Automatically generated register definition: don't edit
* GL5202 Spec Version_V2.02
* Sat 05-21-2012  11:30:38
*/
#ifndef __OWL2X_REG_DEFINITION_H___
#define __OWL2X_REG_DEFINITION_H___


//--------------Bits Location------------------------------------------//
//--------------SPS_PG-------------------------------------------//
//--------------Register Address---------------------------------------//
#define SPS_PG_BASE             0xB01C0100
#define SPS_PG_CTL              (SPS_PG_BASE+0x0000)
#define SPS_PG_ACK              (SPS_PG_BASE+0x0004)
#define SPS_BDG_CTL             (SPS_PG_BASE+0x0008)

/* USB OTG Registers */
#define AOTG0_BASE              0xB0120000
#define AOTG1_BASE              0xB0128000
//--------------Bits Location------------------------------------------//
//--------------CMU_Control_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define CMU_CTL_BASE                    0xB0150000

/* Clock Controller Offsets */
#define CMU_DEVPLL              0x0004          
#define CMU_DISPLAYPLL          0x0010
#define CMU_TVOUTPLL            0x0018
#define CMU_SENSCLK             0x0020
#define CMU_LCDCLK              0x0024
#define CMU_DSICLK              0x0028
#define CMU_DECLK               0x0030
#define CMU_BISPCLK             0x0034
#define CMU_PWM0CLK             0x0070
#define CMU_PWM1CLK             0x0074
#define CMU_PWM2CLK             0x0078
#define CMU_PWM3CLK             0x007C
#define CMU_USBPLL              0x0080
#define CMU_DEVCLKEN0           0x00A0
#define CMU_DEVCLKEN1           0x00A4
#define CMU_DEVRST0             0x00A8
#define CMU_DEVRST1             0x00AC
#define CMU_DEVPLLDEBUG         0x00DC
#define CMU_TVOUTPLL0DEBUG      0x00EC
#define CMU_TVOUTPLL1DEBUG      0x00F0
#define CMU_DCPPLLDEBUG         0x00F4

//--------------Bits Location------------------------------------------//
//--------------TwoHZ_Timer-------------------------------------------//
//--------------Register Address---------------------------------------//
#define TWOHZ_TIMER_BASE        0xB0158000
#define TWOHZ0_CTL              (TWOHZ_TIMER_BASE+0x0000)
#define T0_CTL                  (TWOHZ_TIMER_BASE+0x0008)
#define T0_CMP                  (TWOHZ_TIMER_BASE+0x000C)
#define T0_VAL                  (TWOHZ_TIMER_BASE+0x0010)
#define T1_CTL                  (TWOHZ_TIMER_BASE+0x0014)
#define T1_CMP                  (TWOHZ_TIMER_BASE+0x0018)
#define T1_VAL                  (TWOHZ_TIMER_BASE+0x001C)
#define TWOHZ1_CTL              (TWOHZ_TIMER_BASE+0x0020)
#define USB3_P0_CTL             (TWOHZ_TIMER_BASE+0x0080)
#define USB2_0ECS               (TWOHZ_TIMER_BASE+0x0090)
#define USB2_1ECS               (TWOHZ_TIMER_BASE+0x0094)


#define NAND_BASE
#define NAND_CHK_STATUS         (0xF81C00 + 0x4C)
#define NAND_SET_PADDRV         (0xF81C00 + 0x88)
#define NAND_FLASH_INIYHW       (0xF81C00 + 0x74)

//--------------Bits Location------------------------------------------//
//--------------DMA-------------------------------------------//
//--------------Register Address---------------------------------------//
#define DMA_BASE                      0xb0220000
#define DMA_CTL                       (DMA_BASE+0x0000)
#define DMA_IRQEN                     (DMA_BASE+0x0004)
#define DMA_IRQPD                     (DMA_BASE+0x0008)
#define DMA_PAUSE                     (DMA_BASE+0x000C)

//--------------Bits Location------------------------------------------//
//--------------BDMA0-------------------------------------------//
//--------------Register Address---------------------------------------//
#define BDMA0_BASE                    0xb0220100
#define BDMA0_MODE                    (BDMA0_BASE+0x0000)
#define BDMA0_SRC                     (BDMA0_BASE+0x0004)
#define BDMA0_DST                     (BDMA0_BASE+0x0008)
#define BDMA0_CNT                     (BDMA0_BASE+0x000C)
#define BDMA0_REM                     (BDMA0_BASE+0x0010)
#define BDMA0_CMD                     (BDMA0_BASE+0x0014)
#define BDMA0_CACHE                   (BDMA0_BASE+0x0018)

//--------------Bits Location------------------------------------------//
//--------------BDMA1-------------------------------------------//
//--------------Register Address---------------------------------------//
#define BDMA1_BASE                    0xb0220130
#define BDMA1_MODE                    (BDMA1_BASE+0x0000)
#define BDMA1_SRC                     (BDMA1_BASE+0x0004)
#define BDMA1_DST                     (BDMA1_BASE+0x0008)
#define BDMA1_CNT                     (BDMA1_BASE+0x000C)
#define BDMA1_REM                     (BDMA1_BASE+0x0010)
#define BDMA1_CMD                     (BDMA1_BASE+0x0014)
#define BDMA1_CACHE                   (BDMA1_BASE+0x0018)

//--------------Bits Location------------------------------------------//
//--------------BDMA2-------------------------------------------//
//--------------Register Address---------------------------------------//
#define BDMA2_BASE                    0xb0220160
#define BDMA2_MODE                    (BDMA2_BASE+0x0000)
#define BDMA2_SRC                     (BDMA2_BASE+0x0004)
#define BDMA2_DST                     (BDMA2_BASE+0x0008)
#define BDMA2_CNT                     (BDMA2_BASE+0x000C)
#define BDMA2_REM                     (BDMA2_BASE+0x0010)
#define BDMA2_CMD                     (BDMA2_BASE+0x0014)
#define BDMA2_CACHE                   (BDMA2_BASE+0x0018)

//--------------Bits Location------------------------------------------//
//--------------BDMA3-------------------------------------------//
//--------------Register Address---------------------------------------//
#define BDMA3_BASE                    0xb0220190
#define BDMA3_MODE                    (BDMA3_BASE+0x0000)
#define BDMA3_SRC                     (BDMA3_BASE+0x0004)
#define BDMA3_DST                     (BDMA3_BASE+0x0008)
#define BDMA3_CNT                     (BDMA3_BASE+0x000C)
#define BDMA3_REM                     (BDMA3_BASE+0x0010)
#define BDMA3_CMD                     (BDMA3_BASE+0x0014)
#define BDMA3_CACHE                   (BDMA3_BASE+0x0018)

/* NAND Registres */
#define NAND_BASE                       0xb0320000

//--------------Bits Location------------------------------------------//
//--------------ISP_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define ISP_REGISTER_BASE                0xB0210000
#define ISP_CB_TIME                   (ISP_REGISTER_BASE+0x00)
#define ISP_ENABLE                    (ISP_REGISTER_BASE+0x10)
#define ISP_CTL                       (ISP_REGISTER_BASE+0x20)
#define ISP_CHANNEL_1_STATE                (ISP_REGISTER_BASE+0x24)
#define ISP_CHANNEL_1_BA_OFFSET                                           (ISP_REGISTER_BASE+0x28)
#define ISP_CHANNEL_1_ROW_RANGE                                           (ISP_REGISTER_BASE+0x2C)
#define ISP_CHANNEL_1_COL_RANGE                                           (ISP_REGISTER_BASE+0x30)
#define ISP_CHANNEL_1_ADDR0                (ISP_REGISTER_BASE+0x34)
#define ISP_CHANNEL_1_ADDR1                (ISP_REGISTER_BASE+0x38)
#define ISP_CHANNEL_1_ADDR2                (ISP_REGISTER_BASE+0x3C)
#define ISP_CHANNEL_1_ADDR3                (ISP_REGISTER_BASE+0x40)
#define ISP_CHANNEL_1_ADDR4                (ISP_REGISTER_BASE+0x44)
#define ISP_CHANNEL_1_ADDR5                (ISP_REGISTER_BASE+0x48)
#define ISP_CHANNEL_1_ADDR6                (ISP_REGISTER_BASE+0x4C)
#define ISP_CHANNEL_1_ADDR7                (ISP_REGISTER_BASE+0x50)
#define VNC_LUT_PORT                  (ISP_REGISTER_BASE+0x54)
#define VNC_RST                       (ISP_REGISTER_BASE+0x58)
#define ISP_LSC_LUT_PORT                (ISP_REGISTER_BASE+0x60)
#define ISP_LSC_PIX_INC                 (ISP_REGISTER_BASE+0x64)
#define ISP_LSC_SCALING                 (ISP_REGISTER_BASE+0x68)
#define ISP_LSC_CENTER0                 (ISP_REGISTER_BASE+0x6C)
#define ISP_LSC_CENTER1                 (ISP_REGISTER_BASE+0x70)
#define ISP_LSC_RST                   (ISP_REGISTER_BASE+0x74)
#define ISP_NR_CONTROL                (ISP_REGISTER_BASE+0x80)
#define ISP_DPC_THRESHOLD                (ISP_REGISTER_BASE+0x88)
#define ISP_CG_B_GAIN                 (ISP_REGISTER_BASE+0x90)
#define ISP_CG_G_GAIN                 (ISP_REGISTER_BASE+0x94)
#define ISP_CG_R_GAIN                 (ISP_REGISTER_BASE+0x98)
#define ISP_STAT_REGION_Y                (ISP_REGISTER_BASE+0xA0)
#define ISP_STAT_REGION_X                (ISP_REGISTER_BASE+0xA4)
#define ISP_STAT_HIST_Y                 (ISP_REGISTER_BASE+0xA8)
#define ISP_STAT_HIST_X                 (ISP_REGISTER_BASE+0xAC)
#define ISP_WB_THRESHOLD                (ISP_REGISTER_BASE+0xB0)
#define ISP_CSC_OFFSET1                 (ISP_REGISTER_BASE+0xC0)
#define ISP_CSC_OFFSET2                 (ISP_REGISTER_BASE+0xC4)
#define ISP_CSC_OFFSET3                 (ISP_REGISTER_BASE+0xC8)
#define ISP_CSC_Y_R                   (ISP_REGISTER_BASE+0xCC)
#define ISP_CSC_Y_G                   (ISP_REGISTER_BASE+0xD0)
#define ISP_CSC_Y_B                   (ISP_REGISTER_BASE+0xD4)
#define ISP_CSC_CB_R                  (ISP_REGISTER_BASE+0xD8)
#define ISP_CSC_CB_G                  (ISP_REGISTER_BASE+0xDC)
#define ISP_CSC_CB_B                  (ISP_REGISTER_BASE+0xE0)
#define ISP_CSC_CR_R                  (ISP_REGISTER_BASE+0xE4)
#define ISP_CSC_CR_G                  (ISP_REGISTER_BASE+0xE8)
#define ISP_CSC_CR_B                  (ISP_REGISTER_BASE+0xEC)
#define ISP_CSC_CONTROL                 (ISP_REGISTER_BASE+0xF0)
#define ISP_GC_COEFF_0                (ISP_REGISTER_BASE+0x100)
#define ISP_GC_COEFF_1                (ISP_REGISTER_BASE+0x104)
#define ISP_GC_COEFF_2                (ISP_REGISTER_BASE+0x108)
#define ISP_GC_COEFF_3                (ISP_REGISTER_BASE+0x10C)
#define ISP_GC_COEFF_4                (ISP_REGISTER_BASE+0x110)
#define ISP_GC_COEFF_5                (ISP_REGISTER_BASE+0x114)
#define ISP_GC_COEFF_6                (ISP_REGISTER_BASE+0x118)
#define ISP_GC_COEFF_7                (ISP_REGISTER_BASE+0x11C)
#define ISP_GC_COEFF_8                (ISP_REGISTER_BASE+0x120)
#define ISP_GC_COEFF_9                (ISP_REGISTER_BASE+0x124)
#define ISP_GC_COEFF_10                 (ISP_REGISTER_BASE+0x128)
#define ISP_GC_COEFF_11                 (ISP_REGISTER_BASE+0x12C)
#define ISP_GC_COEFF_12                 (ISP_REGISTER_BASE+0x130)
#define ISP_GC_COEFF_13                 (ISP_REGISTER_BASE+0x134)
#define ISP_GC_COEFF_14                 (ISP_REGISTER_BASE+0x138)
#define ISP_GC_COEFF_15                 (ISP_REGISTER_BASE+0x13C)
#define ISP_OUT_FMT                   (ISP_REGISTER_BASE+0x148)
#define ISP_OUT_ADDRY                 (ISP_REGISTER_BASE+0x14C)
#define ISP_OUT_ADDRU                 (ISP_REGISTER_BASE+0x150)
#define ISP_OUT_ADDRV                 (ISP_REGISTER_BASE+0x154)
#define ISP_OUT_ADDR1UV                 (ISP_REGISTER_BASE+0x158)
#define ISP_STAT_ADDR                 (ISP_REGISTER_BASE+0x160)
#define ISP_COLOR_REPLACE1                 (ISP_REGISTER_BASE+0x170)
#define ISP_COLOR_REPLACE2                 (ISP_REGISTER_BASE+0x174)
#define ISP_COLOR_REPLACE3                 (ISP_REGISTER_BASE+0x178)
#define ISP_COLOR_REPLACE4                 (ISP_REGISTER_BASE+0x17C)
#define ISP_COLOR_REPLACE5                 (ISP_REGISTER_BASE+0x180)
#define ISP_COLOR_REPLACE6                 (ISP_REGISTER_BASE+0x184)
#define ISP_COLOR_REPLACE7                 (ISP_REGISTER_BASE+0x188)
#define ISP_COLOR_REPLACE8                 (ISP_REGISTER_BASE+0x18C)
#define ISP_COLOR_REPLACE9                 (ISP_REGISTER_BASE+0x190)
#define ISP_COLOR_REPLACE10                (ISP_REGISTER_BASE+0x194)
#define ISP_COLOR_REPLACE11                (ISP_REGISTER_BASE+0x198)
#define ISP_COLOR_REPLACE12                (ISP_REGISTER_BASE+0x19C)
#define ISP_COLOR_REPLACE13                (ISP_REGISTER_BASE+0x1A0)
#define ISP_COLOR_REPLACE14                (ISP_REGISTER_BASE+0x1A4)
#define ISP_COLOR_REPLACE15                (ISP_REGISTER_BASE+0x1A8)
#define ISP_COLOR_REPLACE16                (ISP_REGISTER_BASE+0x1AC)
#define ISP2_CTL                      (ISP_REGISTER_BASE+0x1EC)
#define ISP_CHANNEL_2_STATE                (ISP_REGISTER_BASE+0x1F0)
#define ISP_CHANNEL_2_BA_OFFSET                                           (ISP_REGISTER_BASE+0x1F4)
#define ISP_CHANNEL_2_ROW_RANGE                                           (ISP_REGISTER_BASE+0x1F8)
#define ISP_CHANNEL_2_COL_RANGE                                           (ISP_REGISTER_BASE+0x1FC)
#define ISP_CHANNEL_2_ADDR0                (ISP_REGISTER_BASE+0x200)
#define ISP_CHANNEL_2_ADDR1                (ISP_REGISTER_BASE+0x204)
#define ISP_CHANNEL_2_ADDR2                (ISP_REGISTER_BASE+0x208)
#define ISP_CHANNEL_2_ADDR3                (ISP_REGISTER_BASE+0x20C)
#define ISP_CHANNEL_2_ADDR4                (ISP_REGISTER_BASE+0x210)
#define ISP_CHANNEL_2_ADDR5                (ISP_REGISTER_BASE+0x214)
#define ISP_CHANNEL_2_ADDR6                (ISP_REGISTER_BASE+0x218)
#define ISP_CHANNEL_2_ADDR7                (ISP_REGISTER_BASE+0x21C)
#define ISP2_LSC_LUT_PORT                (ISP_REGISTER_BASE+0x22C)
#define ISP2_LSC_PIX_INC                (ISP_REGISTER_BASE+0x230)
#define ISP2_LSC_SCALING                (ISP_REGISTER_BASE+0x234)
#define ISP2_LSC_CENTER0                (ISP_REGISTER_BASE+0x238)
#define ISP2_LSC_CENTER1                (ISP_REGISTER_BASE+0x23C)
#define ISP2_LSC_RST                  (ISP_REGISTER_BASE+0x240)
#define ISP2_NR_CONTROL                 (ISP_REGISTER_BASE+0x24C)
#define ISP2_DPC_THRESHOLD                 (ISP_REGISTER_BASE+0x254)
#define ISP2_CG_B_GAIN                (ISP_REGISTER_BASE+0x25C)
#define ISP2_CG_G_GAIN                (ISP_REGISTER_BASE+0x260)
#define ISP2_CG_R_GAIN                (ISP_REGISTER_BASE+0x264)
#define ISP2_STAT_REGION_Y                 (ISP_REGISTER_BASE+0x26C)
#define ISP2_STAT_REGION_X                 (ISP_REGISTER_BASE+0x270)
#define ISP2_STAT_HIST_Y                (ISP_REGISTER_BASE+0x274)
#define ISP2_STAT_HIST_X                (ISP_REGISTER_BASE+0x278)
#define ISP2_WB_THRESHOLD                (ISP_REGISTER_BASE+0x27C)
#define ISP2_CSC_OFFSET1                (ISP_REGISTER_BASE+0x28C)
#define ISP2_CSC_OFFSET2                (ISP_REGISTER_BASE+0x290)
#define ISP2_CSC_OFFSET3                (ISP_REGISTER_BASE+0x294)
#define ISP2_CSC_Y_R                  (ISP_REGISTER_BASE+0x298)
#define ISP2_CSC_Y_G                  (ISP_REGISTER_BASE+0x29C)
#define ISP2_CSC_Y_B                  (ISP_REGISTER_BASE+0x2A0)
#define ISP2_CSC_CB_R                 (ISP_REGISTER_BASE+0x2A4)
#define ISP2_CSC_CB_G                 (ISP_REGISTER_BASE+0x2A8)
#define ISP2_CSC_CB_B                 (ISP_REGISTER_BASE+0x2AC)
#define ISP2_CSC_CR_R                 (ISP_REGISTER_BASE+0x2B0)
#define ISP2_CSC_CR_G                 (ISP_REGISTER_BASE+0x2B4)
#define ISP2_CSC_CR_B                 (ISP_REGISTER_BASE+0x2B8)
#define ISP2_CSC_CONTROL                (ISP_REGISTER_BASE+0x2BC)
#define ISP2_GC_COEFF_0                 (ISP_REGISTER_BASE+0x2CC)
#define ISP2_GC_COEFF_1                 (ISP_REGISTER_BASE+0x2D0)
#define ISP2_GC_COEFF_2                 (ISP_REGISTER_BASE+0x2D4)
#define ISP2_GC_COEFF_3                 (ISP_REGISTER_BASE+0x2D8)
#define ISP2_GC_COEFF_4                 (ISP_REGISTER_BASE+0x2DC)
#define ISP2_GC_COEFF_5                 (ISP_REGISTER_BASE+0x2E0)
#define ISP2_GC_COEFF_6                 (ISP_REGISTER_BASE+0x2E4)
#define ISP2_GC_COEFF_7                 (ISP_REGISTER_BASE+0x2E8)
#define ISP2_GC_COEFF_8                 (ISP_REGISTER_BASE+0x2EC)
#define ISP2_GC_COEFF_9                 (ISP_REGISTER_BASE+0x2F0)
#define ISP2_GC_COEFF_10                (ISP_REGISTER_BASE+0x2F4)
#define ISP2_GC_COEFF_11                (ISP_REGISTER_BASE+0x2F8)
#define ISP2_GC_COEFF_12                (ISP_REGISTER_BASE+0x2FC)
#define ISP2_GC_COEFF_13                (ISP_REGISTER_BASE+0x300)
#define ISP2_GC_COEFF_14                (ISP_REGISTER_BASE+0x304)
#define ISP2_GC_COEFF_15                (ISP_REGISTER_BASE+0x308)
#define ISP2_OUT_FMT                  (ISP_REGISTER_BASE+0x314)
#define ISP2_OUT_ADDRY                (ISP_REGISTER_BASE+0x318)
#define ISP2_OUT_ADDRU                (ISP_REGISTER_BASE+0x31C)
#define ISP2_OUT_ADDRV                (ISP_REGISTER_BASE+0x320)
#define ISP2_OUT_ADDR1UV                (ISP_REGISTER_BASE+0x324)
#define ISP2_STAT_ADDR                (ISP_REGISTER_BASE+0x32C)
#define ISP2_COLOR_REPLACE1                (ISP_REGISTER_BASE+0x33C)
#define ISP2_COLOR_REPLACE2                (ISP_REGISTER_BASE+0x340)
#define ISP2_COLOR_REPLACE3                (ISP_REGISTER_BASE+0x344)
#define ISP2_COLOR_REPLACE4                (ISP_REGISTER_BASE+0x348)
#define ISP2_COLOR_REPLACE5                (ISP_REGISTER_BASE+0x34C)
#define ISP2_COLOR_REPLACE6                (ISP_REGISTER_BASE+0x350)
#define ISP2_COLOR_REPLACE7                (ISP_REGISTER_BASE+0x354)
#define ISP2_COLOR_REPLACE8                (ISP_REGISTER_BASE+0x358)
#define ISP2_COLOR_REPLACE9                (ISP_REGISTER_BASE+0x35C)
#define ISP2_COLOR_REPLACE10                                              (ISP_REGISTER_BASE+0x360)
#define ISP2_COLOR_REPLACE11                                              (ISP_REGISTER_BASE+0x364)
#define ISP2_COLOR_REPLACE12                                              (ISP_REGISTER_BASE+0x368)
#define ISP2_COLOR_REPLACE13                                              (ISP_REGISTER_BASE+0x36C)
#define ISP2_COLOR_REPLACE14                                              (ISP_REGISTER_BASE+0x370)
#define ISP2_COLOR_REPLACE15                                              (ISP_REGISTER_BASE+0x374)
#define ISP2_COLOR_REPLACE16                                              (ISP_REGISTER_BASE+0x378)
#define ISP_FREEMODE_IMAGE_SIZE                                           (ISP_REGISTER_BASE+0x398)
#define ISP_FREEMODE_SET                (ISP_REGISTER_BASE+0x39C)
#define ISP_FREEMODE_ADDRESS                                              (ISP_REGISTER_BASE+0x3A0)
#define AF_ENABLE                     (ISP_REGISTER_BASE+0x3A4)
#define AF_CTRL                       (ISP_REGISTER_BASE+0x3A8)
#define AF_WP0                        (ISP_REGISTER_BASE+0x3AC)
#define AF_WP1                        (ISP_REGISTER_BASE+0x3B0)
#define AF_WP2                        (ISP_REGISTER_BASE+0x3B4)
#define AF_WP3                        (ISP_REGISTER_BASE+0x3B8)
#define AF_WP4                        (ISP_REGISTER_BASE+0x3BC)
#define AF_WP5                        (ISP_REGISTER_BASE+0x3C0)
#define AF_WP6                        (ISP_REGISTER_BASE+0x3C4)
#define AF_WP7                        (ISP_REGISTER_BASE+0x3C8)
#define AF_WP8                        (ISP_REGISTER_BASE+0x3CC)
#define AF_FV0                        (ISP_REGISTER_BASE+0x3D0)
#define AF_FV1                        (ISP_REGISTER_BASE+0x3D4)
#define AF_FV2                        (ISP_REGISTER_BASE+0x3D8)
#define AF_FV3                        (ISP_REGISTER_BASE+0x3DC)
#define AF_FV4                        (ISP_REGISTER_BASE+0x3E0)
#define AF_FV5                        (ISP_REGISTER_BASE+0x3E4)
#define AF_FV6                        (ISP_REGISTER_BASE+0x3E8)
#define AF_FV7                        (ISP_REGISTER_BASE+0x3EC)
#define AF_FV8                        (ISP_REGISTER_BASE+0x3F0)
#define AF_TEN_THOD                   (ISP_REGISTER_BASE+0x3F4)
#define ISP_INT_STAT                  (ISP_REGISTER_BASE+0x400)

//--------------Bits Location------------------------------------------//
//--------------DE-------------------------------------------//
//--------------Register Address---------------------------------------//
#define DE_BASE                       0XB02F0000
#define DE_INTEN                      (DE_BASE+0x0000)
#define DE_STAT                       (DE_BASE+0x0004)
#define OUTPUT_CON                    (DE_BASE+0x4000)
#define OUTPUT_STAT                   (DE_BASE+0x402c)
#define WB_CON                        (DE_BASE+0x4070)
#define WB_ADDR                       (DE_BASE+0x4074)
#define WB_CNT                        (DE_BASE+0x4078)

//--------------Bits Location------------------------------------------//
//--------------PATH1-------------------------------------------//
//--------------Register Address---------------------------------------//
#define PATH1_BASE                    0XB02F0100
#define PATH1_CTL                     (PATH1_BASE+0x0000)
#define PATH1_FCR                     (PATH1_BASE+0x0004)
#define PATH1_EN                      (PATH1_BASE+0x0008)
#define PATH1_BK                      (PATH1_BASE+0x000C)
#define PATH1_SIZE                    (PATH1_BASE+0x0010)
#define PATH1_A_COOR                  (PATH1_BASE+0x0014)
#define PATH1_B_COOR                  (PATH1_BASE+0x0018)
#define PATH1_ALPHA_CFG                 (PATH1_BASE+0x001C)
#define PATH1_CKMAX                   (PATH1_BASE+0x0020)
#define PATH1_CKMIN                   (PATH1_BASE+0x0024)
#define PATH1_GAMMA1_IDX                (PATH1_BASE+0x0028)
#define PATH1_GAMMA1_RAM                (PATH1_BASE+0x002C)
#define PATH1_GAMMA2_IDX                (PATH1_BASE+0x0030)
#define PATH1_GAMMA2_RAM                (PATH1_BASE+0x0034)

//--------------Bits Location------------------------------------------//
//--------------PATH2-------------------------------------------//
//--------------Register Address---------------------------------------//
#define PATH2_BASE                    0XB02F0140
#define PATH2_CTL                     (PATH2_BASE+0x0000)
#define PATH2_FCR                     (PATH2_BASE+0x0004)
#define PATH2_EN                      (PATH2_BASE+0x0008)
#define PATH2_BK                      (PATH2_BASE+0x000C)
#define PATH2_SIZE                    (PATH2_BASE+0x0010)
#define PATH2_A_COOR                  (PATH2_BASE+0x0014)
#define PATH2_B_COOR                  (PATH2_BASE+0x0018)
#define PATH2_ALPHA_CFG                 (PATH2_BASE+0x001C)
#define PATH2_CKMAX                   (PATH2_BASE+0x0020)
#define PATH2_CKMIN                   (PATH2_BASE+0x0024)
#define PATH2_GAMMA1_IDX                (PATH2_BASE+0x0028)
#define PATH2_GAMMA1_RAM                (PATH2_BASE+0x002C)
#define PATH2_GAMMA2_IDX                (PATH2_BASE+0x0030)
#define PATH2_GAMMA2_RAM                (PATH2_BASE+0x0034)

//--------------Bits Location------------------------------------------//
//--------------GRAPHIC-------------------------------------------//
//--------------Register Address---------------------------------------//
#define GRAPHIC_BASE                  0XB02F0200
#define GRAPHIC_CFG                   (GRAPHIC_BASE+0x0000)
#define GRAPHIC_SIZE                  (GRAPHIC_BASE+0x0004)
#define GRAPHIC_FB                    (GRAPHIC_BASE+0x0008)
#define GRAPHIC_STR                   (GRAPHIC_BASE+0x000C)

//--------------Bits Location------------------------------------------//
//--------------VIDEO1-------------------------------------------//
//--------------Register Address---------------------------------------//
#define VIDEO1_BASE                   0XB02F0220
#define VIDEO1_CFG                    (VIDEO1_BASE+0x0000)
#define VIDEO1_ISIZE                  (VIDEO1_BASE+0x0004)
#define VIDEO1_OSIZE                  (VIDEO1_BASE+0x0008)
#define VIDEO1_SR                     (VIDEO1_BASE+0x000C)
#define VIDEO1_SCOEF0                 (VIDEO1_BASE+0x0010)
#define VIDEO1_SCOEF1                 (VIDEO1_BASE+0x0014)
#define VIDEO1_SCOEF2                 (VIDEO1_BASE+0x0018)
#define VIDEO1_SCOEF3                 (VIDEO1_BASE+0x001C)
#define VIDEO1_SCOEF4                 (VIDEO1_BASE+0x0020)
#define VIDEO1_SCOEF5                 (VIDEO1_BASE+0x0024)
#define VIDEO1_SCOEF6                 (VIDEO1_BASE+0x0028)
#define VIDEO1_SCOEF7                 (VIDEO1_BASE+0x002C)
#define VIDEO1_FB_0                   (VIDEO1_BASE+0x0030)
#define VIDEO1_FB_1                   (VIDEO1_BASE+0x0034)
#define VIDEO1_FB_2                   (VIDEO1_BASE+0x0038)
#define VIDEO1_FB_RIGHT_0                (VIDEO1_BASE+0x003C)
#define VIDEO1_FB_RIGHT_1                (VIDEO1_BASE+0x0040)
#define VIDEO1_FB_RIGHT_2                (VIDEO1_BASE+0x0044)
#define VIDEO1_STR                    (VIDEO1_BASE+0x0048)
#define VIDEO1_CRITICAL                 (VIDEO1_BASE+0x004C)
#define VIDEO1_REMAPPING                (VIDEO1_BASE+0x0050)

//--------------Bits Location------------------------------------------//
//--------------VIDEO2-------------------------------------------//
//--------------Register Address---------------------------------------//
#define VIDEO2_BASE                   0XB02F02A0
#define VIDEO2_CFG                    (VIDEO2_BASE+0x0000)
#define VIDEO2_ISIZE                  (VIDEO2_BASE+0x0004)
#define VIDEO2_OSIZE                  (VIDEO2_BASE+0x0008)
#define VIDEO2_SR                     (VIDEO2_BASE+0x000C)
#define VIDEO2_SCOEF0                 (VIDEO2_BASE+0x0010)
#define VIDEO2_SCOEF1                 (VIDEO2_BASE+0x0014)
#define VIDEO2_SCOEF2                 (VIDEO2_BASE+0x0018)
#define VIDEO2_SCOEF3                 (VIDEO2_BASE+0x001C)
#define VIDEO2_SCOEF4                 (VIDEO2_BASE+0x0020)
#define VIDEO2_SCOEF5                 (VIDEO2_BASE+0x0024)
#define VIDEO2_SCOEF6                 (VIDEO2_BASE+0x0028)
#define VIDEO2_SCOEF7                 (VIDEO2_BASE+0x002C)
#define VIDEO2_FB_0                   (VIDEO2_BASE+0x0030)
#define VIDEO2_FB_1                   (VIDEO2_BASE+0x0034)
#define VIDEO2_FB_2                   (VIDEO2_BASE+0x0038)
#define VIDEO2_FB_RIGHT_0                (VIDEO2_BASE+0x003C)
#define VIDEO2_FB_RIGHT_1                (VIDEO2_BASE+0x0040)
#define VIDEO2_FB_RIGHT_2                (VIDEO2_BASE+0x0044)
#define VIDEO2_STR                    (VIDEO2_BASE+0x0048)
#define VIDEO2_CRITICAL                 (VIDEO2_BASE+0x004C)
#define VIDEO2_REMAPPING                (VIDEO2_BASE+0x0050)

//--------------Bits Location------------------------------------------//
//--------------VIDEO3-------------------------------------------//
//--------------Register Address---------------------------------------//
#define VIDEO3_BASE                   0XB02F0320
#define VIDEO3_CFG                    (VIDEO3_BASE+0x0000)
#define VIDEO3_ISIZE                  (VIDEO3_BASE+0x0004)
#define VIDEO3_OSIZE                  (VIDEO3_BASE+0x0008)
#define VIDEO3_SR                     (VIDEO3_BASE+0x000C)
#define VIDEO3_SCOEF0                 (VIDEO3_BASE+0x0010)
#define VIDEO3_SCOEF1                 (VIDEO3_BASE+0x0014)
#define VIDEO3_SCOEF2                 (VIDEO3_BASE+0x0018)
#define VIDEO3_SCOEF3                 (VIDEO3_BASE+0x001C)
#define VIDEO3_SCOEF4                 (VIDEO3_BASE+0x0020)
#define VIDEO3_SCOEF5                 (VIDEO3_BASE+0x0024)
#define VIDEO3_SCOEF6                 (VIDEO3_BASE+0x0028)
#define VIDEO3_SCOEF7                 (VIDEO3_BASE+0x002C)
#define VIDEO3_FB_0                   (VIDEO3_BASE+0x0030)
#define VIDEO3_FB_1                   (VIDEO3_BASE+0x0034)
#define VIDEO3_FB_2                   (VIDEO3_BASE+0x0038)
#define VIDEO3_FB_RIGHT_0                (VIDEO3_BASE+0x003C)
#define VIDEO3_FB_RIGHT_1                (VIDEO3_BASE+0x0040)
#define VIDEO3_FB_RIGHT_2                (VIDEO3_BASE+0x0044)
#define VIDEO3_STR                    (VIDEO3_BASE+0x0048)
#define VIDEO3_CRITICAL                 (VIDEO3_BASE+0x004C)
#define VIDEO3_REMAPPING                (VIDEO3_BASE+0x0050)

//--------------Bits Location------------------------------------------//
//--------------MDSB-------------------------------------------//
//--------------Register Address---------------------------------------//
#define MDSB_BASE                     0XB02F4000
#define MDSB_CTL                      (MDSB_BASE+0x0000)
#define MDSB_CVBS_HR                  (MDSB_BASE+0x0004)
#define MDSB_CVBS_VR                  (MDSB_BASE+0x0008)
#define MDSB_LCD_HR                   (MDSB_BASE+0x000C)
#define MDSB_LCD_VR                   (MDSB_BASE+0x0010)
#define MDSB_STSEQ                    (MDSB_BASE+0x0014)
#define MDSB_DELAY0                   (MDSB_BASE+0x0018)
#define MDSB_DELAY1                   (MDSB_BASE+0x001C)
#define MDSB_CVBS_OFF                 (MDSB_BASE+0x0020)
#define MDSB_LCD_OFF                  (MDSB_BASE+0x0024)
#define MDSB_HDMIWH                   (MDSB_BASE+0x0028)
#define MDSB_STAT                     (MDSB_BASE+0x002C)
#define MDSB_HSCALER0                 (MDSB_BASE+0X30)
#define MDSB_HSCALER1                 (MDSB_BASE+0X34)
#define MDSB_HSCALER2                 (MDSB_BASE+0X38)
#define MDSB_HSCALER3                 (MDSB_BASE+0X3C)
#define MDSB_HSCALER4                 (MDSB_BASE+0X40)
#define MDSB_HSCALER5                 (MDSB_BASE+0X44)
#define MDSB_HSCALER6                 (MDSB_BASE+0X48)
#define MDSB_HSCALER7                 (MDSB_BASE+0X4C)
#define MDSB_VSCALER0                 (MDSB_BASE+0X50)
#define MDSB_VSCALER1                 (MDSB_BASE+0X54)
#define MDSB_VSCALER2                 (MDSB_BASE+0X58)
#define MDSB_VSCALER3                 (MDSB_BASE+0X5C)
#define MDSB_VSCALER4                 (MDSB_BASE+0X60)
#define MDSB_VSCALER5                 (MDSB_BASE+0X64)
#define MDSB_VSCALER6                 (MDSB_BASE+0X68)
#define MDSB_VSCALER7                 (MDSB_BASE+0X6C)
#define MDSB_WBCFG                    (MDSB_BASE+0x0070)
#define MDSB_WBADDR                   (MDSB_BASE+0x0074)
#define MDSB_WBLEN                    (MDSB_BASE+0x0078)

/* SD0 OWL2X */
#define SD0_OWL2X_BASE                  0xB0240000
#define SD0_EN_OWL2X                    (SD0_OWL2X_BASE+0x0000)
#define SD0_CTL_OWL2X                   (SD0_OWL2X_BASE+0x0004)
#define SD0_STATE_OWL2X                 (SD0_OWL2X_BASE+0x0008)
#define SD0_CMD_OWL2X                   (SD0_OWL2X_BASE+0x000C)
#define SD0_ARG_OWL2X                   (SD0_OWL2X_BASE+0x0010)
#define SD0_RSPBUF0_OWL2X               (SD0_OWL2X_BASE+0x0014)
#define SD0_RSPBUF1_OWL2X               (SD0_OWL2X_BASE+0x0018)
#define SD0_RSPBUF2_OWL2X               (SD0_OWL2X_BASE+0x001C)
#define SD0_RSPBUF3_OWL2X               (SD0_OWL2X_BASE+0x0020)
#define SD0_RSPBUF4_OWL2X               (SD0_OWL2X_BASE+0x0024)
#define SD0_DAT_OWL2X                   (SD0_OWL2X_BASE+0x0028)
#define SD0_BLK_SIZE_OWL2X              (SD0_OWL2X_BASE+0x002C)
#define SD0_BLK_NUM_OWL2X               (SD0_OWL2X_BASE+0x0030)
#define SD0_DMA_ADDR_OWL2X              (SD0_OWL2X_BASE+0x0034)
#define SD0_DMA_CTL_OWL2X               (SD0_OWL2X_BASE+0x0038)
/* SD1 OWL2X */
#define SD1_OWL2X_BASE                  0xB0244000
#define SD1_EN_OWL2X                    (SD1_OWL2X_BASE+0x0000)
#define SD1_CTL_OWL2X                   (SD1_OWL2X_BASE+0x0004)
#define SD1_STATE_OWL2X                 (SD1_OWL2X_BASE+0x0008)
#define SD1_CMD_OWL2X                   (SD1_OWL2X_BASE+0x000C)
#define SD1_ARG_OWL2X                   (SD1_OWL2X_BASE+0x0010)
#define SD1_RSPBUF0_OWL2X               (SD1_OWL2X_BASE+0x0014)
#define SD1_RSPBUF1_OWL2X               (SD1_OWL2X_BASE+0x0018)
#define SD1_RSPBUF2_OWL2X               (SD1_OWL2X_BASE+0x001C)
#define SD1_RSPBUF3_OWL2X               (SD1_OWL2X_BASE+0x0020)
#define SD1_RSPBUF4_OWL2X               (SD1_OWL2X_BASE+0x0024)
#define SD1_DAT_OWL2X                   (SD1_OWL2X_BASE+0x0028)
#define SD1_BLK_SIZE_OWL2X              (SD1_OWL2X_BASE+0x002C)
#define SD1_BLK_NUM_OWL2X               (SD1_OWL2X_BASE+0x0030)
#define SD1_DMA_ADDR_OWL2X              (SD1_OWL2X_BASE+0x0034)
#define SD1_DMA_CTL_OWL2X               (SD1_OWL2X_BASE+0x0038)
/* SD2 OWL2X */
#define SD2_OWL2X_BASE                  0xB0248000
#define SD2_EN_OWL2X                    (SD2_OWL2X_BASE+0x0000)
#define SD2_CTL_OWL2X                   (SD2_OWL2X_BASE+0x0004)
#define SD2_STATE_OWL2X                 (SD2_OWL2X_BASE+0x0008)
#define SD2_CMD_OWL2X                   (SD2_OWL2X_BASE+0x000C)
#define SD2_ARG_OWL2X                   (SD2_OWL2X_BASE+0x0010)
#define SD2_RSPBUF0_OWL2X               (SD2_OWL2X_BASE+0x0014)
#define SD2_RSPBUF1_OWL2X               (SD2_OWL2X_BASE+0x0018)
#define SD2_RSPBUF2_OWL2X               (SD2_OWL2X_BASE+0x001C)
#define SD2_RSPBUF3_OWL2X               (SD2_OWL2X_BASE+0x0020)
#define SD2_RSPBUF4_OWL2X               (SD2_OWL2X_BASE+0x0024)
#define SD2_DAT_OWL2X                   (SD2_OWL2X_BASE+0x0028)
#define SD2_BLK_SIZE_OWL2X              (SD2_OWL2X_BASE+0x002C)
#define SD2_BLK_NUM_OWL2X               (SD2_OWL2X_BASE+0x0030)
#define SD2_DMA_ADDR_OWL2X              (SD2_OWL2X_BASE+0x0034)
#define SD2_DMA_CTL_OWL2X               (SD2_OWL2X_BASE+0x0038)

/* SD0 OWL3X */
#define SD0_OWL3X_BASE                  0xB0340000
#define SD0_EN_OWL3X                    (SD0_OWL3X_BASE+0x0000)
#define SD0_CTL_OWL3X                   (SD0_OWL3X_BASE+0x0004)
#define SD0_STATE_OWL3X                 (SD0_OWL3X_BASE+0x0008)
#define SD0_CMD_OWL3X                   (SD0_OWL3X_BASE+0x000C)
#define SD0_ARG_OWL3X                   (SD0_OWL3X_BASE+0x0010)
#define SD0_RSPBUF0_OWL3X               (SD0_OWL3X_BASE+0x0014)
#define SD0_RSPBUF1_OWL3X               (SD0_OWL3X_BASE+0x0018)
#define SD0_RSPBUF2_OWL3X               (SD0_OWL3X_BASE+0x001C)
#define SD0_RSPBUF3_OWL3X               (SD0_OWL3X_BASE+0x0020)
#define SD0_RSPBUF4_OWL3X               (SD0_OWL3X_BASE+0x0024)
#define SD0_DAT_OWL3X                   (SD0_OWL3X_BASE+0x0028)
#define SD0_BLK_SIZE_OWL3X              (SD0_OWL3X_BASE+0x002C)
#define SD0_BLK_NUM_OWL3X               (SD0_OWL3X_BASE+0x0030)
#define SD0_DMA_ADDR_OWL3X              (SD0_OWL3X_BASE+0x0034)
#define SD0_DMA_CTL_OWL3X               (SD0_OWL3X_BASE+0x0038)
/* SD1 OWL3X */
#define SD1_OWL3X_BASE                  0xB0344000
#define SD1_EN_OWL3X                    (SD1_OWL3X_BASE+0x0000)
#define SD1_CTL_OWL3X                   (SD1_OWL3X_BASE+0x0004)
#define SD1_STATE_OWL3X                 (SD1_OWL3X_BASE+0x0008)
#define SD1_CMD_OWL3X                   (SD1_OWL3X_BASE+0x000C)
#define SD1_ARG_OWL3X                   (SD1_OWL3X_BASE+0x0010)
#define SD1_RSPBUF0_OWL3X               (SD1_OWL3X_BASE+0x0014)
#define SD1_RSPBUF1_OWL3X               (SD1_OWL3X_BASE+0x0018)
#define SD1_RSPBUF2_OWL3X               (SD1_OWL3X_BASE+0x001C)
#define SD1_RSPBUF3_OWL3X               (SD1_OWL3X_BASE+0x0020)
#define SD1_RSPBUF4_OWL3X               (SD1_OWL3X_BASE+0x0024)
#define SD1_DAT_OWL3X                   (SD1_OWL3X_BASE+0x0028)
#define SD1_BLK_SIZE_OWL3X              (SD1_OWL3X_BASE+0x002C)
#define SD1_BLK_NUM_OWL3X               (SD1_OWL3X_BASE+0x0030)
#define SD1_DMA_ADDR_OWL3X              (SD1_OWL3X_BASE+0x0034)
#define SD1_DMA_CTL_OWL3X               (SD1_OWL3X_BASE+0x0038)
/* SD3 OWL3X */
#define SD2_OWL3X_BASE                  0xB0348000
#define SD2_EN_OWL3X                    (SD2_OWL3X_BASE+0x0000)
#define SD2_CTL_OWL3X                   (SD2_OWL3X_BASE+0x0004)
#define SD2_STATE_OWL3X                 (SD2_OWL3X_BASE+0x0008)
#define SD2_CMD_OWL3X                   (SD2_OWL3X_BASE+0x000C)
#define SD2_ARG_OWL3X                   (SD2_OWL3X_BASE+0x0010)
#define SD2_RSPBUF0_OWL3X               (SD2_OWL3X_BASE+0x0014)
#define SD2_RSPBUF1_OWL3X               (SD2_OWL3X_BASE+0x0018)
#define SD2_RSPBUF2_OWL3X               (SD2_OWL3X_BASE+0x001C)
#define SD2_RSPBUF3_OWL3X               (SD2_OWL3X_BASE+0x0020)
#define SD2_RSPBUF4_OWL3X               (SD2_OWL3X_BASE+0x0024)
#define SD2_DAT_OWL3X                   (SD2_OWL3X_BASE+0x0028)
#define SD2_BLK_SIZE_OWL3X              (SD2_OWL3X_BASE+0x002C)
#define SD2_BLK_NUM_OWL3X               (SD2_OWL3X_BASE+0x0030)
#define SD2_DMA_ADDR_OWL3X              (SD2_OWL3X_BASE+0x0034)
#define SD2_DMA_CTL_OWL3X               (SD2_OWL3X_BASE+0x0038)

/* SHARESRAM */
#define SHARESRAM_BASE                  0xB0200004
#define SHARESRAM_CTL                   (SHARESRAM_BASE+0x00)

//--------------Bits Location------------------------------------------//
//--------------USB3_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define USB3_REGISTER_BASE              0xB0400000
#define USB3_CAPLENGTH                  (USB3_REGISTER_BASE+0x0000)
#define USB3_HCSPARAMS1                 (USB3_REGISTER_BASE+0x0004)
#define USB3_HCSPARAMS2                 (USB3_REGISTER_BASE+0x0008)
#define USB3_HCSPARAMS3                 (USB3_REGISTER_BASE+0x000C)
#define USB3_HCCPARAMS                  (USB3_REGISTER_BASE+0x0010)
#define USB3_DBOFF                      (USB3_REGISTER_BASE+0x0014)
#define USB3_RTSOFF                     (USB3_REGISTER_BASE+0x0018)
#define USB3_USBCMD                     (USB3_REGISTER_BASE+0x0020)
#define USB3_USBSTS                     (USB3_REGISTER_BASE+0x0024)
#define USB3_PAGESIZE                   (USB3_REGISTER_BASE+0x0028)
#define USB3_DNCTRL                     (USB3_REGISTER_BASE+0x0034)
#define USB3_CRCR_L                     (USB3_REGISTER_BASE+0x0038)
#define USB3_CRCR_H                     (USB3_REGISTER_BASE+0x003C)
#define USB3_DCBAAP_L                   (USB3_REGISTER_BASE+0x0050)
#define USB3_DCBAAP_H                   (USB3_REGISTER_BASE+0x0054)
#define USB3_CONFIG                     (USB3_REGISTER_BASE+0x0058)
#define USB3_PORT0_PORTSC               (USB3_REGISTER_BASE+0x0420)
#define USB3_PORT0_PORTPMSC             (USB3_REGISTER_BASE+0x0424)
#define USB3_PORT0_PORTLI               (USB3_REGISTER_BASE+0x0428)
#define USB3_PORT0_PORTHLPMC            (USB3_REGISTER_BASE+0x042c)
#define USB3_PORT1_PORTSC               (USB3_REGISTER_BASE+0x0430)
#define USB3_PORT1_PORTPMSC             (USB3_REGISTER_BASE+0x0434)
#define USB3_PORT1_PORTLI               (USB3_REGISTER_BASE+0x0438)
#define USB3_PORT1_PORTHLPMC            (USB3_REGISTER_BASE+0x043c)
#define USB3_MFINDEX                    (USB3_REGISTER_BASE+0x0440)
#define USB3_IMAN                       (USB3_REGISTER_BASE+0x0460)
#define USB3_IMOD                       (USB3_REGISTER_BASE+0x0464)
#define USB3_ERSTSZ                     (USB3_REGISTER_BASE+0x0468)
#define USB3_ERSTBA_L                   (USB3_REGISTER_BASE+0x0470)
#define USB3_ERSTBA_H                   (USB3_REGISTER_BASE+0x0474)
#define USB3_ERDP_L                     (USB3_REGISTER_BASE+0x0478)
#define USB3_ERDP_H                     (USB3_REGISTER_BASE+0x047c)
#define USB3_DB_HOST                    (USB3_REGISTER_BASE+0x0480)
#define USB3_DB_DEVICE1                 (USB3_REGISTER_BASE+0x0484)
#define USB3_DB_DEVICE2                 (USB3_REGISTER_BASE+0x0488)
#define USB3_DB_DEVICE3                 (USB3_REGISTER_BASE+0x048c)
#define USB3_DB_DEVICE4                 (USB3_REGISTER_BASE+0x0490)
#define USB3_DB_DEVICE5                 (USB3_REGISTER_BASE+0x0494)
#define USB3_DB_DEVICE6                 (USB3_REGISTER_BASE+0x0498)
#define USB3_DB_DEVICE7                 (USB3_REGISTER_BASE+0x049c)
#define USB3_DB_DEVICE8                 (USB3_REGISTER_BASE+0x04a0)
#define USB3_DB_DEVICE9                 (USB3_REGISTER_BASE+0x04a4)
#define USB3_DB_DEVICE10                (USB3_REGISTER_BASE+0x04a8)
#define USB3_DB_DEVICE11                (USB3_REGISTER_BASE+0x04ac)
#define USB3_DB_DEVICE12                (USB3_REGISTER_BASE+0x04b0)
#define USB3_DB_DEVICE13                (USB3_REGISTER_BASE+0x04b4)
#define USB3_DB_DEVICE14                (USB3_REGISTER_BASE+0x04b8)
#define USB3_DB_DEVICE15                (USB3_REGISTER_BASE+0x04bc)
#define USB3_DB_DEVICE16                (USB3_REGISTER_BASE+0x04c0)
#define USB3_DB_DEVICE17                (USB3_REGISTER_BASE+0x04c4)
#define USB3_DB_DEVICE18                (USB3_REGISTER_BASE+0x04c8)
#define USB3_DB_DEVICE19                (USB3_REGISTER_BASE+0x04cc)
#define USB3_DB_DEVICE20                (USB3_REGISTER_BASE+0x04d0)
#define USB3_DB_DEVICE21                (USB3_REGISTER_BASE+0x04d4)
#define USB3_DB_DEVICE22                (USB3_REGISTER_BASE+0x04d8)
#define USB3_DB_DEVICE23                (USB3_REGISTER_BASE+0x04dc)
#define USB3_DB_DEVICE24                (USB3_REGISTER_BASE+0x04e0)
#define USB3_DB_DEVICE25                (USB3_REGISTER_BASE+0x04e4)
#define USB3_DB_DEVICE26                (USB3_REGISTER_BASE+0x04e8)
#define USB3_DB_DEVICE27                (USB3_REGISTER_BASE+0x04ec)
#define USB3_DB_DEVICE28                (USB3_REGISTER_BASE+0x04f0)
#define USB3_DB_DEVICE29                (USB3_REGISTER_BASE+0x04f4)
#define USB3_DB_DEVICE30                (USB3_REGISTER_BASE+0x04f8)
#define USB3_DB_DEVICE31                (USB3_REGISTER_BASE+0x04fc)
#define USB3_USBLEGSUP                  (USB3_REGISTER_BASE+0x0880)
#define USB3_USBLEGCTLSTS               (USB3_REGISTER_BASE+0x0884)
#define USB3_SUPTPRT2_DW0               (USB3_REGISTER_BASE+0x0890)
#define USB3_SUPTPRT2_DW1               (USB3_REGISTER_BASE+0x0894)
#define USB3_SUPTPRT2_DW2               (USB3_REGISTER_BASE+0x0898)
#define USB3_SUPTPRT2_DW3               (USB3_REGISTER_BASE+0x089c)
#define USB3_SUPTPRT3_DW0               (USB3_REGISTER_BASE+0x08a0)
#define USB3_SUPTPRT3_DW1               (USB3_REGISTER_BASE+0x08a4)
#define USB3_SUPTPRT3_DW2               (USB3_REGISTER_BASE+0x08a8)
#define USB3_SUPTPRT3_DW3               (USB3_REGISTER_BASE+0x08ac)
#define USB3_DCID                       (USB3_REGISTER_BASE+0x08b0)
#define USB3_DCDB                       (USB3_REGISTER_BASE+0x08b4)
#define USB3_DCERSTSZ                   (USB3_REGISTER_BASE+0x08b8)
#define USB3_DCERSTBA_LO                (USB3_REGISTER_BASE+0x08c0)
#define USB3_DCERSTBA_HI                (USB3_REGISTER_BASE+0x08c4)
#define USB3_DCERDP_LO                (USB3_REGISTER_BASE+0x08c8)
#define USB3_DCERDP_HI                (USB3_REGISTER_BASE+0x08cc)
#define USB3_DCCTRL                   (USB3_REGISTER_BASE+0x08d0)
#define USB3_DCSTAT                   (USB3_REGISTER_BASE+0x08d4)
#define USB3_DCPORTSC                 (USB3_REGISTER_BASE+0x08d8)
#define USB3_DCECP_LO                 (USB3_REGISTER_BASE+0x08e0)
#define USB3_DCECP_HI                 (USB3_REGISTER_BASE+0x08e4)
#define USB3_DCDDI1                   (USB3_REGISTER_BASE+0x08e8)
#define USB3_DCDDI2                   (USB3_REGISTER_BASE+0x08ec)
#define USB3_GSBUSCFG0                (USB3_REGISTER_BASE+0xC100)
#define USB3_GSBUSCFG1                (USB3_REGISTER_BASE+0xC104)
#define USB3_GTXTHRCFG                (USB3_REGISTER_BASE+0xC108)
#define USB3_GRXTHRCFG                (USB3_REGISTER_BASE+0xC10C)
#define USB3_GCTL                     (USB3_REGISTER_BASE+0xC110)
#define USB3_GSTS                     (USB3_REGISTER_BASE+0xC118)
#define USB3_GSNPSID                  (USB3_REGISTER_BASE+0xC120)
#define USB3_GGPIO                    (USB3_REGISTER_BASE+0xC124)
#define USB3_GUID                     (USB3_REGISTER_BASE+0xC128)
#define USB3_GUCTL                    (USB3_REGISTER_BASE+0xC12C)
#define USB3_GBUSERRADDRLO                 (USB3_REGISTER_BASE+0xC130)
#define USB3_GBUSERRADDRHI                 (USB3_REGISTER_BASE+0xC134)
#define USB3_GPRTBIMAPL                 (USB3_REGISTER_BASE+0xC138)
#define USB3_GPRTBIMAPH                 (USB3_REGISTER_BASE+0xC13C)
#define USB3_GHWPARAMS0                 (USB3_REGISTER_BASE+0xC140)
#define USB3_GHWPARAMS1                 (USB3_REGISTER_BASE+0xC144)
#define USB3_GHWPARAMS2                 (USB3_REGISTER_BASE+0xC148)
#define USB3_GHWPARAMS3                 (USB3_REGISTER_BASE+0xC14C)
#define USB3_GHWPARAMS4                 (USB3_REGISTER_BASE+0xC150)
#define USB3_GHWPARAMS5                 (USB3_REGISTER_BASE+0xC154)
#define USB3_GHWPARAMS6                 (USB3_REGISTER_BASE+0xC158)
#define USB3_GHWPARAMS7                 (USB3_REGISTER_BASE+0xC15C)
#define USB3_GBDGFIFOSPACE                 (USB3_REGISTER_BASE+0xC160)
#define USB3_GDBGLTSSM                (USB3_REGISTER_BASE+0xC164)
#define USB3_GDBGLNMCC                (USB3_REGISTER_BASE+0xC168)
#define USB3_GDBGBMU                  (USB3_REGISTER_BASE+0xC16C)
#define USB3_GDBGLSPMUX                 (USB3_REGISTER_BASE+0xC170)
#define USB3_GDBGLSP                  (USB3_REGISTER_BASE+0xC174)
#define USB3_GDBGEPINFO0                (USB3_REGISTER_BASE+0xC178)
#define USB3_GDBGEPINFO1                (USB3_REGISTER_BASE+0xC17C)
#define USB3_GPRTBIMAPLO_HS                (USB3_REGISTER_BASE+0xC180)
#define USB3_GPRTBIMAPHI_HS                (USB3_REGISTER_BASE+0xC184)
#define USB3_GPRTBIMAPLO_FS                (USB3_REGISTER_BASE+0xC188)
#define USB3_GPRTBIMAPHI_FS                (USB3_REGISTER_BASE+0xC18C)
#define USB3_GUSB2CFG                 (USB3_REGISTER_BASE+0xC200)
#define USB3_GUSB2I2CCTL                (USB3_REGISTER_BASE+0xC240)
#define USB3_GUSB2PHYACC                (USB3_REGISTER_BASE+0xC280)
#define USB3_GUSB3PIPECTL                (USB3_REGISTER_BASE+0xC2C0)
#define USB3_GTXFIFOSIZ0                (USB3_REGISTER_BASE+0xC300)
#define USB3_GTXFIFOSIZ1                (USB3_REGISTER_BASE+0xC304)
#define USB3_GTXFIFOSIZ2                (USB3_REGISTER_BASE+0xC308)
#define USB3_GTXFIFOSIZ3                (USB3_REGISTER_BASE+0xC30C)
#define USB3_GTXFIFOSIZ4                (USB3_REGISTER_BASE+0xC310)
#define USB3_GRXFIFOSIZ0                (USB3_REGISTER_BASE+0xC380)
#define USB3_GRXFIFOSIZ1                (USB3_REGISTER_BASE+0xC384)
#define USB3_GRXFIFOSIZ2                (USB3_REGISTER_BASE+0xC388)
#define USB3_GEVETADR0                (USB3_REGISTER_BASE+0xC400)
#define USB3_GEVNTSIZ0                (USB3_REGISTER_BASE+0xC408)
#define USB3_GEVNTCOUNT0                (USB3_REGISTER_BASE+0xC40C)
#define USB3_DW3_DCFG                 (USB3_REGISTER_BASE+0xC700)
#define USB3_DCTL                     (USB3_REGISTER_BASE+0xC704)
#define USB3_DEVTEN                   (USB3_REGISTER_BASE+0xC708)
#define USB3_DSTS                     (USB3_REGISTER_BASE+0xC70C)
#define USB3_DGCMDPAR                 (USB3_REGISTER_BASE+0xC710)
#define USB3_DGCMD                    (USB3_REGISTER_BASE+0xC714)
#define USB3_DALEPENA                 (USB3_REGISTER_BASE+0xC720)
#define USB3_DEPCMDPAR2_0                (USB3_REGISTER_BASE+0xC800)
#define USB3_DEPCMDPAR1_0                (USB3_REGISTER_BASE+0xC804)
#define USB3_DEPCMDPAR0_0                (USB3_REGISTER_BASE+0xC808)
#define USB3_DEPCMD_0                 (USB3_REGISTER_BASE+0xC80C)
#define USB3_DEPCMDPAR2_1                (USB3_REGISTER_BASE+0xC810)
#define USB3_DEPCMDPAR1_1                (USB3_REGISTER_BASE+0xC814)
#define USB3_DEPCMDPAR0_1                (USB3_REGISTER_BASE+0xC818)
#define USB3_DEPCMD_1                 (USB3_REGISTER_BASE+0xC81C)
#define USB3_DEPCMDPAR2_2                (USB3_REGISTER_BASE+0xC820)
#define USB3_DEPCMDPAR1_2                (USB3_REGISTER_BASE+0xC824)
#define USB3_DEPCMDPAR0_2                (USB3_REGISTER_BASE+0xC828)
#define USB3_DEPCMD_2                 (USB3_REGISTER_BASE+0xC82C)
#define USB3_DEPCMDPAR2_3                (USB3_REGISTER_BASE+0xC830)
#define USB3_DEPCMDPAR1_3                (USB3_REGISTER_BASE+0xC834)
#define USB3_DEPCMDPAR0_3                (USB3_REGISTER_BASE+0xC838)
#define USB3_DEPCMD_3                 (USB3_REGISTER_BASE+0xC83C)
#define USB3_DEPCMDPAR2_4                (USB3_REGISTER_BASE+0xC840)
#define USB3_DEPCMDPAR1_4                (USB3_REGISTER_BASE+0xC844)
#define USB3_DEPCMDPAR0_4                (USB3_REGISTER_BASE+0xC848)
#define USB3_DEPCMD_4                 (USB3_REGISTER_BASE+0xC84C)
#define USB3_DEPCMDPAR2_5                (USB3_REGISTER_BASE+0xC850)
#define USB3_DEPCMDPAR1_5                (USB3_REGISTER_BASE+0xC854)
#define USB3_DEPCMDPAR0_5                (USB3_REGISTER_BASE+0xC858)
#define USB3_DEPCMD_5                 (USB3_REGISTER_BASE+0xC85C)
#define USB3_DEPCMDPAR2_6                (USB3_REGISTER_BASE+0xC860)
#define USB3_DEPCMDPAR1_6                (USB3_REGISTER_BASE+0xC864)
#define USB3_DEPCMDPAR0_6                (USB3_REGISTER_BASE+0xC868)
#define USB3_DEPCMD_6                 (USB3_REGISTER_BASE+0xC86C)
#define USB3_DEPCMDPAR2_7                (USB3_REGISTER_BASE+0xC870)
#define USB3_DEPCMDPAR1_7                (USB3_REGISTER_BASE+0xC874)
#define USB3_DEPCMDPAR0_7                (USB3_REGISTER_BASE+0xC878)
#define USB3_DEPCMD_7                 (USB3_REGISTER_BASE+0xC87C)
#define USB3_DEPCMDPAR2_8                (USB3_REGISTER_BASE+0xC880)
#define USB3_DEPCMDPAR1_8                (USB3_REGISTER_BASE+0xC884)
#define USB3_DEPCMDPAR0_8                (USB3_REGISTER_BASE+0xC888)
#define USB3_DEPCMD_8                 (USB3_REGISTER_BASE+0xC88C)
#define USB3_DEPCMDPAR2_9                (USB3_REGISTER_BASE+0xC890)
#define USB3_DEPCMDPAR1_9                (USB3_REGISTER_BASE+0xC894)
#define USB3_DEPCMDPAR0_9                (USB3_REGISTER_BASE+0xC898)
#define USB3_DEPCMD_9                 (USB3_REGISTER_BASE+0xC89C)
#define USB3_OCFG                     (USB3_REGISTER_BASE+0xCC00)
#define USB3_OCTL                     (USB3_REGISTER_BASE+0xCC04)
#define USB3_OEVEN                    (USB3_REGISTER_BASE+0xCC08)
#define USB3_OSTS                     (USB3_REGISTER_BASE+0xCC0C)
#define USB3_CDR_KIKD                 (USB3_REGISTER_BASE+0xCD00)
#define USB3_CDR_KP1                  (USB3_REGISTER_BASE+0xCD04)
#define USB3_TIMER_INIT                 (USB3_REGISTER_BASE+0xCD08)
#define USB3_CDR_CONTROL                (USB3_REGISTER_BASE+0xCD0C)
#define USB3_RX_OFFSET_PS                (USB3_REGISTER_BASE+0xCD10)
#define USB3_EQ_CONTROL                 (USB3_REGISTER_BASE+0xCD14)
#define USB3_RX_OOBS_SSC0                (USB3_REGISTER_BASE+0xCD18)
#define USB3_CMU_SSC1                 (USB3_REGISTER_BASE+0xCD1C)
#define USB3_CMU_DEBUG_LDO                 (USB3_REGISTER_BASE+0xCD20)
#define USB3_TX_AMP_DEBUG                (USB3_REGISTER_BASE+0xCD24)
#define USB3_Z0                       (USB3_REGISTER_BASE+0xCD28)
#define USB3_DMR_BACR                 (USB3_REGISTER_BASE+0xCD2C)
#define USB3_IER_BCSR                 (USB3_REGISTER_BASE+0xCD30)
#define USB3_BPR                      (USB3_REGISTER_BASE+0xCD34)
#define USB3_BFNR                     (USB3_REGISTER_BASE+0xCD38)
#define USB3_BENR_REV                 (USB3_REGISTER_BASE+0xCD3C)
#define USB3_FLD                      (USB3_REGISTER_BASE+0xCD40)
#define USB3_CMU_PLL2_BISTDEBUG         (USB3_REGISTER_BASE+0xCD44)
#define USB3_USB2_P0_VDCTRL                (USB3_REGISTER_BASE+0xCD48)
#define USB3_BACKDOOR                 (USB3_REGISTER_BASE+0xCD4C)
#define USB3_EXT_CTL                  (USB3_REGISTER_BASE+0xCD50)
#define USB3_EFUSE_CTR                (USB3_REGISTER_BASE+0xCD54)

//--------------Bits Location------------------------------------------//
//--------------SPI0-------------------------------------------//
//--------------Register Address---------------------------------------//
#define SPI0_BASE                     0xB0250000
#define SPI0_CTL                      (SPI0_BASE+0x0000)
#define SPI0_CLKDIV                   (SPI0_BASE+0x0004)
#define SPI0_STAT                     (SPI0_BASE+0x0008)
#define SPI0_RXDAT                    (SPI0_BASE+0x000c)
#define SPI0_TXDAT                    (SPI0_BASE+0x0010)
#define SPI0_TCNT                     (SPI0_BASE+0x0014)
#define SPI0_SEED                     (SPI0_BASE+0x0018)
#define SPI0_TXCR                     (SPI0_BASE+0x001c)
#define SPI0_RXCR                     (SPI0_BASE+0x0020)

//--------------Bits Location------------------------------------------//
//--------------SPI1-------------------------------------------//
//--------------Register Address---------------------------------------//
#define SPI1_BASE                     0xB0254000
#define SPI1_CTL                      (SPI1_BASE+0x0000)
#define SPI1_CLKDIV                   (SPI1_BASE+0x0004)
#define SPI1_STAT                     (SPI1_BASE+0x0008)
#define SPI1_RXDAT                    (SPI1_BASE+0x000c)
#define SPI1_TXDAT                    (SPI1_BASE+0x0010)
#define SPI1_TCNT                     (SPI1_BASE+0x0014)
#define SPI1_SEED                     (SPI1_BASE+0x0018)
#define SPI1_TXCR                     (SPI1_BASE+0x001c)
#define SPI1_RXCR                     (SPI1_BASE+0x0020)

//--------------Bits Location------------------------------------------//
//--------------SPI2-------------------------------------------//
//--------------Register Address---------------------------------------//
#define SPI2_BASE                     0xB0258000
#define SPI2_CTL                      (SPI2_BASE+0x0000)
#define SPI2_CLKDIV                   (SPI2_BASE+0x0004)
#define SPI2_STAT                     (SPI2_BASE+0x0008)
#define SPI2_RXDAT                    (SPI2_BASE+0x000c)
#define SPI2_TXDAT                    (SPI2_BASE+0x0010)
#define SPI2_TCNT                     (SPI2_BASE+0x0014)
#define SPI2_SEED                     (SPI2_BASE+0x0018)
#define SPI2_TXCR                     (SPI2_BASE+0x001c)
#define SPI2_RXCR                     (SPI2_BASE+0x0020)

//--------------Bits Location------------------------------------------//
//--------------SPI3-------------------------------------------//
//--------------Register Address---------------------------------------//
#define SPI3_BASE                     0xB025C000
#define SPI3_CTL                      (SPI3_BASE+0x0000)
#define SPI3_CLKDIV                   (SPI3_BASE+0x0004)
#define SPI3_STAT                     (SPI3_BASE+0x0008)
#define SPI3_RXDAT                    (SPI3_BASE+0x000c)
#define SPI3_TXDAT                    (SPI3_BASE+0x0010)
#define SPI3_TCNT                     (SPI3_BASE+0x0014)
#define SPI3_SEED                     (SPI3_BASE+0x0018)
#define SPI3_TXCR                     (SPI3_BASE+0x001c)
#define SPI3_RXCR                     (SPI3_BASE+0x0020)

//--------------Bits Location------------------------------------------//
//--------------UART0-------------------------------------------//
//--------------Register Address---------------------------------------//
#define UART0_BASE                    0xB0160000
#define UART0_CTL                     (UART0_BASE+0x0000)
#define UART0_RXDAT                   (UART0_BASE+0x0004)
#define UART0_TXDAT                   (UART0_BASE+0x0008)
#define UART0_STAT                    (UART0_BASE+0x000c)

//--------------Bits Location------------------------------------------//
//--------------UART1-------------------------------------------//
//--------------Register Address---------------------------------------//
#define UART1_BASE                    0xB0162000
#define UART1_CTL                     (UART1_BASE+0x0000)
#define UART1_RXDAT                   (UART1_BASE+0x0004)
#define UART1_TXDAT                   (UART1_BASE+0x0008)
#define UART1_STAT                    (UART1_BASE+0x000c)

//--------------Bits Location------------------------------------------//
//--------------UART2-------------------------------------------//
//--------------Register Address---------------------------------------//
#define UART2_BASE                    0xB0164000
#define UART2_CTL                     (UART2_BASE+0x0000)
#define UART2_RXDAT                   (UART2_BASE+0x0004)
#define UART2_TXDAT                   (UART2_BASE+0x0008)
#define UART2_STAT                    (UART2_BASE+0x000c)

//--------------Bits Location------------------------------------------//
//--------------UART3-------------------------------------------//
//--------------Register Address---------------------------------------//
#define UART3_BASE                    0xB0166000
#define UART3_CTL                     (UART3_BASE+0x0000)
#define UART3_RXDAT                   (UART3_BASE+0x0004)
#define UART3_TXDAT                   (UART3_BASE+0x0008)
#define UART3_STAT                    (UART3_BASE+0x000c)

//--------------Bits Location------------------------------------------//
//--------------UART4-------------------------------------------//
//--------------Register Address---------------------------------------//
#define UART4_BASE                    0xB0168000
#define UART4_CTL                     (UART4_BASE+0x0000)
#define UART4_RXDAT                   (UART4_BASE+0x0004)
#define UART4_TXDAT                   (UART4_BASE+0x0008)
#define UART4_STAT                    (UART4_BASE+0x000c)

//--------------Bits Location------------------------------------------//
//--------------UART5-------------------------------------------//
//--------------Register Address---------------------------------------//
#define UART5_BASE                    0xB016a000
#define UART5_CTL                     (UART5_BASE+0x0000)
#define UART5_RXDAT                   (UART5_BASE+0x0004)
#define UART5_TXDAT                   (UART5_BASE+0x0008)
#define UART5_STAT                    (UART5_BASE+0x000c)

//--------------Bits Location------------------------------------------//
//--------------I2C0-------------------------------------------//
//--------------Register Address---------------------------------------//
#define I2C0_BASE                     0xB0180000
#define I2C0_CTL                      (I2C0_BASE+0x0000)
#define I2C0_CLKDIV                   (I2C0_BASE+0x0004)
#define I2C0_STAT                     (I2C0_BASE+0x0008)
#define I2C0_ADDR                     (I2C0_BASE+0x000C)
#define I2C0_TXDAT                    (I2C0_BASE+0x0010)
#define I2C0_RXDAT                    (I2C0_BASE+0x0014)
#define I2C0_CMD                      (I2C0_BASE+0x0018)
#define I2C0_FIFOCTL                  (I2C0_BASE+0x001C)
#define I2C0_FIFOSTAT                 (I2C0_BASE+0x0020)
#define I2C0_DATCNT                   (I2C0_BASE+0x0024)
#define I2C0_RCNT                     (I2C0_BASE+0x0028)

//--------------Bits Location------------------------------------------//
//--------------I2C1-------------------------------------------//
//--------------Register Address---------------------------------------//
#define I2C1_BASE                     0xB0184000
#define I2C1_CTL                      (I2C1_BASE+0x0000)
#define I2C1_CLKDIV                   (I2C1_BASE+0x0004)
#define I2C1_STAT                     (I2C1_BASE+0x0008)
#define I2C1_ADDR                     (I2C1_BASE+0x000C)
#define I2C1_TXDAT                    (I2C1_BASE+0x0010)
#define I2C1_RXDAT                    (I2C1_BASE+0x0014)
#define I2C1_CMD                      (I2C1_BASE+0x0018)
#define I2C1_FIFOCTL                  (I2C1_BASE+0x001C)
#define I2C1_FIFOSTAT                 (I2C1_BASE+0x0020)
#define I2C1_DATCNT                   (I2C1_BASE+0x0024)
#define I2C1_RCNT                     (I2C1_BASE+0x0028)

//--------------Bits Location------------------------------------------//
//--------------I2C2-------------------------------------------//
//--------------Register Address---------------------------------------//
#define I2C2_BASE                     0xB0188000
#define I2C2_CTL                      (I2C2_BASE+0x0000)
#define I2C2_CLKDIV                   (I2C2_BASE+0x0004)
#define I2C2_STAT                     (I2C2_BASE+0x0008)
#define I2C2_ADDR                     (I2C2_BASE+0x000C)
#define I2C2_TXDAT                    (I2C2_BASE+0x0010)
#define I2C2_RXDAT                    (I2C2_BASE+0x0014)
#define I2C2_CMD                      (I2C2_BASE+0x0018)
#define I2C2_FIFOCTL                  (I2C2_BASE+0x001C)
#define I2C2_FIFOSTAT                 (I2C2_BASE+0x0020)
#define I2C2_DATCNT                   (I2C2_BASE+0x0024)
#define I2C2_RCNT                     (I2C2_BASE+0x0028)

//--------------Bits Location------------------------------------------//
//--------------LCD-------------------------------------------//
//--------------Register Address---------------------------------------//
#define LCD_BASE                      0xB02A0000
#define LCD0_CTL                      (LCD_BASE+0x0000)
#define LCD0_SIZE                     (LCD_BASE+0x0004)
#define LCD0_STATUS                   (LCD_BASE+0x0008)
#define LCD0_TIM0                     (LCD_BASE+0x000C)
#define LCD0_TIM1                     (LCD_BASE+0x0010)
#define LCD0_TIM2                     (LCD_BASE+0x0014)
#define LCD0_COLOR                    (LCD_BASE+0x0018)
#define LCD0_CPU_CTL                  (LCD_BASE+0x001c)
#define LCD0_CPU_CMD                  (LCD_BASE+0x0020)
#define LCD0_TEST_P0                  (LCD_BASE+0x0024)
#define LCD0_TEST_P1                  (LCD_BASE+0x0028)
#define LCD0_IMG_XPOS                 (LCD_BASE+0x002c)
#define LCD0_IMG_YPOS                 (LCD_BASE+0x0030)
#define LCD1_CTL                      (LCD_BASE+0x0100)
#define LCD1_SIZE                     (LCD_BASE+0x0104)
#define LCD1_TIM1                     (LCD_BASE+0x0108)
#define LCD1_TIM2                     (LCD_BASE+0x010c)
#define LCD1_COLOR                    (LCD_BASE+0x0110)
#define LCD1_CPU_CTL                  (LCD_BASE+0x0114)
#define LCD1_CPU_CMD                  (LCD_BASE+0x0118)
#define LCD1_IMG_XPOS                 (LCD_BASE+0x011C)
#define LCD1_IMG_YPOS                 (LCD_BASE+0x0120)

//--------------Bits Location------------------------------------------//
//--------------LVDS-------------------------------------------//
//--------------Register Address---------------------------------------//
#define LVDS_BASE                     0xB02A0200
#define LVDS_CTL                      (LVDS_BASE+0x0000)
#define LVDS_ALG_CTL0                 (LVDS_BASE+0x0004)
#define LVDS_DEBUG                    (LVDS_BASE+0x0008)

//--------------Bits Location------------------------------------------//
//--------------TVOUT-------------------------------------------//
//--------------Register Address---------------------------------------//
#define TVOUT_BASE                    0xB02B0000
#define TVOUT_EN                      (TVOUT_BASE+0x0000)
#define TVOUT_OCR                     (TVOUT_BASE+0x0004)
#define TVOUT_STA                     (TVOUT_BASE+0x0008)
#define TVOUT_CCR                     (TVOUT_BASE+0x000C)
#define TVOUT_BCR                     (TVOUT_BASE+0x0010)
#define TVOUT_CSCR                    (TVOUT_BASE+0x0014)
#define TVOUT_PRL                     (TVOUT_BASE+0x0018)
#define TVOUT_VFALD                   (TVOUT_BASE+0x001C)
#define CVBS_MSR                      (TVOUT_BASE+0x0020)
#define CVBS_AL_SEPO                  (TVOUT_BASE+0x0024)
#define CVBS_AL_SEPE                  (TVOUT_BASE+0x0028)
#define CVBS_AD_SEP                   (TVOUT_BASE+0x002C)
#define CVBS_HUECR                    (TVOUT_BASE+0x0030)
#define CVBS_SCPCR                    (TVOUT_BASE+0x0034)
#define CVBS_SCFCR                    (TVOUT_BASE+0x0038)
#define CVBS_CBACR                    (TVOUT_BASE+0x003C)
#define CVBS_SACR                     (TVOUT_BASE+0x0040)
#define BT_MSR0                       (TVOUT_BASE+0x0100)
#define BT_MSR1                       (TVOUT_BASE+0x0104)
#define BT_AL_SEPO                    (TVOUT_BASE+0x0108)
#define BT_AL_SEPE                    (TVOUT_BASE+0x010C)
#define BT_AP_SEP                     (TVOUT_BASE+0x0110)
#define TVOUT_DCR                     (TVOUT_BASE+0x0070)
#define TVOUT_DDCR                    (TVOUT_BASE+0x0074)
#define TVOUT_DCORCTL                 (TVOUT_BASE+0x0078)
#define TVOUT_DRCR                    (TVOUT_BASE+0x007C)

//--------------Bits Location------------------------------------------//
//--------------HDMI_Transmitter_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define HDMI_BASE                     0xb02c0000
#define HDMI_VICTL                    (HDMI_BASE+0x0000)
#define HDMI_VIVSYNC                  (HDMI_BASE+0x0004)
#define HDMI_VIVHSYNC                 (HDMI_BASE+0x0008)
#define HDMI_VIALSEOF                 (HDMI_BASE+0x000C)
#define HDMI_VIALSEEF                 (HDMI_BASE+0x0010)
#define HDMI_VIADLSE                  (HDMI_BASE+0x0014)
#define HDMI_AIFRAMEC                 (HDMI_BASE+0x0020)
#define HDMI_AICHSTABYTE0TO3                                              (HDMI_BASE+0x0024)
#define HDMI_AICHSTABYTE4TO7                                              (HDMI_BASE+0x0028)
#define HDMI_AICHSTABYTE8TO11                                             (HDMI_BASE+0x002C)
#define HDMI_AICHSTABYTE12TO15                                            (HDMI_BASE+0x0030)
#define HDMI_AICHSTABYTE16TO19                                            (HDMI_BASE+0x0034)
#define HDMI_AICHSTABYTE20TO23                                            (HDMI_BASE+0x0038)
#define HDMI_AICHSTASCN                 (HDMI_BASE+0x003C)
#define HDMI_VR                       (HDMI_BASE+0x0050)
#define HDMI_CR                       (HDMI_BASE+0x0054)
#define HDMI_SCHCR                    (HDMI_BASE+0x0058)
#define HDMI_ICR                      (HDMI_BASE+0x005C)
#define HDMI_SCR                      (HDMI_BASE+0x0060)
#define HDMI_LPCR                     (HDMI_BASE+0x0064)
#define HDCP_CR                       (HDMI_BASE+0x0068)
#define HDCP_SR                       (HDMI_BASE+0x006C)
#define HDCP_ANLR                     (HDMI_BASE+0x0070)
#define HDCP_ANMR                     (HDMI_BASE+0x0074)
#define HDCP_ANILR                    (HDMI_BASE+0x0078)
#define HDCP_ANIMR                    (HDMI_BASE+0x007C)
#define HDCP_DPKLR                    (HDMI_BASE+0x0080)
#define HDCP_DPKMR                    (HDMI_BASE+0x0084)
#define HDCP_LIR                      (HDMI_BASE+0x0088)
#define HDCP_SHACR                    (HDMI_BASE+0x008C)
#define HDCP_SHADR                    (HDMI_BASE+0x0090)
#define HDCP_ICR                      (HDMI_BASE+0x0094)
#define HDCP_KMMR                     (HDMI_BASE+0x0098)
#define HDCP_KMLR                     (HDMI_BASE+0x009C)
#define HDCP_MILR                     (HDMI_BASE+0x00A0)
#define HDCP_MIMR                     (HDMI_BASE+0x00A4)
#define HDCP_KOWR                     (HDMI_BASE+0x00A8)
#define HDCP_OWR                      (HDMI_BASE+0x00AC)
#define TMDS_STR0                     (HDMI_BASE+0x00B8)
#define TMDS_STR1                     (HDMI_BASE+0x00BC)
#define TMDS_EODR0                    (HDMI_BASE+0x00C0)
#define TMDS_EODR1                    (HDMI_BASE+0x00C4)
#define HDMI_ASPCR                    (HDMI_BASE+0x00D0)
#define HDMI_ACACR                    (HDMI_BASE+0x00D4)
#define HDMI_ACRPCR                   (HDMI_BASE+0x00D8)
#define HDMI_ACRPCTSR                 (HDMI_BASE+0x00DC)
#define HDMI_ACRPPR                   (HDMI_BASE+0x00E0)
#define HDMI_GCPCR                    (HDMI_BASE+0x00E4)
#define HDMI_RPCR                     (HDMI_BASE+0x00E8)
#define HDMI_RPRBDR                   (HDMI_BASE+0x00EC)
#define HDMI_OPCR                     (HDMI_BASE+0x00F0)
#define HDMI_DIPCCR                   (HDMI_BASE+0x00F4)
#define HDMI_ORP6PH                   (HDMI_BASE+0x00F8)
#define HDMI_ORSP6W0                  (HDMI_BASE+0x00FC)
#define HDMI_ORSP6W1                  (HDMI_BASE+0x0100)
#define HDMI_ORSP6W2                  (HDMI_BASE+0x0104)
#define HDMI_ORSP6W3                  (HDMI_BASE+0x0108)
#define HDMI_ORSP6W4                  (HDMI_BASE+0x010C)
#define HDMI_ORSP6W5                  (HDMI_BASE+0x0110)
#define HDMI_ORSP6W6                  (HDMI_BASE+0x0114)
#define HDMI_ORSP6W7                  (HDMI_BASE+0x0118)
#define HDMI_CECCR                    (HDMI_BASE+0x011C)
#define HDMI_CECRTCR                  (HDMI_BASE+0x0120)
#define HDMI_CECRXCR                  (HDMI_BASE+0x0124)
#define HDMI_CECTXCR                  (HDMI_BASE+0x0128)
#define HDMI_CECTXDR                  (HDMI_BASE+0x012C)
#define HDMI_CECRXDR                  (HDMI_BASE+0x0130)
#define HDMI_CECRXTCR                 (HDMI_BASE+0x0134)
#define HDMI_CECTXTCR0                (HDMI_BASE+0x0138)
#define HDMI_CECTXTCR1                (HDMI_BASE+0x013C)
#define HDMI_CRCCR                    (HDMI_BASE+0x0140)
#define HDMI_CRCDOR                   (HDMI_BASE+0x0144)
#define HDMI_TX_1                     (HDMI_BASE+0x0154)
#define HDMI_TX_2                     (HDMI_BASE+0x0158)
#define CEC_DDC_HPD                   (HDMI_BASE+0x015C)

//--------------Bits Location------------------------------------------//
//--------------KEY-------------------------------------------//
//--------------Register Address---------------------------------------//
#define KEY_BASE                      0xB01A0000
#define KEY_CTL                       (KEY_BASE+0x0000)
#define KEY_DAT0                      (KEY_BASE+0x0004)
#define KEY_DAT1                      (KEY_BASE+0x0008)
#define KEY_DAT2                      (KEY_BASE+0x000C)
#define KEY_DAT3                      (KEY_BASE+0x0010)
#define KEY_DAT4                      (KEY_BASE+0x0014)
#define KEY_DAT5                      (KEY_BASE+0x0018)
#define KEY_DAT6                      (KEY_BASE+0x001C)
#define KEY_DAT7                      (KEY_BASE+0x0020)

//--------------Bits Location------------------------------------------//
//--------------GPIO_MFP_PWM-------------------------------------------//
//--------------Register Address---------------------------------------//
#define GPIO_MFP_PWM_BASE                0xb01C0000
#define GPIO_AOUTEN                   (GPIO_MFP_PWM_BASE+0x0000)
#define GPIO_AINEN                    (GPIO_MFP_PWM_BASE+0x0004)
#define GPIO_ADAT                     (GPIO_MFP_PWM_BASE+0x0008)
#define GPIO_BOUTEN                   (GPIO_MFP_PWM_BASE+0x000C)
#define GPIO_BINEN                    (GPIO_MFP_PWM_BASE+0x0010)
#define GPIO_BDAT                     (GPIO_MFP_PWM_BASE+0x0014)
#define GPIO_COUTEN                   (GPIO_MFP_PWM_BASE+0x0018)
#define GPIO_CINEN                    (GPIO_MFP_PWM_BASE+0x001C)
#define GPIO_CDAT                     (GPIO_MFP_PWM_BASE+0x0020)
#define GPIO_DOUTEN                   (GPIO_MFP_PWM_BASE+0x0024)
#define GPIO_DINEN                    (GPIO_MFP_PWM_BASE+0x0028)
#define GPIO_DDAT                     (GPIO_MFP_PWM_BASE+0x002C)
#define GPIO_EOUTEN                   (GPIO_MFP_PWM_BASE+0x0030)
#define GPIO_EINEN                    (GPIO_MFP_PWM_BASE+0x0034)
#define GPIO_EDAT                     (GPIO_MFP_PWM_BASE+0x0038)

#define MFP_CTL0                      (GPIO_MFP_PWM_BASE+0x0040)
#define MFP_CTL1                      (GPIO_MFP_PWM_BASE+0x0044)
#define MFP_CTL2                      (GPIO_MFP_PWM_BASE+0x0048)
#define MFP_CTL3                      (GPIO_MFP_PWM_BASE+0x004C)

#define PWM_CTL0                      (GPIO_MFP_PWM_BASE+0X50)
#define PWM_CTL1                      (GPIO_MFP_PWM_BASE+0X54)
#define PWM_CTL2                      (GPIO_MFP_PWM_BASE+0X58)
#define PWM_CTL3                      (GPIO_MFP_PWM_BASE+0X5C)

#define PAD_PULLCTL0                  (GPIO_MFP_PWM_BASE+0x0060)
#define PAD_PULLCTL1                  (GPIO_MFP_PWM_BASE+0x0064)
#define PAD_PULLCTL2                  (GPIO_MFP_PWM_BASE+0x0068)
#define PAD_ST0                       (GPIO_MFP_PWM_BASE+0x006C)
#define PAD_ST1                       (GPIO_MFP_PWM_BASE+0x0070)
#define PAD_CTL                       (GPIO_MFP_PWM_BASE+0x0074)
#define PAD_DRV0                      (GPIO_MFP_PWM_BASE+0x0080)
#define PAD_DRV1                      (GPIO_MFP_PWM_BASE+0x0084)
#define PAD_DRV2                      (GPIO_MFP_PWM_BASE+0x0088)

#define DEBUG_SEL                     (GPIO_MFP_PWM_BASE+0x0090)
#define DEBUG_OEN0                    (GPIO_MFP_PWM_BASE+0x0094)
#define DEBUG_OEN1                    (GPIO_MFP_PWM_BASE+0x0098)
#define DEBUG_IEN0                    (GPIO_MFP_PWM_BASE+0x009C)
#define DEBUG_IEN1                    (GPIO_MFP_PWM_BASE+0x00A0)

#define BIST_START0                   (GPIO_MFP_PWM_BASE+0x00C0)
#define BIST_START1                   (GPIO_MFP_PWM_BASE+0x00C4)
#define BIST_DONE0                    (GPIO_MFP_PWM_BASE+0x00C8)
#define BIST_DONE1                    (GPIO_MFP_PWM_BASE+0x00CC)
#define BIST_DONE2                    (GPIO_MFP_PWM_BASE+0x00D0)
#define BIST_FAIL0                    (GPIO_MFP_PWM_BASE+0x00D4)
#define BIST_FAIL1                    (GPIO_MFP_PWM_BASE+0x00D8)
#define BIST_FAIL2                    (GPIO_MFP_PWM_BASE+0x00DC)

#define L2_BIST_INSTR1                (GPIO_MFP_PWM_BASE+0x00F0)
#define L2_BIST_INSTR2                (GPIO_MFP_PWM_BASE+0x00F4)
#define L2_BIST_INSTR3                (GPIO_MFP_PWM_BASE+0x00F8)

#define INTC_EXTCTL                   (GPIO_MFP_PWM_BASE+0x0200)
#define INTC_GPIOCTL                  (GPIO_MFP_PWM_BASE+0x0204)
#define INTC_GPIOA_PD                 (GPIO_MFP_PWM_BASE+0x0208)
#define INTC_GPIOA_MSK                (GPIO_MFP_PWM_BASE+0x020c)
#define INTC_GPIOB_PD                 (GPIO_MFP_PWM_BASE+0x0210)
#define INTC_GPIOB_MSK                (GPIO_MFP_PWM_BASE+0x0214)
#define INTC_GPIOC_PD                 (GPIO_MFP_PWM_BASE+0x0218)
#define INTC_GPIOC_MSK                (GPIO_MFP_PWM_BASE+0x021c)
#define INTC_GPIOD_PD                 (GPIO_MFP_PWM_BASE+0x0220)
#define INTC_GPIOD_MSK                (GPIO_MFP_PWM_BASE+0x0224)
#define INTC_GPIOE_PD                 (GPIO_MFP_PWM_BASE+0x0228)
#define INTC_GPIOE_MSK                (GPIO_MFP_PWM_BASE+0x022c)

#endif
