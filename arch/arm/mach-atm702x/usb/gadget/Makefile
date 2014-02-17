PWD := $(shell pwd)

ifeq ($(KERNELRELEASE),)
# current directory is driver
CFGDIR = $(PWD)/../../../config

include $(CFGDIR)/build_config.cfg
include $(CFGDIR)/rules.make

endif

EXTRA_CFLAGS += -DCONFIG_USB_GADGET_DUALSPEED  -DCONFIG_USB_GADGET_STORAGE_NUM_BUFFERS=2


obj-m				+= g_android.o
obj-m				+= g_mass_storage.o
#obj-m				+= g_webcam.ko
obj-m				+= stub.o
obj-m				+= remotefs_inode.o
obj-m       += dummy_udc.o

g_android-objs			:= android.o
g_mass_storage-objs		:= mass_storage.o
g_webcam-objs			:= webcam.o

all:
	$(MAKE) -C $(KERNEL_BUILD_PATH) ARCH=$(R_ARCH) CROSS_COMPILE=$(R_CROSS_COMPILE) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" M=$(PWD) modules
	cp *.ko $(DEFAULT_MODULES_DIR)

clean:
	$(MAKE) -C $(KERNEL_BUILD_PATH) ARCH=$(R_ARCH) CROSS_COMPILE=$(R_CROSS_COMPILE) \
		M=$(PWD) clean
