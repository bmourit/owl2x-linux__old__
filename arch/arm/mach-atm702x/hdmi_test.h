#ifndef __HDMI_TEST_H__
#define __HDMI_TEST_H__

#include <linux/types.h>

#define DEBUG
#ifdef DEBUG
#define HDMI_DEBUG(format, ...) \
	do { \
		printk(KERN_INFO "ASOC_HDMI: " format, ## __VA_ARGS__); \
	} while (0)
#else
#define HDMI_DEBUG(format, ...)
#endif

/*the basic video table, which is supported by asoc's ic*/
enum VIDEO_ID_TABLE {	
	VID640x480P_60_4VS3 = 1,	
	VID720x480P_60_4VS3,	
	VID720x480P_60_16VS9,	
	VID1280x720P_60_16VS9,	
	VID1920x1080I_60_16VS9,	
	VID720x480I_60_4VS3,	
	VID720x480I_60_16VS9,	
	VID1440x480P_60_4VS3 = 14,	
	VID1440x480P_60_16VS9,	
	VID1920x1080P_60_16VS9,	
	VID720x576P_50_4VS3,	
	VID720x576P_50_16VS9,	
	VID1280x720P_50_16VS9,	
	VID1920x1080I_50_16VS9,	
	VID720x576I_50_4VS3,	
	VID720x576I_50_16VS9,	
	VID1440x576P_50_4VS3 = 29,	
	VID1440x576P_50_16VS9,	
	VID1920x1080P_50_16VS9,	
	VID1920x1080P_24_16VS9,	
	VID1920x1080P_25_16VS9,	
	VID1920x1080P_30_16VS9,	
	VID720x480P_59P94_4VS3 = 72,	
	VID720x480P_59P94_16VS9,	
	VID1280X720P_59P94_16VS9,	
	VID1920x1080I_59P94_16VS9,	
	VID720x480I_59P54_4VS3,	
	VID720x480I_59P54_16VS9,	
	VID1920x1080P_59P94_16VS9 = 86,	
	VID1920x1080P_29P97_16VS9 = 104,	
	VID_MAX
};

enum HDMI_MODE {
	HDMI_MODE_HDMI = 0,
	HDMI_MODE_DVI,
	HDMI_MODE_MAX
};

enum HDMI_WHICH_TABLE {
	VIDEO_HDMI = 0,
	VIDEO_DE,
	AUDIO_HDMI
	
};
enum HDMI_PLUGGING {
	HDMI_PLUGOUT = 0,
	HDMI_PLUGIN,
	HDMI_PLUGGING_MAX
};
enum SRC_SEL {
	VITD = 0,
	DE,
	SRC_MAX
};

enum PIXEL_ENCODING {
	VIDEO_PEXEL_ENCODING_RGB = 0,
	VIDEO_PEXEL_ENCODING_YCbCr444 = 2,
	VIDEO_PEXEL_ENCODING_MAX
};

enum DEEP_COLOR {
	DEEP_COLOR_24_BIT = 0,
	DEEP_COLOR_30_BIT,
	DEEP_COLOR_36_BIT,
	DEEP_COLOR_MAX
};

enum COLOR_XVYCC {
	YCC601_YCC709 = 0,
	XVYCC601,
	XVYCC709,
	XVYCC_MAX
};

enum _3D_FORMAT{
	_3D_NOT = 0,
	_3D,
	_3D_FORMAT_MAX
};

enum DATA_BLOCK_TYPE {
	AUDIO_DATA_BLOCK = 1,
	VIDEO_DATA_BLOCK = 2,
	VENDOR_SPECIFIC_DATA_BLOCK = 3,
	SPEAKER_ALLOOCATION_DATA_BLOCK = 4,
	VESA_DTC_DATA_BLOCK = 5,
	USE_EXTENDED_TAG = 7
};

struct sink_capabilities_t {
	unsigned int hdmi_mode;
	/*
	 * audio capabilites
	 * for now(20090817), only Linear PCM(IEC60958) considered
	 */

	/*
	 * maximum audio channel number
	 * it should be (<=8)
	 */
	unsigned int max_channel_cap;

	/*
	 * audio sampling rate
	 *
	 * for each bit, if the value is 1 one sampling rate is supported. if 0, not supported.
	 * bit0: when the value is 1, 32kHz    is supported. when 0, 32kHz    is not supported.
	 * bit1: when the value is 1, 44.1kHz  is supported. when 0, 44.1kHz  is not supported.
	 * bit2: when the value is 1, 48kHz    is supported. when 0, 48kHz    is not supported.
	 * bit3: when the value is 1, 88.2kHz  is supported. when 0, 88.2kHz  is not supported.
	 * bit4: when the value is 1, 96kHz    is supported. when 0, 96kHz    is not supported.
	 * bit5: when the value is 1, 176.4kHz is supported. when 0, 176.4kHz is not supported.
	 * bit6: when the value is 1, 192kHz   is supported. when 0, 192kHz   is not supported.
	 * bit7~31: reserved
	 */
	unsigned int sampling_rate_cap;

	/*
	 * audio sample size
	 *
	 * for each bit, if the value is 1 one sampling size is supported. if 0, not supported.
	 * bit0: when the value is 1, 16-bit is supported. when 0, 16-bit is not supported.
	 * bit1: when the value is 1, 20-bit is supported. when 0, 20-bit is not supported.
	 * bit2: when the value is 1, 24-bit is supported. when 0, 24-bit is not supported.
	 * bit3~31: reserved
	 */
	unsigned int sampling_size_cap;

	/*
	 * speaker allocation information
	 *
	 * bit0: when the value is 1, FL/FR   is supported. when 0, FL/FR   is not supported.
	 * bit1: when the value is 1, LFE     is supported. when 0, LFE     is not supported.
	 * bit2: when the value is 1, FC      is supported. when 0, FC      is not supported.
	 * bit3: when the value is 1, RL/RR   is supported. when 0, RL/RR   is not supported.
	 * bit4: when the value is 1, RC      is supported. when 0, RC      is not supported.
	 * bit5: when the value is 1, FLC/FRC is supported. when 0, FLC/FRC is not supported.
	 * bit6: when the value is 1, RLC/RRC is supported. when 0, RLC/RRC is not supported.
	 * bit7~31: reserved
	 *
	 * NOTICE:
	 *      FL/FR, RL/RR, FLC/FRC, RLC/RRC should be presented in pairs.
	 */
	unsigned int speader_allo_info;

	/*
	 * video capabilites
	 */

	/*
	 * pixel encoding (byte3(starting from 0) of CEA Extension Version3)
	 *	  Only pixel encodings of RGB, YCBCR4:2:2, and YCBCR4:4:4 may be used on HDMI.
	 *	  All HDMI Sources and Sinks shall be capable of supporting RGB pixel encoding.
	 *	  If an HDMI Sink supports either YCBCR4:2:2 or YCBCR4:4:4 then both shall be supported.
	 *	  An HDMI Source may determine the pixel-encodings that are supported by the Sink through
	 *		  the use of the E-EDID. If the Sink indicates that it supports YCBCR-formatted video
	 *		  data and if the Source can deliver YCBCR data, then it can enable the transfer of
	 *		  this data across the link.
	 *
	 * bit0: when the value is 1, RGB   is supported. when 0, RGB   is not supported.
	 * bit1: when the value is 1, YCBCR4:4:4 is supported. when 0, YCBCR4:4:4 is not supported.
	 * bit2: when the value is 1, YCBCR4:2:2 is supported. when 0, YCBCR4:2:2 is not supported.
	 * bit3~31: reserved
	 */
	unsigned int pixel_encoding;

	/*
	 * video formats
	 *
	 * all 32 bits of VideoFormatInfo[0] and the former 27 bits of VideoFormatInfo[1] are valid bits.
	 *  the value of each bit indicates whether one video format is supported by sink device.
	 *  video format ID can be found in enum video_id_code.
	 *  the bit postion corresponds to the video format ID in enum video_id_code. For example,
	 *  bit  0 of VideoFormatInfo[0] corresponds to VIDEO_ID_640x480P_60Hz_4VS3
	 *  bit 31 of VideoFormatInfo[0] corresponds to VIDEO_ID_1920x1080P_24Hz_16VS9
	 *  bit  0 of VideoFormatInfo[1] corresponds to VIDEO_ID_1920x1080P_25Hz_16VS9
	 *  bit 26 of VideoFormatInfo[1] corresponds to VIDEO_ID_720x480I_240Hz_16VS9
	 *  when it is 1, the video format is supported, when 0, the video format is not supported.
	 *
	 *  notice:
	 *	  Follwings is video format supported by our hdmi source,
	 *	  please send supported video source to HDMI module
	 *		  VIDEO_ID_640x480P_60Hz_4VS3
	 *		  VIDEO_ID_720x480P_60Hz_4VS3
	 *		  VIDEO_ID_720x480P_60Hz_16VS9
	 *		  VIDEO_ID_720x576P_50Hz_4VS3
	 *		  VIDEO_ID_720x576P_50Hz_16VS9
	 *		  VIDEO_ID_1280x720P_60Hz_16VS9
	 *		  VIDEO_ID_1280x720P_50Hz_16VS9
	 *		  VIDEO_ID_720x480I_60Hz_4VS3
	 *		  VIDEO_ID_720x480I_60Hz_16VS9
	 *		  VIDEO_ID_720x576I_50Hz_4VS3
	 *		  VIDEO_ID_720x576I_50Hz_16VS9
	 *		  VIDEO_ID_1440x480P_60Hz_4VS3
	 *		  VIDEO_ID_1440x480P_60Hz_16VS9
	 *		  VIDEO_ID_1440x576P_50Hz_4VS3
	 *		  VIDEO_ID_1440x576P_50Hz_16VS9
	 *  for 480P/576P, 4:3 is recommended, but 16:9 can be displayed.
	 *  for 720P, only 16:9 format.
	 */
	unsigned int video_formats[4];
};


/**
* hdmi_settings - include the vaviable part of video
* name : video format,include resolution ratio, ar, frequency
* pixel_encoding : rgb,yuv422,etc
*/
struct hdmi_settings {
	unsigned int vid;
	unsigned int hdmi_mode;
	unsigned int video2to1scaler;
	unsigned int hdmi_src;
	unsigned int pixel_encoding;
	unsigned int color_xvycc;
	unsigned int _3d;
	unsigned int deep_color;
};

struct video_parameters_t {
	unsigned int vid;
	unsigned int video2to1scaler;
	unsigned int ar;//1--4:3; 2--16:9
	unsigned int colorimetry;
	unsigned int  interleave;
	
	int victl;
	int vivsync;
	int vivhsync;
	int vialseof;
	int vialseef;
	int viadlse;
	int dipccr_24;
	int dipccr_30; 
	int dipccr_36;
	int vhsync_p;//Vsync & Hsync Active Low
	int vhsync_inv; //Vsync and Hsync Invert---Active low
};

struct hdmi_video_config {
	struct hdmi_settings *settings;
	struct de_video_mode *mode;
	struct video_parameters_t *parameters;
};

/*audio*/
struct audio_settings {

	unsigned int audio_channel;
	unsigned int audio_fs;
	unsigned int audio60958;
		
};
 
struct audio_parameters_t {

	unsigned int audio60958;
	unsigned int audio_channel;
	int ASPCR;      
	int ACACR;       
	int AUDIOCA;     
	
};

struct hdmi_audio_config {
	struct audio_settings *settings;
	struct audio_parameters_t *parameters;
};

struct hdmi_ip_data {
	struct sink_capabilities_t sink_cap;
	const struct asoc_hdmi_ip_ops *ops;
	struct hdmi_video_config v_cfg;
	struct hdmi_audio_config a_cfg;
	int which_table;
	//struct hdmi_core_infoframe_avi avi_cfg;
};

#endif

