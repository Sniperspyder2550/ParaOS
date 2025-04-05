#include <io.h>
#include <vga.h>  // Annahme: print_char() existiert hier

static const char keymap[] = "\x00\x1B" "1234567890-=...";

void keyboard_handler() {
    uint8_t scancode = inb(0x60);
    if (scancode < sizeof(keymap)) {
        print_char(keymap[scancode]);
    }
    outb(0x20, 0x20);  // EOI an PIC
}