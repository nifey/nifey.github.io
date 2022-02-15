+++
title = "Using static checkers on Linux Kernel"
date = 2022-02-15
tags = ["linux", "kernel"]
draft = false
+++

One of the hurdles in contributing to Linux kernel as a beginner is to find something to work on.
If you are working as a kernel developer or are a part of a kernel mentorship program, this shouldn't be a problem. But for others, the first step is to send a bunch of coding style fixes for warnings reported by checkpatch.

In addition to checkpatch, there are many other static analysis tools like sparse, coccinelle and smatch, used to analyse source code and find possible bugs.
In this post, I'll show how to use some of these tools to find issues to fix.

When using static analysis tools to find bugs or issues,

1.  Verify if there really is an underlying issue that needs fixing.
    Static analysis tools have false positives. Ensure that you are not fixing a false positive.
2.  Try to solve the underlying issue and not just silence the warning.
3.  In your commit message, it is better to describe why the change is needed or what problem it is solving, than to say that it fixes some tool's warning. You can mention the tool that reported the warning.


## [Checkpatch](https://www.kernel.org/doc/html/latest/dev-tools/checkpatch.html?highlight=coding%20style) {#checkpatch}

The kernel has it's own coding style which is described in the [documentation](https://www.kernel.org/doc/html/latest/process/coding-style.html).
Enforcing a coding style helps, for instance, when grepping through the code base.

Checkpatch is a perl script, used to check patches and files for coding style violations.
It is present under the kernel's scripts folder.

```bash
# To use it on patches
./scripts/checkpatch.pl *.patch

# To use it on files
./scripts/checkpatch.pl -f file.c

# To run checkpatch before committing
git diff | ./scripts/checkpatch.pl
```

If checkpatch finds a coding style violation, it shows a short description, the file, line number and the line that needs to be fixed.
In the output below, it shows a check to avoid camel case in a function name.

```nil
CHECK: Avoid CamelCase: <rtl8188e_PHY_SetRFReg>
#14: FILE: drivers/staging/r8188eu/os_dep/ioctl_linux.c:3628:
+		rtl8188e_PHY_SetRFReg(padapter, RF_PATH_A, arg, 0xffffffff, extra_arg);

total: 0 errors, 0 warnings, 1 checks, 12 lines checked
Your patch has style problems, please review.
```

Fixing checkpatch errors is an easy way to send your first patch, as most checkpatch errors are trivial to fix.
KernelNewbies site has an excellent writeup on setting up the tools, building the kernel and sending your first patch: [FirstKernelPatch](https://kernelnewbies.org/FirstKernelPatch).

-   Not all subsystem maintainers accept checkpatch fixes as it can possibly break working code and also changes the git blame for the lines modified.
    The staging subsystem ([drivers/staging](https://elixir.bootlin.com/linux/latest/source/drivers/staging)) is a good place to send checkpatch fixes.
-   Some checks like "CHECK: line length of 126 exceeds 100 columns" are better ignored.
    A KernelNewbies post lists some of the [warnings to ignore](https://kernelnewbies.org/CheckpatchTips).

When sending patches, always run checkpatch to ensure that they conform to the kernel coding style.
Patches with style errors are usually rejected.
To run checkpatch automatically whenever we commit changes, add checkpatch to git's post commit hook, as shown below: (I learnt this from [LFD 103](https://training.linuxfoundation.org/training/a-beginners-guide-to-linux-kernel-development-lfd103/))

```bash
cat << EOF > .git/hooks/post-commit
#!/bin/sh
exec git show --format=email HEAD | ./scripts/checkpatch.pl --strict --codespell
EOF
chmod a+x .git/hooks/post-commit

# Make changes, stage and commit
git commit -s -v

# Post-commit hook will run checkpatch on your code
# Fix any errors found and amend to previous commit
git commit --amend
```


## [Sparse](https://www.kernel.org/doc/html/latest/dev-tools/sparse.html) {#sparse}

Sparse is a semantic parser which uses annotations in the kernel source code to find possible mistakes.
It can find bugs caused by incorrect locking, endianness mismatch, mixing kernel and user pointers, etc.

Sparse is available to install in most distributions. The latest development version can be installed from the git repository.

```bash
sudo apt-get install sparse

# Or from git repository
git clone git://git.kernel.org/pub/scm/devel/sparse/sparse.git
cd sparse
make
make install
```

To run sparse checks, we have to set the C argument when compiling the kernel:

```bash
# To run sparse on files about to be compiled
make C=1
# To run sparse on all files
make C=2

# To save the warnings to a file
make C=2 2>outfile
```


## [Coccinelle](https://www.kernel.org/doc/html/latest/dev-tools/coccinelle.html) {#coccinelle}

Coccinelle is a tool used for making collateral evolutions i.e. making the same change throughout the code base.
For example, if we want to ensure that the result of a particular function is checked for null, we can use coccinelle to find locations where it is not checked.

We can specify code matching and transformation rules as _semantic patches_ written in SmPL (Semantic patch language).
For instance, the following semantic patch replaces expressions of the form (1 &lt;&lt; X) with BIT(X).

```nil
@@
expression e;
@@
- (1 << e)
+ BIT(e)
```

Semantic patches written for the kernel are found in [scripts/coccinelle](https://elixir.bootlin.com/linux/latest/source/scripts/coccinelle) directory and can run in four modes:

1.  report : Reports the issue and file location
2.  patch : Generates a patch to fix the issue
3.  context : Displays the context of the issue
4.  org : Generates an org file report to be viewed in emacs

Report mode is supported by all kernel semantic patches. The other modes may not be implemented by all kernel semantic patches.

Coccinelle can be installed from the git repository.

```bash
git clone https://github.com/coccinelle/coccinelle
cd coccinelle
./autogen
./configure
make
sudo make install
```

This installs a command line tool called **spatch** (stands for semantic patch).

To run coccinelle on the kernel source:

```bash
# To run all kernel semantic patches (in report mode (default))
make coccicheck

# To run all kernel semantic patches in patch mode
make coccicheck MODE=patch

# To run all kernel semantic patches on a directory
make coccicheck M=drivers/staging/

# To run a particular semantic patch
make coccicheck COCCI=scripts/coccinelle/misc/returnvar.cocci
```

We can also directly use spatch and specify a semantic patch with the --sp-file argument.

```bash
# When running kernel semantic patches, set the mode using -D
spatch --sp-file scripts/coccinelle/misc/returnvar.cocci -D report *.c

# When running other semantic patches
spatch --sp-file other.cocci *.c
```
