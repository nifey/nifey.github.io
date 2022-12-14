+++
title = "Coccinelle"
date = 2022-12-14
tags = ["linux", "kernel"]
draft = false
+++

Coccinelle is static analysis tool used for semantic pattern matching and automated transformation of C programs.
It is written in OCaml.
Unlike other pattern matching tools like grep which use regular expressions, Coccinelle understands C syntax and
can find semantic code pattern in the source code and automatically transform them, irrespective of the name of identifiers, comments or formatting.

Coccinelle is _intraprocedural_, i.e. all its matching and transformation happens within functions.

It can be used for

1.  Fixing bugs based on the bug pattern. For example, whenever we allocate memory using kmalloc, we have to check if the return value is NULL.
    Using coccinelle, we can find places in the code where the check is not performed.
    This allows us to "Find (a bug) once, Fix everywhere".

2.  Performing _collateral evolutions_, i.e. making a change that needs to be propagated throughout the codebase.
    For example, a change in the arguments used for a function needs updating all the points in the code where that function is called.
    Similarly, there could be old deprecated functions that need to be replaced by new alternatives.

Coccinelle uses SmPL (Semantic Patch Language) to detect and fix bugs using a form of pattern matching.
The syntax is similar to C and Patch language (+,-,etc)


## Installing and using Coccinelle {#installing-and-using-coccinelle}

-   To install coccinelle from source

    ```bash
      git clone https://github.com/coccinelle/coccinelle
      cd coccinelle
      ./autogen
      ./configure
      make
      sudo make install
    ```

    This will install a tool called **spatch**, which is like the _patch_ tool but for semantic patches.
    We use spatch for applying semantic patches.

-   To check if your semantic patch is valid:

    ```bash
      spatch --parse-cocci your_patch.cocci
    ```

-   To apply or run your patch on a file or directory:

    ```bash
      spatch --sp-file your_patch.cocci file.c
      spatch --sp-file your_patch.cocci --dir directory
    ```

-   To set a virtual dependency (like report,patch,org) use -D flag:

    ```bash
      spatch -D report --sp-file your_patch.cocci file.c
    ```


## Writing Semantic patches {#writing-semantic-patches}

-   Writing semantic patches
    -   Write a patch for the change
    -   Abstract of unneeded code with ...
    -   Abstract over identifiers, expressions and constants with metavariables
    -   Check for matches and refine incrementally for special cases

A semantic patch consists of many _rules_ and each rule consists of two parts:

1.  Metavariable declaration
2.  Transformation specification


### Metavariable declaration {#metavariable-declaration}

```Coccinelle
   @<name of the rule>@
   metavariable_type name;
   @@

   @depends on rule1||rule2@@
   metavariable_type name;
   @@
```


### Transformation specification {#transformation-specification}

-   On the leftmost column, we can have
    -   + to denote addition of a line
    -   - to denote removal of a line
    -   \* to denote lines of interest


### Rule dependencies {#rule-dependencies}


### Advanced features {#advanced-features}

-   Embedding Python/Ocaml scripts
-   Isomorphism
-   Position metavariables
-   Adding constraints using when
-   Iterations


## Examples {#examples}

1.  Hello World example

The following script removes 1 &lt;&lt; c in lines and replaces with BIT(c) where c
can be any number

```Coccinelle
   @@
   constant c;
   @@

   - 1 << c
   + BIT(c)
```

1.  Use of disjunctions and expressions

But c can be an expression like (3\*somevar), in that case we use an expression
Here we also use a disjunction | operator

```Coccinelle
   @@
   constant c;
   expression E;
   @@

   {
   - 1 << c
   + BIT(c)
   |
   - 1 << E
   + BIT(E)
   }
```

1.  Use of depends

The following example checks if BITS is used in the file and only then replaces
otherwise it doesn't do anything
... can be use to match any code that we don't care about

```Coccinelle
   @usesbit@
   @@
   BIT(...)

   @depends on usesbit@
   expression E;
   @@

   - 1 << E
   + BIT(E)
```

1.  Use of identifiers

The following example compresses return statement lines

```Coccinelle
   @@
   identifier f;
   expression r;
   @@

   - r = f(...);
   + return f(...);
   - return r;
```

To match multiple instances use &lt;... ...&gt;
The following code highlights every function call inside every function

```Coccinelle
   @@
   identifier f;
   @@
   *f(...)
   {
   <...
   * g(...)
   ...>
   }
```


## References {#references}

-   [Coccinelle: Finding bugs in the Linux Kernel - Vaishali Thakkar - FOSSASIA Summit 2017](https://www.youtube.com/watch?v=2sfJ9HNlU5w)
-   [Keynote: Inside the Mind of a Coccinelle Programmer by Julia Lawall, Developer of Coccinelle](https://www.youtube.com/watch?v=xA5FBvuCvMs)
-   <http://coccinellery.org>
-   [Julia Lawall: An Introduction to Coccinelle Bug Finding and Code Evolution for the Linux Kernel](https://www.youtube.com/watch?v=buZrNd6XkEw)
