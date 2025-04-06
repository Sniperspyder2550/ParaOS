#include "interrupts.h"
#include "idt.h"

// Declare idt_set_gate as extern
extern void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

// Declare idt as extern
extern struct idt_entry idt[256];
extern struct idt_ptr idt_ptr;

void default_handler(void) {
    // Handle unexpected interrupts
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

void init_idt() {
    idt_ptr.limit = sizeof(struct idt_entry) * 256 - 1; // Corrected from idt_descriptor to idt_ptr
    idt_ptr.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    idt_set_gate(32, (uint32_t)timer_handler_asm, 0x08, 0x8E);

    // Load the IDT
    asm volatile("lidt %0" : : "m"(idt_ptr));
}

// Corrected timer_handler_asm declaration
extern void timer_handler_asm(); // Declare as external if implemented in assembly

__attribute__((section(".multiboot")))
const unsigned int multiboot_header[] = {
    0x1BADB002, // Magic number
    0x0,        // Flags (no additional features required)
    -(0x1BADB002 + 0x0) // Checksum (magic + flags + checksum = 0)
};

void idt_enable_irq1() {
    idt_set_gate(0x21, (uint32_t)keyboard_handler_asm, 0x08, 0x8E); // IRQ1 -> INT 0x21
    __asm__ __volatile__("sti"); // Enable interrupts
}