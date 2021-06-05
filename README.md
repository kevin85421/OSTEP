UW-Madison CS 537: Introduction to Operating Systems
---
* This repository mainly focuses on projects for the online course ["UW-Madison CS 537: Introduction to Operating Systems, Spring 2018"](https://pages.cs.wisc.edu/~remzi/Classes/537/Spring2018/) and the textbook [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/).

* This repo is duplicated from the [skeleton repo](https://github.com/remzi-arpacidusseau/ostep-projects).

* Read [project-descriptions](project-descriptions.md) for more informations.

Development Environment
---
* Docker
```sh
# Step1: Build Docker image
./xv6-container build

# Step2: Create Docker container
docker run -v "`pwd`":/home/xv6user/xv6  --name OSTEP -itd xv6env

# Step3: Enter Docker container
docker exec -it OSTEP bash

# Step4:
# * xv6-public # (MIT version)
# * xv6-wisc   # (UW-Madison refactor version)
```
* Compile xv6
```sh
make qemu-nox
```
* Reference
  * [xv6-public](https://github.com/mit-pdos/xv6-public/tree/master)
  * [xv6-docker](https://github.com/jrodal98/xv6-docker)

C/Linux Projects
---
### Initial Projects
1. (Project 1a) [Unix Utilities](initial-utilities) (cat, grep, zip/unzip)
* ✅ (May 12, 2021)
* Implement Unix utilities on my own, including ***cat, grep, zip/unzip***.

### Processes and Scheduling
2. (Project 2a) [Command Line Interpreter (My Unix Shell)](processes-shell) 
* ✅ (May 22, 2021)
* **Basic shell:** The shell supports both interactive mode and batch mode. In interactive mode, when a user types a command, the shell will use the `fork()`, `execv()`, and `wait()` syscalls to create a child process to execute the command. When the execution finishes, the main process will print the prompt again and wait for more user commands.
* **Build-in commands:** Implement three built-in commands, including `exit`, `cd`, and `path` (search path of the shell).
* **Redirection:** With the support of redirection, a shell user can send the output of a program to a file rather than to the screen. I implement redirection with `open()` and `dup2()` syscalls. Use `open()` to get the file descriptor of the target file, and then use `dup2()` to duplicate the file descriptor of STDOUT. Example: `ls -l > output`.
* **Parallel Commands:** In this shell, users can run commands in parallel rather than starting a command only when the previous command finishes. Then, after starting all such processes, the shell uses a `wait()` loop to wait for them to complete. After all processes are done, return control to the user. Example: 
  ```
  wish> ls -l & echo 123
  ```


Kernel Hacking Projects (xv6)
---
These projects all are to be done inside the [xv6](https://pdos.csail.mit.edu/6.828/2017/xv6.html) kernel based on an early version of Unix and developed at MIT.

1. (Project 1b) [xv6: Introducing a new system call](initial-xv6)
* ✅ (May 15, 2021) 
* Introduce a new system call `getreadcount()`. This project is very helpful for me to understand the process of calling a syscall.