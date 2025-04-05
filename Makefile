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
    rm -f *.bin *.o *.elf