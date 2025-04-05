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

# Standard-Befehl
CMD ["make", "run"]