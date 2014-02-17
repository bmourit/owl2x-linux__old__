#ifndef __ACT_DE_ATTR_SYS__H__
#define __ACT_DE_ATTR_SYS__H__
#include "display.h"
#include "de_core.h"
#include <linux/platform_device.h>
/*****************************************************************************	
*system info attr
*****************************************************************************/	
ssize_t system_info_show(struct bus_type *bus, char *buf);

/*****************************************************************************	
*display manager  attr
*****************************************************************************/
int disp_mgr_attr_init(struct disp_manager * disp_mgr, struct de_core * core);

/*****************************************************************************	
*layer manager  attr
*****************************************************************************/
int layer_mgr_attr_init(struct layer_manager * layer_mgr, struct de_core * core);

/*****************************************************************************	
*mdsb  manager  attr
*****************************************************************************/
int mdsb_mgr_attr_init(struct mdsb_manager * mdsb_mgr, struct de_core * core);

int de_vsync_create_attrs(struct device * dev);

void vsync_set_state(struct de_core * core, int state);

void vsync_set_counter(struct de_core * core, int counter);

#endif