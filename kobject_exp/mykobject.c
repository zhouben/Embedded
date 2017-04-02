/**
 * Demonstrate how to create kobject and attribute files as well in /sys/
 * insmod mykobject.ko
 * dmesg -c
 * cat /sys/kobject_test/kobj_setting
 * cat /sys/kobject_test/kobj_config
 * echo "hello world!" > /sys/kobject_test/kobj_config
 * echo "hello world!" > /sys/kobject_test/kobj_setting
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/stat.h>

MODULE_AUTHOR("David Xie");
MODULE_LICENSE("Dual BSD/GPL");

/*声明release、show、store函数*/
void obj_test_release(struct kobject *kobject);
ssize_t kobj_test_show(struct kobject *kobject, struct attribute *attr,char *buf);
ssize_t kobj_test_store(struct kobject *kobject,struct attribute *attr,const char *buf, size_t count);

/*对应于kobject的目录下的一个文件,Name成员就是文件名*/
struct attribute test_attr = {
    .name = "kobj_config",
    .mode = S_IRWXUGO, };

struct attribute setting_attr = {
    .name = "kobj_setting",
    .mode = S_IRWXUGO, };

static struct attribute *def_attrs[] = {
    &test_attr,
    &setting_attr,
    NULL,
};

//kobject对象的操作
struct sysfs_ops obj_test_sysops = {
    .show = kobj_test_show,
    .store = kobj_test_store,
};

/*定义kobject对象的一些属性及对应的操作*/
struct kobj_type ktype =  {
    .release = obj_test_release,
    .sysfs_ops=&obj_test_sysops,
    .default_attrs=def_attrs,
};
/*release方法释放该kobject对象*/
void obj_test_release(struct kobject *kobject)
{
    printk("eric_test: release .\n");
}

/*当读文件时执行的操作*/
ssize_t kobj_test_show(struct kobject *kobject, struct attribute *attr,char *buf)
{
    printk("have show.\n");
    printk("attrname:%s.\n", attr->name);
    sprintf(buf,"%s\n",attr->name);
    return strlen(attr->name)+2;
}

/*当写文件时执行的操作*/
ssize_t kobj_test_store(struct kobject *kobject,struct attribute *attr,const char *buf, size_t count)
{
    printk("havestore\n");
    printk("write: %s\n",buf);
    return count;
}

struct kobject kobj;//声明kobject对象

static int kobj_test_init(void)
{
    printk("kboject test init.\n");
    kobject_init_and_add(&kobj,&ktype,NULL,"kobject_test");//初始化kobject对象kobj,并将其注册到linux系统
    return 0;
}

static void kobj_test_exit(void)
{
    printk("kobject test exit.\n");
    kobject_del(&kobj);
}

module_init(kobj_test_init);

module_exit(kobj_test_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhuguojun, ZhiYuan Electronics Co, Ltd.");
MODULE_DESCRIPTION("GPIO DRIVER FOR EasyARM-i.MX28xx");
