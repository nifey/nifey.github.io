#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>

#include<linux/fs.h>            // For struct file_operations
#include<linux/miscdevice.h>    // For struct misc device and register functions
#include<linux/uaccess.h>       // For permissions macros
#include<linux/slab.h>          // For kzalloc
#include<linux/string.h>        // For string helper functions
#include<linux/rwsem.h>         // For reader writers lock

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nihaal");
MODULE_DESCRIPTION("Misc char driver");

static char *kernel_buffer;
static DECLARE_RWSEM(echo_rwlock);

ssize_t echo_read (struct file *filp, char __user * user_buffer, size_t size, loff_t * file_pos) {
	size_t len, ret;

	if ((*file_pos) >= PAGE_SIZE) {
		return 0;
	}

	down_read(&echo_rwlock);
	len = (size > PAGE_SIZE)? PAGE_SIZE: size;
	ret = copy_to_user(user_buffer, kernel_buffer, len);
	len = len - ret;
	*file_pos += len;
	up_read(&echo_rwlock);
	return len;
}

ssize_t echo_write (struct file *filp, const char __user * user_buffer, size_t size, loff_t * file_pos) {
	size_t len, ret;
	if ((*file_pos) >= PAGE_SIZE) {
		return -ENOMEM;
	}

	down_write(&echo_rwlock);
	memset(kernel_buffer, 0, PAGE_SIZE);
	len = (size > PAGE_SIZE)? PAGE_SIZE: size;
	ret = copy_from_user(kernel_buffer, user_buffer, len);
	len = len - ret;
	*file_pos += len;
	up_write(&echo_rwlock);
	return len;
}

static struct file_operations echo_fops = {
	.read	= &echo_read,
	.write	= &echo_write,
};

static struct miscdevice echo_device = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "echo",
	.fops	= &echo_fops,
	.mode	= (S_IRUGO|S_IWUGO),
};

static int __init echo_init(void)
{
	// Allocate a page sized buffer
	kernel_buffer = (char*) kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!kernel_buffer)
		return -ENOMEM;

	// Register misc char device
	if(misc_register(&echo_device)) {
		kfree(kernel_buffer);
		return -ENODEV;
	}
	return 0;
}

static void __exit echo_exit(void)
{
	// Free buffer
	kfree(kernel_buffer);
	// Unregister misc char device
	misc_deregister(&echo_device);
}

module_init(echo_init);
module_exit(echo_exit);
