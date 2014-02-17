
PWD := $(shell pwd)

ifeq ($(KERNELRELEASE),)
# current directory is driver
CFGDIR = $(PWD)/../../../config

include $(CFGDIR)/build_config.cfg
include $(CFGDIR)/rules.make

endif

EXTRA_CFLAGS +=


obj-m		+= acts_hcd.o

acts_hcd-objs	+= aotg_debug.o
acts_hcd-objs	+= aotg_hcd.o
acts_hcd-objs	+= aotg_dma.o
acts_hcd-objs	+= aotg_dev0.o
acts_hcd-objs	+= aotg_dev1.o



default:
	echo $(PWD)
	$(MAKE) -C $(KERNEL_BUILD_PATH) ARCH=$(R_ARCH) CROSS_COMPILE=$(R_CROSS_COMPILE) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" M=$(PWD) modules
	cp acts_hcd.ko $(DEFAULT_MODULES_DIR)

clean:
	$(MAKE) -C $(KERNEL_BUILD_PATH) ARCH=$(R_ARCH) CROSS_COMPILE=$(R_CROSS_COMPILE) M=$(PWD) clean
