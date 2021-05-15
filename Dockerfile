FROM ubuntu:18.04

RUN apt-get update && \
        apt-get install -y \
        build-essential \
        gdb \
        gcc-multilib \
        gdb-multiarch \
        tmux \
        qemu

RUN adduser xv6user
USER xv6user
WORKDIR /home/xv6user/xv6/
CMD ["/bin/bash"]
