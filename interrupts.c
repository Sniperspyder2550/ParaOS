#include "interrupts.h"
#include "io.h"

struct idt_entry idt[256];
struct idt_ptr idt_ptr;

void idt_set_gate(uint8_t num, void* handler, uint8_t flags) {
    uint32_t addr = (uint32_t)handler;
    idt[num].offset_low = addr & 0xFFFF;
    idt[num].offset_high = (addr >> 16) & 0xFFFF;
    idt[num].selector = 0x08;
    idt[num].zero = 0;
    idt[num].type_attr = flags | 0x80;
}
void idt_init() {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt;
    
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0);
    }
    
    // WICHTIG: Timer und Tastatur registrieren
    // Register Timer and Keyboard handlers
    idt_set_gate(0x20, timer_handler, 0x8E);  // IRQ0 -> Timer
    idt_set_gate(0x21, keyboard_handler, 0x8E); // IRQ1 -> Tastatur
    
    asm volatile("lidt [%0]" : : "r"(&idt_ptr));
}