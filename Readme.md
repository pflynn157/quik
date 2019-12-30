## Quik

### Note
Check the dev branch for the latest progress; the master branch may not have all the latest commits.

### Introduction
This is the third version of my Quik programming language. Quik is a simple general-purpose systems programming language. Its similar to C in that its basically portable assembler, but its different in that its intended to be somewhat easier to use and understand, which still being able to interface with the underlying system libraries. This project contains the compiler, standard library, and tests. The compiler in turn is split into two parts: the parser library, which loads a source file and generates an AST; and the compiler, which calls the parser and generates Assembly.

I wrote the entire thing from scratch. I don't intend for Quik to fill any special need or be the next big thing; I wrote this primarily as a learning experience, which is why I wrote the whole thing from scratch. Not to mention, its been a really fun hobby project.

Currently, the compiler is only capable of generating 32-bit x86 Intel Assembly code.

### Building
To build, you simply need CMake, a C++ compiler, and Python 3 (for testing). Quik does not use any libraries other than the standard C++ library. A compiler supporting at least the C++11 standard is necessary.

### Running
In order to use the compiler, you need NASM and GCC. NASM is used to build an object file from the generated assembly. GCC is used to link it (this makes everyone's life easier). 

Three programs are generated: quikc, qkast, and lextest. Quikc is the actual compiler. Simply specify your input. By default, the executable will be named "out" and put in the current directory, but you can change this behavior with the -o switch. Multiple inputs are sort of supported, but there's problems with the standard library when you do this. You can still interface with the underlying system without including any part of the standard library. The qkast program generates a visual representation of the abstract syntax tree (it just prints it to the console). I wrote this primarily for development purposes, but its interesting to understand how the frontend works. The lextest tests the lexical analyzer component. You should have no need to run this unless you plan to work on the language.

Please note that you need to run the "install-stdlib.sh" script before trying to run or build anything.

The generated assembly files are stored in /tmp directory. They are not removed when compiling is complete, so if you wish to see what the assembly looks like, check these out.

### What works?
I originally had a features list here, but removed it after I began making changes to the backend and broke a lot of stuff (these are good changes :) ).

To get an idea of what works, check the test folders. These also currently function as the language specs until I write one later on dwon the line.
