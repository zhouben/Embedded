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
#include <linux/mutex.h>

#define DEVICE_NAME	"rw_wake"
#define BUFFER_SIZ 32
unsigned char data[BUFFER_SIZ] = {0};

struct led_ext
{
    struct mutex sem;
    wait_queue_head_t inq;
    int data_ready_flag;
    struct miscdevice gpio_miscdev;
};

/*--------------------------------------------------------------------------------------------------------
*/
static int gpio_open(struct inode *inode, struct file *filp);
static int  gpio_release(struct inode *inode, struct file *filp);
ssize_t gpio_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t gpio_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
//int gpio_ioctl(struct inode *inode, struct file *flip, unsigned int command, unsigned long arg);
static int gpio_init(void);
static void gpio_exit(void);

static struct file_operations gpio_fops={
	.owner		= THIS_MODULE,
	.open 		= gpio_open,
	.write		= gpio_write,
    .read       = gpio_read,
	.release	= gpio_release,
	//.ioctl		= gpio_ioctl,
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

static int data_ready(void)
{
    if (my_led.data_ready_flag == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

ssize_t gpio_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int siz;
    printk("gpio_read up\n");
    printk("data_ready_flag %d, not ready, need to wait...\n", my_led.data_ready_flag);

    if (count == 0)
        return 0;

    if (*f_pos >= BUFFER_SIZ)
        return -EINVAL;
    if (*f_pos + count > BUFFER_SIZ)
    {
        siz = BUFFER_SIZ - *f_pos;
    }
    else
    {
        siz = count;
    }

    if (mutex_lock_interruptible(&my_led.sem))
        return -ERESTARTSYS;

    while (my_led.data_ready_flag == 0)
    {
        mutex_unlock(&my_led.sem);

        if (wait_event_interruptible(my_led.inq, (data_ready() > 0)))
            return -ERESTARTSYS;
        if (mutex_lock_interruptible(&my_led.sem))
            return -ERESTARTSYS;
    }
    printk("data_ready_flag %d, ready to read\n", my_led.data_ready_flag);

    my_led.data_ready_flag--;
    mutex_unlock (&my_led.sem);
    if (0 != copy_to_user((void *)buf, data + *f_pos, siz))
    {
        return -EFAULT;
    }
    *f_pos += siz;

    return siz;
}

ssize_t gpio_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    int rc;
    int siz;
    printk("gpio_write up\n");
    if (count == 0)
        return 0;

    if (*f_pos >= BUFFER_SIZ)
        return -EINVAL;
    if (*f_pos + count > BUFFER_SIZ)
    {
        siz = BUFFER_SIZ - *f_pos;
    }
    else
    {
        siz = count;
    }

    if (mutex_lock_interruptible(&my_led.sem))
        return -ERESTARTSYS;
    my_led.data_ready_flag++;
    mutex_unlock(&my_led.sem);

    rc = copy_from_user(data + *f_pos, buf, siz);

    printk("write: f_flags: %08X\n", filp->f_flags);

	if (0 == rc)
    {
        *f_pos += siz;
        wake_up_interruptible(&my_led.inq);
        return siz;
    }
    else
    {
        return -EFAULT;
    }
}

#if 0
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
#endif

static int __init gpio_init(void)
{
	misc_register(&my_led.gpio_miscdev);
    init_waitqueue_head(&(my_led.inq));
    mutex_init(&(my_led.sem));
	printk(DEVICE_NAME" up. \n"); 
    printk("kobject->name        %s\n", my_led.gpio_miscdev.this_device->kobj.name);

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


