+++
title = "Hardware compression algorithms"
date = 2022-01-14
tags = ["research"]
draft = false
+++

_This post is a work in progress._

Compression is a common technique that allows us to reduce the size of data.
We use it when we store or send files as _zip_, _7z_ or _rar_ archives.
It helps reduce the space occupied by files in the filesystem and also to save bandwidth while transferring over a network.

In general, compression algorithms try to encode **redundancy or regularity in data** with lesser bits.

There are many different software compression algorithms available.
Lempel-Ziv family of algoritms, used in tools like _zip_ and _7z_, works by replacing repeated chunks of data with a pointer to the first occurrence. This avoids repeating the same chunks of data over and over again.

Compression can also be applied at the hardware level, i.e. in microprocessors, system buses (PCIe), etc.
But algorithms like Lempel-Ziv are too complex to be implemented at this level.
When implementing at hardware level, we need _simple, low latency and parallelizable_ algorithms.

In this post, we will look at some of the hardware compression algorithms that has been proposed in literature for cache, memory and link compression.


## Applications of compression in hardware {#applications-of-compression-in-hardware}

-   Caches are effective in hiding the long latency access to memory.
    Their effectiveness can get reduced if the cache capacity is small or if the application exhibits irregular memory access patterns.
    Cache compression allows us to store more cachelines and thus increase the effective cache capacity.
-   Link compression compresses the data before sending and after receiving it over a communication channel (system bus or over a network).
    This reduces the bandwidth and power consumed by the data transfer.
-   Memory compression is used to increase the effective main memory capacity.
    Memory compression can be helpful in embedded systems that have limited memory.


## Redundancy and Regularity in data {#redundancy-and-regularity-in-data}

Hardware compression algorithms work by exploiting observations in data that show redundancy or regularity.
Some of the observations that have been used to design hardware compression algorithms are:

1.  Zeroes

    There are a lot of zero values observed. Some reasons for this are:

    -   During allocation, most operating systems fill pages with zeroes before handing it over to an application to avoid memory leak.
    -   Variables are usually initialized with zeros, either by the compiler or by the programmer.
2.  Most Significant Bits are unused

    Eventhough C compilers usually allocate 4 bytes of memory for an integer, most applications rarely use values that cannot be represented with one or two bytes.
    So the most significant bits are filled with continuous 0s or 1s depending on the sign extension.
3.  Low Dynamic Range (Value locality)

    Another observation is that most values that are stored together are close to each other in numeric value.
    The differenece (or _Delta_) between adjacent or close-by values can be represented with lesser bits than representing the whole numbers.
