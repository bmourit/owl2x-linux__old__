/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2009 Actions Semi Inc.
*/
/******************************************************************************/

/******************************************************************************/
#ifndef __MBR_INFO_H__
#define __MBR_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MBR_SIZE        1024
#define MAX_PARTITION   20

typedef struct {
	unsigned char flash_ptn;		//flash partition 
	unsigned char partition_num;		//corresponding flash partition number
	unsigned short phy_info;		//reserved: will expand into properties of partition
	unsigned int partition_cap;		//partition size
} __attribute__ ((packed)) partition_info_t;

typedef struct {
	partition_info[MAX_PARTITION];
	unsigned char reserved[0x400 - MAX_PARTITION * sizeof(partition_info_t)];     //mbr_info_t, extention of 1k size
} __attribute__ ((packed)) mbr_info_t;

/********************************************************************
Default Partition Table, arranged in sequence
            flash_ptn       partition_num       partition_cap(in M)
mbrc:           0                   0               block
vmlinux         0                   1               M
rootfs          1                   0               M
configfs        1                   1               M
others          2                   0~n             M

In partition_num, 0xff indicates the last partition
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
