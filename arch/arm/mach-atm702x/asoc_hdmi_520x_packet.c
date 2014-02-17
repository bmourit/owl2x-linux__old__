/*
 * asoc_hdmi_520x_packet.c
 *
 * Copyright (C) 2011 Actions Semiconductor, Inc
 * Author:	Geng A-nan <genganan@actions-semi.com >
 * Data: Monday August 22, 2008
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/err.h>
#include <asm/atomic.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <asm/atomic.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <mach/gl5202_cmu.h>
#include <mach/irqs.h>
#include <mach/hardware.h>
#include <mach/actions_reg_gl5202.h>
#include "../display/act_de/display.h"
#include "../display/act_de/de.h"
#include "gl5202_hdmi.h"
#include "asoc_hdmi.h"
/* Internal SRAM allocation for Periodic Data Island Packet */

#define HDMI_RAMPKT_AVI_SLOT	0
#define HDMI_RAMPKT_AUDIO_SLOT	1
#define HDMI_RAMPKT_SPD_SLOT	2
#define HDMI_RAMPKT_GBD_SLOT	3
//#define HDMI_RAMPKT_ACP_SLOT	4
#define HDMI_RAMPKT_VS_SLOT	    4
#define HDMI_RAMPKT_MPEG_SLOT	5

#define HDMI_RAMPKT_PERIOD		1

int memcpy_1(u8 *des, u8 *source, int len) {
    int i;
    for(i=0;i<len;i++)
        *(des+i) = *(source+i);
    return 1;       
}

/* Packet Command */
int hdmi_EnableWriteRamPacket(void) {
	int i;
	act_writel(act_readl(HDMI_OPCR) | HDMI_OPCR_ENRBPKTSRAM, HDMI_OPCR);
	while (act_readl(HDMI_OPCR) & HDMI_OPCR_ENRBPKTSRAM) {
		for (i = 0; i < 10; i++);
	}

	return 0;
}

int hdmi_SetRamPacketPeriod(unsigned int no, int period) {
	if (no > 5 || no < 0)
		return -1;
	
	if (period > 0xf || period < 0)
		return -1;
	act_writel(act_readl(HDMI_RPCR) &  ~(1 << no), HDMI_RPCR);
	act_writel(act_readl(HDMI_RPCR) & ~(0xf << ( no * 4 + 8)), HDMI_RPCR);

	if (period) {
		act_writel(act_readl(HDMI_RPCR) | (period << ( no * 4 + 8)), HDMI_RPCR);
		act_writel(act_readl(HDMI_RPCR) | (1 << no), HDMI_RPCR);
	}

	return 0;
}

/**
 * convert readable Data Island packet to RAM packet format,
 * and write to RAM packet area
 */
int hdmi_SetRamPacket(unsigned int no, unsigned char *pkt) {
    unsigned char tpkt[36];
    unsigned long *reg = (unsigned long *)tpkt;
    unsigned int addr = 126 + no * 14;

	if (no > 5 || no < 0)
		return -1;
    //change by genganan 2008-09-24
#if 0       
    //Packet Header
	tpkt[0] = 0;
	tpkt[1] = pkt[2];
	tpkt[2] = pkt[1];
	tpkt[3] = pkt[0];
    //Packet word0
	tpkt[4] = pkt[6];
	tpkt[5] = pkt[5];
	tpkt[6] = pkt[4];
	tpkt[7] = pkt[3];
	//Packet word1
	tpkt[8] = 0;
	tpkt[9] = pkt[9];
	tpkt[10] = pkt[8];
	tpkt[11] = pkt[7];
    //Packet word2
	tpkt[12] = pkt[13];
	tpkt[13] = pkt[12];
	tpkt[14] = pkt[11];
	tpkt[15] = pkt[10];
	//Packet word3
	tpkt[16] = 0;
	tpkt[17] = pkt[16];
	tpkt[18] = pkt[15];
	tpkt[19] = pkt[14];
    //Packet word4
	tpkt[20] = pkt[20];
	tpkt[21] = pkt[19];
	tpkt[22] = pkt[18];
	tpkt[23] = pkt[17];
	//Packet word5
	tpkt[24] = 0;
	tpkt[25] = pkt[23];
	tpkt[26] = pkt[22];
	tpkt[27] = pkt[21];
    //Packet word6
	tpkt[28] = pkt[27];
	tpkt[29] = pkt[26];
	tpkt[30] = pkt[25];
	tpkt[31] = pkt[24];
	//Packet word7
	tpkt[32] = 0;
	tpkt[33] = pkt[30];
	tpkt[34] = pkt[29];
	tpkt[35] = pkt[28];
#else
    //Packet Header
    	tpkt[0] = pkt[0];
	tpkt[1] = pkt[1];
	tpkt[2] = pkt[2];
	tpkt[3] = 0;
    //Packet Word0
	tpkt[4] = pkt[3];
	tpkt[5] = pkt[4];
	tpkt[6] = pkt[5];
	tpkt[7] = pkt[6];
    //Packet Word1
	tpkt[8] = pkt[7];
	tpkt[9] = pkt[8];
	tpkt[10] = pkt[9];
	tpkt[11] = 0;
    //Packet Word2
	tpkt[12] = pkt[10];
	tpkt[13] = pkt[11];
	tpkt[14] = pkt[12];
	tpkt[15] = pkt[13];
    //Packet Word3
	tpkt[16] = pkt[14];
	tpkt[17] = pkt[15];
	tpkt[18] = pkt[16];
	tpkt[19] = 0;
    //Packet Word4
	tpkt[20] = pkt[17];
	tpkt[21] = pkt[18];
	tpkt[22] = pkt[19];
	tpkt[23] = pkt[20];
    //Packet Word5
	tpkt[24] = pkt[21];
	tpkt[25] = pkt[22];
	tpkt[26] = pkt[23];
	tpkt[27] = 0;
    //Packet Word6
	tpkt[28] = pkt[24];
	tpkt[29] = pkt[25];
	tpkt[30] = pkt[26];
	tpkt[31] = pkt[27];
    //Packet Word7
	tpkt[32] = pkt[28];
	tpkt[33] = pkt[29];
	tpkt[34] = pkt[30];
	tpkt[35] = 0;
#endif

	act_writel((1<<8) | (addr&0xff), HDMI_OPCR);
	act_writel(reg[0], HDMI_ORP6PH);
	act_writel(reg[1], HDMI_ORSP6W0);
	act_writel(reg[2], HDMI_ORSP6W1);
	act_writel(reg[3], HDMI_ORSP6W2);
	act_writel(reg[4], HDMI_ORSP6W3);
	act_writel(reg[5], HDMI_ORSP6W4);
	act_writel(reg[6], HDMI_ORSP6W5);
	act_writel(reg[7], HDMI_ORSP6W6);
	act_writel(reg[8], HDMI_ORSP6W7);

	hdmi_EnableWriteRamPacket();

	return 0;
}

int hdmi_gen_spd_infoframe(void) {
    static u8 pkt[32];
   	unsigned int checksum=0;
   	unsigned int i;
   	static char spdname[8]="Vienna";
   	static char spddesc[16]="DTV SetTop Box";

	// clear buffer 
	for (i=0;i<32;i++)
        pkt[i]=0;
        
		// header 
		pkt[0] = 0x80 | 0x03;   //HB0: Packet Type = 0x83
		pkt[1] = 1;			    //HB1: version = 1
		pkt[2] = 0x1f & 25; 	//HB2: len = 25 
		pkt[3] = 0x00;	        //PB0: checksum = 0 

	// data
	// Vendor Name, 8 bytes 
	memcpy_1(&pkt[4], spdname, 8);
	// Product Description, 16 bytes
	memcpy_1(&pkt[12], spddesc, 16);
	// Source Device Information 
	pkt[28] = 0x1;          //Digital STB

	// count checksum
	for(i=0; i<31; i++)
		checksum += pkt[i];
	pkt[3] = (~checksum + 1)  & 0xff;

	// set to RAM Packet
	hdmi_SetRamPacket(HDMI_RAMPKT_SPD_SLOT, pkt);
	hdmi_SetRamPacketPeriod(HDMI_RAMPKT_SPD_SLOT, HDMI_RAMPKT_PERIOD);

	return 0;
}

/**
 * function:hdmi_gen_avi_infoframe
 * input:  colorformat : 0--RGB444;1--YCbCr422;2--YCbCr444
 *         			AR : 1--4:3;   2--16:9
 * return: 0
 */

int hdmi_gen_avi_infoframe(struct hdmi_ip_data *ip_data) {
    static u8 pkt[32];
    u32 checksum=0;
    int i;

	struct video_parameters_t *v_parameters = NULL;
	v_parameters = hdmi_get_id(VIDEO_HDMI);
	if (!v_parameters) {
		HDMI_DEBUG("[%s]don't support this format!\n", __func__);
		return -1;
	}

	/* clear buffer */
    	for(i=0;i<32;i++)
        	pkt[i]=0;

	//1. header
	pkt[0] = 0x80 | 0x02; 	//header = 0x82
	pkt[1] = 2;         	//version = 2
	pkt[2] = 0x1f & 13;		// len = 13
	pkt[3] = 0x00;	    	// checksum = 0 
	
	//2. data 
	//PB1--Y1:Y0=colorformat;R3:R1 is invalid ;no bar info and scan info
	pkt[4] = (ip_data->v_cfg.settings->pixel_encoding << 5) | (0<<4) | (0<<2) | (0); 
	if (ip_data->v_cfg.settings->color_xvycc == 0) {
		//PB2--Colorimetry:SMPTE 170M|ITU601; Picture aspect Ratio; same as picture aspect ratio  
		pkt[5] = (v_parameters->colorimetry << 6) | (v_parameters->ar << 4) | (0x8); 
		//PB3--No known non-uniform scaling
		pkt[6] = 0x0;
	} else {
		//PB2--Colorimetry:SMPTE 170M|ITU601; Picture aspect Ratio; same as picture aspect ratio  
		pkt[5] = (0x3<<6) | (v_parameters->ar << 4) | (0x8);
		if (ip_data->v_cfg.settings->color_xvycc == 1) {
			//PB3--xvYCC601;No known non-uniform scaling
			pkt[6] = 0x0;	
		} else {
		//PB3--xvYCC709;No known non-uniform scaling
			pkt[6] = 0x1<<4;	
		}
	}

    //PB4--Video Id
    if (v_parameters->vid > 70)
    	pkt[7] = v_parameters->vid - 70;
   	else
		pkt[7] = v_parameters->vid;
	//PB5--Pixel repeat time:
	if ((v_parameters->vid == VID1440x576I_50_4VS3) || //the same with VID720x576I_50_4VS3
		(v_parameters->vid == VID720x576I_50_16VS9) ||
		(v_parameters->vid == VID720x480I_59P54_4VS3) ||
		(v_parameters->vid == VID720x480I_59P54_16VS9) ||
		(v_parameters->vid == VID720x480I_60_16VS9) ||
		(v_parameters->vid == VID1440x480I_60_4VS3))
	{ 	//the same with VID720x480I_60_4VS3

		pkt[8] = 0x1;
	} else { 
		pkt[8] = 0;
	}
	//PB6--PB13: Bar Info, no bar info
	pkt[9] = 0;
	pkt[10] = 0;
	pkt[11] = 0;
	pkt[12] = 0;
	pkt[13] = 0;
	pkt[14] = 0;
	pkt[15] = 0;
	pkt[16] = 0;
	
	// count checksum
	for (i=0; i<31; i++)
		checksum += pkt[i];
	pkt[3] = (~checksum + 1) & 0xff;

	/* set to RAM Packet */
	hdmi_SetRamPacket(HDMI_RAMPKT_AVI_SLOT, pkt);
	hdmi_SetRamPacketPeriod(HDMI_RAMPKT_AVI_SLOT, HDMI_RAMPKT_PERIOD);

	return 0;
}

int hdmi_gen_audio_infoframe(struct audio_settings *audio_general) {
    static u8 pkt[32];
    u32 checksum=0;
    int i;
	struct audio_parameters_t *a_parameters = NULL;
	HDMI_DEBUG ("[%s start]\n", __func__);
	//a_parameters = (struct audio_parameters_t *)hdmi_get_aid(audio_general);
	a_parameters = hdmi_get_id(AUDIO_HDMI);
	if (!a_parameters) {
		HDMI_DEBUG("[%s]audio info error!\n", __func__);
		return -1;
	}

	// clear buffer 
    for (i=0;i<32;i++)
    	pkt[i]=0;
	//1. header
	pkt[0] = 0x80 | 0x04;   //HB0: Packet Type = 0x84
	pkt[1] = 1;			    //HB1: version = 1
	pkt[2] = 0x1f & 10;	    //HB2: len = 10
	pkt[3] = 0x00;	        //PB0: checksum = 0
	// data
	if (a_parameters->audio60958 == 1) {
#if 0
		pkt[4] = (1<<4) | (AudioChannel & 0x7);    		//PB1: Audio Code Type and Audio Channel
		// uncompressed format
		pkt[5] = (AudioFS<<2) | (0x3 & 0x3);         	//PB2: Sampling Frequency and 24bit sample size
		pkt[6] = 0x0;
#else 	//for Panasonic HDMI Analyzer
		pkt[4] = a_parameters->audio_channel & 0x7; 	//PB1: Audio coding Type is refer to stream header and 5 audio channel
		// uncompressed format
		pkt[5] = 0x0;                               	//PB2: Sampling Frequency and size are refer to stream header
		pkt[6] = 0x0;
#endif

		// channel/speaker allocation:
		pkt[7] = a_parameters->AUDIOCA;               	//PB4: layout by CEA-861B table 22
	}
	if (a_parameters->audio60958 == 0) {
		pkt[4] = 0;	//20091218by genganan to test 61937
		pkt[7] = 0;	//20091218by genganan to test 61937
	}
	// Down-mix Inhibits Flag and level shift value
	pkt[8] = (0x0<<7) | (0x0<<3);   //PB5: premitted or no information about Down-mix; 0dB level shift	

	/* count checksum */
	for(i=0; i<31; i++)
		checksum += pkt[i];
	pkt[3] = (~checksum + 1)  & 0xff;

	// set to RAM Packet 
	hdmi_SetRamPacket(HDMI_RAMPKT_AUDIO_SLOT, pkt);
	hdmi_SetRamPacketPeriod(HDMI_RAMPKT_AUDIO_SLOT, HDMI_RAMPKT_PERIOD);	
	HDMI_DEBUG ("[%s finished]\n", __func__);
	
	return 0;
}

/**
 * function:hdmi_gen_gbd_infoframe
 * input:  Color_xvYCC : 0--Normal YCC601 or YCC709; 1--xvYCC601; 2--xvYCC709
 *  		ColorDepth : 0--24bit; 1--30bit; 2--36bit
 * return: 0
 */

int hdmi_gen_gbd_infoframe(struct hdmi_settings *video_general ) {
	static u8 pkt[32];
	u32 checksum = 0;
	unsigned int deep_color = video_general->deep_color;
	unsigned int color_xvycc = video_general->color_xvycc;
	int i;
	
	/* clear buffer */
	for(i=0;i<32;i++)
		pkt[i]=0;
	
	//1. header
	pkt[0] = 0xa;       						//header
	pkt[1] = (0x1<<7) | (0x0<<4) | (0x1);    	//Next_Field = 1; GBD_Profile = P0; Affected Gamut seq num = 1;
	pkt[2] = (0x3<<4) | (0x1);	            	//Only Packet in sequence; current Gamut seq num = 1;
	pkt[3] = 0x00;	    						// checksum = 0 
	
	//2. data 
	//PB1--Format Flag; GBD_Color_Precision; GBD_Color_Space
	pkt[4] = (0x1<<7) | (deep_color<<3) | (color_xvycc); 
	
	if (deep_color == 0) { 			//24bit
		pkt[5] = 0x0;				//min Red data
		pkt[6] = 0xfe;				//max Red data
		pkt[7] = 0x0;				//min Green data
		pkt[8] = 0xfe;				//max Green data
		pkt[9] = 0x0;				//min Blue data
		pkt[10] = 0xfe;				//max Blue data	    
	}
	if (deep_color == 1) {       	//30bit
		pkt[5] = 0x0;				//min Red data: 0x0
		pkt[6] = 0x3f;				//max Red data: 0x3f8
		pkt[7] = 0x80;				//min Green data
		pkt[8] = 0x3;				//max Green data
		pkt[9] = 0xf8;				//min Blue data
		pkt[10] = 0x0;				//max Blue data	 
		pkt[11] = 0x3f;
		pkt[12] = 0x80;   
	}
	if (deep_color == 2) {       	//36bit
		pkt[5] = 0x0;				//min Red data: 0x0
		pkt[6] = 0xf;				//max Red data: 0xfe0
		pkt[7] = 0xe0;				//min Green data
		pkt[8] = 0x0;				//max Green data
		pkt[9] = 0xf;				//min Blue data
		pkt[10] = 0xe0;				//max Blue data	 
		pkt[11] = 0x0;
		pkt[12] = 0xf;
		pkt[13] = 0xe0;   
	}
		
	// count checksum
	for(i=0; i<31; i++)
		checksum += pkt[i];
	pkt[3] = (~checksum + 1) & 0xff;

	/* set to RAM Packet */
	hdmi_SetRamPacket(HDMI_RAMPKT_GBD_SLOT, pkt);
	hdmi_SetRamPacketPeriod(HDMI_RAMPKT_GBD_SLOT, HDMI_RAMPKT_PERIOD);

	return 0;
}

/**
 * hdmi_gen_vs_infoframe(Vendor Specific)
 * input: 3D format
 * return: 0
 */

int hdmi_gen_vs_infoframe(struct hdmi_settings *video_general) {
	static u8 pkt[32];
	u32 checksum = 0;
	int i;
	
	/* clear buffer */
	for (i = 0; i < 32; i++)
		pkt[i] = 0;
	
	//1. header
	pkt[0] = 0x81;       //header
	pkt[1] = 0x1;        //Version

	if (video_general->vid == 2) {
		pkt[2] = 0x6; 	//length, for Side-by-Side Half 3D
	} else {
		pkt[2] = 0x5; 	//length,  for Frame 3D and Top-Bottom Half 3D
	}
	pkt[3] = 0x00; 		// checksum = 0 
	
	//2. data 
	//PB1--PB3:24bit IEEE Registration Identifier
	pkt[4] = 0x03;
	pkt[5] = 0x0c;
	pkt[6] = 0x00; 
	
	//PB4: HDMI_Video_Format:000--no additional;001--extended resolution(4k*2k);010--3D format
	pkt[7] = 0x2<<5;       //3D format
	switch(video_general->vid) {
	 case   1:  pkt[8] = 0x0<<4;                //3D Frame
	            pkt[9] = 0x0;
	            break;
	 case   2:  pkt[8] = 0x8<<4;                //Side-by-Side (Half)
	            pkt[9] = 0x1<<4;
	            break;
	 case   3:  pkt[8] = 0x6<<4;                //Top-and-bottom (Half)
	            pkt[9] = 0x0;
	            break;
	 default :  break;	            
	}
		
	// count checksum
	for(i=0; i<31; i++)
		checksum += pkt[i];
	pkt[3] = (~checksum + 1) & 0xff;

	/* set to RAM Packet */
	hdmi_SetRamPacket(HDMI_RAMPKT_VS_SLOT, pkt);
	hdmi_SetRamPacketPeriod(HDMI_RAMPKT_VS_SLOT, HDMI_RAMPKT_PERIOD);

	return 0;
}
