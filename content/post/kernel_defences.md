+++
title = "Kernel defences"
date = 2023-01-09
tags = ["linux", "kernel"]
draft = false
+++

Usually when an attacker exploits a vulnerability, the attack starts out as a _Illegal memory access_ or _Control flow hijack_, which the attacker would use to write to sensitive memory locations or execute arbitrary code in supervisor mode, to try to increase privileges in the system.

-   _Illegal memory accesses_ are memory accesses which the programmer didn't intend to happen, which allows attackers to read or write to some memory locations.
    Illegal memory accesses can be classified on three aspects:
    1.  Read or Write access
    2.  Access to Arbitrary address or restricted address
    3.  If it is a write, Arbitrary value or restricted value write
    Write accesses and arbitrary address/value writes are more serious bugs, as they allow attackers more control over where or what value they can write, making it easier to subvert execution.
-   In a control flow hijack, the vulnerability provides a way to divert execution into an attacker controlled path.
    For instance, when an attacker controls the value of a function pointer, she can hijack control flow when that function is dereferenced.
    Control flow hijack can happen either on the forward edge (when a function is called) or on the backward edge (when a function returns).

Despite having vulnerabilities that allow illegal writes or control flow hijack, the kernel has a few defence mechanisms in place to make
it difficult to convert a vulnerability into a useful attack.


## SMEP and SMAP {#smep-and-smap}

Some architectures like x86_64 provides hardware mechanisms, in the form of additional instructions, for restricting execution or accessing
data from untrusted userspace.

-   Supervisor Mode Execution Prevention (SMEP) prevents execution of code from userspace pages while executing in supervisor mode.
    -   _Pro:_ This prevents shellcode execution from userspace pages.
    -   _Con:_ However, the attacker can still use userspace pages as ROP address chains.
-   Supervisor Mode Access Prevention (SMAP) prevents accessing userspace data _unexpectedly_ while executing in supervisor mode.
    -   Whenever userspace data is legally accessed in the code, we use a pair of instructions _clac_ and _stac_ to temporarily disable this feature.
    -   Compared to SMEP, SMAP is a stronger form of protection as it restricts all unexpected accesses to userspace.
    -   With SMAP, an attacker has to somehow unset the AC bit in the flag before userspace data can be accessed.


## KASLR {#kaslr}


## CFI {#cfi}
