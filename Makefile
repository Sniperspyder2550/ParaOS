# Makefile für ParaOS
QEMU = qemu-system-x86  # Nutze system-QEMU im Container
KERNEL = kernel.elf
IMAGE = os_image.bin

all: $(IMAGE)

$(IMAGE): boot.bin kernel.bin
    cat $^ > $@

boot.bin: boot.asm
    nasm -f bin $< -o $@

kernel.bin: kernel.elf
    objcopy -O binary $< $@

kernel.elf: kernel.o
    ld -m elf_i386 -T linker.ld -o $@ $^

kernel.o: kernel.c
    gcc -m32 -ffreestanding -c $< -o $@

run: $(IMAGE)
    $(QEMU) -drive format=raw,file=$(IMAGE) -serial stdio

clean:
    rm -f *.bin *.o *.elf# ParaOS Build System
QEMU = qemu-system-x86
KERNEL = kernel.elf
IMAGE = os_image.bin
CC = gcc
LD = ld
ASM = nasm

.PHONY: all run clean

all: $(IMAGE)

$(IMAGE): boot.bin kernel.bin
	@echo "[+] Linking bootloader and kernel"
	cat $^ > $@

boot.bin: boot.asm
	@echo "[ASM] $<"
	$(ASM) -f bin $< -o $@ -Wall

kernel.bin: $(KERNEL)
	@echo "[OBJCOPY] Creating flat binary"
	objcopy -O binary $< $@

$(KERNEL): kernel.o
	@echo "[LD] Linking kernel"
	$(LD) -m elf_i386 -T linker.ld -o $@ $^

kernel.o: kernel.c
	@echo "[CC] Compiling kernel"
	$(CC) -m32 -ffreestanding -c $< -o $@ -Wall -Wextra

run: $(IMAGE)
	@echo "[QEMU] Starting VM"
	$(QEMU) -drive format=raw,file=$(IMAGE) -serial stdio -d guest_errors

clean:
	@echo "[CLEAN] Removing build files"
	rm -f *.bin *.o *.elf