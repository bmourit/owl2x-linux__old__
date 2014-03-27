#ifndef __XML_PINCTRL_H___
#define __XML_PINCTRL_H___

#include "../../pinctrl-asoc.h"

#define GPIO_CFG_MAX 32

typedef struct pcfg_s
{
    char name[32];
    struct file *fd;
    void *tree;
	unsigned int tree_size;
    void *xml_file_buf;
    struct pcfg_s *next;
}pcfg_t;

#define MAX_PINMUX_GROUP_PINS 32
#define MAX_PINMUX_GROUP_REGCFGS 4
#define MAX_ASOC_PINMUX_GROUPS 64
#define MAX_ASOC_PINMUX_FUNCS 64

struct pinctrl_map_data {
	char dev_name[32];
	char name[32];
	char ctrl_dev_name[32];
	char group[32];
	char function[32];
};

typedef int (*handle_element_attr_func)(const char *element_attr);
typedef struct
{
    char name[16];
    handle_element_attr_func func;
}handle_element_attr;


struct asoc_pinmux_group_data {
	char name[32];
	unsigned int pins[MAX_PINMUX_GROUP_PINS];
    struct asoc_regcfg regcfgs[MAX_PINMUX_GROUP_REGCFGS];
};

struct asoc_pinmux_name {
	char name[32];
	char *groups[8];
};

struct pinctrl_args
{
	unsigned int groups_i;
	unsigned int mappings_i;
	unsigned int fuctions_i;

	unsigned int addr_asoc_pinmux_group_datas;
	unsigned int addr_asoc_pinmux_groups;
	unsigned int addr_asoc_pinmux_func_names;
	unsigned int addr_asoc_pinmux_func_groups;
	unsigned int addr_asoc_pinmux_funcs;
	unsigned int addr_pinctrl_map_datas;
	unsigned int addr_pinctrl_maps;		

	unsigned int gpios_i;
	unsigned int addr_gpio_cfgs;
};

extern int paser_config(pcfg_t *p_pcfg);
extern int paser_pinctrls(pcfg_t *p_pcfg);
extern int paser_gpiocfgs(pcfg_t *p_pcfg);
#endif
