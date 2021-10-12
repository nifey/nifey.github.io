+++
title = "Eudyptula Challenge task 5"
date = 2021-10-12
tags = ["linux", "kernel"]
draft = false
+++

In this post, I want to share what I learnt by doing task 5 of the Eudyptula challenge.

The Eudyptula Challenge is a set of 20 tasks designed to
help people get started with Linux kernel development.

Task 5 of the challenge is to make a [hello world kernel module](https://nihaal.me/post/hello%5Fworld%5Flkm/) get loaded
automatically when a USB keyboard is plugged in.


## Loadable kernel modules {#loadable-kernel-modules}

Linux kernel allows us to load modules to the kernel while the kernel is running.
Once loaded, the module's code executes in kernel mode and can access all the kernel's global symbols.

Loadable kernel modules reduce the memory footprint of the kernel as most modules are loaded only when they are needed.
It also reduces the attack surface.
Any vulnerability in a module can only be exploited if the module is loaded.

Device drivers are either compiled with the kernel or are made available as kernel modules.
Some of these kernel modules are loaded at boot time while others are loaded on demand, for instance, when a
hardware device is plugged in.


## Dynamic loading of kernel modules {#dynamic-loading-of-kernel-modules}


### 1. udev {#1-dot-udev}

Whenever the kernel detects new hardware that is _hotplugged_ into the system, it generates
and sends a **uevent** to a userspace daemon called **udevd**.
The uevent message contains information about the hardware ID of the device and the bus to which the device is connected to.
We can view the stream of uevent messages using udevadm:

```sh
$ sudo udevadm monitor --kernel
```

udevd processes these uevent messages and is responsible for

-   creating or deleting device files in /dev directory and setting appropriate permissions for them
-   executing scripts based on rules defined in /etc/udev/rules.d
-   loading the kernel modules required by the device by invoking **modprobe**.

The udev daemon generates the _modalias_ for the device by encoding the information from the uevent
and passes that modalias to modprobe.
For example, the alias of my USB keyboard is usb:v03F0p0324d0104dc00dsc00dp00ic03isc01ip01in00.
(We can find that from the _modalias_ file in the sysfs directory corresponding to the device.)

```bash
$ modprobe usb:v03F0p0324d0104dc00dsc00dp00ic03isc01ip01in00
```


### 2. modprobe {#2-dot-modprobe}

Modprobe is a userspace utility used for loading kernel modules.
The role of modprobe is to find and load all modules that a device might need
by using the modalias passed by udevd.

But, How does modprobe know which modules to load?

Each driver module can specify the list of devices that it is written for using
the MODULE\_DEVICE\_TABLE macro.
During compilation, the module device table of each driver is encoded as a modalias for that driver.

We can find the modalias of a module using modinfo:

```sh {hl_lines=[9]}
$ modinfo usbhid | head
filename:       /lib/modules/5.14.2-arch1-2/kernel/drivers/hid/usbhid/usbhid.ko.zst
license:        GPL
description:    USB HID core driver
author:         Jiri Kosina
author:         Vojtech Pavlik
author:         Andreas Gal
srcversion:     538A7DD58A0E86450F80FB8
alias:          usb:v*p*d*dc*dsc*dp*ic03isc*ip*in*
depends:
retpoline:      Y
```

The modalias of every module is present in the module.alias and module.builtin.alias files
present in  /lib/modules/\`uname -r\`.

```sh {hl_lines=[4]}
$ cat /lib/modules/`uname -r`/modules.alias | grep -C 2 usbhid
alias platform:HID-SENSOR-2000e2 hid_sensor_custom
alias platform:HID-SENSOR-2000e1 hid_sensor_custom
alias usb:v*p*d*dc*dsc*dp*ic03isc*ip*in* usbhid
alias acpi*:PNP0C50:* i2c_hid_acpi
alias acpi*:ACPI0C50:* i2c_hid_acpi
```

So now all modprobe has to do to load the correct modules is to go through the module.alias file
and load modules with a modalias that matches the modalias passed by udevd.

We can list all modules that match with a modalias by using modprobe's -R flag.

```bash
$ modprobe -R usb:v03F0p0324d0104dc00dsc00dp00ic03isc01ip01in00
usbhid
```


## Difference between modprobe and insmod {#difference-between-modprobe-and-insmod}

**modprobe** and **insmod** are the two userspace utilities used to load kernel modules.
The differences between the two are:

| modprobe                                                            | insmod                                                 |
|---------------------------------------------------------------------|--------------------------------------------------------|
| Resolves module dependencies                                        | Only loads the specified .ko (kernel object) file      |
| Loads module from default module path (/lib/modules/\`uname -r\`)   | Loads module from user specified location              |
| Module parameters can be provided in config file in /etc/modprobe.d | Module parameters are given as commmand-line arguments |

Before loading a module, modprobe will load all modules that the current module depends on.
modprobe finds dependencies between modules by looking at the /lib/modules/\`uname -r\`/modules.dep file.


## Solution for the task {#solution-for-the-task}


### 1. Adding a modalias {#1-dot-adding-a-modalias}

The solution for the task was simply to use the MODULE\_DEVICE\_TABLE macro to set the modalias
of the hello world module to correspond to all USB keyboards.

To find the device table entry to use, I looked at the source code of the current USB keyboard driver in my system.
The driver that handles USB keyboards is **usbhid** (hid stands for human interface device).

To find the driver that is being used for a device, we can use udevadm.

```sh
$ udevadm info -a -n /dev/input/by-id/usb-your-keyboard-name | grep -i driver
E: ID_USB_DRIVER=usbhid
```

Usually, for every module, we can find a directory or C file with the module name in the kernel source.
The source code of usbhid module is present under drivers/hid/usbhid.
In that directory, the file usbkbd.c contains driver code for USB keyboards.
We can find the device table entry for USB keyboards in that file.

The following snippet creates the device table with an entry corresponding to USB keyboards
(taken from usbkbd.c) and registers it with the MODULE\_DEVICE\_TABLE macro.

```c
static const struct usb_device_id hello_id_table[] = {
  { USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
                       USB_INTERFACE_PROTOCOL_KEYBOARD) },
  { }						/* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, hello_id_table);
```

I compiled the hello world module after adding the above snippet.
After compilation, running modinfo shows us an alias entry.

```sh {hl_lines=[8]}
$ modinfo hello.ko
filename:       /home/nihaal/lk/hello.ko
version:        1.0
description:    Hello world kernel module that loads automatically when USB keyboard is plugged in
author:         Abdun Nihaal
license:        GPL
srcversion:     2D34CB10D98468A6F260278
alias:          usb:v*p*d*dc*dsc*dp*ic03isc01ip01in*
depends:
retpoline:      Y
name:           hello
vermagic:       5.14.2-arch1-2 SMP preempt mod_unload
```


### 2. Make modprobe detect the new driver {#2-dot-make-modprobe-detect-the-new-driver}

To make modprobe detect our module, we have to copy the module (.ko file) into the default module
path (/lib/modules/\`uname -r\`) and use **depmod** to update the module.alias file.
When used with the -A flag, depmod will only check files that are more recent than
the module.alias file.

```bash
sudo cp hello.ko /lib/modules/`uname -r`/
depmod -A
```

Now modprobe will be able to load our module and will load it automatically when a USB keyboard is plugged in.


## References {#references}

-   [Eudyptula Challenge website](https://eudyptula-challenge.org/)
-   [Eudyptula Challenge tasks](https://github.com/nifey/eudyptula/)
-   [LDD3 Chapter 2](https://lwn.net/images/pdf/LDD3/ch02.pdf)
-   [Hello world kernel module](https://nihaal.me/post/hello%5Fworld%5Flkm/)
