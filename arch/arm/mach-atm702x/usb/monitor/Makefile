PWD := $(shell pwd)

ifeq ($(KERNELRELEASE),)
# current directory is driver
CFGDIR = $(PWD)/../../../config

include $(CFGDIR)/build_config.cfg
include $(CFGDIR)/rules.make

ifeq ($(R_BOARD),ATM7029_DEMO)
EXTRA_CFLAGS += -DATM7029_DEMO
else
EXTRA_CFLAGS += -DATM7029_EVB
endif

endif

EXTRA_CFLAGS += -DDEBUG


OBJ_FILE = umonitor_core.o umonitor_hal.o umonitor_api.o

obj-m				+= monitor.o

monitor-objs := $(OBJ_FILE)

all:
	$(MAKE) -C $(KERNEL_BUILD_PATH) ARCH=$(R_ARCH) CROSS_COMPILE=$(R_CROSS_COMPILE) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" M=$(PWD) modules
	cp *.ko $(DEFAULT_MODULES_DIR)

clean:
	$(MAKE) -C $(KERNEL_BUILD_PATH) ARCH=$(R_ARCH) CROSS_COMPILE=$(R_CROSS_COMPILE) \
		M=$(PWD) clean
