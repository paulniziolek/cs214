Authors: Nick Belov, Paul Niziolek
netids: ndb68, pcn32

HOW TO BUILD & RUN:
USING CMAKE: 
In the `mysh` directory, run `./build.sh` (or `sh build.sh`) to build the project. 
Now run `build/mysh` to run the executable. 

USING MAKE:
TODO

DESCRIPTION:
This project implements a basic shell CLI that supports piping, redirection, conditionals, builtin commands, and running external programs with support for batch input mode. 
The design of the `mysh` project is based on polymorphic `cmds` (such as pipe, redirection, condition, executable cmd, builtin cmd).
The program will execute a user's command recursively, by first parsing a user input to build a tree of the above mentioned polymorphic command types.

For example, if a command is a pipe, such as `pwd | ls`, 
the tree will consist of a `pipe cmd` as a root node, with `builtin cmd: pwd` and `exec cmd: ls` as its children.

BATCH MODE:
Running the executable followed by a filepath will run the file at a specified location in batch mode.
Batch mode will run a file until EOF is reached, `exit` command is ran, or an empty line is ran. 

INTERACTIVE MODE:
Running the executable will prompt the entry of `mysh` and run until the `exit` command. 

PIPING: 
Piping is supported on multiple levels through the use of the `|` token. 
Given `x | y`, piping will redirect x's stdout to y's stdin (assuming there is no redirection to interfere).
Note that piping is concurrent, meaning, x and y will execute at the same time on separate processes, 
and y will stop reading from x's STDOUT until x finishes running, but will process its input as it runs. 

REDIRECTION:
Redirection is supported with either the `>` or `<` token, which will change the stdin or stdout of a command respectively. 
If redirecting stdout, and a file specified doesn't exist, we will always create a new file. 

CONDITIONALS:
Conditionals can be used throughout the runtime of `mysh`, which is dependent on the exit status of the previous command. 
We use `then` and `else` tokens. If the exit status of the last command is 0 (meaning success) then a command following `then` will run.
Otherwise, the command following `else` will run.
Please note that `then`/`else` only work if a command is specified right after it, like `then x`, and not `then \n x`.

BUILTINS:
This shell program has 3 builtin programs, `cd`, `pwd`, and `which`.
`cd` will change the current working directory, given a relative, absolute, or home path. 
Examples: `cd ~/Documents`, `cd ./../mysh`, `cd /`
`pwd` will print the current working directory to stdout (or be redirected elsewhere if specified).
`which` will print the location of an external executable if found, by first searching these directories in order:
/usr/local/bin -> /usr/bin -> /bin

EXECUTABLES AND EXTERNAL PROGRAMS:
If given an executable name, such as `ls`, mysh will search the following directories in order to execute it:
/usr/local/bin -> /usr/bin -> /bin
If a path to an external program is given, mysh will run the file at the specified path. 

WILDCARDS:
If arguments are given, an argument can be a wildcard argument if `*` is in the argument (can be multiple *). 
Wildcard arguments will search for all files that match a given pattern, such as `*.c`. If no files are found, then the
wildcard argument itself is returned as an argument to the command. Otherwise, all file paths are provided as arguments to the command. 

TESTING METHODOLOGY:
These are our tests for the program:
Running `build/builtins_test` will test all builtin functions on all expected inputs, as shown in the `builtins_test.c` file. 
The output of this test will be various printed outputs from `which`, `cd`, and `pwd` commands. This ensured our builtin functions work. 

Executing multiple piped `sh concurrency_test.sh` within the `mysh` instance, such as:
`mysh> concurrency_test.sh | concurrency_test.sh | concurrency_test.sh | concurrency_test.sh | concurrency_test.sh | concurrency_test.sh`
will test the concurrency of piped commands. Each `concurrency_test.sh` has a `sleep 3`, and the whole command being executed in ~3 seconds
validates that piping supports concurrency. The program will print `hello` at first, and `wokeup` after 3 seconds. 

Finally, the validity of most functions and edge cases are tested by running a batch script `build/mysh batch.sh`, 
which covers all points mentioned in this README.

The expected output should be:
"Creating batch test file..."
/some/path/mysh
/some/path/
/bin/ls
HelloWorld
PipingOverride
PipingOverride
redirection_test
FILE_EXISTS
CMakeLists.txt  README.txt      test_output.txt
redirection_test
Finished Running

