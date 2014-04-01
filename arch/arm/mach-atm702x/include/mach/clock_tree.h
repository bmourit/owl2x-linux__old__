
#ifndef __ARM_LEOPARD_CLOCK_TREE_H__
#define __ARM_LEOPARD_CLOCK_TREE_H__

#define CLK_RATE_LOSC   32768 
#define CLK_RATE_HOSC    24000000


#define GPLL_ENABLE_BIT     8
#define GPLL_ENABLE_MASK    (0x1<<GPLL_ENABLE_BIT)
#define GPLL_VALUE_SHIFT    (0)
#define GPLL_VALUE_MASK     (0x7f<<GPLL_VALUE_SHIFT)

#define COREPLL_CLK_MAX_RATE           1524000000 
#define COREPLL_CLK_MIN_RATE           48000000
#define COREPLL_CLK_RATE_STEP          12000000 
#define PLL_LOCK_DELAY                 50

#define DEVPLL_CLK_MAX_RATE  756000000
#define DEVPLL_CLK_MIN_RATE  48000000
#define DEVPLL_CLK_RATE_STEP  6000000

#define NANDPLL_CLK_MAX_RATE    516000000 
#define NANDPLL_CLK_MIN_RATE    12000000
#define NANDPLL_CLK_RATE_STEP   6000000

#define DDRPLL_CLK_MAX_RATE    1440000000 
#define DDRPLL_CLK_MIN_RATE    12000000
#define DDRPLL_CLK_RATE_STEP   12000000

#define DISPLAYPLL_CLK_MAX_RATE    756000000 
#define DISPLAYPLL_CLK_MIN_RATE    12000000
#define DISPLAYPLL_CLK_RATE_STEP   6000000

#define AUDIOPLL_ENABLE_MASK  (0x1<<4)
#define AUDIOPLL_VALUE_MASK   0x1

#define TVOUT0PLL_ENABLE_MASK  (0x1<<3)
#define TVOUT0PLL_VALUE_MASK   0x3
#define TVOUT1PLL_ENABLE_MASK  (0x1<<11)
#define TVOUT1PLL_VALUE_MASK   (0x7<<8)

#define DEEPCOLORPLL_ENABLE_MASK  (0x1<<19)

#define PLL120M_ENABLE_MASK  (0x1)
#define PLL120M_RATE        120000000

#define CPUCLK_SEL_MASK   0x3  
#define DEVCLK_SEL_MASK   (0x1 << 12) 


#define AHBCLK_SEL_MASK   (0x1 << 7)
#define AHBCLK_DIV_MASK   (0x7 << 4)
#define AHBCLK_MAX_RATE    180000000
#define AHBCLK_MIN_RATE    80000000

#define APBCLK_DIV_MASK   (0x7 << 8)
#define APBCLK_MAX_RATE    80000000
#define APBCLK_MIN_RATE    24000000


#define NIC_DCU_SEL_MASK   (0x1 << 16)

#define LCD0CLK_DIV_MASK   0xf
#define LCD1CLK_DIV_MASK   (0xf << 4)
#define BISPCLK_SEL_MASK    (0x1 << 4)
#define BISPCLK_DIV_MASK    (0xf)
#define SENSORCLK_SEL_MASK    (0x1 << 4)
#define TVOUTMUXCLK_SEL_MASK    (0x1 << 6)
#define TVOUT0CLK_DIV_MASK    (0x3<<4)
#define I2STXCLK_DIV_MASK    (0xf<<16)
#define I2SRXCLK_DIV_MASK    (0xf<<20)
#define HDMICLK_DIV_MASK    (0xf<<24)
#define SPDIFCLK_DIV_MASK    (0xf<<28)
#define LENSCLK_DIV_MASK    (0x7)
#define DDRCLK_SEL_MASK    (0x1<<9)

#define PWMCLK_SEL_MASK    (0x1<<12)
#define PWMCLK_DIV_MASK    (0x3ff)

#define IMG5CLK_SEL_MASK    (0x1<<17)
#define IMG5CLK_DIV_MASK    (0x3ff)
#define DMACLK_DIV_MASK    (0x3)
#define DMACLK_MAX_RATE    200000000

#define VCECLK_DIV_MASK    0xf 
#define VDECLK_SEL_MASK    (0x3<<4)
#define VDECLK_DIV_MASK     0xf
#define GPU2DCLK_SEL_MASK    (0x3<<4)
#define GPU2DCLK_DIV_MASK     0x3
#define GPU2D_NIC_CLK_SEL_MASK    (0x3<<20)
#define GPU2D_NIC_CLK_DIV_MASK    (0x3<<16)
#define GPU3DCLK_SEL_MASK    (0x3<<4)
#define GPU3DCLK_DIV_MASK     0x3
#define GPU3D_NIC_CLK_SEL_MASK    (0x3<<20)
#define GPU3D_NIC_CLK_DIV_MASK    (0x3<<16)

#define SDCLK_SEL_MASK    (0x3<<9)  
#define SDCLK_DIV_MASK    0x1f
#define UARTCLK_SEL_MASK    (0x1<<16)
#define UARTCLK_DIV_MASK    0x1ff

#define LCDCLK_SEL_MASK  (0x3<<12)   
#define LCDCLK_DIV_MASK   (0x1<<8)
#define CSICLK_SEL_MASK  (0x1<<4) 
#define CSICLK_DIV_MASK   (0xf)

#define DECLK_SEL_MASK    (0x1<<12)
#define DE1CLK_DIV_MASK    0xf
#define DE2CLK_DIV_MASK    (0xf<<4)
#define DE3CLK_DIV_MASK    (0xf<<8)
#define DE_WB_CLK_DIV_MASK    (0xf<<20)

#define SENSOROUT0CLK_DIV_MASK    0xf
#define SENSOROUT1CLK_DIV_MASK    (0xf<<8)

#endif