#!/bin/bash

# Build-Script für ParaOS
echo "=== ParaOS Build-System ==="

# 1. Assemblierung
nasm -f elf32 irq.asm -o irq.o || exit 1

# 2. C-Kompilierung
gcc -m32 -ffreestanding -I. -c kernel.c -o kernel.o || exit 1
gcc -m32 -ffreestanding -I. -c pic.c -o pic.o || exit 1
gcc -m32 -ffreestanding -I. -c keyboard.c -o keyboard.o || exit 1

# 3. Linken
ld -m elf_i386 -T linker.ld -o os_image.bin *.o --oformat binary || exit 1

echo "=== Build erfolgreich ==="