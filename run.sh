#!/bin/bash
qemu-system-i386 -L /usr/lib/x86_64-linux-gnu/ \
-drive format=raw,file=os_image.bin \
-serial stdio \
-d cpu_reset,int,guest_errors