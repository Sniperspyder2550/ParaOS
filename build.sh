#!/bin/bash

# Compile all .c files
gcc -m32 -ffreestanding -c *.c

# Assemble all .asm files
nasm -f elf32 handlers.asm -o handlers.o

# Link all object files
ld -m elf_i386 -T linker.ld -o kernel.bin *.o

# Run with QEMU
qemu-system-i386 -kernel kernel.bin