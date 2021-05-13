ifneq ($(KERNELRELEASE),)
obj-m := irq_timestamp.o
else
K_DIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(K_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(K_DIR) M=$(PWD) clean

endif


