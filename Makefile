CC = /path/to/i686-elf-gcc  # Replace with the actual path to your cross-compiler
LD = i686-elf-ld
CFLAGS = -DCROSS_COMPILE -ffreestanding -nostdlib -I.
ASM = nasm

ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# Source-Pfade
BOOTLOADER = boot/boot.bin
KERNEL_SOURCES = kernel/core.asm kernel/nucleus.c drivers/drivers.c lib/lib.c
OBJS = $(KERNEL_SOURCES:.c=.o) $(KERNEL_SOURCES:.asm=.o)

all: os_image.bin

# Bootloader kompilieren
$(BOOTLOADER): boot/boot.asm
	@echo "[ASM] Bootloader"
	@mkdir -p boot
	nasm -f bin $< -o $@

# Kernel kompilieren
%.o: %.asm
	@echo "[ASM] $<"
	@mkdir -p $(@D)
	nasm $(ASMFLAGS) $< -o $@

%.o: %.c
	@echo "[CC] $<"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Kernel binary erstellen
kernel.bin: $(OBJS)
	@echo "[LD] Linking kernel binary..."
	$(LD) $(LDFLAGS) -o $@ $^

# Image erstellen
os_image.bin: $(BOOTLOADER) $(OBJS)
	@echo "[LD] Linking kernel..."
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJS)
	@echo "[IMG] Merging components..."
	dd if=/dev/zero of=os_image.bin bs=512 count=2048  # 1MB Platz
	dd conv=notrunc if=boot/boot.bin of=os_image.bin 
	dd conv=notrunc if=kernel.bin of=os_image.bin seek=2048  # 1MB Offset

clean:
	rm -rf boot/*.bin kernel/*.o drivers/*.o lib/*.o *.bin

run: os_image.bin
	qemu-system-i386 -drive format=raw,file=os_image.bin

ISO = ParaOS.iso

$(ISO): kernel.bin
	@echo "[ISO] Building bootable ISO..."
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/
	echo 'menuentry "ParaOS" { multiboot /boot/kernel.bin }' > iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) iso

run-iso: $(ISO)
	@echo "[QEMU] Running ISO..."
	qemu-system-i386 -cdrom $(ISO)