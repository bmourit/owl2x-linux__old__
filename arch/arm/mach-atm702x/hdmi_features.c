#include "asoc_hdmi.h"

static const struct asoc_hdmi_ip_ops asoc_hdmi_functions = {
	.is_hdmi_enabled	=	asoc_520x_is_hdmi_enabled,
	.is_hpd_enabled		=	asoc_520x_is_hpd_enabled,
	.is_hpdint_enabled	=	asoc_520x_is_hpdint_enabled,
	.init_configure		=	asoc_520x_hdmi_init_configure,
	.get_configure		= 	asoc_520x_hdmi_get_config,
	.set_mode			= 	asoc_520x_hdmi_set_mode,
	.gcp_configure		=	asoc_520x_hdmi_gcp_configure,
	.timing_configure	=	asoc_520x_hdmi_timing_configure,
	.pixel_coding		= 	asoc_520x_hdmi_pixel_coding,
	.video_configure	=	asoc_520x_hdmi_video_configure,
	.gen_infoframe		=	asoc_520x_hdmi_gen_infoframe,
	.phy_enable			=	asoc_520x_hdmi_phy_enable,
	.phy_disable		=	asoc_520x_hdmi_phy_disable,
	.tvoutpll_disable	= 	asoc_tvoutpll_disable,
	.set_src			=	asoc_520x_hdmi_src_set,
	.read_edid			=	asoc_520x_hdmi_read_edid,
	.detect_plug		=	asoc_520x_hdmi_detect_plug,
	.detect_pending		=	asoc_520x_hdmi_detect_pending,
	.hpd_enable			=	asoc_520x_hdmi_hpd_enable,
	.dump_hdmi			=	asoc_520x_hdmi_dump_hdmi,
	.dump_de			=	asoc_520x_hdmi_dump_de,
};

void asoc_init_hdmi_ip_ops(struct hdmi_ip_data *ip_data) {
	HDMI_DEBUG("[ %s start]\n", __func__);
	ip_data->ops = &asoc_hdmi_functions;
}
