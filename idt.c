#include "idt.h"

// Define the IDT and its descriptor
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
        __asm__ __volatile__("hlt");
    }
}

void init_idt() {
    idt_descriptor.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
    idt_descriptor.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, (uint32_t)default_handler, 0x08, 0x8E);
    }

    idt_set_gate(32, (uint32_t)timer_handler_asm, 0x08, 0x8E);
}

// Corrected timer_handler_asm declaration
extern void timer_handler_asm(); // Declare as external if implemented in assembly

__attribute__((section(".multiboot")))
const unsigned int multiboot_header[] = {
    0x1BADB002, // Magic number
    0x0,        // Flags (no additional features required)
    -(0x1BADB002 + 0x0) // Checksum (magic + flags + checksum = 0)
};

void _start() {
    init_idt();
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

void idt_enable_irq1() {
    idt_set_gate(0x21, (uint32_t)keyboard_handler_asm, 0x08, 0x8E); // IRQ1 -> INT 0x21
    __asm__ __volatile__("sti"); // Enable interrupts
}