#include "io.h"

// Define PIC1_DATA as the data port for the master PIC
#define PIC1_DATA 0x21

void pic_remap(void) {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void pic_enable_keyboard() {
    outb(PIC1_DATA, 0xFD); // Only allow IRQ1 (keyboard) (Bit 1 = 0)
}