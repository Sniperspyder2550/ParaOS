#include "idt.h"

// Declare pic_remap as extern
extern void pic_remap();

void main() {
    init_idt();
    pic_remap();
    asm volatile("sti");

    while (1) {

    }
}
