+++
title = "Kernel Sanitizers"
date = 2022-12-05
lastmod = 2022-12-05T10:26:39+05:30
tags = ["linux", "kernel"]
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

**Overheads** : 1/8 of memory used, performance overhead due to memory access checks

KASAN is of three types. Generic KASAN uses compiler instrumentation. In addition there is a software and hardware tag based variants that reduce the performance overheads of runtime memory access checking.

-   **Shadow memory**: KASAN uses a shadow memory to record if it is valid to access a region of memory. It hooks to the _alloc_ and _free_ calls of memory allocators, and updates the shadow memory with access information during every allocation and deallocation.

    It reserves a byte in shadow memory for every 8 bytes of memory, which is then used to store how many bytes starting from the beginning is unpoisoned. The byte stores negative values to denote memory that should not be accessed.
-   **Memory access checks**: Before every memory access (or at the beginning of each control flow block, I'm not sure which exactly), KASAN adds code to check the shadow memory to ensure that the memory accesses are performed on valid unpoisoned memory. If not, it crashes the kernel after printing a report.
-   **Red zones between objects**: To detect out of bound accesses, KASAN inserts _red zones_, i.e. inaccessible zones between objects. This helps detect accesses that read or write to memory regions outside the allocated region.
-   **Quarantine**: To detect dangling pointers (which causes use after free bugs), KASAN prevents reallocating a freed object immediately. Instead it waits for some time before reallocating the same memory to a different object. If there is any memory access to the region during this time window, it is most likely a dangling pointer dereference.

    KASAN supports a outline configuration (CONFIG_KASAN_OUTLINE), where it creates a function to check the memory access and calls it instead of inlining the code into every function. This reduces the kernel image size but increases performance overheads due to additional function calls.


## KMSAN: Kernel memory sanitizer {#kmsan-kernel-memory-sanitizer}

**Detects** : Uninitialized memory


## KCSAN: Kernel concurrency sanitizer {#kcsan-kernel-concurrency-sanitizer}

**Detects** : Data races


## UBSAN: Undefined behaviour sanitizer {#ubsan-undefined-behaviour-sanitizer}

**Detects** : Variable overflows, alignment issues
