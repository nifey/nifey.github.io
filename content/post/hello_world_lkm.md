+++
title = "Hello world Linux Kernel Module"
author = ["nihaal"]
date = 2020-01-31T10:42:00+05:30
draft = false
+++

A Loadable Kernel Module (LKM) is a piece of code that can be loaded to a running kernel to add additional functionality to the kernel.
The code that runs as a kernel module will be executed with kernel privileges. Most operating system kernels support kernel modules.

The advantage of using kernel modules is that it can be loaded to and unloaded from the kernel without the need to recompile or reboot the kernel.

Device manufacturers usually add the driver code to the Linux source code itself, in which case they have to comply to the GPL v2 (Linux's license).
Kernel modules allows device manufacturers to provide drivers to users as a kernel module, without revealing the source code or complying with the GPL v2 license.

Kernel modules are used to add device drivers and  filesystem drivers to the kernel. On the dark side, it can also be used to build rootkits that can intercept system calls without being noticed.
But thankfully, kernel modules can only be loaded and unloaded with superuser privileges.

In this blog, Let's write a simple Hello world Kernel module for Linux.


## Hello world Kernel module {#hello-world-kernel-module}

First we include some header files containing macros and functions that we will use.

```c
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
```

Linux kernel provides us macros to add information about the kernel module. We can add information like the author, description, license and version of the module.

```c
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abdun Nihaal");
MODULE_DESCRIPTION("Hello world module");
MODULE_VERSION("1.0");
```

Unlike other C programs, kernel modules don't have a main function. Instead the module's functions are invoked whenever necessary.

The kernel execute an initialization function when a module is loaded and a cleanup function when the module is unloaded. Let's  write the init function first.
The initialization function should be a static function marked with `__init` macro.

```c
static int __init hello_init(void)
{
        printk(KERN_INFO "Hello from kernel module\n");
        return 0;
}
```

The printk function is like the kernel version of printf. We cannot use printf in a kernel module because inside the kernel we cannot call the C standard library.
So we use printk function provided by the kernel to print information. printk allows us to print messages to the kernel log (which can be viewed using `dmesg` command).
In addition to the string to be printed, we also need to specify a log level that indicates the criticality of the information.
We'll use the KERN\_INFO log level to print the message.

The cleanup function should also be a static function marked with the `__exit` macro.
Here again, we just use printk to print a message and return.

```c
static void __exit hello_exit(void)
{
        printk(KERN_INFO "Bye from kernel module\n");
}
```

Finally we use some macros to specify the initialization and cleanup functions of this module.

```c
module_init(hello_init);
module_exit(hello_exit);
```

Save this file as hello.c


## Building the Kernel module {#building-the-kernel-module}

To build the kernel module, we can use the following Makefile.

```makefile
obj-m := hello.o

all:
        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

Run `make` to build the kernel module. This will create a new file with .ko (Kernel Object) extension.

{{< figure src="/ox-hugo/lkm_make.png" >}}


## Commands for working with kernel modules {#commands-for-working-with-kernel-modules}

-   `lsmod` - Lists all the modules that are loaded in the kernel
-   `modinfo` - Outputs information about a loaded module or a kernel object file (with .ko extension)
-   `insmod` - Loads a kernel object file as a kernel module. This requires superuser privileges
-   `rmmod` - Unloads a loaded kernel module. This also requires superuser privileges
-   `dmesg` - Displays the kernel log

Before loading our kernel module lets use `modinfo` to view information about the kernel module

```bash
modinfo hello.ko
```

This displays the information that we provided using the macros.
![](/ox-hugo/lkm_modinfo.png)

Now let's load our kernel module using insmod. While loading the module, the init function will be executed.
You can check if the module has been loaded using `lsmod`.

```bash
sudo insmod hello.ko
lsmod | grep hello
```

Check the kernel log to see if our hello world message is printed.

```bash
dmesg
```

{{< figure src="/ox-hugo/lkm_insmod.png" >}}

To unload the kernel module use the `rmmod` command. This will invoke the exit function of the module.

```bash
sudo rmmod hello
lsmod | grep hello
```

Now if we check the kernel log we can see the message printed in the exit function.

```bash
dmesg
```

{{< figure src="/ox-hugo/lkm_rmmod.png" >}}


## What's next? {#what-s-next}

Our hello world kernel module is not very useful or interesting but a lot of interesting things can be done using kernel modules.
We can write a pseudo character device like the `/dev/random` (which when read prints random characters), write drivers for devices, create a simple filesystem or a simple keylogger, etc using kernel modules.

You can learn more by reading the [Linux device drivers](https://lwn.net/Kernel/LDD3/) book.
