obj-m:=mykobject.o
PWD:=$(shell pwd)
KDIR:=$(KERNEL_PATH)
all:
	$(MAKE) -C $(KDIR) M=$(PWD) 
	cp *.ko $(NFSROOT)
clean:
	rm -rf *.ko *.order *.symvers *.cmd *.o *.mod.c *.tmp_versions .*.cmd .tmp_versions
