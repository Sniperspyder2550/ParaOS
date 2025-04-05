#include "idt.h"

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idt_descriptor;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void default_handler(void) {
    // Handle unexpected interrupts
    while (1) {
        // Halt the CPU or log the error
    }
}

void init_idt(){
    idt_descriptor.limit = sizeof(struct idt_entry) * IDT_ENTRIES -1;
    idt_descriptor.base = (uint32_t)&idt;

    extern void timer_handler_asm(void);
    idt_set_gate(32, (uint32_t)timer_handler_asm, 0x08, 0x8E);
    
    
    for(int i = 0; i < IDT_ENTRIES; i++){
        idt_set_gate(i, (uint32_t)default_handler, 0x08, 0x8E);
    }
}