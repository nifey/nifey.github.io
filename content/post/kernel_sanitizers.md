+++
title = "Kernel Sanitizers"
date = 2022-12-05
lastmod = 2022-12-12T10:12:24+05:30
tags = ["fuzzing", "linux", "kernel"]
draft = false
+++

When fuzzing a program by feeding random inputs to it, we need a mechanism to tell when the program is doing unexpected things.
Sanitizers help detect bugs in the program at runtime.
They are usually used along with fuzzing to detect bugs in programs.

The two roles of sanitizers:

-   _Detect incorrect program behaviour_: like accessing memory that the program is not supposed to access
-   _Report incorrect behaviour_: To be useful, the sanitizer needs to report useful information (like the stack trace and ) that makes it easier to understand and fix the bug.

The Linux kernel supports a number of sanitizers, each focussing on a different class of bugs.


## KASAN: Kernel address sanitizer {#kasan-kernel-address-sanitizer}

**Detects** : Use after free, Out of bounds reads/writes

**Implementation**: Compiler instrumentation and Shadow memory

**Overheads** : 1 byte for every 8 byte being monitored, performance overhead due to memory access checks

KASAN is of three types. **Generic KASAN** uses compiler instrumentation and shadow memory.
In addition to generic mode, there are **hardware and software tag-based KASAN modes** that are only supported on arm64 architecture.
These tag-based modes place a random tag in the unused upper bits of the pointer and check the tag while dereferencing the address.
The tag based hardware mode helps reduce the performance overhead by moving the pointer checks to hardware.
However, it requires hardware extensions, namely Memory Tagging Extension (MTE) in arm64 to perform these checks.

The working of generic KASAN is described below:

-   **Shadow memory**: KASAN uses a shadow memory to record if it is valid to access a region of memory. It hooks to the _alloc_ and _free_ calls of memory allocators, and updates the shadow memory with access information during every allocation and deallocation.
    For every 8 bytes of memory, KASAN reserves a byte in shadow memory, used to store how many bytes starting from the beginning is unpoisoned, i.e. is legally accessible.
    For instance, if the value stored is 3, it indicates that 3 bytes starting from the beginning are legally accessible.
    However if the value stored is 0, it indicates that all 8 corresponding bytes are legally accessible.
    Negative values stored in shadow memory indicates illegal access and different negative values are used to differentiate between red zones (out of bound access) and freed objects (use after free).

-   **Memory access checks**: Before every memory access (or at the beginning of each control flow block, I'm not sure which exactly), KASAN adds code to check the shadow memory to ensure that the memory accesses are performed on valid unpoisoned memory. If not, it crashes the kernel after printing a report.

-   **Red zones between objects**: To detect out of bound accesses, KASAN inserts _red zones_, i.e. inaccessible zones between objects. This helps detect accesses that read or write to memory regions outside the allocated region.

-   **Quarantine**: To detect dangling pointers (which causes use after free bugs), KASAN prevents reallocating a freed object immediately. Instead it waits for some time before reallocating the same memory to a different object. If there is any memory access to the region during this time window, it is most likely a dangling pointer dereference.

When KASAN detects an illegal memory access, it either prints a warning or panics the kernel depending on the _panic_on_warn_ kernel command line parameter.

By default (when CONFIG_KASAN_INLINE is set), the instrumented code is inlined into the functions. This allows the checks to be faster but also increases the overall kernel size.
KASAN also supports an outline configuration (CONFIG_KASAN_OUTLINE), where it creates a function to check the memory access and calls it instead of inlining into every function. This reduces the kernel image size but increases performance overheads due to additional function calls.
This can be useful when running on embedded devices where space is a constraint.

KASAN also has a _kasan_multi_shot_ boot parameter that instead of panicking the kernel, will continue executing after reporting an illegal access to detect other illegal accesses.


## KMSAN: Kernel memory sanitizer {#kmsan-kernel-memory-sanitizer}

**Detects** : Uninitialized memory


## KCSAN: Kernel concurrency sanitizer {#kcsan-kernel-concurrency-sanitizer}

**Detects** : Data races


## UBSAN: Undefined behaviour sanitizer {#ubsan-undefined-behaviour-sanitizer}

**Detects** : Variable overflows, alignment issues


## KFENCE: Kernel electric fence {#kfence-kernel-electric-fence}

KFENCE is a sanitizer that can detect memory access violations similar to KASAN.
However, KFENCE is designed as a low overhead sanitizer that can be enabled at runtime.

Instead of checking accesses to every objects, KFENCE samples and randomly picks a few objects (smaller than page size) and places them in an empty page on either end of the page and inserts guard pages on both sides of the page.
Whenever there is an access which goes beyond the object and into the guard page, it causes a page fault, allowing us to detect illegal access.
Any excess free space inside the page allocated, is filled with some fixed values to detect write overflow accesses within the same page.

The advantage of KFENCE is that it does not perform checks on every memory accesses, instead it uses the page fault mechanism to detect illegal accesses.
Moreover, it samples the memory objects and picks only a few for checking accesses.


## References {#references}

-   [Dynamic Program Analysis for Fun and Profit](https://www.youtube.com/watch?v=ufcyOkgFZ2Q)
