#include "interrupts.h"
#include "idt.h"
#include "pic.h"

// Prototyp vor der _start-Funktion deklarieren
void main(void);

// Kernel-Entry-Point
void __attribute__((noreturn)) _start(void) {
    main();
    while(1) asm("hlt");
}

// Hauptfunktion
void main(void) {
    volatile char *vga = (volatile char*)0xB8000;
    vga[0] = 'O';
    vga[1] = 0x0F;  // Weißer Text

    // Interrupt-System initialisieren
    init_idt();
    pic_remap();
    init_timer();
    idt_enable_irq1();  // Neu!
    pic_enable_keyboard();  // Neu!

    // Interrupts aktivieren
    asm volatile("sti");

    // Hauptschleife mit Tick-Anzeige
    while(1) {
        if (ticks % 100 == 0) {
            vga[2] = '0' + (ticks / 100 % 10);
        }
    }
}