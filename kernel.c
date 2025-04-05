#include "interrupts.h"

// Prototypen
void _start();
void main();

// Kernel-Entry-Point
void _start() {
    main();
    while(1);  // Falls main() zurückkehrt
}

void main() {
    volatile char *vga = (volatile char*)0xB8000;
    vga[0] = 'O';
    vga[1] = 0x0F;  // Weißer Text

    // Interrupt-System initialisieren
    init_idt();
    pic_remap();
    init_timer();

    // Interrupts aktivieren
    asm volatile("sti");

    while(1) {
        // Hauptschleife
    }
}