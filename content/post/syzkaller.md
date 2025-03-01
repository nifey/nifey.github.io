+++
title = "Finding bugs with Syzkaller"
date = 2022-12-12
tags = ["fuzzing", "syzkaller"]
draft = false
+++

Syzkaller is an unsupervised, grammar based, coverage guided fuzzer used for fuzzing operating system kernels.
It primarily performs system call fuzzing, but it can also be used for fuzzing USB and network packets.
It is currently used for continuous fuzzing of Linux, Android and other BSD kernels.

-   _Automated_:
    Syzkaller can automatically restart crashed virtual machines and also create a reproducer for the crashes.
-   _Coverage guided_:
    Syzkaller gets coverage information using the KCOV infrastructure, which is built into the kernel.
    After executing a mutated program from the corpus, Syzkaller checks if the program has increased the coverage, i.e. it checks whether any
    new code paths in the kernel have been reached during the execution of the program. If so, it adds the program to the corpus for further mutation.
-   _Grammar based_:
    Syzkaller understands the structure and API of system calls.
    It uses system calls descriptions written in  _[Syzlang](https://github.com/google/syzkaller/blob/master/docs/syscall_descriptions_syntax.md)_ to generate valid system calls with randomized values in arguments.
    This allows it to execute deeper code paths that would not be possible with blind random fuzzing.
    Syzlang can describe structs that are passed as arguments to syscalls, the possible values for flags and resources (like file descriptors) which are passed across multiple syscalls and have to be created and destroyed using specific system calls.


## Syzkaller overview {#syzkaller-overview}

![](https://github.com/google/syzkaller/blob/master/docs/process_structure.png?raw=true)

The above image, taken from [Syzkaller documentation](https://github.com/google/syzkaller/blob/master/docs/internals.md) shows the overall working of Syzkaller.
Syzkaller consists of different components like syz-manager, syz-fuzzer and syz-executor.
_syz-manager_ is the process that manages virtual machines, maintains and updates the corpus of input program based on coverage information obtained through _syz-fuzzer_.
It also serves the web interface that displays VM information, bugs found and their respective crash logs.
Inside the virtual machine, _syz-executor_ is the program that executes the system calls based on the input from _syz-fuzzer_.


## Setting up Syzkaller {#setting-up-syzkaller}


### 1. Compile the Linux kernel for fuzzing {#1-dot-compile-the-linux-kernel-for-fuzzing}

-   Download source
    ```bash
      git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git linux
      cd linux
      make defconfig;        # Generate default .config
      make kvm_guest.config; # Enable some configs for virtualization
    ```
-   Ensure that the following configurations are enabled in the .config file.
    ```nil
      CONFIG_KCOV=y          # Coverage collection
      CONFIG_DEBUG_INFO=y    # Debug info

      CONFIG_KASAN=y         # Memory bug detector
      CONFIG_KASAN_INLINE=y

      CONFIG_CONFIGFS_FS=y   # Required for debian rootfs
      CONFIG_SECURITYFS=y
    ```
-   Compile
    ```bash
      make -j16
    ```

### 2. Setup Qemu {#2-dot-setup-qemu}

-   Create rootfs with SSH access.
    We need a root filesystem for booting the kernel in a QEMU virtual machine.
    Syzkaller provides a helper script _tools/create-image.sh_ for creating a debian image.
    The script also creates ssh keys for logging into the VM through SSH.
    ```nil
      git clone https://github.com/google/syzkaller
      cd syzkaller
      ./tools/create-image.sh -d buster             # Create a debian buster image
      # Now you should find buster.img, buster.id_rsa, buster.id_rsa.pub created
    ```
-   Test Qemu.
    Syzkaller accesses VM through SSH and so we need to ensure that the kernel boots and that we are able to SSH into the VM using the generated SSH keys.
    If the VM boots fine and we are able to SSH into it, then we are all set for fuzzing.
    ```nil
      # Start qemu with the compiled kernel and debian image
      KERNEL=/home/nihaal/linux     # Path to linux source
      IMAGE=/home/nihaal/syzkaller  # Path to directory containing debian image and SSH keys
      qemu-system-x86_64 \
          -m 2G \
          -smp 2 \
          -kernel $KERNEL/arch/x86/boot/bzImage \
          -append "console=ttyS0 root=/dev/sda earlyprintk=serial net.ifnames=0" \
          -drive file=$IMAGE/buster.img,format=raw \
          -net user,host=10.0.2.10,hostfwd=tcp:127.0.0.1:10021-:22 \
          -net nic,model=e1000 \
          -enable-kvm \
          -nographic \
          -pidfile vm.pid \
          2>&1 | tee vm.log

      # From a different console, test SSH access
      ssh -i $IMAGE/buster.id_rsa -p 10021 -o "StrictHostKeyChecking no" root@localhost
    ```

### 3. Setup and run Syzkaller {#3-dot-setup-and-run-syzkaller}

-   Build Syzkaller.
    Syzkaller is written in Go. Install golang on your system and compile Syzkaller with make.
    ```bash
      # Install golang
      cd syzkaller
      make
    ```
-   Create syz-manager config.
    We need to pass a configuration file for Syzkaller that contains information about the location of the kernel and root filesystem,
    number of processes, number of VM and the number of virtual CPUs and memory in each VM.
    Description of different configs can be found <span class="underline">[here](https://github.com/google/syzkaller/blob/master/pkg/mgrconfig/config.go)</span>
    ```nil
      {
          "target": "linux/amd64",          # System architecture
          "http": "127.0.0.1:54321",        # URL for web interface
          "workdir": "$WORKDIR",            # Used for storing crash logs and corpus
          "kernel_obj": "$KERNEL",          # Kernel source directory
          "image": "$IMAGE/buster.img",     # Generated root filesystem image
          "sshkey": "$IMAGE/buster.id_rsa", # Generated SSH key
          "syzkaller": "$SYZKALLER",        # Syzkaller directory
          "procs": 2,                       # Number of processes to use in each VM
          "type": "qemu",
          "reproduce": false,               # Disable reproducer generation
          "vm": {
              "count": 4,                   # Number of VMs to use
              "kernel": "$KERNEL/arch/x86/boot/bzImage",  # Path to compiled kernel
              "cpu": 2,                     # Number of CPUs in each VM
              "mem": 2048                   # Memory used by each VM
          }
      }
    ```
-   Start fuzzing
    ```nil
      ./bin/syz-manager -config my.cfg
    ```

## Syzbot {#syzbot}

Syzbot is an automated continuous fuzzing infrastructure that performs continous fuzzing of different kernels using Syzkaller and reports any crashes and associated reproducers to the respective mailing lists.
It can even test patches to test if the patch actually fixes the bug.
The Syzbot infrastructure also has a [public dashboard](https://syzkaller.appspot.com/) that shows the currently open bugs and crash logs.


## TODO {#todo}

-   Syzlang
-   Testing patches with syzbot
-   Adding custom syscall description
-   USB and network fuzzing
-   Fixing syzbot bugs


## References {#references}

-   <span class="underline">[LWN article: Coverage-guided kernel fuzzing with syzkaller](https://lwn.net/Articles/677764/)</span>
-   Collabora's blog series on Syzkaller:
    -   <span class="underline">[Syzkaller: Fuzzing the kernel](https://www.collabora.com/news-and-blog/blog/2020/03/26/syzkaller-fuzzing-the-kernel/)</span>
    -   <span class="underline">[Using Syzkaller to detect programming bugs in the kernel](https://www.collabora.com/news-and-blog/blog/2020/04/17/using-syzkaller-to-detect-programming-bugs-in-linux/)</span>
    -   <span class="underline">[Using Syzkaller to fuzz your changes](https://www.collabora.com/news-and-blog/blog/2020/05/12/using-syzkaller-fuzzing-your-changes/)</span>
    -   <span class="underline">[Driver fuzzing](https://www.collabora.com/news-and-blog/blog/2020/06/26/using-syzkaller-part-4-driver-fuzzing/)</span>
-   <span class="underline">[Syzkaller](https://github.com/google/syzkaller)</span>
-   <span class="underline">[Syzbot Dashboard](https://syzkaller.appspot.com/)</span>
