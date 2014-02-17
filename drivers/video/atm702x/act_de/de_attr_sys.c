/***************************************************************************
 *                              GLBASE
 *                            Module: DE driver
 *                 Copyright(c) 2011-2015 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       wanghui     2012-07-30 9:00     1.0             build this file 
 ***************************************************************************/
#include "de_attr_sys.h"

/*****************************************************************************	
*system info attr
*****************************************************************************/	
ssize_t system_info_show(struct bus_type *bus, char *buf)
{ 
	short offset = 0; 
#if 0
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "de status information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CMU_DISPLAYPLL = %x\n",act_readl(CMU_DISPLAYPLL));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CMU_DEVPLL = %x\n",act_readl(CMU_DEVPLL));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CMU_DECLK = %x\n",act_readl(CMU_DECLK));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CMU_DEVRST0 = %x\n",act_readl(CMU_DEVRST0));
#endif 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "DE_INTEN = %x\n",act_readl(DE_INTEN));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "DE_STAT = %x\n",act_readl(DE_STAT));

  offset += snprintf(&buf[offset], PAGE_SIZE - offset,  "path1 information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 

  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_CTL = %x\n",act_readl(PATH1_CTL));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_FCR = %x\n",act_readl(PATH1_FCR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_EN = %x\n",act_readl(PATH1_EN));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_BK = %x\n",act_readl(PATH1_BK));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_SIZE = %x\n",act_readl(PATH1_SIZE));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_A_COOR = %x\n",act_readl(PATH1_A_COOR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_B_COOR = %x\n",act_readl(PATH1_B_COOR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_ALPHA_CFG = %x\n",act_readl(PATH1_ALPHA_CFG));		
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_CKMAX = %x\n",act_readl(PATH1_CKMAX));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_CKMIN = %x\n",act_readl(PATH1_CKMIN));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_GAMMA1_IDX = %x\n",act_readl(PATH1_GAMMA1_IDX));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_GAMMA1_RAM = %x\n",act_readl(PATH1_GAMMA1_RAM));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_GAMMA2_IDX = %x\n",act_readl(PATH1_GAMMA2_IDX));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH1_GAMMA2_RAM = %x\n",act_readl(PATH1_GAMMA2_RAM));
	
	offset += snprintf(&buf[offset], PAGE_SIZE - offset,  "path2 information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
	 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_CTL = %x\n",act_readl(PATH2_CTL));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_FCR = %x\n",act_readl(PATH2_FCR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_EN = %x\n",act_readl(PATH2_EN));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_BK = %x\n",act_readl(PATH2_BK));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_SIZE = %x\n",act_readl(PATH2_SIZE));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_A_COOR = %x\n",act_readl(PATH2_A_COOR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_B_COOR = %x\n",act_readl(PATH2_B_COOR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_ALPHA_CFG = %x\n",act_readl(PATH2_ALPHA_CFG));		
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_CKMAX = %x\n",act_readl(PATH2_CKMAX));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_CKMIN = %x\n",act_readl(PATH2_CKMIN));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_GAMMA1_IDX = %x\n",act_readl(PATH2_GAMMA1_IDX));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_GAMMA1_RAM = %x\n",act_readl(PATH2_GAMMA1_RAM));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_GAMMA2_IDX = %x\n",act_readl(PATH2_GAMMA2_IDX));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PATH2_GAMMA2_RAM = %x\n",act_readl(PATH2_GAMMA2_RAM));

  offset += snprintf(&buf[offset], PAGE_SIZE - offset,  "graphic layer information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "GRAPHIC_CFG = %x\n",act_readl(GRAPHIC_CFG));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "GRAPHIC_SIZE = %x\n",act_readl(GRAPHIC_SIZE));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "GRAPHIC_FB = %x\n",act_readl(GRAPHIC_FB));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "GRAPHIC_STR = %x\n",act_readl(GRAPHIC_STR));


  offset += snprintf(&buf[offset], PAGE_SIZE - offset,  "video layer 1  information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
	
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_CFG = %x\n",act_readl(VIDEO1_CFG));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_ISIZE = %x\n",act_readl(VIDEO1_ISIZE));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_OSIZE = %x\n",act_readl(VIDEO1_OSIZE));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_SR = %x\n",act_readl(VIDEO1_SR));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_SCOEF0 = %x\n",act_readl(VIDEO1_SCOEF0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_SCOEF1 = %x\n",act_readl(VIDEO1_SCOEF1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_SCOEF2 = %x\n",act_readl(VIDEO1_SCOEF2));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_SCOEF3 = %x\n",act_readl(VIDEO1_SCOEF3));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_SCOEF4 = %x\n",act_readl(VIDEO1_SCOEF4));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_SCOEF5 = %x\n",act_readl(VIDEO1_SCOEF5));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_SCOEF6 = %x\n",act_readl(VIDEO1_SCOEF6));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_SCOEF7 = %x\n",act_readl(VIDEO1_SCOEF7));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_FB_0 = %x\n",act_readl(VIDEO1_FB_0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_FB_1 = %x\n",act_readl(VIDEO1_FB_1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_FB_2 = %x\n",act_readl(VIDEO1_FB_2));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_FB_RIGHT_0 = %x\n",act_readl(VIDEO1_FB_RIGHT_0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_FB_RIGHT_1 = %x\n",act_readl(VIDEO1_FB_RIGHT_1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_FB_RIGHT_2 = %x\n",act_readl(VIDEO1_FB_RIGHT_2));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_STR = %x\n",act_readl(VIDEO1_STR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_CRITICAL = %x\n",act_readl(VIDEO1_CRITICAL));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO1_REMAPPING = %x\n",act_readl(VIDEO1_REMAPPING));
 
  offset += snprintf(&buf[offset], PAGE_SIZE - offset,  "video layer 2  information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");  
 	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_CFG = %x\n",act_readl(VIDEO2_CFG));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_ISIZE = %x\n",act_readl(VIDEO2_ISIZE));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_OSIZE = %x\n",act_readl(VIDEO2_OSIZE));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_SR = %x\n",act_readl(VIDEO2_SR));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_SCOEF0 = %x\n",act_readl(VIDEO2_SCOEF0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_SCOEF1 = %x\n",act_readl(VIDEO2_SCOEF1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_SCOEF2 = %x\n",act_readl(VIDEO2_SCOEF2));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_SCOEF3 = %x\n",act_readl(VIDEO2_SCOEF3));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_SCOEF4 = %x\n",act_readl(VIDEO2_SCOEF4));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_SCOEF5 = %x\n",act_readl(VIDEO2_SCOEF5));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_SCOEF6 = %x\n",act_readl(VIDEO2_SCOEF6));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_SCOEF7 = %x\n",act_readl(VIDEO2_SCOEF7));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_FB_0 = %x\n",act_readl(VIDEO2_FB_0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_FB_1 = %x\n",act_readl(VIDEO2_FB_1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_FB_2 = %x\n",act_readl(VIDEO2_FB_2));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_FB_RIGHT_0 = %x\n",act_readl(VIDEO2_FB_RIGHT_0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_FB_RIGHT_1 = %x\n",act_readl(VIDEO2_FB_RIGHT_1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_FB_RIGHT_2 = %x\n",act_readl(VIDEO2_FB_RIGHT_2));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_STR = %x\n",act_readl(VIDEO2_STR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_CRITICAL = %x\n",act_readl(VIDEO2_CRITICAL));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO2_REMAPPING = %x\n",act_readl(VIDEO2_REMAPPING));

  offset += snprintf(&buf[offset], PAGE_SIZE - offset,  "video layer 3  information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");  
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_CFG = %x\n",act_readl(VIDEO3_CFG));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_ISIZE = %x\n",act_readl(VIDEO3_ISIZE));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_OSIZE = %x\n",act_readl(VIDEO3_OSIZE));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_SR = %x\n",act_readl(VIDEO3_SR));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_SCOEF0 = %x\n",act_readl(VIDEO3_SCOEF0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_SCOEF1 = %x\n",act_readl(VIDEO3_SCOEF1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_SCOEF2 = %x\n",act_readl(VIDEO3_SCOEF2));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_SCOEF3 = %x\n",act_readl(VIDEO3_SCOEF3));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_SCOEF4 = %x\n",act_readl(VIDEO3_SCOEF4));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_SCOEF5 = %x\n",act_readl(VIDEO3_SCOEF5));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_SCOEF6 = %x\n",act_readl(VIDEO3_SCOEF6));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_SCOEF7 = %x\n",act_readl(VIDEO3_SCOEF7));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_FB_0 = %x\n",act_readl(VIDEO3_FB_0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_FB_1 = %x\n",act_readl(VIDEO3_FB_1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_FB_2 = %x\n",act_readl(VIDEO3_FB_2));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_FB_RIGHT_0 = %x\n",act_readl(VIDEO3_FB_RIGHT_0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_FB_RIGHT_1 = %x\n",act_readl(VIDEO3_FB_RIGHT_1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_FB_RIGHT_2 = %x\n",act_readl(VIDEO3_FB_RIGHT_2));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_STR = %x\n",act_readl(VIDEO3_STR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_CRITICAL = %x\n",act_readl(VIDEO3_CRITICAL));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "VIDEO3_REMAPPING = %x\n",act_readl(VIDEO3_REMAPPING));
 
  offset += snprintf(&buf[offset], PAGE_SIZE - offset,  " mdsb infomations  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");  

  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_CTL =%x\n", act_readl(MDSB_CTL));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_CVBS_HR =%x\n", act_readl(MDSB_CVBS_HR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_CVBS_VR = 0x%x\n", act_readl(MDSB_CVBS_VR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_LCD_HR = 0x%x\n", act_readl(MDSB_LCD_HR));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_LCD_VR = 0x%x\n", act_readl(MDSB_LCD_VR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_STSEQ =%x\n", act_readl(MDSB_STSEQ));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_DELAY0 =%x\n", act_readl(MDSB_DELAY0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_DELAY1 =%x\n", act_readl(MDSB_DELAY1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_CVBS_OFF =%x\n", act_readl(MDSB_CVBS_OFF));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_LCD_OFF =%x\n", act_readl(MDSB_LCD_OFF));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_HDMIWH =%x\n", act_readl(MDSB_HDMIWH));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_STAT =%x\n", act_readl(MDSB_STAT));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_HSCALER0 =%x\n", act_readl(MDSB_HSCALER0)); 
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_HSCALER1 =%x\n", act_readl(MDSB_HSCALER1)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_HSCALER2 =%x\n", act_readl(MDSB_HSCALER2));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_HSCALER3 =%x\n", act_readl(MDSB_HSCALER3));  	
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_HSCALER4 =%x\n", act_readl(MDSB_HSCALER4)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_HSCALER5 =%x\n", act_readl(MDSB_HSCALER5));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_HSCALER6 =%x\n", act_readl(MDSB_HSCALER6));  	
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_HSCALER7 =%x\n", act_readl(MDSB_HSCALER7)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_VSCALER0 =%x\n", act_readl(MDSB_VSCALER0));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_VSCALER1 =%x\n", act_readl(MDSB_VSCALER1));  
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_VSCALER2 =%x\n", act_readl(MDSB_VSCALER2)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_VSCALER3 =%x\n", act_readl(MDSB_VSCALER3)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_VSCALER4 =%x\n", act_readl(MDSB_VSCALER4));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_VSCALER5 =%x\n", act_readl(MDSB_VSCALER5));  	
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_VSCALER6 =%x\n", act_readl(MDSB_VSCALER6)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_VSCALER7 =%x\n", act_readl(MDSB_VSCALER7));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_WBCFG =%x\n", act_readl(MDSB_WBCFG));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_WBADDR =%x\n", act_readl(MDSB_WBADDR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_WBLEN =%x\n", act_readl(MDSB_WBLEN));
	
	
	offset += snprintf(&buf[offset], PAGE_SIZE - offset,  " LCD infomations  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");  
#if 0
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CMU_LCDCLK =%x\n", act_readl(CMU_LCDCLK)); 
#endif 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_CTL =%x\n", act_readl(LCD0_CTL)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_SIZE =%x\n", act_readl(LCD0_SIZE)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_STATUS =%x\n", act_readl(LCD0_STATUS)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_TIM0 =%x\n", act_readl(LCD0_TIM0)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_TIM1 =%x\n", act_readl(LCD0_TIM1)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_TIM2 =%x\n", act_readl(LCD0_TIM2)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_COLOR =%x\n", act_readl(LCD0_COLOR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_CPU_CTL =%x\n", act_readl(LCD0_CPU_CTL)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_CPU_CMD =%x\n", act_readl(LCD0_CPU_CMD)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_TEST_P0 =%x\n", act_readl(LCD0_TEST_P0)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_TEST_P1 =%x\n", act_readl(LCD0_TEST_P1)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_IMG_XPOS =%x\n", act_readl(LCD0_IMG_XPOS)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD0_IMG_YPOS =%x\n", act_readl(LCD0_IMG_YPOS)); 
	
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD1_CTL =%x\n", act_readl(LCD1_CTL)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD1_SIZE =%x\n", act_readl(LCD1_SIZE)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD1_TIM1 =%x\n", act_readl(LCD1_TIM1)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD1_TIM2 =%x\n", act_readl(LCD1_TIM2)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD1_COLOR =%x\n", act_readl(LCD1_COLOR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD1_CPU_CTL =%x\n", act_readl(LCD1_CPU_CTL)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD1_CPU_CMD =%x\n", act_readl(LCD1_CPU_CMD)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD1_IMG_XPOS =%x\n", act_readl(LCD1_IMG_XPOS)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LCD1_IMG_YPOS =%x\n", act_readl(LCD1_IMG_YPOS)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LVDS_CTL =%x\n", act_readl(LVDS_CTL)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "LVDS_ALG_CTL0 =%x\n", act_readl(LVDS_ALG_CTL0));
	
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PAD_CTL =%x\n", act_readl(PAD_CTL));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MFP_CTL1 =%x\n", act_readl(MFP_CTL1));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MFP_CTL0 =%x\n", act_readl(MFP_CTL0));
#if 0
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CMU_DEVCLKEN0 =%x\n", act_readl(CMU_DEVCLKEN0));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CMU_DEVCLKEN1 =%x\n", act_readl(CMU_DEVCLKEN1));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CMU_PWM0CLK =%x\n", act_readl(CMU_PWM0CLK));
#endif
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "PWM_CTL0 =%x\n", act_readl(PWM_CTL0));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "GPIO_BOUTEN =%x\n", act_readl(GPIO_BOUTEN));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "GPIO_BDAT =%x\n", act_readl(GPIO_BDAT));

#if 0
	offset += snprintf(&buf[offset], PAGE_SIZE - offset,  " tvout infomations  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
	
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CMU_TVOUTPLL =%x\n", act_readl(CMU_TVOUTPLL)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_EN =%x\n", act_readl(TVOUT_EN)); 
			  
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_EN =%x\n", act_readl(TVOUT_EN)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_OCR =%x\n", act_readl(TVOUT_OCR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_STA =%x\n", act_readl(TVOUT_STA)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_CCR =%x\n", act_readl(TVOUT_CCR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_BCR =%x\n", act_readl(TVOUT_BCR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_CSCR =%x\n", act_readl(TVOUT_CSCR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_PRL =%x\n", act_readl(TVOUT_PRL)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_VFALD =%x\n", act_readl(TVOUT_VFALD)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CVBS_MSR =%x\n", act_readl(CVBS_MSR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CVBS_AL_SEPO =%x\n", act_readl(CVBS_AL_SEPO)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CVBS_AL_SEPE =%x\n", act_readl(CVBS_AL_SEPE)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CVBS_AD_SEP =%x\n", act_readl(CVBS_AD_SEP)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CVBS_HUECR =%x\n", act_readl(CVBS_HUECR)); 	
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CVBS_SCPCR =%x\n", act_readl(CVBS_SCPCR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CVBS_SCFCR =%x\n", act_readl(CVBS_SCFCR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CVBS_CBACR =%x\n", act_readl(CVBS_CBACR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "CVBS_SACR =%x\n", act_readl(CVBS_SACR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "BT_MSR0 =%x\n", act_readl(BT_MSR0)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "BT_MSR1 =%x\n", act_readl(BT_MSR1)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "BT_AL_SEPO =%x\n", act_readl(BT_AL_SEPO)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "BT_AL_SEPE =%x\n", act_readl(BT_AL_SEPE)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "BT_AP_SEP =%x\n", act_readl(BT_AP_SEP)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_DCR =%x\n", act_readl(TVOUT_DCR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_DDCR =%x\n", act_readl(TVOUT_DDCR)); 
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_DCORCTL =%x\n", act_readl(TVOUT_DCORCTL));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "TVOUT_DRCR =%x\n", act_readl(TVOUT_DRCR)); 

#endif

  return offset;
}	
/*****************************************************************************	
*display manager  attr
*****************************************************************************/
struct temp_parama{
		int offset;
		int disp_ids;
		char ** buf;
};

struct disp_mgr_attribute {
	struct attribute attr;
	ssize_t (*show)(struct disp_manager *, char *);
	ssize_t	(*store)(struct disp_manager *, const char *, size_t);
};

#define DISP_MANAGER_ATTR(_name, _mode, _show, _store) \
	struct disp_mgr_attribute disp_mgr_attr_##_name = \
	__ATTR(_name, _mode, _show, _store)

static int show_disp_device_info(struct device *dev, void *data)
{
	char *plug_st;
	struct temp_parama * m_parama_t = (struct temp_parama *)data;
	int offset = m_parama_t->offset;
	char * buf = *(m_parama_t->buf);
	struct asoc_display_device * disp_dev = to_asoc_display_device(dev);
	
	if(disp_dev == NULL){
			 return 0;
	}
	if (disp_dev->is_plugged) {
			plug_st = "plugged";
	} else {
			plug_st = "unplugged";
	}

	disp_dev->power = FB_BLANK_UNBLANK;		
		
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "%s-%s\n",	disp_dev->name, plug_st);
	
	* (int *)data = offset;
	
	return 0;
}	
static ssize_t show_displayers(struct disp_manager * disp_mgr, char * buf) {
	int r = 0;
	struct temp_parama parama_t;
	
	parama_t.offset = 0;
  parama_t.buf = &buf;
  
	r = bus_for_each_dev(de_get_bus(), NULL, &parama_t, show_disp_device_info);
	
	if(r){
		printk("show_displayers get attr info error ,r = %d" , r);
		return 0;
	}
		
	return parama_t.offset;

}


static int show_disp_device_info_current(struct device *dev, void *data)
{
	struct asoc_display_device * disp_dev = to_asoc_display_device(dev);
	struct temp_parama * m_parama_t = (struct temp_parama *)data;
	int offset = m_parama_t->offset;
	int current_disp_ids  = m_parama_t->disp_ids;
	char * buf = *(m_parama_t->buf);
	
	
	if(disp_dev == NULL){			  
			 return -1;
	}

	if (current_disp_ids & disp_dev->display_id) {				
		offset += snprintf(&buf[offset],
		  PAGE_SIZE - offset, "%s\n",	disp_dev->name);	
		m_parama_t->offset =  offset;	
	}    		
	return 0;
}
static ssize_t show_cur_displayer(struct disp_manager * disp_mgr, char *buf) {	
	
	int r = 0;				
	struct temp_parama m_parama_t;	
	
	DEDBG("show_cur_displayer disp_mgr->current_disp_ids = %x \n", disp_mgr->current_disp_ids );

	m_parama_t.offset = 0;
	m_parama_t.disp_ids = disp_mgr->current_disp_ids;
  m_parama_t.buf = &buf;
	r = bus_for_each_dev(de_get_bus(), NULL, &m_parama_t, show_disp_device_info_current);
	
	if(r){
		printk("show_displayers get attr info error ,r = %d" , r);
		return 0;
	}
		
	return m_parama_t.offset;
}
static ssize_t store_cur_displayer(struct disp_manager * disp_mgr, const char *buf, size_t count) {
	int r, disp_dev_ids;

	disp_dev_ids = simple_strtoul(buf, NULL, 10);

	DEDBG("request display id = %d\n", disp_dev_ids);
	
	r = disp_manager_switch_displayer(&core, disp_dev_ids);

	if (r) {
		DEDBG("disp_manager_switch_displayer error code = %d\n", r);
		return r;
	}
	
	return count;
}
static ssize_t show_disp_mode(struct disp_manager * disp_mgr, char *buf) {	
	
	DEDBG("show_cur_displayer disp_mgr->disp_mode = %x \n", disp_mgr->disp_mode );	
	switch(disp_mgr->disp_mode){
		case MODE_DISP_SYNC_DEFAULT_TV_GV_LCD_GV:
			return snprintf(buf, PAGE_SIZE, "%s\n", "MODE_DISP_SYNC_DEFAULT_TV_GV_LCD_GV");
		case MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV:
			return snprintf(buf, PAGE_SIZE, "%s\n", "MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV");
		case MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_G:
			return snprintf(buf, PAGE_SIZE, "%s\n", "MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV");
		case MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_V_LCD_G:
			return snprintf(buf, PAGE_SIZE, "%s\n", "MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV");
		case MODE_DISP_DOUBLE_DEFAULT_SYNC_TV_GV_LCD_GV:
			return snprintf(buf, PAGE_SIZE, "%s\n", "MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV");
		case MODE_DISP_DOUBLE_DEFAULT_SYNC_TV_GV_LCD_G:
			return snprintf(buf, PAGE_SIZE, "%s\n", "MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV");
		case MODE_DISP_DOUBLE_DEFAULT_SYNC_TV_V_LCD_G:
			return snprintf(buf, PAGE_SIZE, "%s\n", "MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV");
			
		case MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_GV_LCD_GV:
			return snprintf(buf, PAGE_SIZE, "%s\n", "MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV");
		case MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_GV_LCD_G:
			return snprintf(buf, PAGE_SIZE, "%s\n", "MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV");
		case MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_V_LCD_G:
			return snprintf(buf, PAGE_SIZE, "%s\n", "MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV");
		default:
			return snprintf(buf, PAGE_SIZE, "%s = %d\n","unkown moud",disp_mgr->disp_mode);	
	}
	return -1;
}
static ssize_t store_disp_mode(struct disp_manager * disp_mgr, const char *buf, size_t count) {
	int disp_mode;

	disp_mode = simple_strtoul(buf, NULL, 10);

	DEDBG("store_disp_mode mode = %d\n", disp_mode);
	
	disp_mgr->disp_mode = disp_mode;	
	
	return count;
}
static ssize_t disp_manager_name_show(struct disp_manager * disp_mgr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", "display_mgr");
}

static DISP_MANAGER_ATTR(name, S_IRUGO, 
                   disp_manager_name_show,
                   NULL);                                     
static DISP_MANAGER_ATTR(display, S_IRUGO|S_IWUSR,	
                    show_displayers, 
                    NULL);
                    
static DISP_MANAGER_ATTR(current_diplay, S_IRUGO|S_IWUSR,
                   	show_cur_displayer,          	
                   	store_cur_displayer);
                   	
static DISP_MANAGER_ATTR(disp_mode, S_IRUGO|S_IWUSR,
                   	show_disp_mode,          	
                   	store_disp_mode);
                   	
static struct attribute * disp_mgr_sysfs_attrs[] = {
	&disp_mgr_attr_name.attr,
	&disp_mgr_attr_display.attr,
	&disp_mgr_attr_current_diplay.attr,
	&disp_mgr_attr_disp_mode.attr,
	NULL
};
static ssize_t disp_mgr_attr_show(struct kobject *kobj, struct attribute *attr,
		char *buf)
{
	struct disp_manager * disp_mgr;
	struct disp_mgr_attribute * disp_mgr_attr;

	disp_mgr = container_of(kobj, struct disp_manager, kobj);
	disp_mgr_attr = container_of(attr, struct disp_mgr_attribute, attr);
	
	if (!disp_mgr_attr->show)
		return -ENOENT;
		
	return disp_mgr_attr->show(disp_mgr, buf);
}
static ssize_t disp_mgr_attr_store(struct kobject *kobj, struct attribute *attr,
		const char *buf, size_t size)
{
	struct disp_manager * disp_mgr;
	struct disp_mgr_attribute * disp_mgr_attr;

	disp_mgr = container_of(kobj, struct disp_manager, kobj);
	disp_mgr_attr = container_of(attr, struct disp_mgr_attribute, attr);

	if (!disp_mgr_attr->store)
		return -ENOENT;

	return disp_mgr_attr->store(disp_mgr, buf, size);
}
static const struct sysfs_ops disp_mgr_sysfs_ops = {
	.show = disp_mgr_attr_show,
	.store = disp_mgr_attr_store,
};
static struct kobj_type disp_mgr_ktype = {
	.sysfs_ops = &disp_mgr_sysfs_ops,
	.default_attrs = disp_mgr_sysfs_attrs,
};
int disp_mgr_attr_init(struct disp_manager * disp_mgr, struct de_core * core){
	
  int r = 0;
	
	r = kobject_init_and_add(&disp_mgr->kobj, &disp_mgr_ktype,
				&core->dev->kobj, "disp_mgr");
				
	if (r)
			printk("failed to create sysfs file for disp mgr\n");		
			
	return r;

}
/*****************************************************************************	
*layer attr
*****************************************************************************/
struct layer_attribute {
	struct attribute attr;
	ssize_t (*show)(struct act_layer *, char *);
	ssize_t	(*store)(struct act_layer *, const char *, size_t);
};

#define LAYER_ATTR(_name, _mode, _show, _store) \
	struct layer_attribute layer_attr_##_name = \
	__ATTR(_name, _mode, _show, _store)
	
/*add attr operations here */
static ssize_t layer_name_show(struct act_layer * layer, char *buf)
{ 
	switch(layer->layer_id){
		case GRAPHIC_LAYER:
			return snprintf(buf, PAGE_SIZE, "%s\n", "graphic");
		case VIDEO1_LAYER:
			return snprintf(buf, PAGE_SIZE, "%sn", "video1");
		case VIDEO2_LAYER:
			return snprintf(buf, PAGE_SIZE, "%s\n", "video2");
		case VIDEO3_LAYER:
			return snprintf(buf, PAGE_SIZE, "%s\n", "video3 ");
		default:
		  return snprintf(buf, PAGE_SIZE, "%s\n", "unkown layer");
	}
	return 0;
}

static ssize_t layer_info_show(struct act_layer * layer, char *buf)
{	
	struct act_layer_info * info = layer->info;
	short offset = 0; 
	const char * pix_format[] ={
		"RGB_565",
		"ARGB_8888",
		"YUV_420_PLANAR",
		"YUV_420_SEMI_PLANAR",
		"ABGR_1555",
		"ABGR_8888",
		"YVU_420_PLANAR",
	}	;
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "act layer information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "enabled = %d\n",info->enabled);
  switch(info->scale_option){
  	case SCALE_ORIGINAL_SIZE:
  		offset += snprintf(&buf[offset], PAGE_SIZE - offset, "scale_option = %s , scale_rate = %x\n","SCALE_ORIGINAL_SIZE",info->scale_rate);	
  		break;
  	case SCALE_FIT_SIZE:
  		offset += snprintf(&buf[offset], PAGE_SIZE - offset, "scale_option = %s , scale_rate = %x\n","SCALE_FIT_SIZE",info->scale_rate);	
  		break;
  	case SCALE_FULL_SIZE:
  		offset += snprintf(&buf[offset], PAGE_SIZE - offset, "scale_option = %s , scale_rate = %x\n","SCALE_FULL_SIZE",info->scale_rate);	
  		break;
  	case SCALE_CUSTOM_SCALE:
  		offset += snprintf(&buf[offset], PAGE_SIZE - offset, "scale_option = %s , scale_rate = %x\n","SCALE_CUSTOM_SCALE",info->scale_rate);	
  		break;
		default:
			offset += snprintf(&buf[offset], PAGE_SIZE - offset, "scale_option = %s , scale_rate = %x\n","unkown",info->scale_rate);				
			break;
  }
	
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "screen_width = %d , screen_height = %d\n",info->screen_width,info->screen_height);
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "pixel_format = %s, img_width = %d,img_height = %d\n",pix_format[info->pixel_format],info->img_width,info->img_height);
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "xoff = %d yoff = %d width = %d height = %d \n",info->xoff,info->yoff,info->width,info->height);
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "pos_x = %d pos_y = %d out_width = %d out_height = %d \n",info->pos_x,info->pos_y,info->out_width,info->out_height);
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "paddr = %x ,vaddr= %p \n",info->paddr ,info->vaddr);

	return offset;
}

static ssize_t layer_info_store(struct act_layer * layer, const char *buf, size_t size)
{
	return size;
}

static ssize_t layer_enable_show(struct act_layer * layer, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "enabled = %d, needconfig = %d \n", layer->info->enabled,  layer->needconfig);
}

static ssize_t layer_enable_store(struct act_layer * layer,const char *buf, size_t size)
{
	int enabled ;

	enabled = simple_strtoul(buf, NULL, 10);
	
	layer->info->enabled = enabled;	
	
	layer->needconfig = true;
	
	layer->apply_layer(layer);
	
	layer->wait_for_sync();
	
	return size;
}

static ssize_t layer_zorder_show(struct act_layer * layer, char *buf)
{
	switch(layer->layer_tag){
		case DE_LAYER_A:
			return snprintf(buf, PAGE_SIZE, "%s\n", "layer A");
		case DE_LAYER_B:
			return snprintf(buf, PAGE_SIZE, "%s\n", "layer B");
		default:
			return snprintf(buf, PAGE_SIZE, "%s\n", "unkown");
	}
}
static ssize_t layer_zorder_store(struct act_layer * layer, const char *buf, size_t size)
{
	int layer_tag ;

	layer_tag = simple_strtoul(buf, NULL, 10);
	
	layer->layer_tag = layer_tag;
	
	return size;
}

static ssize_t layer_scale_rate_show(struct act_layer * layer, char *buf) {
	return snprintf(buf, PAGE_SIZE, "%d,%d\n", (u8)(layer->info->scale_rate >> 8),
			(u8)(layer->info->scale_rate & 0xff));	
}

static ssize_t layer_scale_rate_store(struct act_layer * layer, const char *buf, size_t count) {
	int h_rate,v_rate;
	char *last = NULL;
				
	h_rate = simple_strtoul(buf, &last, 0);
	last++;		
	if (last - buf >= count)
		return -EINVAL;				
	v_rate = simple_strtoul(last, &last, 0);	

	layer->info->scale_rate = (u16)((h_rate << 8) | v_rate);
	
	layer->needconfig = true;
	
	DEDBG("scale_rate = %d\n", layer->info->scale_rate );
	
	layer->set_layer_info(layer,layer->info);
	
	layer->apply_layer(layer);
	
	layer->wait_for_sync();	
	
	return count;
}

static ssize_t layer_scale_mode_show(struct act_layer * layer, char *buf) {
	 int offset = 0;
	 struct act_layer_info * info = layer->info;
	 switch(info->scale_option){
  	case SCALE_ORIGINAL_SIZE:
  		offset += snprintf(&buf[offset], PAGE_SIZE - offset, "scale_option = %s\n","SCALE_ORIGINAL_SIZE");	
  		break;
  	case SCALE_FIT_SIZE:
  		offset += snprintf(&buf[offset], PAGE_SIZE - offset, "scale_option = %s \n","SCALE_FIT_SIZE");	
  		break;
  	case SCALE_FULL_SIZE:
  		offset += snprintf(&buf[offset], PAGE_SIZE - offset, "scale_option = %s \n","SCALE_FULL_SIZE");	
  		break;
  	case SCALE_CUSTOM_SCALE:
  		offset += snprintf(&buf[offset], PAGE_SIZE - offset, "scale_option = %s\n","SCALE_CUSTOM_SCALE");	
  		break;
		default:
			offset += snprintf(&buf[offset], PAGE_SIZE - offset, "scale_option = %s\n","unkown");				
			break;
  }
	return offset;	
}

static ssize_t layer_scale_mode_store(struct act_layer * layer, const char *buf, size_t count) {
	int scale_option;
	char *last = NULL;
				
	scale_option = simple_strtoul(buf, &last, 0);
	
	DEDBG("scale_option = %d\n",scale_option);

	layer->info->scale_option = scale_option;
	
	layer->set_layer_info(layer,layer->info);
	
	layer->apply_layer(layer);
	
	layer->wait_for_sync();	
	
	return count;
}

static LAYER_ATTR(name, S_IRUGO, 
                   layer_name_show,
                   NULL);
                   
static LAYER_ATTR(info, S_IRUGO|S_IWUSR,	
                    layer_info_show, 
                    layer_info_store);
                    
static LAYER_ATTR(enable, S_IRUGO|S_IWUSR,
                   	layer_enable_show,          	
                   	layer_enable_store);
                  	
static LAYER_ATTR(zorder, S_IRUGO|S_IWUSR,
                   	layer_zorder_show,          	
                   	layer_zorder_store);
                   	                   	                   	               
static LAYER_ATTR(scale_rate, S_IRUGO|S_IWUSR,
                   	layer_scale_rate_show,          	
                   	layer_scale_rate_store);  
                   				                
static LAYER_ATTR(scale_mode, S_IRUGO|S_IWUSR,
                   	layer_scale_mode_show,          	
                   	layer_scale_mode_store);  
                   	
static struct attribute * layer_sysfs_attrs[] = {
	&layer_attr_name.attr,
	&layer_attr_info.attr,
	&layer_attr_enable.attr,
	&layer_attr_zorder.attr,
	&layer_attr_scale_rate.attr,
	&layer_attr_scale_mode.attr,
	NULL
};

static ssize_t layer_attr_show(struct kobject *kobj, struct attribute *attr,
		char *buf)
{
	struct act_layer * layer;
	struct layer_attribute * layer_attr;

	layer = container_of(kobj, struct act_layer, kobj);
	layer_attr = container_of(attr, struct layer_attribute, attr);
	
	if (!layer_attr->show)
		return -ENOENT;
		
	return layer_attr->show(layer, buf);
}

static ssize_t layer_attr_store(struct kobject *kobj, struct attribute *attr,
		const char *buf, size_t size)
{
	struct act_layer * layer;
	struct layer_attribute * layer_attr;

	layer = container_of(kobj, struct act_layer, kobj);
	layer_attr = container_of(attr, struct layer_attribute, attr);

	if (!layer_attr->store)
		return -ENOENT;

	return layer_attr->store(layer, buf, size);
}

static const struct sysfs_ops layer_sysfs_ops = {
	.show = layer_attr_show,
	.store = layer_attr_store,
};

static struct kobj_type layer_ktype = {
	.sysfs_ops = &layer_sysfs_ops,
	.default_attrs = layer_sysfs_attrs,
};	

/*****************************************************************************	
*layer mgr  attr
*****************************************************************************/		
struct layer_mgr_attribute {
	struct attribute attr;
	ssize_t (*show)(struct layer_manager *, char *);
	ssize_t	(*store)(struct layer_manager *, const char *, size_t);
};

#define LAYER_MANAGER_ATTR(_name, _mode, _show, _store) \
	struct layer_mgr_attribute layer_manager_attr_##_name = \
	__ATTR(_name, _mode, _show, _store)	
/*add attr operations here */
static ssize_t layer_manager_name_show(struct layer_manager * layer_mgr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s-%d\n", "layer_mgr",layer_mgr->layer_manager_id);
}

static ssize_t layer_manager_display_show(struct layer_manager * layer_mgr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", layer_mgr->disp_dev->name);
}
static ssize_t layer_manager_display_store(struct layer_manager * layer_mgr,  const char *buf, size_t count)
{
	int display_id = 0;
	struct asoc_display_device * disp_dev = NULL;
						
	display_id = simple_strtoul(buf, NULL, 0);
	
	disp_dev = disp_manager_get_disp_dev(display_id);
	
	if(disp_dev != NULL){
		layer_mgr->disp_dev = disp_dev;
	}else{
		printk("disp device not found \n");
	}
	
	layer_mgr->apply_layers(layer_mgr);
	
	return count;
}

static ssize_t layer_manager_default_color_show(struct layer_manager * layer_mgr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n",layer_mgr->default_color);
}
static ssize_t layer_manager_default_color_store(struct layer_manager * layer_mgr, const char *buf, size_t count)
{
	int back_ground_color = 0;
					
	back_ground_color = simple_strtoul(buf, NULL, 0);
	
	layer_mgr->default_color = back_ground_color;	
	
	layer_mgr->config_manager_info(layer_mgr);
		
	return count;
}

static ssize_t layer_manager_fb_layer_show(struct layer_manager * layer_mgr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n",layer_mgr->layers[layer_mgr->fb_layer_current]->layer_id);
}
static ssize_t layer_manager_fb_layer_store(struct layer_manager * layer_mgr, const char *buf, size_t count)
{
	int fb_layer = 0;
					
	fb_layer = simple_strtoul(buf, NULL, 0);
		
	layer_mgr->fb_layer = fb_layer;	
		
	return count;

}

static ssize_t layer_manager_alpha_blending_enabled_show(struct layer_manager * layer_mgr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "enabled = %d, alpha value = %d \n",layer_mgr->alpha_enabled,layer_mgr->global_alpha);
}

static ssize_t layer_manager_alpha_blending_enabled_store(struct layer_manager * layer_mgr,  const char *buf, size_t count)
{
	int enable = 0;
	int alpha = 0xff;
	char *last = NULL;
					
	enable = simple_strtoul(buf, &last, 0);
	last++;		
	
	if (last - buf >= count)
		return -EINVAL;	
				
	alpha = simple_strtoul(last, &last, 0);		
	
	layer_mgr->alpha_enabled = enable;	
	
	layer_mgr->global_alpha = alpha;
	
	layer_mgr->config_manager_info(layer_mgr);
		
	return count;

}

static ssize_t layer_manager_color_key_enabled_show(struct layer_manager * layer_mgr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "enable = %d color_key_max_value = %d,color_key_min_value =%d \n",layer_mgr->color_key_enabled,layer_mgr->color_key_max_value,layer_mgr->color_key_min_value);
}
static ssize_t layer_manager_color_key_enabled_store(struct layer_manager * layer_mgr, const char *buf, size_t count)
{	
	int color_key_enabled = 0;
	int color_key_max_value = 0;
	int color_key_min_value = 0;
	
	char *last = NULL;
					
	color_key_enabled = simple_strtoul(buf, &last, 0);
	last++;		
	
	if (last - buf >= count)
		return -EINVAL;	
				
	color_key_max_value = simple_strtoul(last, &last, 0);		
	
	last++;		
	
	if (last - buf >= count)
		return -EINVAL;	
		
	color_key_min_value = simple_strtoul(last, &last, 0);		
	
	layer_mgr->color_key_enabled = color_key_enabled;	
	
	layer_mgr->color_key_max_value = color_key_max_value;
	
	layer_mgr->color_key_min_value = color_key_min_value;
	
	layer_mgr->config_manager_info(layer_mgr);
		
	return count;
}

static LAYER_MANAGER_ATTR(name, S_IRUGO, 
                   layer_manager_name_show,
                   NULL);
                   
static LAYER_MANAGER_ATTR(display, S_IRUGO|S_IWUSR,	
                    layer_manager_display_show, 
                    layer_manager_display_store);
                    
static LAYER_MANAGER_ATTR(default_color, S_IRUGO|S_IWUSR,
                   	layer_manager_default_color_show,          	
                   	layer_manager_default_color_store);
                   	
static LAYER_MANAGER_ATTR(fb_layer, S_IRUGO|S_IWUSR,
                   	layer_manager_fb_layer_show,          	
                   	layer_manager_fb_layer_store);               
                   	
static LAYER_MANAGER_ATTR(alpha_blending_enabled, S_IRUGO|S_IWUSR,
										layer_manager_alpha_blending_enabled_show,
										layer_manager_alpha_blending_enabled_store);
										
static LAYER_MANAGER_ATTR(color_key_enabled, S_IRUGO|S_IWUSR,
										layer_manager_color_key_enabled_show,
										layer_manager_color_key_enabled_store);										                

static struct attribute * layer_mgr_sysfs_attrs[] = {
	&layer_manager_attr_name.attr,
	&layer_manager_attr_display.attr,
	&layer_manager_attr_fb_layer.attr,
	&layer_manager_attr_default_color.attr,
	&layer_manager_attr_alpha_blending_enabled.attr,
  &layer_manager_attr_color_key_enabled.attr,
	NULL
};

static ssize_t layer_mgr_attr_show(struct kobject *kobj, struct attribute *attr,
		char *buf)
{
	struct layer_manager * layer_mgr;
	struct layer_mgr_attribute * layer_mgr_attr;

	layer_mgr = container_of(kobj, struct layer_manager, kobj);
	layer_mgr_attr = container_of(attr, struct layer_mgr_attribute, attr);
	
	if (!layer_mgr_attr->show)
		return -ENOENT;
		
	return layer_mgr_attr->show(layer_mgr, buf);
}

static ssize_t layer_mgr_attr_store(struct kobject *kobj, struct attribute *attr,
		const char *buf, size_t size)
{
	struct layer_manager * layer_mgr;
	struct layer_mgr_attribute * layer_mgr_attr;

	layer_mgr = container_of(kobj, struct layer_manager, kobj);
	layer_mgr_attr = container_of(attr, struct layer_mgr_attribute, attr);

	if (!layer_mgr_attr->store)
		return -ENOENT;

	return layer_mgr_attr->store(layer_mgr, buf, size);
}

static const struct sysfs_ops layer_mgr_sysfs_ops = {
	.show = layer_mgr_attr_show,
	.store = layer_mgr_attr_store,
};

static struct kobj_type layer_mgr_ktype = {
	.sysfs_ops = &layer_mgr_sysfs_ops,
	.default_attrs = layer_mgr_sysfs_attrs,
};

int layer_mgr_attr_init(struct layer_manager * layer_mgr, struct de_core * core){
	
  int r = 0;
  int i = 0;
	
	r = kobject_init_and_add(&layer_mgr->kobj, &layer_mgr_ktype,
				&core->dev->kobj, "layer_mgr%d",layer_mgr->layer_manager_id);				
	if (r)
			printk("failed to create sysfs file for layer mgr\n");		
			
	// this init layer attr for manager 		
	for(i = 0 ; i < 2 ; i++){
		struct act_layer * layer = layer_mgr->layers[i];
		r = kobject_init_and_add(&layer->kobj, &layer_ktype,
					&layer_mgr->kobj, "layer%d",layer->layer_id);				
		if (r)
				printk("failed to create sysfs file for layer\n");		
	}	
	return r;
}
/*****************************************************************************	
*mdsb  manager  attr
****************************************************************************/	
struct mdsb_mgr_attribute {
	struct attribute attr;
	ssize_t (*show)(struct  mdsb_manager *, char *);
	ssize_t	(*store)(struct  mdsb_manager *, const char *, size_t);
};
                   
#define MDSB_MANAGER_ATTR(_name, _mode, _show, _store) \
	struct mdsb_mgr_attribute mdsb_manager_attr_##_name = \
	__ATTR(_name, _mode, _show, _store)	
	
static ssize_t lcd_vr_hr_drop_show(struct mdsb_manager * layer_mgr, char *buf)
{
	int offset = 0;
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_LCD_HR = %x\n",act_readl(MDSB_LCD_HR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_LCD_VR = %x\n",act_readl(MDSB_LCD_VR));
	offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_LCD_OFF = %x\n",act_readl(MDSB_LCD_OFF));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_DELAY0 = %x\n",act_readl(MDSB_DELAY0));
  offset += snprintf(&buf[offset], PAGE_SIZE - offset, "MDSB_DELAY1 = %x\n",act_readl(MDSB_DELAY1));
  return offset;
}

static ssize_t lcd_vr_hr_drop_store(struct  mdsb_manager * mdsb_mgr, const char *buf, size_t count) {
	char *last = NULL;

	mdsb_mgr->debug_mode = simple_strtoul(buf, &last, 0);
	last++;		
	if (last - buf >= count)
		return -EINVAL;	
				
	mdsb_mgr->lcd_vr = simple_strtoul(buf, &last, 0);
	last++;		
	if (last - buf >= count)
		return -EINVAL;	
					
	mdsb_mgr->lcd_hr = simple_strtoul(last, &last, 0);	
	
	last++;		
	if (last - buf >= count)
		return -EINVAL;	
					
	mdsb_mgr->lcd_drop = simple_strtoul(last, &last, 0);
	
	
  last++;	
	if (last - buf >= count)
		return -EINVAL;	
					
	mdsb_mgr->delay0 = simple_strtoul(last, &last, 0);
			
	DEDBG("~~~~~~~~lcd_hr = %d,lcd_vr = %d,lcd_drop = %d,delay = %d\n",mdsb_mgr->lcd_hr,mdsb_mgr->lcd_vr,mdsb_mgr->lcd_drop,mdsb_mgr->delay0);
	return count;		
}
static ssize_t mdsb_manager_name_show(struct mdsb_manager * mdsb_mgr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", "mdbs_mgr");
}


static MDSB_MANAGER_ATTR(name, S_IRUGO, 
                   mdsb_manager_name_show,
                   NULL);
	
static MDSB_MANAGER_ATTR(lcd_mdsb_info, S_IRUGO|S_IWUSR,
										lcd_vr_hr_drop_show,
										lcd_vr_hr_drop_store);			
																	                

static struct attribute * mdsb_mgr_sysfs_attrs[] = {
	&mdsb_manager_attr_name.attr,
	&mdsb_manager_attr_lcd_mdsb_info.attr,
	NULL
};

static ssize_t mdsb_mgr_attr_show(struct kobject *kobj, struct attribute *attr,
		char *buf)
{
	struct mdsb_manager * mdsb_mgr;
	struct mdsb_mgr_attribute * mdsb_mgr_attr;

	mdsb_mgr = container_of(kobj, struct mdsb_manager, kobj);
	mdsb_mgr_attr = container_of(attr, struct mdsb_mgr_attribute, attr);
	
	if (!mdsb_mgr_attr->show)
		return -ENOENT;
		
	return mdsb_mgr_attr->show(mdsb_mgr, buf);
}

static ssize_t mdsb_mgr_attr_store(struct kobject *kobj, struct attribute *attr,
		const char *buf, size_t size)
{
	struct mdsb_manager * mdsb_mgr;
	struct mdsb_mgr_attribute * mdsb_mgr_attr;

	mdsb_mgr = container_of(kobj, struct mdsb_manager, kobj);
	mdsb_mgr_attr = container_of(attr, struct mdsb_mgr_attribute, attr);

	if (!mdsb_mgr_attr->store)
		return -ENOENT;

	return mdsb_mgr_attr->store(mdsb_mgr, buf, size);
}

static const struct sysfs_ops mdsb_mgr_sysfs_ops = {
	.show = mdsb_mgr_attr_show,
	.store = mdsb_mgr_attr_store,
};

static struct kobj_type mdsb_mgr_ktype = {
	.sysfs_ops = &mdsb_mgr_sysfs_ops,
	.default_attrs = mdsb_mgr_sysfs_attrs,
};

int mdsb_mgr_attr_init(struct mdsb_manager * mdsb_mgr, struct de_core * core){
	
  int r = 0;

	r = kobject_init_and_add(&mdsb_mgr->kobj, &mdsb_mgr_ktype,
				&core->dev->kobj, "mdsb_mgr");				
	if (r)
			printk("failed to create sysfs file for mdsb mgr\n");				
	
	return r;
}

/*****************************************************************************	
*de vsync attr 
****************************************************************************/	
static ssize_t de_vsync_show_status_attrs(struct device *dev,
					      struct device_attribute *attr,
					      char *buf) {
	struct de_core * core = (struct de_core *)dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", core->v_vsync.state);
}
static ssize_t de_vsync_show_cnt_attrs(struct device *dev,
					      struct device_attribute *attr,
					      char *buf) {
	struct de_core * core = (struct de_core *)dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", core->v_vsync.counter);
}

static struct device_attribute de_vsync_attrs[] = {
	__ATTR(status, S_IRUGO, de_vsync_show_status_attrs, NULL),
	__ATTR(vsync_cnt, S_IRUGO, de_vsync_show_cnt_attrs, NULL),
};

int de_vsync_create_attrs(struct device *dev)
{
	int rc = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(de_vsync_attrs); i++) {
		rc = device_create_file(dev,
			    &de_vsync_attrs[i]);
		if (rc)
			goto attrs_create_failed;
	}	
	goto succeed;

attrs_create_failed:
	while (i--)
		device_remove_file(dev,
			   &de_vsync_attrs[i]);
succeed:
	return rc;
}

void vsync_set_state(struct de_core * core, int state){

	char state_buf[120];
	char *prop_buf;
	char *envp[3];
	int env_offset = 0;
	int length;

	if (core->v_vsync.state != state) {
		core->v_vsync.state = state;
		
		prop_buf = (char *)get_zeroed_page(GFP_KERNEL);
		if (prop_buf) {
			length = de_vsync_show_status_attrs(core->dev, NULL, prop_buf);
			if (length > 0) {
				if (prop_buf[length - 1] == '\n')
					prop_buf[length - 1] = 0;
				snprintf(state_buf, sizeof(state_buf),
					"VSYNC_STATE=%s", prop_buf);
				envp[env_offset++] = state_buf;
			}
			envp[env_offset] = NULL;
			kobject_uevent_env(&core->dev->kobj, KOBJ_CHANGE, envp);
			free_page((unsigned long)prop_buf);
		} else {
			printk(KERN_ERR "out of memory in switch_set_state\n");
			kobject_uevent(&core->dev->kobj, KOBJ_CHANGE);
		}
	}
}
static char prop_buf[1204];
void vsync_set_counter(struct de_core * core, int counter){
	char cnt_buf[120];
	//char *prop_buf;
	char *envp[3];
	int env_offset = 0;
	int length;
	if (core->v_vsync.counter != counter) {
		core->v_vsync.counter = counter;
		//prop_buf = (char *)get_zeroed_page(GFP_KERNEL);
		if (prop_buf) {
			length = de_vsync_show_cnt_attrs(core->dev, NULL, &prop_buf);
			if (length > 0) {
				if (prop_buf[length - 1] == '\n')
					prop_buf[length - 1] = 0;
				snprintf(cnt_buf, sizeof(cnt_buf),
					"VSYNC_COUNTER=%s", prop_buf);
				envp[env_offset++] = cnt_buf;
			}
			envp[env_offset] = NULL;
			kobject_uevent_env(&core->dev->kobj, KOBJ_CHANGE, envp);
			//DEDBG("vsync_set_counter counter = %d core->v_vsync.counter = %d\n",counter,core->v_vsync.counter);
			//free_page((unsigned long)prop_buf);
		} else {
			printk(KERN_ERR "out of memory in switch_set_state\n");
			kobject_uevent(&core->dev->kobj, KOBJ_CHANGE);
		}
	}
}