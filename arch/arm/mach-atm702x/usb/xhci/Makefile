PWD := $(shell pwd)

ifeq ($(KERNELRELEASE),)
# current directory is driver
CFGDIR = $(PWD)/../../../config

include $(CFGDIR)/build_config.cfg
include $(CFGDIR)/rules.make

endif

EXTRA_CFLAGS += -DCONFIG_USB_XHCI_PLATFORM=y
#EXTRA_CFLAGS += -DDEBUG -DCONFIG_USB_XHCI_PLATFORM=y -DCONFIG_USB_XHCI_HCD_DEBUGGING=y


obj-m				+= xhci-hcd.o

xhci-hcd-objs			:= xhci.o xhci-mem.o xhci-ring.o xhci-hub.o xhci-dbg.o xhci-plat.o

all:
	$(MAKE) -C $(KERNEL_BUILD_PATH) ARCH=$(R_ARCH) CROSS_COMPILE=$(R_CROSS_COMPILE) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" M=$(PWD) modules
	cp *.ko $(DEFAULT_MODULES_DIR)

clean:
	$(MAKE) -C $(KERNEL_BUILD_PATH) ARCH=$(R_ARCH) CROSS_COMPILE=$(R_CROSS_COMPILE) \
		M=$(PWD) clean
