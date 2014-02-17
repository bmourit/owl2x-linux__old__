PWD := $(shell pwd)

ifeq ($(KERNELRELEASE),)
# current directory is driver
CFGDIR = $(PWD)/../../../config

include $(CFGDIR)/build_config.cfg
include $(CFGDIR)/rules.make

endif

EXTRA_CFLAGS +=


obj-m				+= dwc3.o
obj-m				+= dwc3-actions.o

dwc3-objs			:= core.o host.o gadget.o ep0.o debugfs.o

all:
	$(MAKE) -C $(KERNEL_BUILD_PATH) ARCH=$(R_ARCH) CROSS_COMPILE=$(R_CROSS_COMPILE) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" M=$(PWD) modules
	cp *.ko $(DEFAULT_MODULES_DIR)

clean:
	$(MAKE) -C $(KERNEL_BUILD_PATH) ARCH=$(R_ARCH) CROSS_COMPILE=$(R_CROSS_COMPILE) \
		M=$(PWD) clean
