/*
 * C Application Example for ARM Linux
 * 
 * Copyright (C) actions Limited, 2007-2010. All rights reserved.
 */

/*
 * hello.c: A simple "de test" C program
 */
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/types.h>
#include <sys/mman.h>
#include "../asoc_ioctl.h"
#include "hdmi_test.h"

typedef unsigned int		u32;
/*fd --> file handle of de */
static int fd = 0;

/*open cvbs device,device init*/
int hdmi_init() {
	
	fd = open("/dev/hdmi", O_RDWR);
	
	if (fd < 0) {
		printf("Error : Can not open hdmi device fd = %d\n",fd);
		return 1;
	}	
	printf("hdmi_init end of ok \n");
	return 0;
}
/*close de device */
void hdmi_close() {
	close(fd); 
	printf("closed hdmi device\n");
}

/*²âÊÔcvbsµÄ control*/
int main()
{
	struct hdmi_settings default_settings = {
	
	.vid = VID1280x720P_50_16VS9,
	.hdmi_mode = HDMI_MODE_HDMI,
	.video2to1scaler = 0,
	.hdmi_src = DE,
	.pixel_encoding = VIDEO_PEXEL_ENCODING_YCbCr444,
	.color_xvycc = YCC601_YCC709,
	.deep_color = DEEP_COLOR_24_BIT,
	._3d = _3D_NOT,
	
	};
	
	hdmi_init();
	
	if (ioctl(fd, HDMI_SET_VIDEO_CONFIG, &default_settings)) {
	
		printf("HDMI_SET_VIDEO_CONFIG test failed \n");
	} else 
		printf("HDMI_SET_VIDEO_CONFIG test success \n");
	
	
	hdmi_close();
	return 0;
}
