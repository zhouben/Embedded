目的：

1. 测试应用程序通过 block和nonblock两种方式访问设备情况。参考LDD3 的 scull example
2. 演示如何实现主线程调用pthread_cancel去cancel一个正在 blocking read的 thread 的情况。
driver在 read callback中调用 wait_event_interruptible进入wait状态，此时其他线程cancel该线程。可以成功cancel掉该blocking的线程。


使用方法：

将rw_wake_drv.ko和led_test/read_led拷贝到开发套件上后，首先装载led.ko驱动：
	insmod rw_wake_drv.ko

驱动加载完毕后，运行led_test即可：
    ./led_test -> for point 1
    ./read_led -> for point 2
