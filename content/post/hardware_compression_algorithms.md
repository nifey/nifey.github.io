+++
title = "Hardware compression algorithms"
date = 2022-01-14
lastmod = 2022-02-01T16:56:23+05:30
tags = ["research"]
draft = false
+++

Compression is a common technique that allows us to reduce the size of data.
We use it when we store or send files as _zip_, _7z_ or _rar_ archives.
It helps reduce the space occupied by files in the filesystem and also to save bandwidth while transferring over a network.

In general, compression algorithms try to encode **redundancy or regularity in data** with lesser bits.

There are many different software compression algorithms available.
Lempel-Ziv family of algorithms, used in tools like _zip_ and _7z_, works by replacing repeated chunks of data with a pointer to an earlier occurrence. This avoids repeating the same chunks of data over and over again.

Compression can also be applied at the hardware level, i.e. in microprocessors, system buses (PCIe), etc.
But algorithms like Lempel-Ziv are too complex to be implemented at this level.
When implementing at hardware level, we need _simple, low latency and parallelizable_ algorithms.

In this post, we will look at some of the hardware compression algorithms that have been proposed in literature for cache, memory and link compression.


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

    -   During allocation, most operating systems fill pages with zeroes before handing it over to an application to avoid memory leaks.
    -   Variables are usually initialized with zeros, either by the compiler or by the programmer.
2.  Most Significant Bits are unused

    Even though C compilers usually allocate 4 bytes of memory for an integer, most applications rarely use values that cannot be represented with one or two bytes.
    So the most significant bits are filled with continuous 0s or 1s depending on the sign extension.
3.  Low Dynamic Range (Value locality)

    Another observation is that most values that are stored together are close to each other in numeric value.
    The difference (or _Delta_) between adjacent or close-by values can be represented with lesser bits than representing the whole numbers.


## Algorithms {#algorithms}


### Frequent Pattern Compression {#frequent-pattern-compression}

Frequent pattern compression tries to encode common patterns within 4 Byte words.
It exploits the presence of repeated zero words and the fact that most significant bits are usually not used.
It uses 3 bits of metadata to indicate which pattern to use for the 4B word and some extra data bits depending on the pattern matched with.

The seven patterns that FPC uses are given in the table below (which is taken from the paper):

| Prefix (bits) | Pattern                                  | Data size                   |
|---------------|------------------------------------------|-----------------------------|
| 000           | Zero run                                 | 3 bits (up to 8 zeroes)     |
| 001           | 4 bit sign extended                      | 4 bits                      |
| 010           | 1 byte sign extended                     | 8 bits                      |
| 011           | Halfword sign extended                   | 16 bits                     |
| 100           | Halfword padded with zero halfword       | 16 bits (non zero halfword) |
| 101           | Two halfwords, each a byte sign extended | 16 bits (the two bytes)     |
| 110           | Word with repeated bytes                 | 8 bits                      |
| 111           | Uncompressed word                        | 32 bits                     |

Some 4B words and their FPC compressed form is shown below:

| 4B word (hex) | Prefix (bits) | Extra data (hex) |
|---------------|---------------|------------------|
| 00000007      | 001           | 7                |
| 00000071      | 010           | 71               |
| FFFF8000      | 011           | 8000             |
| 94949494      | 110           | 94               |


### Base Delta Immediate {#base-delta-immediate}

Base Delta Immediate compression tries to compress data by using _low dynamic range_.
The algorithm stores a cacheline as a 4B base value followed by the _deltas_, i.e. the difference between the actual data and the base value.
If there is high value locality, the _deltas_ can be represented with less number of bits than required for 4B values.

For instance, consider a 32B data containing the following 4B values

| 0x00203008 | 0x00203009 | 0x0020301F | 0x00203020 | 0x00203000 | 0x002030F0 | 0x002030EE | 0x00203003 |
|------------|------------|------------|------------|------------|------------|------------|------------|

After applying BDI compression, we can store it like this:

| 0x00203008 | 0x00 | 0x01 | 0x17 | 0x18 | 0xF8 | 0xE8 | 0xE6 | 0xFB |
|------------|------|------|------|------|------|------|------|------|

By storing 1B _deltas_ instead of the 4B words, we save 20 bytes.


### CPack {#cpack}

Cache Packer algorithm is a dictionary based algorithm for hardware compression.
During compression, It constructs a dictionary consisting of 4B words already seen during compression.
If new 4B word matches partially or fully with a 4B word present in the dictionary, then the algorithm stores
metadata (to identify which parts match with the dictionary word), index pointing to the matched dictionary entry and the bytes that differ.

The CPack pattern encodings for a dictionary size of 16, are shown below (taken from the paper):

| Prefix (bits) | Pattern                  | Additional Data    |
|---------------|--------------------------|--------------------|
| 00            | zzzz  (All zeroes)       |                    |
| 01            | xxxx  (No match)         | 4B data            |
| 10            | mmmm  (Full match)       | 4b index           |
| 1100          | mmxx  (2B Partial match) | 4b index + 2B data |
| 1101          | zzzx  (1B word)          | 1B data            |
| 1110          | mmmx  (1B Partial match) | 4b index + 1B data |

For example, consider the following 4 words:

| 0x12345678 | 0x000000F0 | 0x12340608 | 0x12345678 |
|------------|------------|------------|------------|

When compressed with CPack, we get:

| 0x12345678 | (1101) 0xF0 | (1100) (0000) 0x0608 | (10) (0000) |
|------------|-------------|----------------------|-------------|

The dictionary is not stored as it is not needed. Because when compressing a 4B word in a cacheline,
It is only compared with dictionary entries that were added before (_never after_) this current word is compressed.
So during decompression, the dictionary is reconstructed and when a word is about to be decompressed, the dictionary values
it needs will be in the reconstructed dictionary.


### Bit Plane Compression {#bit-plane-compression}

Bit Plane Compression is an algorithm that works at the bit level, unlike the algorithms discussed above.
It exploits all three sources of redundancy: zeroes, unused most significant bits and value locality.

It performs a _Delta-Bitplane-XOR_ transform operation on data to increase its compressibility.
The _Delta-Bitplane-XOR_ transform creates continuous runs of 0s and 1s which can then be compressed with run length encoding.
For a 128B (GPU) cacheline, this transform consists of the following three steps:

1.  _Delta_

    The cacheline is split into 4B words. The difference or _delta_ of each 4B word with that of the previous word is calculated.
    After this step, the cacheline data of 32 4B words, becomes 31 _deltas_ along with the first 4B word.
    This step exploits value locality in data.
2.  _Bitplane_

    Bitplane operation is a bitwise operation that looks similar to a matrix transpose operation.
    This is applied on the _deltas_ from the previous stage to create continuous runs of 0s or 1s.
3.  _XOR_

    The final step performs the XOR of each adjacent bitplane value from the previous step.

BPC works best when the data consists of homogeneous structures.
Compared to the above algorithms, BPC has higher compression latency (11 DRAM cycles) but gives higher compression ratio,
especially on GPU workloads, where homogeneous structures are more common.


### Byte Select Compression {#byte-select-compression}

Byte Select Compression is a class of algorithms that uses a set of byte patterns to compress data.
Each cacheline is compared to see if it matches any of the used patterns.
If it matches a pattern, only the bytes required to reconstruct it, is stored along with metadata to identify the pattern matched.
For example, if we use the pattern _ABAB_, where _A_, _B_ are different bytes that are repeated in that word, we only have to store
the two bytes _A_ and _B_, and also store the pattern number of _ABAB_.

The efficacy of byte select compression depends on the set of patterns used.
And for this, the researchers have come up with a technique to find the most frequent byte patterns from trace data.
By collecting cache traces of applications, we can find the simplest describing pattern for each cacheline.
Once we get the list of patterns, now the problem is to select a small set of patterns with high frequency of occurrence, which will be used for byte select compression.

This differs from other algorithms, in that other compression algorithms depend on human observation of patterns.
Finding patterns from application data reveals the _intricate, non obvious_ patterns,
which when used for compression gives higher compression ratio.


## References {#references}

-   [Paper: Frequent Pattern Compression](https://minds.wisconsin.edu/bitstream/handle/1793/60388/TR1500.pdf?sequence=1&isAllowed=y)
-   [Paper: Base Delta Immediate](https://www.cs.toronto.edu/~pekhimenko/courses/csc2231-f17/Papers/BDI.pdf)
-   [Paper: CPack](https://ziyang.eecs.umich.edu/~dickrp//publications/chen10aug.pdf)
-   [Paper: Bit Plane Compression](https://lph.ece.utexas.edu/merez/uploads/MattanErez/isca2016_bpc.pdf)
-   [Paper: Byte Select Compression](https://dl.acm.org/doi/pdf/10.1145/3462209)
