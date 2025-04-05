# Tools
CC = gcc
LD = ld
ASM = nasm
QEMU = $(QEMU_PATH)  # Nutzt Umgebungsvariable

# Flags
ASMFLAGS = -f bin -Wall
CFLAGS = -m32 -ffreestanding -Wall -Wextra -I.
LDFLAGS = -m elf_i386 -T linker.ld

# Dateien
OBJECTS = kernel.o interrupts.o
IMAGE = os_image.bin

.PHONY: all clean run

all: $(IMAGE)

$(IMAGE): boot.bin kernel.bin
	@echo "[LD] Linking final image"
	cat $^ > $@

boot.bin: boot.asm
	@echo "[ASM] $<"
	$(ASM) $(ASMFLAGS) $< -o $@

kernel.bin: kernel.elf
	@echo "[OBJCOPY] Creating flat binary"
	objcopy -O binary $< $@

kernel.elf: $(OBJECTS)
	@echo "[LD] Linking kernel"
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	@echo "[CC] $<"
	$(CC) $(CFLAGS) -c $< -o $@

run: $(IMAGE)
	@echo "[QEMU] Starting with portable version"
	./qemu -drive format=raw,file=$(IMAGE) -serial stdio

clean:
	@echo "[CLEAN] Removing build files"
	rm -f *.bin *.o *.elf