+++
title = "Looking inside /dev/null"
date = 2021-11-20
tags = ["linux", "kernel"]
draft = false
+++

## Accessing hardware devices like files {#accessing-hardware-devices-like-files}

An important role of the operating system is to allow the user applications to access hardware resources. It has to allow the user programs to use hardware while also ensuring that the applications have permissions to access it and don't misuse them.

To simplify access to hardware, Unix, by design, exposes most of the hardware devices to userspace as _device files_, usually present under /dev directory.
Applications can then access the hardware by invoking system calls like read or write on the device files associated with that device.

Why do it this way?
The advantage is that file related system calls like open, read, write, close, etc can be reused to access hardware. And so we avoid adding too many extra system calls specifically to access hardware. Also it provides a unified interface to interact with things, be it files or devices.
It allows the user application to use hardware without dealing with the quirks of the underlying hardware.

But somebody has to deal with the quirks and details of the hardware. That is where device drivers come in.
Device drivers tell the kernel what to do when a particular system call is invoked on a device file.
It does this by

1.  Defining functions corresponding to each system call (that it supports)
2.  Creating a _file\_operations_ structure with pointers to the defined functions.
3.  Registering the device with the kernel

The driver provides the _file\_operations_ structure that contains a set of function pointers corresponding to some system calls.
Whenever an application executes a system call on a device file, the kernel will lookup the _file\_operations_ structure corresponding to the
device and invoke the function pointed by the struct's fields.

Setting the fields of _file\_operations_ struct to _NULL_ usually disables that system call on that device file.
Information about what each of the fields of _file\_operations_ struct are, and what happens when they are set to null can be found in the [LDD3 book](https://lwn.net/images/pdf/LDD3/ch03.pdf).


## An example: /dev/null {#an-example-dev-null}

To better understand this, we'll look at the simplest device file: **/dev/null**.
This device file is special because it has no associated hardware.
/dev/zero, /dev/random are some of the other special files.

The behaviour of /dev/null file is this:

-   When we read from it, it returns nothing.
-   When we write to it, the data is discarded.

The most common usecase of /dev/null is discarding the output of a command by redirecting it to /dev/null.

```bash
echo "going into the void" > /dev/null
```

The definitions of some special device files including /dev/null, /dev/zero and /dev/random can be found in [drivers/char/mem.c](https://elixir.bootlin.com/linux/latest/source/drivers/char/mem.c).
Here we will look at the read and write functions of /dev/null, which are used when a process invokes a read or write system call (respectively) on /dev/null device file.

```c
static ssize_t read_null(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t write_null(struct file *file, const char __user *buf,
			  size_t count, loff_t *ppos)
{
	return count;
}

static const struct file_operations null_fops = {
	.read		= read_null,
	.write		= write_null,
	//...
};
```


### read\_null {#read-null}

The read function of _file\_operations_ structure is expected to copy _count_ bytes of device data into the userspace buffer _buf_, update the file position _ppos_ and return the number of bytes written to the buffer.
The read function of /dev/null, does not copy anything to the buffer. It just returns 0 everytime, which indicates that the end of file has been reached.


### write\_null {#write-null}

The write function of _file\_operations_ structure is expected to read _count_ bytes of userspace data into kernelspace and return the number of bytes read from the buffer.
The write function of /​dev/null, does not read anything from the buffer.
It simply returns _count_.
