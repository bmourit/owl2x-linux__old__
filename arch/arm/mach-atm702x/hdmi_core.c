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
#include <linux/kthread.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <mach/gl5202_cmu.h>
#include <mach/irqs.h>
#include <mach/hardware.h>
#include <mach/actions_reg_gl5202.h>
#include <linux/switch.h>
#include <linux/sysfs.h>

#include "../display/act_de/display.h"
#include "../display/act_de/de.h"
#include "asoc_hdmi.h"
#include "gl5202_hdmi.h"
#include "i2c_hdmi_edid.h"
#include "hdmi_features.h"

/*edid*/
#define BIT0						(1 << 0)
#define BIT1						(1 << 1)
#define BIT2						(1 << 2)
#define BIT3						(1 << 3)
#define BIT4						(1 << 4)
#define BIT5						(1 << 5)
#define BIT6						(1 << 6)
#define BIT7						(1 << 7)

atomic_t hdmi_plugging = ATOMIC_INIT(0);
atomic_t hdmi_enabled = ATOMIC_INIT(0);
atomic_t hdmi_timing_set = ATOMIC_INIT(HDMI_TIMING_DISABLE);
atomic_t hdcp_authentication_finished = ATOMIC_INIT(0);
atomic_t hdcp_have_goin_authentication = ATOMIC_INIT(0);

static DEFINE_MUTEX(mutex_sink_cap);
static DEFINE_MUTEX(mutex_sink_phy_addr);
static DEFINE_MUTEX(hdmi_onoff_mutex);
static DEFINE_MUTEX(hdmi_disconnect_mutex);
static DEFINE_MUTEX(hdmi_manual_mutex);
static DEFINE_MUTEX(hdmi_suspend_mutex);
static DEFINE_MUTEX(hdmi_vid_table_mutex);
static DEFINE_MUTEX(pm_hibernation_prepare_mutex);

static struct asoc_hdmi hdmi;
extern struct device_attribute hdmi_vid_table_attr;
extern struct miscdevice hdmi_miscdev;
extern struct asoc_display_device *hdmi_data;

unsigned long hdcp_timer_interval = 50;       
unsigned long hdcp_timer_error_interval = 100;

struct hdmi_settings default_v_settings = {
	.vid = VID1280x720P_50_16VS9,
	.hdmi_mode = HDMI_MODE_HDMI,
	.video2to1scaler = 0,
	.hdmi_src = DE,
	.pixel_encoding = VIDEO_PEXEL_ENCODING_YCbCr444,
	.color_xvycc = YCC601_YCC709,
	.deep_color = DEEP_COLOR_24_BIT,
	._3d = _3D_NOT,
};
	
struct video_parameters_t video_parameters[] = {
	               /*vid                                                                                      ar       SCAN_MODE              vivsync                           vialseof                        viadlse                         dipccr_30                        vhsync_p                                                                               vid_str           Video2to1Scaler      colorimetry      victl                            vivhsync                          vialseef                       dipccr_24                      dipccr_36                    vhsync_inv*/
	{VID1280x720P_50_16VS9,       "VID1280x720P_50_16VS9",          0x0, 0x10, 0x10, 0x0, 0x07bb2ed0, 0x00000000, 0x042ed027, 0x002e8018, 0x00000000, 0x06030103, 0x00001107, 0x00001108, 0x00001108, 0x00000000, 0x00000000},
	{VID1280x720P_60_16VS9,        "VID1280x720P_60_16VS9",        0x0, 0x10, 0x10, 0x0, 0x06712ed0, 0x00000000, 0x042ed027, 0x002e8018, 0x00000000, 0x06030103, 0x00001107, 0x00001108, 0x00001108, 0x00000000, 0x00000000},
	{VID1920x1080P_50_16VS9,      "VID1920x1080P_50_16VS9",        0x0, 0x10, 0x10, 0x0, 0xa4f4640,  0x00000000, 0x0446402b, 0x00460028, 0x00000000, 0x083f00bf, 0x00001107,  0x00001108, 0x00001108, 0x00000000, 0x00000000},	
	{VID1920x1080P_60_16VS9,      "VID1920x1080P_60_16VS9",         0x0, 0x10, 0x10, 0x0, 0x08974640, 0x00000000, 0x0446402b, 0x00460028, 0x00000000, 0x083f00bf, 0x00001105, 0x00001107, 0x00001108, 0x00000000, 0x00000000},
	{VID1920x1080P_59P94_16VS9, "VID1920x1080P_59P94_16VS9",  0x0, 0x10, 0x01, 0x0, 0x08974640, 0x00000000, 0x0446402b, 0x00460028, 0x00000000, 0x083f00bf, 0x00001105, 0x00001107, 0x00001108, 0x00000000, 0x00000000},	
	{VID1920x1080P_30_16VS9,      "VID1920x1080P_30_16VS9",        0x0, 0x10, 0x10, 0x0, 0x08974640, 0x00000000, 0x0446402b, 0x00460028, 0x00000000, 0x083f00bf, 0x00001105, 0x00001107, 0x00001108, 0x00000000, 0x00000000},
	//{VID1920x1080P_29P97_16VS9, "VID1920x1080P_29P97_16VS9", 0x0, 0x10, 0x01, 0x0, 0x08974640, 0x00000000, 0x0446402b, 0x00460028, 0x00000000, 0x083f00bf, 0x00001107, 0x00001107, 0x00001108, 0x00000000, 0x00000000},
	{VID1920x1080P_25_16VS9,      "VID1920x1080P_25_16VS9",         0x0, 0x10, 0x10, 0x0, 0x0a4f4640, 0x00000000, 0x0446402b, 0x00460028, 0x00000000, 0x083f00bf, 0x00001107, 0x00001108, 0x00001108, 0x00000000, 0x00000000},
	{VID1920x1080P_24_16VS9,      "VID1920x1080P_24_16VS9",        0x0, 0x10, 0x10, 0x0, 0x0abd4640, 0x00000000, 0x0446402b, 0x00460028, 0x00000000, 0x083f00bf, 0x00001107, 0x00001108, 0x00001108, 0x00000000, 0x00000000},
	{VID1920x1080I_60_16VS9,      "VID1920x1080I_60_16VS9",          0x0, 0x10, 0x10, 0x1, 0x18974640, 0x00237232, 0x0446402b, 0x0022f013, 0x00462246, 0x083f00bf, 0x00001105, 0x00001107,0x00001108, 0x00000000, 0x00000000},
	//{VID1920x1080I_59P94_16VS9, "VID1920x1080I_59P94_16VS9",  0x0, 0x10, 0x01, 0x1, 0x18974640, 0x00237232, 0x0446402b, 0x0022f013, 0x00462246, 0x083f00bf, 0x00001105, 0x00001107,0x00001108, 0x00000000, 0x00000000},
	{VID1920x1080I_50_16VS9,      "VID1920x1080I_50_16VS9",           0x0, 0x10, 0x10, 0x1, 0x1a4f4640, 0x00237232, 0x0446402b, 0x0022f013, 0x00462246, 0x083f00bf, 0x00001107, 0x00001108,0x00001108, 0x00000000, 0x00000000},
	{VID1440x576P_50_16VS9,       "VID1440x576P_50_16VS9",          0x1, 0x10, 0x01, 0x0, 0x06bf2700, 0x00000000, 0x0427007f, 0x0026b02b, 0x00000000, 0x06a70107, 0x00001105, 0x00001107,0x00001108, 0x00000000, 0x00000006},
	{VID1440x576P_50_4VS3,         "VID1440x576P_50_4VS3",            0x1, 0x01, 0x01, 0x0, 0x06bf2700, 0x00000000, 0x0427007f, 0x0026b02b, 0x00000000, 0x06a70107, 0x00001105, 0x00001107,0x00001108, 0x00000000, 0x00000006},
	{VID1440x480P_60_16VS9,       "VID1440x480P_60_16VS9",          0x1, 0x10, 0x01, 0x0, 0x06b320c0, 0x00000000, 0x0b00507b, 0x00209029, 0x00000000, 0x069300f3, 0x00001105, 0x00001106,0x00001108, 0x00000000, 0x00000006},	
	{VID1440x480P_60_4VS3,         "VID1440x480P_60_16VS9",          0x1, 0x01, 0x01, 0x0, 0x06b320c0, 0x00000000, 0x0b00507b, 0x00209029, 0x00000000, 0x069300f3, 0x00001105, 0x00001106,0x00001108, 0x00000000, 0x00000006},
       {VID1440x576I_50_4VS3,           "VID720x576I_50_4VS3",                0x1, 0x01, 0x01, 0x1, 0x36bf2700, 0x0013b138, 0x0227007d, 0x00135015, 0x0026e14e, 0x06a70107, 0x00001105, 0x00001107, 0x00001108, 0x00000000, 0x00000006},//the same with VID720x576I_50_16VS9
       {VID1440x480I_60_4VS3,           "VID720x480I_60_4VS3",             0x1, 0x01, 0x01, 0x1, 0x36b320c0, 0x0010c109, 0x0500207b, 0x00104014, 0x0020b11b, 0x068d00ed, 0x00001105, 0x0,            0x0,            0x00000000, 0x00000000},//the same with VID720x480I_60_4VS3
       {VID1280X720P_59P94_16VS9,  "VID1280X720P_59P94_16VS9", 0x0, 0x10, 0x01, 0x0, 0x06712ed0, 0x00000000, 0x042ed027, 0x002e8018, 0x00000000, 0x06030103, 0x00001107, 0x00001108, 0x00001108, 0x00000000, 0x00000000},
	{VID720x576P_50_16VS9,         "VID720x576P_50_16VS9",           0x0, 0x10, 0x01, 0x0, 0x035f2700, 0x00000000, 0x0427003f, 0x0026b02b, 0x00000000, 0x03530083, 0x00000701, 0x00000701, 0x00000702, 0x00000000, 0x00000006},
	{VID720x576P_50_4VS3,           "VID720x576P_50_4VS3",             0x0, 0x01, 0x01,0x0, 0x035f2700, 0x00000000, 0x0427003f,  0x0026b02b, 0x00000000, 0x03530083, 0x00000701, 0x00000701, 0x00000702, 0x00000000, 0x00000006},
	{VID720x576I_50_16VS9,         "VID720x576I_50_16VS9",              0x1, 0x10, 0x01, 0x1, 0x36bf2700, 0x0013b138, 0x0227007d, 0x00135015, 0x0026e14e, 0x06a70107, 0x00001105, 0x00001107, 0x00001108, 0x00000000, 0x00000006},
	{VID720x480P_60_16VS9,          "VID720x480P_60_16VS9",         0x0, 0x10, 0x01, 0x0, 0x035920c0, 0x00000000, 0x0b00503d, 0x00209029, 0x00000000, 0x03490079, 0x00000701, 0x00000701, 0x00000702, 0x00000000, 0x00000006},	
	{VID720x480P_60_4VS3,            "VID720x480P_60_4VS3",          0x0, 0x01, 0x01, 0x0, 0x035920c0, 0x00000000, 0x0b00503d, 0x00209029, 0x00000000, 0x03490079, 0x00000701, 0x00000701, 0x00000702, 0x00000000, 0x00000006},	
	{VID640x480P_60_4VS3,           "VID640x480P_60_4VS3",           0x0, 0x01, 0x01, 0x0, 0x031f20c0, 0x00000000, 0x120c05f,  0x00202022, 0x00000000, 0x030f008f, 0x00000701, 0x00000702,  0x00000703, 0x00000000, 0x00000006},	
	//{VID720x480P_59P94_4VS3,    "VID720x480P_59P94_4VS3",    0x0, 0x01, 0x01, 0x0, 0x035920c0, 0x00000000, 0x0b00503d, 0x00209029, 0x00000000, 0x03490079, 0x00000701, 0x00000701, 0x00000702, 0x00000000, 0x00000006},	
	//{VID720x480P_59P94_16VS9,   "VID720x480P_59P94_16VS9",  0x0, 0x10, 0x01, 0x0, 0x035920c0, 0x00000000, 0x0b00503d, 0x00209029, 0x00000000, 0x03490079, 0x00000701, 0x00000701, 0x00000702, 0x00000000, 0x00000006},	
	//the followings need modify
	//{VID720x480I_59P54_4VS3,      "VID720x480I_59P54_4VS3",       0x1, 0x01, 0x01, 0x1, 0x36b320c0, 0x0010c109, 0x0500207b, 0x00104014, 0x0020b11b, 0x068d00ed, 0x00001105, 0x0,            0x0,            0x00000000, 0x00000000},
	//{VID720x480I_59P54_16VS9,    "VID720x480I_59P54_16VS9",     0x1, 0x10, 0x01, 0x1, 0x36b320c0, 0x0010c109, 0x0500207b, 0x00104014, 0x0020b11b, 0x068d00ed, 0x00001105, 0x0,            0x0,            0x00000000, 0x00000006};
	//{VID720x480I_60_16VS9,         "VID720x480I_60_16VS9",            0x1, 0x10, 0x01, 0x1, 0x36b320c0, 0x0010c109, 0x0500207b, 0x00104014, 0x0020b11b, 0x068d00ed, 0x00001105, 0x0,            0x0,            0x00000000, 0x00000000},
};


static  struct de_video_mode hdmi_display_modes[] = {
	/*name              xres         pixclock        hbp        vbp          vsw             flag
	            refresh          yres               hfp         vfp          hsw         vmode        vid*/
	{"HDMI640x480P_60_4VS3", 60, 640, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID640x480P_60_4VS3},
	{"HDMI720x576P_50_4VS3", 50, 720, 576, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID720x576P_50_4VS3},
	{"HDMI720x576P_50_16VS9", 50, 720, 576, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID720x576P_50_16VS9},
	{"HDMI720x480P_59P94_4VS3", 59.94, 720, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID720x480P_59P94_4VS3},
	{"HDMIVID720x480P_59P94_16VS9", 59.94, 720, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID720x480P_59P94_16VS9},
	{"HDMI720x480I_59P54_4VS3", 59.94, 720, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_INTERLACED, FB_MODE_IS_STANDARD, VID720x480I_59P54_4VS3},
	{"HDMI720x480I_59P54_16VS9", 59.94, 720, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_INTERLACED, FB_MODE_IS_STANDARD, VID720x480I_59P54_16VS9},
	{"HDMI_720x576I_50_4VS3", 50, 720, 576, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_INTERLACED, FB_MODE_IS_STANDARD, VID1440x576I_50_4VS3},
	{"HDMI_720x576I_50_16VS9", 50, 720, 576, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_INTERLACED, FB_MODE_IS_STANDARD, VID720x576I_50_16VS9},
	{"HDMI_720x480P_60_4VS3", 60, 720, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID720x480P_60_4VS3},
	{"HDMI_720x480P_60_16VS9", 60, 720, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID720x480P_60_16VS9},
	{"HDMI_720x480I_60_4VS3", 60, 720, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_INTERLACED, FB_MODE_IS_STANDARD, VID1440x480I_60_4VS3},
	{"HDMI_720x480I_60_16VS9", 60, 720, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_INTERLACED, FB_MODE_IS_STANDARD, VID720x480I_60_16VS9},
	{"HDMI_1440x480P_60_4VS3", 60, 1440, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1440x480P_60_4VS3,},
	{"HDMI_1440x480P_60_16VS9", 60, 1440, 480, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1440x480P_60_16VS9},
	{"HDMI_1440x576P_50_4VS3", 50, 1440, 576, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1440x576P_50_4VS3},
	{"HDMI_1440x576P_50_16VS9", 50, 1440, 576, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1440x576P_50_16VS9},
	{"HDMI_1280x720P_60_16VS9", 60, 1280, 720, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1280x720P_60_16VS9},
	{"HDMI_1280x720P_50_16VS9", 50, 1280, 720, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1280x720P_50_16VS9},
	{"HDMI_1920x1080I_60_16VS9", 60, 1920, 1080, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_INTERLACED, FB_MODE_IS_STANDARD, VID1920x1080I_60_16VS9},
	{"HMDI_1920x1080I_50_16VS9", 50, 1920, 1080, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_INTERLACED, FB_MODE_IS_STANDARD, VID1920x1080I_50_16VS9},
	{"HDMI_1920x1080P_24_16VS9", 24, 1920, 1080, 0, 0, 0, 0, 0, 0, 0,  FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1920x1080P_24_16VS9},
	{"VID1920x1080P_25_16VS9", 25, 1920, 1080, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1920x1080P_25_16VS9},
	{"HDMI_1920x1080P_30_16VS9", 60, 1920, 1080, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1920x1080P_30_16VS9},
	{"HDMI_1920x1080P_60_16VS9", 60, 1920, 1080, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1920x1080P_60_16VS9}, 
	{"HDMI_1920x1080P_50_16VS9", 50, 1920, 1080, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1920x1080P_50_16VS9},
	{"HDMI_1280X720P_59P94_16VS9", 59.94, 1280, 720, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1280X720P_59P94_16VS9},
	{"VID1920x1080I_59P94_16VS9", 59.94, 1920, 1080, 0, 0, 0, 0, 0, 0,0, FB_VMODE_INTERLACED, FB_MODE_IS_STANDARD, VID1920x1080I_59P94_16VS9},
	{"HDMI_1920x1080P_29P97_16VS9", 29.97, 1920, 1080, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1920x1080P_29P97_16VS9},
	{"HDMI_1920x1080P_59P94_16VS9", 59.94, 1920, 1080, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, FB_MODE_IS_STANDARD, VID1920x1080P_59P94_16VS9} ,
};

/*audio*/
struct audio_settings default_a_settings = {
	.audio_channel = 1,
	.audio_fs = 3,
	.audio60958 = 1,
};

struct audio_parameters_t audio_parameters[] = {
/* audio60958       ASPCR               AudioCA
       audio_channel            ACACR*/
    {0x01, 0x001, 0x00000011, 0x00fac688, 0x00},
    {0x01, 0x010, 0x0002d713, 0x00004008, 0x04},
    {0x01, 0x011, 0x0003df1b, 0x00004608, 0x06},
    {0x01, 0x100, 0x0003df3b, 0x00034608, 0x0a},
    {0x01, 0x101, 0x0003df3f, 0x0002c688, 0x0b},
    {0x01, 0x110, 0x0007ff7f, 0x001ac688, 0x0f},
    {0x01, 0x111, 0x0007ffff, 0x00fac688, 0x13},
    {0x00, 0/*NULL*/,  0x7f87c003, 0x00fac688, 0/*NULL*/},
};

/*HDCP authen*/
#define HDCP_XMT_LINK_H0    0
#define HDCP_XMT_LINK_H1    1
#define HDCP_XMT_AUTH_A0    2
#define HDCP_XMT_AUTH_A1    3
#define HDCP_XMT_AUTH_A2    4
#define HDCP_XMT_AUTH_A3    5
#define HDCP_XMT_AUTH_A4    6
#define HDCP_XMT_AUTH_A5    7
#define HDCP_XMT_AUTH_A6    8
#define HDCP_XMT_AUTH_A7    9
#define HDCP_XMT_AUTH_A8    10
#define HDCP_XMT_AUTH_A9    11
#define HDCP_XMT_AUTH_A9_1  12

#define HOT_PLUG_PENDING    0x1
#define HDCP_RIUPDATED        0x2

#define HDCP_FAIL_TIMES       50
unsigned char key_r0[40][7];

#ifdef I2C_RW_DEBUG
unsigned char aksv[5] = { 
    0x1e, 
    0xc8, 
    0x42, 
    0xdd, 
    0xd9 
};
#else
unsigned char aksv[6] = { 
    0x10,
    
    0x1e, 
    0xc8, 
    0x42, 
    0xdd, 
    0xd9 
};
#endif

void *hdmi_find_id(int which_table, int len) {
    int i;

    //HDMI_DEBUG("[ %s start]\n", __func__);

    if (which_table == VIDEO_HDMI) {
        for (i = 0; i < len; i++) {
            if (video_parameters[i].vid == default_v_settings.vid) {
                hdmi.ip_data.v_cfg.parameters = &video_parameters[i];
                return (void *) &video_parameters[i];
            }
        }
    } else if (which_table == VIDEO_DE) {
        for (i = 0; i < len; i++) {
            if (hdmi_display_modes[i].vid == default_v_settings.vid) {
                hdmi.ip_data.v_cfg.mode = &hdmi_display_modes[i];
                return (void *) &hdmi_display_modes[i];
            }
        }
    } else if (which_table == AUDIO_HDMI) {
        for (i = 0; i < len; i++) {
            if (audio_parameters[i].audio60958
                    == default_a_settings.audio60958) {
                if (!audio_parameters[i].audio_channel) {
                    hdmi.ip_data.a_cfg.parameters = &audio_parameters[i];
                    return (void *) &audio_parameters[i];

                } else {

                    if (audio_parameters[i].audio_channel
                            == default_a_settings.audio_channel) {
                        hdmi.ip_data.a_cfg.parameters = &audio_parameters[i];
                        return (void *) &audio_parameters[i];
                    }
                }
            }
        }

    }
    //HDMI_DEBUG("[ %s finished]\n", __func__);

    return NULL;

}

void * hdmi_get_id(int which_table) {
    int len;

    //HDMI_DEBUG("[ %s start]\n", __func__);

    if (which_table == VIDEO_HDMI) {
        len = ARRAY_SIZE(video_parameters);

    } else if (which_table == VIDEO_DE) {
        len = ARRAY_SIZE(hdmi_display_modes);

    } else if (which_table == AUDIO_HDMI) {
        len = ARRAY_SIZE(audio_parameters);

    } else {
        printk("[%s]have no this kind of vid table!\n", __func__);
        return NULL;
    }

    return hdmi_find_id(which_table, len);
}

unsigned int analyze_hdmi_mode(unsigned char * edid_data) {

    unsigned char *start_addr;
    unsigned char *end_addr;
    unsigned int block_tag;
    unsigned int block_len;

    //HDMI_DEBUG("[ %s start]\n", __func__);

    start_addr = edid_data + 0x84;
    end_addr = edid_data + 0x80 + *(edid_data + 0x82);

    if ((*(edid_data + 0x7e) == 0x1) && (end_addr > start_addr)) {

        block_tag = ((*(start_addr)) >> 5) & 0x7;
        block_len = (*(start_addr)) & 0x1f;

        while ((block_tag != 3) && (end_addr > start_addr)) {

            start_addr += (block_len + 1);
            block_tag = ((*(start_addr)) >> 5) & 0x7;
            block_len = (*(start_addr)) & 0x1f;

        }

        if (block_tag != 3) {
            hdmi.ip_data.sink_cap.hdmi_mode = HDMI_MODE_DVI;
            printk("[%s]hdmi mode :%d\n",  hdmi.ip_data.sink_cap.hdmi_mode);
            return HDMI_MODE_DVI;

        } else {

            if ((*(start_addr + 1) == 0x03) && (*(start_addr + 2) == 0x0c)
                    && (*(start_addr + 3) == 0x00)) {

                hdmi.ip_data.sink_cap.hdmi_mode = HDMI_MODE_HDMI;
	  printk("[%s]hdmi mode :%d\n",  hdmi.ip_data.sink_cap.hdmi_mode);
                return HDMI_MODE_HDMI;

            } else {

                hdmi.ip_data.sink_cap.hdmi_mode = HDMI_MODE_DVI;
	  printk("[%s]hdmi mode :%d\n",  hdmi.ip_data.sink_cap.hdmi_mode);
                return HDMI_MODE_DVI;
            }

        }

    } else {

        hdmi.ip_data.sink_cap.hdmi_mode = HDMI_MODE_DVI;
        printk("[%s]hdmi mode :%d\n",  hdmi.ip_data.sink_cap.hdmi_mode);
        return HDMI_MODE_DVI;

    }
}

static int analyze_video_data_block(unsigned char *video_data,
        unsigned int block_len) {
    unsigned int i, j, k;
    unsigned int video_format;
    int index = 0;
    int offset = 0;
    //HDMI_DEBUG("[%s start]\n", __func__);

    for (i = 0; i < block_len; i++) {

        video_format = (unsigned int) ((*(video_data + i + 1)) & 0x7F);

        HDMI_DEBUG("[in %s]support video_format:%d", __func__, video_format);

        if (video_format >= VID_MAX) {

            HDMI_DEBUG("[in %s]incorrect ", __func__);
            HDMI_DEBUG("video data block!\n");
            return -1;

        }

        for (j = 0; j < ARRAY_SIZE(hdmi.ip_data.config.hdmi_vids); j++) {
            if (video_format == hdmi.ip_data.config.hdmi_vids[j]) {
                hdmi.ip_data.sink_cap.video_formats[(video_format - 1) / 32] |=
                        1 << ((video_format - 1) % 32);
            }

        }

    }
    
    for (i = 0, j = 0, k = 0; i < ARRAY_SIZE(video_parameters); i++) {         
         j = (video_parameters[i].vid - 1)  / 32;        
         k = (video_parameters[i].vid - 1)  % 32;        
         if (hdmi.ip_data.sink_cap.video_formats[j]  & (1 << k)) {       
             offset += sprintf(hdmi.vid_table + offset, "%s,%d;",
             video_parameters[i].vid_str,               
             video_parameters[i].vid);
     
             hdmi.orderd_table[index++] = video_parameters[i].vid;
            
         }
     }
    
    
    //HDMI_DEBUG("[%s finished]\n", __func__);

    return 0;
}

static int analyze_audio_data_block(unsigned char *audio_data,
        unsigned int block_len) {
    unsigned char data_read;
    unsigned char audio_format_code;
    unsigned int i;
    //unsigned int is_LPCM_info_got;

    //HDMI_DEBUG("[%s start]!\n", __func__);

    if (!audio_data)
        return -EINVAL;

    for (i = 0; i <= block_len; i++) {

        data_read = *(audio_data + 1 + i * 3);
        audio_format_code = (data_read >> 3) & 0x0F;

        if (audio_format_code == 1) { //liner PCM
            HDMI_DEBUG("[%s]support linear PCM!\n", __func__);

            hdmi.ip_data.sink_cap.max_channel_cap = (data_read & 0x07) + 1;
            HDMI_DEBUG("[%s]supported max audio ", __func__);
            HDMI_DEBUG("channel number is %d!\n",
                    hdmi.ip_data.sink_cap.max_channel_cap);

            hdmi.ip_data.sink_cap.sampling_rate_cap = (u32)(
                    (*(audio_data + 1 + i * 3 + 1)) & 0x7F);
            HDMI_DEBUG("[%s]supported audio sampling ", __func__);
            HDMI_DEBUG("frequency is 0x%x!\n",
                    hdmi.ip_data.sink_cap.sampling_rate_cap);

            hdmi.ip_data.sink_cap.sampling_size_cap = (u32)(
                    (*(audio_data + 1 + i * 3 + 2)) & 0x07);
            HDMI_DEBUG("[%s]supported audio sample size", __func__);
            HDMI_DEBUG(" is 0x%x\n", hdmi.ip_data.sink_cap.sampling_size_cap);

        } else {

            HDMI_DEBUG("[%s]don't support this audio code!\n", __func__);
            return -1;
        }
    }

    //HDMI_DEBUG("[ %s finished]\n", __func__);

    return 0;
}

s32 hdmi_sink_cap(unsigned char *edid_data) {
    unsigned char *ext_data;
    unsigned char *start_addr;
    unsigned char *end_addr;
    unsigned char data_block_header;
    unsigned int data_block_type;
    unsigned int data_block_length;
    //unsigned int video_format;

   // HDMI_DEBUG("[%s start]\n", __func__);

    if (!edid_data)
        return -EINVAL;

    mutex_lock(&mutex_sink_cap);

    mutex_lock(&mutex_sink_phy_addr);
    hdmi.sink_phy_addr = 0;
    mutex_unlock(&mutex_sink_phy_addr);

    ext_data = edid_data + HDMI_EDID_BLOCK_SIZE;

    /*hdmi mode*/
    if (analyze_hdmi_mode(edid_data) == HDMI_MODE_HDMI) {

        hdmi.ip_data.v_cfg.settings->hdmi_mode = HDMI_MODE_HDMI;

    } else {

        hdmi.ip_data.v_cfg.settings->hdmi_mode = HDMI_MODE_DVI;

    }

    /*pixel encoding,BIT0(RGB):this is always supported;
     BIT4(YCBCR=4:2:2);BIT5(YCBCR=4:4:4)*/
    hdmi.ip_data.sink_cap.pixel_encoding |= BIT0;

    if (((*(ext_data + 3)) & BIT4) && ((*(ext_data + 3)) & BIT5)) {

        hdmi.ip_data.sink_cap.pixel_encoding |= BIT1 | BIT2;
        printk("[%s]support YCbCr4:4:4 and YCbCr4:2:2!\n", __func__);

    } else if (((*(ext_data + 3)) & BIT4) || ((*(ext_data + 3)) & BIT5)) {

        HDMI_DEBUG("[%s]error in EDID.", __func__);
        HDMI_DEBUG("bit[4:5] of byte 3 in the first extension!\n");

    }
    printk("[%s]hdmi.ip_data.sink_cap.pixel_encoding is 0x%x\n", __func__,
            hdmi.ip_data.sink_cap.pixel_encoding);
    /*basic audio, channel=2;rate=32k,44.1k,48k;size=16bit*/
    if ((*(ext_data + 3)) & BIT6) {

        HDMI_DEBUG("[%s]support basic audio!\n", __func__);
        hdmi.ip_data.sink_cap.max_channel_cap = 2;
        hdmi.ip_data.sink_cap.sampling_rate_cap = BIT0 | BIT1 | BIT2;
        hdmi.ip_data.sink_cap.sampling_size_cap = BIT0;

    }

    /*data blocks:audio,video,speader_allocation,vendor_special*/
    start_addr = ext_data + 4;
    HDMI_DEBUG("detailed_timing_start_addr is 0x%x!\n", (u32) start_addr);
    end_addr = ext_data + (unsigned int) (*(ext_data + 2)) - 1;
    HDMI_DEBUG("detailed_timing_end_addr is 0x%x!\n", (unsigned int) end_addr);

    while (start_addr <= end_addr) {

        data_block_header = *start_addr;

        data_block_type = (unsigned int) (data_block_header >> 5);
        data_block_length = (unsigned int) (data_block_header & 0x1F);

        if ((start_addr + data_block_length) > end_addr)
            break;

        switch (data_block_type) {

        case AUDIO_DATA_BLOCK:
            analyze_audio_data_block(start_addr, data_block_length);
            break;

        case VIDEO_DATA_BLOCK:
            analyze_video_data_block(start_addr, data_block_length);
            break;

        case SPEAKER_ALLOOCATION_DATA_BLOCK:
            HDMI_DEBUG("[%s]speaker allocation", __func__);
            HDMI_DEBUG(" data block!\n");
            hdmi.ip_data.sink_cap.speader_allo_info =
                    (unsigned int) ((*(start_addr + 1)) & 0x7F);
            HDMI_DEBUG("[%s]speaker allocation value", __func__);
            HDMI_DEBUG(" is 0x%x!\n", hdmi.ip_data.sink_cap.speader_allo_info);
            break;

        case VENDOR_SPECIFIC_DATA_BLOCK:
            mutex_lock(&mutex_sink_phy_addr);
            hdmi.sink_phy_addr = (unsigned short) ((*(start_addr + 1 + 3))
                    << 8);
            hdmi.sink_phy_addr |= (unsigned short) (*(start_addr + 1 + 3 + 1));

            HDMI_DEBUG("[%s]device physical address ", __func__);
            HDMI_DEBUG("is 0x%04x!\n", hdmi.sink_phy_addr);
            mutex_unlock(&mutex_sink_phy_addr);

            break;

        default:
            break;
        }

        start_addr += data_block_length + 1;
    }

    mutex_unlock(&mutex_sink_cap);

    //HDMI_DEBUG("[%s finished]\n", __func__);

    return 0;
}

/*---------------------edid info----------------------*/
void hdmi_print_edid(unsigned char *p_buf) {
    int i;
    HDMI_DEBUG("\n\n\n=== EDID in doHDP() start===");
    for (i = 0; i < EDID_DATA_SIZE; i++) {
        printk(KERN_ALERT "0x%x:0x%x  ", i, *(p_buf + i));
    }
    mdelay(1);
    printk(KERN_ALERT "\n=== EDID in doHDP() end===\n\n\n");
    mdelay(1);

}
#if 0
s32 hdmi_read_edid(unsigned char *p_buf) {
    unsigned char ext_tag;
    int ret = 0;

    HDMI_DEBUG("[%s start]\n", __func__);
    /*read base edid block*/
    ext_tag = read_hdmi_edid_block(p_buf, 0, 0);
    if (ext_tag < 0) {

        ret = -EINVAL;
        return ret;

    }

    /*edid header*/
    if ((p_buf[0] == 0x00) && (p_buf[1] == 0xff) && (p_buf[2] == 0xff)
            && (p_buf[3] == 0xff) && (p_buf[4] == 0xff) && (p_buf[5] == 0xff)
            && (p_buf[6] == 0xff) && (p_buf[7] == 0x00)) {

        HDMI_DEBUG("[%s]find EDID header\n", __func__);

    } else {
        HDMI_DEBUG("[%s]fail to find EDID header\n", __func__);

        ret = -EINVAL;
        return ret;
    }

    /*read extention edid block*/
    if (ext_tag >= 1) {

        ret = read_hdmi_edid_block(p_buf + HDMI_EDID_BLOCK_SIZE, 0, 128);
        if (ret < 0) {

            ret = -EINVAL;
            return ret;

        }
    }

    if (ext_tag >= 1) {

        ret = read_hdmi_edid_block(p_buf + 2 * HDMI_EDID_BLOCK_SIZE, 1, 0);
        if (ret < 0) {

            ret = -EINVAL;
            return ret;

        }
    }

     if (ext_tag >= 1) {

        ret = read_hdmi_edid_block(p_buf + 3 * HDMI_EDID_BLOCK_SIZE, 1, 128);
        if (ret < 0) {

            ret = -EINVAL;
            return ret;

        }
    }

    HDMI_DEBUG("[ %s finished]\n", __func__);
    return ret;
}
#else
s32 hdmi_read_edid(unsigned char *p_buf) {
    int ret = 0;

    //HDMI_DEBUG("[%s start]\n", __func__);
    /*read base edid block*/
    ret = read_hdmi_edid_block(p_buf, 0, 0);
    if (ret < 0) {
    	printk("[%s]hdmi read edid failed\n", __func__);
    	return ret;
    }

    /*edid header*/
    if ((p_buf[0] == 0x00) && (p_buf[1] == 0xff) && (p_buf[2] == 0xff)
            && (p_buf[3] == 0xff) && (p_buf[4] == 0xff) && (p_buf[5] == 0xff)
            && (p_buf[6] == 0xff) && (p_buf[7] == 0x00)) {

        HDMI_DEBUG("[%s]find EDID header\n", __func__);

    } else {
        HDMI_DEBUG("[%s]fail to find EDID header\n", __func__);

        ret = -EINVAL;
        return ret;
    }

    if (!( *(p_buf + 0x7e) & 0x1)) {
        ret = 0;
        return ret;

    } 

    /*read extention edid block*/
    ret = read_hdmi_edid_block(p_buf + HDMI_EDID_BLOCK_SIZE, 0, 128);
    if (ret < 0) {

        ret = -EINVAL;
        return ret;

    }
    

     if ( *(p_buf + 0x7e) & 0x2) {

        ret = read_hdmi_edid_block(p_buf + 2 * HDMI_EDID_BLOCK_SIZE, 1, 0);
        if (ret < 0) {

            ret = -EINVAL;
            return ret;

        }

        ret = read_hdmi_edid_block(p_buf + 3 * HDMI_EDID_BLOCK_SIZE, 1, 128);
        if (ret < 0) {

            ret = -EINVAL;
            return ret;

        }
    }

    //HDMI_DEBUG("[ %s finished]\n", __func__);
    return ret;
}
#endif
int hdmi_analyze_edid(void) {

    s32 ret;

    //HDMI_DEBUG("[%s start]\n", __func__);
 
#if 0
    memset(hdmi.edid_data, 0, sizeof(u8) * 256);
#else
    memset(hdmi.edid_data, 0, sizeof(hdmi.edid_data));
#endif
    if (hdmi_read_edid(hdmi.edid_data) < 0) {
        printk("[%s]read edid data failed!\n", __func__);

#if 0
        memset(hdmi.edid_data, 0, sizeof(u8) * 256);
#else
       memset(hdmi.edid_data, 0, sizeof(hdmi.edid_data));
#endif
        return -EFAULT;

    }

    /* get sink device capabilities */
    ret = hdmi_sink_cap(hdmi.edid_data);

    if (ret != 0) {

        HDMI_DEBUG("[%s]read device capabilities fail!\n", __func__);
        return -EFAULT;

    } else {

        HDMI_DEBUG("[%s]success in reading ", __func__);
        HDMI_DEBUG("sink device capabilities!\n");
    }

    //HDMI_DEBUG("[%s finished]\n", __func__);

    return 0;
}

void hdmi_set_switch(int status) {
    switch_set_state(&hdmi.switch_data->sdev, status);
    printk("[%s]####switch_set_stat:%d#######\n",__func__, status);

}
//set this flag true, when hdmi auto mode
void hdmi_set_onoff_flag(bool flag) {
    mutex_lock(&hdmi_onoff_mutex);
    hdmi.onoff_flag = flag;
    HDMI_DEBUG("[%s]hdmi.onoff_flag :%d\n", __func__, hdmi.onoff_flag);
    mutex_unlock(&hdmi_onoff_mutex);
}
bool hdmi_get_onoff_flag(void) {
    bool flag;
    mutex_lock(&hdmi_onoff_mutex);
    flag = hdmi.onoff_flag;
    mutex_unlock(&hdmi_onoff_mutex);
    HDMI_DEBUG("[%s]hdmi.onoff_flag :%d\n", __func__, hdmi.onoff_flag);
    return flag;

}
//set false, when hdmi function is on in Settings menu
void hdmi_set_disconnect_flag(bool flag) {
    mutex_lock(&hdmi_disconnect_mutex);
    hdmi.disconnect_flag = flag;
    HDMI_DEBUG("[%s]hdmi.disconnect_flag :%d\n", __func__, hdmi.disconnect_flag);
    mutex_unlock(&hdmi_disconnect_mutex);
}
bool hdmi_get_disconnect_flag(void) {
    bool flag;
    mutex_lock(&hdmi_disconnect_mutex);
    flag = hdmi.disconnect_flag;
    mutex_unlock(&hdmi_disconnect_mutex);
    HDMI_DEBUG("[%s]hdmi.disconnect_flag :%d\n", __func__, hdmi.disconnect_flag);
    return flag;

}
//when  manual hdmi opts,set this flag.avoid conflict with switch auto.
void hdmi_set_manual_flag(bool flag) {
    mutex_lock(&hdmi_manual_mutex);
    hdmi.manual_flag = flag;
    HDMI_DEBUG("[%s]hdmi.manual_flag :%d\n", __func__, hdmi.manual_flag);
    mutex_unlock(&hdmi_manual_mutex);
}
bool hdmi_get_manual_flag(void) {
    bool flag;
    mutex_lock(&hdmi_manual_mutex);
    flag = hdmi.manual_flag;
    mutex_unlock(&hdmi_manual_mutex);
    HDMI_DEBUG("[%s]hdmi.manual_flag :%d\n", __func__, hdmi.manual_flag);
    return flag;

}
//avoid hpd irq for samsung,when disale hdmi in suspend
void hdmi_set_suspend_flag(bool flag) {
    mutex_lock(&hdmi_suspend_mutex);
    hdmi.suspend_flag = flag;
    HDMI_DEBUG("[%s]hdmi.suspend_flag :%d\n", __func__, hdmi.suspend_flag);
    mutex_unlock(&hdmi_suspend_mutex);
}
bool hdmi_get_suspend_flag(void) {
    bool flag;
    mutex_lock(&hdmi_suspend_mutex);
    flag = hdmi.suspend_flag;
    mutex_unlock(&hdmi_suspend_mutex);
    HDMI_DEBUG("[%s]hdmi.suspend_flag :%d\n", __func__, hdmi.suspend_flag);
    return flag;

}
//for vid_table_file, set true when file create success,or set false
void hdmi_set_vid_table_file_flag(bool flag) {
    mutex_lock(&hdmi_vid_table_mutex);
    hdmi.hdmi_vid_table_file_flag = flag;
    HDMI_DEBUG("[%s]hdmi.hdmi_vid_table_file_flag :%d\n", __func__,
            hdmi.hdmi_vid_table_file_flag);
    mutex_unlock(&hdmi_vid_table_mutex);
}
bool hdmi_get_vid_table_file_flag(void) {
    bool flag;
    mutex_lock(&hdmi_vid_table_mutex);
    flag = hdmi.hdmi_vid_table_file_flag;
    mutex_unlock(&hdmi_vid_table_mutex);
    HDMI_DEBUG("[%s]hdmi.hdmi_vid_table_file_flag :%d\n", __func__,
            hdmi.hdmi_vid_table_file_flag);
    return flag;

}
//before suspend,set this flag avoiding update hdmi from more than one path
void hdmi_set_pm_hibernation_prepare_flag(bool flag) {
    mutex_lock(&pm_hibernation_prepare_mutex);
    hdmi.pm_hibernation_prepare_flag = flag;
    HDMI_DEBUG("[%s]hdmi.pm_hibernation_prepare_flag :%d\n", __func__,
            hdmi.pm_hibernation_prepare_flag);
    mutex_unlock(&pm_hibernation_prepare_mutex);
}
bool hdmi_get_pm_hibernation_prepare_flag(void) {
    bool flag;
    mutex_lock(&pm_hibernation_prepare_mutex);
    flag = hdmi.pm_hibernation_prepare_flag;
    mutex_unlock(&pm_hibernation_prepare_mutex);
    HDMI_DEBUG("[%s]hdmi.pm_hibernation_prepare_flag :%d\n", __func__,
            hdmi.pm_hibernation_prepare_flag);
    return flag;

}

//store aksv key
void hdcp_set_aksv(unsigned char* _aksv) {
    int i = 0;
    for (i = 0; i < 5; i++) {
        aksv[i] = _aksv[i];
    }
}

//key
void hdcp_set_key(unsigned char *key) {
    int i = 0, j = 0;
    for (j = 0; j < 40; j++) {
        for (i = 0; i < 7; i++) {
            key_r0[j][i] = key[j * 7 + i];
        }
    }
}

int hdcp_anlay_SRM(unsigned char *srm, unsigned char *srm_buf) {
    unsigned int tmp = 0;
    int j = 0, i = 0, k = 0;
    if (srm != NULL) {
        tmp = srm[0];
    }
    if ((srm != NULL) && (srm_buf != NULL)) {
        if ((tmp & 0x80) != 0) { //hdcp flag
            tmp = srm[8];
            HDCP_DEBUG("there are %d device Number\n", tmp);
            for (i = 1; i <= (int) tmp; i++) {
                for (j = 4; j >= 0; j--) {
                    srm_buf[j + (i - 1) * 5] = srm[9 + k];
                    k++;
                }
            }

            return tmp;
        } else {
            return -1;
        }
    } else {
        return -1;
    }

}


/*
  * check if Bksv is valid, need have 20 "1"  and 20 "0"
  */
int Check_Bksv_Invalid(void) {
    int i, j;
    unsigned char counter = 0;
    unsigned char invalid_bksv[4][5] = {
        {0x0b,0x37,0x21,0xb4,0x7d},
        {0xf4,0xc8,0xde,0x4b,0x82},
        {0x23,0xde,0x5c,0x43,0x93},
        {0x4e,0x4d,0xc7,0x12,0x7c},
    };

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
            if (((hdmi.ip_data.hdcp.Bksv[i] >> j) & 0x1) != 0) {
                counter++;
            }
        }
    }

    if (counter != 20) {
        printk("[%s]fail\n", __func__);
        return 0;
    }

    for (i = 0; i < 4; i++) {
        for(j = 0; j < 5; j++) {
            if (invalid_bksv[i][j] == hdmi.ip_data.hdcp.Bksv[j]) {
                return 0;
            }
        }
    }

    printk("[%s]successful!\n", __func__);
    return 1;
}

static void hdcp_check_handle(void) {
    unsigned char An[9] = {0};
    unsigned char Bcaps = 0, Ri_temp[8] = {0};
    int i;
   // HDCP_DEBUG("[%s start]\n", __func__);
    if ((atomic_read(&hdmi_plugging) == HDMI_PLUGOUT) 
    	|| (atomic_read(&hdmi_enabled) == HDMI_DISABLE)) {
    	hdcp_authentication_switch(false);
              return;
    }
    //disable_hdcp_ri_interrupt();
    if(hdmi.ip_data.hdcp.hdcp_fail_times > HDCP_FAIL_TIMES) {//stop play
        hdmi.ip_data.hdcp.hdcp_authentication_success = false;
        printk("\n**********hdcp fail many times************ \n");
        goto end;
    }   

    if((hdmi.ip_data.hdcp.need_to_delay -= 50) > 0) {
        goto restart;
    }

    //state machine
    switch (hdmi.ip_data.hdcp.hdcpOper_state) {
    case HDCP_XMT_LINK_H0:
        printk("************HDCP_XMT_LINK_H0*************\n");
        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_LINK_H1;

    case HDCP_XMT_LINK_H1:
        printk("************HDCP_XMT_LINK_H1*************\n");
        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A0;
	
    /*
     * Authentication phase 1: A0,A1,A2,A3
     * A0.generate An, get Aksv;
     * A1.write An and Aksv, read Bksv; 
     * A2.get R0;
     * A3.computes Km, Ks, M0 and R0;
     *
     * Authentication phase 2:  A6  or  A6,A8,A9,A91
     * A6.check if sink support repeater, if not support repeater then authentication finish; 
     * A8. check if  KSV is ready;
     * A9:get KSV list;
     * A91:compare V value;
     *
     * Authentication phase 3: A4,A5
     * A4.set hdcp to Authenticated state;
     * A5.authentication successful
     */
    case HDCP_XMT_AUTH_A0: //Authentication phase 1
        printk("************HDCP_XMT_AUTH_A0*************\n");
        /* genrate An, get Aksv */
        hdmi.ip_data.hdcp.hdcpOper_retry = 0;
        //get An value
        hdcp_FreeRunGetAn(An);

       //send An
        printk("************send An*************\n");
#ifdef I2C_RW_DEBUG
        if (i2c_hdcp_write(&An[0], 0x18, 8) < 0) {
            HDCP_DEBUG("\n Write An error \n"); 
        }
#else
        if (i2c_hdcp_write(&An[0], 0x18, 9) < 0) {
            HDCP_DEBUG("\n Write An error \n"); 
        }
#endif
        //send aksv
        printk("************send aksv*************\n");  
#ifdef I2C_RW_DEBUG
       if (i2c_hdcp_write(aksv, 0x10, 5) < 0) {
            HDCP_DEBUG("\n Write aksv error \n"); 
        }
#else
        if (i2c_hdcp_write(aksv, 0x10, 6) < 0) {
            HDCP_DEBUG("\n Write aksv error \n"); 
        }
#endif
        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A1;
        hdmi.ip_data.hdcp.need_to_delay = 100;
        break;

    case HDCP_XMT_AUTH_A1://Authentication phase 1
        printk("************HDCP_XMT_AUTH_A1*************\n");
        /* 
         * write An and Aksv, read Bksv 
         * if get Bksv successful mean receiver/repeater support HDCP 
         * check valid Bksv 20 ones and 20 zero 
         */
        printk("************read Bksv*************\n");
        while (i2c_hdcp_read(hdmi.ip_data.hdcp.Bksv, 0x00, 5) < 0) {//if read successful, means support HDCP
            hdmi.ip_data.hdcp.i2c_error++;
            if (hdmi.ip_data.hdcp.i2c_error > 3) {
                hdmi.ip_data.hdcp.i2c_error = 0;
                HDCP_DEBUG("[631]Do not support HDCP \n");
                break;
            }
        }
        printk("************Check_Bksv*************\n");
        if (Check_Bksv_Invalid() == 0) {
            hdmi.ip_data.hdcp.hdcp_fail_times++;
            hdcp_ForceUnauthentication();
            hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_LINK_H0;
            HDCP_DEBUG("\n Check_Bksv_Invalid \n");
            hdmi.ip_data.hdcp.need_to_delay = 100;
            break;
        }

        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A2;
        hdmi.ip_data.hdcp.need_to_delay = 100;
        break;
   
    case HDCP_XMT_AUTH_A2://Authentication phase 1
        printk("************HDCP_XMT_AUTH_A2*************\n");
        // Computations
        // computes Km, Ks, M0 and R0   
        printk("************read Bcaps*************\n");
        if (i2c_hdcp_read(&Bcaps, 0x40, 1) < 0) {
            HDCP_DEBUG("Read Bcaps error \n"); //error_handle();
        }

        if ((Bcaps & (1 << 6)) != 0) {
            // set to support repeater 
            printk("************support repeater*************\n");
            hdmi.ip_data.hdcp.repeater = 1;
            enable_hdcp_repeater();
        } else {
            // set to support NO repeater
            printk("************dont  support repeater*************\n");
            hdmi.ip_data.hdcp.repeater = 0;
            disable_hdcp_repeater();
        }
        printk("************generate Ri*************\n");
        hdcp_AuthenticationSequence(&hdmi.ip_data);
        hdmi.ip_data.hdcp.hdcpOper_retry = 3; //add by keith

        /* if computed results are available */
        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A3;
        //wait for 100 msec to read R0p.        
        mdelay(110); // 建议不改为可调度

        break; //add by keith

    case HDCP_XMT_AUTH_A3://Authentication phase 1
        printk("************HDCP_XMT_AUTH_A3*************\n");
        // Validate Receiver
        // computes Km, Ks, M0 and R0
        // if computed results are available
         HDCP_DEBUG("************read R0*************\n");
        if (i2c_hdcp_read(Ri_temp, 0x08, 2) < 0) {
            memset(Ri_temp, 0, sizeof(Ri_temp)); 
        }
        hdmi.ip_data.hdcp.Ri_Read = (int) ((unsigned int) Ri_temp[1] << 8)
                | Ri_temp[0];
        printk("****Ri_Read:0x%x\n****", hdmi.ip_data.hdcp.Ri_Read);
        if (hdmi.ip_data.hdcp.Ri != hdmi.ip_data.hdcp.Ri_Read) {
            HDCP_DEBUG("\n R0 != Ri_Read \n");
            if (hdmi.ip_data.hdcp.hdcpOper_retry != 0) {
                hdmi.ip_data.hdcp.hdcpOper_retry--;
                hdmi.ip_data.hdcp.need_to_delay = 100;
            } else {
                /* authentication part I failed */
                hdmi.ip_data.hdcp.hdcp_fail_times++;
                hdcp_ForceUnauthentication();
                // restart
                hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A0;
                hdmi.ip_data.hdcp.need_to_delay = 200;
            }
            break;
        }
        //hdmi.ip_data.hdcp.hdcp_fail_times = 0;
        hdmi.ip_data.hdcp.hdcpOper_retry = 0;
        hdcp_read_hdcp_MILR(&hdmi.ip_data.hdcp.hdcpOper_M0[0]);
        hdcp_read_hdcp_MIMR(&hdmi.ip_data.hdcp.hdcpOper_M0[4]);
        // authentication part I successful 
        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A6;

    case HDCP_XMT_AUTH_A6://Authentication phase 2
        printk("************HDCP_XMT_AUTH_A6*************\n");
        // Test for Repeater
        // get REPEATER
        if (hdmi.ip_data.hdcp.repeater != 0) {
            // change to Authentication part II 
            hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A8;
            /* wait 100msec */
            hdmi.ip_data.hdcp.need_to_delay = 100;
            hdmi.ip_data.hdcp.retry_times_for_set_up_5_second = 0;
            break;
        }

        // NO repeater 
        // change to Authentication part III
        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A4;

    case HDCP_XMT_AUTH_A4: ////Authentication phase 3
        printk("************HDCP_XMT_AUTH_A4*************\n");
        // Authenticated 
        // set HDCP module to authenticated state      
        // start encryption
        set_hdcp_to_Authenticated_state();

        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A5;
        hdmi.ip_data.hdcp.hdcpOper_retry = 0;
        // enable Ri update check
        atomic_set(&hdcp_authentication_finished, 1);

        break;

    case HDCP_XMT_AUTH_A5://Authentication phase 3
        printk("************HDCP_XMT_AUTH_A5*************\n");
        /* Link Integrity Check */
        /* Interrupt and BH will do this job */
        //hdmi.ip_data.hdcp.hdcp_fail_times = 0;
        printk("********hdcp Authentication suceesful******** \n");
        enable_ri_update_check();
        hdmi.ip_data.hdcp.hdcp_authentication_success = true;
        return;

    case HDCP_XMT_AUTH_A8: ////Authentication phase 2
        printk("************HDCP_XMT_AUTH_A8*************\n");
        /* 2nd part authentication */
        /* Wait for Ready */
        /* set up 5 second timer poll for KSV list ready */
        if ((hdmi.ip_data.hdcp.retry_times_for_set_up_5_second % 5) == 0) {
            if (i2c_hdcp_read(&Bcaps, 0x40, 1) < 0) {
                HDCP_DEBUG("\n Read Bcaps err \n"); 
            }
        }
        if (!((Bcaps >> 5) & 0x1)) { //if KSVlist not ready!   
            if (hdmi.ip_data.hdcp.retry_times_for_set_up_5_second <= 50) { //100 msec * 50 = 5 sec
                hdmi.ip_data.hdcp.retry_times_for_set_up_5_second++;
                hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A8;
                /* wait 100msec */
                hdmi.ip_data.hdcp.need_to_delay = 100;
                break;
            } else {
                /* restart */
                hdmi.ip_data.hdcp.hdcp_fail_times++;
                hdmi.ip_data.hdcp.retry_times_for_set_up_5_second = 0;
                hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A0;
                hdcp_ForceUnauthentication();
                /* wait 100msec */
                hdmi.ip_data.hdcp.need_to_delay = 100;
                HDCP_DEBUG("\n retry_times_for_set_up_5_second > 50 \n");
                break;
            }

        }
        //hdmi.ip_data.hdcp.hdcp_fail_times = 0;
        hdmi.ip_data.hdcp.retry_times_for_set_up_5_second = 0;
        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A9;

    case HDCP_XMT_AUTH_A9://Authentication phase 2
         printk("************HDCP_XMT_AUTH_A9*************\n");
        /* Read KSV List and Bstatus */
        //hdcp_ReadKsvList(&hdcpOper.Bstatus, hdcpOper.ksvList);//mark by keith
        if (!hdcp_ReadKsvList(hdmi.ip_data.hdcp.Bstatus,
                hdmi.ip_data.hdcp.ksvList)) { //add by keith
            hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A0;
            hdcp_ForceUnauthentication();
            hdmi.ip_data.hdcp.hdcp_fail_times++;
            HDCP_DEBUG("\n hdcp_ReadKsvList \n");
            hdmi.ip_data.hdcp.need_to_delay = 100;

            break;
        }

        hdmi.ip_data.hdcp.need_to_delay = 100;
        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A9_1;
        break;

    case HDCP_XMT_AUTH_A9_1://Authentication phase 2
        printk("************HDCP_XMT_AUTH_A9_1*************\n");
        hdcp_ReadVprime(hdmi.ip_data.hdcp.Vp); //add by ciwu     
        if (!do_Vmatch(&hdmi.ip_data, hdmi.ip_data.hdcp.Vp, hdmi.ip_data.hdcp.ksvList,
                hdmi.ip_data.hdcp.Bstatus, hdmi.ip_data.hdcp.hdcpOper_M0)) {
            /* compare with V' */
            /* authentication part II failed */
            hdmi.ip_data.hdcp.hdcp_fail_times++;
            hdcp_ForceUnauthentication();
            hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A0;

            hdmi.ip_data.hdcp.need_to_delay = 100;
            HDCP_DEBUG("\n do_Vmatch \n");
            break;
        }

        /* KSV list correct , transit to Authentication Part III */
        hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A4;

        hdmi.ip_data.hdcp.need_to_delay = 100;

        break;

    default:
        break;
    }

restart: 
    queue_delayed_work(hdmi.ip_data.hdcp.wq,
            &hdmi.ip_data.hdcp.hdcp_work, msecs_to_jiffies(hdcp_timer_interval));
    return;
    
end: 
    hdcp_ForceUnauthentication();
    if (hdmi.ip_data.sink_cap.hdmi_mode == HDMI_MODE_HDMI) {
        switch_set_state(&hdmi.switch_data->sdev_audio, 2);
        msleep(100);
    }
    switch_set_state(&hdmi.switch_data->sdev, 0);
    switch_set_state(&hdmi.switch_data->sdev, 1);
    if (hdmi.ip_data.sink_cap.hdmi_mode == HDMI_MODE_HDMI)
        switch_set_state(&hdmi.switch_data->sdev_audio, 1);

    return;
}

void hdcp_init_config(void) {
   // HDCP_DEBUG("[%s start]\n", __func__);
    hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_AUTH_A0;
    hdmi.ip_data.hdcp.i2c_error = 0;
    hdmi.ip_data.hdcp.retry_times_for_set_up_5_second = 0;
    hdmi.ip_data.hdcp.hdcp_have_authentication_finished = 0;
    disable_hdcp_ri_interrupt();
    set_hdcp_ri_pj();
}

void hdcp_deinit_config(void) {
  //  HDCP_DEBUG("[%s start]\n", __func__);
    disable_hdcp_ri_interrupt();
    hdcp_ForceUnauthentication();
	
    hdmi.ip_data.hdcp.hdcp_fail_times = 0;
    hdmi.ip_data.hdcp.i2c_error = 0;
    hdmi.ip_data.hdcp.retry_times_for_set_up_5_second = 0;
    hdmi.ip_data.hdcp.hdcp_have_authentication_finished = 0;
}

int hdcp_authentication_switch(bool on) {
    if (hdmi.ip_data.config.hdcp_switch) {
 #ifdef READ_HDCP_KEY_FROM_FLASH
         if (!hdmi.ip_data.hdcp.read_hdcp_success) {
	 printk("[%s]hdcp key is error\n!", __func__);
	return 1;
        }
 #endif
        if (on) {
            printk("********************[%s on]*****************\n", __func__);
            if (atomic_read(&hdcp_have_goin_authentication)) {
            	 HDCP_DEBUG("[%s]hdcp_authentication_switch already on\n", __func__);
                return 1;
            } else {
                atomic_set(&hdcp_have_goin_authentication, 1);
            }
            hdcp_init_config();
            queue_delayed_work(hdmi.ip_data.hdcp.wq, &hdmi.ip_data.hdcp.hdcp_work,
                msecs_to_jiffies(0));

        } else {
            printk("********************[%s off]*****************\n", __func__);
            if (!atomic_read(&hdcp_have_goin_authentication)) {
            	 HDCP_DEBUG("[%s]hdcp_authentication_switch already off\n", __func__);
                return 1;
            } else {
                //hdmi.ip_data.hdcp.hdcp_have_goin_authentication = 0;
                atomic_set(&hdcp_have_goin_authentication, 0);
            }

            hdcp_deinit_config();
        }
    }
    
    return 0;

}
void hdcp_begin_to_check(unsigned int arg) {
    //HDCP_DEBUG("[%s start]\n", __func__);
    hdmi.ip_data.hdcp.hdcp_fail_times++;
    //cancel_delayed_work(&hdmi.ip_data.hdcp.hdcp_work);
    //flush_delayed_work(&hdmi.ip_data.hdcp.hdcp_work);
    hdcp_ForceUnauthentication();
    hdcp_init_config();

    queue_delayed_work(hdmi.ip_data.hdcp.wq, &hdmi.ip_data.hdcp.hdcp_work,
           msecs_to_jiffies(arg));

}

static void hdcp_error_handle(void) {
#if 0    
    int ret = 0;
    static int fail_times = 0;
    hdmi.ip_data.hdcp.hdcp_fail_times++;
    HDCP_DEBUG("[%s start]\n", __func__);
restart:  
    ret = hdcp_check_ri();
    if (ret != 0) {
        fail_times++;
        if (fail_times > 4) {
            fail_times = 0;
            goto end;
        } else {
            goto restart;
        }
    } else {
        hdmi.ip_data.hdcp.hdcp_fail_times = 0;
        return;
    }
end:
#endif
    disable_hdcp_ri_interrupt();
    hdcp_ForceUnauthentication();
    hdcp_begin_to_check(2100);  //delayed 2s
}

static void hdmi_switch_work(struct work_struct *work) {
    int state;
    int r;
    int i;

    struct hdmi_switch_data *data =
        container_of(work, struct hdmi_switch_data, work);
    struct de_video_mode *mode;
    unsigned char Ri_temp[8];

    //HDMI_DEBUG("[ %s start]\n", __func__);

    /*hdmi hpd handle*/
    if (data->pending_state & HOT_PLUG_PENDING) {
        data->pending_state &= ~HOT_PLUG_PENDING;
	
        if (hdmi_get_suspend_flag())
            return;

        state = hdmi.ip_data.ops->detect_plug(&hdmi.ip_data);
        //printk("[%s]plug state is %d\n", __func__, state);

        if (state == HDMI_PLUGIN) { //detect plugin
            msleep(50);
            state = hdmi.ip_data.ops->detect_plug(&hdmi.ip_data);
            if (state == HDMI_PLUGIN) {
	  printk("[%s]hdmi plugin \n", __func__);
                HDMI_DEBUG("[%s]ready to read EDID data!\n", __func__);

                if (atomic_read(&hdmi_plugging) == HDMI_PLUGIN) {
                    printk("[%s]already plug in\n", __func__);
                    return;
                }

                atomic_set(&hdmi_plugging, HDMI_PLUGIN);

                if (hdmi_analyze_edid() < 0) {
                    mutex_lock(&mutex_sink_cap);
                    hdmi.sink_cap_available = false;
                    mutex_unlock(&mutex_sink_cap);
                    printk("[%s]fail to get correct EDID!\n", __func__);
                } else {
                    hdmi.sink_cap_available = true;

                    if (!hdmi_get_vid_table_file_flag()) {
                        r = device_create_file(hdmi_miscdev.this_device,
                                &hdmi_vid_table_attr);
                        if (r) {
                            dev_err(
                                    hdmi_miscdev.this_device,
                                    "failed to create hdmi_vid_table_attr sysfs file\n");
                        } else {
                            hdmi_set_vid_table_file_flag(true);
                        }
                    }
                    if (hdmi.ip_data.sink_cap.hdmi_mode == HDMI_MODE_HDMI) {
                        if (!(hdmi.ip_data.sink_cap.pixel_encoding & BIT1)) {
                            hdmi.ip_data.v_cfg.settings->pixel_encoding =
                                VIDEO_PEXEL_ENCODING_RGB;
                            //act_clearl(HDMI_SCHCR_PIXELENCFMT(0x3), HDMI_SCHCR);//YUV444
                            hdmi.ip_data.ops->pixel_coding(
                                hdmi.ip_data.v_cfg.settings);

                        } else {
                             hdmi.ip_data.v_cfg.settings->pixel_encoding =
                                VIDEO_PEXEL_ENCODING_YCbCr444;
                            //act_clearl(HDMI_SCHCR_PIXELENCFMT(0x3), HDMI_SCHCR);//YUV444
                            hdmi.ip_data.ops->pixel_coding(
                                hdmi.ip_data.v_cfg.settings);
	          }

                    }else if (hdmi.ip_data.sink_cap.hdmi_mode == HDMI_MODE_DVI) {
                         hdmi.ip_data.v_cfg.settings->pixel_encoding =
                                VIDEO_PEXEL_ENCODING_RGB;
                            //act_clearl(HDMI_SCHCR_PIXELENCFMT(0x3), HDMI_SCHCR);//YUV444
                            hdmi.ip_data.ops->pixel_coding(
                                hdmi.ip_data.v_cfg.settings);
                    }
			

                }


                if (!hdmi_get_disconnect_flag() && !hdmi_get_onoff_flag()) {
	      if (hdmi.ip_data.config.hdcp_switch) {
#ifdef READ_HDCP_KEY_FROM_FLASH
	          if (hdmi.ip_data.hdcp.read_hdcp_success) {
                            switch_set_state(&data->sdev, state);
                            printk("[%s]#####switch_set_state:%d##########\n", __func__, state);
                            msleep(4000);
                            hdcp_authentication_switch(true);
	          } else {
                          
                           switch_set_state(&data->sdev, state);
	          }
#else
                        switch_set_state(&data->sdev, state);
                        printk("[%s]############switch_set_state:%d###########\n",__func__, state);
                        msleep(4000);
                        hdcp_authentication_switch(true);
#endif
                    
                    }else {
                        switch_set_state(&data->sdev, state);
	      }
                }

            }

        } else if (state == HDMI_PLUGOUT) { //detect plugout
            
            if (atomic_read(&hdmi_plugging) == HDMI_PLUGOUT) {
                printk("[%s]already pull out\n", __func__);
                return;

            }
            printk("[%s]already plug out\n", __func__);
	    
            atomic_set(&hdmi_plugging, HDMI_PLUGOUT);
            if (hdmi.ip_data.sink_cap.hdmi_mode == HDMI_MODE_HDMI) {
                switch_set_state(&data->sdev_audio, 2);
                msleep(100);
            }
	    
            switch_set_state(&data->sdev, state);

            hdmi.ip_data.hdcp.hdcp_authentication_success = true;
            hdmi.ip_data.v_cfg.settings->vid = VID1280x720P_50_16VS9;
            /*mode = (struct de_video_mode *)hdmi_get_id(VIDEO_DE);
            if (mode) {
                hdmi_data->disp_cur_mode = mode;           
                hdmi_data->fb_blank = FB_BLANK_UNBLANK;
		
            } else {
                HDMI_DEBUG("[%s]don't support this hdmi_display_mode!\n", __func__);
            }*/


            mutex_lock(&mutex_sink_cap);
            memset(&hdmi.ip_data.sink_cap, 0,
                    sizeof(struct sink_capabilities_t));
            hdmi.sink_cap_available = false;
            memset(hdmi.edid_data, 0, sizeof(hdmi.edid_data));
            memset(hdmi.vid_table, 0, sizeof(hdmi.vid_table));
            if (hdmi_get_vid_table_file_flag()) {
                device_remove_file(hdmi_miscdev.this_device,
                        &hdmi_vid_table_attr);
                hdmi_set_vid_table_file_flag(false);
            }
            mutex_unlock(&mutex_sink_cap);
        }
    }
#if 1
     /*Ri update*/
        if (atomic_read(&hdcp_authentication_finished)) {
            /*hdcp Ri update handle*/
            if (data->pending_state & HDCP_RIUPDATED) {
                data->pending_state &= ~HDCP_RIUPDATED;

                if (hdmi.ip_data.hdcp.Ri >= 0) {
                    if (hdmi.ip_data.hdcp.hdcpOper_state == HDCP_XMT_AUTH_A5) {
                        if (i2c_hdcp_read(Ri_temp, 0x08, 2) < 0) {
                            memset(Ri_temp, 0, sizeof(Ri_temp)); //error_handle();
                        }
                        hdmi.ip_data.hdcp.Ri_Read = (Ri_temp[1] << 8)
                                | Ri_temp[0];
	          HDCP_DEBUG("[enter %s]Ri(master):0x%x\n", __func__, hdmi.ip_data.hdcp.Ri);
	          HDCP_DEBUG("[enter %s]Ri(slave):0x%x\n", __func__, hdmi.ip_data.hdcp.Ri_Read );
                        if (hdmi.ip_data.hdcp.Ri != hdmi.ip_data.hdcp.Ri_Read) {
                            //hdcp_begin_to_check(HDCP_AUTHENTICATE_TIME);  //重新进行hdcp验证
                            //trigger irq every 128 frams, if 60hz, almost irq/2ms
                            hdcp_ForceUnauthentication();
                            HDCP_DEBUG("[%s]Ri !=Ri_Read\n", __func__);
                            hdcp_error_handle();
                            
                        }
                    }
                }
            }
        }
#endif

}

static irqreturn_t hdmi_irq_handler(s32 irq, void *dev_id) {

    struct hdmi_switch_data *data = (struct hdmi_switch_data *) dev_id;

    //HDMI_DEBUG("[%s start]\n", __func__);
    /*hot plug*/
    if (hdmi.ip_data.ops->detect_pending(&hdmi.ip_data)) {
        printk("[%s]hot plug irq happend!\n", __func__);
        data->pending_state = HOT_PLUG_PENDING;
    }

#if 1
    /*Ri update*/
    if (hdcp_detect_riupdated()) {
        printk("[%s]Ri update irq happend!\n", __func__);
        data->pending_state |= HDCP_RIUPDATED;
        hdcp_get_ri(&hdmi.ip_data);
    }

#endif
    schedule_work(&data->work);

    //HDMI_DEBUG("[%s finished]\n", __func__);

    return IRQ_HANDLED;

}

void hdmi_hpd_enable(struct asoc_display_device *disp_dev) {
    //HDMI_DEBUG("[ %s start]\n", __func__);
    hdmi.ip_data.ops->hpd_enable(&hdmi.ip_data);
    //HDMI_DEBUG("[ %s finished]\n", __func__);
}
/*
  * for init and resume
  *
  */
void hdmi_init_config(struct asoc_display_device *disp_dev) {

   // HDMI_DEBUG("[ %s start]\n", __func__);
    
    hdmi.ip_data.ops->get_configure(&hdmi.ip_data);
    if (hdmi.ip_data.config.boot_disp_cfg[0] == 3) {
        atomic_set(&hdmi_timing_set, HDMI_TIMING_ENABLE);
    } else if (hdmi.ip_data.config.boot_disp_cfg[0] == 1) {
        atomic_set(&hdmi_timing_set, HDMI_TIMING_DISABLE);
    }
    //check if hdmi is inited depend on 'hdmi hpd enable' bit
    if (hdmi.ip_data.ops->is_hpdint_enabled(&hdmi.ip_data) == HPDINT_ENABLE) {
        /* when boot from hdmi, normal boot && quickboot resume walk this path
         * when boot from lcd,dont walk this path
         */
        if (hdmi.ip_data.ops->detect_plug(&hdmi.ip_data) == HDMI_PLUGIN) { //plug in

            //phase 0:get edid info
            if (hdmi_analyze_edid() < 0) {
                mutex_lock(&mutex_sink_cap);
                hdmi.sink_cap_available = false;
                mutex_unlock(&mutex_sink_cap);
                HDMI_DEBUG("fail to get correct EDID!\n");
            } else {
                mutex_lock(&mutex_sink_cap);
                hdmi.sink_cap_available = true;
                mutex_unlock(&mutex_sink_cap);

                if (!hdmi_get_vid_table_file_flag()) {
                    int r = device_create_file(hdmi_miscdev.this_device,
                            &hdmi_vid_table_attr);
                    if (r) {
                        dev_err(
                                hdmi_miscdev.this_device,
                                "failed to create hdmi_vid_table_attr sysfs file\n");
                    } else {
                        hdmi_set_vid_table_file_flag(true);
                    }
                }

                if (!(hdmi.ip_data.sink_cap.pixel_encoding & BIT1)) {
                    hdmi.ip_data.v_cfg.settings->pixel_encoding =
                            VIDEO_PEXEL_ENCODING_RGB;
                    hdmi.ip_data.ops->pixel_coding(hdmi.ip_data.v_cfg.settings);
                } else {
                            hdmi.ip_data.v_cfg.settings->pixel_encoding =
                                VIDEO_PEXEL_ENCODING_YCbCr444;
                            //act_clearl(HDMI_SCHCR_PIXELENCFMT(0x3), HDMI_SCHCR);//YUV444
                            hdmi.ip_data.ops->pixel_coding(
                                hdmi.ip_data.v_cfg.settings);
                }
            }

            //phase 1:enable switch work
            if (hdmi_get_pm_hibernation_prepare_flag())
                hdmi_set_pm_hibernation_prepare_flag(false);
            hdmi_set_onoff_flag(false);
           
            if (hdmi_get_suspend_flag())
                hdmi_set_suspend_flag(false);
            //phase 2:set hdmi menu on, in order to response interrupt
            hdmi_set_disconnect_flag(false);

            //phase 3:enable update_mode or update_status for de
            if (hdmi_get_pm_hibernation_prepare_flag())
                hdmi_set_pm_hibernation_prepare_flag(false);

            //phase 4:set hdmi plugin
            atomic_set(&hdmi_plugging, HDMI_PLUGIN);

            //phase 5: add clk reference
            hdmi_timing_cfg(disp_dev);

            //phase 6:set hdmi enable
            atomic_set(&hdmi_enabled, HDMI_ENABLE);

            //phase 7:send uevent
            switch_set_state(&hdmi.switch_data->sdev, 1);
            printk("[%s]#######switch_set_state:1###########\n",__func__);
            if (hdmi.ip_data.sink_cap.hdmi_mode == HDMI_MODE_HDMI) 
                switch_set_state(&hdmi.switch_data->sdev_audio, 1);

        } else {
            //phase 0:clear irq pending
            hdmi.ip_data.ops->detect_pending(&hdmi.ip_data);

            //phase 1:set hdmi plugout
            atomic_set(&hdmi_plugging, HDMI_PLUGOUT);
            mutex_lock(&mutex_sink_cap);
            memset(&hdmi.ip_data.sink_cap, 0,
                    sizeof(struct sink_capabilities_t));
            hdmi.sink_cap_available = false;
            memset(hdmi.edid_data, 0, sizeof(hdmi.edid_data));
            memset(hdmi.vid_table, 0, sizeof(hdmi.vid_table));
            mutex_unlock(&mutex_sink_cap);

            //phase 2:delete hdmi_vid_table attr file
            if (hdmi_get_vid_table_file_flag()) {
                device_remove_file(hdmi_miscdev.this_device,
                        &hdmi_vid_table_attr);
                hdmi_set_vid_table_file_flag(false);
            }

            //phase 3:enable switch work
            if (hdmi_get_suspend_flag())
                hdmi_set_suspend_flag(false);
            hdmi_set_onoff_flag(false);

            //phase 4:enable update_mode or update_status for de
            if (hdmi_get_pm_hibernation_prepare_flag())
                hdmi_set_pm_hibernation_prepare_flag(false);

            //phase 5:set hdmi menu on, in order to response interrupt
            hdmi_set_disconnect_flag(false);
            //phase 6: add clk reference
            hdmi_timing_cfg(disp_dev);
            //phase 7::set hdmi enable
            atomic_set(&hdmi_enabled, HDMI_ENABLE);
            //phase 8:send uevent
            switch_set_state(&hdmi.switch_data->sdev, 0);
            printk("[%s]####switch_set_state:0##############\n",__func__);
            if (hdmi.ip_data.sink_cap.hdmi_mode == HDMI_MODE_HDMI) 
                switch_set_state(&hdmi.switch_data->sdev_audio, 2);

        }
        /*
         * when boot from hdmi, normal resume walk this path
         *  when boot from lcd, normal boot, normal resume and quickboot resume walk this path
         */
    } else {
        //phase 0:set hdmi plugout
        atomic_set(&hdmi_plugging, HDMI_PLUGOUT);

        //phase 1:enable switch work
        if (hdmi_get_suspend_flag())
            hdmi_set_suspend_flag(false);
        hdmi_set_onoff_flag(false);

        //phase 2:enable update_mode or update_status for de
        if (hdmi_get_pm_hibernation_prepare_flag())
            hdmi_set_pm_hibernation_prepare_flag(false);

        //phase 3:
        atomic_set(&hdmi_enabled, HDMI_DISABLE);
        //phase 4:init
        hdmi.ip_data.ops->init_configure(&hdmi.ip_data);

        //phase 4:
        disp_dev->is_plugged = hdmi.ip_data.ops->detect_plug(&hdmi.ip_data);
    }

    HDMI_DEBUG("[ %s finished]\n", __func__);

}

int hdmi_basic_init(struct asoc_display_device *disp_dev) {
    int ret = 0;
    struct de_video_mode *mode;

    //HDMI_DEBUG("[ %s start]\n", __func__);

    asoc_init_hdmi_ip_ops(&hdmi.ip_data);
    
    hdmi.ip_data.v_cfg.settings = &default_v_settings;
    hdmi.sink_cap_available = false;

    hdmi_set_onoff_flag(false);
    hdmi_set_disconnect_flag(false);
    hdmi_set_manual_flag(false);
    hdmi_set_suspend_flag(false);
    hdmi_set_vid_table_file_flag(false);

    disp_dev->disp_priv_info = &hdmi;

    /*switch for hdmi*/
    hdmi.switch_data = kzalloc(sizeof(struct hdmi_switch_data), GFP_KERNEL);
    hdmi.switch_data->sdev.name = "hdmi";
    hdmi.switch_data->sdev_audio.name = "hdmi_audio";
    INIT_WORK(&hdmi.switch_data->work, hdmi_switch_work);

    memset(hdmi.orderd_table, 0, sizeof(hdmi.orderd_table));

    ret = switch_dev_register(&hdmi.switch_data->sdev);
    if (ret < 0)
        return ret;

    ret = switch_dev_register(&hdmi.switch_data->sdev_audio);
    if (ret < 0)
        return ret;

    /*general config, when insmod*/
    /*mode = (struct de_video_mode *) hdmi_get_id(VIDEO_DE);
    if (mode) {
        disp_dev->disp_cur_mode = mode;
        disp_dev->fb_blank = FB_BLANK_UNBLANK;

    } else {
        HDMI_DEBUG("[%s]don't support this hdmi_display_mode!\n", __func__);
    }*/

    hdmi_init_config(disp_dev);

        /*hdcp*/
    memset(&hdmi.ip_data.hdcp, 0, sizeof(struct hdmi_hdcp));
	
#ifdef READ_HDCP_KEY_FROM_FLASH
    if (hdmi.ip_data.config.hdcp_switch) {
        if (!hdcp_read_key()) {
            hdmi.ip_data.hdcp.read_hdcp_success = true;
        } else {
            hdmi.ip_data.hdcp.read_hdcp_success = false;
        }
    }
#endif
    hdmi.ip_data.hdcp.hdcp_authentication_success = true;
    hdmi.ip_data.hdcp.hdcpOper_state = HDCP_XMT_LINK_H0;
    INIT_DELAYED_WORK(&hdmi.ip_data.hdcp.hdcp_work, hdcp_check_handle);
    INIT_DELAYED_WORK(&hdmi.ip_data.hdcp.hdcp_err_work, hdcp_error_handle);
    hdmi.ip_data.hdcp.wq = create_workqueue("gl5202hdmi-hdcp");
    
    /*irq*/
    ret = request_irq(IRQ_ASOC_HDMI, hdmi_irq_handler, 0, "asoc_hdmi",
            hdmi.switch_data);
    if (ret < 0)
        return ret;

    //HDMI_DEBUG("[ %s finished]\n", __func__);
    return 0;

}

int hdmi_get_link_status(struct asoc_display_device *disp_dev) {
    HDMI_DEBUG("[ %s]hdmi plug state:%d\n", __func__, hdmi.ip_data.ops->detect_plug(&hdmi.ip_data));
    return hdmi.ip_data.ops->detect_plug(&hdmi.ip_data);
    //HDMI_DEBUG("[ %s finished]\n", __func__);
}

int hdmi_set_mode(struct asoc_display_device *disp_dev) {
    //HDMI_DEBUG("[ %s start]\n", __func__);
    return hdmi.ip_data.ops->set_mode(&hdmi.ip_data);
    //HDMI_DEBUG("[ %s finished]\n", __func__);
}

void hdmi_enable_output(struct asoc_display_device *disp_dev) {
    //HDMI_DEBUG("[ %s start]\n", __func__);
    if (atomic_read(&hdmi_enabled) == HDMI_ENABLE) {
        printk("[ %s]hdmi already enable!\n", __func__);
        return;
    }

    hdmi.ip_data.ops->phy_enable(&(hdmi.ip_data));

    atomic_set(&hdmi_enabled, HDMI_ENABLE);

    if (hdmi_get_link_status(disp_dev) && !hdmi_get_disconnect_flag()) {
        if (hdmi.ip_data.sink_cap.hdmi_mode == HDMI_MODE_HDMI)
            switch_set_state(&hdmi.switch_data->sdev_audio, 1);
    }

}

void hdmi_disable_output(struct asoc_display_device *disp_dev) {
    //HDMI_DEBUG("[ %s start]\n", __func__);
    if (atomic_read(&hdmi_enabled) == HDMI_DISABLE) {
        HDMI_DEBUG("[ %s]hdmi already disable!\n", __func__);
        return;
    }
    hdcp_authentication_switch(false);
    hdmi.ip_data.ops->phy_disable(&hdmi.ip_data);
    
    atomic_set(&hdmi_enabled, HDMI_DISABLE);

    if (hdmi.ip_data.sink_cap.hdmi_mode == HDMI_MODE_HDMI) 
        switch_set_state(&hdmi.switch_data->sdev_audio, 2);

    if (atomic_read(&hdmi_timing_set) == HDMI_TIMING_ENABLE) {
        if (set_safe_status(HDMI_TIMING_DISABLE)) {
            HDMI_DEBUG("%s set_safe_status(HDMI_TIMING_DISABLE) error!\n",
                    __func__);

        } else {
            atomic_set(&hdmi_timing_set, HDMI_TIMING_DISABLE);
            HDMI_DEBUG("%s set_safe_status(HDMI_TIMING_DISABLE) success!\n",
                    __func__);
        }
    }

    //HDMI_DEBUG("[ %s finished]\n", __func__);
}

void hdmi_timing_cfg(struct asoc_display_device *disp_dev) {
    //HDMI_DEBUG("[ %s start]\n", __func__);
    if (atomic_read(&hdmi_timing_set) == HDMI_TIMING_DISABLE) {
        if (set_safe_status(HDMI_TIMING_ENABLE)) {
            HDMI_DEBUG("%s set_safe_status(HDMI_TIMING_ENABLE) error!\n",
                    __func__);

        } else {
            atomic_set(&hdmi_timing_set, HDMI_TIMING_ENABLE);
            HDMI_DEBUG("%s set_safe_status(HDMI_TIMING_ENABLE) success!\n",
                    __func__);
        }
    }

    hdmi.ip_data.ops->timing_configure(&hdmi.ip_data);
}

int hdmi_video_cfg(struct asoc_display_device *disp_dev) {
    struct video_parameters_t *para;

    //HDMI_DEBUG("[ %s start]\n", __func__);

    para = hdmi_get_id(VIDEO_HDMI);
    if (para) {
        hdmi.ip_data.ops->video_configure(&hdmi.ip_data);
    } else {
        printk("[%s]don't support this format!\n", __func__);
        return -EINVAL;
    }

    //HDMI_DEBUG("[ %s finished]\n", __func__);
    return 0;
}

int hdmi_packet_cfg(struct asoc_display_device *disp_dev) {

    //HDMI_DEBUG("[ %s start]\n", __func__);

    if (hdmi.ip_data.ops->gcp_configure(&hdmi.ip_data))
        return -EINVAL;

    if (hdmi.ip_data.ops->gen_infoframe(&hdmi.ip_data))
        return -EINVAL;

    return 0;
}

void hdmi_dump_regs(struct asoc_display_device *disp_dev) {
    //HDMI_DEBUG("[ %s start]\n", __func__);
#ifdef DUMP_DE_REG
    hdmi.ip_data.ops->dump_de(&hdmi.ip_data);
#endif
    hdmi.ip_data.ops->dump_hdmi(&hdmi.ip_data);
    //HDMI_DEBUG("[ %s finished]\n", __func__);
}
