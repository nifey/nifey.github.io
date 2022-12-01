+++
title = "TLB;DR Reversing TLBs with TLB desynchronization"
date = 2022-12-01
lastmod = 2022-12-01T10:26:45+05:30
tags = ["research", "hardware-security"]
draft = false
+++

Yesterday, I read an interesting research paper about reverse engineering TLBs using TLB desynchronization.
In this post, I'll write briefly about the key ideas and what I found very interesting in the paper.

You can find the paper here:

-   [**TLB;DR: Enhancing TLB-based Attacks with TLB Desynchronized Reverse Engineering**](https://www.usenix.org/conference/usenixsecurity22/presentation/tatar).
-   [TLB;DR Source code](https://github.com/vusec/tlbdr)


## Reverse engineering CPU internals {#reverse-engineering-cpu-internals}

In the subfield of hardware security that focuses on communicating (covert channels) or leaking (side channels) critical information using timing or storage channels, accurate information about the CPU internals helps create more efficient and reliable channels.
Information about the size, associativity, set mapping, etc. of caches and TLBs allow the attacker to fine tune their attacks.
However, most CPU vendors don't disclose such information in detail, and so, attackers resort to reverse engineering these microarchitectural details.
Reverse engineering hardware structures is usually performed by observing timing differences, which can have a lot of noise.
In the paper, TLB;DR, the author propose a new method for reverse engineering TLBs with high accuracy.


## TLB coherency {#tlb-coherency}

Translation Lookaside buffers are a critical component in memory virtualization.
TLBs cache page table entries (PTEs) for fast address translation.
Each PTE contains two main information: the physical address of the page and permission bits to denote if a process has permission to read, write or execute from a page (NX bit).
Whenever the operating system remaps a page to a different location or updates the permission of a page, it updates the page table entry.
But if the PTE is cached in a TLB, it can lead to correctness issues: accessing a different page or performing a write when the write permission has been revoked.
I'm not sure of other architectures, but in x86 architecture, ensuring TLB coherency is the responsibility of the operating system, i.e. whenever the OS updates a PTE, it is resposible for invalidating the corresponding PTE entry from the TLBs.

In x86, there are two ways to invalidate PTE entries in TLBs

1.  Perform individual PTE invalidations using special instructions like _invlpg_ or _invlpcid_.
2.  Perform a full TLB flush by writing to the _CR3_ register.

Both of these operations are privileged, i.e. only the kernel can execute it.


## TLB desynchronization {#tlb-desynchronization}

The authors of TLB;DR propose using a technique called TLB desynchronization to accurately reverse engineer TLBs.
One of the most basic information we need for reverse engineering, is whether a particular entry is present in the TLB or not.
To get this information, we can change the PTE of a page to map to a different physical page and not perform TLB invalidation. Now when we access that page, if it is a TLB hit (i.e. PTE entry is present in the cache), then it will access data from the originally mapped physical page. If it is a TLB miss, the memory access would cause a page table walk and eventually read data from the newly mapped page.
By having different data in the physical pages, we can then use the data returned by the memory access to find out accurately it the PTE entry was cached in the TLB.

This is the key technique proposed in the paper which they then use to reverse engineer multiple aspects of TLBs on different Intel and AMD CPUs. They reverse engineer the inclusivity/exclusivity properties, number of TLB entries, number of sets, associativity, replacement policy and how PCIDs are used.
They also discover a undisclosed TLB replacement policy used in some Intel CPUs which combines pseudo-LRU with a MRU+1 scheme.


## Implementation {#implementation}

There are a few things about the source code that I found interesting.

-   First is that since OSes don't allow userspace processes to desynchronize TLBs, the source code consists of a kernel module which performs PTE swapping to perform TLB desynchronization.
-   To reduce interferance from other processes and interrupts, the module disables kernel premption when performing experiments. It uses the following two functions (which I believe are functions defined in the Linux kernel source code) to do that.

    ```nil
      raw_local_irq_restore(flags);
      preempt_enable();
    ```
-   In trigger.c, the program allocates a large buffer and writes some data to each page to uniquely identify the physical page. I thought this would be some integer that denotes the page number, but I found this:

    ```nil
      //Write an identifier to each unique physcial page
      //The identifier will be returned when this code is executed
      volatile unsigned char *p1;
      for(i = 0; i < unique_pages; i++){
              p1 = BASE + (4096 * i);
              *(uint16_t *)p1 = 0x9090;
              p1[2] = 0x48; p1[3] = 0xb8;
              *(uint64_t *)(&p1[4]) = i;
              p1[12] = 0xc3;
      }
    ```

    This actually writes some assembly code which returns the unique ID of the page when executed. The advantage of doing it this way is that we can now perform both data access and instruction access to the same page and in both cases identify which physical page we are accessing.
