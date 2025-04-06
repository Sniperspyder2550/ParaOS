#include "interrupts.h"
#include "idt.h"
#include "pic.h"

// Declare ticks as extern
extern volatile uint32_t ticks;

// Prototype for the main function
void main(void);

// Kernel Entry Point
void __attribute__((noreturn)) _start(void) {
    main();
    while (1) asm("hlt");
}

// Main function
void main(void) {
    init_idt(); // Initialize the IDT

    volatile char *vga = (volatile char *)0xB8000;
    vga[0] = 'O';
    vga[1] = 0x0F; // White text

    // Initialize the interrupt system
    pic_remap();
    init_timer();
    idt_enable_irq1();  // Enable IRQ1 (keyboard)
    pic_enable_keyboard();  // Enable keyboard interrupts

    // Enable interrupts
    asm volatile("sti");

    // Main loop with tick display
    while (1) {
        if (ticks % 100 == 0) {
            vga[2] = '0' + (ticks / 100 % 10);
        }
    }
}