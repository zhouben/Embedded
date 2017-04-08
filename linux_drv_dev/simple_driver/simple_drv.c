/*
 * Simple driver framework
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

#define DEVICE_NAME	"simple_drv"

#define BUFFER_SIZ 32
unsigned char dev_buffer[BUFFER_SIZ] = {0};

struct simple_drv_ext
{
    int write_count;
    int read_count;
    struct miscdevice dev_miscdev;
};

/*--------------------------------------------------------------------------------------------------------
*/
static int simple_drv_open(struct inode *inode, struct file *filp);
static int  simple_drv_release(struct inode *inode, struct file *filp);
ssize_t simple_drv_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t simple_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
int simple_drv_ioctl(struct inode *inode,struct file *flip,unsigned int command,unsigned long arg);
static int simple_drv_init(void);
static void simple_drv_exit(void);

static struct file_operations simple_drv_fops={
	.owner		= THIS_MODULE,
	.open 		= simple_drv_open,
	.write		= simple_drv_write,
    .read       = simple_drv_read,
	.release	= simple_drv_release,
	//.ioctl		= simple_drv_ioctl,
};

struct simple_drv_ext my_dev_ext = {
    .write_count = 0,
    .read_count = 0,
    .dev_miscdev.minor  = MISC_DYNAMIC_MINOR,
    .dev_miscdev.name	= DEVICE_NAME,
    .dev_miscdev.fops	= &simple_drv_fops,
};

/*--------------------------------------------------------------------------------------------------------
*/
static int simple_drv_open(struct inode *inode, struct file *filp)
{
    printk("open %08X: f_flags: %08X\n", (unsigned int)simple_drv_open, filp->f_flags);
	return 0;
}

static int  simple_drv_release(struct inode *inode, struct file *filp)
{
    printk("release %08X: \n", (unsigned int)simple_drv_release);
	return 0;
}

ssize_t simple_drv_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int siz;
    my_dev_ext.read_count++;
    printk("simple_drv_read %08X up, read count %d\n", (unsigned int)simple_drv_read, my_dev_ext.read_count);

    siz = count < BUFFER_SIZ ? count : BUFFER_SIZ;
    if (copy_to_user((void *)buf, dev_buffer, siz))
    {
        return -EFAULT;
    }

    return siz;
}

ssize_t simple_drv_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    int siz;
    int rc;
    my_dev_ext.write_count++;
    printk("simple_drv_write up %08X, write count %d\n", (unsigned int)simple_drv_write, my_dev_ext.write_count);
    if (count == 0)
        return 0;

    siz = (BUFFER_SIZ < count) ? BUFFER_SIZ : count;
    rc = copy_from_user(dev_buffer, buf, siz);
    if (rc == 0)
    {
        return siz;
    }
    else
    {
        return -EFAULT;
    }
}

int simple_drv_ioctl(struct inode *inode,struct file *flip,unsigned int command,unsigned long arg)
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

static int __init simple_drv_init(void)
{
	misc_register(&my_dev_ext.dev_miscdev);
	printk(DEVICE_NAME" up. \n"); 
    printk("init %08X\n", (unsigned int)simple_drv_init);
    printk("kobject->name        %s\n", my_dev_ext.dev_miscdev.this_device->kobj.name);
    //printk("parent kobject->name %s\n", my_dev_ext.dev_miscdev.parent->kobj.name);

	return 0;
}

static void __exit simple_drv_exit(void)
{
    printk("exit %08X\n", (unsigned int)simple_drv_exit);
	misc_deregister(&my_dev_ext.dev_miscdev);
	printk(DEVICE_NAME " down.\n");
}

module_init(simple_drv_init);
module_exit(simple_drv_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhuguojun, ZhiYuan Electronics Co, Ltd.");
MODULE_DESCRIPTION("GPIO DRIVER FOR EasyARM-i.MX28xx");


