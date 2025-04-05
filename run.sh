#!/bin/bash
QEMU_CMD="./qemu"

if [ ! -f "os_image.bin" ]; then
    echo "Error: os_image.bin not found!"
    exit 1
fi

$QEMU_CMD \
    -drive format=raw,file=os_image.bin \
    -serial stdio \
    -d int,cpu_reset \
    -D qemu.log
