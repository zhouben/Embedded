obj-m:=led.o
APPDIR = test
PWD:=$(shell pwd)
KDIR:=$(KERNEL_PATH)
all:
	$(MAKE) -C $(KDIR) M=$(PWD) 
	$(MAKE) -C $(APPDIR)
	cp *.ko $(NFSROOT)
clean:
	$(MAKE) -C $(APPDIR) clean
	rm -rf *.ko *.order *.symvers *.cmd *.o *.mod.c *.tmp_versions .*.cmd .tmp_versions
