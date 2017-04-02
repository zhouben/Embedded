1. qemu\_script    scripts for qemu, like make busybox image file, initiate qemu with so many arguments
1. simple\_driver  simple driver framework (misc), support read/write with offset
1. cmos\_drv       demo creating class (/sys/class/my\_cmos), character (/sys/devices/virtual/cmos/cmos[01])
1. second\_thread\_drv misc device, demo how to create a second thread and debug it. Write operation can wakeup the thread.
1. rw\_wake\_drv   block process reading/writing until wake. cancel blocking thread.
                   Application cancel a blocking read thread successfully.
