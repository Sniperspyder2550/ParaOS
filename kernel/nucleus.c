#include "os.h"

// GDT
struct gdt_entry gdt[3];
struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_ptr gdt_ptr;

// IDT
struct idt_entry idt[256] __attribute__((aligned(8)));
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_ptr idt_ptr;

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
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xCF); // Code-Segment
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xCF); // Data-Segment

    gdt_flush();
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags | 0x80; // Immer Present-Flag setzen
}

void idt_init() {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt;

    // Alle Interrupts zunächst deaktivieren
    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0x08, 0x8E);
    }

    // Timer-Interrupt (IRQ0)
    idt_set_gate(0x20, (uint32_t)timer_handler_asm, 0x08, 0x8E);
    
    // Tastatur-Interrupt (IRQ1)
    idt_set_gate(0x21, (uint32_t)keyboard_handler_asm, 0x08, 0x8E);

    // IDT mit korrekter Assembler-Syntax laden
    asm volatile("lidt %0" : : "m"(idt_ptr));
}

void main() {
    // Hardware-Initialisierung
    gdt_init();
    idt_init();
    pic_remap();
    init_timer();
    init_keyboard();

    // Interrupts global aktivieren
    asm("sti");

    // VGA-Testausgabe
    volatile char *vga = (volatile char *)0xB8000;
    vga[0] = 'O';
    vga[1] = 0x0F; // Weißer Text auf schwarzem Grund

    // Hauptschleife mit Ticker-Anzeige
    while(1) {
        if(ticks % 100 == 0) {
            vga[2] = '0' + (ticks / 100 % 10);
            vga[3] = 0x0F;
        }
    }
}