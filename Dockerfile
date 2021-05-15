FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && \
        apt-get install -y \
        build-essential \
        gdb \
        gcc-multilib \
        gdb-multiarch \
        tmux \
        qemu \
        expect \
        gawk

WORKDIR /home/xv6user/xv6/
CMD ["/bin/bash"]
