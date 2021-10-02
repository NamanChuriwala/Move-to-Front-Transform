# Homework 1 - CSE 320 - Fall 2021
#### Professor Eugene Stark

### **Due Date: Friday 09/17/2021 @ 11:59pm**

**Read the entire doc before you start**

## Introduction

In this assignment, you will write a command line utility
(called `mtft`, for *Move to Front Transform*)
to perform compression of a data stream by applying the
"[move-to-front transform](https://en.wikipedia.org/wiki/Move-to-front_transform)",
followed by [Fibonacci coding](https://en.wikipedia.org/wiki/Fibonacci_coding).
The goal of this homework is to familiarize yourself with C programming,
with a focus on input/output, bitwise manipulations, and the use of pointers.

For all assignments in this course, you **MUST NOT** put any of the functions
that you write into the `main.c` file.  The file `main.c` **MUST ONLY** contain
`#include`s, local `#define`s and the `main` function (you may of course modify
the `main` function body).  The reason for this restriction has to do with our
use of the Criterion library to test your code.
Beyond this, you may have as many or as few additional `.c` files in the `src`
directory as you wish.  Also, you may declare as many or as few headers as you wish.
Note, however, that header and `.c` files distributed with the assignment base code
often contain a comment at the beginning which states that they are not to be
modified.  **PLEASE** take note of these comments and do not modify any such files,
as they will be replaced by the original versions during grading.

> :scream: Array indexing (**'A[]'**) is not allowed in this assignment. You
> **MUST USE** pointer arithmetic instead. All necessary arrays are declared in
> the `mtft.h` header file. You **MUST USE** these arrays. **DO NOT** create
> your own arrays. We **WILL** check for this.

> :nerd: Reference for pointers: [https://beej.us/guide/bgc/html/#pointers](https://beej.us/guide/bgc/html/#pointers).

# Getting Started

Fetch base code for `hw1` as described in `hw0`. You can find it at this link:
[https://gitlab02.cs.stonybrook.edu/cse320/hw1](https://gitlab02.cs.stonybrook.edu/cse320/hw1).
**IMPORTANT: 'FETCH', DO NOT 'CLONE'.**

Both repos will probably have a file named `.gitlab-ci.yml` with different contents.
Simply merging these files will cause a merge conflict. To avoid this, we will
merge the repos using a flag so that the `.gitlab-ci.yml` found in the `hw1`
repo will replace the `hw0` version.  To merge, use this command:

```
git merge -m "Merging HW1_CODE" HW1_CODE/master --strategy-option=theirs
```

> :scream: Based on past experience, many students will either ignore the above command or forget
> to use it.  The result will be a **merge conflict**, which will be reported by git.
> Once a merge conflict has been reported, it is essential to correct it before committing
> (or to abort the merge without committing -- use `git merge --abort` and go back and try again),
> because git will have inserted markers into the files involved indicating the locations of the
> conflicts, and if you ignore this and commit anyway, you will end up with corrupted files.
> You should consider it important to read up at an early stage on merge conflicts with git and
> how to resolve them properly.

Here is the structure of the base code:

<pre>
.
├── .gitlab-ci.yml
└── hw1
    ├── .gitignore
    ├── hw1.sublime-project
    ├── include
    │   ├── debug.h
    │   └── mtft.h
    ├── Makefile
    ├── rsrc
    │   ├── banana.out
    │   ├── banana.txt
    │   ├── bison
    │   ├── bison.out
    │   ├── gettysburg.out
    │   └── gettysburg.txt
    ├── src
    │   ├── main.c
    │   └── mtft.c
    ├── test_output
    │   └── .git_keep
    └── tests
        └── basecode_tests.c
</pre>

- The `.gitlab-ci.yml` file is a file that specifies "continuous integration" testing
to be performed by the GitLab server each time you push a commit.  Usually it will
be configured to check that your code builds and runs, and that any provided unit tests
are passed.  You are free to change this file if you like.

> :scream:  The CI testing is for your own information; it does not directly have
> anything to do with assignment grading or whether your commit has been properly
> pushed to the server.  If some part of the testing fails, you will see the somewhat
> misleading message "commit failed" on the GitLab web interface.
> This does **not** mean that "your attempt to commit has failed" or that "your commit
> didn't get pushed to the server"; the very fact that the testing was triggered at
> all means that you successfully pushed a commit.  Rather, it means that "the CI tests
> performed on a commit that you pushed did not succeed".  The purpose of the tests are
> to alert you to possible problems with your code; if you see that testing has failed
> it is worth investigating why that has happened.  However, the tests can sometimes
> fail for reasons that are not your fault; for example, the entire CI "runner" system
> may fail if someone submits code that fills up the system disk.  You should definitely
> try to understand why the tests have failed if they do, but it is not necessary to be
> overly obsessive about them.

- The `hw1.sublime-project` file is a "project file" for use by the Sublime Text editor.
It is included to try to help Sublime understand the organization of the project so that
it can properly identify errors as you edit your code.

- The `Makefile` is a configuration file for the `make` build utility, which is what
you should use to compile your code.  In brief, `make` or `make all` will compile
anything that needs to be, `make debug` does the same except that it compiles the code
with options suitable for debugging, and `make clean` removes files that resulted from
a previous compilation.  These "targets" can be combined; for example, you would use
`make clean debug` to ensure a complete clean and rebuild of everything for debugging.

- The `include` directory contains C header files (with extension `.h`) that are used
by the code.  Note that these files often contain `DO NOT MODIFY` instructions at the beginning.
You should observe these notices carefully where they appear.

- The `src` directory contains C source files (with extension `.c`).

- The `tests` directory contains C source code (and sometimes headers and other files)
that are used by the Criterion tests.

- The `rsrc` directory contains some samples of data files that you can use for
testing purposes.

 - The `test_output` directory is a scratch directory where the Criterion tests can
put output files.  You should not commit any files in this directory to your
`git` repository.

## A Note about Program Output

What a program does and does not print is VERY important.
In the UNIX world stringing together programs with piping and scripting is
commonplace. Although combining programs in this way is extremely powerful, it
means that each program must not print extraneous output. For example, you would
expect `ls` to output a list of files in a directory and nothing else.
Similarly, your program must follow the specifications for normal operation.
One part of our grading of this assignment will be to check whether your program
produces EXACTLY the specified output.  If your program produces output that deviates
from the specifications, even in a minor way, or if it produces extraneous output
that was not part of the specifications, it will adversely impact your grade
in a significant way, so pay close attention.

**Use the debug macro `debug` (described in the 320 reference document in the
Piazza resources section) for any other program output or messages you many need
while coding (e.g. debugging output).**

# Part 1: Program Operation and Argument Validation

In this part of the assignment, you will write a function to validate the arguments
passed to your program via the command line. Your program will treat arguments
as follows:

- If no flags are provided, you will display the usage and return with an
`EXIT_FAILURE` return code.

- If the `-h` flag is provided, you will display the usage for the program and
  exit with an `EXIT_SUCCESS` return code.

- If the `-e` flag is provided, then the program will read uncompressed data from
  standard input (`stdin`) and emit compressed data to standard output (`stdout`)

- If the `-d` flag is provided, then the program will read compressed data from
  standard input (`stdin`) and emit uncompressed data to standard output (`stdout`).

- If the `-b` flag is provided, it will be followed by a numeric token that
  specifies the number of bytes of uncompressed data that comprise a single "symbol"
  (This is explained further below.) The possible values are 1 or 2.
  Normal integer format (including possible leading zeroes) is to be accepted.

In all cases, the program reads data from `stdin` and writes transformed data
to `stdout`.  Any other printout, such as diagnostic messages produced by the
program, are written to `stderr`.  If the program runs without error, then it
will exit with the `EXIT_SUCCESS` status code; if any error occurs during the
execution of the program, then it will exit with the `EXIT_FAILURE` status code.

> :nerd: `EXIT_SUCCESS` and `EXIT_FAILURE` are macros defined in `<stdlib.h>` which
> represent success and failure return codes respectively.

> :nerd: `stdin`, `stdout`, and `stderr` are special I/O "streams", defined
> in `<stdio.h>`, which are automatically opened at the start of execution
> for all programs, do not need to be reopened, and (almost always) should not
> be closed.

The usage scenarios for this program are described by the following message,
which is printed by the program when it is invoked without any arguments:

<pre>
USAGE: bin/mtft [-h] [-e|-d] [-b BYTES]
   -h       Help: displays this help menu.
   -e       Read uncompressed data from stdin and write compressed data to stdout.
   -d       Read compressed data from stdin and write uncompressed data to stdout.
   -b       Interpret BYTES (1 or 2) bytes of uncompressed data as a single
            input "symbol" (default is 1).
</pre>

The square brackets indicate that the enclosed argument is optional.
The `-e|-d` means that one of `-e` or `-d` may be specified.
The `-b BYTES` means that if `-b` is present, then it is immediately followed
by a parameter `BYTES`.

A valid invocation of the program implies that the following hold about
the command-line arguments:

- All "positional arguments" (`-h`, `-e`, or `-d`) come before any optional arguments
(`-b`).
The optional arguments may come in any order after the positional ones.

- If the `-h` flag is provided, it is the first positional argument after
the program name and any other arguments that follow are ignored.

- If the `-h` flag is *not* specified, then exactly one of `-e` or `-d`
must be specified.

- If an option requires a parameter, the corresponding parameter must be provided
(*e.g.* `-b` must always be followed by a BYTES specification).

    - If `-b` is given, the BYTES argument must be an integer literal whose value is
      either 1 or 2.

For example, the following are a subset of the possible valid argument
combinations:

- `$ bin/mtft -h ...`
- `$ bin/mtft -e`
- `$ bin/mtft -d -b 01`

> :scream: The `...` means that all arguments, if any, are to be ignored; e.g.
> the usage `bin/mtft -h -x -y BLAHBLAHBLAH -z` is equivalent to `bin/mtft -h`.

Some examples of invalid combinations would be:

- `$ bin/mtft -b 1 -e`
- `$ bin/mtft -e -d`
- `$ bin/mtft -e -b 5`
- `$ bin/mtft -z 20`

> :scream: You may use only "raw" `argc` and `argv` for argument parsing and
> validation. Using any libraries that parse command line arguments (e.g.
> `getopt`) is prohibited.

> :scream: Any libraries that help you parse strings are prohibited as well
> (`string.h`, `ctype.h`, etc).  The use of `atoi`, `scanf`, `fscanf`, `sscanf`,
> and similar functions is likewise prohibited.  *This is intentional and
> will help you practice parsing strings and manipulating pointers.*

> :scream: You **MAY NOT** use dynamic memory allocation in this assignment
> (i.e. `malloc`, `realloc`, `calloc`, `mmap`, etc.).

> :nerd: Reference for command line arguments: [https://beej.us/guide/bgc/html/#command-line-arguments](https://beej.us/guide/bgc/html/#command-line-arguments).

**NOTE:** The `make` command compiles the `mtft` executable into the `bin` folder.
All commands from here on are assumed to be run from the `hw1` directory.

### **Required** Validate Arguments Function

In `mtft.h`, you will find the following function prototype (function
declaration) already declared for you. You **MUST** implement this function
as part of the assignment.

```c
int validargs(int argc, char **argv);
```

The file `mtft.c` contains the following specification of the required behavior
of this function:

```c
/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the various options that were specified will be
 * encoded in the global variable 'global_options', where it will be
 * accessible elsewhere in the program.  For details of the required
 * encoding, see the assignment handout.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * @modifies global variable "global_options" to contain an encoded representation
 * of the selected program options.
 */
```

> :scream: This function must be implemented as specified as it will be tested
> and graded independently. **It should always return -- the USAGE macro should
> never be called from validargs.**

The `validargs` function should return -1 if there is any form of failure.
This includes, but is not limited to:

- Invalid number of arguments (too few or too many).

- Invalid ordering of arguments.

- A missing parameter to an option that requires one (e.g. `-b` with no
  `BYTES` specification).

- Invalid parameter.  A numeric parameter specfied with `-b` is invalid if
  it does not have the form of a sequence of digits ('0'-'9'), or if it denotes
  a value outside of the specified set ({1, 2} for `BYTES`).

The `global_options` variable of type `int` is used to record the mode
of operation (i.e. encode/decode) of the program and associated parameters.
This is done as follows:

- If the `-h` flag is specified, the most significant bit (bit 31) is 1.

- If the `-e` flag is specified, the second-most significant bit (bit 30)
  is 1.

- If the `-d` flag is specified, the third-most significant bit (bit 29)
  is 1.

- The least signficant byte (bits 7 - 0) records the number of bytes per symbol
  (1 or 2) specified with `-b`, or the default value (1) if `-b` was not
  given.

If `validargs` returns -1 indicating failure, your program must call
`USAGE(program_name, return_code)` and return `EXIT_FAILURE`.
**Once again, `validargs` must always return, and therefore it must not
call the `USAGE(program_name, return_code)` macro itself.
That should be done in `main`.**

If `validargs` sets the most-significant bit of `global_options` to 1
(i.e. the `-h` flag was passed), your program must call `USAGE(program_name, return_code)`
and return `EXIT_SUCCESS`.

> :nerd: The `USAGE(program_name, return_code)` macro is already defined for you
> in `mtft.h`.

If validargs returns 0, then your program must read input data from `stdin`
and write output data to `stdout`.
Upon successful completion, your program should exit with exit status `EXIT_SUCCESS`;
otherwise, in case of an error it should exit with exit status `EXIT_FAILURE`.

Unless the program has been compiled for debugging (using `make debug`),
in a successful run that exits with `EXIT_SUCCESS` no other output may be produced
by the program.  In an unsuccessful run in which the program exits with `EXIT_FAILURE`
it is permissible to output to `stderr` a diagnostic message that indicates the reason
for the failure, even if the program has not been compiled for debugging.

> :nerd: Remember `EXIT_SUCCESS` and `EXIT_FAILURE` are defined in `<stdlib.h>`.
> Also note, `EXIT_SUCCESS` is 0 and `EXIT_FAILURE` is 1.

### Example validargs Executions

The following are examples of the setting of `global_options` and the
other global variables for various command-line inputs.
Each input is a bash command that can be used to invoke the program.

- **Input:** `bin/mtft -h`.  **Setting:** `global_options=0x80000000`
(`help` bit is set, other bits clear).

- **Input:** `bin/mtft -e `.  **Setting:** `global_options=0x40000001`
(mode is "encode", bytes per symbol is 1).

- **Input:** `bin/mtft -d -b 2`.  **Setting:** `global_options=0x20000002`
(mode is "decode", bytes per symbol is 2).

- **Input:** `bin/mtft -b 2 -e`.  **Setting:** `global_options=0x0`.
This is an error case because the specified argument ordering is invalid
(`-b` is before `-e`). In this case `validargs` returns -1, leaving
`global_options` unset.

# Part 2: Overview of the Data Compression Algorithm

The objective of
[data compression](https://en.wikipedia.org/wiki/Data_compression)
is to read binary input data (in general, a sequence of "raw" bits)
and apply some kind of an encoding transformation to it to produce binary output
data (some other sequence of bits) in such a way that the number of bits in the
output is generally less than the number of bits in the input.
For *lossless* data compression, the applied transformation is invertible,
so that the original input data can be exactly recovered from the compressed
version.

In this assignment, you will implement a lossless data compression program that
performs its I/O using the basic operations of reading or writing one
8-bit byte at a time.  In C, the function `getchar()` reads a single byte of
data from `stdin` and the function `putchar()` writes a single byte of data
to `stdout`.  (More generally, the function `fgetc()` reads a single
byte of data from a specified "input stream", of which `stdin` is an example,
and the function `fputc()` writes a single byte of data to a specified output
stream, of which `stdout` and `stderr` are examples.)
Your program should perform its I/O to `stdin` and `stdout` using these
specific functions.

We may consider each 8-bit byte of uncompressed data read by a data compression
program as a "symbol", drawn from an "alphabet" having 256 elements.
We do not otherwise assign any meaning to these symbols; in particular,
we do not assume anything about whether the sequence of symbols is used to
represent text or any other type of data.
However, the type of information that is encoded by the sequence of symbols
and the way in which it is encoded will affect statistical properties of the
sequence such as the probability of occurrence of each individual symbol.
For example, the ASCII code assigns a meaning (letter, digit, punctuation,
etc.) to each of 128 distinct symbol values (0 to 127).  In a sequence of
bytes produced using ASCII-encoding, the symbol values from (128 to 255)
will never occur; *i.e.* they will have probability zero.  In addition,
in ASCII-encoded text the symbol value for the letter 'z' will occur much less
frequently than that for the letter 'e'.
Nevertheless, we are using the same number of bits (*i.e.* 8) to encode symbols
that will occur frequently as we are using for symbols that occur only
infrequently.  If we could somehow use fewer bits for symbols that occur
frequently and more bits for symbols that occur only infrequently, the overall
number of bits required to represent the same information could be reduced.
This is the basic idea underlying data compression.

> :nerd: There is nothing particularly special about 8-bit "symbols" or 256-element
> "alphabets", other than that our programming language (and the underlying operating
> system) provide us with functions that are convenient for reading eight bits at a time.
> With a little bit of programming work, we can easily implement functions that
> can read and write two bytes at a gulp.
> So, we will alternatively consider 16-bit "symbols" drawn from an alphabet having
> 65536 elements.  The parameter given with the `-b` option to your program will be used
> to select between these modes of operation.
> For concreteness, the discussion below will assume 8-bit symbols and a 256-element
> alphabet, but everything that is said also applies to 16-bit bytes,
> if we just change the way in which the input is read.

What we are going to do is to perform data compression by reading input symbols
having a fixed number of bits (8 or 16) and applying a variable-length encoding
that uses fewer bits for more-frequently occurring symbols and more bits for
less-frequently occurring symbols.  In order to do this, we have to be able to
do two things: (1) we need to be able to determine which symbols occur more
frequently and which occur less frequently; and (2) we need an appropriate
variable-length encoding that takes this information into account.

To address issue (1) we will first apply to the input data a mapping called the
*[move to front transform](https://en.wikipedia.org/wiki/Move-to-front_transform)*.
The move-to-front transform maintains information
about how recently each symbol was last seen in the input and replaces each
symbol by its *rank*, which tells how long ago it was last seen.
For example, the symbol that was read most recently has rank 0, the symbol
that was read second-most-recently has rank 1, and so on, up to the symbol
that was last read the longest ago, which has maximum rank (equal to the size
of the alphabet, minus one).  A simple way of maintaining this rank information
is using an array whose entries are symbols and whose indices are ranks.
Each time a symbol is read, the array is scanned to find the current rank of
that symbol.  Then the symbol is taken from its current position in the array
and "moved to the front", shifting each of the intervening symbols down by one rank.
Over time, the contents of the array will come to approximately represent the
relative probabilities of the symbols, with the most frequently occurring symbols
having the lowest rank.
So far, we haven't reduced the number of bits at all, since we are just outputting
ranks using the same number of bits as for the input symbols.
Essentially, the move-to-front transform just replaces each symbol by a different
one (given by its rank), but the mapping is dynamically updated as we go along.

To actually reduce the number of bits and achieve compression, we need a
variable-length encoding scheme that assigns fewer bits to lower ranks and more bits
to higher ranks.  The *Fibonacci code* is such a scheme.
Recall that the Fibonacci sequence (1, 1, 2, 3, 5, 8, 13 ...) is the sequence of
integers generated by starting with 1, 1 and then extending the sequence by repeatedly
appending the sum of the last two entries.
The set {1, 2, 3, 5, 8, 13, ...} of numbers generated in this way is called
the set of *Fibonacci numbers*.
[*Zeckendorf's Theorem*](https://en.wikipedia.org/wiki/Zeckendorf%27s_theorem)
states that every positive integer can be represented uniquely as the sum
of one or more distinct Fibonacci numbers in such a way that the sum does not
include two consecutive Fibonacci numbers.
The unique such sum for a given positive integer N is called the
*Zeckendorf representation* of N.
The existence of the Zeckendorf representation is very interesting, because it
provides the basis for a *prefix code* for the positive integers.
A prefix code is a set of bit sequences, called *code words*, none of which is
a prefix of any other sequence in the set.  Prefix codes have the property that
if we concatenate any sequence of code words, we obtain a sequence of bits
that can be unambiguously parsed back into the original sequence of code words.
For example, suppose N = 64.  The Zeckendorf representation of N is 1 + 8 + 55.
We may express this sum as a sequence of bits, where a 1 bit in the ith position
means that the ith Fibonacci number is in the sum and a 0 bit in the ith
position means that the ith Fibonacci number is not in the sum.
Then the sum for N = 64 may be expressed as the sequence of bits 100010001
(the first nine Fibonacci numbers are; 1 2 3 5 8 13 21 34 55) with no two
consecutive 1 bits.  The code word for such a sequence is obtained by appending
an additional 1 (which is not interpreted as corresponding to a Fibonacci number).
If we read any sequence of bits obtained by concatenating a sequence of code
words obtained from Zeckendorf representations in this way, a sequence of
two consecutive 1 bits will always signal the end of a code word.
For example, from the bit sequence 100100111000100011 we may extract the
code words 10010011, 1000100011.  The second of these represents the integer 64.
The first of them represents 1 + 5 + 21 = 27, so this sequence of bits encodes
the integer sequence 27, 64.

It turns out that the code word for a positive integer N may be computed by the
following "greedy algorithm":

>Given positive integer N, look for the largest Fibonacci number F that is
>less than or equal to N.
>If F is the ith Fibonacci number F<sub>i</sub> (numbered starting from 1),
>then the code word will have i+1 bits, with a 1 bit in the last position.
>Now consider in decreasing order the Fibonacci numbers
>F<sub>i-1</sub>, F<sub>i-2</sub>, ..., F<sub>1</sub> less than F<sub>i</sub>.
>If the number F<sub>k</sub> being considered is less than or equal to N,
>then put a 1 in the kth position of the code word (numbered starting from 1),
>subtract F<sub>k</sub> from N, and continue on to the next smaller Fibonacci
>number.

To recover the original value N from its code word, just read the bits in
sequence while accumulating a sum.  If the ith bit is a 1, then add the ith
Fibonacci number to the sum.  When a 1 bit is seen immediately following
a preceding 1 bit, then return the accumulated sum (without adding any
Fibonacci number for the last 1 bit).

With the above information in mind, we may now summarize the operation of our
data compression program as follows:

>Read bytes from the standard input, regarding each sequence of either
>one or two bytes as a symbol (according to the `BYTES` parameter
>passed to the program with `-b`).  Using the move-to-front transform,
>replace each symbol by its current rank.  Then, form the Fibonacci encoding
>of the rank plus 1 (because the Fibonacci encoding requires positive integers,
>whereas ranks start from 0) to obtain a sequence of output bits.
>Each successive group of eight bits is output as a single byte,
>with the first bit of each group of eight as the most-significant bit
>and the last bit of each group as the least-significant bit.

Let's consider a very simple example.  Suppose we wish to apply our data compression
algorithm to an ASCII-encoded text file `banana.txt` that contains a single line of text
with the word "`BANANA`", followed by a newline character.
By using the `od` ("octal dump") program, we can view the contents file as a sequence
of ASCII codes, rather than as text:

```
$ od -t x1 banana.txt
0000000 42 41 4e 41 4e 41 0a
0000007
```

Here the `$` is the shell prompt and `od -t x1 banana.txt` is the command
given to the shell by the user.  The option `-t x1` specifies that each
individual byte of data (the `1` means one byte) in the file is to be printed
as an 8-bit numeric value shown in hexadecimal.
The first column of the output specifies (in octal) the offset
from the beginning of the file at which the data in that row begins.
The data after the first column are the individual data bytes (in hexadecimal),
starting from the indicated offset.  This particular file is exactly 7 bytes long
and contains bytes with values `42`, `41`, `4e`, `41`, `4e`, `41`, `0a`
(expressed in hexadecimal).
Referring to a table of ASCII codes (*e.g.* using the command `man ascii`)
we can see that `41` is the hexadecimal encoding of the ASCII value for `A`,
`42` is the hexadecimal encoding of the ASCII value for `B`, `4e` is the hexadecimal
encoding of the ASCII value for `N`, and `0a` is the hexadecimal encoding of the
ASCII value for newline ("line feed" or LF in an ASCII table, `\n` in C code).
Alternatively, we can get the `od` program to show the characters that are
encoded as follows:

```
$ od -c -t x1 rsrc/banana.txt
0000000   B   A   N   A   N   A  \n
         42  41  4e  41  4e  41  0a
0000007
```

We can run our data compression program `mtft` so that its standard input has
been "redirected" to the file `banana.txt`:

```
$ bin/mtft -e < banana.txt
```

If the program uses the `getchar()` function to read from `stdin`, the following
sequence of integer values (expressed this time in decimal) will be returned:
`66`, `65`, `78`, `65` `78`, `65`, `10`, `-1`.
The decimal numeral `66` represents the same integer value as the hexadecimal numeral `42`;
that is, the ASCII encoding for `B`.  The decimal numeral `10` represents the same
integer value as the hexadecimal numeral `0a`; that is, the ASCII encoding for
newline.  The integer value `-1` does not represent any character -- it is returned
by `getchar()` to indicate that the end of the input has been reached.

Next, let's consider what happens when our program applies the move-to-front transform
to this data.
We need to make some assumption about the initial ranks of each of the 256 symbol
values that can be represented by an 8-bit byte.
We will assume (and our program will perform its initialization accordingly) that the
initial rank of a symbol is equal to its integer value plus 256 (the size of the alphabet).
The reasons for this (admittedly somewhat strange) assumption will be explained in more
detail later, but for the moment note that it means that the output alphabet for the
move-to-front transform (as we will implement it) will be twice the size of the input alphabet;
in this case 512, rather than 256.
Output symbols in the range [256, 511] will be produced in response to input symbols
that have not yet been seen, so that they still have their initial ranks.
Once we have seen a symbol and output its current rank, the ranks of the symbols are
adjusted by moving the just-seen symbol to rank 0 and (if necessary) shifting the ranks
other already-seen symbols up by one.

When we read the first symbol, `66`, it is the only symbol that has been seen so far,
so we output its initial rank, which is `322` (*i.e.* 66 + 256).
Then we "move this symbol to the front" and assign it rank `0`.  Since it is the only
symbol that has been seen so far, we don't need to make any change to the ranks of any
other symbols, which retain their initial ranks.
The next symbol read is `65`.  That symbol has also not yet been seen, so we output
its initial rank, which is `321`.  We then move this symbol to the front and give it
rank 0.  However, this time it is necessary to shift the symbol `66` up by one rank
from rank 0 to rank 1, in order to make room for the just-seen symbol `65`.
Next, we read the symbol `78`, which has also not yet been seen, so we output its
initial rank `334`, set `78` to have rank 0 and move the symbols `65` and `66`
up by one rank so that their new ranks are 1 and 2, respectively.
Next, we read the symbol `65`.  This symbol has already been seen, and it currently
has rank 1, so we output `1`.  The symbol `65` is moved back to rank 0, and the symbols
`78` and `66` are moved up to ranks 1 and 2, respectively.
Next, we read the symbol `78` again.  Its current rank is 1, so that is what we output,
and again we move `78` to the front so that symbols `78`, `65`, and `66` now have
ranks 0, 1, and 2, respectively.
We then read symbol `65` (the last `A`) and output 1 since the current rank of this
symbol is 1.  The symbol `65` is moved to rank 0 and the ranks of the other symbols
are adjusted as before.
Finally, we read the symbol `10` (which encodes the newline).  This symbol has not
yet been seen, so we output its initial rank `266` and move it to rank `0`,
making the required adjustments to the ranks of the other symbols we have already seen.
The symbols `10`, `65`, `78`, `66` have final ranks 0, 1, 2, 3, respectively,
and all other symbols have their initial ranks because they did not occur in the input.

To summarize what has occurred so far: applying the move-to-front transform to the
sequence `66`, `65`, `78`, `65` `78`, `65`, `10` of symbols (from an input alphabet
of size 256) has resulted in the transformed sequence of symbols
`322`, `321`, `334`, `1`, `1`, `1`, `266` (using an output alphabet of size 512).
Notice how recently seen symbols end up having small ranks and therefore produce
small values in the output.

> :nerd: The move-to-front transform is very easy to implement if we maintain the
> rank information as an array that maps ranks to symbols.  To find the current rank
> of a symbol, we scan the array looking for the desired symbol.  If the symbol is
> in the array, it has been seen before and its index is its current rank, otherwise
> the symbol has not yet been seen and its current rank is the default initial rank.
> To "move a symbol to the front", if the symbol has not yet been seen then we put it
> at index zero in the array, after shifting all other symbols up by one slot to make
> room for it.  If the symbol has already been seen, the operation is similar,
> except that we only shift the symbols whose ranks are less than the current rank
> of the symbol being moved to the front.
> If the alphabet (and hence the array) is not very large, then generally the amount
> of scanning and shifting we will have to do is small.  This is helped by the fact
> that we will more likely be scanning for symbols that have been seen recently
> and which are therefore likely to be near the front of the array.
> If the alphabet is large, however, there could be a substantial amount of scanning
> and shifting required with a simple implementation like this, so we will want to
> consider how we can do better.  (We will have more to say about this later on.)

We next apply Fibonacci coding.  From the description of Fibonacci coding given
above, you may verify that the output symbols have the Fibonacci code words
shown below.  It is important to note that we have added one to the rank of each
symbol before calculating the Fibonacci code, because Fibonacci codes apply only
to positive integers, whereas we have numbered ranks starting at 0.
Thus, the rank `1` is treated as the integer value 2, which gives Fibonacci code
word `011` (the `01` indicates that the Zeckendorf sum contains a 2 but not a 1,
and the final `1` is appended to terminate the code word).

   * (`322`) => 1000000001011
   * (`321`) => 0000000001011
   * (`334`) => 0000010001011
   * (`1`)   => 011
   * (`266`) => 0000000100011

Concatenating these code words (note that `011` occurs three times in succession)
results in the following sequence of bits:

```
1000000001011000000000101100000100010110110110110000000100011
```

Notice how the end of each code word can be unambiguously identified during
a scan from left to right: we just look for the first occurrence of two
consecutive ones after the start of each code word.

We now block this sequence of bits into 8-bit bytes, after first "padding" it
with `0` bits to reach the next multiple of 8 if the length of the sequence
is not already a multiple of 8:

```
10000000 01011000 00000010 11000001 00010110 11011011 00000001 00011000
```

We treat each of the 8-blocks as the binary representation of an integer value
(given most significant bit first) and we obtain the following sequence of bytes
(shown in hexadecimal):

```
80 58 02 c1 16 db 01 18
```

These are the final output bytes.  The following C code would output this
sequence:

```
putchar(0x80); putchar(0x58); putchar(0x02); putchar(0xc1);
putchar(0x16); putchar(0xdb); putchar(0x01); putchar(0x18);
```

Notice that we haven't actually achieved any compression in this example,
because whereas the input was only 7 bytes in length, the output is now 8 bytes
long.  However, this is the result of the fact that most of the symbols
occurred only for the first time in the sequence, so they had large ranks
and produced long Fibonacci code words.  For a longer input, it will almost
always be the case that a symbol has already been seen.

To invert the transformation and recover the input, we read byte-by-byte,
concatenating to recover the bit sequence:

```
1000000001011000000000101100000100010110110110110000000100011000
```

By looking for consecutive `1` bits, we can extract the sequence of Fibonacci
code words, discarding the padding bits that were added to the end to reach
a multiple of 8:

```
1000000001011 0000000001011 0000010001011 011 011 011 0000000100011
```

We invert the Fibonacci code and subtract one from the resulting value
(to compensate for the one that was added during coding to ensure positive
values), and obtain the original sequence of ranks (shown here in decimal):

```
`322`, `321`, `334`, `1`, `1`, `1`, `266`
```

Next, we invert the move-to-front transform.  This can be done by maintaining
the rank information the exact same way as during encoding, except that the
ranks are assigned to the output (*i.e.* decoded) symbols, rather than the
input symbols.  Specifically, we recognize `322` as the initial rank assigned
to symbol `66`, so we output `66` and re-assign it rank 0.  Similarly, we
recognize `321` and `334` as the initial ranks assigned to symbols `65`
and `78`, so we output those symbols and adjust the ranks by "moving to the front"
each symbol that has just been decoded.  When we reach the first `1`,
we look up the symbol that currently has rank `1` and we find that it is
the symbol with value `65`, so we output that value and move it to the front.
 Continuing in this way, we recover the original sequence of symbols:

```
`66`, `65`, `78`, `65`, `78`, `65`, `10`
```

# Part 3: Initial Implementation

The header file `mtft.h` lists prototypes for several functions you are
required to implement:

```c
int validargs(int argc, char **argv);
int mtf_encode(void);
int mtf_decode(void);
CODE mtf_map_encode(SYMBOL s);
SYMBOL mtf_map_decode(CODE c);
```

The `validargs()` function has already been discussed above.

The `mtf_encode()` function reads uncompressed data from `stdin` and writes
compressed data to `stdout`.
The `mtf_decode()` function reads compressed data from `stdin` and writes
uncompressed data to `stdout`.
More detailed specifications for the required behavior of these functions
are given in the comments preceding the (non-functional) stubs in `mtft.c`.

The `mtf_map_encode()` and `mtf_map_decode()` functions are used to perform
encoding and decoding of individual symbols according to the "move-to-front"
transform.  These functions (which are to be called from `mtf_encode()` and
`mtf_decode()`, respectively) must be implemented separately according to the
specifications given with the stubs in `mtf_map.c`, because some of the grading
tests will likely be unit tests designed to test these functions in isolation
from the rest of the program.
Note that no separate function has been specified for initializing the state
of the "move-to-front" heuristic, and a client of the `mtf_map_encode()`
and `mtf_map_decode()` functions should not be required to call any such
initialiation function.  Instead, any required initialization should be performed
from within the `mtf_map_encode()` or `mtf_map_decode()` function the first
time it is called.

A good place to start the assignment might be with the `mtf_encode()`
function: fill in an implementation that reads `stdin` in a byte-by-byte fashion,
calls the `mtf_map_encode()` function to perform the move-to_front encoding,
calls other functions (at your discretion) to perform Fibonacci coding,
and writes the resulting bytes of data to `stdout`.  Be forewarned that proper
modularization of this functionality will make the difference between a very
complex program that you will not be able to understand or debug and a much simpler
and more understandable program that you will be able to develop and test one piece
at a time.  In particular, you would be well-advised to invent some useful
auxiliary functions, rather than putting everything in `mtf_encode()`
and `mtf_decode()`.

Once you have verified that you are able to read input, perform Fibonacci
encoding, and write output, you could consider implementing the `mtf_map_encode()`
function.  As already discussed, a very straightforward implementation that
is adequate for relatively small alphabets (for example, with 256 symbols)
would be to maintain an array that maps a rank to the symbol currently having
that rank.

> :scream: As you are prohibited from declaring your own arrays in this
> assignment, you will have to use one that we have already declared for you.
> The `symbol_rank` array declared in `mtft.h` can be used for this purpose
> in a preliminary implementation.

As already discussed, when a symbol is read, the array can be scanned to
find the corresponding rank.  The move-to-front operation is performed by
shifting some or all of the symbols in the array up by one slot and putting
the most-recently read symbol in at index 0.

At this point, you should be able to reproduce the encoded versions of the
various sample files that have been provided for you (in the `rsrc` directory).
It should then not be very difficult to go on and implement the `mtf_decode()`
and `mtf_map_decode()` functions using the same array as an underlying data
structure.

# Part 4: A More Sophisticated Implementation

Although an implementation using a simple array as the data structure underlying
the "move-to-front" heuristic will work fine for relatively small alphabets,
for larger alphabets the amount of time spent scanning the array and shifting
entries around will likely become significant.
The array required for an alphabet that uses one-byte symbols has only 256 entries,
but for two-byte symbols we need 65536 entries.  Even if frequently occurring symbols
end up closer to the front of an array with 16 million entries, on the average many
hundreds of entries might have to be shifted during a move-to-front operation on such
an array.
So, we would like to come up with a scheme that is more efficient for large
alphabets.
Also, the array-based implementation will not have given you much experience in using
C language features such as pointers to structures, so that is another consideration
from the point of view of this course.

So, in this part of the assignment you will replace the array-based implementation
of the "move-to-front" heuristic with a tree-based data structure that will be
more efficient for large alphabets.  In particular, the cost of updating the tree-based
data structure as each symbol is encoded or decoded will only increase logarithmically
in the size of the alphabet, rather than linearly as is the case for the array-based
implementation.  In addition, we will be able to arrange to store in the tree only
information about symbols that we have actually seen, rather than all symbols.
This means that a large alphabet does not necessarily imply a large data structure,
if only a small subset of the possible symbols are actually used.

To understand how we might make the program more efficient for large alphabets,
let us think about the basic operation that we have to do each time a symbol is
encoded or decoded.  On encoding, we need the ability to map a symbol to its current
rank and to update the rank information according to the move-to-front heuristic.
On decoding, we need to be able to map a rank to the corresponding symbol, and
again to update the rank information using "move-to-front".  With an array that
maps ranks to symbols, during decoding we can look up a symbol given its rank
very quickly (*i.e.* in constant time), however during encoding we have to scan
the array (*i.e.* taking worst-case time linear in the size of the array) to find
the rank of a symbol.  In situations like this, it can sometime help to maintain *two*
mappings that are inverse to each other: here we would want one array to map map symbols
to ranks and one to map ranks to symbols, but updating these arrays via "move-to-front"
still ends up being a worst-case linear operation.

While thinking about this issue during the creation of this assignment, I hit a
bit of a dead end and went looking around on the Web for any interesting ideas.
I found [the following thread](https://encode.su/threads/546-Move-to-Front-Implementation)
in which one of the contributors (Shelwien) suggests a method that uses an
(unbalanced) ordered binary tree to store symbols indexed by the
*offsets in the input stream* at which they were most recently encountered.
Note that the most-recently read symbol will always have the greatest offset,
the next-most-recently read symbol will have the next-greatest offset, and so on,
down to the least-most-recently read symbol, which will have the least offset.
The key insight is that we can perform the "move-to-front" update in a tree that
indexes symbols by their offsets in this way by simply removing the just-read symbol
from its current position and re-inserting it at the current offset -- unlike the
array implementation we don't have to do anything with any of the other symbols.
How is this possible?
If we use the bits in the binary representation of the offset to determine the
path from the root to the place where a symbol will be stored (*e.g.* a `0`
bit means "go left" and a `1` bit means "go right"), and we store the symbols
themselves at the leaves of the tree, then the rank of a symbol is precisely
the number of symbols *to the right of that symbol in the tree*.
If we can efficiently count the number of symbols to the right of a given symbol,
then we have solved the problem!

With a little bit of thought, you can see that if we maintain, besides the left and
right child pointers at each node of the tree, counts of the number of leaves
in the left and right subtrees headed by each child, then we can use these counts
to determine the number of leaves to the right of a given leaf.
We simply walk our way from the given leaf back to the root of the tree,
adding up the number of nodes in subtrees to the right as we go.  This produces
a count of the number of leaves to the right of a given leaf while visiting only
the nodes in a path from the given leaf to the root.  The length of such paths
will be independent of the size of the alphabet and will grow at most logarithmically
with the offset in the unencoded data stream, which is pretty good.

There are a few "gotchas" that we have to worry about in order to make this all work.
First of all, we need to be able to trace a path from any given leaf in the tree
to the root.  We can easily accomplish this if we have a parent pointer at each
node in the tree, in addition to the left and right child pointers.

> :nerd: With clever coding, we could probably eliminate the need for the parent pointers
> and save some space, but it doesn't advance the purposes of this assignment any
> more by going crazy over this.

Secondly, we have to use a fixed-depth tree in which the symbols are stored only at
the leaves -- with other schemes either it becomes inefficient to count the number of
leaves to the right or it becomes inefficient to perform the "move-to-front" operation.
So it at first seems like we have to choose in advance the tree depth, and hence the maximum
length of a data stream that we can process.  The original proposer of this tree
idea on the thread referenced above addressed this, and suggested the idea of processing
the input in fixed-size blocks and doing some kind of re-indexing operation when the maximum
offset was reached.  However, while thinking about this, it occurred to me that it was
very straightforward to increase the depth of the tree each time the offset reached the
next power of two: all that is required is to add a new root to the tree having the
existing tree as its left child.
Although as we process longer and longer input the number of nodes we need to represent
the tree will grow in an unbounded fashion, as long as we recycle nodes that we remove
from the tree the number of nodes required will only grow logarithmically in the length
of the data stream.

A third issue concerns the initialization of the algorithm.  One way to do it is to
"prime" the tree by initially inserting each symbol at default offset, as if we had
read each of the symbols in the alphabet once before beginning to read the input.
However, such an initialization scheme would take a lot of time and would fill up the
tree with symbols that might never actually even occur in the input that we are going
to process.  What we would rather do is have some way to initially assign symbols a
default rank without having to insert them in the tree.  That way, the tree could start
out empty and would only accumulate symbols that actually occur in the input.
This is the reason for using a range of ranks that is twice as long as what is required
for the alphabet and reserving values in the upper half of the range as defaut values
for symbols that have not yet been seen.

As a very simple illustration of what we are talking about, consider the "BANANA"
example discussed above.  Carrying out the algorithm that will be sketched in
somewhat more detail later on, by the time we reach the end of the input we will
have constructed a tree that looks like this (for clarity, the letters represented,
rather than their codes have been shown at the leaves).

```
                 |
	         ---------
             |       | 
           ---     -----
           |       |   |
          --      --- --
          |       | | |
symbol:   B       N A \n
offset:   0 1 2 3 4 5 6
```

The tree has depth 3, which means that the offsets are represented using three bits.
The letter `B` was last seen at offset 0 (bits `000`), the letter `N` was last seen
at offset 4 (bits `100`), the letter `A` was last seen at offset 5 (bits `101`),
and the newline was last seen at offset 6 (bits `110`).  There used to be symbols
stored at offsets 1, 2, and 3, but these were removed (together with the internal nodes
that connected them to the root) during the "move-to-front" operation when the
symbols stored at those offsets were seen later.
Note that we are using an alphabet of size 256, but only a few of these symbols have
actually been encountered and indexed in the tree.

The following printout, which was generated by my debugging code, shows the full details
of the tree, including the node counts and memory addresses:

```
{node = 0x563892908810, left_child = 0x5638929082b0, right_child = 0x563892908840, parent = (nil), left_count = 1, right_count = 3, symbol = -1}
 {node = 0x5638929082b0, left_child = 0x5638929082e0, right_child = (nil), parent = 0x563892908810, left_count = 1, right_count = 0, symbol = -1}
  {node = 0x5638929082e0, left_child = 0x563892908310, right_child = (nil), parent = 0x5638929082b0, left_count = 1, right_count = 0, symbol = -1}
   {node = 0x563892908310, left_child = (nil), right_child = (nil), parent = 0x5638929082e0, left_count = 0, right_count = 0, symbol = 66}
 {node = 0x563892908840, left_child = 0x563892908870, right_child = 0x563892908900, parent = 0x563892908810, left_count = 2, right_count = 1, symbol = -1}
  {node = 0x563892908870, left_child = 0x5638929088a0, right_child = 0x5638929088d0, parent = 0x563892908840, left_count = 1, right_count = 1, symbol = -1}
   {node = 0x5638929088a0, left_child = (nil), right_child = (nil), parent = 0x563892908870, left_count = 0, right_count = 0, symbol = 78}
   {node = 0x5638929088d0, left_child = (nil), right_child = (nil), parent = 0x563892908870, left_count = 0, right_count = 0, symbol = 65}
  {node = 0x563892908900, left_child = 0x563892908930, right_child = (nil), parent = 0x563892908840, left_count = 1, right_count = 0, symbol = -1}
   {node = 0x563892908930, left_child = (nil), right_child = (nil), parent = 0x563892908900, left_count = 0, right_count = 0, symbol = 10}
```

Note that the `N` (symbol value `78`) currently has rank 2 (there are two symbols to its right),
and that we can calculate this rank by ascending from this node toward the root and accumulating
into a sum the `right_count` at each node for which the `N` node lies in the direction of the
left child and 0 at each node for which the `N` node lies in the direction of the right child
(the sequence of values summed in this case is therefore: 1 + 1 + 0 = 2).

So, let's get more concrete about the implementation of this scheme.
Although not strictly necessary, for clarity in the code we define separate integer types for
symbols, codes, and offsets, and some special sentinel values:

```
/*
 * Type used to represent symbols in an unencoded data stream.
 */
typedef int SYMBOL;

/* Value of type SYMBOL that does not represent a valid symbol. */
#define NO_SYMBOL ((SYMBOL)(-1))

/*
 * Type used to represent codes in an encoded data stream.
 */
typedef int CODE;

/*
 * Type used to represent an offset (numbered 0, 1, 2, ...) in a stream of symbols.
 */
typedef long OFFSET;

/* Value of type OFFSET that does not represent a valid offset. */
#define NO_OFFSET ((OFFSET)(-1))
```

Unfortunately, the type checking performed by the C compiler regards mostly
regards these integer types as interchangeable, so we will have to be careful
about the kind of values that we pass as parameters to functions and so on.

The following C structure definition (found in `mtft.h`) defines a node of the tree:

```c
typedef struct mtf_node {
    struct mtf_node *left_child;   // Pointer to left child node
    struct mtf_node *right_child;  // Pointer to right child node
    struct mtf_node *parent;       // Pointer to parent node
    int left_count;                // Number of symbols stored below left child
    int right_count;               // Number of symbols stored below right child
    SYMBOL symbol;                 // Symbol stored at this node (only at a leaf)
} MTF_NODE;
```

> :nerd:  Instead of storing a separate `left_count` and `right_count` at each
> node of the tree, we could probably save space by storing just one count,
> using the observation that we would be able to reconstruct the separate
> counts for a parent node by looking at the counts for the child nodes and
> doing some subtraction.  But this is a little more complicated and once again
> it doesn't advance the purposes of this assignment to be too obsessive here.

> :nerd:  If you are really on top of things, you might also have observed that
> since symbols are only stored at leaves, and leaf nodes have no children,
> that we are actually wasting space by having a separate symbol field in every
> node.  By some trickery using C union types, we could arrange for the symbol
> field at a leaf node to use the space occupied by the child pointers at an internal
> node, but again, this would add unnecessary complexity to the present assignment.

We need a variable to point to the root of the tree:

```c
/*
 * Root node of the "MTF map", which is used to map between codes and symbols.
 *
 * You *must* make sure that this variable points to your map,
 * because some of our tests will check its contents for correctness.
 */
MTF_NODE *mtf_map;
```

We need a variable to keep track of the current offset in the unencoded data stream.
The logarithm to the base 2 of this value will be the current depth of the tree,
so each time this variable reaches a power of two we have to increase the depth
by adding a new root node.

```c
/*
 * The current offset in the sequence of symbols being encoded or decoded.
 * This variable starts from zero and is incremented each time a symbol is
 * read (during encoding) or each time a symbol is written (during decoding).
 *
 * You *must* make sure that this variable is correctly updated, because
 * some of our tests will look at it.
 */
OFFSET current_offset;
```

We will need a way to determine which symbols are currently in the tree
(*i.e.* have already occurred in the input) without having to traverse the
entire tree looking for them.  The following array is used to map symbol
values to the offsets at which they were most recently seen (or the special
value NO_SYMBOL if they were not yet seen).  Note that this not only tells
us which symbols are currently in the tree, but tells us where they are so
that we can remove and re-insert them.

```
/*
 * Array used to map each symbol to the last offset at which it occurred
 * in the unencoded data stream, or to NO_OFFSET if it has not yet occurred.
 *
 * You *must* use this array to store this mapping, because you are not
 * permitted to declare your own arrays.
 */
OFFSET last_offset[SYMBOL_MAX];
```

Finally, we need a source of `NODE` structures for building the tree.
Typically, the C general-purpose storage-management functions `malloc()`
and `free()` would be used to allocate and free nodes as needed, but we are
not going to use these functions in this assignment.  Instead, we will provide
a fixed pool of pre-allocated nodes (as an array) and implement a scheme for
recycling nodes that were previously removed from the tree.

```c
/*
 * Maximum supported size of the "alphabet" of symbols.
 * Valid symbol values are in the range [0, SYMBOL_MAX-1].
 */
#define SYMBOL_MAX (1 << 16)
```

```c
/*
 * Pool of nodes to use in constructing the MTF map.
 *
 * You *must* use the entries of this array for the nodes in your map, because
 * you are not permitted to use malloc() or to declare any arrays yourself.
 */
#define MAX_NODES (3 * SYMBOL_MAX)
MTF_NODE node_pool[MAX_NODES];

/*
 * Index of the first unused node in the node pool.
 * To allocate a node from the pool, obtain a pointer to the node at this index
 * and then increment the index by one.
 */
int first_unused_node_index;
```

If we simply consume nodes from the node pool and never worry about recycling them,
we will need at least one node for each symbol in the input stream, which is
very extravagant.  Instead, we will maintain a "recycling list" where we can put
nodes removed from the tree.  When we need a node, we first use a recycled node
if there is one, otherwise we will allocate a node from the node pool.

```c
/*
 * Head of a list of "recycled" nodes that are currently available for use.
 * This variable is either NULL, which represents an empty list, or it points to
 * the first of a sequence of MTF_NODE structures arranged as a singly linked list
 * using the left_child field as the "next" pointer.
 *
 * Whenever a node is removed from your MTF map, it should be linked at the head
 * of this list for possible later re-use.
 *
 * Whenever a node is needed for the tree, if there are any nodes in this list,
 * then the first one should be removed and used.  Otherwise, if this list is empty,
 * then the first unused node in the node pool should be used, and the
 * first_unused_node_index variable incremented.
 */
MTF_NODE *recycled_node_list;  // NULL if the list is empty.
```

## Implementing the Operations

Let's briefly walk through how the data structures above are to be used to
implement the `mtf_map_encode()` and `mtf_map_decode()` operations.
The actual coding is, of course, your responsibility, but you might find it
helpful to have a sketch of what needs to be done.

When `mtf_map_encode()` is called, it receives a symbol.  The symbol is looked
up in the `last_offset` array to find the last offset at which it was seen,
or `NO_SYMBOL` if the symbol has not yet been seen.
The value of the `current_offset` variable records the current offset in the
input stream.  It will be updated before returning from `mtf_map_encode()`.
The logarithm to the base two (rounded up) of `current_offset` determines the number
of bits currently required to represent an offset.  This is also the depth
of the tree; *i.e.* the number of edges in the path from the root to a leaf.
Each time `current_offset` reaches the next power of two, the depth of the
tree will need to be increased by one.  This is done by allocating a new root
node, setting the left child pointer of the new root node to point to the old
root, and updating the other fields appropriately.

Assuming the tree is now the proper depth, we have to use the offset to
find the leaf node in the tree indexed at that offset
We do this by descending the tree from root to leaf, using the bits of
the offset obtained from the `last_offset` array, from most-significant
to least-significant, to determine which direction to take (`0` for left,
`1` for right) at each node. 
Once the leaf node is reached, we need to determine its rank.
This is done by ascending back to the root, using the `right_count` fields
to count the total number of leaves to the right of the current one.
The rank computed in this way will become the value returned by `mtf_map_encode()`.

Once having computed the rank, the symbol needs to be removed from the tree
and re-indexed at `current_offset`.  The removal can either be done while
ascending the tree during the computation of the rank or as a separate operation.
The leaf node and any other nodes removed from the tree are added to the
recycle list.  Of course, all the pointers and counts have to be properly
updated to keep the tree consistent.

Next, we re-index the removed symbol.  This can be done by descending the
tree from root to leaf, this time using the bits of `current_offset` to
determine the path.  If null pointers are encountered where nodes need
to be, new nodes are taken from the recyle list, or as a last resort,
from the node pool, to be inserted into the tree.  Again, all pointers
and counts have to be properly updated.

Once the current symbol has been removed and re-indexed, its entry in the
`last_offset` array has to be updated to `current_offset`.
Finally, the `current_offset` variable is incremented, in preparation for
the next call to `mtf_map_encode()`.  The rank that was computed earlier is returned.

When `mtf_map_decode()` is called, it is passed a rank, but the leaves of the
tree are indexed by offsets.  However, it is possible to look up a leaf in the
tree by its rank, using the `left_count` and `right_count` fields and the fact
that the rank of a leaf is equal to the number of leaves to its right.
So, we can find the leaf with the specified rank by descending the tree from root
to leaf, using the `left_count` and `right_count` fields to determine the path.
The `symbol` field of the leaf tells us the symbol having that rank, and this
will become the return value of `mtf_map_decode()`.

Once having found the symbol, we can perform the "move-to-front" operation.
This is done just as for `mtf_map_encode()`: the leaf node is removed from its
current position, the symbol is re-indexed using `current_offset`, and then the
`last_offset` array and `current_offset` are updated.  Finally, `mtf_map_decode()`
returns the symbol value as its result.

## Coding Strategy

Although the description sounds involved, the number of lines of code required
to implement it is very modest.  However, there are lots of things that can
go wrong.  If you want to be able to successfully write and debug this code,
there are a few things that I strongly suggest you do:

- As you code, write some extra code to help you debug.  You may feel like
  you are wasting time writing extra code, but you will actually be saving
  time during the debugging process.

  One example would be a function that prints out the content of the tree in
  format that you can readily understand.

  Yet another example would be to include debugging printout that gives you
  a "running commentary" on what your program is doing.  Here is a rough example
  from my "proof of concept" implementation of the program:
	  
```
$ bin/mtft -e < rsrc/banana.txt
'B' {322} -> 10000000 [emit 80] 01011
'A' {321} -> 000 [emit 58] 00000010 [emit 02] 11
'N' {334} -> 000001 [emit c1] 0001011
'A' {1} -> 0 [emit 16] 11
'N' {1} -> 011
'A' {1} -> 011 [emit db] 
'\012' {266} -> 00000001 [emit 01] 00011
flush 5 bits (3)
emit 18
'\012' {0} 'A' {1} 'B' {3} 'N' {2} 
```

  Make sure that the debugging printout is removed or disabled in the "production"
  version of your program.  One way to do this is to use the macros from `debug.h`,
  which only produce printout when the program has been compiled for debugging.

  Another example of something that is very useful during debugging is
  a function that traverses the entire tree starting from the root and
  "sanity checks" the tree for every consistency property that you can think of.
  For example, check things like that the parent pointer at a child node points
  back to its parent, that nodes with symbols have null child pointers, and that
  the values in the node count fields contain value that match the actual structure
  of the tree.  You can call this function every time you do an insertion or deletion
  from the tree, to check that you haven't messed anything up.  Later, when you
  are confident that everything is working, you can comment out the calls
  to this function.

- Be sure that you initialize every field of a node.  Fields left uninitialized
  can result in dangling pointers, looping, or other unpredictable behavior.

- Test thoroughly, starting with small amounts of data and individual operations.
  If you start right off and try to run a large example, it will not work and you
  will not be able to understand why in finite time.

# Part 5: Running the Completed Program

In either of its operating modes, `mtft` reads from `stdin` and writes
to `stdout`.  Since the data output by the program is "raw binary",
it will not be useful to output it directly to the terminal -- you will
you will just see "garbage characters".
Instead, the program will generally be run using **input and output redirection**,
in which it is arranged (via the shell) for the program to take its input
either from a file or from another program, and to direct its output either
to a file or to another program.
A simple example of a command that uses such redirection is the following:

```
$ bin/mtft -e < rsrc/banana.txt > banana.out
```

This will cause the input to the program to be redirected from the text file
`rsrc/banana.txt` and the output from the program to be redirected to the file `banana.out`.
For debugging purposes, the contents of these files can be viewed using
the `od` ("octal dump") command as has already been discussed.
The redirection is accomplished by the shell, which interprets the `<` symbol
to mean "input redirection from a file" and the `>` symbol to mean
"output redirection to a file".  It is important to understand that redirection
is handled by the shell and that the `bin/mtft` program never sees any
of the redirection arguments; in the above example it sees only `bin/mtft -e`
and it just reads from `stdin` and writes to `stdout`.

Alternatively, the output from the above command could be **piped**
directly to the `od` program, without the use of a disk file.
This could be done, for example, by the following command:

```
$ bin/mtft -e < rsrc/banana.txt | od -t x1
```

If you try this on a longer input file (after implementing the required functionality),
the output will be fairly long and will probably scroll out of the terminal window.
To avoid this, you can use another pipe to direct the output to a program called `less`:

```
$ bin/mtft -e < rsrc/gettysburg.txt | od -bc | less
```

This will display only the first screenful of the output and give you the
ability to scan forward and backward to see different parts of the output.
Type `h` at the `less` prompt to get help information on what you can do
with it.  Type `q` at the prompt to exit `less`.

Programs that read from standard input and write to standard output are
often used as components in more complex "pipelines" that perform multiple
transformations on data.

For example,
one way to test your implementation is by using one instance of it
to compress some data and then piping the output to another instance to
uncompress it; *e.g.*:

```
$ cat rsrc/banana.txt | bin/mtft -e | bin/mtft -d | od -c
```

Here `cat` (short for "concatenate") is a command that reads the files
specified as arguments, concatenates their contents, and prints the
concatenated content to `stdout`.  In the above command, this output
is redirected through a pipe to become the input to `bin/mtft -e`.

If `bin/mtft` has been correctly implemented, the compression performed by
`bin/mtft -e` should be inverted by `bin/mtft -d` and you should recover
the original content of `banana.txt`.  The `-c` argument to `od` tells
it to show each character in an unambiguous way so that you can not only
see the printable characters, but also the newline and any other "junk"
that might happen to have crept in.

Alternatively, you could redirect the output of the above pipeline to a file:

```
$ cat rsrc/banana.txt | bin/mtft -e | bin/mtft -d > out.txt
```

You could of course view the contents of `out.txt` by printing it on the
terminal, but there might be subtle differences that you don't readily see.
The `diff` command (use `man diff` to read the manual page) is useful for comparison
of text files, but not particularly useful for comparing binary files.
However the `cmp` command can be used to perform a byte-by-byte comparison of two files,
regardless of their content:

```
$ cmp rsrc/banana.txt out.txt
```

If the files have identical content, `cmp` exits silently.
If one file is shorter than the other, but the content is otherwise identical,
`cmp` will report that it has reached `EOF` on the shorter file.
Finally, if the files disagree at some point, `cmp` will report the
offset of the first byte at which the files disagree.
If the `-l` flag is given, `cmp` will report all disagreements between the
two files.

The BASH shell provides a still more sophisticated form of redirection that
can run the above transformation and test whether the result is equal to
what you started with, all in one gulp:

```
$ cmp -l <(cat rsrc/banana.txt) <(cat rsrc/banana.txt | bin/mtft -e | bin/mtft -d)
```

This compares the original file `rsrc/banana.txt` with the result of taking that file,
compressing it, and then decompressing it.
If the result is identical to the original file, `cmp` will output nothing.

> :nerd: `<(...)` is known as **process substitution**. It is allows the output of the
> program(s) inside the parentheses to appear as a file for the outer program.

> :nerd: `cat` is a command that outputs a file to `stdout`.

## Unit Testing

Unit testing is a part of the development process in which small testable
sections of a program (units) are tested individually to ensure that they are
all functioning properly. This is a very common practice in industry and is
often a requested skill by companies hiring graduates.

> :nerd: Some developers consider testing to be so important that they use a
> work flow called **test driven development**. In TDD, requirements are turned into
> failing unit tests. The goal is then to write code to make these tests pass.

This semester, we will be using a C unit testing framework called
[Criterion](https://github.com/Snaipe/Criterion), which will give you some
exposure to unit testing. We have provided a basic set of test cases for this
assignment.

The provided tests are in the `tests/basecode_tests.c` file. These tests do the
following:

- `validargs_help_test` ensures that `validargs` sets the help bit
correctly when the `-h` flag is passed in.

- `validargs_encode_test` ensures that `validargs` sets the encode-mode bit
correctly when the `-e` flag is passed.

- `validargs_decode_test` ensures that `validargs` sets the decode-mode bit
correctly when the `-d` flag is passed in.

- `validargs_bits_test` ensures that `validargs` sets the decode-mode bit
correctly when the `-d` flag is passed in and that the value passed with `-b`
is correctly stored in the least-signficant byte of `global_options`.

- `validargs_error_test` ensures that `validargs` returns an error when no
parameter is supplied with the `-b` flag.

- `help_system_test` uses the `system` syscall to execute your program through
Bash and checks to see that your program returns with `EXIT_SUCCESS`.

- `mtft_basic_test` performs a basic test of the encoding mode of the program.

### Compiling and Running Tests

When you compile your program with `make`, a `mtft_tests` executable will be
created in your `bin` directory alongside the `mtft` executable. Running this
executable from the `hw1` directory with the command `bin/mtft_tests` will run
the unit tests described above and print the test outputs to `stdout`. To obtain
more information about each test run, you can use the verbose print option:
`bin/mtft_tests --verbose=0`.

The tests we have provided are very minimal and are meant as a starting point
for you to learn about Criterion, not to fully test your homework. You may write
your own additional tests in `tests/basecode_tests.c`, or in additional source
files in the `tests` directory.  However, this is not required for this assignment.
Criterion documentation for writing your own tests can be
found [here](http://criterion.readthedocs.io/en/master/).

Note that grades are assigned based on the number of our own test cases
(not given to you in advance) that your program passes.
So you should work on the assignments in such a way that whatever you do submit
will function.  Code that is completely broken will not score any points,
regardless of how voluminous it might be or how long you might have spent on it.

### Test Files

In the `rsrc` directory I have provided several files that you might find useful
in testing your code.  They are:

- `banana.txt`, `banana.out`  The uncompressed and compressed files corresponding
to the running example used in this document.

- `gettysburg.txt`, `gettysburg.out`  A somewhat longer test case; the input is
still a text file.

- `bison`, `bison.out`  A rather longer, binary file, in uncompressed and compressed
form.

# Hand-in instructions

**TEST YOUR PROGRAM VIGOROUSLY BEFORE SUBMISSION!**

Make sure that you have implemented all the required functions specifed in `mtft.h`.

Make sure that you have adhered to the restrictions (no array brackets, no prohibited
header files, no modifications to files that say "DO NOT MODIFY" at the beginning,
no functions other than `main()` in `main.c`) set out in this assignment document.

Make sure your directory tree looks basically like it did when you started
(there could possibly be additional files that you added, but the original organization
should be maintained) and that your homework compiles (you should be sure to try compiling
with both `make clean all` and `make clean debug` because there are certain errors that can
occur one way but not the other).

This homework's tag is: `hw1`

`$ git submit hw1`

> :nerd: When writing your program try to comment as much as possible. Try to
> stay consistent with your formatting. It is much easier for your TA and the
> professor to help you if we can figure out what your code does quickly!

