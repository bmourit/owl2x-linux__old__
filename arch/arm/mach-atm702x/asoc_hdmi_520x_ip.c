#include <linux/delay.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <asm-generic/uaccess.h>
#include <linux/regulator/consumer.h>
#include <mach/hardware.h>
#include <mach/gl5202_cmu.h>
#include <mach/clock.h>
#include <mach/actions_reg_gl5202.h>
#include "asoc_hdmi.h"
#include "asoc_hdmi_5202_reg.h"
#include "asoc_hdmi_520x_packet.h"

#define IC_DEBUG
#define M0_LENGTH       8
#define BSTATUS_LENGTH  2
#define KSV_LENGTH      5
#define KSV_RESERVED  3
#define KEY_COL_LENGTH 7
#define KEY_ARRAY_LENGTH 40
#define KEY_LENGTH      280
#define VERIFY_LENGTH 20
#define MAX_SHA_1_INPUT_LENGTH  704
#define swapl(x)	((((x)&0x000000ff)<<24)+(((x)&0x0000ff00)<<8)+(((x)&0x00ff0000)>>8)+(((x)&0xff000000)>>24))
#define SXOLN(n,x)	((x<<n)|(x>>(32-n)))    //x leftrotate n
#define SXORN(n,x)	((x>>n)|(x<<(32-n)))    //x rightrotate n
#define hLen    20   //sha_1 160bit

//flash info type
#define MISC_INFO_TYPE_SN               0
#define MISC_INFO_TYPE_DRM              1
#define MISC_INFO_TYPE_HDCP             2
#define MISC_INFO_READ                  0
#define MISC_INFO_WRITE                 1

atomic_t deepcolor_enabled = ATOMIC_INIT(0);
atomic_t tvoutpll0_enabled = ATOMIC_INIT(0);
atomic_t tvoutpll1_enabled = ATOMIC_INIT(0);

const char *hdmi_set_which_pll = CLK_NAME_TVOUT1PLL;
extern int get_config(const char *key, char *buff, int len);
extern int hibernate_reg_setmap(unsigned int reg, unsigned int bitmap);
//extern int NAND_GetHDCPKey(char * buf);
extern int NAND_GetMiscInfo(int type, char * buf, int size);

/*hdcp*/
int c2ln14(unsigned char *num, char *a);
int hdcp_GenKm(struct hdmi_ip_data *ip_data);
int hdcp_SetKm(unsigned char *key, int pnt);

#ifdef READ_HDCP_KEY_FROM_NATIVE
char test_keyr0[40][32] ={
    "aa6197eb701e4e",
    "780717d2d425d3",
    "7c4f7efe39f44d",
    "1e05f28f0253bd",
    "d614c6ccb090ee",
    "82f7f2803ffefc",
    "4ae2ebe12741d7",
    "e203695a82311c",
    "4537b3e7f9f557",
    "b02d715e2961ed",
    "cd6e0ab9834016",
    "d64fd9edfdd35b",
    "3a13f170726840",
    "4f52fb759d6f92",
    "2afc1d08b4221f",
    "b99db2a29aea40",
    "2020da8eb3282a",
    "0b4c6aee4aa771",
    "edeef811ec6ac0",
    "8f451ad92112f3",
    "705e2fddc47e6e",
    "6efc72aaa3348f",
    "7debb76130fb0e",
    "5fd1a853c40f52",
    "70168fbeaeba3c",
    "a0cb8f3162d198",
    "5ed0a9eef0c1ed",
    "fa18541a02d283",
    "0661135d19feec",
    "da6b02ca363cf3",
    "dd7142f81e832f",
    "cd3490a48be99a",
    "0e08bceaeec81a",
    "9b0d183dbbbbf3",
    "7788f601c07e57",
    "4e7271a716e4aa",
    "f510b293bec62a",
    "16158c6a1f3c84",
    "33656236296f06",
    "a5438e67d72d80",
};
#else
unsigned char test_keyr0[40][15];
#endif

#ifdef I2C_RW_DEBUG
extern unsigned char aksv[5];
#else
extern unsigned char aksv[6];
#endif


char *key[] = { 
    "hdmi.package", 
    "boot_disp_cfg.hdmi_cfg",
    "boot_disp_cfg.disp_cfg", 
    "hdmi.vids",
    "hdmi.hdcp_switch"
};

void set_quickboot_not_control_reg(void) {
    hibernate_reg_setmap(CMU_TVOUTPLL, 0xffffffff);
}

int asoc_520x_hdmi_detect_plug(struct hdmi_ip_data *ip_data) {
    u32 status_val;
    status_val = act_readl(HDMI_CR) & HDMI_CR_HPLGSTATUS;
    msleep(2);
    status_val &= act_readl(HDMI_CR) & HDMI_CR_HPLGSTATUS;
    if (status_val)
        return HDMI_PLUGIN;
    else
        return HDMI_PLUGOUT;
}

int asoc_520x_hdmi_detect_pending(struct hdmi_ip_data *ip_data) {
    int reg_val;
    reg_val = act_readl(HDMI_CR);
    if ((reg_val & HDMI_CR_ENHPINT) && (reg_val & HDMI_CR_PHPLG)) {
        act_setl(HDMI_CR_PHPLG, HDMI_CR);
        return 1;
    }

    return 0;
}

void asoc_520x_hdmi_hpd_enable(bool flag) {
    if (flag) {
        /* set debounce */
        act_writel((act_readl(HDMI_CR) | HDMI_CR_HPDEBOUNCE(0xF))
                & (~HDMI_CR_PHPLG), HDMI_CR);
        /* enable hpd */
        act_writel((act_readl(HDMI_CR) | HDMI_CR_ENHPINT | HDMI_CR_HPDENABLE)
                & (~HDMI_CR_PHPLG), HDMI_CR);
    } else {
        /* disable hpd */
        act_writel((act_readl(HDMI_CR) & ~(HDMI_CR_ENHPINT | HDMI_CR_HPDENABLE))
                & (~HDMI_CR_PHPLG), HDMI_CR);
    }
}

int asoc_520x_is_hdmi_enabled(struct hdmi_ip_data *ip_data) {
    if (act_readl(HDMI_CR) & HDMI_CR_ENABLEHDMI)
        return HDMI_ENABLE;
    else
        return HDMI_DISABLE;
}

int asoc_520x_is_hpd_enabled(struct hdmi_ip_data *ip_data) {
    if (act_readl(HDMI_CR) & HDMI_CR_HPDENABLE)
        return HPD_ENABLE;
    else
        return HPD_DISABLE;
}

int asoc_520x_is_hpdint_enabled(struct hdmi_ip_data *ip_data) {
    if (act_readl(HDMI_CR) & HDMI_CR_ENHPINT)
        return HPDINT_ENABLE;
    else
        return HPDINT_DISABLE;
}

void asoc_520x_hdmi_init_configure(struct hdmi_ip_data *ip_data) {
    struct clk *hdmi_clk;
    hdmi_clk = clk_get_sys(CLK_NAME_HDMI_CLK, NULL);
    hdmi_clk = clk_get_sys(CLK_NAME_HDMI24M_CLK, NULL);
    clk_enable(hdmi_clk);
    /* enable Audio FIFO_FILL  disable wait cycle*/
    act_writel((act_readl(HDMI_CR) | 0x50) & (~HDMI_CR_PHPLG), HDMI_CR);
    /*reset HDCP_CR,HDMI_SCR*/
    act_writel(0, HDMI_SCR);
    act_writel(0, HDCP_CR);
    act_writel(0x2a28b1f9, HDCP_KOWR);
    act_writel(0x20e1fe, HDCP_OWR);
    /* enable hdmi gcp transmission,clear AVMUTE flag*/
    act_writel(HDMI_GCPCR_ENABLEGCP | HDMI_GCPCR_CLEARAVMUTE, HDMI_GCPCR);
    /* disable all RAM packet transmission */
    act_writel(0, HDMI_RPCR);
    asoc_520x_hdmi_hpd_enable(true);
}

int asoc_520x_hdmi_get_config(struct hdmi_ip_data *ip_data) {
    int ret;
    /*get package value*/
    ret = get_config(key[0], ip_data->config.package, sizeof(key[0]));
    if (ret != 0) {
        printk("[%s]get package fail!\n", __func__);
        ret = -EINVAL;
    } else {
        HDMI_DEBUG("[%s] package is %s!\n", __func__, ip_data->config.package);
    }
    /*get hdmi vid*/
    ret = get_config(key[1], (char *) ip_data->config.boot_hdmi_cfg,
            sizeof(ip_data->config.boot_hdmi_cfg));
    if (ret != 0) {
        printk("[%s]get hdmi vid fail!\n", __func__);
        //ip_data->v_cfg.settings->vid = VID1280x720P_50_16VS9;
        ret = -EINVAL;
    } else {
        //ip_data->v_cfg.settings->vid = VID1280x720P_50_16VS9;
        HDMI_DEBUG("[%s] hdmi vid  is %d!\n", __func__, ip_data->config.boot_hdmi_cfg[0]);
    }
    /*get boot config*/
    ret = get_config(key[2], (char *) ip_data->config.boot_disp_cfg,
            sizeof(ip_data->config.boot_disp_cfg[0]));
    if (ret != 0) {
        printk("[%s]get display cfg fail!\n", __func__);
        ret = -EINVAL;
    } else {
        HDMI_DEBUG("[%s] get display cfg  is %d!\n", __func__,
                ip_data->config.boot_disp_cfg[0]);
    }
    /*get hdmi vids that we want support*/
    ret = get_config(key[3], (char *) ip_data->config.hdmi_vids,
            sizeof(ip_data->config.hdmi_vids));
    if (ret != 0) {
        printk("[%s]get hdmi vids fail!\n", __func__);
        ret = -EINVAL;
    } else {
        int i;
        for (i = 0; i < ARRAY_SIZE(ip_data->config.hdmi_vids); i++)
            HDMI_DEBUG("[%s] get hdmi vids  is %d!\n", __func__,
                    ip_data->config.hdmi_vids[i]);
    }
     /*get hdmi hdmi_switch that we want support*/
    ret = get_config(key[4], (char *) &ip_data->config.hdcp_switch,
            sizeof(ip_data->config.hdcp_switch));
    if (ret != 0) {
        printk("[%s]get  hdcp_switch fail!\n", __func__);
        ret = -EINVAL;
    } else {
       HDMI_DEBUG("[%s] get hdcp_switch cfg  is %d!\n", __func__,
                ip_data->config.hdcp_switch);
    }

    return ret;
}

void asoc_520x_hdmi_tx_config(struct hdmi_ip_data *ip_data) {
    unsigned int vid = ip_data->v_cfg.settings->vid;
    unsigned int _3d = ip_data->v_cfg.settings->_3d;
    unsigned int deep_color = ip_data->v_cfg.settings->deep_color;

    if (!strcmp("bga", ip_data->config.package)) {
        switch (vid) {
        case VID640x480P_60_4VS3:
            if (_3d == _3D) {
                act_writel(0x918ae903, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            } else {
                act_writel(0x918ee903, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            }
            break;
        case VID720x576P_50_4VS3:
        case VID720x576P_50_16VS9:
        case VID720x480P_60_4VS3:
        case VID720x480P_60_16VS9:
        case VID1440x480I_60_4VS3://the same with VID720x480I_60_4VS3
        case VID720x480I_60_16VS9:
        case VID1440x576I_50_4VS3://the same with VID720x576I_50_4VS3
        case VID720x576I_50_16VS9:
            if (_3d == _3D) {
                act_writel(0x918ae903, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            } else {
                act_writel(0x918ee903, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            }
            break;
        case VID1440x480P_60_4VS3:
        case VID1440x480P_60_16VS9:
        case VID1440x576P_50_4VS3:
        case VID1440x576P_50_16VS9:
            if (_3d == _3D) {
                act_writel(0xb18ae987, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            } else {
                act_writel(0x918ae903, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            }
            break;
        case VID1280x720P_60_16VS9:
        case VID1280x720P_50_16VS9:
        case VID1280X720P_59P94_16VS9:
        case VID1920x1080I_60_16VS9:
        case VID1920x1080I_50_16VS9:
        case VID1920x1080P_24_16VS9:
        case VID1920x1080P_25_16VS9:
        case VID1920x1080P_30_16VS9:
            if (_3d == _3D) {
                act_writel(0xb182eb9e, HDMI_TX_1);
                act_writel(0x1ffb0fc1, HDMI_TX_2);
            } else {
                if (deep_color == DEEP_COLOR_24_BIT) {
                    act_writel(0x918ae90a, HDMI_TX_1);
                    act_writel(0x1dfa0fc1, HDMI_TX_2);
                }
                if (deep_color == DEEP_COLOR_30_BIT) {
                    act_writel(0x918ae903, HDMI_TX_1);
                    act_writel(0x1df20fc1, HDMI_TX_2);
                }
                if (deep_color == DEEP_COLOR_36_BIT) {
                    act_writel(0xb18ae987, HDMI_TX_1);
                    act_writel(0x1df20fc1, HDMI_TX_2);
                }
            }
            break;
        case VID1920x1080P_60_16VS9:
        case VID1920x1080P_50_16VS9:
        case VID1920x1080P_59P94_16VS9:
            if (_3d == _3D) {
                act_writel(0xb182e987, HDMI_TX_1);
                act_writel(0x1df20fc1, HDMI_TX_2);
            } else {
                if (deep_color == DEEP_COLOR_24_BIT) {
                    act_writel(0xb182eb9e, HDMI_TX_1);
                    act_writel(0x1ffa0fc1, HDMI_TX_2);
                }
                if (deep_color == DEEP_COLOR_30_BIT) {
                    act_writel(0xb182e99a, HDMI_TX_1);
                    act_writel(0x1ffb0fc1, HDMI_TX_2);
                }
                if (deep_color == DEEP_COLOR_36_BIT) {
                    act_writel(0xb182ebaa, HDMI_TX_1);
                    act_writel(0x1ffb0fc1, HDMI_TX_2);
                }
            }
            break;

        default:
            break;
        }
    }

    if (!strcmp("epad", ip_data->config.package)) {
        switch (vid) {
        case VID640x480P_60_4VS3:
            if (_3d == _3D) {
                act_writel(0x918ae903, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            } else {
                act_writel(0x918ee903, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            }
            break;

        case VID720x576P_50_4VS3:
        case VID720x576P_50_16VS9:
        case VID720x480P_60_4VS3:
        case VID720x480P_60_16VS9:
        case VID1440x480I_60_4VS3://the same with VID720x480I_60_4VS3
        case VID720x480I_60_16VS9:
        case VID1440x576I_50_4VS3://the same with VID720x576I_50_4VS3
        case VID720x576I_50_16VS9:
            if (_3d == _3D) {
                act_writel(0x918ae903, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            } else {
                act_writel(0x918ee903, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            }
            break;

        case VID1440x480P_60_4VS3:
        case VID1440x480P_60_16VS9:
        case VID1440x576P_50_4VS3:
        case VID1440x576P_50_16VS9:
            if (_3d == _3D) {
                act_writel(0xb18aeb9e, HDMI_TX_1);
                act_writel(0x1ffa0fc1, HDMI_TX_2);
            } else {
                act_writel(0x918ae903, HDMI_TX_1);
                act_writel(0x1df00fc1, HDMI_TX_2);
            }
            break;

        case VID1280x720P_60_16VS9:
        case VID1280x720P_50_16VS9:
        case VID1280X720P_59P94_16VS9:
        case VID1920x1080I_60_16VS9:
        case VID1920x1080I_50_16VS9:
        case VID1920x1080P_24_16VS9:
        case VID1920x1080P_25_16VS9:
        case VID1920x1080P_30_16VS9:
            if (_3d == _3D) {
                act_writel(0xb182eb9e, HDMI_TX_1);
                act_writel(0x1ffa0fc1, HDMI_TX_2);
            } else {
                if (deep_color == DEEP_COLOR_24_BIT) {
                    act_writel(0xb18ae987, HDMI_TX_1);
                    act_writel(0x1df00fc1, HDMI_TX_2);
                }
                if (deep_color == DEEP_COLOR_30_BIT) {
                    act_writel(0xb18ae987, HDMI_TX_1);
                    act_writel(0x1df00fc1, HDMI_TX_2);
                }
                if (deep_color == DEEP_COLOR_36_BIT) {
                    act_writel(0xb18aeb9e, HDMI_TX_1);
                    act_writel(0x1ffa0fc1, HDMI_TX_2);
                }
            }
            break;

        case VID1920x1080P_60_16VS9:
        case VID1920x1080P_50_16VS9:
        case VID1920x1080P_59P94_16VS9:
            if (_3d == _3D) {
                act_writel(0xb182eb9e, HDMI_TX_1);
                act_writel(0x1ffa0fc1, HDMI_TX_2);
            } else {
                if (deep_color == DEEP_COLOR_24_BIT) {
                    act_writel(0xb182eb9e, HDMI_TX_1);
                    act_writel(0x1ffa0fc1, HDMI_TX_2);
                }
                if (deep_color == DEEP_COLOR_30_BIT) {
                    act_writel(0xb182ebab, HDMI_TX_1);
                    act_writel(0x1ffa0fc1, HDMI_TX_2);
                }
                if (deep_color == DEEP_COLOR_36_BIT) {
                    act_writel(0xb182ebac, HDMI_TX_1);
                    act_writel(0x1ffa0fc1, HDMI_TX_2);
                }
            }
            break;

        default:
            break;
        }
    }
}

int asoc_tvoutpll_enable(const char *pll_name) {
    struct clk *dev_clk;
    dev_clk = clk_get_sys(pll_name, NULL);
    if (IS_ERR(dev_clk)) {
        return PTR_ERR(dev_clk);
    }
    if (!strcmp(pll_name, CLK_NAME_TVOUT0PLL)) {
        HDMI_DEBUG("[ %s]atomic_read(&tvoutpll0_enabled)):%d\n", __func__,
                atomic_read(&tvoutpll0_enabled));
        if (!atomic_read(&tvoutpll0_enabled)) {
            clk_enable(dev_clk);
            atomic_set(&tvoutpll0_enabled, 1);
        }
    } else if (!strcmp(pll_name, CLK_NAME_TVOUT1PLL)) {
        HDMI_DEBUG("[ %s]atomic_read(&tvoutpll1_enabled)):%d\n", __func__,
                atomic_read(&tvoutpll1_enabled));
        if (!atomic_read(&tvoutpll1_enabled)) {
            clk_enable(dev_clk);
            atomic_set(&tvoutpll1_enabled, 1);
        }
    } else if (!strcmp(pll_name, CLK_NAME_DEEPCOLORPLL)) {
        HDMI_DEBUG("[ %s]atomic_read(&deepcolor_enabled)):%d\n", __func__,
                atomic_read(&deepcolor_enabled));
        if (!atomic_read(&deepcolor_enabled)) {
            clk_enable(dev_clk);
            atomic_set(&deepcolor_enabled, 1);
        }
    }
    return 0;

}

int asoc_tvoutpll_disable(const char *pll_name) {
    struct clk *dev_clk;
    dev_clk = clk_get_sys(pll_name, NULL);
    if (IS_ERR(dev_clk)) {
        return PTR_ERR(dev_clk);

    }
    if (!strcmp(pll_name, CLK_NAME_TVOUT0PLL)) {
        HDMI_DEBUG("[ %s]atomic_read(&tvoutpll0_enabled)):%d\n", __func__,
                atomic_read(&tvoutpll0_enabled));
        if (atomic_read(&tvoutpll0_enabled)) {
            clk_disable(dev_clk);
            atomic_set(&tvoutpll0_enabled, 0);
        }
    } else if (!strcmp(pll_name, CLK_NAME_TVOUT1PLL)) {
        HDMI_DEBUG("[ %s]atomic_read(&tvoutpll1_enabled)):%d\n", __func__,
                atomic_read(&tvoutpll1_enabled));
        if (atomic_read(&tvoutpll1_enabled)) {
            clk_disable(dev_clk);
            atomic_set(&tvoutpll1_enabled, 0);
        }
    } else if (!strcmp(pll_name, CLK_NAME_DEEPCOLORPLL)) {
        HDMI_DEBUG("[ %s]atomic_read(&deepcolor_enabled)):%d\n", __func__,
                atomic_read(&deepcolor_enabled));
        if (atomic_read(&deepcolor_enabled)) {
            clk_disable(dev_clk);
            atomic_set(&deepcolor_enabled, 0);
        }
    }

    return 0;
}

void asoc_520x_hdmi_phy_enable(struct hdmi_ip_data *ip_data) {
    //enable tmds output&HDMI output
    act_writel(act_readl(TMDS_EODR0) | 0x80000000, TMDS_EODR0);
    act_setl(HDMI_CR_ENABLEHDMI & ~HDMI_CR_PHPLG, HDMI_CR);
    asoc_520x_hdmi_tx_config(ip_data);
}

void asoc_520x_hdmi_phy_disable(struct hdmi_ip_data *ip_data) {
    //when disable, clear HDMI_TX_2's bit8-11,bit17-19 by sd's suggestion
    act_clearl(HDMI_TX_2_REG_TX_PU(0xf) | HDMI_TX_2_REG_TX_RT_EN
                    | HDMI_TX_2_REG_TX_RT_SEL(0x3), HDMI_TX_2);
    act_clearl(HDMI_CR_ENABLEHDMI | HDMI_CR_PHPLG, HDMI_CR);
    asoc_tvoutpll_disable(CLK_NAME_DEEPCOLORPLL);
    if (!strcmp(hdmi_set_which_pll, CLK_NAME_TVOUT0PLL))
        asoc_tvoutpll_disable(CLK_NAME_TVOUT0PLL);
    else
        asoc_tvoutpll_disable(CLK_NAME_TVOUT1PLL);
    msleep(100);
}

int asoc_520x_hdmi_src_set(struct hdmi_ip_data *ip_data) {
    int hdmi_src = ip_data->v_cfg.settings->hdmi_src;
    if (hdmi_src == VITD) {
        act_setl(HDMI_ICR_VITD(0x809050) | HDMI_ICR_ENVITD, HDMI_ICR);
    } else if (hdmi_src == DE) {
        act_clearl(HDMI_ICR_ENVITD, HDMI_ICR);
    } else {
        printk("don't support this display src config\n");
        return -EINVAL;
    }

    return 0;
}

int asoc_520x_hdmi_set_mode(struct hdmi_ip_data *ip_data) {
    unsigned int hdmi_mode = ip_data->v_cfg.settings->hdmi_mode;
    if (hdmi_mode == HDMI_MODE_HDMI) {
        act_setl(HDMI_SCHCR_HDMI_MODESEL, HDMI_SCHCR);
    } else if (hdmi_mode == HDMI_MODE_DVI) {
        act_clearl(HDMI_SCHCR_HDMI_MODESEL, HDMI_SCHCR);
    } else {
        printk("[%s]don'n support this mode!\n", __func__);
        return -EINVAL;
    }

    return 0;
}

unsigned long asoc_tvoutpll_or_clk_set_rate(const char *name, unsigned long new_rate) {
    struct clk *dev_clk;
    unsigned long rate;
    int ret;

    dev_clk = clk_get_sys(name, NULL);
    if (IS_ERR(dev_clk)) {
        return PTR_ERR(dev_clk);

    }

    if (!strcmp(name, CLK_NAME_TVOUT0PLL)) {
        hdmi_set_which_pll = name;
        asoc_tvoutpll_enable(name);

    } else if (!strcmp(name, CLK_NAME_TVOUT1PLL)) {
        hdmi_set_which_pll = name;
        asoc_tvoutpll_enable(name);
    } else if (!strcmp(name, CLK_NAME_DEEPCOLORPLL)) {
        asoc_tvoutpll_enable(name);

    }

    rate = clk_get_rate(dev_clk) / 1000;
    printk("[%s] old_rate:%ld\n", __func__, rate);
    if (new_rate != rate) {
        printk("[%s] new_rate:%ld\n", __func__, new_rate);
        ret = clk_set_rate(dev_clk, new_rate * 1000);
        if (ret) {
            printk("[%s](%d):clk_set_rate  failed1\n", __func__, __LINE__);
            return -EINVAL;

        }

        rate = clk_get_rate(dev_clk) / 1000;
        if (new_rate != rate) {
            printk("[%s](%d):clk_set_rate  failed2\n", __func__, __LINE__);
            asoc_tvoutpll_disable(name);

            return -EINVAL;

        }
    }

    return rate;
}

unsigned int asoc_get_clk_div(unsigned int vid, unsigned int _3d) {
    unsigned int clk_div;
    switch (vid) {
    case VID640x480P_60_4VS3:
    case VID1440x480P_60_4VS3:
    case VID1440x480P_60_16VS9:
    case VID1440x576P_50_4VS3:
    case VID1440x576P_50_16VS9:
    case VID1280x720P_60_16VS9:
    case VID1920x1080I_60_16VS9:
    case VID1920x1080I_50_16VS9:
    case VID1920x1080P_24_16VS9:
    case VID1920x1080P_25_16VS9:
    case VID1920x1080P_30_16VS9:
    case VID1280x720P_50_16VS9:
    case VID1280X720P_59P94_16VS9:
        if (_3d == _3D)
            clk_div = 1;
        else
            clk_div = 2;
        break;

    case VID720x576P_50_4VS3:
    case VID720x576P_50_16VS9:
    case VID720x480P_60_4VS3:
    case VID720x480P_60_16VS9:
    case VID1440x480I_60_4VS3://the same with VID720x480I_60_4VS3
    case VID720x480I_60_16VS9:
    case VID1440x576I_50_4VS3://the same with VID720x576I_50_4VS3
    case VID720x576I_50_16VS9:
        if (_3d == _3D)
            clk_div = 2;
        else
            clk_div = 4;
        break;

    case VID1920x1080P_60_16VS9:
    case VID1920x1080P_50_16VS9:
    case VID1920x1080P_59P94_16VS9:
        if (_3d == _3D)
            clk_div = 1;
        else
            clk_div = 1;
        break;

    default:
        clk_div = 2;
    }

    printk("[%s]  clk_div:%d\n", __func__, clk_div);

    return clk_div;
}

unsigned long asoc_deepcolorpll_set_rate(unsigned int _3d,
        unsigned int deep_color) {

    struct clk *dev_clk;
    struct clk *parent_clk;
    unsigned long rate;

    dev_clk = clk_get_sys(CLK_NAME_DEEPCOLORPLL, NULL);
    if (IS_ERR(dev_clk)) {
        return PTR_ERR(dev_clk);

    }

    if (_3d) {
        asoc_tvoutpll_enable(CLK_NAME_DEEPCOLORPLL);
        return 0;

    }

    parent_clk = clk_get_parent(dev_clk);
    printk("[%s] parent_clk->rate:%ld\n", __func__, parent_clk->rate);

    if (deep_color == DEEP_COLOR_24_BIT) {
        rate = asoc_tvoutpll_or_clk_set_rate(CLK_NAME_DEEPCOLORPLL,
                parent_clk->rate / 1000 * 1);

    } else if (deep_color == DEEP_COLOR_30_BIT) {
        rate = asoc_tvoutpll_or_clk_set_rate(CLK_NAME_DEEPCOLORPLL,
                parent_clk->rate / 1000 * 125 / 100);

    } else if (deep_color == DEEP_COLOR_36_BIT) {
        rate = asoc_tvoutpll_or_clk_set_rate(CLK_NAME_DEEPCOLORPLL,
                parent_clk->rate / 1000 * 150 / 100);

    }

    rate = clk_get_rate(dev_clk) / 1000;

    return rate;
}

unsigned long asoc_tvoutpll_mux_set_parent(const char *parent) {
    struct clk *dev_clk;
    struct clk *parent_clk;

    dev_clk = clk_get_sys(CLK_NAME_TVOUTMUX0_CLK, NULL);
    if (IS_ERR(dev_clk)) {
        return PTR_ERR(dev_clk);

    }

    parent_clk = clk_get_sys(parent, NULL);
    if (IS_ERR(parent_clk)) {
        return PTR_ERR(parent_clk);

    }

    clk_set_parent(dev_clk, parent_clk);

    return dev_clk->rate / 1000;

}

void asoc_520x_hdmi_tvoutpll_cfg(const char *pll_name, unsigned long pll_rate,
        unsigned int deep_color, unsigned int vid, unsigned int _3d) {
    unsigned long rate;
    unsigned int clk_div;

    //enable tvoutpllx and set tvoutpllx's clk
    asoc_tvoutpll_or_clk_set_rate(pll_name, pll_rate);
    //set hdmi pixel's parent to be pll_name
    rate = asoc_tvoutpll_mux_set_parent(pll_name);
    printk("[%s] rate:%ld\n", __func__, rate);
    //set deepcolor rate
    asoc_deepcolorpll_set_rate(_3d, deep_color);
    clk_div = asoc_get_clk_div(vid, _3d);
    //set tvoutpll0 rate, because only tvoutpll0 need choose mux, tvoutpll1 not need, so set tvoutpll0's rate only.
    asoc_tvoutpll_or_clk_set_rate(CLK_NAME_TVOUT0_CLK, rate / clk_div);

}

void asoc_520x_hdmi_timing_configure(struct hdmi_ip_data *ip_data) {
    unsigned int vid = ip_data->v_cfg.settings->vid;
    unsigned int _3d = ip_data->v_cfg.settings->_3d;
    unsigned int deep_color = ip_data->v_cfg.settings->deep_color;

#ifdef IC_DEBUG
    act_writel(act_readl(CMU_TVOUTPLL1DEBUG) & ~(1 << 23), CMU_TVOUTPLL1DEBUG);
#endif
    act_writel(0x827, CMU_TVOUTPLL0DEBUG);
    act_writel(0xF827, CMU_TVOUTPLL1DEBUG);
    act_writel(0xC027, CMU_DCPPLLDEBUG);

   // HDMI_DEBUG("[%s start]\n", __func__);

    switch (vid) {
    case VID640x480P_60_4VS3:
        /*if (_3d == _3D) { 
         //pll0:100.8m; mux:/1; deep mux:1; pixel:50.4m
         act_writel(0x00080008, CMU_TVOUTPLL); 

         } else {
         
         if (deep_color == DEEP_COLOR_24_BIT) {
         //pll0:108.8m; mux:/2; deep mux:1; pixel:25.2m
         act_writel(0x00080018, CMU_TVOUTPLL);  
         }else if (deep_color == DEEP_COLOR_30_BIT) {
         //pll0:108.8m; mux:/2; deep mux:1.25; pixel:25.2m
         act_writel(0x00090018, CMU_TVOUTPLL); 
         }else if (deep_color == DEEP_COLOR_36_BIT) {//36bit normal 2D
         //pll0:108.8m; mux:/2; deep mux:1.5; pixel:25.2m
         act_writel(0x000a0018, CMU_TVOUTPLL);
         }
         }*/
        asoc_520x_hdmi_tvoutpll_cfg(CLK_NAME_TVOUT0PLL, 100800, deep_color, vid,
                _3d);
        break;

    case VID720x576P_50_4VS3:
    case VID720x576P_50_16VS9:
    case VID720x480P_60_4VS3:
    case VID720x480P_60_16VS9:
    case VID1440x480I_60_4VS3://the same with VID720x480I_60_4VS3
    case VID720x480I_60_16VS9:
    case VID1440x576I_50_4VS3://the same with VID720x576I_50_4VS3
    case VID720x576I_50_16VS9:
        /*if (_3d == _3D) { 
         //pll1:216m; mux:/2; deep mux:1; pixel:54m
         act_writel(0x00080c50, CMU_TVOUTPLL); 
         } else {
         
         if (deep_color == DEEP_COLOR_24_BIT) {
         //pll1:216m; mux:/4; deep mux:1; pixel:27m
         act_writel(0x00080c60, CMU_TVOUTPLL);  
         }else if (deep_color == DEEP_COLOR_30_BIT) {
         //pll1:216m; mux:/4; deep mux:1.25; pixel:27m
         act_writel(0x00090c60, CMU_TVOUTPLL); 
         }else if (deep_color == DEEP_COLOR_36_BIT) {
         //pll1:216m; mux:/4; deep mux:1.5; pixel:27m
         act_writel(0x000a0c60, CMU_TVOUTPLL);  
         }
         }*/
        asoc_520x_hdmi_tvoutpll_cfg(CLK_NAME_TVOUT1PLL, 216000, deep_color, vid,
                _3d);
        break;

    case VID1440x480P_60_4VS3:
    case VID1440x480P_60_16VS9:
    case VID1440x576P_50_4VS3:
    case VID1440x576P_50_16VS9:
        /*if (_3d == _3D) { 
         //pll1:216m; mux:/1; deep mux:1; pixel:108m
         act_writel(0x00080c40, CMU_TVOUTPLL); 
         } else {
         if (deep_color == DEEP_COLOR_24_BIT) {
         //pll1:216m; mux:/2; deep mux:1; pixel:54m
         act_writel(0x00080c50, CMU_TVOUTPLL);  
         }else if (deep_color == DEEP_COLOR_30_BIT) {
         //pll1:216m; mux:/2; deep mux:1.25; pixel:54m
         act_writel(0x00090c50, CMU_TVOUTPLL);
         }else if (deep_color == DEEP_COLOR_36_BIT) {
         //pll1:216m; mux:/2; deep mux:1.5; pixel:54m
         act_writel(0x000a0c50, CMU_TVOUTPLL);
         }
         }*/
        asoc_520x_hdmi_tvoutpll_cfg(CLK_NAME_TVOUT1PLL, 216000, deep_color, vid,
                _3d);
        break;

    case VID1280x720P_60_16VS9:
    case VID1920x1080I_60_16VS9:
    case VID1920x1080I_50_16VS9:
    case VID1920x1080P_24_16VS9:
    case VID1920x1080P_25_16VS9:
    case VID1920x1080P_30_16VS9:
        /*if (_3d == _3D) { 
         //pll0:297m; mux:/1; deep mux:1; pixel:148.5m
         act_writel(0x00080009, CMU_TVOUTPLL); 
         
         } else {
         if (deep_color == DEEP_COLOR_24_BIT) {
         //pll0:297m; mux:/2; deep mux:1; pixel:74.25m
         act_writel(0x00080019, CMU_TVOUTPLL);  
         }else if (deep_color == DEEP_COLOR_30_BIT) {
         //pll0:297m; mux:/2; deep mux:1.25; pixel:74.25m
         act_writel(0x00090019, CMU_TVOUTPLL); 
         }else if (deep_color == DEEP_COLOR_36_BIT) {
         //pll0:297m; mux:/2; deep mux:1.5; pixel:74.25m
         act_writel(0x000a0019, CMU_TVOUTPLL);
         }
         }*/
        asoc_520x_hdmi_tvoutpll_cfg(CLK_NAME_TVOUT0PLL, 297000, deep_color, vid,
                _3d);
        break;

    case VID1280x720P_50_16VS9: //same show with pal
        /*if (_3d == _3D) { 
         //pll0:297m; mux:/1; deep mux:1; pixel:148.5m
         act_writel(0x00080009, CMU_TVOUTPLL); 
         
         } else {
         if (deep_color == DEEP_COLOR_24_BIT) {
         //pll0:297m; mux:/2; deep mux:1; pixel:74.25m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         //pll1:enable,216m
         act_writel(0x00080c19, CMU_TVOUTPLL);  
         else
         //pll1:disable
         act_writel(0x00080019, CMU_TVOUTPLL); 
         }else if (deep_color == DEEP_COLOR_30_BIT) {
         //pll0:297m; mux:/2; deep mux:1.25; pixel:74.25m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         //pll1:enable,216m
         act_writel(0x00090c19, CMU_TVOUTPLL); 
         else
         //pll1:disable
         act_writel(0x00090019, CMU_TVOUTPLL); 
         }else if (deep_color == DEEP_COLOR_36_BIT) {
         //pll0:297m; mux:/2; deep mux:1.5; pixel:74.25m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         //pll1:enable,216m
         act_writel(0x000a0c19, CMU_TVOUTPLL);
         else 
         //pll1:disable
         act_writel(0x000a0019, CMU_TVOUTPLL);
         }
         }*/
        asoc_520x_hdmi_tvoutpll_cfg(CLK_NAME_TVOUT0PLL, 297000, deep_color, vid,
                _3d);
        break;

    case VID1280X720P_59P94_16VS9: //same show with ntsc 
        /*if (_3d == _3D) { 
         //pll0:297/1.001m; mux:/1; deep mux:1; pixel:148.5/1.001m
         act_writel(0x0008000b, CMU_TVOUTPLL);
         } else {
         if (deep_color == DEEP_COLOR_24_BIT) {
         //pll0:297/1.001m; mux:/2; deep mux:1; pixel:74.25/1.001m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         act_writel(0x0008081b, CMU_TVOUTPLL); 
         else 
         act_writel(0x0008001b, CMU_TVOUTPLL); 
         }else if (deep_color == DEEP_COLOR_30_BIT) {
         
         //pll0:297/1.001m; mux:/2; deep mux:1.25; pixel:74.25/1.001m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         act_writel(0x00090c1b, CMU_TVOUTPLL);  
         else 
         act_writel(0x0009001b, CMU_TVOUTPLL);  
         }else if (deep_color == DEEP_COLOR_36_BIT) {
         //pll0:297/1.001m; mux:/2; deep mux:1.5; pixel:74.25/1.001m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         act_writel(0x000a0c1b, CMU_TVOUTPLL);   
         else
         act_writel(0x000a001b, CMU_TVOUTPLL); 
         
         }
         
         }*/
        asoc_520x_hdmi_tvoutpll_cfg(CLK_NAME_TVOUT0PLL, 296000, deep_color, vid,
                _3d);

        break;

    case VID1920x1080P_60_16VS9: //need check
        /*if (_3d == _3D) { 
         //pll0:297m; mux:/1; deep mux:1; pixel:148.5m
         act_writel(0x00080009, CMU_TVOUTPLL);
         
         } else {
         if (deep_color == DEEP_COLOR_24_BIT) {
         //pll0:297m; mux:/1; deep mux:1; pixel:148.5m
         act_writel(0x00080009, CMU_TVOUTPLL);  
         } else if (deep_color == DEEP_COLOR_30_BIT) {
         //pll0:297m; mux:/1; deep mux:1.25; pixel:148.5m
         act_writel(0x00090009, CMU_TVOUTPLL);
         }else if (deep_color == DEEP_COLOR_36_BIT) {
         //pll0:297m; mux:/1; deep mux:1; pixel:148.5m
         act_writel(0x000a0009, CMU_TVOUTPLL); 

         }
         
         }*/
        asoc_520x_hdmi_tvoutpll_cfg(CLK_NAME_TVOUT0PLL, 297000, deep_color, vid,
                _3d);
        break;

    case VID1920x1080P_50_16VS9: //same show with pal
        /*if (_3d == _3D) { 
         //pll0:297m; mux:/1; deep mux:1; pixel:148.5m
         act_writel(0x00080009, CMU_TVOUTPLL);
         
         } else {
         if (deep_color == DEEP_COLOR_24_BIT) {
         //pll0:297m; mux:/1; deep mux:1; pixel:148.5m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         //pll1:enable,216m
         act_writel(0x00080c09, CMU_TVOUTPLL);  
         else 
         //pll1:disable
         act_writel(0x00080009, CMU_TVOUTPLL); 
         } else if (deep_color == DEEP_COLOR_30_BIT) {
         //pll0:297m; mux:/1; deep mux:1.25; pixel:148.5m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         //pll1:enable,216m
         act_writel(0x00090c09, CMU_TVOUTPLL);
         else
         //pll1:disable
         act_writel(0x00090009, CMU_TVOUTPLL); 
         }else if (deep_color == DEEP_COLOR_36_BIT) {
         //pll0:297m; mux:/1; deep mux:1.5; pixel:148.5m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         //pll1:enable,216m
         act_writel(0x000a0c09, CMU_TVOUTPLL); 
         else
         //pll1:disable
         act_writel(0x000a0009, CMU_TVOUTPLL); 

         }
         
         }*/
        asoc_520x_hdmi_tvoutpll_cfg(CLK_NAME_TVOUT0PLL, 297000, deep_color, vid,
                _3d);
        break;

    case VID1920x1080P_59P94_16VS9: //same show with ntsc
        /*if (_3d == _3D) { 
         //pll0:297/1.001m; mux:/1; deep mux:1; pixel:148.5/1.001m
         act_writel(0x0008000b, CMU_TVOUTPLL);
         
         } else {
         if (deep_color == DEEP_COLOR_24_BIT) {
         //pll0:297/1.001m; mux:/1; deep mux:1; pixel:148.5/1.001m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         //pll1:enable,216m
         act_writel(0x00080c0b, CMU_TVOUTPLL);  
         else 
         //pll1:disable
         act_writel(0x0008000b, CMU_TVOUTPLL); 
         } else if (deep_color == DEEP_COLOR_30_BIT) {
         //pll0:297/1.001m; mux:/1; deep mux:1.25; pixel:148.5/1.001m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         //pll1:enable,216m
         act_writel(0x00090c0b, CMU_TVOUTPLL);
         else
         //pll1:disable
         act_writel(0x0009000b, CMU_TVOUTPLL); 
         }else if (deep_color == DEEP_COLOR_36_BIT) {
         //pll0:297/1.001m; mux:/1; deep mux:1.5; pixel:148.5/1.001m
         if (act_readl(CMU_TVOUTPLL) & (1 << 11))
         //pll1:enable,216m
         act_writel(0x000a0c0b, CMU_TVOUTPLL); 
         else
         //pll1:disable
         act_writel(0x000a000b, CMU_TVOUTPLL); 

         }
         
         }*/
        asoc_520x_hdmi_tvoutpll_cfg(CLK_NAME_TVOUT0PLL, 296000, deep_color, vid,
                _3d);
        break;

    default:
        printk("%s,don't support this format\n", __func__);
        break;
    }
}

void asoc_520x_hdmi_video_interface_setting(
        struct hdmi_video_config *video_config) {
    int video2to1scaler = video_config->settings->video2to1scaler;

    //HDMI_DEBUG("[ %s start]\n", __func__);

    if (video2to1scaler)
        act_writel(video_config->parameters->victl | (1 << 29), HDMI_VICTL);
    else
        act_writel(video_config->parameters->victl, HDMI_VICTL);

    //HDMI_DEBUG("act_readl(DMA_CTL):%x\n", act_readl(DMA_CTL));
   // HDMI_DEBUG("BDMA0_MODE(0x%x) value is 0x%x\n", BDMA0_MODE,
    //        act_readl(BDMA0_MODE));

    /*video interface config*/
    act_writel(video_config->parameters->vivsync, HDMI_VIVSYNC);
    act_writel(video_config->parameters->vivhsync, HDMI_VIVHSYNC);
    act_writel(video_config->parameters->vialseof, HDMI_VIALSEOF);
    act_writel(video_config->parameters->vialseef, HDMI_VIALSEEF);
    act_writel(video_config->parameters->viadlse, HDMI_VIADLSE);
    act_writel(
            act_readl(HDMI_SCHCR)
                    & ~(HDMI_SCHCR_HSYNCPOLINV | HDMI_SCHCR_VSYNCPOLINV| HDMI_SCHCR_HSYNCPOLIN | HDMI_SCHCR_VSYNCPOLIN)
                    | (video_config->parameters->vhsync_p+ video_config->parameters->vhsync_inv),
            HDMI_SCHCR);

}

void asoc_520x_hdmi_pixel_coding(struct hdmi_settings *settings) {
    //HDMI_DEBUG("[ %s start]\n", __func__);
    /*pixel coding(RGB default)*/
    if (settings->pixel_encoding == VIDEO_PEXEL_ENCODING_YCbCr444)
        act_setl(HDMI_SCHCR_PIXELENCFMT(0x2), HDMI_SCHCR); //YUV444

    else
        act_clearl(HDMI_SCHCR_PIXELENCFMT(0x3), HDMI_SCHCR); //RGB
    printk("[%s]pixel encoding:%d\n", __func__, settings->pixel_encoding);
}

void asoc_520x_hdmi_deepcolor_setting(struct hdmi_video_config *video_config) {

    //HDMI_DEBUG("[ %s start]\n", __func__);

    /*deep color,8bit default*/
    if (video_config->settings->deep_color == DEEP_COLOR_24_BIT) { //8bit
        act_writel(act_readl(HDMI_SCHCR) & ~HDMI_SCHCR_DEEPCOLOR_MASK,
                HDMI_SCHCR);
        act_writel(video_config->parameters->dipccr_24, HDMI_DIPCCR); //Max 18 and 4 Island packets in Vertical and horizontal Blanking Interval

    } else if (video_config->settings->deep_color == DEEP_COLOR_30_BIT) { //10bit
        act_writel(
                (act_readl(HDMI_SCHCR) & ~HDMI_SCHCR_DEEPCOLOR_MASK)
                        | HDMI_SCHCR_DEEPCOLOR(0x1), HDMI_SCHCR);
        act_writel(video_config->parameters->dipccr_30, HDMI_DIPCCR); //Max 18 and 4 Island packets in Vertical and horizontal Blanking Interval

    } else if (video_config->settings->deep_color == DEEP_COLOR_36_BIT) { //12bit
        act_writel(
                (act_readl(HDMI_SCHCR) & ~HDMI_SCHCR_DEEPCOLOR_MASK)
                        | HDMI_SCHCR_DEEPCOLOR(0x2), HDMI_SCHCR);
        act_writel(video_config->parameters->dipccr_36, HDMI_DIPCCR); //Max 18 and 4 Island packets in Vertical and horizontal Blanking Interval
    }
}

void asoc_520x_hdmi_3d_setting(struct hdmi_settings *settings) {
   // HDMI_DEBUG("[ %s start]\n", __func__);

    if (settings->_3d == _3D)
        act_writel(act_readl(HDMI_SCHCR) | HDMI_SCHCR_HDMI_3D_FRAME_FLAG,
                HDMI_SCHCR);

    else
        act_writel(act_readl(HDMI_SCHCR) & ~HDMI_SCHCR_HDMI_3D_FRAME_FLAG,
                HDMI_SCHCR);

}

void asoc_520x_hdmi_video_configure(struct hdmi_ip_data *ip_data) {
   // HDMI_DEBUG("[ %s start]\n", __func__);

    /*video interface*/
    asoc_520x_hdmi_video_interface_setting(&ip_data->v_cfg);
    /*pixel coding ,include RGB and YUV*/
    asoc_520x_hdmi_pixel_coding(ip_data->v_cfg.settings);
    /*deep color settings,include 8bit/10bit/12bit*/
    asoc_520x_hdmi_deepcolor_setting(&ip_data->v_cfg);
    /*set 3d format*/
    asoc_520x_hdmi_3d_setting(ip_data->v_cfg.settings);

}

int asoc_520x_hdmi_gcp_configure(struct hdmi_ip_data *ip_data) {
    unsigned int deep_color = ip_data->v_cfg.settings->deep_color;

    //HDMI_DEBUG("[ %s start]\n", __func__);

    if (deep_color == DEEP_COLOR_24_BIT) {
        /*Clear AVMute flag and enable GCP transmission*/
        act_writel(0x0 | 0x80000002, HDMI_GCPCR);

    } else if (deep_color == DEEP_COLOR_30_BIT) {
        act_writel(0x40000050 | 0x80000002, HDMI_GCPCR);

    } else if (deep_color == DEEP_COLOR_36_BIT) {
        act_writel(0x40000060 | 0x80000002, HDMI_GCPCR);

    } else {
        printk("don't support the deep color mode!\n");
        return -EINVAL;
    }

    return 0;
}

int asoc_520x_hdmi_gen_infoframe(struct hdmi_ip_data *ip_data) {

    struct hdmi_settings *settings = ip_data->v_cfg.settings;

    //HDMI_DEBUG("[ %s start]\n", __func__);

    if (asoc_520x_hdmi_gcp_configure(ip_data))
        return -EINVAL;

    if (hdmi_gen_avi_infoframe(ip_data))
        return -EINVAL;

    if (settings->color_xvycc != YCC601_YCC709) {
        hdmi_gen_gbd_infoframe(settings);
    }

    if (settings->_3d != _3D_NOT) {
        hdmi_gen_vs_infoframe(settings);
    }

    return 0;
}

int asoc_520x_hdmi_read_edid(struct hdmi_ip_data *ip_data, u8 *edid, int len) {

    //HDMI_DEBUG("[ %s start]\n", __func__);
    return 0;
}


unsigned char sha_1(unsigned char *sha_output, unsigned char *M_input, int len) {
    unsigned int Kt[4] = {
        0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6 
    };
    
    unsigned int h[5] = {
        0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476,0xc3d2e1f0 
    };

    int t, n, n_block;
    unsigned int Ft, x, temp;
    unsigned int A, B, C, D, E;
    int i, j;
    unsigned char sha_input[700];
    unsigned char *out_temp;
    unsigned int W[80];

    memset(sha_input, 0, 700);

    for (i = 0; i < len; i++)
        sha_input[i] = M_input[i];

    /* do message padding */
    n_block = (len * 8 + 1 + 64) / 512 + 1;
    sha_input[len] = 0x80;
    HDCP_DEBUG("n_block=%d\n", n_block);
    /* set len */

    sha_input[(n_block - 1) * 64 + 60] = (unsigned char)(
            ((len * 8) & 0xff000000) >> 24);
    sha_input[(n_block - 1) * 64 + 61] = (unsigned char)(
            ((len * 8) & 0x00ff0000) >> 16);
    sha_input[(n_block - 1) * 64 + 62] = (unsigned char)(((len * 8) & 0x0000ff00) >> 8);
    sha_input[(n_block - 1) * 64 + 63] = (unsigned char)((len * 8) & 0x000000ff);

    for (j = 0; j < n_block; j++) {
        HDCP_DEBUG("\nBlock %d\n", j);
        for (i = 0; i < 64; i++) {
            if ((i % 16 == 0) && (i != 0))
                HDCP_DEBUG("\n0x%2x,", sha_input[j * 64 + i]);
            else
                HDCP_DEBUG("0x%2x,", sha_input[j * 64 + i]);
        }
    }
    HDCP_DEBUG("SHA sha_input end\n");

    for (n = 0; n < n_block; n++) {
        for (t = 0; t < 16; t++) {
            x = *((unsigned long *) &sha_input[n * 64 + t * 4]);
            W[t] = swapl(x);
//          if (t%2 == 0)
//              HDCP_DEBUG("\n");
//          HDCP_DEBUG("x=0x%x,W[%d]=0x%x\t",x,t,W[t]);
        }
//      HDCP_DEBUG("\n");   
        for (t = 16; t < 80; t++) {
            x = W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16];
            W[t] = SXOLN(1, x);
//          HDCP_DEBUG("W[%d]=0x%x\t",t,W[t]);
//          if (t % 3 == 0)
//              HDCP_DEBUG("\n");
        }

        A = h[0];
        B = h[1];
        C = h[2];
        D = h[3];
        E = h[4];

        for (t = 0; t < 80; t++) {
            if (t >= 0 && t <= 19)
                Ft = (B & C) | ((~B) & D);
            else if (t >= 20 && t <= 39)
                Ft = B ^ C ^ D;
            else if (t >= 40 && t <= 59)
                Ft = (B & C) | (B & D) | (C & D);
            else
                Ft = (B ^ C ^ D);

            temp = SXOLN(5, A) + Ft + E + W[t] + Kt[t / 20]; //temp = S^5(A) + f(t;B,C,D) + E + W(t) + K(t)

            E = D;
            D = C;
            C = SXOLN(30, B); //C = S^30(B)
            B = A;
            A = temp;
        }

        h[0] += A; // H0 = H0 + A
        h[1] += B; // H1 = H1 + B
        h[2] += C; //H2 = H2 + C
        h[3] += D; //H3 = H3 + D
        h[4] += E; //H4 = H4 + E
    }

    HDCP_DEBUG("\noutput original sha_input:\n");
    for (i = 0; i < hLen / 4; i++)
        HDCP_DEBUG("0x%x\t", h[i]);

    HDCP_DEBUG("\nconvert to little endien\n");
    for (i = 0; i < hLen / 4; i++) {
        h[i] = swapl(h[i]);
        HDCP_DEBUG("0x%x\t", h[i]);
    }
    HDCP_DEBUG("\n");
//
//  /* conerting for match Vp reading from Rx */ 
//  HDCP_DEBUG("\nconvert for match Vp reading from Rx\n");
//  for(i=0; i<hLen/4; i++)
//  {
//      h[i] = swapV(h[i]);
//      printk("h[%d]=0x%x\t",i,h[i]);
//  }
    /* copy to output pointer */
    out_temp = (unsigned char *) h;
    HDCP_DEBUG("out_temp:\n");
    for (i = 0; i < hLen; i++) {
        sha_output[i] = out_temp[i];
        HDCP_DEBUG("0x%x\t", out_temp[i]);
    }
    HDCP_DEBUG("\n");

    return 0;
}

int check_one_number(unsigned char data)
{
    int num = 0, i;
    for(i=0;i<8;i++) {
	if ((data >> i) & 0x1) {
		num ++;
	}
    }
    return num;
}

int hdcp_read_key(void)
{
    int array, col, index;
    int num = 0;
    int i = 0;
    unsigned char key[308];
    unsigned char sha1_verify[20];
    unsigned char sha1_result[20];
#ifdef READ_HDCP_KEY_FROM_FILE
    struct file *fp = NULL;
    ssize_t result;
    mm_segment_t fs;
    loff_t pos = 0;
    
    fp = filp_open("/misc/modules/Actions_keys_0.bin", O_RDONLY, 0);
    if (IS_ERR(fp)) {
        HDCP_DEBUG("Sorry,the file can not be found ! \n");		
        return 1;	
    }

    //result = fp->f_op->read(fp, key, sizeof(key), &fp->f_pos);
    fs = get_fs();
    set_fs(KERNEL_DS);

    result = vfs_read(fp, key, sizeof(key), &pos);
    filp_close(fp, NULL);
    set_fs(fs);
    if(result >= 0) {
        for (i = 0; i < sizeof(key); i++) {
            if (!(i % 16)) 
                HDCP_DEBUG("\n");
            HDCP_DEBUG(" %x", key[i]);
        }
        HDCP_DEBUG("\n\n");
    } else {
        HDCP_DEBUG("read file failed\n");
        return 1;
    }
#endif

#ifdef READ_HDCP_KEY_FROM_FLASH
    //NAND_GetHDCPKey(key);
    NAND_GetMiscInfo(MISC_INFO_TYPE_HDCP, key, sizeof(key));
#endif

    //aksv
#ifdef I2C_RW_DEBUG
        for (i =0; i < sizeof(aksv); i++) {
            aksv[i] = key[i];
        }
#else
        for (i =1; i < sizeof(aksv); i++) {
            aksv[i] = key[i -1];
        }
#endif
    for (i = 0; i < KSV_LENGTH; i++) {
        num += check_one_number(key[i]);
    }
    
    //key
    if (num == 20) {
    	HDCP_DEBUG("aksv is valid\n");
	HDCP_DEBUG("hdcp key is as follows:\n");
             for (array = 0; array < KEY_ARRAY_LENGTH; array++) {
                 for (col = 0; col < KEY_COL_LENGTH; col++) {
	       index = (KSV_LENGTH + KSV_RESERVED + (array + 1) * KEY_COL_LENGTH) - col - 1;
                     sprintf(&test_keyr0[array][2 * col], "%x", ((key[index] & 0xf0) >> 4) & 0x0f);
                     sprintf(&test_keyr0[array][2 * col +1], "%x", key[index] & 0x0f);
                     
                     HDCP_DEBUG("key_tmp[%d][%d]:%c\n", array, 2 * col, test_keyr0[array][2 * col]);
                     HDCP_DEBUG("key_tmp[%d][%d]:%c\n", array, 2 * col + 1, test_keyr0[array][2 * col + 1]);
                     
                }
	   HDCP_DEBUG("%s\n", test_keyr0[array]);
            }
            HDCP_DEBUG("\n\nhdcp key parse finished\n\n");
            HDCP_DEBUG("verify code is as follows:\n");
            for (i = 0; i < sizeof(sha1_verify); i++) {
	     index = i + KSV_LENGTH + KSV_RESERVED + KEY_LENGTH;
                   //sprintf(&sha1_verify[i], "%x",  key[index]);
                   sha1_verify[i] = key[index];
	     HDCP_DEBUG("sha1_verify[%d]:%x\n", i, sha1_verify[i]);
            }
            HDCP_DEBUG("verify code parse finished\n\n");
            sha_1(sha1_result, key, KSV_LENGTH + KSV_RESERVED + KEY_LENGTH);

    } else {
        return 1;
    }
    
    //verify
    for(i = 0; i < VERIFY_LENGTH; i++) {
	if (sha1_verify[i] != sha1_result[i]) {
	        printk("sha1 verify error!\n");
	        return 1;
	}
    }
    printk("[%s]sha1 verify success !\n", __func__);

    return 0;
    
}

int set_hdcpmode_by_hdmimode(struct hdmi_ip_data *ip_data) {
    int tmp;
    if (ip_data->sink_cap.hdmi_mode == HDMI_MODE_HDMI) {
        tmp = act_readl(HDCP_CR);
        tmp = tmp | (0x1 << 31);
        act_writel(tmp, HDCP_CR);

        act_writel(0x20e1fe, HDCP_OWR);

    } else if (ip_data->sink_cap.hdmi_mode == HDMI_MODE_HDMI) {
        tmp = act_readl(HDCP_CR);
        tmp = tmp & (~(0x1 << 31));
        act_writel(tmp, HDCP_CR);

        act_writel(0x9207e, HDCP_OWR);
    } else {
        return 1;
    }

    return 0;
}
int hdcp_detect_riupdated(void) {
    unsigned int tmp;
    tmp = act_readl(HDCP_SR);
    if (tmp & HDCP_SR_RIUPDATED) {
        act_writel(tmp & (~HDCP_SR_PJUPDATED), HDCP_SR);
        return 1;
    } else {
        return 0;
    }
}
void disable_hdcp_repeater(void) {
    unsigned int tmp;
    tmp = act_readl(HDCP_CR);
    tmp &= (~HDCP_CR_DOWNSTRISREPEATER);
    act_writel(tmp, HDCP_CR);
    tmp = act_readl(HDCP_CR);
}

void enable_hdcp_repeater(void) {
    unsigned int tmp;
    tmp = act_readl(HDCP_CR);
    tmp |= HDCP_CR_DOWNSTRISREPEATER;
    act_writel(tmp, HDCP_CR);
    tmp = act_readl(HDCP_CR);
}
void enable_hdcp_ri_interrupt(void) {
    unsigned int tmp;
    tmp = act_readl(HDCP_CR);
    tmp |= HDCP_CR_ENRIUPDINT;
    act_writel(tmp, HDCP_CR);
    tmp = act_readl(HDCP_CR);
}

void disable_hdcp_ri_interrupt(void) {
    unsigned int tmp;
    tmp = act_readl(HDCP_CR);
    tmp &= ~(HDCP_CR_ENRIUPDINT);
    act_writel(tmp, HDCP_CR);
    tmp = act_readl(HDCP_CR);
}

void set_hdcp_ri_pj(void) {
    act_writel(act_readl(HDCP_CR) | HDCP_CR_EN1DOT1_FEATURE, HDCP_CR);
    act_writel(0x7f0f, HDCP_ICR);
    act_readl(HDCP_ICR);

}

void set_hdcp_to_Authenticated_state(void) {
    // Authenticated 
    // set HDCP module to authenticated state 
    unsigned int tmp;
    tmp = act_readl(HDCP_CR);
    tmp |= HDCP_CR_DEVICEAUTHENTICATED;
    act_writel(tmp, HDCP_CR);
    // start encryption    
    tmp = act_readl(HDCP_CR);
    tmp |= HDCP_CR_HDCP_ENCRYPTENABLE;
    act_writel(tmp, HDCP_CR);

    tmp = act_readl(HDCP_CR);

}

void enable_ri_update_check(void) {
    unsigned int tmp;

    tmp = act_readl(HDCP_SR);
    tmp |= HDCP_SR_RIUPDATED;
    act_writel(tmp, HDCP_SR);
    act_readl(HDCP_SR);

    tmp = act_readl(HDCP_CR);
    tmp |= HDCP_CR_ENRIUPDINT;
    act_writel(tmp, HDCP_CR);
    act_readl(HDCP_CR);
}

int hdcp_ForceUnauthentication(void) {
    /* disable link integry check */
    /* force Encryption disable */
    unsigned int tmp;
    tmp = act_readl(HDCP_CR);
    tmp = (tmp & (~HDCP_CR_ENRIUPDINT) & (~HDCP_CR_ENPJUPDINT) & (~HDCP_CR_HDCP_ENCRYPTENABLE))
            | HDCP_CR_FORCETOUNAUTHENTICATED;
    act_writel(tmp, HDCP_CR);
    act_readl(HDCP_CR);
    /* force HDCP module to unauthenticated state */
    //P_HDCP_CR |= HDCP_CR_FORCE_UNAUTH;
    return 0;
}
#ifdef I2C_RW_DEBUG
int hdcp_FreeRunGetAn(unsigned char *an) {
    /* Get An */
    /* get An influence from CRC64 */
    unsigned int tmp;
    tmp = act_readl(HDCP_CR);

#if 0
    tmp |= HDCP_CR_AN_INF_REQ|HDCP_CR_LOAD_AN;
#else
    tmp |= HDCP_CR_ANINFREQ | HDCP_CR_ANINFLUENCEMODE;
#endif

    act_writel(tmp, HDCP_CR);
    tmp = act_readl(HDCP_CR);

#if 0
    tmp |= HDCP_CR_AUTH_REQ;
#else
    tmp |= HDCP_CR_AUTHREQUEST;
#endif

    act_writel(tmp, HDCP_CR);

    //P_HDCP_CR |= HDCP_CR_AN_INF_REQ; //25 bit

    /* set An Influence Mode, influence will be load from AnIR0, AnIR1 */
    //P_HDCP_CR |= HDCP_CR_LOAD_AN;  //7 bit
    /* trigger to get An */
    //P_HDCP_CR |= HDCP_CR_AUTH_REQ;  //0 bit  --写1，生成an
    HDCP_DEBUG("[hdcp_FreeRunGetAn]:wait An\n");
#if 0
    while(!(act_readl(HDCP_SR) & (HDCP_SR_AN_READY))); //等待An ready
#else
    while (!(act_readl(HDCP_SR) & (HDCP_SR_ANREADY)))
        ; //等待An ready
#endif
    HDCP_DEBUG("[hdcp_FreeRunGetAn]:wait An ok\n");
    /* leave An influence mode */
    tmp = act_readl(HDCP_CR);
#if 0
    tmp &= (~HDCP_CR_LOAD_AN);
#else
    tmp &= (~HDCP_CR_ANINFLUENCEMODE);
#endif
    act_writel(tmp, HDCP_CR);

    /* 
     * Convert HDCP An from bit endien to little endien 
     * HDCP An should stored in little endien, 
     * but HDCP HW store in bit endien. 
     */
    tmp = act_readl(HDCP_ANLR);
    an[0] = tmp & 0xff;
    an[1] = (tmp >> 8) & 0xff;
    an[2] = (tmp >> 16) & 0xff;
    an[3] = (tmp >> 24) & 0xff;

    tmp = act_readl(HDCP_ANMR);

    an[4] = tmp & 0xff;
    an[5] = (tmp >> 8) & 0xff;
    an[6] = (tmp >> 16) & 0xff;
    an[7] = (tmp >> 24) & 0xff;

    int i;
    for (i = 0; i < 8; i++)
        HDCP_DEBUG("an[%d]:0x%x\n", i, an[i]);
    
    return 0;
}
#else
int hdcp_FreeRunGetAn(unsigned char *an) {
    /* Get An */
    /* get An influence from CRC64 */
    unsigned int tmp;
    int i;
    tmp = act_readl(HDCP_CR);

#if 0
    tmp |= HDCP_CR_AN_INF_REQ|HDCP_CR_LOAD_AN;
#else
    tmp |= HDCP_CR_ANINFREQ | HDCP_CR_ANINFLUENCEMODE;
#endif

    act_writel(tmp, HDCP_CR);
    tmp = act_readl(HDCP_CR);

#if 0
    tmp |= HDCP_CR_AUTH_REQ;
#else
    tmp |= HDCP_CR_AUTHREQUEST;
#endif

    act_writel(tmp, HDCP_CR);

    //P_HDCP_CR |= HDCP_CR_AN_INF_REQ; //25 bit

    /* set An Influence Mode, influence will be load from AnIR0, AnIR1 */
    //P_HDCP_CR |= HDCP_CR_LOAD_AN;  //7 bit
    /* trigger to get An */
    //P_HDCP_CR |= HDCP_CR_AUTH_REQ;  //0 bit  --写1，生成an
    HDCP_DEBUG("[hdcp_FreeRunGetAn]:wait An\n");
#if 0
    while(!(act_readl(HDCP_SR) & (HDCP_SR_AN_READY))); //等待An ready
#else
    while (!(act_readl(HDCP_SR) & (HDCP_SR_ANREADY)))
        ; //等待An ready
#endif
    HDCP_DEBUG("[hdcp_FreeRunGetAn]:wait An ok\n");
    /* leave An influence mode */
    tmp = act_readl(HDCP_CR);
#if 0
    tmp &= (~HDCP_CR_LOAD_AN);
#else
    tmp &= (~HDCP_CR_ANINFLUENCEMODE);
#endif
    act_writel(tmp, HDCP_CR);

    /* 
     * Convert HDCP An from bit endien to little endien 
     * HDCP An should stored in little endien, 
     * but HDCP HW store in bit endien. 
     */
    an[0] = 0x18;
    tmp = act_readl(HDCP_ANLR);
    an[1] = tmp & 0xff;
    an[2] = (tmp >> 8) & 0xff;
    an[3] = (tmp >> 16) & 0xff;
    an[4] = (tmp >> 24) & 0xff;

    tmp = act_readl(HDCP_ANMR);

    an[5] = tmp & 0xff;
    an[6] = (tmp >> 8) & 0xff;
    an[7] = (tmp >> 16) & 0xff;
    an[8] = (tmp >> 24) & 0xff;

    for (i = 0; i < 9; i++)
        HDCP_DEBUG("an[%d]:0x%x\n", i, an[i]);
    
    return 0;
}
#endif

int hdcp_AuthenticationSequence(struct hdmi_ip_data *ip_data) {
    /* force Encryption disable */
    //  P_HDCP_CR &= ~HDCP_CR_ENC_ENABLE; //6 bit
    /* reset Km accumulation */
    //  P_HDCP_CR |= HDCP_CR_RESET_KM_ACC; //4 bit
    unsigned int tmp;
    tmp = act_readl(HDCP_CR);
    tmp = (tmp & (~HDCP_CR_HDCP_ENCRYPTENABLE)) | HDCP_CR_RESETKMACC;
    act_writel(tmp, HDCP_CR);

    /* set Bksv to accumulate Km */
    hdcp_GenKm(ip_data);

    /* disable Ri update interrupt */
    tmp = act_readl(HDCP_CR);
    tmp &= (~HDCP_CR_ENRIUPDINT);
    act_writel(tmp, HDCP_CR);

    /* clear Ri updated pending bit */
    tmp = act_readl(HDCP_CR);
    tmp |= HDCP_SR_RIUPDATED | HDCP_CR_AUTHCOMPUTE;
    act_writel(tmp, HDCP_CR);

    /* trigger hdcpBlockCipher at authentication */
    //  P_HDCP_CR |= HDCP_CR_AUTH_COMP; //1 bit
    /* wait 48+56 pixel clock to get R0 */
    //  while(!(P_HDCP_SR & HDCP_SR_RI_UPDATE));
    while (!(act_readl(HDCP_SR) & HDCP_SR_RIUPDATED))
        ;
    //HDCP_DEBUG("wait Ri\n");
    /* get Ri */
    ip_data->hdcp.Ri = (act_readl(HDCP_LIR) >> 16) & 0xffff;
    HDCP_DEBUG("Ri:0x%x\n", ip_data->hdcp.Ri);

    return 0;
}

int hdcp_GenKm(struct hdmi_ip_data *ip_data) 
{
    unsigned char key[11];
    int i, j;

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
            if (ip_data->hdcp.Bksv[i] & (1 << j)) {
                c2ln14(key, test_keyr0[i * 8 + j]);
                hdcp_SetKm(&key[0], 0x55);
            }
        }
    }
    return 0;
}

int hdcp_SetKm(unsigned char *key, int pnt) {
    unsigned int tmp;
    unsigned char dKey[11];
    dKey[0] = key[0] ^ pnt;
    dKey[1] = ~key[1] ^ dKey[0];
    dKey[2] = key[2] ^ dKey[1];
    dKey[3] = key[3] ^ dKey[2];
    dKey[4] = key[4] ^ dKey[3];
    dKey[5] = ~key[5] ^ dKey[4];
    dKey[6] = ~key[6] ^ dKey[5];
    /* write to HW */
    /*P_HDCP_DPKLR*/
    tmp = pnt | (dKey[0] << 8) | (dKey[1] << 16) | (dKey[2] << 24);
    act_writel(tmp, HDCP_DPKLR);
    /*P_HDCP_DPKMR*/
    tmp = dKey[3] | (dKey[4] << 8) | (dKey[5] << 16) | (dKey[6] << 24);
    act_writel(tmp, HDCP_DPKMR);

    /* trigger accumulation */

    while (!(act_readl(HDCP_SR) & (1 << 3)))
        ;
    return 0;
}

/* convert INT8 number to little endien number */
int c2ln14(unsigned char *num, char *a) {
    int i;
    int n = 14;
    for (i = 0; i < 11; i++) {
        num[i] = 0;
    }

    for (i = 0; i < n; i++) {
        if (i % 2) {
            if (a[n - i - 1] >= '0' && a[n - i - 1] <= '9') {
                num[i / 2] |= (a[n - i - 1] - '0') << 4;
            } else if (a[n - i - 1] >= 'a' && a[n - i - 1] <= 'f') {
                num[i / 2] |= (a[n - i - 1] - 'a' + 10) << 4;
            } else if (a[n - i - 1] >= 'A' && a[n - i - 1] <= 'F') {
                num[i / 2] |= (a[n - i - 1] - 'A' + 10) << 4;
            }
        } else {
            if (a[n - i - 1] >= '0' && a[n - i - 1] <= '9') {
                num[i / 2] |= (a[n - i - 1] - '0');
            } else if (a[n - i - 1] >= 'a' && a[n - i - 1] <= 'f') {
                num[i / 2] |= (a[n - i - 1] - 'a' + 10);
            } else if (a[n - i - 1] >= 'A' && a[n - i - 1] <= 'F') {
                num[i / 2] |= (a[n - i - 1] - 'A' + 10);
            }
        }
    }
    return 0;
}

int hdcp_ReadKsvList(unsigned char *Bstatus, unsigned char *ksvlist) {
    int cnt;

    /* get device count in Bstatus [6:0] */
    if (i2c_hdcp_read(Bstatus, 0x41, 2) < 0) {
        if (i2c_hdcp_read(Bstatus, 0x41, 2) < 0) {
            return 0;
        }
    }

    if (Bstatus[0] & 0x80) //if Max_devs_exceeded then quit
        return 0;

    if (Bstatus[1] & 0x8) //if Max_cascade_exceeded then quit
        return 0;

    cnt = Bstatus[0] & 0x7f;

    if (!cnt) {
        return 1;
    }

    if (i2c_hdcp_read(ksvlist, 0x43, 5 * cnt) < 0) {
        if (i2c_hdcp_read(ksvlist, 0x43, 5 * cnt) < 0) {
            return 0;
        }
    }

    return 1;
}
int hdcp_ReadVprime(unsigned char *Vp) {
    /* read Vp */
    if (i2c_hdcp_read(Vp, 0x20, 20) != 0) {
        if (i2c_hdcp_read(Vp, 0x20, 20) != 0) {
            return 0;
        }
    }
    return 1;
}

int do_Vmatch(struct hdmi_ip_data *ip_data, unsigned char *v, unsigned char *ksvlist, unsigned char *bstatus,
        unsigned char * m0) {
    unsigned int tmp;
    int data_counter;
    unsigned char sha_1_input_data[MAX_SHA_1_INPUT_LENGTH];
    int nblock, llen;
    int cnt2 = ip_data->hdcp.Bstatus[0] & 0x7f;
    int i, j;
    volatile int hdcp_shacr = 0;

    llen = 8 * M0_LENGTH + 8 * BSTATUS_LENGTH + cnt2 * 8 * KSV_LENGTH;

    for (i = 0; i < MAX_SHA_1_INPUT_LENGTH; i++)
        sha_1_input_data[i] = 0;

    for (data_counter = 0;
            data_counter < cnt2 * KSV_LENGTH + BSTATUS_LENGTH + M0_LENGTH;
            data_counter++) {
        if (data_counter < cnt2 * KSV_LENGTH) {
            sha_1_input_data[data_counter] = ksvlist[data_counter];

        } else if ((data_counter >= cnt2 * KSV_LENGTH)
                && (data_counter < cnt2 * KSV_LENGTH + BSTATUS_LENGTH)) {
            sha_1_input_data[data_counter] = bstatus[data_counter
                    - (cnt2 * KSV_LENGTH)];
        } else {
            sha_1_input_data[data_counter] = m0[data_counter
                    - (cnt2 * KSV_LENGTH + BSTATUS_LENGTH)];
        }
    }
    sha_1_input_data[data_counter] = 0x80; //block ending signal       

    nblock = (int) (data_counter / 64);
    sha_1_input_data[nblock * 64 + 62] = (unsigned char) (((data_counter * 8)
            >> 8) & 0xff); //total SHA counter high
    sha_1_input_data[nblock * 64 + 63] = (unsigned char) ((data_counter * 8)
            & 0xff); //total SHA counter low

    //  P_HDCP_SHACR |= 0x1;    //reset SHA write pointer
    tmp = act_readl(HDCP_SHACR);
    act_writel(tmp | 0x1, HDCP_SHACR);

    while (act_readl(HDCP_SHACR) & 0x1)
        ; //wait completing reset operation
    //  P_HDCP_SHACR |= 0x2;            //set new SHA-1 operation
    tmp = act_readl(HDCP_SHACR);
    act_writel(tmp | 0x2, HDCP_SHACR);

    for (i = 0; i < nblock; i++) {
        for (j = 0; j < 16; j++) {
            //P_HDCP_SHADR 
            tmp = (sha_1_input_data[i * 64 + (j * 4 + 0)] << 24)
                    | (sha_1_input_data[i * 64 + (j * 4 + 1)] << 16)
                    | (sha_1_input_data[i * 64 + (j * 4 + 2)] << 8)
                    | (sha_1_input_data[i * 64 + (j * 4 + 3)]);
            act_writel(tmp, HDCP_SHADR);
            act_readl(HDCP_SHADR);
        }
        //      P_HDCP_SHACR |= 0x4;         //Start 512bit SHA operation           
        tmp = act_readl(HDCP_SHACR);
        act_writel(tmp | 0x4, HDCP_SHACR);
        while (!(act_readl(HDCP_SHACR) & 0x8))
            ; //after 512bit SHA operation, this bit will be set to 1

        //P_HDCP_SHACR &= 0xfd;                       //clear SHAfirst bit
        tmp = act_readl(HDCP_SHACR);
        act_writel(tmp & 0xfd, HDCP_SHACR);
        act_readl(HDCP_SHACR);
    }
    for (j = 0; j < 16; j++) {
        // P_HDCP_SHADR 
        tmp = (sha_1_input_data[nblock * 64 + (j * 4 + 0)] << 24)
                | (sha_1_input_data[nblock * 64 + (j * 4 + 1)] << 16)
                | (sha_1_input_data[nblock * 64 + (j * 4 + 2)] << 8)
                | (sha_1_input_data[nblock * 64 + (j * 4 + 3)]);
        act_writel(tmp, HDCP_SHADR);
        act_readl(HDCP_SHADR);
    }
    //P_HDCP_SHACR |= 0x4;                        //Start 512bit SHA operation        
    tmp = act_readl(HDCP_SHACR);
    act_writel(tmp | 0x4, HDCP_SHACR);

    while (!(act_readl(HDCP_SHACR) & 0x8))
        ; //after 512bit SHA operation, this bit will be set to 1
    //write V
    //P_HDCP_SHADR 
    tmp = (v[3] << 24) | (v[2] << 16) | (v[1] << 8) | (v[0] << 0);
    act_writel(tmp, HDCP_SHADR);
    act_readl(HDCP_SHADR);

    //P_HDCP_SHADR 
    tmp = (v[7] << 24) | (v[6] << 16) | (v[5] << 8) | (v[4] << 0);
    act_writel(tmp, HDCP_SHADR);
    act_readl(HDCP_SHADR);

    //P_HDCP_SHADR 
    tmp = (v[11] << 24) | (v[10] << 16) | (v[9] << 8) | (v[8] << 0);
    act_writel(tmp, HDCP_SHADR);
    act_readl(HDCP_SHADR);

    //P_HDCP_SHADR 
    tmp = (v[15] << 24) | (v[14] << 16) | (v[13] << 8) | (v[12] << 0);
    act_writel(tmp, HDCP_SHADR);
    act_readl(HDCP_SHADR);

    //P_HDCP_SHADR 
    tmp = (v[19] << 24) | (v[18] << 16) | (v[17] << 8) | (v[16] << 0);
    act_writel(tmp, HDCP_SHADR);
    act_readl(HDCP_SHADR);

    //wait Vmatch

    for (i = 0; i < 3; i++) {
        j = 0;
        while ((j++) < 100)
            ;
        hdcp_shacr = act_readl(HDCP_SHACR);
        if (hdcp_shacr & 0x10) {
            return 1; //Vmatch
        }
    }
    return 0; //V unmatch

}

int hdcp_get_ri(struct hdmi_ip_data *ip_data)
{
         ip_data->hdcp.Ri = (act_readl(HDCP_LIR) >> 16) & 0xffff;
         HDCP_DEBUG("[%s Ri(master):0x%x]\n", __func__, ip_data->hdcp.Ri);

         return ip_data->hdcp.Ri;
}

int hdcp_check_ri(void) {
    int Ri, Ri_Read;
    unsigned char Ri_temp[8] = {0};
    Ri = (act_readl(HDCP_LIR) >> 16) & 0xffff;
    if (i2c_hdcp_read(Ri_temp, 0x08, 2) != 0) {
        memset(Ri_temp, 0, sizeof(Ri_temp)); 
    }
    Ri_Read = (Ri_temp[1] << 8) | Ri_temp[0];
    if (Ri != Ri_Read) {
        return -1;
    }
    return 0;
}

/*读取M0的前4字节*/
void hdcp_read_hdcp_MILR(unsigned char *M0) {
    unsigned int tmp = 0;
    tmp = act_readl(HDCP_MILR);
    M0[0] = (unsigned char) (tmp & 0xff);
    M0[1] = (unsigned char) ((tmp >> 8) & 0xff);
    M0[2] = (unsigned char) ((tmp >> 16) & 0xff);
    M0[3] = (unsigned char) ((tmp >> 24) & 0xff);
}
/*读取M0的后4字节*/
void hdcp_read_hdcp_MIMR(unsigned char *M0) {
    unsigned int tmp = 0;
    tmp = act_readl(HDCP_MIMR);
    M0[0] = (unsigned char) (tmp & 0xff);
    M0[1] = (unsigned char) ((tmp >> 8) & 0xff);
    M0[2] = (unsigned char) ((tmp >> 16) & 0xff);
    M0[3] = (unsigned char) ((tmp >> 24) & 0xff);
}
void asoc_520x_hdmi_dump_hdmi(struct hdmi_ip_data *ip_data) {
    printk("\nfollowing list cmu registers' value!\n");

    printk("CMU_TVOUTPLL0DEBUG(0x%x) value is 0x%x\n", CMU_TVOUTPLL0DEBUG,
            act_readl(CMU_TVOUTPLL0DEBUG));
    printk("CMU_TVOUTPLL1DEBUG(0x%x) value is 0x%x\n", CMU_TVOUTPLL1DEBUG,
            act_readl(CMU_TVOUTPLL1DEBUG));
    printk("CMU_TVOUTPLL(0x%x) value is 0x%x\n", CMU_TVOUTPLL,
            act_readl(CMU_TVOUTPLL));

    printk("CMU_DEVCLKEN0(0x%x) value is 0x%x\n", CMU_DEVCLKEN0,
            act_readl(CMU_DEVCLKEN0));
    printk("CMU_DEVCLKEN1(0x%x) value is 0x%x\n", CMU_DEVCLKEN1,
            act_readl(CMU_DEVCLKEN1));

    printk("\nfollowing list hdmi registers' value!\n");
    printk("HDMI_VICTL(0x%x) value is 0x%x\n", HDMI_VICTL,
            act_readl(HDMI_VICTL));
    printk("HDMI_VIVSYNC(0x%x) value is 0x%x\n", HDMI_VIVSYNC,
            act_readl(HDMI_VIVSYNC));
    printk("HDMI_VIVHSYNC(0x%x) value is 0x%x\n", HDMI_VIVHSYNC,
            act_readl(HDMI_VIVHSYNC));
    printk("HDMI_VIALSEOF(0x%x) value is 0x%x\n", HDMI_VIALSEOF,
            act_readl(HDMI_VIALSEOF));
    printk("HDMI_VIALSEEF(0x%x) value is 0x%x\n", HDMI_VIALSEEF,
            act_readl(HDMI_VIALSEEF));
    printk("HDMI_VIADLSE(0x%x) value is 0x%x\n", HDMI_VIADLSE,
            act_readl(HDMI_VIADLSE));
    printk("HDMI_AIFRAMEC(0x%x) value is 0x%x\n", HDMI_AIFRAMEC,
            act_readl(HDMI_AIFRAMEC));
    printk("HDMI_AICHSTAByte0to3(0x%x) value is 0x%x\n", HDMI_AICHSTABYTE0TO3,
            act_readl(HDMI_AICHSTABYTE0TO3));
    printk("HDMI_AICHSTAByte4to7(0x%x) value is 0x%x\n", HDMI_AICHSTABYTE4TO7,
            act_readl(HDMI_AICHSTABYTE4TO7));
    printk("HDMI_AICHSTAByte8to11(0x%x) value is 0x%x\n", HDMI_AICHSTABYTE8TO11,
            act_readl(HDMI_AICHSTABYTE8TO11));
    printk("HDMI_AICHSTAByte12to15(0x%x) value is 0x%x\n",
            HDMI_AICHSTABYTE12TO15, act_readl(HDMI_AICHSTABYTE12TO15));
    printk("HDMI_AICHSTAByte16to19(0x%x) value is 0x%x\n",
            HDMI_AICHSTABYTE16TO19, act_readl(HDMI_AICHSTABYTE16TO19));
    printk("HDMI_AICHSTAByte20to23(0x%x) value is 0x%x\n",
            HDMI_AICHSTABYTE20TO23, act_readl(HDMI_AICHSTABYTE20TO23));
    printk("HDMI_AICHSTASCN(0x%x) value is 0x%x\n", HDMI_AICHSTASCN,
            act_readl(HDMI_AICHSTASCN));
    printk("HDMI_VR(0x%x) value is 0x%x\n", HDMI_VR, act_readl(HDMI_VR));
    printk("HDMI_CR(0x%x) value is 0x%x\n", HDMI_CR, act_readl(HDMI_CR));
    printk("HDMI_SCHCR(0x%x) value is 0x%x\n", HDMI_SCHCR,
            act_readl(HDMI_SCHCR));
    printk("HDMI_ICR(0x%x) value is 0x%x\n", HDMI_ICR, act_readl(HDMI_ICR));
    printk("HDMI_SCR(0x%x) value is 0x%x\n", HDMI_SCR, act_readl(HDMI_SCR));
    printk("HDMI_LPCR(0x%x) value is 0x%x\n", HDMI_LPCR, act_readl(HDMI_LPCR));
    printk("HDCP_CR(0x%x) value is 0x%x\n", HDCP_CR, act_readl(HDCP_CR));
    printk("HDCP_SR(0x%x) value is 0x%x\n", HDCP_SR, act_readl(HDCP_SR));
    printk("HDCP_ANLR(0x%x) value is 0x%x\n", HDCP_ANLR, act_readl(HDCP_ANLR));
    printk("HDCP_ANMR(0x%x) value is 0x%x\n", HDCP_ANMR, act_readl(HDCP_ANMR));
    printk("HDCP_ANILR(0x%x) value is 0x%x\n", HDCP_ANILR,
            act_readl(HDCP_ANILR));
    printk("HDCP_ANIMR(0x%x) value is 0x%x\n", HDCP_ANIMR,
            act_readl(HDCP_ANIMR));
    printk("HDCP_DPKLR(0x%x) value is 0x%x\n", HDCP_DPKLR,
            act_readl(HDCP_DPKLR));
    printk("HDCP_DPKMR(0x%x) value is 0x%x\n", HDCP_DPKMR,
            act_readl(HDCP_DPKMR));
    printk("HDCP_LIR(0x%x) value is 0x%x\n", HDCP_LIR, act_readl(HDCP_LIR));
    printk("HDCP_SHACR(0x%x) value is 0x%x\n", HDCP_SHACR,
            act_readl(HDCP_SHACR));
    printk("HDCP_SHADR(0x%x) value is 0x%x\n", HDCP_SHADR,
            act_readl(HDCP_SHADR));
    printk("HDCP_ICR(0x%x) value is 0x%x\n", HDCP_ICR, act_readl(HDCP_ICR));
    printk("HDCP_KMMR(0x%x) value is 0x%x\n", HDCP_KMMR, act_readl(HDCP_KMMR));
    printk("HDCP_KMLR(0x%x) value is 0x%x\n", HDCP_KMLR, act_readl(HDCP_KMLR));
    printk("HDCP_MILR(0x%x) value is 0x%x\n", HDCP_MILR, act_readl(HDCP_MILR));
    printk("HDCP_MIMR(0x%x) value is 0x%x\n", HDCP_MIMR, act_readl(HDCP_MIMR));
    printk("HDCP_KOWR(0x%x) value is 0x%x\n", HDCP_KOWR, act_readl(HDCP_KOWR));
    printk("HDCP_OWR(0x%x) value is 0x%x\n", HDCP_OWR, act_readl(HDCP_OWR));

    printk("TMDS_STR0(0x%x) value is 0x%x\n", TMDS_STR0, act_readl(TMDS_STR0));
    printk("TMDS_STR1(0x%x) value is 0x%x\n", TMDS_STR1, act_readl(TMDS_STR1));
    printk("TMDS_EODR0(0x%x) value is 0x%x\n", TMDS_EODR0,
            act_readl(TMDS_EODR0));
    printk("TMDS_EODR1(0x%x) value is 0x%x\n", TMDS_EODR1,
            act_readl(TMDS_EODR1));
    printk("HDMI_ASPCR(0x%x) value is 0x%x\n", HDMI_ASPCR,
            act_readl(HDMI_ASPCR));
    printk("HDMI_ACACR(0x%x) value is 0x%x\n", HDMI_ACACR,
            act_readl(HDMI_ACACR));
    printk("HDMI_ACRPCR(0x%x) value is 0x%x\n", HDMI_ACRPCR,
            act_readl(HDMI_ACRPCR));
    printk("HDMI_ACRPCTSR(0x%x) value is 0x%x\n", HDMI_ACRPCTSR,
            act_readl(HDMI_ACRPCTSR));
    printk("HDMI_ACRPPR(0x%x)value is 0x%x\n", HDMI_ACRPPR,
            act_readl(HDMI_ACRPPR));
    printk("HDMI_GCPCR(0x%x) value is 0x%x\n", HDMI_GCPCR,
            act_readl(HDMI_GCPCR));
    printk("HDMI_RPCR(0x%x) value is 0x%x\n", HDMI_RPCR, act_readl(HDMI_RPCR));
    printk("HDMI_RPRBDR(0x%x) value is 0x%x\n", HDMI_RPRBDR,
            act_readl(HDMI_RPRBDR));
    printk("HDMI_OPCR(0x%x) value is 0x%x\n", HDMI_OPCR, act_readl(HDMI_OPCR));
    printk("HDMI_DIPCCR(0x%x) value is 0x%x\n", HDMI_DIPCCR,
            act_readl(HDMI_DIPCCR));
    printk("HDMI_ORP6PH(0x%x) value is 0x%x\n", HDMI_ORP6PH,
            act_readl(HDMI_ORP6PH));
    printk("HDMI_ORSP6W0(0x%x) value is 0x%x\n", HDMI_ORSP6W0,
            act_readl(HDMI_ORSP6W0));
    printk("HDMI_ORSP6W1(0x%x) value is 0x%x\n", HDMI_ORSP6W1,
            act_readl(HDMI_ORSP6W1));
    printk("HDMI_ORSP6W2(0x%x) value is 0x%x\n", HDMI_ORSP6W2,
            act_readl(HDMI_ORSP6W2));
    printk("HDMI_ORSP6W3(0x%x) value is 0x%x\n", HDMI_ORSP6W3,
            act_readl(HDMI_ORSP6W3));
    printk("HDMI_ORSP6W4(0x%x) value is 0x%x\n", HDMI_ORSP6W4,
            act_readl(HDMI_ORSP6W4));
    printk("HDMI_ORSP6W5(0x%x) value is 0x%x\n", HDMI_ORSP6W5,
            act_readl(HDMI_ORSP6W5));
    printk("HDMI_ORSP6W6v(0x%x) value is 0x%x\n", HDMI_ORSP6W6,
            act_readl(HDMI_ORSP6W6));
    printk("HDMI_ORSP6W7(0x%x) value is 0x%x\n", HDMI_ORSP6W7,
            act_readl(HDMI_ORSP6W7));
    printk("HDMI_CECCR(0x%x) value is 0x%x\n", HDMI_CECCR,
            act_readl(HDMI_CECCR));
    printk("HDMI_CECRTCR(0x%x) value is 0x%x\n", HDMI_CECRTCR,
            act_readl(HDMI_CECRTCR));
    //printk("HDMI_CECRxCR(0x%x) value is 0x%x\n", HDMI_CECRxCR,
    //  act_readl(HDMI_CECRxCR));
    //printk("HDMI_CECTxCR(0x%x) value is 0x%x\n", HDMI_CECTxCR,
    //  act_readl(HDMI_CECTxCR));
    //printk("HDMI_CECTxDR(0x%x) value is 0x%x\n", HDMI_CECTxDR,
    //  act_readl(HDMI_CECTxDR));
    //printk("HDMI_CECRxDR(0x%x) value is 0x%x\n", HDMI_CECRxDR,
    //  act_readl(HDMI_CECRxDR));
    //printk("HDMI_CECRxTCR(0x%x) value is 0x%x\n",HDMI_CECRxTCR,
    //  act_readl(HDMI_CECRxTCR));
    //printk("HDMI_CECTxTCR0(0x%x) value is 0x%x\n",HDMI_CECTxTCR0,
    //  act_readl(HDMI_CECTxTCR0));
    //printk("HDMI_CECTxTCR1(0x%x) value is 0x%x\n",HDMI_CECTxTCR1,
    //  act_readl(HDMI_CECTxTCR1));
    printk("HDMI_CRCCR(0x%x) value is 0x%x\n", HDMI_CRCCR,
            act_readl(HDMI_CRCCR));
    printk("HDMI_CRCDOR(0x%x) value is 0x%x\n", HDMI_CRCDOR,
            act_readl(HDMI_CRCDOR));
    printk("HDMI_TX_1(0x%x) value is 0x%x\n", HDMI_TX_1, act_readl(HDMI_TX_1));
    printk("HDMI_TX_2(0x%x) value is 0x%x\n", HDMI_TX_2, act_readl(HDMI_TX_2));
    printk("CEC_DDC_HPD(0x%x) value is 0x%x\n", CEC_DDC_HPD,
            act_readl(CEC_DDC_HPD));
    printk("hdmi print registers' value finished!\n\n\n");

}

void asoc_520x_hdmi_dump_de(struct hdmi_ip_data *ip_data) {
#if 0
    HDMI_DEBUG("\n\nfollowing list all de register's value!\n");
    HDMI_DEBUG("DE_FCTL(0x%x) value is 0x%x\n",DE_FCTL ,act_readl(DE_FCTL));
    HDMI_DEBUG("DE_SCRCTL(0x%x) value is 0x%x\n", DE_SCRCTL,act_readl(DE_SCRCTL));
    HDMI_DEBUG("DE_BCCTL(0x%x) value is 0x%x\n", DE_BCCTL,act_readl(DE_BCCTL));
    HDMI_DEBUG("DE_PHCTL(0x%x) value is 0x%x\n", DE_PHCTL,act_readl(DE_PHCTL));
    HDMI_DEBUG("DE_BLKEN(0x%x) value is 0x%x\n", DE_BLKEN,act_readl(DE_BLKEN));
    HDMI_DEBUG("DE_OUTFIFO(0x%x) value is 0x%x\n",DE_OUTFIFO, act_readl(DE_OUTFIFO));
    HDMI_DEBUG("DE_LR0_SIZE(0x%x) value is 0x%x\n", DE_LR0_SIZE,act_readl(DE_LR0_SIZE));
    HDMI_DEBUG("DE_LR1_SIZE(0x%x) value is 0x%x\n", DE_LR1_SIZE,act_readl(DE_LR1_SIZE));
    HDMI_DEBUG("DE_LR2_SIZE(0x%x) value is 0x%x\n", DE_LR2_SIZE,act_readl(DE_LR2_SIZE));
    HDMI_DEBUG("DE_LR3_SIZE(0x%x) value is 0x%x\n", DE_LR3_SIZE,act_readl(DE_LR3_SIZE));
    HDMI_DEBUG("DE_LR0_COR(0x%x) value is 0x%x\n", DE_LR0_COR,act_readl(DE_LR0_COR));
    HDMI_DEBUG("DE_LR1_COR(0x%x) value is 0x%x\n", DE_LR1_COR,act_readl(DE_LR1_COR));
    HDMI_DEBUG("DE_LR2_COR(0x%x) value is 0x%x\n",DE_LR2_COR ,act_readl(DE_LR2_COR));
    HDMI_DEBUG("DE_LR3_COR(0x%x) value is 0x%x\n", DE_LR3_COR,act_readl(DE_LR3_COR));
    HDMI_DEBUG("DE_LR0_CFG(0x%x) value is 0x%x\n", DE_LR0_CFG,act_readl(DE_LR0_CFG));
    HDMI_DEBUG("DE_LR1_CFG(0x%x) value is 0x%x\n", DE_LR1_CFG,act_readl(DE_LR1_CFG));
    HDMI_DEBUG("DE_LR2_CFG(0x%x) value is 0x%x\n", DE_LR2_CFG,act_readl(DE_LR2_CFG));
    HDMI_DEBUG("DE_LR3_CFG(0x%x) value is 0x%x\n",DE_LR3_CFG ,act_readl(DE_LR3_CFG));
    HDMI_DEBUG("DE_FB0_G(0x%x) value is 0x%x\n",DE_FB0_G ,act_readl(DE_FB0_G));
    HDMI_DEBUG("DE_FB1_G(0x%x) value is 0x%x\n",DE_FB1_G ,act_readl(DE_FB1_G));
    HDMI_DEBUG("DE_FB2_G(0x%x) value is 0x%x\n",DE_FB2_G ,act_readl(DE_FB2_G));
    HDMI_DEBUG("DE_FBY_V(0x%x) value is 0x%x\n", DE_FBY_V,act_readl(DE_FBY_V));
    HDMI_DEBUG("DE_FBU_V(0x%x) value is 0x%x\n", DE_FBU_V,act_readl(DE_FBU_V));
    HDMI_DEBUG("DE_FBV_V(0x%x) value is 0x%x\n", DE_FBV_V,act_readl(DE_FBV_V));
    HDMI_DEBUG("DE_FBUV_V(0x%x) value is 0x%x\n",DE_FBUV_V ,act_readl(DE_FBUV_V));
    HDMI_DEBUG("DE_FB3_V(0x%x) value is 0x%x\n",DE_FB3_V ,act_readl(DE_FB3_V));
    HDMI_DEBUG("DE_FBYC_V(0x%x) value is 0x%x\n",DE_FBYC_V ,act_readl(DE_FBYC_V));
    HDMI_DEBUG("DE_FBUVC_V(0x%x) value is 0x%x\n",DE_FBUVC_V ,act_readl(DE_FBUVC_V));
    HDMI_DEBUG("DE_FBYLUT_V(0x%x) value is 0x%x\n",DE_FBYLUT_V ,act_readl(DE_FBYLUT_V));
    HDMI_DEBUG("DE_FBUVLUT_V(0x%x) value is 0x%x\n", DE_FBUVLUT_V,act_readl(DE_FBUVLUT_V));
    HDMI_DEBUG("DE_FBLY_V(0x%x) value is 0x%x\n", DE_FBLY_V,act_readl(DE_FBLY_V));
    HDMI_DEBUG("DE_FBLVC_V(0x%x) value is 0x%x\n",DE_FBLVC_V ,act_readl(DE_FBLVC_V));
    HDMI_DEBUG("DE_FBRY_V(0x%x) value is 0x%x\n", DE_FBRY_V,act_readl(DE_FBRY_V));
    HDMI_DEBUG("DE_FBRUV_V(0x%x) value is 0x%x\n", DE_FBRUV_V,act_readl(DE_FBRUV_V));
    HDMI_DEBUG("DE_STR0(0x%x) value is 0x%x\n", DE_STR0,act_readl(DE_STR0));
    HDMI_DEBUG("DE_STR1(0x%x) value is 0x%x\n", DE_STR1,act_readl(DE_STR1));
    HDMI_DEBUG("DE_STR2(0x%x) value is 0x%x\n", DE_STR2,act_readl(DE_STR2));
    HDMI_DEBUG("DE_STR3(0x%x) value is 0x%x\n", DE_STR3,act_readl(DE_STR3));
    HDMI_DEBUG("DE_CKMAX(0x%x) value is 0x%x\n", DE_CKMAX,act_readl(DE_CKMAX));
    HDMI_DEBUG("DE_CKMIN(0x%x) value is 0x%x\n", DE_CKMIN,act_readl(DE_CKMIN));
    HDMI_DEBUG("DE_CKCFG(0x%x) value is 0x%x\n", DE_CKCFG,act_readl(DE_CKCFG));
    HDMI_DEBUG("DE_CURADDR_L0(0x%x) value is 0x%x\n", DE_CURADDR_L0,act_readl(DE_CURADDR_L0));
    HDMI_DEBUG("DE_CURADDR_L1(0x%x) value is 0x%x\n", DE_CURADDR_L1,act_readl(DE_CURADDR_L1));
    HDMI_DEBUG("DE_CURADDR_L2(0x%x) value is 0x%x\n",DE_CURADDR_L2, act_readl(DE_CURADDR_L2));
    HDMI_DEBUG("DE_CURADDR_L3(0x%x) value is 0x%x\n", DE_CURADDR_L3,act_readl(DE_CURADDR_L3));
    HDMI_DEBUG("DE_DITHER(0x%x) value is 0x%x\n", DE_DITHER,act_readl(DE_DITHER));
    HDMI_DEBUG("DE_LR3_SUBSIZE(0x%x) value is 0x%x\n", DE_LR3_SUBSIZE,act_readl(DE_LR3_SUBSIZE));
    HDMI_DEBUG("DE_SCLCOEF0(0x%x) value is 0x%x\n", DE_SCLCOEF0,act_readl(DE_SCLCOEF0));
    HDMI_DEBUG("DE_SCLCOEF1(0x%x) value is 0x%x\n", DE_SCLCOEF1,act_readl(DE_SCLCOEF1));
    HDMI_DEBUG("DE_SCLCOEF2(0x%x) value is 0x%x\n", DE_SCLCOEF2,act_readl(DE_SCLCOEF2));
    HDMI_DEBUG("DE_SCLCOEF3(0x%x) value is 0x%x\n", DE_SCLCOEF3,act_readl(DE_SCLCOEF3));
    HDMI_DEBUG("DE_SCLCOEF4(0x%x) value is 0x%x\n", DE_SCLCOEF4,act_readl(DE_SCLCOEF4));
    HDMI_DEBUG("DE_SCLCOEF5(0x%x) value is 0x%x\n", DE_SCLCOEF5,act_readl(DE_SCLCOEF5));
    HDMI_DEBUG("DE_SCLCOEF6(0x%x) value is 0x%x\n", DE_SCLCOEF6,act_readl(DE_SCLCOEF6));
    HDMI_DEBUG("DE_SCLCOEF7(0x%x) value is 0x%x\n", DE_SCLCOEF7,act_readl(DE_SCLCOEF7));
    HDMI_DEBUG("DE_SCLOFS(0x%x) value is 0x%x\n", DE_SCLOFS,act_readl(DE_SCLOFS));
    HDMI_DEBUG("DE_SCLSR(0x%x) value is 0x%x\n", DE_SCLSR,act_readl(DE_SCLSR));
    HDMI_DEBUG("DE_BRI(0x%x) value is 0x%x\n", DE_BRI,act_readl(DE_BRI));
    HDMI_DEBUG("DE_CON(0x%x) value is 0x%x\n", DE_CON,act_readl(DE_CON));
    HDMI_DEBUG("DE_SAT(0x%x) value is 0x%x\n", DE_SAT,act_readl(DE_SAT));
    HDMI_DEBUG("DE_CRITICAL(0x%x) value is 0x%x\n", DE_CRITICAL,act_readl(DE_CRITICAL));
    HDMI_DEBUG("DE_INTEN(0x%x) value is 0x%x\n", DE_INTEN,act_readl(DE_INTEN));
    HDMI_DEBUG("DE_INTSTA(0x%x) value is 0x%x\n", DE_INTSTA,act_readl(DE_INTSTA));
    HDMI_DEBUG("DE_PRELINE(0x%x) value is 0x%x\n", DE_PRELINE,act_readl(DE_PRELINE));
    HDMI_DEBUG("DE_RAMIDX(0x%x) value is 0x%x\n", DE_RAMIDX,act_readl(DE_RAMIDX));
    HDMI_DEBUG("DE_RAMWIN(0x%x) value is 0x%x\n", DE_RAMWIN,act_readl(DE_RAMWIN));
    HDMI_DEBUG("MDSB_CTL(0x%x) value is 0x%x\n", MDSB_CTL,act_readl(MDSB_CTL));

#endif
}

