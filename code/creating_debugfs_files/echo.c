#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/debugfs.h>
#include<linux/slab.h>
#include<linux/rwsem.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nihaal");
MODULE_DESCRIPTION("Creating debugfs files");
MODULE_VERSION("1.0");

static char *echo_buf;
static DECLARE_RWSEM(echo_rwlock);
static struct dentry *root_dentry;

ssize_t echo_read (struct file *filp, char __user * buf, size_t size, loff_t * f_pos) {
	size_t len, ret;

	if ((*f_pos) >= PAGE_SIZE) {
		return 0;
	}

	down_read(&echo_rwlock);
	len = (size > PAGE_SIZE)? PAGE_SIZE: size;
	ret = copy_to_user(buf, echo_buf, len);
	len = len - ret;
	*f_pos += len;
	up_read(&echo_rwlock);
	return len;
}

ssize_t echo_write (struct file *filp, const char __user * buf, size_t size, loff_t * f_pos) {
	size_t len, ret;
	if ((*f_pos) >= PAGE_SIZE) {
		return -ENOMEM;
	}

	down_write(&echo_rwlock);
	memset(echo_buf, 0, PAGE_SIZE);
	len = (size > PAGE_SIZE)? PAGE_SIZE: size;
	ret = copy_from_user(echo_buf, buf, len);
	len = len - ret;
	*f_pos += len;
	up_write(&echo_rwlock);
	return len;
}

static struct file_operations echo_fops = {
	.read	= &echo_read,
	.write	= &echo_write,
};

static int __init echo_init(void)
{
	// Create "hello" directory
	root_dentry = debugfs_create_dir("hello", NULL);
	if (IS_ERR(root_dentry))
		return -ENODEV;

	// Allocate buffer
	echo_buf = (char*) kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!echo_buf) {
		return -ENOMEM;
	}

	// Create echo file
	if (IS_ERR(debugfs_create_file("echo", (S_IRUGO|S_IWUGO), root_dentry, NULL, &echo_fops))) {
		kfree(echo_buf);
		return -ENODEV;
	}
	return 0;
}

static void __exit echo_exit(void)
{
	kfree(echo_buf);

	// Recursively remove debugfs entries
	debugfs_remove(root_dentry);
}

module_init(echo_init);
module_exit(echo_exit);
