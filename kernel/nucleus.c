#include "os.h"

struct gdt_entry gdt[3];
struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry idt[256] __attribute__((aligned(8)));
struct idt_ptr idt_descriptor;

struct gdt_ptr gdt_ptr;

void print_number(uint32_t num, uint8_t x, uint8_t y) {
    char buffer[11];
    int i = 0;
    
    if(num == 0) {
        print_char('0', x, y);
        return;
    }

    while(num > 0 && i < 10) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    for(int j = i-1, k = 0; j >= 0; j--, k++) {
        print_char(buffer[j], x + k, y);
    }
}

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = limit & 0xFFFF;
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_init() {
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uint32_t)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xCF);
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xCF);

    __asm__ volatile("lgdt %0" : : "m"(gdt_ptr));
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags | 0x80;
}

void idt_init() {
    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base = (uint32_t)&idt;

    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0x08, 0x8E);
    }

    idt_set_gate(0x20, (uint32_t)timer_handler_asm, 0x08, 0x8E);
    idt_set_gate(0x21, (uint32_t)keyboard_handler_asm, 0x08, 0x8E);

    __asm__ volatile("lidt %0" : : "m"(idt_descriptor));
}

void main() {
    gdt_init();
    idt_init();
    pic_remap();
    init_timer();
    init_keyboard();
    asm("sti");
    clear_screen();

    uint32_t last_ticks = 0;
    while(1) {
        if(ticks != last_ticks) {
            print_number(ticks, 0, 0);
            last_ticks = ticks;
        }
    }
}