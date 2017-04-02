目的：
测试应用程序通过 block和nonblock两种方式访问设备情况。
参考LDD3 的 scull example

使用方法：

将led.ko和led_test拷贝到开发套件上后，首先装载led.ko驱动：
	insmod led.ko

驱动加载完毕后，运行led_test即可：
	./led_test
