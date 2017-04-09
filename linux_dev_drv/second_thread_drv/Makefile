obj-y:=second_thread_drv.o
PWD:=$(shell pwd)
KDIR:=$(KERNEL_PATH)
EXTRA_CFLAGS=-O0 -g -ggdb
all:
	$(MAKE) -C $(KDIR) M=$(PWD)

clean:
	@-$(MAKE) -C $(APPDIR) clean
	@rm -rf *.ko *.order *.symvers *.cmd *.o *.mod.c *.tmp_versions .*.cmd .tmp_versions
