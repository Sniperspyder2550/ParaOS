#include "interrupts.h"
#include "io.h"

// Define idt and idt_ptr here
struct idt_entry idt[256];
struct idt_ptr idt_ptr;

// Declare external handlers to resolve undefined references
extern void timer_handler(void);
extern void keyboard_handler(void);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel; // Segment selector
    idt[num].always0 = 0;
    idt[num].flags = flags; // Descriptor type and attributes
}

void idt_init() {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt;
    
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Register Timer and Keyboard handlers
    idt_set_gate(0x20, (uint32_t)timer_handler, 0x08, 0x8E);  // IRQ0 -> Timer
    idt_set_gate(0x21, (uint32_t)keyboard_handler, 0x08, 0x8E); // IRQ1 -> Keyboard
    
    // Corrected assembly syntax for lidt
    asm volatile("lidt %0" : : "m"(idt_ptr));
}