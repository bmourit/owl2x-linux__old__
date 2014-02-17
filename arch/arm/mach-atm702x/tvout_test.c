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
#include "asoc_ioctl.h"

struct tv_settings {
	unsigned int tv_mode;		/* enum TV_MODE_TYPE */
	unsigned int encoder_type;	/* enum TV_ENCODER_TYPE */
};

enum TV_MODE_TYPE {
	/* following three modes are used by CVBS OUTPUT*/
	TV_MODE_PAL = 0,
	TV_MODE_NTSC = 1,
	TV_MODE_MAX = 2
};

struct tv_settings settings = {
	.tv_mode = TV_MODE_PAL,	
}; 
/*fd --> file handle of de */
static int fd = 0;

/*open cvbs device,device init*/
int cvbs_init() {
	
	fd = open("/dev/cvbs", O_RDWR);
	
	if (fd < 0) {
		printf("Error : Can not open cvbs device fd = %d\n",fd);
		return 1;
	}	
	printf("cvbs_init end of ok \n");
	return 0;
}
/*close de device */
void cvbs_close() {
	close(fd); 
	printf("closed cvbs device\n");
}

/*²âÊÔcvbsµÄ control*/
int main()
{
	struct tv_settings setting = {
		.tv_mode = TV_MODE_PAL,
	};
	
	cvbs_init();
	
	if (ioctl(fd, CVBS_SET_CONFIGURE, &setting)) {
	
		printf("CVBS_CONFIGURE test failed \n");
	} else 
		printf("CVBS_CONFIGURE test success \n");
	
	
	cvbs_close();
	return 0;
}
