#include "io.h"   // Use local io.h
#include "vga.h"  // Use local vga.h
#include "interrupts.h"

// Corrected string with valid hex escape sequences
const char keymap[] = "op[]\n\x00asdfghjkl;'`\x00\\zxcvbnm,./\x00*\x00 \x00";

#define PIC1_CMD 0x20  // Ensure this is correctly defined

void keyboard_handler() {  
    uint8_t scancode = inb(0x60);  

    // Ignore break codes (key release events)
    if (scancode & 0x80) {
        outb(PIC1_CMD, 0x20); // EOI für IRQ1
        return;
    }

    if (scancode < (sizeof(keymap) - 1)) {  // Fix sizeof issue
        char c = keymap[scancode];  
        if (c != '\x00') { // Ignore invalid characters
            // Updated print_char call with all required arguments
            print_char(c, 0x07, 0, 0); // Example: white text at (0, 0)
        }
    }  

    outb(PIC1_CMD, 0x20); // EOI für IRQ1  
}

void keyboard_handler_c() {
    // Keyboard interrupt handler logic
    // Process key presses or perform other tasks
}