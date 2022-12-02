+++
title = "TLB;DR Reversing TLBs with TLB desynchronization"
date = 2022-12-01
lastmod = 2022-12-02T11:17:59+05:30
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
In the paper, TLB;DR, the authors propose a new method for reverse engineering TLBs with high accuracy.


## TLB coherency {#tlb-coherency}

Translation Lookaside buffers are a critical component in memory virtualization.
TLBs cache page table entries (PTEs) for fast address translation.
Each PTE contains two main information: the physical address of the page and permission bits to denote if a process has permission to read, write or execute (NX bit) from a page.
Whenever the operating system remaps a page to a different location or updates the permission of a page, it updates the page table entry.
But if the PTE is cached in a TLB, it can lead to correctness issues: accessing a different page or performing a write when the write permission has been revoked.
I'm not sure of other architectures, but in x86 architecture, ensuring TLB coherency is the responsibility of the operating system, i.e. whenever the OS updates a PTE, it is resposible for invalidating the corresponding PTE entry from the TLBs.

In x86, there are two ways to invalidate PTE entries in TLBs

1.  Perform individual PTE invalidations using special instructions like _invlpg_ or _invlpcid_.
2.  Perform a full TLB flush by writing to the _CR3_ register.

Both of these operations are privileged, i.e. only the kernel can execute it.


## TLB desynchronization {#tlb-desynchronization}

One of the most basic information we need for reverse engineering, is whether a particular PTE is present in the TLB or not.
The authors of TLB;DR propose a new technique called _TLB desynchronization_ to accurately find if a PTE is cached in the TLB.
The idea is simple: Change the PTE of a page to map to a different physical page without performing TLB invalidation. Now when we access that page, if it is a TLB hit (i.e. PTE entry is present in the cache), then it will access data from the originally mapped physical page. If it is a TLB miss, the memory access would cause a page table walk and eventually read data from the newly mapped physical page.
By preloading different data values in the physical pages, we can then use the data read by this memory access to find out which physical page was accessed and then using this information, infer if the PTE entry was cached in the TLB.

This is the key technique proposed in the paper which they then use to reverse engineer multiple aspects of TLBs on different Intel and AMD CPUs. They reverse engineer the inclusivity/exclusivity properties, number of TLB entries, number of sets, associativity, replacement policy and how PCIDs are used.
They also discover a undisclosed TLB replacement policy used in some Intel CPUs which combines pseudo-LRU with a MRU+1 scheme.


## Implementation {#implementation}

There are a few things about the [source code](https://github.com/vusec/tlbdr) that I found interesting.

-   First, since OSes don't allow userspace processes to desynchronize TLBs, they use a Linux kernel module to perform TLB desynchronization by swapping the PTEs of (virtually) adjacent pages.
-   To reduce interference from other processes and interrupts, the module disables kernel preemption when performing experiments. It uses the following two functions (which I believe are functions defined in the Linux kernel source code) to do that:

    ```C
      raw_local_irq_restore(flags);
      preempt_enable();
    ```
-   In trigger.c, the program allocates a large buffer and writes some data to each page to uniquely identify the physical page. I thought this would be some integer that denotes the page number, but I found this:

    ```C
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

    This actually writes x86 assembly code which returns the unique ID of the page when executed. The advantage of doing it this way is that we can now perform both data access and instruction access to the same page and in both cases identify which physical page we are accessing.
    For example, if the value stored in _i_ is _0x1234_, the following assembly code gets written to the page.

    ```nil
      ❯ echo -en "\x90\x90\x48\xb8\x34\x12\x00\x00\x00\x00\x00\x00\xc3" | disasm -c amd64
         0:    90                             nop
         1:    90                             nop
         2:    48 b8 34 12 00 00 00 00 00 00  movabs rax,  0x1234
         c:    c3                             ret
    ```

    Since this code is written in userspace pages and needs to be executed in kernel mode during the experiments, they have to disable _Supervisor Mode Execution Prevention (SMEP)_ which is a defence mechanism that prevents executing userspace code when executing in supervisor mode.
    In addition, the NX bit in the page table entries, which prevents execution of code from dirty pages (to avoid shellcode execution), has to be cleared before performing experiments.

-   An experiment from _AMD/mmuctl/source/experiments.c_ to test the presence of a shared TLB.

    ```c
      /*
      	This function tests whether an PTE cached in response to a data load
      	can be used for a consequent instruction fetch.
      	It is explained in Section A.1 of the paper.
      */
      int seperate_itlb_and_dtlb(int ways){
      	// 1. Disable SMEP since we will be executing from userspace pages
      	disable_smep();

      	// 2. Flush TLBs by writing to CR3
      	u64 cr3k;
      	cr3k = getcr3();
      	setcr3(cr3k);

      	// 3. Pick a random page from the allocated buffer and clear NX bit
      	volatile unsigned long addr;
      	unsigned long random_offset;
      	get_random_bytes(&random_offset, sizeof(random_offset));
      	//Take a random page out of the first 1000 ones
      	addr = (void *)BASE + (4096 * (random_offset % 1000));
      	//If the PTE of our address is at the end of a page table, resample
      	int difference = ((addr - (unsigned long)BASE) / 4096) % 512;
      	while(difference % 512 == 511){
      		get_random_bytes(&random_offset, sizeof(random_offset));
      		addr = (void *)BASE + (4096 * (random_offset % 1000));
      		difference = ((addr - (unsigned long)BASE) / 4096) % 512;
      	}
      	//Perform the page walk
      	struct ptwalk walk;
      	resolve_va(addr, &walk, 0);
      	clear_nx(walk.pgd);

      	// 4. Disable kernel preemption in claim_cpu()
      	down_write(TLBDR_MMLOCK);
      	claim_cpu();

      	// 5. Here is where the actual experiment starts
      	// 5.1 Load the PTE into the DTLB by reading from the page address.
      	//     Store the physical page id returned in original.
      	int original = read(addr);

      	// 5.2 Desync the TLB by swapping PTEs with adjacent page
      	switch_pages(walk.pte, walk.pte + 1);

      	// 5.3 Issue additional data loads to evict the DTLB entry from L1
      	volatile int i;
      	for(i = 0; i < ways; i++){
      		read((void *)BASE + (4096 * ((random_offset % 1000) + 1 + (i * 2))));
      	}

      	// 5.4 Perform a instruction fetch for the same address
      	//     Store the physical page id in curr
      	int curr = execute(addr);

      	// 6. Reenable kernel preemption and undo PTE swapping
      	give_up_cpu();
      	switch_pages(walk.pte, walk.pte + 1);
      	up_write(TLBDR_MMLOCK);
      	setcr3(cr3k);

      	// 7. If the instruction fetch was a TLB hit (i.e. original == curr), then there exists a shared TLB
      	//    Else if it was a TLB miss (i.e. original != curr), then there is no shared TLB
      	return !!(original == curr);
      }
    ```
