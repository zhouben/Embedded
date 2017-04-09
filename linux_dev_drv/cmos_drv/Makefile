obj-y:=cmos_drv.o
PWD:=$(shell pwd)
KDIR:=$(KERNEL_PATH)
EXTRA_CFLAGS=-O0 -g -ggdb
all:
	$(MAKE) -C $(KDIR) M=$(PWD)
	sudo cp *.ko /opt/work/busybox-1.26.2/rootfs/tmp/
clean:
	@-$(MAKE) -C $(APPDIR) clean
	@rm -rf *.ko *.order *.symvers *.cmd *.o *.mod.c *.tmp_versions .*.cmd .tmp_versions
