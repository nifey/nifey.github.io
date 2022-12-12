+++
title = "Linux kernel fuzzing"
date = 2022-12-03
lastmod = 2022-12-12T10:11:08+05:30
tags = ["fuzzing", "linux", "kernel"]
draft = false
+++

In this post, we'll see how fuzzing is used for finding different types of bugs in the Linux kernel.
This post consists of my notes taken from [the talk by Andrey Konavalov about Linux fuzzing](https://www.linuxfoundation.org/webinars/fuzzing-linux-kernel?hsLang=en).

Operating systems kernels are complex.
Testing kernels is of prime importance since any vulnerability in the kernel can lead to compromising the whole system.

Fuzzing is a dynamic program analysis technique, used to find bugs in software.
It works by feeding random input to programs until it crashes.
In recent times, Fuzzing has been effective in finding bugs, especially the ones that are hard to detect manually.

{{% admonition example %}}
Fuzzing is **feeding random inputs into a program until it crashes**
{{% /admonition %}}

**[Syzkaller](https://github.com/google/syzkaller)** is a coverage guided kernel fuzzer written in Go, that has discovered a lot of bugs by fuzzing system calls.
It supports various kernels including Linux, Android, OpenBSD, NetBSD, etc.
In addition to system call fuzzing, it also performs USB fuzzing and fuzzing through filesystem images.

In his [talk](https://www.linuxfoundation.org/webinars/fuzzing-linux-kernel?hsLang=en), Andrey Konavalov describes the five aspects of fuzzing:

1.  Running the program
2.  Feeding input to the program
3.  Generating random inputs
4.  Detecting crashes
5.  Automating the whole process


## Running the program {#running-the-program}

When fuzzing the kernel, we can execute the kernel either on physical hardware or on a virtual machines.
The advantage of using physical devices is that we can fuzz drivers, but fuzzing on real devices is difficult to
scale and can also lead to devices getting bricked. Virtual machines, on the other hand, are easy to manage
and scale, but it can fuzz only what the hypervisor or emulator supports.


## Feeding input to the program {#feeding-input-to-the-program}

Normal userspace programs takes input from command line, standard input, environment variables or input files.
However, for kernels, the primary mode of interacting with it is through system calls (called by userspace programs).
To perform fuzzing on system calls, the fuzzer needs to create and execute a binary that invokes those system calls.
The other external ways of input to the kernel include network packets, file system mounting and hardware interactions (USB/PCI devices).


## Generating random inputs {#generating-random-inputs}

It does not suffice to generate random inputs.
A good fuzzer needs to generate random inputs that are most likely to crash the program and also goes through different execution paths of the program.

-   _Structure aware_: Input generation process needs to be aware of the structure of arguments.
    Most system calls takes some C structure as argument. The input generated should follow the semantics of the structure.

-   _Coverage guided fuzzing_: To detect a large number of bugs, fuzzing needs to have good coverage.
    In coverage guided fuzzing, the input generation process uses a corpus of valid inputs and mutates them.
    If a mutated input is found to increase coverage, then it is added into the corpus.
    This process is repeated to increase the fuzzing coverage.

-   _API aware_: System calls act like an API. For example, the open syscall returns a file descriptor which is then used in subsequent syscalls.
    The input generation must understand this API to generate valid programs that can test deeper parts of the kernel code.

-   _Script aware_: Linux kernel has internal script languages like BPF.
    The input generation process should understand and generate valid programs in the script language which can then be passed through syscalls.


## Detecting crashes {#detecting-crashes}

During fuzzing, bugs can be detected by dynamic program analysis tools or by programmer inserted checks (BUG_ON, WARN_ON).
Kernel Sanitizers like KASAN, KMSAN, KCSAN, UBSAN can be enabled by setting respective config values before building the kernel.
These sanitizers detect bugs and prints useful information like the stack trace or memory dump to the kernel log.
These reports can then be used to locate the bug and fix it.

| Kernel Sanitizers                     | Type of bugs detected                |
|---------------------------------------|--------------------------------------|
| Kernel Address Sanitizer (KASAN)      | Out of bounds access, Use after free |
| Kernel Memory Sanitizer (KMSAN)       | Uninitialized memory                 |
| Kernel Concurrency Sanitizer (KCSAN)  | Data races                           |
| Undefined Behaviour Sanitizer (UBSAN) | Variable overflows, alignment issues |


## Automating the whole process {#automating-the-whole-process}

Syzkaller automates the fuzzing process by monitoring the kernel logs for crash reports. When a crash is
detected, it ensure that the crash is not a duplicate crash by comparing with older crashes. It also continues
the fuzzing process by restarting virtual machines. In addition, Syzkaller can, in some cases, generate a
reproducer for the crash which will help in checking if a patch has fixed the bug.

[Syzbot](https://syzkaller.appspot.com/) infrastructure helps report these crashes to the SyzBot dashboard and to the respective mailing list.


## References {#references}

-   [Andrey Konovalov's talk in LFX mentorship series](https://www.linuxfoundation.org/webinars/fuzzing-linux-kernel?hsLang=en)
-   [Syzkaller](https://github.com/google/syzkaller)
-   [Syzbot Dashboard](https://syzkaller.appspot.com/)
