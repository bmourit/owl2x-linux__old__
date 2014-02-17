#include <linux/init.h>
#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

static const unsigned int virtual_keymap[]=
{
    KEY_HOME,       KEY_ESC,        KEY_MENU,       KEY_POWER,
    KEY_LEFT,       KEY_RIGHT,      KEY_UP,         KEY_DOWN,
    KEY_ENTER,      KEY_VOLUMEUP,   KEY_VOLUMEDOWN,
};

struct virtual_keyboard {
	struct input_dev *input_dev;

	unsigned int *keymap;
	unsigned int keymap_size;
};

/*
 * read or write a atc260x register from sysfs
 *
 * example:
 *   read reg 0x101: echo 0x101 > /sys/...
 *   write 0x55aa to reg 0x101: echo 0x101=0x55aa > /sys/...
 *
 * note: only used for debug
 */
static ssize_t store_scancode(struct device *dev, struct device_attribute *attr,
                                 const char *buf, size_t count)
{
	struct input_dev *input_dev = to_input_dev(dev);
	struct virtual_keyboard *vkb = input_get_drvdata(input_dev);
	unsigned int i, scancode;
    char *end_ptr;

    pr_info("[virtal key] %s\n", __FUNCTION__);

    i = simple_strtoul(buf, &end_ptr, 16);
	if (buf == end_ptr)
        goto out;

    if (i >= vkb->keymap_size) {
        pr_err("[virtal key] invalid key index: %d\n", i);
		goto out;
    }

    scancode = vkb->keymap[i];
    pr_info("[virtal key] index %d, scancode %d\n", i, scancode);

    input_report_key(input_dev, scancode, 1);
    input_sync(input_dev);
    msleep(100);
    input_report_key(input_dev, scancode, 0);
    input_sync(input_dev);
    pr_info("[virtal key] release scancode %d\n", scancode);

out:
	return count;
}

static ssize_t show_scancode(struct device *dev,
                                struct device_attribute *attr, char *buf)
{
	struct input_dev *input_dev = to_input_dev(dev);
	struct virtual_keyboard *vkb = input_get_drvdata(input_dev);
	unsigned int i, len;

    len = sprintf(buf, "[virtal key] keymap:\n");
    len += sprintf(buf + len, "  index    scancode\n");

    for (i = 0; i < vkb->keymap_size; i++)
        len += sprintf(buf + len, "  [%02d]    %d\n", i, vkb->keymap[i]);
    
    return len;
}

static struct device_attribute virtual_keyboard_attrs[] = {
    __ATTR(scancode, 0644, show_scancode, store_scancode),
};

/* create sysfs register operation interface */
static int virtual_keyboard_create_attr(struct device *dev)
{
    int i, ret;

    for (i = 0; i < ARRAY_SIZE(virtual_keyboard_attrs); i++) {
        ret = device_create_file(dev, &virtual_keyboard_attrs[i]);
        if (ret)
            return ret;
    }

    return 0;
}

static void virtual_keyboard_remove_attr(struct device *dev)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(virtual_keyboard_attrs); i++) {    
        device_remove_file(dev, &virtual_keyboard_attrs[i]);
    }
}

static int __devinit virtual_keyboard_probe(struct platform_device *pdev)
{
	struct virtual_keyboard *vkb;
	struct input_dev *input_dev;
	int ret = 0;
	int i;
	
    printk("[Virtual KEY] key_count: %x\n", ARRAY_SIZE(virtual_keymap));

	vkb = kzalloc(sizeof(struct virtual_keyboard), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!vkb || !input_dev) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		ret = -ENOMEM;
		goto failed_free;
	}

	vkb->keymap = virtual_keymap;
	vkb->keymap_size = ARRAY_SIZE(virtual_keymap);
	vkb->input_dev = input_dev;

	input_dev->name = pdev->name;
	input_dev->phys = "virtual_keyboard/input0";
	input_dev->id.bustype = BUS_HOST;
	input_dev->dev.parent = &pdev->dev;
	input_dev->keycode = vkb->keymap;
	input_dev->keycodesize = vkb->keymap_size;
	input_dev->keycodemax = vkb->keymap_size;
	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_REP);

    input_set_drvdata(input_dev, vkb);

	for (i = 0; i < input_dev->keycodemax; i++)
        __set_bit(vkb->keymap[i], input_dev->keybit);
	__clear_bit(KEY_RESERVED, input_dev->keybit);

	/* Register the input device */
	ret = input_register_device(input_dev);
	if (ret) {
		dev_err(&pdev->dev, "failed to register input device\n");
		goto failed_free;
	}

    ret = virtual_keyboard_create_attr(&input_dev->dev);
    if (ret)
        goto failed_free;

    platform_set_drvdata(pdev, vkb);

	return 0;

failed_free:
	input_free_device(input_dev);
	kfree(vkb);
		
	return ret;
}

static __devexit int virtual_keyboard_remove(struct platform_device *pdev)
{
	struct virtual_keyboard *vkb = platform_get_drvdata(pdev);

	input_unregister_device(vkb->input_dev);
	input_free_device(vkb->input_dev);
    virtual_keyboard_remove_attr(&pdev->dev);
	platform_set_drvdata(pdev, NULL);

    return 0;
}

static struct platform_driver virtual_keyboard_driver = {
    .probe      = virtual_keyboard_probe,
    .remove     = __devexit_p(virtual_keyboard_remove),
    .driver     = {
        .name = "virtual-keyboard",
    },
};

static struct platform_device virtual_keyboard_device = {
	.name           = "virtual-keyboard",
};

static int virtual_keyboard_init(void)
{
    pr_info("%s\n", __FUNCTION__);

    platform_device_register(&virtual_keyboard_device);
    platform_driver_register(&virtual_keyboard_driver);

	return 0;
}

static void virtual_keyboard_exit(void)
{
    pr_info("%s\n", __FUNCTION__);
}

module_init(virtual_keyboard_init);
module_exit(virtual_keyboard_exit);

MODULE_AUTHOR("Actions Semi Inc");
MODULE_DESCRIPTION("virtual keyboard");
MODULE_LICENSE("GPL");