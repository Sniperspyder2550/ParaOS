/* drivers.c */
#include "os.h"

volatile uint32_t ticks = 0;
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;
static uint16_t* vga_buffer = (uint16_t*)0xB8000;

// Timer handler: increment tick count and send End-Of-Interrupt.
void timer_handler() {
    ticks++;
    outb(PIC1_CMD, 0x20);
}

// Initialize the timer to 1000 Hz.
void init_timer() {
    uint32_t divisor = 1193180 / 1000;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

// Move the hardware text cursor.
void move_cursor(uint8_t x, uint8_t y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// Print a character at (x, y) in text mode.
void print_char(char c, uint8_t x, uint8_t y) {
    vga_buffer[y * VGA_WIDTH + x] = (uint16_t)c | (VGA_COLOR_WHITE << 8);
}

// Clear the text screen.
void clear_screen() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            print_char(' ', x, y);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
    move_cursor(0, 0);
}

// Keyboard mapping table.
static const char keymap[] = {
    '\0', '\e', '1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s',
    'd','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v',
    'b','n','m',',','.','/',0,'*',0,' ',0
};

// Keyboard handler: read scancode, map to key, and pass to shell.
void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA);
    static uint8_t shift = 0;
    char key = 0;

    if (scancode & 0x80) {
        if (scancode == (KEY_LSHIFT | 0x80) || scancode == (KEY_RSHIFT | 0x80))
            shift = 0;
        return;
    }

    switch (scancode) {
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            shift = 1;
            break;
        case KEY_ENTER:
            key = '\n';
            break;
        case KEY_BACKSPACE:
            key = '\b';
            break;
        default:
            if (scancode < sizeof(keymap)) {
                key = keymap[scancode];
                if (shift && key >= 'a' && key <= 'z')
                    key -= 32;
            }
    }

    // Pass key to shell if active.
    if (key && active_shell)
         shell_handle_key(key, active_shell);

    move_cursor(cursor_x, cursor_y);
    outb(PIC1_CMD, 0x20);
}

// Initialize the keyboard (unmask IRQ1 in the PIC).
void init_keyboard() {
    outb(PIC1_DATA, inb(PIC1_DATA) & ~0x02);
}
