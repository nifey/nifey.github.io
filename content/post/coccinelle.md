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
Coccinelle also does not expand C macros.

It can be used for

1.  Fixing bugs based on the bug pattern. For example, whenever we allocate memory using kmalloc, we have to check if the return value is NULL.
    Using coccinelle, we can find places in the code where the check is not performed.
    This allows us to "Find (a bug) once, Fix everywhere".

2.  Performing _collateral evolutions_, i.e. making a change that needs to be propagated throughout the codebase.
    For example, a change in the arguments used for a function needs updating all the points in the code where that function is called.
    Similarly, there could be old deprecated functions that need to be replaced by new alternatives.

Coccinelle takes as input the C program files that need to be matched and transformed, and a _semantic patch_.
Semantic patches are coccinelle scripts written in Semantic Patch Language (SmPL), that specifies the code pattern that needs to be matched
and the code transformation that needs to be performed.
The syntax of semantic patches is similar to C and the notations used in patches (eg: _+_ to denote added lines and _-_ to denote deleted lines).


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
    -   Abstract unneeded code with _dots_ (...)
    -   Abstract over identifiers, expressions and constants with _metavariables_
    -   Check for matches and refine incrementally for special cases


### Rules syntax {#rules-syntax}

A semantic patch consists of many _rules_ and each rule consists of two parts:

1.  Metavariable declaration
2.  Transformation specification

The general structure of a rule is given below: (The rule name is optional.)

```Coccinelle
@rule_name@
// Metavariable declaration
@@
// Transformation specification
```


#### Metavariable declaration {#metavariable-declaration}

Metavariables are used to abstract over identifiers, expressions, constants and types.
For example, if I use a metavariable of type _expression_ , it can match over any C expression.
We can also use metavariables of a specific struct or type defined in the program.


#### Transformation specification {#transformation-specification}

-   **Matching code**:
    We can use (three) dot symbols: **...** to match any code. This allows us to match lines irrespective of
    the code present between the lines.
    However, in some situations, we may want to restrict the code that we want to skip.
    For this, we can use the **when** clause along with the dots.
    -   **...** matches any code
    -   **... when != e** matches any code which is not equivalent to the expression e.
    -   **&lt;... x ...&gt;** matches the expression represented by _x_, zero or more times.
    -   **&lt;+... x ...+&gt;** matches the expression represented by _x_, one or more times.
-   **Transforming code**:
    The left most column specifies the transformation that needs to be performed.
    The following symbols have special meaning when used as the first character in a line:
    -   **+**  adds the line to the matched code
    -   **-**   removes the matched line
    -   **\***   highlights the matched line
    -   **( | )** Disjunction allows us to specify multiple possible match patterns

#### Examples {#examples}

1.  Replace expressions of the form 1 &lt;&lt; C with the macro BIT(C) where C is any constant.
    ```Coccinelle
          @@
          constant C;
          @@

          - 1 << C
    ​      + BIT(C)
    ```
2.  Using _disjunction_: Replace expressions of the form 1 &lt;&lt; C with BIT(C) where C is any constant, and
    replace expressions of the form 1 &lt;&lt; E with BIT(E) where E is any expression.
    ```Coccinelle
          @@
          constant C;
          expression E;
          @@

          {
    ​      - 1 << C
    ​      + BIT(C)
          |
    ​      - 1 << E
    ​      + BIT(E)
          }
    ```
3.  Using _identifiers_ and _dots_: Replace assignment to a local variable followed by immediate return of that value,
    by simply the return statement.
    Notice here, we don't care about what the arguments of the function is, and so we can use dots there.
    ```Coccinelle
          @@
          identifier f;
          expression r;
          @@

          - r = f(...);
    ​      + return f(...);
    ​      - return r;
    ```
4.  Matching zero or more function calls using **&lt;... ...&gt;**
    ```Coccinelle
          @@
          identifier f;
          @@
          *f(...)
          {
          <...
    ​      * g(...)
          ...>
          }
    ```

### Advanced features {#advanced-features}


#### Rule dependencies {#rule-dependencies}

We can specify dependency between rules. This allows matching a rule only if another rule has been matched.
For example, the following sematic patch has two rules. The first rule matches if the BIT macro is used anywhere
in the file. If it is used, then the second rule which _depends_ on the first rule converts expressions 1 &lt;&lt; E into BIT(E).

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

We can also pass values between rules with dependency using **&lt;&lt;** syntax inside the metavariable declaration.

```Coccinelle
   @usesbit@
   expression E;
   @@
   1 << E

   @depends on usesbit@
   expression E << usesbit.E;
   @@

   - 1 << E
   + BIT(E)
```


#### Isomorphisms {#isomorphisms}

Coccinelle can also find and match code that is equivalent to the expression that we want to match. These statements that are semantically equivalent are called _Isomorphisms_.
For example, the following statements are equivalent and so when we try to match _(x==NULL)_, Coccinelle also tries to match the other three statements.

```c
!x  <=>  x == NULL  <=>  NULL == x;
```

Coccinelle provides a set of default isomorphisms in _/usr/lib/coccinelle/standard.iso_.
We can use custom isomorphisms by writing them in a file, let's say in custom.iso and using it in the semantic patches with **using** keyword. We can apply a custom isomorphism file-wide or for a specific rule.

```coccinelle
using "custom.iso"
@rule1@
expression e;
@@
*e
```

```coccinelle
@rule1 using "custom.iso"@
expression e;
@@
*e
```


#### Embedding Python or Ocaml scripts {#embedding-python-or-ocaml-scripts}

We can also embed scripts written in Python or OCaml, inside the semantic patches and have them run when specific rules are matched.
The rules that contain script code should uses **script:python** or **script:ocaml** in the rulename part.

```coccinelle
@zero_variable@
identifier i;
@@
i = 0;

@script:python depends on zero_variable@
i << zero_variable.i;
@@
print (f"Identifier {i} was set to zero")
```


#### Position metavariables {#position-metavariables}

Coccinelle also provides _position_ metavariables that can be attached to other metavariables (by appending with @) to find and print the position of the match
inside the file.

```coccinelle
@zero_variable@
identifier i;
position p;
@@
i@p = 0;

@script:python depends on zero_variable@
i << zero_variable.i;
p << zero_variable.p;
@@

print (f"Identifier {i} was set to zero in file {p[0].file}, in function {p[0].current_element} at line {p[0].line}.")
```


#### Universal and Existential quantification {#universal-and-existential-quantification}

todo

-   when with exists and strict


## Coccinelle Internals {#coccinelle-internals}

In [Hunting bugs with Coccinelle](https://web.imt-atlantique.fr/x-info/coccinelle/stuart_thesis.pdf), it is mentioned that Coccinelle has three parts:

1.  A C parser that converts C code into an _Abstract Syntax Tree_ (AST) and a _Control Flow Graph_ (CFG), without expanding all macros.
2.  A parser that converts semantic patches into a formula expressed in Computation Tree Logic with existentially quantified program variables (CTL-VW).
    I don't know what CTL-VW is, but I'm guessing it is a way to mathematically express code patterns in a program with multiple execution paths.
3.  A model checker that compares the CTL-VW formula with the AST and performs any modifications if specified.

After the modifications are done, the AST and CFG are unparsed to produce the modified C code.


## References {#references}

-   [Julia Lawall: An Introduction to Coccinelle Bug Finding and Code Evolution for the Linux Kernel](https://www.youtube.com/watch?v=buZrNd6XkEw)
-   [Coccinelle: Finding bugs in the Linux Kernel - Vaishali Thakkar - FOSSASIA Summit 2017](https://www.youtube.com/watch?v=2sfJ9HNlU5w)
-   [Keynote: Inside the Mind of a Coccinelle Programmer by Julia Lawall, Developer of Coccinelle](https://www.youtube.com/watch?v=xA5FBvuCvMs)
-   <http://coccinellery.org>
-   [Hunting bugs with Coccinelle](https://web.imt-atlantique.fr/x-info/coccinelle/stuart_thesis.pdf) (Henrik Stuart's Thesis)
