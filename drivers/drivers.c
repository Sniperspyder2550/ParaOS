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

// VGA
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

// Tastatur
static const char keymap[] = {
    '\0', '\e', '1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s',
    'd','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v',
    'b','n','m',',','.','/',0,'*',0,' ',0
};

void keyboard_handler() {
    uint8_t scancode = inb(0x60);
    static uint8_t shift = 0;

    if(scancode & 0x80) {
        if(scancode == (KEY_LSHIFT|0x80) || scancode == (KEY_RSHIFT|0x80)) {
            shift = 0;
        }
        return;
    }

    switch(scancode) {
        case KEY_LSHIFT:
        case KEY_RSHIFT: shift = 1; break;
        case KEY_ENTER:
            cursor_x = 0;
            cursor_y++;
            break;
        case KEY_BACKSPACE:
            if(cursor_x > 0) {
                cursor_x--;
                print_char(' ', cursor_x, cursor_y);
            }
            break;
        default:
            if(scancode < sizeof(keymap)) {
                char c = keymap[scancode];
                if(shift && c >= 'a' && c <= 'z') c -= 32;
                if(c) {
                    print_char(c, cursor_x, cursor_y);
                    cursor_x++;
                }
            }
    }

    if(cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if(cursor_y >= VGA_HEIGHT) {
        for(int y = 1; y < VGA_HEIGHT; y++) {
            for(int x = 0; x < VGA_WIDTH; x++) {
                vga_buffer[(y-1)*VGA_WIDTH+x] = vga_buffer[y*VGA_WIDTH+x];
            }
        }
        cursor_y = VGA_HEIGHT-1;
        for(int x = 0; x < VGA_WIDTH; x++) {
            print_char(' ', x, cursor_y);
        }
    }
    
    move_cursor(cursor_x, cursor_y);
    outb(PIC1_CMD, 0x20);
}

void init_keyboard() {
    outb(PIC1_DATA, inb(PIC1_DATA) & ~0x02);
}