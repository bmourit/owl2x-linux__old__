
/*
 * module common macro
 *
 * Copyright (C) 2008 Renesas Solutions Corp.
 * Kuninori Morimoto <morimoto.kuninori@renesas.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __MODULE_COMM_H__
#define __MODULE_COMM_H__

#include <media/soc_camera.h>

/* for flags */
#define MODULE_FLAG_VFLIP	(1 << 0) /* Vertical flip image */
#define MODULE_FLAG_HFLIP	(1 << 1) /* Horizontal flip image */
#define MODULE_FLAG_8BIT	(1 << 2) /* default 8 bit */
#define MODULE_FLAG_10BIT	(1 << 3) /*  10 bit interface*/
#define MODULE_FLAG_12BIT	(1 << 4) /* 12 bit interface*/
#define MODULE_FLAG_FRONT	(1 << 5) /* posization front */
#define MODULE_FLAG_BACK	(1 << 6) /* posization back */
#define MODULE_FLAG_PALL	(1 << 7) /* parellal interface */
#define MODULE_FLAG_MIPI	(1 << 8) /* mipi interface */
#define MODULE_FLAG_CHANNEL1	(1 << 9) /* use isp channel1 */
#define MODULE_FLAG_CHANNEL2	(1 << 10) /* use isp channel2 */
#define MODULE_FLAG_3D	(1 << 11) /* 3d mode */
#define MODULE_FLAG_HOST1	(1 << 12) /* on host1 */
#define MODULE_FLAG_HOST2	(1 << 13) /* on host2 */
#define MODULE_FLAG_NODVDD	(1 << 14) /* no use dvdd */
#define MODULE_FLAG_AF	(1 << 15) /* AUTO FOCUS */
#define MODULE_FLAG_ALWAYS_POWER	(1 << 16) /* always power on */
#define MODULE_FLAG_NO_AVDD	(1 << 17) /* no need to operate avdd */

//module interface type
#define INTERFACE_PALL "pall"
#define INTERFACE_MIPI "mipi"

//module id
#define OV772X_PLATFORM_ID   0
#define OV2643_PLATFORM_ID   1
#define GC0307_PLATFORM_ID   2
#define GC0309_PLATFORM_ID   3
#define SP0838_PLATFORM_ID   4
#define GC0308_PLATFORM_ID   5
#define GT2005_PLATFORM_ID   6
#define HI253_PLATFORM_ID    7
#define GC2035_PLATFORM_ID   8
#define SP2518_PLATFORM_ID   9
#define OV5640_PLATFORM_ID   10
#define GC0329_PLATFORM_ID   11
#define OV5647_PLATFORM_ID   12
#define GC2235_PLATFORM_ID   13
#define GC0328_PLATFORM_ID   14
#define HM2057_PLATFORM_ID   15

struct v4l2_ctl_cmd_info {
unsigned int  id; 
int min; 
int max; 
unsigned int step; 
int def;
};

struct v4l2_ctl_cmd_info_menu {
unsigned int  id; 
int max; 
int mask; 
int def;
};

static unsigned int *debug_enable = NULL;
#define debug_print(fmt, arg...) do { if ((debug_enable) && (*debug_enable)) printk(KERN_ERR fmt, ##arg); } while (0)

#endif  //__MODULE_COMM_H__
