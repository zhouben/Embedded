This experiment demonstrates how to create kobject and corresponding attribute files in /sys

Steps:
On host:
1. make and copy mykobject.ko to target device.

On target:
1. insmod mykobject.ko
2. dmesg -c
3. cat /sys/kobject_test/kobj_setting
4. cat /sys/kobject_test/kobj_config
5. echo "hello world!" > /sys/kobject_test/kobj_config
6. echo "hello world!" > /sys/kobject_test/kobj_setting
7. dmesg to see the log
