FROM ubuntu:22.04

# Tools installieren
RUN apt update && apt install -y \
    build-essential \
    nasm \
    qemu-system-x86 \
    gcc-multilib \
    git \
    make

# Projekt kopieren
WORKDIR /os
COPY . .

RUN apt-get install -y gcc-11-i686-linux-gnu binutils-i686-linux-gnu

RUN apt-get install -y gdb xorriso grub-pc-bin

# Standard-Befehl
CMD ["make", "run"]docker build -t paraos . 2>&1 | tee build.log