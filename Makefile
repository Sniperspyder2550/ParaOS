# Tools
CC = gcc
LD = ld
ASM = nasm

# Flags
ASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -Wall -Wextra -I.
LDFLAGS = -m elf_i386 -T linker.ld --oformat binary

# Dateien
KERNEL_OBJS = kernel.o interrupts.o pic.o irq.o
OUTPUT = os_image.bin

.PHONY: all clean run debug

all: $(OUTPUT)

$(OUTPUT): $(KERNEL_OBJS)
	@echo "[LD] Linking kernel image"
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.asm
	@echo "[ASM] $<"
	$(ASM) $(ASMFLAGS) $< -o $@

%.o: %.c
	@echo "[CC] $<"
	$(CC) $(CFLAGS) -c $< -o $@

boot_image.bin: bootloader.asm os_image.bin
	nasm -f bin bootloader.asm -o bootloader.bin
	cat bootloader.bin os_image.bin > boot_image.bin

run: boot_image.bin
	@echo "[QEMU] Starting VM"
	qemu-system-i386 -drive format=raw,file=boot_image.bin -serial stdio -d int

debug: $(OUTPUT)
	@echo "[QEMU] Starting in debug mode"
	qemu-system-i386 -drive format=raw,file=$(OUTPUT) -serial stdio -d int -no-reboot -s -S &

clean:
	@echo "[CLEAN] Removing build files"
	rm -f *.o *.bin