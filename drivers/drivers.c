// drivers.c
#include "os.h"

volatile uint32_t ticks = 0;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint16_t* vga_buffer = (uint16_t*)0xB8000;

// Timer
void timer_handler() {
    ticks++;
    outb(PIC1_CMD, 0x20);
}

void init_timer() {
    uint32_t divisor = 1193180 / 1000;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

// VGA (Textmodus – wird im Grafikmodus nicht weiter verwendet)
void move_cursor(uint8_t x, uint8_t y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void print_char(char c, uint8_t x, uint8_t y) {
    vga_buffer[y * VGA_WIDTH + x] = (uint16_t)c | (VGA_COLOR_WHITE << 8);
}

void clear_screen() {
    for(int y = 0; y < VGA_HEIGHT; y++) {
        for(int x = 0; x < VGA_WIDTH; x++) {
            print_char(' ', x, y);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
    move_cursor(0, 0);
}

// Tastatur-Keymap (unverändert)
static const char keymap[] = {
    '\0', '\e', '1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s',
    'd','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v',
    'b','n','m',',','.','/',0,'*',0,' ',0
};

// Im Grafikmodus soll der Tastaturhandler die Eingaben an die Shell weiterleiten.
void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA);
    static uint8_t shift = 0;
    char key = 0;

    if(scancode & 0x80) {
        if(scancode == (KEY_LSHIFT|0x80) || scancode == (KEY_RSHIFT|0x80)) {
            shift = 0;
        }
        return;
    }

    switch(scancode) {
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
            if(scancode < sizeof(keymap)) {
                key = keymap[scancode];
                if(shift && key >= 'a' && key <= 'z') key -= 32;
            }
    }

    // Wenn im Grafikmodus eine Shell aktiv ist, leite die Taste an diese weiter
    if(key && active_shell)
         shell_handle_key(key, active_shell);

    move_cursor(cursor_x, cursor_y); // für Textmodus, ansonsten irrelevant
    outb(PIC1_CMD, 0x20);
}
 
void init_keyboard() {
    outb(PIC1_DATA, inb(PIC1_DATA) & ~0x02);
}
