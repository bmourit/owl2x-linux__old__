
#ifndef __AOTG_PLAT_DATA_H__
#define __AOTG_PLAT_DATA_H__

struct aotg_plat_data {
	void __iomem *usbecs;
	void __iomem *usbpll;
	u32 usbpll_bits;
	void __iomem *devrst;
	u32 devrst_bits;
	void __iomem *devclk;
	u32 devclk_bits;
	int no_hs;

	/* below is revoved. */
	//u32 major, minor;
	//u32 vbus_gpio;
	//u32 vbus_gpio_bits;
	//u32 dma_trigger;
	//u32 mon_flags;
	//unsigned int mod_id_phy;
	//unsigned int mod_id_intf;
	//unsigned int mod_id_pll;
	//unsigned int mod_id_rst_phy;
	//unsigned int mod_id_rst_intf;
};

int aotg0_device_init(int power_only);
void aotg0_device_exit(int power_only);

int aotg1_device_init(int power_only);
void aotg1_device_exit(int power_only);

void aotg0_device_mod_init(void);
void aotg1_device_mod_init(void);

void aotg0_device_mod_exit(void);
void aotg1_device_mod_exit(void);

#endif
