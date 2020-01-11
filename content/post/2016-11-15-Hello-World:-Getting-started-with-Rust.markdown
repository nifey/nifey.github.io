---
categories: Rust
date: "2016-11-15T17:50:37Z"
title: 'Hello World: Getting started with Rust'
---
## The Rust programming language

Rust is a system programming language that focusses on three aspects: Speed, Safety and Concurrency. It is being developed by an active community of programmers. Its development is officially sponsored by Mozilla.

### Installing rust

First, we have to install the rust compiler which is called **rustc** and **cargo** ,rust's package manager. I recommend installing rustc and cargo using rustup. Why rustup? Before answering that I have to mention that rustc comes in three versions : Stable , Beta and Nightly.

**Rustup** is a multi toolchain installer that lets you manage different versions of rustc and also lets you cross compile for different architectures and operating systems. Simply put , it makes installation easy.

To install rustup ,go to rustup.rs and follow the instructions.

Sometimes you have to add the path of  cargo directory to the environment variables.

In linux , type in

``` export PATH=$PATH:$HOME/.cargo/bin ```     

where $HOME/.cargo/bin is the default cargo directory.

After installing , type in rustc and cargo in the terminal or command prompt. If it prints unknown command , you probably have not installed it properly.

### Writing rust code

To write rust code , you can use an ordinary text editor or you can install bindings for popular IDEs. I'll have links to that below.

I recommend using Atom. It has a lot of packages that helps in writing rust code. If you are going to use atom then be sure to install the following packages.

### Atom packages for rust:

  *  language-rust
  *  linter-rust (shows errors while typing  )
  *  rustfmt  (formats code while saving the file)
  *  racer (code completion utility)

### Hello World

So now that you have installed rustc and cargo. Let's create a sample program. To create a binary application using cargo run :

```cargo new helloworld --bin```

Cargo automatically creates a hello world program in a file named main.rs in the src folder. To build it open terminal in the project directory and run

```cargo build```

This will create an executable in the debug folder. To run this program type in

```cargo run```

If you see Hello World printed on the screen then you have just executed your first rust program.

### External links

The official rust website : [www.rust-lang.org](www.rust-lang.org)

Rustup   : [www.rustup.rs](www.rustup.rs)

Atom text editor : [www.atom.io](www.atom.io)

The book (Official rust documentation) : [doc.rust-lang.org/book](doc.rust-lang.org/book)

Rust extensions for various IDEs : see [awesome-rust](https://github.com/kud1ing/awesome-rust#ides) list

IRC channels : #rust  #rust-beginners on [irc.mozilla.org](irc.mozilla.org)
