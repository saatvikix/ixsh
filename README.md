# ixsh — A Mini Shell in C

`ixsh` is a small Unix-like command shell written in C. It reads a command from the terminal, parses it, runs external system commands such as `ls`, `cat`, and `grep`, and supports selected shell features such as built-ins, redirection, and pipelines.

The aim is not to recreate Bash. Instead, this project is a compact way to understand the operating-system ideas behind a shell: processes, program execution, file descriptors, redirection, pipes for inter-process communication, and `wait()`/`waitpid()` for parent-child synchronization.

## Features

- Interactive prompt that shows the current working directory
- External command execution through the system `PATH`
- Built-in commands: `cd`, `pwd`, `help`, and `exit`
- Input redirection: `<`
- Output redirection: `>`
- Append redirection: `>>`
- Pipelines, including chained pipelines with more than two commands
- Built-ins used inside a pipeline
- Redirection combined with pipelines
- Basic detection of malformed pipelines with missing commands

## Build and run

ixsh is intended for a Linux/Unix environment with a C compiler and `make` installed.

```bash
make
./ixsh
```

To remove the compiled program:

```bash
make clean
```

The prompt will look similar to:

```text
Welcome to ixsh!
ixsh:/home/user/ixsh>
```

## Supported commands

External commands are found using the system `PATH`, so common programs such as `ls`, `cat`, `grep`, `wc`, `mkdir`, and `echo` can be run normally.

### Built-ins

| Command | Purpose |
| --- | --- |
| `cd <directory>` | Changes ixsh's current directory. |
| `pwd` | Prints the current working directory. |
| `help` | Displays the built-in command summary. |
| `exit` | Leaves the shell. |

`cd` is a built-in because changing directories in a child process would not change the directory of the shell itself. A normal external command can run in a child; `cd` must affect the parent shell process.

## Project structure

```text
ixsh/
├── include/
│   ├── builtins.h
│   ├── executor.h
│   ├── parser.h
│   └── redirection.h
├── src/
│   ├── builtins.c
│   ├── executor.c
│   ├── main.c
│   ├── parser.c
│   └── redirection.c
├── Makefile
└── README.md
```

- `main.c` contains the prompt-and-execute loop.
- `parser.c` converts input into commands and pipelines.
- `builtins.c` handles commands owned by the shell.
- `executor.c` creates processes and connects pipeline stages.
- `redirection.c` applies input and output redirection.

## How a command is handled

For each line entered by the user, ixsh follows this flow:

```text
read input → parse command/pipeline → run builtin or create child process(es) → wait → show next prompt
```

The parser splits a command into an argument array suitable for `execvp()`. For example:

```bash
grep hello notes.txt
```

is represented conceptually as:

```text
argv[0] = "grep"
argv[1] = "hello"
argv[2] = "notes.txt"
argv[3] = NULL
```

Redirection operators are stored separately from the regular arguments. Therefore, in `echo hello > out.txt`, `>` and `out.txt` are not passed to `echo`.

Pipelines are split at each `|`, then each part is parsed as an individual command. The parser is deliberately simple and whitespace-based, which keeps the project focused on the core shell mechanisms.

## External command execution

External programs use the standard Unix process model:

```text
ixsh
 ├─ parent: waits for foreground work
 └─ child: applies redirection, then runs execvp()
```

1. `fork()` creates a child process.
2. In the child, ixsh sets up any required file descriptors.
3. `execvp()` replaces that child with the requested program.
4. The parent uses `wait()`/`waitpid()` so the prompt returns after the foreground command completes.

The important distinction is: `fork()` creates a process, while `execvp()` replaces the current child process with another program.

## Redirection

Unix programs normally use these standard file descriptors:

```text
0 — standard input  (stdin)
1 — standard output (stdout)
2 — standard error  (stderr)
```

ixsh uses `open()` and `dup2()` to redirect standard input or output.

```bash
cat < input.txt          # stdin comes from input.txt
echo hello > output.txt  # stdout goes to output.txt; file is replaced
echo world >> output.txt # stdout is appended to output.txt
```

`dup2(source, destination)` makes `destination` refer to the same open file as `source`. For example, redirecting output uses `dup2(file_fd, STDOUT_FILENO)`.

External commands are redirected in their child process before `execvp()`. Built-ins need a little extra care because they normally run in ixsh itself: the shell saves its original input/output descriptors, runs the built-in with the requested redirection, then restores those descriptors. This prevents a command such as `pwd > cwd.txt` from redirecting the entire shell permanently.

## Pipes and chained pipelines

A pipe sends the output of one command to the input of another:

```bash
ls | grep src
```

Each pipe has a read end and a write end. ixsh connects the first command's stdout to the write end and the next command's stdin to the read end using `dup2()`.

Pipelines are not limited to two commands:

```bash
cat file.txt | grep hello | wc -l
```

For **N commands**, ixsh creates **N − 1 pipes**. The first command writes to a pipe, middle commands read from one pipe and write to the next, and the final command reads from the last pipe and writes normally to the terminal (or to a redirected output file).

All unused pipe descriptors are closed after setup. This is important: leaving a write end open can stop a reader from receiving end-of-file and may cause a pipeline to hang. The parent waits for the processes in the pipeline before returning to the prompt. This waiting is **synchronization**, not inter-process communication: it lets ixsh collect each child's completion status and prevents the next foreground prompt from appearing too early. The pipes are the inter-process communication mechanism because they carry bytes from one command's output to another command's input.

Built-ins can also appear in a pipeline, for example:

```bash
pwd | cat
help | grep pwd
```

When a built-in runs as part of a pipeline, it runs in the pipeline child. As a result, a state-changing command such as `cd /tmp | cat` does not change the parent ixsh directory—this is the expected process-isolation behavior.

## Example session

```text
ixsh:/home/user/ixsh> pwd
/home/user/ixsh

ixsh:/home/user/ixsh> echo hello > message.txt

ixsh:/home/user/ixsh> echo world >> message.txt

ixsh:/home/user/ixsh> cat < message.txt
hello
world

ixsh:/home/user/ixsh> cat message.txt | grep hello | wc -l
1

ixsh:/home/user/ixsh> pwd | cat
/home/user/ixsh
```

## Memory management

The parser dynamically allocates argument arrays because a command can have a variable number of arguments. The array grows when needed. Argument strings refer to the current input buffer, while the allocated argument arrays and pipeline command list are released after execution through the pipeline cleanup logic.

Keeping allocation and cleanup together is important in a long-running shell loop: each command should release its temporary parsing data before the next prompt is shown.

## Current limitations

ixsh intentionally supports a focused subset of shell behavior. It does not currently aim to provide:

- Quote-aware parsing, such as `echo "hello world"` as one argument
- Operators without spaces, such as `echo hello>file.txt`
- Background execution or job control
- `&&`, `||`, `;`, command substitution, or shell scripts
- Wildcard expansion, environment-variable expansion, command history, or tab completion
- Advanced signal handling

These are reasonable future additions, but they are outside the scope of this mini-shell.

## Concepts demonstrated

This project brings together several C and operating-system concepts:

- C strings, pointers, structures, and dynamic memory
- Modular C programs with headers and source files
- Parent and child processes
- `fork()`, `execvp()`, `wait()`, and `waitpid()`
- File descriptors and `dup2()`
- `open()` and file-creation/append modes
- Pipes as inter-process communication: one process writes data and another reads it
- `wait()`/`waitpid()` as parent-child synchronization and child-process cleanup
- Resource cleanup, including closing descriptors and freeing allocated memory

The most useful way to explain the project in a viva is with one combined command:

```bash
cat < input.txt | grep hello > result.txt
```

It shows the complete path through ixsh: parsing, input redirection, pipe creation, child processes, file-descriptor setup, external execution, output redirection, and parent-side waiting.

## Possible future improvements

- Improve parser support for quotes and operators without surrounding spaces
- Add background commands and safe cleanup of completed child processes
- Improve signal handling for an interactive terminal experience
- Add command history and tab completion
- Add shell operators such as `&&`, `||`, and `;`
- Add automated regression tests for parsing, redirection, and pipeline behavior

---

ixsh is a learning project, but its core behavior follows the same Unix principles used by larger shells: parse a command, create processes when needed, connect file descriptors correctly, and clean up resources carefully.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.