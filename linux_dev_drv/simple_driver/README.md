目的：
简单的linux驱动框架程序。需要在 kernel source tree中编译。

使用方法：

编译后的驱动会整合在vmlinux中。
可以通过
echo "hello world" > /dev/simple_drv 实现写操作
od -N 16 -j 4 /dev/simple_drv 实现跳过前4个字节，读取后面的16 bytes
