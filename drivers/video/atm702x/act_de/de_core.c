/***************************************************************************
 *                              GLBASE
 *                            Module: DE driver
 *                 Copyright(c) 2011-2015 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       wanghui     2011-08-19 9:00     1.0             build this file
 ***************************************************************************/
#include <linux/seq_file.h>
#include <linux/debugfs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>

#include "clock.h"
#include "de.h"
#include "de_core.h"

void print_debug_info(void) {
#if 0 
	printk(KERN_INFO "de status information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printk(KERN_INFO "CMU_DISPLAYPLL =%x\n", act_readl(CMU_DISPLAYPLL)); 
	printk(KERN_INFO "CMU_DEVPLL =%x\n", act_readl(CMU_DEVPLL));
	printk(KERN_INFO "CMU_DECLK =%x\n", act_readl(CMU_DECLK));
	printk(KERN_INFO "DE_INTEN =%x\n", act_readl(DE_INTEN));
	printk(KERN_INFO "DE_STAT =%x\n", act_readl(DE_STAT));    
	printk(KERN_INFO "\n\n");   
	
	printk(KERN_INFO "path1 information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 

	printk(KERN_INFO "PATH1_CTL =%x\n", act_readl(PATH1_CTL)); 
	printk(KERN_INFO "PATH1_FCR =%x\n", act_readl(PATH1_FCR)); 
	printk(KERN_INFO "PATH1_EN =%x\n", act_readl(PATH1_EN)); 
	printk(KERN_INFO "PATH1_BK =%x\n", act_readl(PATH1_BK));
	printk(KERN_INFO "PATH1_SIZE =%x\n", act_readl(PATH1_SIZE));
	printk(KERN_INFO "PATH1_A_COOR=%x\n", act_readl(PATH1_A_COOR));
	printk(KERN_INFO "PATH1_B_COOR =%x\n", act_readl(PATH1_B_COOR));
	printk(KERN_INFO "PATH1_ALPHA_CFG =%x\n", act_readl(PATH1_ALPHA_CFG)); 	
	printk(KERN_INFO "PATH1_CKMAX =%x\n", act_readl(PATH1_CKMAX));
	printk(KERN_INFO "PATH1_CKMIN =%x\n", act_readl(PATH1_CKMIN));
	printk(KERN_INFO "PATH1_GAMMA1_IDX=%x\n", act_readl(PATH1_GAMMA1_IDX));
	printk(KERN_INFO "PATH1_GAMMA1_RAM =%x\n", act_readl(PATH1_GAMMA1_RAM));  	
	printk(KERN_INFO "PATH1_GAMMA2_IDX =%x\n", act_readl(PATH1_GAMMA2_IDX));
	printk(KERN_INFO "PATH1_GAMMA2_RAM =%x\n", act_readl(PATH1_GAMMA2_RAM)); 
	printk(KERN_INFO "\n\n");  	
	printk(KERN_INFO "path2 information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 

	printk(KERN_INFO "PATH2_CTL =%x\n", act_readl(PATH2_CTL)); 
	printk(KERN_INFO "PATH2_FCR =%x\n", act_readl(PATH2_FCR)); 
	printk(KERN_INFO "PATH2_EN =%x\n", act_readl(PATH2_EN)); 
	printk(KERN_INFO "PATH2_BK =%x\n", act_readl(PATH2_BK));
	printk(KERN_INFO "PATH2_SIZE =%x\n", act_readl(PATH2_SIZE));
	printk(KERN_INFO "PATH2_A_COOR=%x\n", act_readl(PATH2_A_COOR));
	printk(KERN_INFO "PATH2_B_COOR =%x\n", act_readl(PATH2_B_COOR));
	printk(KERN_INFO "PATH2_ALPHA_CFG =%x\n", act_readl(PATH2_ALPHA_CFG)); 	
	printk(KERN_INFO "PATH2_CKMAX =%x\n", act_readl(PATH2_CKMAX));
	printk(KERN_INFO "PATH2_CKMIN =%x\n", act_readl(PATH2_CKMIN));
	printk(KERN_INFO "PATH2_GAMMA1_IDX=%x\n", act_readl(PATH2_GAMMA1_IDX));
	printk(KERN_INFO "PATH2_GAMMA1_RAM =%x\n", act_readl(PATH2_GAMMA1_RAM));  	
	printk(KERN_INFO "PATH2_GAMMA2_IDX =%x\n", act_readl(PATH2_GAMMA2_IDX));
	printk(KERN_INFO "PATH2_GAMMA2_RAM =%x\n", act_readl(PATH2_GAMMA2_RAM)); 
	printk(KERN_INFO "\n\n"); 
	 
	printk(KERN_INFO "graphic layer information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 

	printk(KERN_INFO "GRAPHIC_BASE =%x\n", act_readl(GRAPHIC_BASE));
	printk(KERN_INFO "GRAPHIC_CFG=%x\n", act_readl(GRAPHIC_CFG));
	printk(KERN_INFO "GRAPHIC_SIZE =%x\n", act_readl(GRAPHIC_SIZE));
	printk(KERN_INFO "GRAPHIC_FB =%x\n", act_readl(GRAPHIC_FB));
	printk(KERN_INFO "GRAPHIC_STR =%x\n", act_readl(GRAPHIC_STR)); 
	printk(KERN_INFO "\n\n");    
	
	printk(KERN_INFO "video layer 1  information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 

	printk(KERN_INFO "VIDEO1_BASE =%x\n", act_readl(VIDEO1_BASE));
	printk(KERN_INFO "VIDEO1_CFG =%x\n", act_readl(VIDEO1_CFG));
	printk(KERN_INFO "VIDEO1_ISIZE = 0x%x\n", act_readl(VIDEO1_ISIZE));
	printk(KERN_INFO "VIDEO1_OSIZE = 0x%x\n", act_readl(VIDEO1_OSIZE));
	printk(KERN_INFO "VIDEO1_SR = 0x%x\n", act_readl(VIDEO1_SR));
	printk(KERN_INFO "VIDEO1_SCOEF0 =%x\n", act_readl(VIDEO1_SCOEF0));
	printk(KERN_INFO "VIDEO1_SCOEF1 =%x\n", act_readl(VIDEO1_SCOEF1)); 
	printk(KERN_INFO "VIDEO1_SCOEF2 =%x\n", act_readl(VIDEO1_SCOEF2));
	printk(KERN_INFO "VIDEO1_SCOEF3 =%x\n", act_readl(VIDEO1_SCOEF3));
	printk(KERN_INFO "VIDEO1_SCOEF4 =%x\n", act_readl(VIDEO1_SCOEF4));
	printk(KERN_INFO "VIDEO1_SCOEF5 =%x\n", act_readl(VIDEO1_SCOEF5)); 
	printk(KERN_INFO "VIDEO1_SCOEF6 =%x\n", act_readl(VIDEO1_SCOEF6));
	printk(KERN_INFO "VIDEO1_SCOEF7 =%x\n", act_readl(VIDEO1_SCOEF7)); 
	printk(KERN_INFO "VIDEO1_FB_0 =%x\n", act_readl(VIDEO1_FB_0)); 
	printk(KERN_INFO "VIDEO1_FB_1 =%x\n", act_readl(VIDEO1_FB_1));
	printk(KERN_INFO "VIDEO1_FB_2 =%x\n", act_readl(VIDEO1_FB_2));  	
	printk(KERN_INFO "VIDEO1_FB_RIGHT_0 =%x\n", act_readl(VIDEO1_FB_RIGHT_0)); 
	printk(KERN_INFO "VIDEO1_FB_RIGHT_1 =%x\n", act_readl(VIDEO1_FB_RIGHT_1));
	printk(KERN_INFO "VIDEO1_FB_RIGHT_2 =%x\n", act_readl(VIDEO1_FB_RIGHT_2));  	
	printk(KERN_INFO "VIDEO1_STR =%x\n", act_readl(VIDEO1_STR)); 
	printk(KERN_INFO "VIDEO1_CRITICAL =%x\n", act_readl(VIDEO1_CRITICAL));
	printk(KERN_INFO "VIDEO1_REMAPPING =%x\n", act_readl(VIDEO1_REMAPPING)); 
	printk(KERN_INFO "\n\n"); 
  printk(KERN_INFO "video layer 2  information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 

	printk(KERN_INFO "VIDEO2_BASE =%x\n", act_readl(VIDEO2_BASE));
	printk(KERN_INFO "VIDEO2_CFG =%x\n", act_readl(VIDEO2_CFG));
	printk(KERN_INFO "VIDEO2_ISIZE = 0x%x\n", act_readl(VIDEO2_ISIZE));
	printk(KERN_INFO "VIDEO2_OSIZE = 0x%x\n", act_readl(VIDEO2_OSIZE));
	printk(KERN_INFO "VIDEO2_SR = 0x%x\n", act_readl(VIDEO2_SR));
	printk(KERN_INFO "VIDEO2_SCOEF0 =%x\n", act_readl(VIDEO2_SCOEF0));
	printk(KERN_INFO "VIDEO2_SCOEF1 =%x\n", act_readl(VIDEO2_SCOEF1)); 
	printk(KERN_INFO "VIDEO2_SCOEF2 =%x\n", act_readl(VIDEO2_SCOEF2));
	printk(KERN_INFO "VIDEO2_SCOEF3 =%x\n", act_readl(VIDEO2_SCOEF3));
	printk(KERN_INFO "VIDEO2_SCOEF4 =%x\n", act_readl(VIDEO2_SCOEF4));
	printk(KERN_INFO "VIDEO2_SCOEF5 =%x\n", act_readl(VIDEO2_SCOEF5)); 
	printk(KERN_INFO "VIDEO2_SCOEF6 =%x\n", act_readl(VIDEO2_SCOEF6));
	printk(KERN_INFO "VIDEO2_SCOEF7 =%x\n", act_readl(VIDEO2_SCOEF7)); 
	printk(KERN_INFO "VIDEO2_FB_0 =%x\n", act_readl(VIDEO2_FB_0)); 
	printk(KERN_INFO "VIDEO2_FB_1 =%x\n", act_readl(VIDEO2_FB_1));
	printk(KERN_INFO "VIDEO2_FB_2 =%x\n", act_readl(VIDEO2_FB_2));  	
	printk(KERN_INFO "VIDEO2_FB_RIGHT_0 =%x\n", act_readl(VIDEO2_FB_RIGHT_0)); 
	printk(KERN_INFO "VIDEO2_FB_RIGHT_1 =%x\n", act_readl(VIDEO2_FB_RIGHT_1));
	printk(KERN_INFO "VIDEO2_FB_RIGHT_2 =%x\n", act_readl(VIDEO2_FB_RIGHT_2));  	
	printk(KERN_INFO "VIDEO2_STR =%x\n", act_readl(VIDEO2_STR)); 
	printk(KERN_INFO "VIDEO2_CRITICAL =%x\n", act_readl(VIDEO2_CRITICAL));
	printk(KERN_INFO "VIDEO2_REMAPPING =%x\n", act_readl(VIDEO2_REMAPPING)); 
	printk(KERN_INFO "\n\n");
	printk(KERN_INFO "video layer 3 information  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
	printk(KERN_INFO "VIDEO3_BASE =%x\n", act_readl(VIDEO3_BASE));
	printk(KERN_INFO "VIDEO3_CFG =%x\n", act_readl(VIDEO3_CFG));
	printk(KERN_INFO "VIDEO3_ISIZE = 0x%x\n", act_readl(VIDEO3_ISIZE));
	printk(KERN_INFO "VIDEO3_OSIZE = 0x%x\n", act_readl(VIDEO3_OSIZE));
	printk(KERN_INFO "VIDEO3_SR = 0x%x\n", act_readl(VIDEO3_SR));
	printk(KERN_INFO "VIDEO3_SCOEF0 =%x\n", act_readl(VIDEO3_SCOEF0));
	printk(KERN_INFO "VIDEO3_SCOEF1 =%x\n", act_readl(VIDEO3_SCOEF1)); 
	printk(KERN_INFO "VIDEO3_SCOEF2 =%x\n", act_readl(VIDEO3_SCOEF2));
	printk(KERN_INFO "VIDEO3_SCOEF3 =%x\n", act_readl(VIDEO3_SCOEF3));
	printk(KERN_INFO "VIDEO3_SCOEF4 =%x\n", act_readl(VIDEO3_SCOEF4));
	printk(KERN_INFO "VIDEO3_SCOEF5 =%x\n", act_readl(VIDEO3_SCOEF5)); 
	printk(KERN_INFO "VIDEO3_SCOEF6 =%x\n", act_readl(VIDEO3_SCOEF6));
	printk(KERN_INFO "VIDEO3_SCOEF7 =%x\n", act_readl(VIDEO3_SCOEF7)); 
	printk(KERN_INFO "VIDEO3_FB_0 =%x\n", act_readl(VIDEO3_FB_0)); 
	printk(KERN_INFO "VIDEO3_FB_1 =%x\n", act_readl(VIDEO3_FB_1));
	printk(KERN_INFO "VIDEO3_FB_2 =%x\n", act_readl(VIDEO3_FB_2));  	
	printk(KERN_INFO "VIDEO3_FB_RIGHT_0 =%x\n", act_readl(VIDEO3_FB_RIGHT_0)); 
	printk(KERN_INFO "VIDEO3_FB_RIGHT_1 =%x\n", act_readl(VIDEO3_FB_RIGHT_1));
	printk(KERN_INFO "VIDEO3_FB_RIGHT_2 =%x\n", act_readl(VIDEO3_FB_RIGHT_2));  	
	printk(KERN_INFO "VIDEO3_STR =%x\n", act_readl(VIDEO3_STR)); 
	printk(KERN_INFO "VIDEO3_CRITICAL =%x\n", act_readl(VIDEO3_CRITICAL));
	printk(KERN_INFO "VIDEO3_REMAPPING =%x\n", act_readl(VIDEO3_REMAPPING)); 
	printk(KERN_INFO "\n\n");  
	
	printk(KERN_INFO " mdsb infomations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");   
	printk(KERN_INFO "MDSB_CTL =%x\n", act_readl(MDSB_CTL));
	printk(KERN_INFO "MDSB_CVBS_HR =%x\n", act_readl(MDSB_CVBS_HR));
	printk(KERN_INFO "MDSB_CVBS_VR = 0x%x\n", act_readl(MDSB_CVBS_VR));
	printk(KERN_INFO "MDSB_LCD_HR = 0x%x\n", act_readl(MDSB_LCD_HR));
	printk(KERN_INFO "MDSB_LCD_VR = 0x%x\n", act_readl(MDSB_LCD_VR));
	printk(KERN_INFO "MDSB_STSEQ =%x\n", act_readl(MDSB_STSEQ));
	printk(KERN_INFO "MDSB_DELAY0 =%x\n", act_readl(MDSB_DELAY0)); 
	printk(KERN_INFO "MDSB_DELAY1 =%x\n", act_readl(MDSB_DELAY1));
	printk(KERN_INFO "MDSB_CVBS_OFF =%x\n", act_readl(MDSB_CVBS_OFF));
	printk(KERN_INFO "MDSB_LCD_OFF =%x\n", act_readl(MDSB_LCD_OFF));
	printk(KERN_INFO "MDSB_HDMIWH =%x\n", act_readl(MDSB_HDMIWH)); 
	printk(KERN_INFO "MDSB_STAT =%x\n", act_readl(MDSB_STAT));
	printk(KERN_INFO "MDSB_HSCALER0 =%x\n", act_readl(MDSB_HSCALER0)); 
	printk(KERN_INFO "MDSB_HSCALER1 =%x\n", act_readl(MDSB_HSCALER1)); 
	printk(KERN_INFO "MDSB_HSCALER2 =%x\n", act_readl(MDSB_HSCALER2));
	printk(KERN_INFO "MDSB_HSCALER3 =%x\n", act_readl(MDSB_HSCALER3));  	
	printk(KERN_INFO "MDSB_HSCALER4 =%x\n", act_readl(MDSB_HSCALER4)); 
	printk(KERN_INFO "MDSB_HSCALER5 =%x\n", act_readl(MDSB_HSCALER5));
	printk(KERN_INFO "MDSB_HSCALER6 =%x\n", act_readl(MDSB_HSCALER6));  	
	printk(KERN_INFO "MDSB_HSCALER7 =%x\n", act_readl(MDSB_HSCALER7)); 
	printk(KERN_INFO "MDSB_VSCALER0 =%x\n", act_readl(MDSB_VSCALER0));
	printk(KERN_INFO "MDSB_VSCALER1 =%x\n", act_readl(MDSB_VSCALER1));  
	printk(KERN_INFO "MDSB_VSCALER2 =%x\n", act_readl(MDSB_VSCALER2)); 
	printk(KERN_INFO "MDSB_VSCALER3 =%x\n", act_readl(MDSB_VSCALER3)); 
	printk(KERN_INFO "MDSB_VSCALER4 =%x\n", act_readl(MDSB_VSCALER4));
	printk(KERN_INFO "MDSB_VSCALER5 =%x\n", act_readl(MDSB_VSCALER5));  	
	printk(KERN_INFO "MDSB_VSCALER6 =%x\n", act_readl(MDSB_VSCALER6)); 
	printk(KERN_INFO "MDSB_VSCALER7 =%x\n", act_readl(MDSB_VSCALER7));
	printk(KERN_INFO "MDSB_WBCFG =%x\n", act_readl(MDSB_WBCFG));  	
	printk(KERN_INFO "MDSB_WBADDR =%x\n", act_readl(MDSB_WBADDR)); 
	printk(KERN_INFO "MDSB_WBLEN =%x\n", act_readl(MDSB_WBLEN)); 
	printk(KERN_INFO "\n\n");  
	printk(KERN_INFO" LCD infomations  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");  
	printk(KERN_INFO "CMU_LCDCLK =%x\n", act_readl(CMU_LCDCLK)); 
	printk(KERN_INFO "LCD0_CTL =%x\n", act_readl(LCD0_CTL)); 
	printk(KERN_INFO "LCD0_SIZE =%x\n", act_readl(LCD0_SIZE)); 
	printk(KERN_INFO "LCD0_STATUS =%x\n", act_readl(LCD0_STATUS)); 
	printk(KERN_INFO "LCD0_TIM0 =%x\n", act_readl(LCD0_TIM0)); 
	printk(KERN_INFO "LCD0_TIM1 =%x\n", act_readl(LCD0_TIM1)); 
	printk(KERN_INFO "LCD0_TIM2 =%x\n", act_readl(LCD0_TIM2)); 
	printk(KERN_INFO "LCD0_COLOR =%x\n", act_readl(LCD0_COLOR)); 
	printk(KERN_INFO "LCD0_CPU_CTL =%x\n", act_readl(LCD0_CPU_CTL)); 
	printk(KERN_INFO "LCD0_CPU_CMD =%x\n", act_readl(LCD0_CPU_CMD)); 
	printk(KERN_INFO "LCD0_TEST_P0 =%x\n", act_readl(LCD0_TEST_P0)); 
	printk(KERN_INFO "LCD0_TEST_P1 =%x\n", act_readl(LCD0_TEST_P1)); 
	printk(KERN_INFO "LCD0_IMG_XPOS =%x\n", act_readl(LCD0_IMG_XPOS)); 
	printk(KERN_INFO "LCD0_IMG_YPOS =%x\n", act_readl(LCD0_IMG_YPOS)); 
		
	printk(KERN_INFO "LCD1_CTL =%x\n", act_readl(LCD1_CTL)); 
	printk(KERN_INFO "LCD1_SIZE =%x\n", act_readl(LCD1_SIZE)); 
	printk(KERN_INFO "LCD1_TIM1 =%x\n", act_readl(LCD1_TIM1)); 
	printk(KERN_INFO "LCD1_TIM2 =%x\n", act_readl(LCD1_TIM2)); 
	printk(KERN_INFO "LCD1_COLOR =%x\n", act_readl(LCD1_COLOR)); 
	printk(KERN_INFO "LCD1_CPU_CTL =%x\n", act_readl(LCD1_CPU_CTL)); 
	printk(KERN_INFO "LCD1_CPU_CMD =%x\n", act_readl(LCD1_CPU_CMD)); 
	printk(KERN_INFO "LCD1_IMG_XPOS =%x\n", act_readl(LCD1_IMG_XPOS)); 
	printk(KERN_INFO "LCD1_IMG_YPOS =%x\n", act_readl(LCD1_IMG_YPOS)); 
#if 0
	printk(KERN_INFO  " tvout infomations  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
		
	printk(KERN_INFO "CMU_TVOUTPLL =%x\n", act_readl(CMU_TVOUTPLL)); 
	printk(KERN_INFO "TVOUT_EN =%x\n", act_readl(TVOUT_EN)); 
				  
	printk(KERN_INFO "TVOUT_EN =%x\n", act_readl(TVOUT_EN)); 
	printk(KERN_INFO "TVOUT_OCR =%x\n", act_readl(TVOUT_OCR)); 
	printk(KERN_INFO "TVOUT_STA =%x\n", act_readl(TVOUT_STA)); 
	printk(KERN_INFO "TVOUT_CCR =%x\n", act_readl(TVOUT_CCR)); 
	printk(KERN_INFO "TVOUT_BCR =%x\n", act_readl(TVOUT_BCR)); 
	printk(KERN_INFO "TVOUT_CSCR =%x\n", act_readl(TVOUT_CSCR)); 
	printk(KERN_INFO "TVOUT_PRL =%x\n", act_readl(TVOUT_PRL)); 
	printk(KERN_INFO "TVOUT_VFALD =%x\n", act_readl(TVOUT_VFALD)); 
	printk(KERN_INFO "CVBS_MSR =%x\n", act_readl(CVBS_MSR)); 
	printk(KERN_INFO "CVBS_AL_SEPO =%x\n", act_readl(CVBS_AL_SEPO)); 
	printk(KERN_INFO "CVBS_AL_SEPE =%x\n", act_readl(CVBS_AL_SEPE)); 
	printk(KERN_INFO "CVBS_AD_SEP =%x\n", act_readl(CVBS_AD_SEP)); 
	printk(KERN_INFO "CVBS_HUECR =%x\n", act_readl(CVBS_HUECR)); 	
	printk(KERN_INFO "CVBS_SCPCR =%x\n", act_readl(CVBS_SCPCR)); 
	printk(KERN_INFO "CVBS_SCFCR =%x\n", act_readl(CVBS_SCFCR)); 
	printk(KERN_INFO "CVBS_CBACR =%x\n", act_readl(CVBS_CBACR)); 
	printk(KERN_INFO "CVBS_SACR =%x\n", act_readl(CVBS_SACR)); 
	printk(KERN_INFO "BT_MSR0 =%x\n", act_readl(BT_MSR0)); 
	printk(KERN_INFO "BT_MSR1 =%x\n", act_readl(BT_MSR1)); 
	printk(KERN_INFO "BT_AL_SEPO =%x\n", act_readl(BT_AL_SEPO)); 
	printk(KERN_INFO "BT_AL_SEPE =%x\n", act_readl(BT_AL_SEPE)); 
	printk(KERN_INFO "BT_AP_SEP =%x\n", act_readl(BT_AP_SEP)); 
	printk(KERN_INFO "TVOUT_DCR =%x\n", act_readl(TVOUT_DCR)); 
	printk(KERN_INFO "TVOUT_DDCR =%x\n", act_readl(TVOUT_DDCR)); 
	printk(KERN_INFO "TVOUT_DCORCTL =%x\n", act_readl(TVOUT_DCORCTL));
	printk(KERN_INFO "TVOUT_DRCR =%x\n", act_readl(TVOUT_DRCR)); 
#endif
                         
#endif
}
EXPORT_SYMBOL(print_debug_info);

void capture_image(void){
	 /*	struct file *filp;
		mm_segment_t fs;
		unsigned int paddr;
		void __iomem *vaddr;
		unsigned long page;
		unsigned map_size = PAGE_ALIGN(1280 * 720);	
		vaddr = dma_alloc_coherent(NULL, map_size, &paddr, GFP_KERNEL);
		if (!vaddr) {
			printk(KERN_ERR "fail to allocate ddr ram error )\n", map_size / 1024);
			return -ENOMEM;
		}
		memset(vaddr, 0x00, map_size);
		page = (unsigned long) paddr;
		SetPageReserved(pfn_to_page(page >> PAGE_SHIFT));		

		act_writel(paddr,MDSB_WBADDR);
		act_writel(map_size,MDSB_WBLEN);
		act_writel(0X101,MDSB_WBCFG);		
		
		filp = filp_open("/mnt/sdcard/power_info_log.txt", O_CREAT | O_RDWR, 0644);

    	if (IS_ERR(filp)) {
    		printk("<0>""can't accessed sd card power_info_log, exiting.../n");
			return 0;
		}

		fs = get_fs();
		set_fs(KERNEL_DS);	
		filp->f_op->llseek(filp, 0, SEEK_END);
		filp->f_op->write(filp, (char *)vaddr, map_size, &filp->f_pos);
		set_fs(fs);
		filp_close(filp, NULL);*/
}


static void video_sclcoef_config(unsigned int flag_sclc, int VIDEO_BASE) {
	if (((flag_sclc != DE_SCLCOEF_NORMAL) && (flag_sclc != DE_SCLCOEF_DELHIGH))) {
		printk("de flag error %x %s \n", __LINE__, __FILE__);
		return;
	} 
	if (flag_sclc == DE_SCLCOEF_NORMAL) {
		act_writel(0X00004000, VIDEO_BASE + VIDEO_OFF_SCOEF0);
		act_writel(0xFF073EFC, VIDEO_BASE + VIDEO_OFF_SCOEF1);
		act_writel(0xFE1038FA, VIDEO_BASE + VIDEO_OFF_SCOEF2);
		act_writel(0xFC1B30F9, VIDEO_BASE + VIDEO_OFF_SCOEF3);
		act_writel(0xFA2626FA, VIDEO_BASE + VIDEO_OFF_SCOEF4);
		act_writel(0xF9301BFC, VIDEO_BASE + VIDEO_OFF_SCOEF5);
		act_writel(0xFA3810FE, VIDEO_BASE + VIDEO_OFF_SCOEF6);
		act_writel(0xFC3E07FF, VIDEO_BASE + VIDEO_OFF_SCOEF7);
	} else {
		act_writel(0X00102010, VIDEO_BASE + VIDEO_OFF_SCOEF0);
		act_writel(0X02121E0E, VIDEO_BASE + VIDEO_OFF_SCOEF1);
		act_writel(0X04141C0C, VIDEO_BASE + VIDEO_OFF_SCOEF2);
		act_writel(0X06161A0A, VIDEO_BASE + VIDEO_OFF_SCOEF3);
		act_writel(0X08181808, VIDEO_BASE + VIDEO_OFF_SCOEF4);
		act_writel(0X0A1A1606, VIDEO_BASE + VIDEO_OFF_SCOEF5);
		act_writel(0X0C1C1404, VIDEO_BASE + VIDEO_OFF_SCOEF6);
		act_writel(0X0E1E1202, VIDEO_BASE + VIDEO_OFF_SCOEF7);
	}
}

void change_critical_signal(unsigned int displayer_id) {
}

void de_hw_init() { 
	// init config path1 for de (display engine) 
	act_writel(PATHx_CTL_SELA(VIDEO2_LAYER) | PATHx_CTL_SELB(VIDEO3_LAYER), PATH2_CTL);
	act_writel(act_readl(PATH2_CTL) | (1 << 5) | (7 << 12), PATH2_CTL);
	// set black color
	act_writel(0x0, PATH2_BK);    
	// init config path2 for display engine
	act_writel(PATHx_CTL_SELA(GRAPHIC_LAYER) | PATHx_CTL_SELB(VIDEO1_LAYER), PATH1_CTL); 
	act_writel(act_readl(PATH1_CTL) | (1 << 5) | (7 << 12), PATH1_CTL);
	act_writel(0x0, PATH1_BK); 
}

void de_path_config(struct de_core *core) {
	int PATH_SEL = 0, i = 0;
	struct layer_manager *layer_mgr = NULL;

	for ( i = 0 ; i < MAX_NUM_LAYERS_MANAGER; i++) {	
		PATH_SEL = 0;	
		layer_mgr = &core->layer_mgr[i];
		PATH_SEL = PATHx_CTL_SELA(layer_mgr->layers[0]->layer_id);
		layer_mgr->layers[0]->layer_tag = DE_LAYER_A;
		PATH_SEL |= PATHx_CTL_SELB(layer_mgr->layers[1]->layer_id);		
		layer_mgr->layers[1]->layer_tag = DE_LAYER_B;
		if (layer_mgr->path_id == 0) {			
			act_writel(PATH_SEL, PATH1_CTL); 			
		} else {
			if (layer_mgr->disp_dev == NULL) {
				PATH_SEL = (act_readl(PATH2_CTL) & 0x00007020) | PATH_SEL;
				act_writel(PATH_SEL, PATH2_CTL);
			}
		}	
	}	
}

void de_clk_init(struct clk_manager *clk) {
	int ret = 0;
	struct clk * display_pll;
	struct clk * dev_clk;
	struct clk * de_clk;
	struct clk * de1_clk;
	struct clk * de2_clk;
	struct clk * de3_clk;
	unsigned long old_rate = 0, display_pll_rate = clk->clk_rate;
  
	if (clk->src_clk_type == DECLK_SRC_DISPLAY_PLL) {  		
  		display_pll = clk_get_sys(CLK_NAME_DISPLAYPLL, NULL);
    	old_rate = clk_get_rate(display_pll) / 1000; 
    	printk("displayer pll set %dMhz de clk is %dMhz \n", old_rate, display_pll_rate);
    	clk_enable(display_pll);
    	de_clk = clk_get_sys(CLK_NAME_DE_CLK, NULL);
		clk_set_parent(de_clk,display_pll);
	} else if (clk->src_clk_type == DECLK_SRC_DEVSCLK) {	
        dev_clk = clk_get_sys(CLK_NAME_DEVCLK, NULL); 
        old_rate = clk_get_rate(dev_clk)/ 1000; 
        clk_enable(dev_clk);
        de_clk = clk_get_sys(CLK_NAME_DE_CLK, NULL);          
        clk_set_parent(de_clk,dev_clk);
    }

	de1_clk = clk_get_sys(CLK_NAME_DE1_CLK, NULL);
    clk_set_rate(de1_clk,  clk->clk_rate * 1000);
    de2_clk = clk_get_sys(CLK_NAME_DE2_CLK, NULL);
    clk_set_rate(de2_clk, clk->clk_rate  * 1000);
	de3_clk = clk_get_sys(CLK_NAME_DE3_CLK, NULL);
	clk_set_rate(de3_clk,  clk->clk_rate * 1000); 
	clk_enable(de1_clk);
}

void de_set_clk_div(unsigned int src_clk_type, unsigned int de_clk_rate) {
	struct clk * de_clk;
	struct clk * dev_clk;
	struct clk * display_pll;
	struct clk * de1_clk;
	struct clk * de2_clk;
	struct clk * de3_clk;
	
	display_pll = clk_get_sys(CLK_NAME_DISPLAYPLL, NULL);
	dev_clk = clk_get_sys(CLK_NAME_DEVCLK, NULL);
    de_clk = clk_get_sys(CLK_NAME_DE_CLK, NULL);
	
	switch (src_clk_type) {
		case DECLK_SRC_DISPLAY_PLL:
			clk_set_parent(de_clk,display_pll);
			break;
		case DECLK_SRC_DEVSCLK:
			clk_set_parent(de_clk,dev_clk);
			break;
		default:
			BUG_ON(src_clk_type > 1);
	}
	de1_clk = clk_get_sys(CLK_NAME_DE1_CLK, NULL);
	clk_set_rate(de1_clk, de_clk_rate * 1000); 
	de2_clk = clk_get_sys(CLK_NAME_DE2_CLK, NULL);
	clk_set_rate(de2_clk, de_clk_rate * 1000); 
	de3_clk = clk_get_sys(CLK_NAME_DE3_CLK, NULL);
	clk_set_rate(de3_clk, de_clk_rate * 1000); 
}

int de_set_global_alpha(u8 global_alpha,bool enable) {
  //we keep the two paths in sync, the config is the same
	unsigned int alpha_value = act_readl(PATH1_ALPHA_CFG);
	unsigned int alpha_enable = act_readl(PATH1_CTL);
	if (enable) {
		alpha_value = 0x80;
		alpha_enable |= PATHx_CTL_ALPHA_ENABLE_A;			
	} else {
		alpha_enable &= (~PATHx_CTL_ALPHA_ENABLE_A);
	}

	act_writel(alpha_value, PATH1_ALPHA_CFG);
	act_writel(alpha_value, PATH2_ALPHA_CFG);
	act_writel(alpha_enable, PATH1_CTL);
	act_writel(alpha_enable, PATH2_CTL);

	return 0;
}

int de_set_color_key(int max,int min,int match,bool enable) {
	//we keep the two paths in sync, the config is the same
	unsigned int tmp_cfg = act_readl(PATH1_CTL);
	if (enable) {
		tmp_cfg |= PATHx_CTL_CK_ENABLE;
		act_writel(max,PATH1_CKMAX);	// set colorkey max color 
		act_writel(min, PATH1_CKMIN);	// set colorkey min color 
		act_writel(max,PATH2_CKMAX);	// set colorkey max color 
		act_writel(min, PATH2_CKMIN);	// set colorkey min color 
	} else {
		tmp_cfg &= (~PATHx_CTL_CK_ENABLE);
	}
	act_writel(tmp_cfg, PATH1_CTL);
	act_writel(tmp_cfg, PATH2_CTL);

	return 0;
}

void de_setout_channel(struct de_core * core) {
	struct layer_manager * l_manager = NULL;
	struct disp_manager * disp_mgr = &core->disp_mgr;			 
	unsigned int mdsb_ctl  = 0, path1_ctl = 0, path2_ctl = 0;
	int disp_id = disp_mgr->current_disp_ids;
	int interlace_en, path1_size = 0, path2_size = 0, interrupt_en = 0; 
	int hdmi_data_type = 0; 
	DEDBG("de_setout_channel called core %p disp_mgr %p\n",core,disp_mgr);
				
	path1_ctl = (act_readl(PATH1_CTL) & (~PATHx_CTL_INTERLACE_ENABLE));
	path2_ctl = (act_readl(PATH2_CTL) & (~PATHx_CTL_INTERLACE_ENABLE));

	switch (disp_mgr->disp_mode) {
		case DE_SINGLE_DISP:			
			DEDBG("de_setout_channel single disp dev\n");
			l_manager =  &core->layer_mgr[0];
			switch (disp_id) {
				case HDMI_DISPLAYER:			
					mdsb_ctl |= MDSB_CTL_IMG3_EN;
					mdsb_ctl |= (hdmi_data_type << 7);
					interrupt_en = 0x10;
					break;
				case TV_CVBS_DISPLAYER:	
					mdsb_ctl |= MDSB_CTL_IMG1_EN;
					interrupt_en = 0x08;	
					break;
				case TV_YPbPr_DISPLAYER:
					mdsb_ctl |= MDSB_CTL_IMG2_EN;
					mdsb_ctl |= MDSB_CTL_YPbPr_VGA_SEL;	
					interrupt_en = 0x20;
					break;
				case LCD_DISPLAYER:
					mdsb_ctl |= MDSB_CTL_IMG5_EN;
					act_writel((act_readl(LCD0_CTL) & (~(0x03 << 6))) | (0x01 << 6),LCD0_CTL);
					interrupt_en = 0x02;				
					break;
				default:
					printk("displayer %d not support !", disp_id);
					return;
			}
			interlace_en = ((l_manager->disp_dev->disp_cur_mode->vmode & FB_VMODE_INTERLACED) ? 1 : 0);

			if (disp_id == LCD_DISPLAYER) {
				path2_size |= (unsigned int)((l_manager->disp_dev->disp_cur_mode->yres - 1) << 16)
				           | (unsigned int)(l_manager->disp_dev->disp_cur_mode->xres - 1);
	  			if (interlace_en != 0) {
					path2_ctl |= PATHx_CTL_INTERLACE_ENABLE; //enable interlace
				} else {
					path2_ctl &= ~PATHx_CTL_INTERLACE_ENABLE; //enable interlace					
				}
				if (l_manager->disp_dev->dither_info) {
					path2_ctl |= l_manager->disp_dev->dither_info;		
				} else {
					path2_ctl &= ~PATHx_CTL_DITHER_ENABLE;		
				}
				act_writel(path2_ctl,PATH2_CTL);
				act_writel(path2_size, PATH2_SIZE);
				act_writel(0x1, PATH2_EN);
				act_writel(0x0, PATH1_EN); 
			} else {
				path1_size |= (unsigned int) ((l_manager->disp_dev->disp_cur_mode->yres - 1) << 16)
				           | (unsigned int) (l_manager->disp_dev->disp_cur_mode->xres - 1);
		  		if (interlace_en != 0) {
					path1_ctl |= PATHx_CTL_INTERLACE_ENABLE; //enable interlace
				} else {
					path1_ctl &= ~PATHx_CTL_INTERLACE_ENABLE; //enable interlace					
				}
				if ((l_manager->disp_dev->disp_cur_mode->flag & 0x01) != 0) {
					hdmi_data_type = 1;
				} else {
					hdmi_data_type = 0;
				}
				act_writel(path1_ctl,PATH1_CTL);
				act_writel(path1_size, PATH1_SIZE);
			  	act_writel(0x1, PATH1_EN);
			  	act_writel(0x0, PATH2_EN);
			}
			break;

		case DE_DOUBLE_DISP: 
			DEDBG("de_setout_channel DE_DOUBLE_DISP mode \n"); 
			l_manager =  &core->layer_mgr[0];
    		DEDBG("l_manager = %p l_manager->disp_dev %p, l_manager->disp_dev->disp_cur_mode = %p\n", l_manager, l_manager->disp_dev, l_manager->disp_dev->disp_cur_mode);
			path2_size |= (unsigned int) ((l_manager->disp_dev->disp_cur_mode->yres - 1) << 16)	| (unsigned int)(l_manager->disp_dev->disp_cur_mode->xres - 1);
			if (l_manager->disp_dev->dither_info) {
				path2_ctl |= l_manager->disp_dev->dither_info;		
			} else {
				path2_ctl &= ~PATHx_CTL_DITHER_ENABLE;		
			}			
			l_manager =  &core->layer_mgr[1];	
			interlace_en = ((l_manager->disp_dev->disp_cur_mode->vmode & FB_VMODE_INTERLACED) ? 1 : 0);
			path1_size |= (unsigned int) ((l_manager->disp_dev->disp_cur_mode->yres - 1) << 16)	| (unsigned int) (l_manager->disp_dev->disp_cur_mode->xres - 1);	
			if (l_manager->disp_dev->dither_info) {
				path1_ctl |= l_manager->disp_dev->dither_info;		
			} else {
				path1_ctl &= ~PATHx_CTL_DITHER_ENABLE;		
			}
			if ((l_manager->disp_dev->disp_cur_mode->flag & 0x01) != 0) {
				hdmi_data_type = 1;
			} else {
				hdmi_data_type = 0;
			}
			switch (disp_id) {
				case LCD_HDMI_DISPLAYER:
					mdsb_ctl |= MDSB_CTL_IMG3_EN;
					if (interlace_en != 0) {
						path1_ctl |= PATHx_CTL_INTERLACE_ENABLE; //enable interlacing
					} else {
						path1_ctl &= ~PATHx_CTL_INTERLACE_ENABLE; //enable interlacing
					}
					path1_ctl |= PATHx_CTL_CSC_CONTROL_BIT601;
					mdsb_ctl |= MDSB_CTL_IMG5_EN;
					mdsb_ctl |= (hdmi_data_type << 7);
					interrupt_en = 0x02;
					break;

				case LCD_CVBS_DISPLAYER:
					DEDBG("LCD_CVBS_DISPLAYER  \n");
					mdsb_ctl |= MDSB_CTL_IMG1_EN;
					path1_ctl |= PATHx_CTL_INTERLACE_ENABLE;
					path1_ctl |= PATHx_CTL_CSC_CONTROL_BIT601;
					mdsb_ctl |= MDSB_CTL_IMG5_EN;						
					interrupt_en = 0x02;
					break;

				case LCD_DOUBLE_DISPLAYER:
					mdsb_ctl |= MDSB_CTL_IMG4_EN;
			  		mdsb_ctl |= MDSB_CTL_IMG5_EN;
			  		act_writel((act_readl(LCD0_CTL) & (~(0x3 << 6))) | (0x00 << 6) | (1 << 28), LCD0_CTL);
			  		interrupt_en = 0x02;
					break;

				default:
           			printk("not support this mode \n");
					break;
			}

			struct clk * image5_clk = clk_get_sys(CLK_NAME_IMG5_CLK, NULL);
			struct clk * lcd0_clk = clk_get_sys(CLK_NAME_LCD0_CLK, NULL);
			clk_set_parent(image5_clk,lcd0_clk);
	  		act_writel(path1_ctl,PATH1_CTL);
			act_writel(path1_size, PATH1_SIZE);
	  		act_writel(0x1, PATH1_EN);
			break;

			case DE_SYNC_DISP:
				DEDBG("de_setout_channel mutiple disp dev\n");
				l_manager =  &core->layer_mgr[0];
				mdsb_ctl |= MDSB_CTL_IMG1_EN;
				mdsb_ctl |= MDSB_CTL_IMG3_EN;
				mdsb_ctl |= MDSB_CTL_IMG4_EN;
				mdsb_ctl |= MDSB_CTL_CVBS_SCALE;
				mdsb_ctl |= MDSB_CTL_LCD0_SCALE;
				path1_ctl |= PATHx_CTL_CSC_CONTROL_BIT601;
				interlace_en = ((l_manager->disp_dev->disp_cur_mode->vmode & FB_VMODE_INTERLACED) ? 1 : 0);				
				path1_size |= (unsigned int) ((l_manager->disp_dev->disp_cur_mode->yres - 1) << 16) | (unsigned int) (l_manager->disp_dev->disp_cur_mode->xres - 1);			
	
  				if (interlace_en != 0) {
					path1_ctl |= PATHx_CTL_INTERLACE_ENABLE; // enable interlace
				} else {
					path1_ctl &= ~PATHx_CTL_INTERLACE_ENABLE; // enable interlace				
				}
				act_writel((act_readl(LCD0_CTL) & (~(0x03 << 6))) | (0x02 << 6),LCD0_CTL);	
				interrupt_en = 0x10;		
				break;
	}
	act_writel(mdsb_ctl, MDSB_CTL);  
	act_writel(interrupt_en, DE_INTEN);
	
	return;
}

void de_set_background_color(struct rgb_color *bg_color) {
	int tmp = (bg_color->red << 16) | (bg_color->green << 8) | (bg_color->blue);
	act_writel(tmp, PATH1_BK);
	act_writel(tmp, PATH2_BK);

	return;
}

void de_reset_de(void) {
	struct clk * de_clk = clk_get_sys((const char *)CLK_NAME_DE1_CLK, NULL); /*根据clk_name获取clk结构体*/
	if (IS_ERR(de_clk)) {
        printk("DE Driver : clk_get_sys(CLK_NAME_DE_CLK, NULL) failed\n");
        return;
	} 
	clk_reset(de_clk);
}

bool de_is_init_boot(void) {
	return ((act_readl(PATH2_EN) | act_readl(PATH1_EN)) != 0); 
}

void de_disable(void) {
}

void de_enable(void) {
}

void de_modify_start(void) {
	act_writel(0x00 , PATH1_FCR);
	act_writel(0x00 , PATH2_FCR);
	DEDBG("PATH0_FCR = %x!\n", act_readl(PATH1_FCR));
	DEDBG("PATH1_FCR = %x!\n", act_readl(PATH2_FCR));
}

void de_modify_end(void) {
	act_writel(0x01 , PATH1_FCR);
	act_writel(0x01 , PATH2_FCR);
	DEDBG("PATH0_FCR = %x!\n", act_readl(PATH1_FCR));
	DEDBG("PATH1_FCR = %x!\n", act_readl(PATH2_FCR));
}

/***********************************************layer manager *****************/

void de_set_layer_status(int layerid, bool enable) {
}

int de_get_layer_status(int layerid) {
	return -0;
}

int de_graphics_layer_init(struct act_layer * layer) { 
	struct act_layer_info * info = layer->info;
	int tmp = 0 ;
	int init_cfg = act_readl(GRAPHIC_CFG);         
	init_cfg  |=  GRAPHIC_CFG_FMT(info->pixel_format);	
	init_cfg  |=  GRAPHIC_CFG_CRITICAL(CRITICAL_ALWAYS_ON);
	DEDBG("de_graphics_layer_init init_cfg = %x!\n", init_cfg);
	act_writel(init_cfg,GRAPHIC_CFG);
	DEDBG("de_graphics_layer_init act_readl(GRAPHIC_CFG) = %x! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", act_readl(GRAPHIC_CFG));
	tmp = (unsigned int)(((info->height - 1) << 16) | (info->width - 1)); 
	act_writel(tmp, GRAPHIC_SIZE);  
	
	return 0;
}

int de_graphics_layer_config(struct act_layer *layer) { 
	struct act_layer_info * info = layer->info;
	u16 img_width = info->img_width;
	u16 width = info->width;
	u16 height = info->height;
	u16 out_width = info->out_width;
	u16 out_height = info->out_height;
	if (!core.v_vsync.state) {
		return 0;
	}
	unsigned int tmp = 0, tmp_cfg = 0;
	if (width != out_width || height != out_height) {
		DEDBG("graphic layer config failed, not support sclae\n");
		return -EINVAL;
	}
	/*set the graphic size */
	tmp = (unsigned int) (((height - 1) << 16) | (width - 1));
	act_writel(tmp, GRAPHIC_SIZE);
	/*set positon of graphic layer */ 
	tmp = (unsigned int) (((info->pos_y) << 16) | (info->pos_x));
	act_writel(tmp, PATH1_A_COOR);		
	tmp_cfg = act_readl(GRAPHIC_CFG); 
	tmp_cfg  &=  ~GRAPHIC_CFG_FMT_MASK ;
	img_width = img_width / 2; 
	tmp_cfg |= GRAPHIC_CFG_FMT(info->pixel_format);

	switch(info->pixel_format) {
		case RGB_565:
			break;

		case ARGB_8888:
		case ABGR_8888:  
			img_width *= 2;
			break;

		default:
			printk(KERN_ERR "invalid pixel_format\n");
			break;		
	} 
	act_writel(tmp_cfg, GRAPHIC_CFG);	
	DEDBG("de_graphics_layer_config act_readl(GRAPHIC_CFG)= %x! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", act_readl(GRAPHIC_CFG));
	act_writel(img_width / 2 ,GRAPHIC_STR); //width *2/4	

	return 0;
}

int de_graphics_layer_update(struct act_layer *layer) {
	unsigned int temp;
	struct act_layer_info * info = layer->info;
	struct layer_manager *layer_mgr = layer->layer_mgr;
	int path_fcr = 0;
	int path_ctl = 0;

	if (!core.v_vsync.state) {
		return 0;
	}

	if (layer_mgr->path_id == DE_PATH_1) {
   		path_fcr = PATH1_FCR;
   		path_ctl = PATH1_CTL;
	} else if (layer_mgr->path_id == DE_PATH_2) {
  		path_fcr = PATH2_FCR;
  		path_ctl = PATH2_CTL;
	}
	if (!info->enabled) {
		DEDBG("graphic layer disable\n");
		temp = act_readl(path_ctl);
		if ((temp & 0x01) != 0) {
			act_writel((temp & (~0x01)), path_ctl); 
			act_writel(0x01,path_fcr); 
			printk("close graphic layer \n");	
		}

		return 0 ;
	}
	act_writel((unsigned int)info->paddr, GRAPHIC_FB);
	DEDBG("update layer addr info->paddr = %x\n", info->paddr);
	if (info->enabled) {		
		temp = act_readl(path_ctl);
		if ((temp & 0x01) != 0x01) {			
			act_writel(temp | 0x01, path_ctl);
			DEDBG("open graphic layer ~~~~ %x\n", act_readl(path_ctl));
		}
		act_writel(0x01, path_fcr);
	}

	return 0;
}

int de_video_layer_init(struct act_layer* layer) {
	struct act_layer_info * info = layer->info;
	int VIDEO_BASE = VIDEO1_BASE;  
	int init_cfg;
	VIDEO_BASE = layer->reg_base;
	video_sclcoef_config(DE_SCLCOEF_NORMAL,VIDEO_BASE);
	init_cfg =  act_readl(VIDEO_BASE + VIDEO_OFF_CFG);
	init_cfg  |=  VIDEO_CFG_FMT(info->pixel_format);
	init_cfg  |=  VIDEO_CFG_CRITICAL(CRITICAL_AUTO);
	act_writel(init_cfg,VIDEO_BASE + VIDEO_OFF_CFG);

	return 0;
}

int de_video_layer_config(struct act_layer *layer) {
	struct act_layer_info * info = layer->info;  
	struct layer_manager *layer_mgr = layer->layer_mgr;

	u16 img_width = info->img_width;
	u16 width = info->width;
	u16 height = info->height;
	u16 out_width = info->out_width;
	u16 out_height = info->out_height;
			
	unsigned int tmp = 0;
	unsigned int w_factor = 1;
	unsigned int h_factor = 1;
	unsigned int need_sub_sample = 0;
	unsigned int tmp_str,tmp_cfg;

	int layer_coor = 0;
	int VIDEO_BASE = layer->reg_base;
 	if (!core.v_vsync.state) {
		return 0;
	}
	if (layer_mgr->path_id == DE_PATH_1) {
  		if (layer->layer_tag == DE_LAYER_A) {
  			layer_coor = PATH1_A_COOR;
  		} else {
  			layer_coor = PATH1_B_COOR;
		}
	} else if (layer_mgr->path_id == DE_PATH_2) {
  		if (layer->layer_tag == DE_LAYER_A) {
  			layer_coor = PATH2_A_COOR;
  		} else {
  			layer_coor = PATH2_B_COOR;
  		}
	}
	DEDBG("de_video_layer_config called layer_id = %d \n",layer->layer_id);

	if ((out_width == 0) || (out_height == 0)) {
		printk("DE BUG out_width == 0 %s %d\n", __FILE__, __LINE__);
		return -1;
	}
	DEDBG("img_width =%d\n", img_width);
	//output pos in pix
	tmp = (info->pos_y << 16) | info->pos_x;
	act_writel(tmp, layer_coor);
  	//input image size
	tmp = ((height - 1) << 16) | (width - 1);
	act_writel(tmp, VIDEO_BASE + VIDEO_OFF_ISIZE);
	//output frame size
	tmp = ((out_height - 1) << 16) | (out_width - 1);
	act_writel(tmp, VIDEO_BASE + VIDEO_OFF_OSIZE);
	// scale factor
	w_factor = (width * 8192  +  out_width - 1)/ out_width;
	h_factor = (height * 8192  +  out_height - 1)/ out_height;	
	tmp = (h_factor << 16) | w_factor;
	act_writel(tmp, VIDEO_BASE + VIDEO_OFF_SR);
	tmp_cfg = act_readl(VIDEO_BASE + VIDEO_OFF_CFG); 	
	tmp_cfg |= VIDEO_CFG_CSC_IYUV_QEN;
	tmp_cfg |= VIDEO_CFG_CSC_IYUV_FMT;
	tmp_cfg &= ~VIDEO_CFG_FMT_MASK;	

	if (info->pixel_format == YVU_420_PLANAR) {
		tmp_cfg |= VIDEO_CFG_FMT(YUV_420_PLANAR);
	} else {
		tmp_cfg |= VIDEO_CFG_FMT(info->pixel_format);
	}	
	if ((info->rotation & NEED_XFLIP) != 0) {
		tmp_cfg |= VIDEO_CFG_XFLIP;
	}
	if ((info->rotation & NEED_YFLIP) != 0) {
		tmp_cfg |= VIDEO_CFG_YFLIP;
	}	
	//set critical to be always off
	tmp_cfg = (tmp_cfg & ~VIDEO_CFG_CRITICAL(0x3)) | VIDEO_CFG_CRITICAL(0x2);
	if (need_sub_sample == 0) {
		img_width = img_width / 4; 
	}

	switch (info->pixel_format) {
		case RGB_565:
			tmp_str = img_width;
			break;

		case ARGB_8888:
		case ABGR_8888:
			tmp_str = img_width * 2;
			break;

		case YUV_420_PLANAR:
		case YVU_420_PLANAR:
			tmp_str = ((img_width) >> 1) | (((img_width) >> 2) << 11) | (((img_width) >> 2) << 21);
			break;

		case YUV_420_SEMI_PLANAR:
			tmp_str = ((img_width) >> 1) | (((img_width) >> 1) << 11);
			break;

		case ABGR_1555:
			tmp_str = img_width;
			break;

		default:
			printk("format not support \n");
			return -1;
	}
	act_writel(tmp_cfg,  VIDEO_BASE + VIDEO_OFF_CFG);
	act_writel(tmp_str, VIDEO_BASE + VIDEO_OFF_STR);

	return 0;
}

int de_video_layer_update(struct act_layer *layer) {
	struct act_layer_info * info = layer->info;
	struct layer_manager *layer_mgr = layer->layer_mgr;
	int rc = 0;
	int path_ctl = 0, layer_enable_mask = 0;
	int path_fcr = 0;
	int mode;
	int VIDEO_BASE;

	u32 start_addr0 = 0, start_addr1 = 0, start_addr2 = 0, source_addr = 0;
	u32 cx, cy, iw, ih, temp;
	iw = info->img_width;
	ih = info->img_height;
	cx = info->xoff;
	cy = info->yoff;
	
	if (!core.v_vsync.state) {
		return 0;
	}
	unsigned long long st, end;
	struct timeval begin_time ,now; 
	if (layer_mgr->path_id == DE_PATH_1) {
   		path_ctl = PATH1_CTL;
   		path_fcr = PATH1_FCR;
  		path_ctl = PATH1_CTL;
  		path_fcr = PATH1_FCR;
  		do_gettimeofday(&begin_time);
  		st = begin_time.tv_sec * 1000000 + begin_time.tv_usec;
  	    while (act_readl(PATH1_FCR)) {
  	  		do_gettimeofday(&now);
  	  		end = now.tv_sec * 1000000 + now.tv_usec;
  	  		if (end - st > 16000) {
  	  		break;
			}
		}
	} else if (layer_mgr->path_id == DE_PATH_2) {
  		path_ctl = PATH2_CTL;
  		path_fcr = PATH2_FCR;
  		do_gettimeofday(&begin_time);
  		st = begin_time.tv_sec * 1000000 + begin_time.tv_usec;
  		while (act_readl(PATH2_FCR)) {
  	  		do_gettimeofday(&now);
  	  		end = now.tv_sec * 1000000 + now.tv_usec;
  	  		if (end - st > 16000) {
  	  			break;
			}
		}
	}
	VIDEO_BASE = layer->reg_base;
	if (layer->layer_tag == DE_LAYER_A) {
		layer_enable_mask = PATHx_CTL_INPUTA_ENABLE;
	} else {
		layer_enable_mask = PATHx_CTL_INPUTB_ENABLE;
	}
	
	if (!info->enabled) {
		temp = act_readl(path_ctl);
		if ((temp & layer_enable_mask) != 0) {
			act_writel((temp & (~layer_enable_mask)), path_ctl);
			act_writel(0x01,path_fcr);
			printk("close video layer \n");	
		}

		return 0 ;
	}	
	/*if memory mmap the paddr is the phy addr */
	source_addr = info->paddr;
	mode = info->pixel_format;
	DEDBG("gl5201vout_dss_update_overlay\n");
	DEDBG("iw = %x, ih = %x, cx = %x, cy = %x, source_addr = %x\n", iw, ih, cx, cy, source_addr);
	switch (mode) {
		case RGB_565:
			start_addr0 = source_addr + cy * iw + cx;
			act_writel(start_addr0, VIDEO_BASE + VIDEO_OFF_FB_0);
			break;

		case YUV_420_PLANAR:
		case YVU_420_PLANAR:
			start_addr0 = source_addr + cy * iw + cx;
			start_addr1 = source_addr + iw * ih + (cy * iw) / 4 + cx / 2;
			start_addr2 = start_addr1 + (iw * ih) / 4;
			DEDBG("start_addr0 = %x, start_addr1 = %x, start_addr2 = %x \n", start_addr0, start_addr1, start_addr2);
			act_writel(start_addr0, VIDEO_BASE + VIDEO_OFF_FB_0);
			if (mode == YUV_420_PLANAR) {
				act_writel(start_addr1, VIDEO_BASE + VIDEO_OFF_FB_1);
		    	act_writel(start_addr2, VIDEO_BASE + VIDEO_OFF_FB_2);
			} else {
				act_writel(start_addr2, VIDEO_BASE + VIDEO_OFF_FB_1);
		    	act_writel(start_addr1, VIDEO_BASE + VIDEO_OFF_FB_2);
			}
		break;

		case YUV_420_SEMI_PLANAR:
			start_addr0 = source_addr + cy * iw + cx;
			start_addr1 = source_addr + iw * ih + (cy * iw )/2 + cx;	
			act_writel(start_addr0, VIDEO_BASE + VIDEO_OFF_FB_0);
			act_writel(start_addr1, VIDEO_BASE + VIDEO_OFF_FB_1);
			break;

		case ARGB_8888:
		case ABGR_1555:
		case ABGR_8888:
			start_addr0 = source_addr + cy * iw + cx;
			act_writel(start_addr0, VIDEO_BASE + VIDEO_OFF_FB_0);
			break;

		default:
			printk(KERN_ERR "invalid pixel_format\n");
			rc = -EINVAL;
			break;
	}
	if (info->enabled) { 
		temp = act_readl(path_ctl);
		if ((temp & layer_enable_mask) != layer_enable_mask) {
			act_writel((temp | layer_enable_mask), path_ctl);
			act_writel(0x01,path_fcr);
			DEDBG("open video layer ~~~~ %x\n",	act_readl(path_ctl));
		} else {
			act_writel(0x01,path_fcr);
		}
	}

	return rc;
}

int de_reg_backup(struct de_reg_bak_t * regs) {
	regs->PATH1_CTL_bak = act_readl(PATH1_CTL);
	regs->PATH1_EN_bak = act_readl(PATH1_EN);
	regs->PATH1_BK_bak = act_readl(PATH1_BK);
	regs->PATH1_SIZE_bak = act_readl(PATH1_SIZE);
	regs->PATH1_A_COOR_bak = act_readl(PATH1_A_COOR);
	regs->PATH1_B_COOR_bak = act_readl(PATH1_B_COOR);
	regs->PATH1_ALPHA_bak = act_readl(PATH1_ALPHA_CFG);
	regs->PATH1_CKMAX_bak = act_readl(PATH1_CKMAX);
	regs->PATH1_CKMIN_bak = act_readl(PATH1_CKMIN);

	regs->PATH2_CTL_bak = act_readl(PATH2_CTL);
	regs->PATH2_EN_bak = act_readl(PATH2_EN);
	regs->PATH2_BK_bak = act_readl(PATH2_BK);
	regs->PATH2_SIZE_bak = act_readl(PATH2_SIZE);
	regs->PATH2_A_COOR_bak = act_readl(PATH2_A_COOR);
	regs->PATH2_B_COOR_bak = act_readl(PATH2_B_COOR);
	regs->PATH2_ALPHA_bak = act_readl(PATH2_ALPHA_CFG);
	regs->PATH2_CKMAX_bak = act_readl(PATH2_CKMAX);
	regs->PATH2_CKMIN_bak = act_readl(PATH2_CKMIN);	
	
	regs->GRAPHIC_CFG_bak = act_readl(GRAPHIC_CFG);
	regs->GRAPHIC_SIZE_bak = act_readl(GRAPHIC_SIZE);
	regs->GRAPHIC_FB_bak = act_readl(GRAPHIC_FB);
	regs->GRAPHIC_STR_bak = act_readl(GRAPHIC_STR);				

	regs->VIDEO1_CFG_bak = act_readl(VIDEO1_CFG);
	regs->VIDEO1_ISIZE_bak = act_readl(VIDEO1_ISIZE);
	regs->VIDEO1_OSIZE_bak = act_readl(VIDEO1_OSIZE);
	regs->VIDEO1_SR_bak = act_readl(VIDEO1_SR);
	regs->VIDEO1_SCOEF0_bak = act_readl(VIDEO1_SCOEF0);
	regs->VIDEO1_SCOEF1_bak = act_readl(VIDEO1_SCOEF1);
	regs->VIDEO1_SCOEF2_bak = act_readl(VIDEO1_SCOEF2);
	regs->VIDEO1_SCOEF3_bak = act_readl(VIDEO1_SCOEF3);
	regs->VIDEO1_SCOEF4_bak = act_readl(VIDEO1_SCOEF4);
	regs->VIDEO1_SCOEF5_bak = act_readl(VIDEO1_SCOEF5);
	regs->VIDEO1_SCOEF6_bak = act_readl(VIDEO1_SCOEF6);
	regs->VIDEO1_SCOEF7_bak = act_readl(VIDEO1_SCOEF7);	
	regs->VIDEO1_FB_0_bak = act_readl(VIDEO1_FB_0);
	regs->VIDEO1_FB_1_bak = act_readl(VIDEO1_FB_1);
	regs->VIDEO1_FB_2_bak = act_readl(VIDEO1_FB_2);
	regs->VIDEO1_FB_RIGHT_0_bak = act_readl(VIDEO1_FB_RIGHT_0);
	regs->VIDEO1_FB_RIGHT_1_bak = act_readl(VIDEO1_FB_RIGHT_1);
	regs->VIDEO1_FB_RIGHT_2_bak = act_readl(VIDEO1_FB_RIGHT_2);
	regs->VIDEO1_STR_bak = act_readl(VIDEO1_STR);
	regs->VIDEO1_CRITICAL_bak = act_readl(VIDEO1_CRITICAL);
	regs->VIDEO1_REMAPPING_bak = act_readl(VIDEO1_REMAPPING);
	
	regs->VIDEO2_CFG_bak = act_readl(VIDEO2_CFG);
	regs->VIDEO2_ISIZE_bak = act_readl(VIDEO2_ISIZE);
	regs->VIDEO2_OSIZE_bak = act_readl(VIDEO2_OSIZE);
	regs->VIDEO2_SR_bak = act_readl(VIDEO2_SR);
	regs->VIDEO2_SCOEF0_bak = act_readl(VIDEO2_SCOEF0);
	regs->VIDEO2_SCOEF1_bak = act_readl(VIDEO2_SCOEF1);
	regs->VIDEO2_SCOEF2_bak = act_readl(VIDEO2_SCOEF2);
	regs->VIDEO2_SCOEF3_bak = act_readl(VIDEO2_SCOEF3);
	regs->VIDEO2_SCOEF4_bak = act_readl(VIDEO2_SCOEF4);
	regs->VIDEO2_SCOEF5_bak = act_readl(VIDEO2_SCOEF5);
	regs->VIDEO2_SCOEF6_bak = act_readl(VIDEO2_SCOEF6);
	regs->VIDEO2_SCOEF7_bak = act_readl(VIDEO2_SCOEF7);	
	regs->VIDEO2_FB_0_bak = act_readl(VIDEO2_FB_0);
	regs->VIDEO2_FB_1_bak = act_readl(VIDEO2_FB_1);
	regs->VIDEO2_FB_2_bak = act_readl(VIDEO2_FB_2);
	regs->VIDEO2_FB_RIGHT_0_bak = act_readl(VIDEO2_FB_RIGHT_0);
	regs->VIDEO2_FB_RIGHT_1_bak = act_readl(VIDEO2_FB_RIGHT_1);
	regs->VIDEO2_FB_RIGHT_2_bak = act_readl(VIDEO2_FB_RIGHT_2);
	regs->VIDEO2_STR_bak = act_readl(VIDEO2_STR);
	regs->VIDEO2_CRITICAL_bak = act_readl(VIDEO2_CRITICAL);
	regs->VIDEO2_REMAPPING_bak = act_readl(VIDEO2_REMAPPING);
	
	regs->VIDEO3_CFG_bak = act_readl(VIDEO3_CFG);
	regs->VIDEO3_ISIZE_bak = act_readl(VIDEO3_ISIZE);
	regs->VIDEO3_OSIZE_bak = act_readl(VIDEO3_OSIZE);
	regs->VIDEO3_SR_bak = act_readl(VIDEO3_SR);
	regs->VIDEO3_SCOEF0_bak = act_readl(VIDEO3_SCOEF0);
	regs->VIDEO3_SCOEF1_bak = act_readl(VIDEO3_SCOEF1);
	regs->VIDEO3_SCOEF2_bak = act_readl(VIDEO3_SCOEF2);
	regs->VIDEO3_SCOEF3_bak = act_readl(VIDEO3_SCOEF3);
	regs->VIDEO3_SCOEF4_bak = act_readl(VIDEO3_SCOEF4);
	regs->VIDEO3_SCOEF5_bak = act_readl(VIDEO3_SCOEF5);
	regs->VIDEO3_SCOEF6_bak = act_readl(VIDEO3_SCOEF6);
	regs->VIDEO3_SCOEF7_bak = act_readl(VIDEO3_SCOEF7);	
	regs->VIDEO3_FB_0_bak = act_readl(VIDEO3_FB_0);
	regs->VIDEO3_FB_1_bak = act_readl(VIDEO3_FB_1);
	regs->VIDEO3_FB_2_bak = act_readl(VIDEO3_FB_2);
	regs->VIDEO3_FB_RIGHT_0_bak = act_readl(VIDEO3_FB_RIGHT_0);
	regs->VIDEO3_FB_RIGHT_1_bak = act_readl(VIDEO3_FB_RIGHT_1);
	regs->VIDEO3_FB_RIGHT_2_bak = act_readl(VIDEO3_FB_RIGHT_2);
	regs->VIDEO3_STR_bak = act_readl(VIDEO3_STR);
	regs->VIDEO3_CRITICAL_bak = act_readl(VIDEO3_CRITICAL);
	regs->VIDEO3_REMAPPING_bak = act_readl(VIDEO3_REMAPPING); 

	return 0;
}

int de_reg_resume(struct de_reg_bak_t * regs) {
	//**********config de regs**************************************/
	act_writel(0x0, PATH1_CTL);
	act_writel(regs->PATH1_CTL_bak,PATH1_CTL);
	act_writel(regs->PATH1_EN_bak,PATH1_EN);
	act_writel(regs->PATH1_BK_bak,PATH1_BK);
	act_writel(regs->PATH1_SIZE_bak,PATH1_SIZE);
	act_writel(regs->PATH1_A_COOR_bak,PATH1_A_COOR);
	act_writel(regs->PATH1_B_COOR_bak,PATH1_B_COOR);
	act_writel(regs->PATH1_ALPHA_bak,PATH1_ALPHA_CFG);
	act_writel(regs->PATH1_CKMAX_bak,PATH1_CKMAX);
	act_writel(regs->PATH1_CKMIN_bak,PATH1_CKMIN);
	
	act_writel(regs->PATH2_CTL_bak,PATH2_CTL);
	act_writel(regs->PATH2_EN_bak,PATH2_EN);
	act_writel(regs->PATH2_BK_bak,PATH2_BK);
	act_writel(regs->PATH2_SIZE_bak,PATH2_SIZE);
	act_writel(regs->PATH2_A_COOR_bak,PATH2_A_COOR);
	act_writel(regs->PATH2_B_COOR_bak,PATH2_B_COOR);
	act_writel(regs->PATH2_ALPHA_bak,PATH2_ALPHA_CFG);
	act_writel(regs->PATH2_CKMAX_bak,PATH2_CKMAX);
	act_writel(regs->PATH2_CKMIN_bak,PATH2_CKMIN);	
		
	act_writel(regs->GRAPHIC_CFG_bak,GRAPHIC_CFG);
	act_writel(regs->GRAPHIC_SIZE_bak,GRAPHIC_SIZE);
	act_writel(regs->GRAPHIC_FB_bak,GRAPHIC_FB);
	act_writel(regs->GRAPHIC_STR_bak,GRAPHIC_STR);				
	
	act_writel(regs->VIDEO1_CFG_bak,VIDEO1_CFG);
	act_writel(regs->VIDEO1_ISIZE_bak,VIDEO1_ISIZE);
	act_writel(regs->VIDEO1_OSIZE_bak,VIDEO1_OSIZE);
	act_writel(regs->VIDEO1_SR_bak,VIDEO1_SR);
	act_writel(regs->VIDEO1_SCOEF0_bak,VIDEO1_SCOEF0);
	act_writel(regs->VIDEO1_SCOEF1_bak,VIDEO1_SCOEF1);
	act_writel(regs->VIDEO1_SCOEF2_bak,VIDEO1_SCOEF2);
	act_writel(regs->VIDEO1_SCOEF3_bak,VIDEO1_SCOEF3);
	act_writel(regs->VIDEO1_SCOEF4_bak,VIDEO1_SCOEF4);
	act_writel(regs->VIDEO1_SCOEF5_bak,VIDEO1_SCOEF5);
	act_writel(regs->VIDEO1_SCOEF6_bak,VIDEO1_SCOEF6);
	act_writel(regs->VIDEO1_SCOEF7_bak,VIDEO1_SCOEF7);	
	act_writel(regs->VIDEO1_FB_0_bak,VIDEO1_FB_0);
	act_writel(regs->VIDEO1_FB_1_bak,VIDEO1_FB_1);
	act_writel(regs->VIDEO1_FB_2_bak,VIDEO1_FB_2);
	act_writel(regs->VIDEO1_FB_RIGHT_0_bak,VIDEO1_FB_RIGHT_0);
	act_writel(regs->VIDEO1_FB_RIGHT_1_bak,VIDEO1_FB_RIGHT_1);
	act_writel(regs->VIDEO1_FB_RIGHT_2_bak,VIDEO1_FB_RIGHT_2);
	act_writel(regs->VIDEO1_STR_bak,VIDEO1_STR);
	act_writel(regs->VIDEO1_CRITICAL_bak,VIDEO1_CRITICAL);
	act_writel(regs->VIDEO1_REMAPPING_bak,VIDEO1_REMAPPING);
		
	act_writel(regs->VIDEO2_CFG_bak,VIDEO2_CFG);
	act_writel(regs->VIDEO2_ISIZE_bak,VIDEO2_ISIZE);
	act_writel(regs->VIDEO2_OSIZE_bak,VIDEO2_OSIZE);
	act_writel(regs->VIDEO2_SR_bak,VIDEO2_SR);
	act_writel(regs->VIDEO2_SCOEF0_bak,VIDEO2_SCOEF0);
	act_writel(regs->VIDEO2_SCOEF1_bak,VIDEO2_SCOEF1);
	act_writel(regs->VIDEO2_SCOEF2_bak,VIDEO2_SCOEF2);
	act_writel(regs->VIDEO2_SCOEF3_bak,VIDEO2_SCOEF3);
	act_writel(regs->VIDEO2_SCOEF4_bak,VIDEO2_SCOEF4);
	act_writel(regs->VIDEO2_SCOEF5_bak,VIDEO2_SCOEF5);
	act_writel(regs->VIDEO2_SCOEF6_bak,VIDEO2_SCOEF6);
	act_writel(regs->VIDEO2_SCOEF7_bak,VIDEO2_SCOEF7);	
	act_writel(regs->VIDEO2_FB_0_bak,VIDEO2_FB_0);
	act_writel(regs->VIDEO2_FB_1_bak,VIDEO2_FB_1);
	act_writel(regs->VIDEO2_FB_2_bak,VIDEO2_FB_2);
	act_writel(regs->VIDEO2_FB_RIGHT_0_bak,VIDEO2_FB_RIGHT_0);
	act_writel(regs->VIDEO2_FB_RIGHT_1_bak,VIDEO2_FB_RIGHT_1);
	act_writel(regs->VIDEO2_FB_RIGHT_2_bak,VIDEO2_FB_RIGHT_2);
	act_writel(regs->VIDEO2_STR_bak,VIDEO2_STR);
	act_writel(regs->VIDEO2_CRITICAL_bak,VIDEO2_CRITICAL);
	act_writel(regs->VIDEO2_REMAPPING_bak,VIDEO2_REMAPPING);
		
	act_writel(regs->VIDEO3_CFG_bak,VIDEO3_CFG);
	act_writel(regs->VIDEO3_ISIZE_bak,VIDEO3_ISIZE);
	act_writel(regs->VIDEO3_OSIZE_bak,VIDEO3_OSIZE);
	act_writel(regs->VIDEO3_SR_bak,VIDEO3_SR);
	act_writel(regs->VIDEO3_SCOEF0_bak,VIDEO3_SCOEF0);
	act_writel(regs->VIDEO3_SCOEF1_bak,VIDEO3_SCOEF1);
	act_writel(regs->VIDEO3_SCOEF2_bak,VIDEO3_SCOEF2);
	act_writel(regs->VIDEO3_SCOEF3_bak,VIDEO3_SCOEF3);
	act_writel(regs->VIDEO3_SCOEF4_bak,VIDEO3_SCOEF4);
	act_writel(regs->VIDEO3_SCOEF5_bak,VIDEO3_SCOEF5);
	act_writel(regs->VIDEO3_SCOEF6_bak,VIDEO3_SCOEF6);
	act_writel(regs->VIDEO3_SCOEF7_bak,VIDEO3_SCOEF7);	
	act_writel(regs->VIDEO3_FB_0_bak,VIDEO3_FB_0);
	act_writel(regs->VIDEO3_FB_1_bak,VIDEO3_FB_1);
	act_writel(regs->VIDEO3_FB_2_bak,VIDEO3_FB_2);
	act_writel(regs->VIDEO3_FB_RIGHT_0_bak,VIDEO3_FB_RIGHT_0);
	act_writel(regs->VIDEO3_FB_RIGHT_1_bak,VIDEO3_FB_RIGHT_1);
	act_writel(regs->VIDEO3_FB_RIGHT_2_bak,VIDEO3_FB_RIGHT_2);
	act_writel(regs->VIDEO3_STR_bak,VIDEO3_STR);
	act_writel(regs->VIDEO3_CRITICAL_bak,VIDEO3_CRITICAL);
	act_writel(regs->VIDEO3_REMAPPING_bak,VIDEO3_REMAPPING);
	act_writel(1,PATH2_FCR);
	act_writel(1,PATH1_FCR);

	while (act_readl(VIDEO1_SCOEF0) == 0) {
		printk("de_reg_resume ~~~~~~~~~~~~~~ error \n");
		act_writel(0x4000,VIDEO1_SCOEF0);
		act_writel(0xff073efc,VIDEO1_SCOEF1);
		act_writel(0xfe1038fa,VIDEO1_SCOEF2);
		act_writel(0xfc1b30f9,VIDEO1_SCOEF3);
		act_writel(0xfa2626fa,VIDEO1_SCOEF4);
		act_writel(0xf9301bfc,VIDEO1_SCOEF5);
		act_writel(0xfa3810fe,VIDEO1_SCOEF6);
		act_writel(0xfc3e07ff,VIDEO1_SCOEF7);
		act_writel(0x20002000, VIDEO1_SR);
		act_writel(0x4000,VIDEO2_SCOEF0);
		act_writel(0xff073efc,VIDEO2_SCOEF1);
		act_writel(0xfe1038fa,VIDEO2_SCOEF2);
		act_writel(0xfc1b30f9,VIDEO2_SCOEF3);
		act_writel(0xfa2626fa,VIDEO2_SCOEF4);
		act_writel(0xf9301bfc,VIDEO2_SCOEF5);
		act_writel(0xfa3810fe,VIDEO2_SCOEF6);
		act_writel(0xfc3e07ff,VIDEO2_SCOEF7);	  
		act_writel(0x4000,VIDEO3_SCOEF0);
		act_writel(0xff073efc,VIDEO3_SCOEF1);
		act_writel(0xfe1038fa,VIDEO3_SCOEF2);
		act_writel(0xfc1b30f9,VIDEO3_SCOEF3);
		act_writel(0xfa2626fa,VIDEO3_SCOEF4);
		act_writel(0xf9301bfc,VIDEO3_SCOEF5);
		act_writel(0xfa3810fe,VIDEO3_SCOEF6);
		act_writel(0xfc3e07ff,VIDEO3_SCOEF7);
		act_writel(0x17021,PATH2_CTL);
		printk(KERN_INFO "regs->PATH1_CTL_bak =%x\n",regs->PATH1_CTL_bak); 
		printk(KERN_INFO "regs->PATH2_CTL_bak =%x\n",regs->PATH2_CTL_bak); 
		printk(KERN_INFO "regs->VIDEO1_SCOEF0_bak =%x\n",regs->VIDEO1_SCOEF0_bak); 
		printk(KERN_INFO "regs->VIDEO1_SCOEF1_bak =%x\n",regs->VIDEO1_SCOEF1_bak); 
		printk(KERN_INFO "regs->VIDEO1_SCOEF2_bak =%x\n",regs->VIDEO1_SCOEF2_bak); 
		printk(KERN_INFO "regs->VIDEO1_SCOEF3_bak =%x\n",regs->VIDEO1_SCOEF3_bak); 
		printk(KERN_INFO "regs->VIDEO1_SCOEF4_bak =%x\n",regs->VIDEO1_SCOEF4_bak); 
		printk(KERN_INFO "regs->VIDEO1_SCOEF5_bak =%x\n",regs->VIDEO1_SCOEF5_bak); 
		printk(KERN_INFO "regs->VIDEO1_SCOEF6_bak =%x\n",regs->VIDEO1_SCOEF6_bak); 
		printk(KERN_INFO "regs->VIDEO1_SCOEF7_bak =%x\n",regs->VIDEO1_SCOEF7_bak); 
	}

	return 0;
}
