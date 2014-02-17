/*
 *
 * Copyright (C) 2012 Actions Corporation
 * Author: chenbo  <chenbo@semi-actions.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/stat.h>
#include <mach/hardware.h>
#include <linux/clk.h>
#include <mach/clock.h>
#include <mach/gl5202_cmu.h>
#include "../display/act_de/display.h"
#include "asoc_hdmi.h"

static struct {
	int addr;
	int val;
	int scale_flag;
} hdmi_attr;

static ssize_t show_hdmi_state(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	return snprintf(buf, PAGE_SIZE, "%x\n", hdmi->hdmi_state);
	
}

static ssize_t store_hdmi_clk_enable(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int state;
	int r;
	struct clk *hdmi_clk;

	r = kstrtoint(buf, 0, &state);
	if (r)
		return r;
	
	hdmi_clk = clk_get_sys(CLK_NAME_HDMI24M_CLK, NULL);
              	
	if (!state) {
	    clk_disable(hdmi_clk);
	} else {
	    clk_enable(hdmi_clk);
	}
	
	return r ? r : count;
}

static ssize_t store_hdmi_enable(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	int state;
	int r;

	r = kstrtoint(buf, 0, &state);
	if (r)
		return r;
		
	if (!state) {
	    hdmi_disable_output(disp_dev);
	} else {
	    hdmi_enable_output(disp_dev);
	}
	
	return r ? r : count;
}

static ssize_t store_hdmi_switch(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	int state;
	int r;

	r = kstrtoint(buf, 0, &state);
	if (r)
		return r;

	switch_set_state(&hdmi->switch_data->sdev, state);
	
	return r ? r : count;
}

static ssize_t show_hdmi_plug(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	return hdmi->ip_data.ops->detect_plug(&hdmi->ip_data);
	
}


static ssize_t show_hdmi_edid(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	hdmi_print_edid(hdmi->edid_data);
	return 0;
	
}

static ssize_t show_hdmi_vid_table(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	printk("%s\n", hdmi->vid_table);
	return 0;
	
}

static ssize_t show_hdmi_encoding(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	return snprintf(buf, PAGE_SIZE, "%d\n", hdmi->ip_data.v_cfg.settings->pixel_encoding);
}

static ssize_t store_hdmi_encoding(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;
	int pixel_encoding;
	int r;

	r = kstrtoint(buf, 0, &pixel_encoding);
	if (r)
		return r;

	hdmi->ip_data.v_cfg.settings->pixel_encoding = pixel_encoding;
	
	return r ? r : count;
}

static ssize_t show_hdmi_scale(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	return snprintf(buf, PAGE_SIZE, "%d\n", hdmi->ip_data.v_cfg.settings->video2to1scaler);
}

static ssize_t store_hdmi_scale(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;
	int video2to1scaler;
	int r;

	r = kstrtoint(buf, 0, &video2to1scaler);
	if (r)
		return r;

	hdmi->ip_data.v_cfg.settings->video2to1scaler= video2to1scaler;

	hdmi_get_id(VIDEO_DE);
	if (video2to1scaler) {
		if (!hdmi_attr.scale_flag ) {
			hdmi->ip_data.v_cfg.mode->xres = hdmi->ip_data.v_cfg.mode->xres/2;
			hdmi_attr.scale_flag = 1;
		}
	} else {

		if (hdmi_attr.scale_flag ) {
			hdmi->ip_data.v_cfg.mode->xres = hdmi->ip_data.v_cfg.mode->xres * 2;
			hdmi_attr.scale_flag = 0;
		}
	}
	
	printk("hdmi->ip_data.v_cfg.mode->xres:%d\n", hdmi->ip_data.v_cfg.mode->xres);
	
	return r ? r : count;
}

static ssize_t show_hdmi_mode(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	return snprintf(buf, PAGE_SIZE, "%d\n", hdmi->ip_data.v_cfg.settings->hdmi_mode);
}

static ssize_t store_hdmi_mode(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;
	int mode;
	int r;

	r = kstrtoint(buf, 0, &mode);
	if (r)
		return r;

	hdmi->ip_data.v_cfg.settings->hdmi_mode = mode;
	
	return r ? r : count;
}

static ssize_t show_hdmi_deepcolor(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	return snprintf(buf, PAGE_SIZE, "%d\n", hdmi->ip_data.v_cfg.settings->deep_color);
}

static ssize_t store_hdmi_deepcolor(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;
	int deep_color;
	int r;

	r = kstrtoint(buf, 0, &deep_color);
	if (r)
		return r;

	hdmi->ip_data.v_cfg.settings->deep_color = deep_color;
	
	return r ? r : count;
}

static ssize_t show_hdmi_3d(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	return snprintf(buf, PAGE_SIZE, "%d\n", hdmi->ip_data.v_cfg.settings->_3d);
}

static ssize_t store_hdmi_3d(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;
	int _3d;
	int r;

	r = kstrtoint(buf, 0, &_3d);
	if (r)
		return r;

	hdmi->ip_data.v_cfg.settings->_3d = _3d;
	
	return r ? r : count;
}

static ssize_t show_hdmi_vid(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	return snprintf(buf, PAGE_SIZE, "%d\n", hdmi->ip_data.v_cfg.settings->vid);
}

static ssize_t store_hdmi_vid(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;
	struct de_video_mode *mode;
	int vid;
	int r;

	r = kstrtoint(buf, 0, &vid);
	if (r)
		return r;

	hdmi->ip_data.v_cfg.settings->vid = vid;
	mode = (struct de_video_mode *)hdmi_get_id(VIDEO_DE);

	if (mode) {
		disp_dev->disp_cur_mode = mode;           
		disp_dev->fb_blank = FB_BLANK_UNBLANK;
		
	} else {
		HDMI_DEBUG("[%s]don't support this hdmi_display_mode!\n", __func__);
	}
	
	
	return r ? r : count;
}


static ssize_t show_hdmi_dump(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	hdmi->ip_data.ops->dump_hdmi(&hdmi->ip_data);

	return 0;
}

static ssize_t show_hdmi_reg(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int val;
	if (hdmi_attr.addr) {
	    val = act_readl(hdmi_attr.addr);
	    return snprintf(buf, PAGE_SIZE, "0x%d\n", val);
	} else {
	    printk("hdmi_attr.addr is null \n");
	    return -1;
	}
	
}


static ssize_t store_hdmi_reg(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t count)
{
	int r = 0;
	int flag = 0;

	char * result = NULL;
	
	while ((result = strsep((char **)&buf,">")) != NULL) {
		if (flag == 0) {
    			r = kstrtoint(result, 0, &hdmi_attr.addr);
			flag++;
		} else { 
			r = kstrtoint(result, 0, &hdmi_attr.val);
		}
  	}

	act_writel(hdmi_attr.val, hdmi_attr.addr);
	return r;	
	
}

static ssize_t show_hdmi_src(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;

	return snprintf(buf, PAGE_SIZE, "%d\n", hdmi->ip_data.v_cfg.settings->hdmi_src);
}

static ssize_t store_hdmi_src(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct asoc_display_device *disp_dev = dev_get_drvdata(dev);
	struct asoc_hdmi *hdmi = (struct asoc_hdmi *)disp_dev->disp_priv_info;
	int hdmi_src;
	int r;
	struct video_parameters_t *para;
	
	r = kstrtoint(buf, 0, &hdmi_src);
	if (r)
		return r;
	
	hdmi->ip_data.v_cfg.settings->hdmi_src = hdmi_src;

	hdmi->ip_data.ops->phy_disable(&hdmi->ip_data);
	
	/*timing configure*/
	hdmi->ip_data.ops->timing_configure(&hdmi->ip_data);

	/*video config*/
	para = (struct video_parameters_t *)hdmi_get_id(VIDEO_HDMI);
	if (para)  {
		hdmi->ip_data.ops->video_configure(&hdmi->ip_data);
	} else {
		HDMI_DEBUG("[%s]don't support this format!\n", __func__);	
		return -EINVAL;
	}

	/*gcp config*/
	 if (hdmi->ip_data.ops->gcp_configure(&hdmi->ip_data))
	 	return -EINVAL;
	 if (hdmi->ip_data.ops->gen_infoframe(&hdmi->ip_data))
	 	return -EINVAL;

	 if (hdmi->ip_data.ops->set_src(&hdmi->ip_data))
	 	return -EINVAL;

	 if (hdmi->ip_data.ops->set_mode(&hdmi->ip_data))
	 	return -EINVAL;

	 hdmi->ip_data.ops->phy_enable(&hdmi->ip_data);
	
	return count;
}

static ssize_t store_hdcp_switch(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int on;
	int r;

	r = kstrtoint(buf, 0, &on);
	if (r)
		return r;
              	
	hdcp_authentication_switch(on);
	
	return r ? r : count;
}

static ssize_t store_read_hdcp_key(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int on;
	int r;

	r = kstrtoint(buf, 0, &on);
	if (r)
		return r;
              	
	hdcp_read_key();
	
	return r ? r : count;
}


struct device_attribute hdmi_vid_table_attr = __ATTR(hdmi_vid_table, S_IRUGO | S_IWUSR, show_hdmi_vid_table, NULL);

static struct device_attribute asoc_hdmi_attrs[] = {
	__ATTR(hdmi_state, S_IRUGO | S_IWUSR, show_hdmi_state, NULL),
	__ATTR(hdmi_clk_enable, S_IRUGO | S_IWUSR, NULL, store_hdmi_clk_enable),
	__ATTR(hdmi_enable, S_IRUGO | S_IWUSR, NULL, store_hdmi_enable),
	__ATTR(hdmi_switch, S_IRUGO | S_IWUSR, NULL, store_hdmi_switch),
	__ATTR(hdmi_plug, S_IRUGO | S_IWUSR, show_hdmi_plug, NULL),
	__ATTR(hdmi_edid, S_IRUGO | S_IWUSR, show_hdmi_edid, store_hdmi_encoding),
	//__ATTR(hdmi_vid_table, S_IRUGO | S_IWUSR, show_hdmi_vid_table, NULL),
	__ATTR(hdmi_encoding, S_IRUGO | S_IWUSR, show_hdmi_encoding, store_hdmi_encoding),
	__ATTR(hdmi_scale, S_IRUGO | S_IWUSR, show_hdmi_scale, store_hdmi_scale),
	__ATTR(hdmi_mode, S_IRUGO | S_IWUSR, show_hdmi_mode, store_hdmi_mode),
	__ATTR(hdmi_vid, S_IRUGO | S_IWUSR, show_hdmi_vid, store_hdmi_vid),
	__ATTR(hdmi_dump, S_IRUGO | S_IWUSR, show_hdmi_dump, NULL),
	__ATTR(hdmi_reg, S_IRUGO | S_IWUSR, show_hdmi_reg, store_hdmi_reg),
	__ATTR(hdmi_src, S_IRUGO | S_IWUSR, show_hdmi_src, store_hdmi_src),
	__ATTR(hdmi_deepcolor, S_IRUGO | S_IWUSR, show_hdmi_deepcolor, store_hdmi_deepcolor),
	__ATTR(hdmi_3d, S_IRUGO | S_IWUSR, show_hdmi_3d, store_hdmi_3d),
	__ATTR(hdcp, S_IRUGO | S_IWUSR, NULL, store_hdcp_switch),
	__ATTR(hdcp_read_key, S_IRUGO | S_IWUSR, NULL, store_read_hdcp_key),
};

int asoc_hdmi_create_sysfs(struct miscdevice *device)
{
	int r, t;

	HDMI_DEBUG("create sysfs for asoc hdmi\n");

	for (t = 0; t < ARRAY_SIZE(asoc_hdmi_attrs); t++) {
		r = device_create_file(device->this_device,
				&asoc_hdmi_attrs[t]);

		if (r) {
			dev_err(device->this_device, "failed to create sysfs "
						"file\n");
			return r;
		}
	}

	return 0;
}

void asoc_hdmi_remove_sysfs(struct miscdevice *device)
{
	int  t;

	HDMI_DEBUG("[%s]remove sysfs for asoc hdmi\n", __func__);
	for (t = 0; t < ARRAY_SIZE(asoc_hdmi_attrs); t++) {
		device_remove_file(device->this_device,
				&asoc_hdmi_attrs[t]);
	}
}


