+++
title = "Creating sysfs files"
date = 2021-11-29
tags = ["linux", "kernel"]
draft = false
+++

The kernel provides a few ways in which userspace programs can get information from the kernel space.

1.  procfs: Used to get information about running processes
2.  debugfs: Used by kernel developers for debugging
3.  sysfs

Sysfs is used for data that is not related to a particular process.
It has information about hardware devices attached to the system and about drivers handling those devices.

Any file added to the sysfs becomes a part of the Linux Application Binary Interface (ABI).
This means that applications might start using this file and now it has to be supported (like) forever,
because kernel developers care about _not breaking userspace_.

Most sysfs files are documented under [Documentation/ABI](https://elixir.bootlin.com/linux/latest/source/Documentation/ABI).
Each entry in files under [Documentation/ABI](https://elixir.bootlin.com/linux/latest/source/Documentation/ABI) has information about a sysfs file:

-   what it does
-   when and in which kernel version it was first introduced
-   who or which mailing list to contact if it is not working as expected


## Creating sysfs files {#creating-sysfs-files}

1.  Include sysfs.h

    ```c
       #include<linux/sysfs.h>
    ```
2.  Define show and store functions for the sysfs file

    Each sysfs attribute should have one or both of the following two functions defined.

    -   **Show function**
        -   defines what is returned when we read from the sysfs file.
        -   should copy data into the given buffer and return the number of bytes copied into buffer. The buffer is page size long (4096 KB) and we should not copy beyond that size.
    -   **Store function**
        -   defines what happens when we write to the sysfs file.
        -   should read data from the buffer and return the number of bytes decoded.

    The function prototype of show and store functions are shown below.

    ```c
          static ssize_t var_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
          static ssize_t var_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
    ```
3.  Create a kobj\_attribute using the \_\_ATTR macro

    The _\_\_ATTR_ macro creates a kobject attribute by taking the name, permissions, show and store functions as arguments.

    ```c
       static struct kobj_attribute var_attribute = __ATTR(var, 0644, var_show, var_store);
    ```
4.  Create an attribute group, if creating multiple attributes that have to be created and deleted together

    ```c
       static struct attribute *attrs[] = {
       	&var_attribute.attr,
       	&var1_attribute.attr,
       	&var2_attribute.attr,
       	...,
       	NULL,
       };
       static struct attribute_group attr_group = {
       	.attrs = attrs,
       };
    ```
5.  Create a kobject

    ```c
       static struct kobject *var_kobj;
       var_kobj = kobject_create_and_add("var", kernel_kobj);
    ```

    Kobject stands for kernel object. Inside the kernel, _struct kobject_ is used to represent devices, how they are connected to each other via buses and how they are related to drivers.

    The _struct kobject_ also does reference counting, i.e. it keeps track of how many kernel references exists to the kobject.
    This is required to ensure that the kernel does not remove some kobject that is being used.
    Pointers to _struct kobject_ are accessed using get and put functions which increment and decrement the reference count respectively.

    To create a sysfs file we need a kobject.
    So we create a simple kobject using the _kobject\_create\_and\_add_ function that takes the name of the kobject and the parent kobject as parameters.

    The parent kobject decides the folder under which the sysfs files for this kobject would appear.
    A few toplevel kobjects corresponding to /​sys/kernel, /​sys/mm, etc are defined in [include/linux/kobject.h](https://elixir.bootlin.com/linux/latest/source/include/linux/kobject.h).
    For instance, If we use _kernel\_kobj_, defined in the header file, as the parent kobject then a directory named "var" will be created under /sys/kernel.

    In the cleanup function of the kernel module, the kobject must be freed by calling _kobject\_put_ function.
6.  Register attribute group or files

    Once the kobject is created, the sysfs files can be created using _sysfs\_create\_files_ or _sysfs\_create\_group_ functions. There are also other functions available for creating sysfs files in [include/linux/sysfs.h](https://elixir.bootlin.com/linux/latest/source/include/linux/sysfs.h).  Most of these functions take in the kobject and attributes as parameters.

    ```c
       retval = sysfs_create_group(var_kobj, &attr_group);
       if (retval) {
         // Error
       }
    ```


## An example: Echo file {#an-example-echo-file}

The source code shown below is a kernel module that creates a simple sysfs file that

-   When written to, will store upto a page of data
-   When read from, will return the data that was last written

This is the same functionality that was implemented as a [char device](https://nihaal.me/post/misc%5Fchar%5Fdevices/#an-example-echo-device) and as a [debugfs file](https://nihaal.me/post/creating%5Fdebugfs%5Ffiles/#an-example-echo-file) in previous posts.


### echo.c {#echo-dot-c}

```c
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
```


### Testing echo {#testing-echo}
-   Build and load the module
    ```bash
      make
      sudo insmod echo.ko
      ls /sys/kernel/echo/echo
      #.rw-r--r-- 4.1k root 28 Nov 22:34 /sys/kernel/echo/echo
    ```
-   Write to the sysfs file (as root)
    ```bash
      echo "Good Morning!" | sudo tee /sys/kernel/echo/echo
    ```
-   Read from the sysfs file. It will return what was last written to it.
    ```bash
      cat /sys/kernel/echo/echo
      # Good Morning!
    ```
-   Unload the module
    ```bash
      sudo rmmod echo
    ```

## Race conditions {#race-conditions}

I came to know about race conditions with sysfs files in [Greg KH's blog post on creating sysfs files correctly](http://kroah.com/log/blog/2013/06/26/how-to-create-a-sysfs-file-correctly/).
In that post, he describes how race conditions can happen with sysfs files and how to handle them with default attributes.

If the sysfs files are created using _sysfs\_create\_files_ or _sysfs\_create\_group_ functions when the device or driver is first created, the notification about the new device or driver may be sent to the userspace daemon (like udevd) before the sysfs files have been created. Now if the userspace daemon tries to read the sysfs files, it may not find those expected sysfs files.

To avoid this, we can set the default attributes field in _struct bus, class, device\_driver or device_ before registering them (instead of creating sysfs files using _sysfs\_create\_file_ or _sysfs\_create\_group_ in the probe function).


## References {#references}

-   [Documentation/core-api/kobject.rst](https://elixir.bootlin.com/linux/latest/source/Documentation/core-api/kobject.rst)
-   [samples/kobject/kobject-example.c](https://elixir.bootlin.com/linux/latest/source/samples/kobject/kobject-example.c)
-   [LDD3 Chapter 14](https://lwn.net/images/pdf/LDD3/ch14.pdf)
-   [Greg KH's blog post on creating sysfs files correctly](http://kroah.com/log/blog/2013/06/26/how-to-create-a-sysfs-file-correctly/)
-   [Source code of echo module](https://github.com/nifey/nifey.github.io/tree/master/code/creating%5Fsysfs%5Ffiles)
