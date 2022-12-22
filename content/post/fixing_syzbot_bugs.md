+++
title = "Fixing syzbot bugs"
date = 2022-12-20
tags = ["linux", "kernel", "fuzzing"]
draft = false
+++

Syzbot is an automated fuzzing infrastructure that uses Syzkaller to perform continuous fuzzing, primarily on the Linux kernel.
Whenever it finds bugs, Syzbot reports it to the relevant mailing list.
It also has a [public dashboard](https://syzkaller.appspot.com/) where it lists all the open bugs that needs to be fixed.

Syzbot is quite effective in finding bugs in the kernel but due to the large number of bugs being found, many of them don't get fixed in time. And so, we can help fix those bugs.
In this post, I'll share the general approaches and steps in fixing Syzbot bugs.


## 0. Setup your system {#0-dot-setup-your-system}

Before we can start fixing bugs, we need to have a Qemu virtual machine setup along with my favorite tool for debugging: kgdb.

-   First, download the Linux source code and compile it.
-   Then install and boot the compiled kernel in a virtual machine using Qemu. We also need a root filesystem for this.
-   Finally, install gdb and set up kgdb.


## 1. Find a bug to fix {#1-dot-find-a-bug-to-fix}

Pick a bug that

-   was recently encountered (in the latest kernels) by Syzbot
-   has a reproducer. Because, otherwise the bug would be harder to fix.
    The reproducer program can also act as a test case for the patch that solves the issue.
-   is not yet fixed and no one else is currently trying to fix it. This is to avoid duplicated effort.
    To check, see if there are any replies to the syzbot mail to the mailing list, any
    patch testing requests on the syzbot dashboard, or any patches in the mailing list that
    mentions this bug in it's Reported-by tag. We don't want to spend time fixing a bug, only to realize that it has already been fixed.
-   is in your subsystem or bug class of your interest (Stick to one subsystem or bug class if possible)


## 2. Reproduce the bug on your system {#2-dot-reproduce-the-bug-on-your-system}

Once you have picked a bug to fix, we have to reproduce the bug on our machine.
If we cannot reproduce the bug, we cannot fix it.

-   Find if the bug is present in mainline or linux-next and checkout to the appropriate branch.
-   Compile the kernel with the respective sanitizer that detected the bug (KASAN, KMSAN, etc).
-   Run the reproducer on the compiled kernel in a virtual machine and test if the bug is detected.

If you are able to reliably reproduce the bug then you can try fixing it.


## 3. Find the cause of the bug {#3-dot-find-the-cause-of-the-bug}

-   Compare different reports of the same bug to see if you can get some information
    For example, we can try to answer questions like these:
    -   Is the stack trace changing across reports or is it the same?
        (i.e. can the bug be triggered from different code paths or only in a particular path)
    -   Is the buggy access happenning at the same offset?
    -   Is there anything similar or different across the reports? If yes, why?
-   Go down the stack trace and see what conditions needs to be true for this path to occur.
    (Shuah Khan suggested this during the LFX mentorship meetings.)
    For example, in the simplified example below, we can infer that since the buggy function is on
    the true branch of the if-else, the if condition must be true for the bug to happen.

    ```c
           if (some_bool) {
      	// Buggy function is called from this path
      	buggy_function();
           } else {
              some_function();
           }
    ```
-   Use kgdb to debug the kernel. Set a breakpoint right before the bug and look around to see what is causing the bug.
-   Minimize the reproducer manually. Try to remove some system calls or operations from the reproducer while ensuring that the reproducer still
    causes the bug.


## 4. Create and test your patch with Syzbot {#4-dot-create-and-test-your-patch-with-syzbot}

Once you have identified the root cause of the bug, we can create a patch to fix it.
Make the change that fixes the bug. Test it locally using the reproducer program to see if the change actually fixed the bug.
Then you can send your patch to Syzbot for testing.


## 5. Send your patch {#5-dot-send-your-patch}

Don't forget to add the Reported-by or Reported-and-tested-by tag to give credit to syzbot.
You can also look around the code to see if there are any other similar bugs.
