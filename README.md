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

### Parallel Zip
3. (Project 3a) [Parallel Zip](concurrency-pzip)
* ✅ (June 19, 2021) 
* In project 1a, I implemented a simple compression tool based on run-length encoding, known simply as zip.
* In project 3a, I implement something similar, except I use threads to make a parallel version of zip.
* Keywords: 
  * Multi-thread: **pthread**
  * File I/O: **open**, **mmap**, **munmap**


Kernel Hacking Projects (xv6)
---
These projects all are to be done inside the [xv6](https://pdos.csail.mit.edu/6.828/2017/xv6.html) kernel based on an early version of Unix and developed at MIT.

1. (Project 1b) [xv6: Introducing a new system call](initial-xv6)
* ✅ (May 15, 2021) 
* Introduce a new system call `getreadcount()`. This project is very helpful for me to understand the process of calling a syscall.

2. (Project 2b) [An xv6 Lottery Scheduler](scheduling-xv6-lottery/README.md)
* ✅ (June 6, 2021)
* Implement a [lottery scheduler](https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched-lottery.pdf) in xv6. To elaborate, I implement a syscall `settickets` to set the number of tickets of the calling process. The basic idea is simple: assign each running process a slice of the processor based in proportion to the number of tickets it has; the more tickets a process has, the more it runs. Each time slice, a randomized lottery determines the winner of the lottery; that winning process is the one that runs for that time slice.
  
  <img src="images/p2b_experiment.png" width="600" height="400" align="center"/>

3. (Project 3b) [Virtual Memory (Null Pointer and Read-Only Regions)](vm-xv6-intro/README.md)
* ✅ (July 11, 2021)
* **(Task1) Null-pointer Dereference:** In xv6's VM system, user code is loaded into the very first part of the address space. Thus, if you dereference a null pointer, you will not see an exception; rather, you will see whatever code is the first bit of code in the program that is running. In Task1, we should make xv6 trap and kill the process that dereferences a null pointer.
* **(Task2) Read-only Code:** In most operating systems, code is marked read-only instead of read-write. However, in xv6 this is not the case, so a buggy program could accidentally overwrite its own text. In Task2, we need to implement 2 syscalls to avoid the accidents.
  * **mprotect(void \*addr, int len):** Changes the protection bits of the page range starting at `addr` and of `len` pages to be read only. Thus, a write to this region should cause a trap (and thus kill the process) after mprotect() finishes. 
  * **munprotect(void \*addr, int len):** Sets the region back to both readable and writeable.