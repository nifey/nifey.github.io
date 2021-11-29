#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/sysfs.h>
#include<linux/slab.h>
#include<linux/rwsem.h>
MODULE_LICENSE("GPL");

static char *echo_buf;
static DECLARE_RWSEM(echo_rwlock);
static struct kobject *echo_kobj;

static ssize_t echo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	down_read(&echo_rwlock);
	strncpy(buf, echo_buf, PAGE_SIZE);
	up_read(&echo_rwlock);
	return PAGE_SIZE;
}

static ssize_t echo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	int len;
	down_write(&echo_rwlock);
	memset(echo_buf, 0, PAGE_SIZE);
	len = (count > PAGE_SIZE)? PAGE_SIZE: count;
	strncpy(echo_buf, buf, len);
	up_write(&echo_rwlock);
	return len;
}

// echo file Permissions : Allow write from root and reads from anyone : 0644
static struct kobj_attribute echo_attribute =
	__ATTR(echo, 0644, echo_show, echo_store);

// Create attribute group
static struct attribute *attrs[] = {
	&echo_attribute.attr,
	NULL,
};
static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init echo_init(void)
{
	int retval;

	// Create "echo" kobject
	echo_kobj = kobject_create_and_add("echo", kernel_kobj);
	if (!echo_kobj)
		return -ENOMEM;

	// Allocate space for echo_buf
	echo_buf = (char*) kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!echo_buf) {
		printk(KERN_ERR "echo: Cannot allocate memory for echo file\n");
		kobject_put(echo_kobj);
		return -ENOMEM;
	}

	retval = sysfs_create_group(echo_kobj, &attr_group);
	if (retval) {
		printk(KERN_ERR "echo: Cannot register sysfs attribute group\n");
		kfree(echo_buf);
		kobject_put(echo_kobj);
	}

	return 0;
}

static void __exit echo_exit(void)
{
	// Deallocate buffer
	kfree(echo_buf);
	// Remove kobject
	kobject_put(echo_kobj);
}

module_init(echo_init);
module_exit(echo_exit);
