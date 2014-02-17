/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2009 Actions Semi Inc.
 */

#ifndef __I2C_HDMI_EDID_H__
#define __I2C_HDMI_EDID_H__
#define HDMI_EDID_BLOCK_SIZE 128
s32 read_hdmi_edid_block(u8 *pBuf, u8 segment_index,u8 segment_offset);

#endif /* __I2C_HDMI_EDID_H__ */
