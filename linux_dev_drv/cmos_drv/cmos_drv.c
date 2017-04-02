#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define NUM_CMOS_BANKS          2
#define BUFFER_SIZ          (32)
#define CLASS_DEVICE_NAME             "my_cmos"
int cmos_open(struct inode *inode, struct file *file);
int cmos_release(struct inode *inode, struct file *file);
ssize_t cmos_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t cmos_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

/* Per-device (per-bank) structure */
struct cmos_dev {
    unsigned short current_pointer; /* Current pointer within the
                                       bank */
    unsigned int size;              /* Size of the bank */
    int bank_number;                /* CMOS bank number */
    struct cdev cdev;               /* The cdev structure */
    char name[10];                  /* Name of I/O region */
    unsigned char dev_buffer[BUFFER_SIZ];
    /* ... */                       /* Mutexes, spinlocks, wait
                                       queues, .. */
} *cmos_devp[NUM_CMOS_BANKS];

/* File operations structure. Defined in linux/fs.h */
static struct file_operations cmos_fops = {
    .owner    =   THIS_MODULE,      /* Owner */
    .open     =   cmos_open,        /* Open method */
    .release  =   cmos_release,     /* Release method */
    .read     =   cmos_read,        /* Read method */
    .write    =   cmos_write,       /* Write method */
    //.llseek   =   cmos_llseek,      /* Seek method */
    //.ioctl    =   cmos_ioctl,       /* Ioctl method */
};

static dev_t dev_number;   /* Allotted device number */
struct class *cmos_class;       /* Tie with the device model */
#define CMOS_BANK0_INDEX_PORT   0x70
#define CMOS_BANK0_DATA_PORT    0x71
#define CMOS_BANK1_INDEX_PORT   0x72
#define CMOS_BANK1_DATA_PORT    0x73
unsigned char addrports[NUM_CMOS_BANKS] = {CMOS_BANK0_INDEX_PORT,
    CMOS_BANK1_INDEX_PORT,};
unsigned char dataports[NUM_CMOS_BANKS] = {CMOS_BANK0_DATA_PORT,
    CMOS_BANK1_DATA_PORT,};

int cmos_open(struct inode *inode, struct file *file)
{
    struct cmos_dev *devp;
    devp = container_of(inode->i_cdev, struct cmos_dev, cdev);
    file->private_data = devp;
    devp->size = BUFFER_SIZ;
    devp->current_pointer = 0;
    return 0;
}

int cmos_release(struct inode *inode, struct file *file)
{
    struct cmos_dev *devp;
    devp = container_of(inode->i_cdev, struct cmos_dev, cdev);
    devp->current_pointer = 0;
    return 0;
}
ssize_t cmos_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int siz;
    struct cmos_dev *devp;
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
    devp = filp->private_data;
    if (copy_to_user((void *)buf, devp->dev_buffer + *f_pos, siz))
    {
        return -EFAULT;
    }
    *f_pos += siz;

    return siz;
}

ssize_t cmos_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    int siz;
    int rc;
    struct cmos_dev *devp;

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

    devp = filp->private_data;
    rc = copy_from_user(devp->dev_buffer + *f_pos, buf, siz);
    if (rc == 0)
    {
        *f_pos += siz;
        return siz;
    }
    else
    {
        return -EFAULT;
    }
}
/*
 * Driver Initialization
 */
int __init
cmos_init(void)
{
    int i;
    /* Request dynamic allocation of a device major number */
    if (alloc_chrdev_region(&dev_number, 0,
            NUM_CMOS_BANKS, CLASS_DEVICE_NAME) < 0) {
        printk(KERN_DEBUG "Can't register device\n"); return -1;
    }
    /* Populate sysfs entries */
    cmos_class = class_create(THIS_MODULE, CLASS_DEVICE_NAME);
    for (i=0; i<NUM_CMOS_BANKS; i++) {
        /* Allocate memory for the per-device structure */
        cmos_devp[i] = kmalloc(sizeof(struct cmos_dev), GFP_KERNEL);
        if (!cmos_devp[i]) {
            printk("Bad Kmalloc\n"); return 1;
        }
        /* Request I/O region */
        sprintf(cmos_devp[i]->name, "cmos%d", i);

        //if (!(request_region(addrports[i], 2, cmos_devp->name)))
        //{
        //    printk("cmos: I/O port 0x%x is not free.\n", addrports[i]);
        //    return –EIO;
        //}

        /* Fill in the bank number to correlate this device
           with the corresponding CMOS bank */
        cmos_devp[i]->bank_number = i;
        /* Connect the file operations with the cdev */
        cdev_init(&cmos_devp[i]->cdev, &cmos_fops);
        cmos_devp[i]->cdev.owner = THIS_MODULE;
        /* Connect the major/minor number to the cdev */
        if (cdev_add(&cmos_devp[i]->cdev, (dev_number + i), 1)) {
            printk("Bad cdev\n");
            return 1;
        }
        /* Send uevents to udev, so it'll create /dev nodes */
        device_create(cmos_class, NULL, (dev_number + i), NULL, "cmos%d", i);
    }
    printk("CMOS Driver Initialized.\n");
    return 0;
}
/* Driver Exit */
void __exit
cmos_cleanup(void)
{
    int i;
    /* Remove the cdev */
    /* Release the major number */
    unregister_chrdev_region(MAJOR(dev_number), NUM_CMOS_BANKS);
    /* Release I/O region */
    for (i=0; i<NUM_CMOS_BANKS; i++) {
        device_destroy(cmos_class, MKDEV(MAJOR(dev_number), i));
        cdev_del(&cmos_devp[i]->cdev);
        kfree(cmos_devp[i]);
        //release_region(addrports[i], 2);
    }
    /* Destroy cmos_class */
    class_destroy(cmos_class);
    return;
}
module_init(cmos_init);
module_exit(cmos_cleanup);
