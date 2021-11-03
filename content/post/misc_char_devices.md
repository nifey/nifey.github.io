+++
title = "Misc character devices"
date = 2021-11-03
tags = ["linux", "kernel"]
draft = false
+++

## Misc Char device {#misc-char-device}

Creating a character device involves choosing a major and minor number to use and registering the device with the kernel.
For creating simple character device files which are not associated with any hardware or cannot be put in any other Major number categories, we can instead create a Miscellaneous character device.

Misc char devices makes device registration a bit easier.
The functions for handling open and llseek syscalls on misc char devices are already defined.
We only have to define functions for other syscalls that we want to support.
Also, All misc devices share the same major number: 10 (defined as MISC\_MAJOR).


## Creating a Misc Char device {#creating-a-misc-char-device}

The structure and functions needed to create misc char devices are given below:

```c
struct miscdevice  {
	int minor;
	const char *name;
	const struct file_operations *fops;
	struct list_head list;
	struct device *parent;
	struct device *this_device;
	const struct attribute_group **groups;
	const char *nodename;
	umode_t mode;
};

extern int misc_register(struct miscdevice *misc);
extern void misc_deregister(struct miscdevice *misc);
```

To create a misc char device:

1.  Include [include/linux/miscdevice.h](https://elixir.bootlin.com/linux/latest/source/include/linux/miscdevice.h) file as it contains the structure and functions used for creating a misc char device.
2.  Initialize a miscdevice structure
3.  Register the miscdevice using the _misc\_register_ function in the initialization function of the kernel module
4.  Unregister the miscdevice using the _misc\_unregister_ funciton in the exit function


## An example: Echo device {#an-example-echo-device}

To better understand how to create a misc char device, we will look at a kernel module that implements an **echo** device.

-   When we write to the echo device file, it will store upto a page of data
-   When we read from the echo device file, it will return the data that was last written

I have given the source code of the module with explanations in between.


### echo.c {#echo-dot-c}


#### Prelude {#prelude}

-   Include header files. The first three headers are required by all kernel modules.
-   Use module macros to add license, author and description

<!--listend-->

```c
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>            // For struct file_operations
#include<linux/miscdevice.h>    // For struct misc device and register functions
#include<linux/uaccess.h>       // For permissions macros
#include<linux/slab.h>          // For kzalloc
#include<linux/string.h>        // For string helper functions
#include<linux/rwsem.h>         // For reader-writers lock

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nihaal");
MODULE_DESCRIPTION("Misc char driver");
```


#### Buffer allocation and Locking {#buffer-allocation-and-locking}

We need to allocate some space to store the data that is written to the device file.
To do that, we first declare a static char pointer _kernel\_buffer_.

Then in the initialization function of the module, we allocate memory using the _kzalloc_ function.
_kzalloc_ function takes as input, the size of memory to allocate (in bytes) and the type of memory to allocate.
It allocates the required memory and zero fills it.
We use _GFP\_KERNEL_ which is used for kernel internal allocations, as the type of memory.

Any memory allocated dynamically should be freed when the module exits.
So in the exit function, we use _kfree_ to deallocate memory occupied by the buffer.

Multiple processes may try to read or write to the device file concurrently.
To avoid an inconsistent state, we use a reader-writer lock to protect the buffer.
The reader-writer lock allows multiple readers to read simultaneously, but when a process is writing
to the buffer, no other process can read or write to it at the same time.

_DECLARE\_RWSEM_ macro declares a reader-writer's lock.
We can then use _down\_read, down\_write_ functions to acquire the lock for reading or writing, and release it with
_up\_read_ or _up\_write_ depending on which down function we used.

```c
static char *kernel_buffer;
static DECLARE_RWSEM(echo_rwlock);
```


#### Read function {#read-function}

Here we define echo\_read function which will be executed when a process reads from the device file.
The read function should

-   Copy the requested amount (_size_ bytes) of data into the user buffer (_user\_buffer_),
-   Update the file offset (_file\_pos_)
-   Return
    -   the number of bytes copied to the user buffer, if the copy was successful
    -   0, if the End of File is reached
    -   negative value, if there is an error

We cannot copy data from the kernel buffer to user buffer directly as they are on different address spaces.
So we use _copy\_to\_user_ function to copy data from the kernel address space to the user process address space.

```c
ssize_t echo_read (struct file *filp, char __user * user_buffer, size_t size, loff_t * file_pos) {
	size_t len, ret;

	// Check if a page of data has been read already
	if ((*file_pos) >= PAGE_SIZE) {
		return 0;
	}

	// Acquire read lock
	down_read(&echo_rwlock);
	len = (size > PAGE_SIZE)? PAGE_SIZE: size;
	// Copy data to user buffer
	ret = copy_to_user(user_buffer, kernel_buffer, len);
	len = len - ret;
	*file_pos += len;
	// Release read lock
	up_read(&echo_rwlock);
	return len;
}
```


#### Write function {#write-function}

Here we define echo\_write function which will be executed when a process writes to the device file.
The write function should

-   Copy the requested amount (_size_ bytes) of data from the user buffer (_user\_buffer_),
-   Update the file offset (_file\_pos_)
-   Return
    -   the number of bytes left to be copied, if the copy was successful
    -   negative value, if there is an error

Here again, since we cannot directly copy data between kernel and user address spaces, we use _copy\_from\_user_ function.

```c
ssize_t echo_write (struct file *filp, const char __user * user_buffer, size_t size, loff_t * file_pos) {
	size_t len, ret;

	// Check if a page of data has been written already
	if ((*file_pos) >= PAGE_SIZE) {
		return -ENOMEM;
	}

	// Acquire write lock
	down_write(&echo_rwlock);
	memset(kernel_buffer, 0, PAGE_SIZE);
	len = (size > PAGE_SIZE)? PAGE_SIZE: size;
	// Copy data to kernel buffer
	ret = copy_from_user(kernel_buffer, user_buffer, len);
	len = len - ret;
	*file_pos += len;
	// Release write lock
	up_write(&echo_rwlock);
	return len;
}
```


#### Creating the misc device {#creating-the-misc-device}

Now that we have defined functions for handling read and write system calls, we will first create a _file\_operations_ structure,
setting the read and write members of the _file\_operations_ struct as the address of the functions we have defined.

Then we create the _miscdevice_ structure, initializing some of its members:

-   _minor_ is set to the minor number we want for our device.
    If we use MISC\_DYNAMIC\_MINOR for this member, the kernel will assign any minor number that is available.
    If we specify a number, the kernel will try to assign that minor number
    but device registration may fail if that minor number is already used by some other device.
-   _name_ specifies the name of the device file
-   _fops_ member is set as the file\_operations structure we created
-   _mode_ specifies the permissions for device file access.
    It would be more readable to use macros than to write the octal permission value.
    So we use permission macros defined in [include/linux/stat.h](https://elixir.bootlin.com/linux/latest/source/include/linux/stat.h) and [include/uapi/linux/stat.h](https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/stat.h).

    _S\_IRUGO_ denotes that User, Group and Others (UGO) have Read (R) permissions.
    Similarly _S\_IWUGO_ denotes that User, Group and Others (UGO) have Write (W) permissions.
    For the echo device, we want any user to have read and write access, so we use _(S\_IRUGO|S\_IWUGO)_

<!--listend-->

```c
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
```


#### Registering the misc device {#registering-the-misc-device}

In the module's initialization function, we first allocate space for the buffer.
Then we register the _miscdevice_ struct that we initialized earlier using the _misc\_register_ function.
If the function returns 0, then the device has been created successfully.

In the exit function, we free the buffer and deregister the device using _misc\_deregister_ function.

```c
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
```


### Makefile {#makefile}

```make
obj-m := echo.o
all:
	make -C /usr/lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /usr/lib/modules/$(shell uname -r)/build M=$(PWD) clean
```


### Testing echo {#testing-echo}

-   Build and load the module

    ```bash
      make
      sudo insmod echo.ko
    ```

    The echo device will show up at /dev/echo
-   Write to the device

    ```bash
      echo "Good Morning!" > /dev/echo
    ```
-   Read from the device. It will return what was last written to it.

    ```bash
      cat /dev/echo
      # Good Morning!
    ```
-   Unload the module

    ```bash
      sudo rmmod echo
    ```


## References {#references}

-   [Source code of echo module](https://github.com/nifey/nifey.github.io/tree/master/code/misc%5Fchar%5Fdevices)
-   [LDD3 Chapter 3](https://lwn.net/images/pdf/LDD3/ch03.pdf)
-   [include/linux/miscdevice.h](https://elixir.bootlin.com/linux/latest/source/include/linux/miscdevice.h)
-   [include/linux/stat.h](https://elixir.bootlin.com/linux/latest/source/include/linux/stat.h)
-   [include/uapi/linux/stat.h](https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/stat.h)
