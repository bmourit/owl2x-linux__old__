/*
 * camera_atm702x.h - ATM702X camera driver header file
 *
 * Copyright (C) 2008, Guennadi Liakhovetski, DENX Software Engineering, <lg@denx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _ATM702X_CAMERA_H_
#define _ATM702X_CAMERA_H_

#include <linux/device.h>
#include <media/soc_mediabus.h>

#define ATM702X_CAMERA_DATA_HIGH		1
#define ATM702X_CAMERA_PCLK_RISING	2
#define ATM702X_CAMERA_HSYNC_HIGH	4
#define ATM702X_CAMERA_VSYNC_HIGH	8
#define ATM702X_CAMERA_DATAWIDTH_4	0x10
#define ATM702X_CAMERA_DATAWIDTH_8	0x20
#define ATM702X_CAMERA_DATAWIDTH_10	0x40
#define ATM702X_CAMERA_DATAWIDTH_16	0x80

#define GPIO_NAME_SENSOR_FRONT_PWDN "sensorf_pwdn"
#define GPIO_NAME_SENSOR_FRONT_RESET "sensorf_reset"
#define GPIO_NAME_SENSOR_BACK_PWDN  "sensorb_pwdn"
#define GPIO_NAME_SENSOR_BACK_RESET "sensorb_reset"
#define GPIO_NAME_SENSOR_AVDD_CTL "sensor_avdd"

#define ATM702X_CAM_HOST_NAME "camera_host"
#define ATM702X_CAM_HOST2_NAME "camera_host2"

#define ATM702X_CAMERA_DATAWIDTH_MASK (ATM702X_CAMERA_DATAWIDTH_4 | ATM702X_CAMERA_DATAWIDTH_8 | \
				   ATM702X_CAMERA_DATAWIDTH_10 | ATM702X_CAMERA_DATAWIDTH_15)

/**
 * struct atm702x_camera_pdata - ATM702X camera platform data
 * @flags:	ATM702X_CAMERA_* flags
 * @mclk_10khz:	master clock frequency in 10kHz units
 * @dma_dev:	IPU DMA device to match against in channel allocation
 */
struct atm702x_camera_pdata {
	unsigned long flags;
};

enum atm702x_dev_state{
	DEV_STOP = 0,
	DEV_START,
	DEV_SUSPEND,
	DEV_RESUME,
	DEV_OPEN,
	DEV_CLOSE,
	};

struct atm702x_camera_dev {
	/*
	 * ATM702X is only supposed to handle one camera on its Camera Sensor
	 * Interface. If anyone ever builds hardware to enable more than one
	 * camera _simultaneously_, they will have to modify this driver too
	 */
	struct soc_camera_device	*icd;
	void __iomem				*base;
	int							irq;
	struct atm702x_camera_pdata	*pdata;
	unsigned long				platform_flags;
	struct list_head			capture;
	spinlock_t					lock;		/* Protects video buffer lists */
	struct videobuf_buffer		*cur_frm;
	struct videobuf_buffer		*next_frm;
	struct soc_camera_host		soc_host;

	int	started;
	struct v4l2_format f_previous;
	unsigned int isp_channel;
	int drop_frame;
	unsigned int debug_print_switch;
    
#ifdef CONFIG_ENABLE_DRV_HOTPLUG
    struct delayed_work work;
#endif
};

struct atm702x_camera_param {
	/* ISP data offsets within croped by the ATM702X camera output */
	unsigned int				isp_left;
	unsigned int				isp_top;
	/* Client output, as seen by the ATM702X */
	unsigned int				width;
	unsigned int				height;
	/*
	 * User window from S_CROP / G_CROP, produced by client cropping,
	 * ATM702X cropping, mapped back onto the client
	 * input window
	 */
	struct v4l2_rect subrect;
	/* Camera cropping rectangle */
	struct v4l2_rect rect;
	const struct soc_mbus_pixelfmt *extra_fmt;
	enum v4l2_mbus_pixelcode code;
	unsigned int drop_frame_num;
	unsigned int ahead_line_num;
	unsigned long flags;
	int channel_isp;//ISP1 or ISP2
	int module_type;//0 for yuv sensor,1 for raw-bayer sensor
	int (*ext_cmd)(struct v4l2_subdev *sd,int cmd,void *args);
};


#endif
