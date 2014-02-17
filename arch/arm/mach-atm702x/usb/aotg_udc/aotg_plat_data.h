
#ifndef __AOTG_PLAT_DATA_H__
#define __AOTG_PLAT_DATA_H__

struct aotg_plat_data {
	u32 major, minor;
	void __iomem *usbecs;
	u32 usbecs_bits;
	//void __iomem *usbpll;
	//u32 usbpll_bits;
	void __iomem *devrst;
	u32 devrst_bits;
	void __iomem *devclk;
	u32 devclk_bits;
	void __iomem *usbpll;
	u32 usbclk_bits;
	u32 vbus_gpio;
	u32 vbus_gpio_bits;
	u32 dma_trigger;
	u32 mon_flags;
	int no_hs;
	unsigned int mod_id_phy;
	unsigned int mod_id_intf;
	unsigned int mod_id_pll;
	unsigned int mod_id_rst_phy;
	unsigned int mod_id_rst_intf;
};

#endif
