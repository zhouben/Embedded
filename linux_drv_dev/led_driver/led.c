/*
   GPIO Driver driver for EasyARM-iMX283
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/bcd.h>
#include <linux/capability.h>
#include <linux/rtc.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include <../arch/arm/mach-mx28/mx28_pins.h>

#define DEVICE_NAME	"imx28x_led"

struct led_ext
{
    struct semaphore sem;
    wait_queue_head_t inq;
    int data_ready_flag;
    struct miscdevice gpio_miscdev;
};

/*--------------------------------------------------------------------------------------------------------
*/
static int gpio_open(struct inode *inode, struct file *filp);
static int  gpio_release(struct inode *inode, struct file *filp);
ssize_t gpio_read(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
ssize_t gpio_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos);
static int gpio_ioctl(struct inode *inode,struct file *flip,unsigned int command,unsigned long arg);
static int gpio_init(void);
static void gpio_exit(void);

static struct file_operations gpio_fops={
	.owner		= THIS_MODULE,
	.open 		= gpio_open,
	.write		= gpio_write,
    .read       = gpio_read,
	.release	= gpio_release,
	.ioctl		= gpio_ioctl,
};

struct led_ext my_led = {
    .data_ready_flag = 0,
    .gpio_miscdev.minor  = MISC_DYNAMIC_MINOR,
    .gpio_miscdev.name	= DEVICE_NAME,
    .gpio_miscdev.fops	= &gpio_fops,
};

/*--------------------------------------------------------------------------------------------------------
*/
static int gpio_open(struct inode *inode, struct file *filp)
{
    printk("open: f_flags: %08X\n", filp->f_flags);

	return 0;
}

static int  gpio_release(struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t gpio_read(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    printk("gpio_read up\n");
    if (down_interruptible(&my_led.sem))
        return -ERESTARTSYS;

    while (my_led.data_ready_flag == 0)
    {
        up(&my_led.sem);
        printk("data_ready_flag %d, not ready, need to wait...\n", my_led.data_ready_flag);
        if (wait_event_interruptible(my_led.inq, (my_led.data_ready_flag > 0)))
            return -ERESTARTSYS;
        if (down_interruptible(&my_led.sem))
            return -ERESTARTSYS;
    }
    printk("data_ready_flag %d, ready to read\n", my_led.data_ready_flag);
    my_led.data_ready_flag--;
    up (&my_led.sem);
    return count;
}

ssize_t gpio_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
	char data[2];
    printk("gpio_write up\n");
    if (count == 0)
        return 0;

    if (down_interruptible(&my_led.sem))
        return -ERESTARTSYS;
    my_led.data_ready_flag++;
    up(&my_led.sem);
    wake_up_interruptible(&my_led.inq);

	printk("[input] write buf %08X, [stack] data %08X\n", (unsigned int)buf, (unsigned int)data);
    printk("write: f_flags: %08X\n", filp->f_flags);

	copy_from_user(data, buf, 1);

	return 1;
}

static int gpio_ioctl(struct inode *inode,struct file *flip,unsigned int command,unsigned long arg)
{
	switch (command) {
	case 0: 
        printk("Write 0 for LED_GPIO\n");
		break;
	
	case 1:
        printk("Write 1 for LED_GPIO\n");
		break;
	}

	return 0;
}

static int __init gpio_init(void)
{
	misc_register(&my_led.gpio_miscdev);
    init_waitqueue_head(&(my_led.inq));
    init_MUTEX(&(my_led.sem));
	printk(DEVICE_NAME" up. \n"); 
    printk("kobject->name        %s\n", my_led.gpio_miscdev.this_device->kobj.name);
    //printk("parent kobject->name %s\n", my_led.gpio_miscdev.parent->kobj.name);
	printk("Function gpio_init  %08X\n", (unsigned int)gpio_init);
	printk("Function gpio_write %08X\n", (unsigned int)gpio_write);

	return 0;
}

static void __exit gpio_exit(void)
{
	misc_deregister(&my_led.gpio_miscdev);
	printk(DEVICE_NAME " down.\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhuguojun, ZhiYuan Electronics Co, Ltd.");
MODULE_DESCRIPTION("GPIO DRIVER FOR EasyARM-i.MX28xx");


