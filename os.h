// os.h
#ifndef OS_H
#define OS_H

#ifdef CROSS_COMPILE
    #include <stdint.h>
#else
    typedef unsigned char       uint8_t;
    typedef unsigned short      uint16_t;
    typedef unsigned int        uint32_t;
    typedef unsigned long long  uint64_t;
    typedef signed char         int8_t;
    typedef short               int16_t;
    typedef int                 int32_t;
    typedef long long           int64_t;
#endif

// Hardware-Konstanten
#define PIC1_CMD        0x20
#define PIC1_DATA       0x21
#define PIC2_CMD        0xA0
#define PIC2_DATA       0xA1
#define KEYBOARD_DATA   0x60
#define VGA_WIDTH       80
#define VGA_HEIGHT      25
#define VGA_COLOR_WHITE 0x07

// Tasten-Kodes (Beispiele – erweitere diese nach Bedarf)
#define KEY_LSHIFT 0x2A
#define KEY_RSHIFT 0x36
#define KEY_ENTER  0x1C
#define KEY_BACKSPACE 0x0E

//GUI / Shell
extern Window *active_shell;

extern void shell_handle_key(char key, Window *shell_win);

volatile uint32_t ticks = 0;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint16_t* vga_buffer = (uint16_t*)0xB8000;

// GDT
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

// IDT
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Funktionen
void gdt_flush();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void init_timer();
void init_keyboard();
void print_char(char c, uint8_t x, uint8_t y);
void clear_screen();
void pic_remap();

// Assembly-Linker
extern void timer_handler_asm();
extern void keyboard_handler_asm();

// Globale Variablen
extern volatile uint32_t ticks;
extern struct idt_ptr idt_descriptor;

// IO
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    asm volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

#endif // OS_H