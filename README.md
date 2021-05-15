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

# Operations about containers
# List
docker container ls -a

# Stop
docker stop OSTEP

# Start
docker start OSTEP
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
1. (P1a) [Unix Utilities](initial-utilities) (cat, grep, zip/unzip)
* ✅ (May 12, 2021)
* Implement Unix utilities on my own, including ***cat, grep, zip/unzip***.


Kernel Hacking Projects (xv6)
---
These projects all are to be done inside the [xv6](https://pdos.csail.mit.edu/6.828/2017/xv6.html) kernel based on an early version of Unix and developed at MIT.

1. (P1b) [xv6: Introducing a new system call](initial-xv6)
* ✅ (May 15, 2021) 
* Introduce a new system call `getreadcount()`. This project is very helpful for me to understand the process of calling a syscall.