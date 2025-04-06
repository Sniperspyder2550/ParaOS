#include <io.h>
#include <vga.h>  // Annahme: print_char() existiert hier

static const char keymap[] = "\x00\x1B1234567890-=\b\tqwertyuiop[]\n\x00asdfghjkl;'`\x00\\zxcvbnm,./\x00*\x00 \x00";  
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
        if (c) { // Ignore unmapped keys
            print_char(c); // Annahme: print_char() existiert  
        }
    }  

    outb(PIC1_CMD, 0x20); // EOI für IRQ1  
}