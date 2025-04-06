CC = gcc
LD = ld
ASM = nasm

ASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdinc -I.
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

BOOTLOADER = boot/boot.bin
C_SOURCES = kernel/nucleus.c drivers/drivers.c lib/lib.c
ASM_SOURCES = kernel/core.asm
ISO = ParaOS.iso

C_OBJS = $(C_SOURCES:.c=.o)
ASM_OBJS = $(ASM_SOURCES:.asm=.o)
OBJS = $(C_OBJS) $(ASM_OBJS)

all: os_image.bin $(ISO)

$(BOOTLOADER): boot/boot.asm
	@echo "[ASM] Compiling bootloader"
	mkdir -p boot
	nasm -f bin $< -o $@

%.o: %.asm
	@echo "[ASM] $<"
	nasm $(ASMFLAGS) $< -o $@

%.o: %.c
	@echo "[CC] $<"
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJS)
	@echo "[LD] Linking kernel..."
	$(LD) $(LDFLAGS) -o $@ $^

os_image.bin: $(BOOTLOADER) kernel.bin
	@echo "[IMG] Creating raw image..."
	dd if=/dev/zero of=$@ bs=1M count=2
	dd if=$(BOOTLOADER) of=$@ conv=notrunc
	dd if=kernel.bin of=$@ seek=1 conv=notrunc

$(ISO): kernel.bin
	@echo "[ISO] Building bootable ISO..."
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/
	echo 'menuentry "ParaOS" { multiboot /boot/kernel.bin }' > iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ iso

clean:
	rm -rf boot/*.bin kernel/*.o drivers/*.o lib/*.o *.bin iso $(ISO)

run: os_image.bin
	qemu-system-i386 -drive format=raw,file=$<

run-iso: $(ISO)
	qemu-system-i386 -cdrom $(ISO)