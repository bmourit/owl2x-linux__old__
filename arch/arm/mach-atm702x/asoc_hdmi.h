#ifndef __ASOC_HDMI_MAIN_H__
#define __ASOC_HDMI_MAIN_H__

#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/switch.h>

//#define DEBUG
#ifdef DEBUG
#define HDMI_DEBUG(format, ...) \
	do { \
		printk(KERN_INFO "ASOC_HDMI: " format, ## __VA_ARGS__); \
	} while (0)
#else
#define HDMI_DEBUG(format, ...)
#endif

//#define DEBUG_HDCP
#ifdef DEBUG_HDCP
#define HDCP_DEBUG(format, ...) \
	do { \
		printk(KERN_INFO "ASOC_HDCP: " format, ## __VA_ARGS__); \
	} while (0)
#else
#define HDCP_DEBUG(format, ...)
#endif

//#define I2C_RW_DEBUG
//#define READ_HDCP_KEY_FROM_FILE
//#define READ_HDCP_KEY_FROM_NATIVE
#define READ_HDCP_KEY_FROM_FLASH

struct hdmi_ip_data;
struct asoc_display_device;
struct miscdevice;

#define EDID_DATA_SIZE    (128*4)
extern int set_safe_status(unsigned int on);

/*the basic video table, which is supported by asoc's ic*/
enum VIDEO_ID_TABLE {	
	VID640x480P_60_4VS3 = 1,	
	VID720x480P_60_4VS3,	
	VID720x480P_60_16VS9,	
	VID1280x720P_60_16VS9,	
	VID1920x1080I_60_16VS9,	
	VID1440x480I_60_4VS3,		//the same with VID720x480I_60_4VS3,
	VID720x480I_60_16VS9,	
	VID1440x480P_60_4VS3 = 14,	
	VID1440x480P_60_16VS9,	
	VID1920x1080P_60_16VS9,	
	VID720x576P_50_4VS3,	
	VID720x576P_50_16VS9,	
	VID1280x720P_50_16VS9,	
	VID1920x1080I_50_16VS9,	
	VID1440x576I_50_4VS3,		//the same with VID720x576I_50_4VS3,
	VID720x576I_50_16VS9,	
	VID1440x576P_50_4VS3 = 29,	
	VID1440x576P_50_16VS9,	
	VID1920x1080P_50_16VS9,	
	VID1920x1080P_24_16VS9,	
	VID1920x1080P_25_16VS9,	
	VID1920x1080P_30_16VS9,	
	VID720x480P_59P94_4VS3 = 72,	
	VID720x480P_59P94_16VS9,	
	VID1280X720P_59P94_16VS9 = 74,	
	VID1920x1080I_59P94_16VS9,	
	VID720x480I_59P54_4VS3,	
	VID720x480I_59P54_16VS9,	
	VID1920x1080P_59P94_16VS9 = 86,	
	VID1920x1080P_29P97_16VS9 = 104,	
	VID_MAX
};

enum HDMI_TIMING {
	HDMI_TIMING_DISABLE = 0,
	HDMI_TIMING_ENABLE,
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

enum HDMI_ENABLED {
	HDMI_DISABLE = 0,
	HDMI_ENABLE
};

enum HDMI_PLUGGING {
	HDMI_PLUGOUT = 0,
	HDMI_PLUGIN,
	HDMI_PLUGGING_MAX
};

enum HPD_ENABLED {
	HPD_DISABLE = 0,
	HPD_ENABLE
};

enum HPDINT_ENABLED {
	HPDINT_DISABLE = 0,
	HPDINT_ENABLE
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
	char vid_str[30];
	unsigned int Video2to1Scaler;
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

struct asoc_hdmi_ip_ops {
	int (*is_hdmi_enabled)(struct hdmi_ip_data *ip_data);
	int (*is_hpd_enabled)(struct hdmi_ip_data *ip_data);
	int (*is_hpdint_enabled)(struct hdmi_ip_data *ip_data);
	void (*init_configure)(struct hdmi_ip_data *ip_data);
	int (*get_configure)(struct hdmi_ip_data *ip_data);
	int (*set_mode) (struct hdmi_ip_data *ip_data);
	int (*gcp_configure)(struct hdmi_ip_data *ip_data);
	void (*timing_configure)(struct hdmi_ip_data *ip_data);
	void (*pixel_coding)(struct hdmi_settings *settings);
	void (*video_configure)(struct hdmi_ip_data *ip_data);

	int (*gen_infoframe)(struct hdmi_ip_data *ip_data);

	void (*phy_enable)(struct hdmi_ip_data *ip_data);
	void (*phy_disable)(struct hdmi_ip_data *ip_data);
	
	int  (*tvoutpll_disable)(const char *pll_name);

	int (*set_src)(struct hdmi_ip_data *ip_data);

	int (*read_edid)(struct hdmi_ip_data *ip_data, u8 *edid, int len);

	int (*detect_plug)(struct hdmi_ip_data *ip_data);
	int (*detect_pending)(struct hdmi_ip_data *ip_data);
	void (*hpd_enable)(bool flag);

	void (*dump_hdmi)(struct hdmi_ip_data *ip_data);
	void (*dump_de)(struct hdmi_ip_data *ip_data);

};

struct hdmi_xml_config {
	unsigned char package[10];
	unsigned int boot_hdmi_cfg[4];
	int boot_disp_cfg[2];
	int hdmi_vids[30];
	int hdcp_switch;
};

struct hdmi_hdcp {
	int hdcpOper_state;
	int hdcp_a2;
	int hdcpOper_retry;
	int repeater;
	int Ri; 
	int Ri_Read;
	int retry_times_for_set_up_5_second;

	int hdcp_check_status;
	int need_to_delay;
	int check_state;
	int hot_plug_pin_status;
	int hdcp_have_goin_authentication;
	int hdcp_have_open_authentication;
	int hdcp_have_authentication_finished;
	int hdcp_error_timer_open;

	unsigned char  Bksv[5];
	unsigned char  Bstatus[2];
	unsigned char  ksvList[128*5];
	unsigned char  Vp[20];
	unsigned char  hdcpOper_M0[8];

	int ri_counter;
	int Ri_Read_Error;
	int i_error;
	int i2c_error;
	int hdcp_fail_times;

	struct workqueue_struct *wq;
	struct delayed_work hdcp_work;
	struct delayed_work hdcp_err_work;
	bool read_hdcp_success;
	bool hdcp_authentication_success;
};
struct hdmi_ip_data {
	struct sink_capabilities_t sink_cap;
	const struct asoc_hdmi_ip_ops *ops;
	struct hdmi_video_config v_cfg;
	struct hdmi_audio_config a_cfg;
	struct hdmi_hdcp hdcp;
	int which_table;
	struct hdmi_xml_config config;
	//struct hdmi_core_infoframe_avi avi_cfg;
};

struct hdmi_switch_data {
	struct switch_dev sdev;
	struct switch_dev sdev_audio;
	const char *name_on;
	const char *name_off;
	const char *state_on;
	const char *state_off;
	int pending_state;
	struct work_struct work;
};

struct asoc_hdmi {
	struct hdmi_ip_data ip_data;
	unsigned short sink_phy_addr;
	bool sink_cap_available;
	bool onoff_flag;
	bool disconnect_flag;
	bool manual_flag;
	bool suspend_flag;
	bool hdmi_vid_table_file_flag;
	bool pm_hibernation_prepare_flag;
	unsigned int hdmi_state;
	unsigned char edid_data[EDID_DATA_SIZE];
	unsigned char vid_table[512];
	unsigned int orderd_table[30];
	struct hdmi_switch_data *switch_data;
};

void *hdmi_get_vid(struct hdmi_settings *settings);
void *hdmi_get_aid(struct audio_settings *a_general);
void * hdmi_get_id(int which_table);
void hdmi_print_edid(unsigned char *p_buf);
void hdmi_set_switch(int status);
void hdmi_set_onoff_flag(bool flag);
bool hdmi_get_onoff_flag(void);
void hdmi_set_disconnect_flag(bool flag);
bool hdmi_get_disconnect_flag(void);
void hdmi_set_manual_flag(bool flag);
bool hdmi_get_manual_flag(void);
void hdmi_set_suspend_flag(bool flag);
bool hdmi_get_suspend_flag(void);
void hdmi_set_pm_hibernation_prepare_flag(bool flag);
bool hdmi_get_pm_hibernation_prepare_flag(void);
void hdmi_hpd_enable(struct asoc_display_device *disp_dev);
void hdmi_hpdint_enable(struct asoc_display_device *disp_dev);
void hdmi_resume_init_config(struct asoc_display_device *disp_dev);
void hdmi_init_config(struct asoc_display_device *disp_dev);
int hdmi_basic_init(struct asoc_display_device *disp_dev);
int hdmi_get_link_status(struct asoc_display_device *disp_dev);
int hdmi_set_mode(struct asoc_display_device *disp_dev);
void hdmi_enable_output(struct asoc_display_device *disp_dev);
void hdmi_disable_output(struct asoc_display_device *disp_dev);
int  asoc_tvoutpll_disable(const char *pll_name);
unsigned long  asoc_tvoutpll_or_clk_set_rate(const char *name, unsigned long new_rate);
unsigned long asoc_tvoutpll_mux_set_parent(const char *parent);
unsigned long asoc_deepcolorpll_set_rate(unsigned int _3d, unsigned int deep_color);
void hdmi_timing_cfg(struct asoc_display_device *disp_dev);
int hdmi_video_cfg(struct asoc_display_device *disp_dev);
int hdmi_packet_cfg(struct asoc_display_device *disp_dev);


int asoc_520x_is_hdmi_enabled(struct hdmi_ip_data *ip_data);
int asoc_520x_is_hpd_enabled(struct hdmi_ip_data *ip_data);
int asoc_520x_is_hpdint_enabled(struct hdmi_ip_data *ip_data);
void asoc_520x_hdmi_init_configure(struct hdmi_ip_data *ip_data);
int asoc_520x_hdmi_get_config(struct hdmi_ip_data *ip_data);
int asoc_520x_hdmi_set_mode (struct hdmi_ip_data *ip_data);
int asoc_520x_hdmi_gcp_configure(struct hdmi_ip_data *ip_data);
void asoc_520x_hdmi_timing_configure(struct hdmi_ip_data *ip_data);
void asoc_520x_hdmi_pixel_coding(struct hdmi_settings *settings);
void asoc_520x_hdmi_video_configure(struct hdmi_ip_data *ip_data);
int asoc_520x_hdmi_gen_infoframe(struct hdmi_ip_data *ip_data);
void asoc_520x_hdmi_phy_enable(struct hdmi_ip_data *ip_data);
void asoc_520x_hdmi_phy_disable(struct hdmi_ip_data *ip_data);
int asoc_520x_hdmi_src_set(struct hdmi_ip_data *ip_data);
int asoc_520x_hdmi_read_edid(struct hdmi_ip_data *ip_data, u8 *edid, int len);
void set_quickboot_not_control_reg(void);
int asoc_520x_hdmi_detect_plug(struct hdmi_ip_data *ip_data);
int asoc_520x_hdmi_detect_pending(struct hdmi_ip_data *ip_data);
void asoc_520x_hdmi_hpd_enable(bool flag);
void asoc_520x_hdmi_dump_hdmi(struct hdmi_ip_data *ip_data);
void asoc_520x_hdmi_dump_de(struct hdmi_ip_data *ip_data);

int asoc_hdmi_create_sysfs(struct miscdevice *device);
void asoc_hdmi_remove_sysfs(struct miscdevice *device);
/*hdcp*/
int  set_hdcpmode_by_hdmimode(struct hdmi_ip_data *ip_data);
int hdcp_detect_riupdated(void);
int  hdcp_get_ri(struct hdmi_ip_data *ip_data);
void set_hdcp_ri_pj(void);
int hdcp_check_ri(void);
void disable_hdcp_ri_interrupt(void);
int hdcp_ReadKsvList(unsigned char *Bstatus, unsigned char *ksvlist);
int hdcp_ReadVprime(unsigned char *Vp);
int do_Vmatch(struct hdmi_ip_data *ip_data, unsigned char *v, unsigned char *ksvlist, unsigned char *bstatus,
        unsigned char * m0);
void hdcp_read_hdcp_MILR(unsigned char *M0);
void hdcp_read_hdcp_MIMR(unsigned char *M0);
int hdcp_ForceUnauthentication(void);
int hdcp_FreeRunGetAn(unsigned char *an);
void enable_hdcp_repeater(void);
void disable_hdcp_repeater(void);
int hdcp_AuthenticationSequence(struct hdmi_ip_data *ip_data);
void enable_ri_update_check(void);
void set_hdcp_to_Authenticated_state(void);
int hdcp_authentication_switch(bool on);
int hdcp_read_key(void);

/*i2c*/
int  i2c_hdmi_init(void);
int i2c_hdcp_read(char *buf, unsigned short addr, int count);
int i2c_hdcp_write(const char *buf, unsigned short addr, int count);
#endif
