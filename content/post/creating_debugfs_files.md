+++
title = "Creating debugfs files"
date = 2021-11-06
tags = ["linux", "kernel"]
draft = false
+++

## debugfs {#debugfs}

debugfs is a pseudo-filesystem used for kernel debugging.
It is usually mounted at /sys/kernel/debug.
debugfs contains files that allow us to read debugging information.

By default, only the root user can _cd_ into the  /sys/kernel/debug directory.

To change it to allow the current user to _cd_ into debugfs, we can remount it with _uid_ set to the current user's _uid_.

```bash
sudo umount /sys/kernel/debug
sudo mount -t debugfs none /sys/kernel/debug -o uid=`echo $UID`
cd /sys/kernel/debug
```


## Creating debugfs entries {#creating-debugfs-entries}

Creating debugfs files is similar to creating character device files.
It is done by defining functions and storing pointers to these functions in a _file\_operations_ structure which is then passed to the kernel.

-   Create a debugfs directory using _debugfs\_create\_dir_ function
    This functions takes the directory name and the parent dentry.
    If the parent dentry is set to NULL, the directory is created directly under /sys/kernel/debug,
    otherwise it is created under the given parent directory.
    The return value of the function is a pointer to a dentry or an error value.
    We need to store this dentry pointer to create files in that directory and also to remove the directory in the exit function.

    ```c
      struct dentry *debugfs_create_dir(const char *name, struct dentry *parent);
    ```
-   In some functions like _debugfs\_create\_dir_, the return value is either a normal kernel space pointer or an error code.
    The return value of these functions must be checked with _IS\_ERR_ macro to check if it is an error code or a valid pointer.

    ```c
      root_dentry = debugfs_create_dir("hello", NULL);
      if (IS_ERR(root_dentry))
        return -ENODEV;
    ```
-   Create debugfs files using the functions available in [include/linux/debugfs.h](https://elixir.bootlin.com/linux/latest/source/include/linux/debugfs.h).

    ```c
      struct dentry *debugfs_create_file(const char *name, umode_t mode,
                                         struct dentry *parent, void *data,
                                         const struct file_operations *fops);
    ```

    Arguments of the above function:

    1.  _name_ : Name of the file to be created
    2.  _mode_ : Access permissions of the file
    3.  _parent_ : Dentry of the folder under which we want to create this file
    4.  _data_ : Pointer value that is stored for later use. inode.i\_private will be set to this value on a open() syscall.
    5.  _fops_ : The _file\_operations_ struct initialized with pointers to the defined functions
-   To expose the values of simple variables, we have functions for some basic types like u8, u16, etc.

    ```c
      void debugfs_create_u8(const char *name, umode_t mode, struct dentry *parent,
                             u8 *value);
      void debugfs_create_u16(const char *name, umode_t mode, struct dentry *parent,
                              u16 *value);
      void debugfs_create_u32(const char *name, umode_t mode, struct dentry *parent,
                              u32 *value);
      void debugfs_create_u64(const char *name, umode_t mode, struct dentry *parent,
                              u64 *value);
      void debugfs_create_ulong(const char *name, umode_t mode, struct dentry *parent,
                                unsigned long *value);
    ```
-   In the exit function of the kernel modules, we remove the debugfs entries using _debugfs\_remove_ function.
    We pass the dentry pointer of debugfs directory we created. The function removes debugfs entries recursively, i.e. all files
    under the directory will be removed.

    ```c
      void debugfs_remove(struct dentry *dentry);
    ```


## An example: Echo file {#an-example-echo-file}

The source code shown below is a kernel module that creates a simple debugfs file that

-   When written to, will store upto a page of data
-   When read from, will return the data that was last written

This is the same functionality that was implemented using char device in a [previous post](https://nihaal.me/post/misc%5Fchar%5Fdevices/#an-example-echo-device).
I have highlighted the lines that are related to debugfs operations.


### echo.c {#echo-dot-c}

{{< highlight c "linenos=table, linenostart=1, hl_lines=15 56-59 67-71 79-80" >}}
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
{{< /highlight >}}


### Testing echo {#testing-echo}

-   Build and load the module

    ```bash
      make
      sudo insmod echo.ko
      ls /sys/kernel/debug/hello
      #.rw-rw-rw- 0 root  5 Nov 22:26 echo
    ```
-   Write to the device

    ```bash
      echo "Good Morning!" > /sys/kernel/debug/hello/echo
    ```
-   Read from the device. It will return what was last written to it.

    ```bash
      cat /sys/kernel/debug/hello/echo
      # Good Morning!
    ```
-   Unload the module

    ```bash
      sudo rmmod echo
    ```


## References {#references}

-   [Source code of echo module](https://github.com/nifey/nifey.github.io/tree/master/code/creating%5Fdebugfs%5Ffiles)
-   [Documentation/filesystems/debugfs.rst](https://elixir.bootlin.com/linux/latest/source/Documentation/filesystems/debugfs.rst)
-   [include/linux/debugfs.h](https://elixir.bootlin.com/linux/latest/source/include/linux/debugfs.h)
