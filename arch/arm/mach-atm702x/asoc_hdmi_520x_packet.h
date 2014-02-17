/*
 * asoc_hdmi_520x_packet.h
 *
 * Copyright (C) 2011 Actions Semiconductor, Inc
 * Author:	Geng A-nan <genganan@actions-semi.com >
 * Data: Monday August 22, 2008
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
 
#ifndef __ASOC_HDMI_520X_PACKET_H__
#define __ASOC_HDMI_520X_PACKET_H__


int hdmi_EnableWriteRamPacket(void);
int hdmi_SetRamPacketPeriod(unsigned int no, int period);
int hdmi_SetRamPacket(unsigned int no, unsigned char *pkt);
int hdmi_gen_spd_infoframe(void);
int hdmi_gen_avi_infoframe(struct hdmi_ip_data *ip_data);
int hdmi_gen_audio_infoframe(struct audio_settings *audio_general);
int hdmi_gen_gbd_infoframe(struct hdmi_settings *video_general );
int hdmi_gen_vs_infoframe(struct hdmi_settings *video_general);

#endif/*ifndef __ASOC_HDMI_520X_PACKET_H__*/

