# myshell

A simple Linux shell writen in C to play with system calls.

By Robert Hongpu Ma, as a homework assignment for SFU CMPT300 (Steven Pearce).

## Features

* GNU readline (auto completion, history, line edit, etc.).
* Show current date and time as prompt.
* External program invocation (with PATH finding and arguments support).
* Change working directory ( `cd` command, with `cd` and `cd ~` HOME shortcut support).
* Correctly handle CTRL-C (SIGINT) and CTRL-D (EOF) (same behaviour as Bash).
* Deal with possible errors for system calls.

## Techniques (library, system calls, etc.)

* flex
* readline
* signal
* setjmp
* fork, execvp, waitpid
* time, localtime, strftime
* errno, perror

## How to use

```bash
make
./myshell
```
