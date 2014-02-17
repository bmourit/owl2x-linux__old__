#ifndef __ASOC_TVOUT_H__
#define __ASOC_TVOUT_H__


#include <linux/switch.h>

#define USE_DEBOUNCE	1
#define CVBS_MINOR  22

enum TV_MODE_TYPE {
	TV_MODE_PAL = 0,
	TV_MODE_NTSC = 1,
	TV_MODE_MAX = 2
};

enum TV_ENCODER_TYPE {
	TV_ENCODER_INTERNAL,
	TV_ENCODER_EXTERNAL,
	TV_ENCODER_MAX
};

enum TVOUT_DEV_STATUS {
	TVOUT_DEV_PLUGOUT,
	TVOUT_DEV_PLUGIN,
	TVOUT_DEV_MAX
};

struct tv_settings {
	unsigned int tv_mode;		
	unsigned int encoder_type;	
};

struct cvbs_switch_data {
	struct switch_dev sdev;
	const char *name_on;
	const char *name_off;
	const char *state_on;
	const char *state_off;
	int status_val;
	struct work_struct work;
};

struct cvbs_manager {	
	atomic_t cvbs_plugin_queue_atomic;
	atomic_t cvbs_status;
	atomic_t cvbs_status_changed;
	atomic_t cvbs_enabled;
	
	struct task_struct *cvbs_plugin_task;
#if (USE_DEBOUNCE)
	#define DEBOUNCE_WAITTIME	200
	#define RE_ENABLE_WAITTIME	20
#endif 
	struct tv_settings cvbs_default_settings;
	struct de_video_mode *cvbs_display_mode;

	struct cvbs_switch_data switch_data;

	void *private_data;

};

#endif /* __ASOC_TVOUT_H__ */
