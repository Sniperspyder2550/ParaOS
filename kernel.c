#include "idt.h"
#include "timer.h"

// Declare init_timer as an external function
extern void init_timer();

// Declare pic_remap as extern
extern void pic_remap();

// Declare and initialize ticks as a volatile global variable
extern volatile uint32_t ticks;

void kernel_main(void) {
    // Your kernel's main logic here
    while (1) {
        // Halt the CPU or perform tasks
    }
}

void main() {
    // Initialize the Interrupt Descriptor Table (IDT)
    init_idt();

    // Remap the Programmable Interrupt Controller (PIC)
    pic_remap();

    // Initialize the timer
    // Call the init_timer function to initialize the timer
    init_timer();

    // Enable interrupts
    asm volatile("sti");

    // Main loop
    while (1) {
        // Check if ticks is a multiple of 50
        if (ticks % 50 == 0) {
            // Access VGA memory safely
            volatile char *vga = (volatile char*)0xB8000;
            vga[0] = '0' + (ticks / 50 % 10);
        }
    }
}
