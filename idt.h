#ifndef IDT_H
#define IDT_H
#include <stdint.h> // Include standard integer types

#define IDT_ENTRIES 256 // Define the number of IDT entries

// Define the IDT entry structure
struct idt_entry {
    uint16_t base_low;
    uint16_t sel;       // Selector
    uint8_t always0;    // Always zero
    uint8_t flags;      // Flags
    uint16_t base_high;
} __attribute__((packed));

// Define the IDT pointer structure
struct idt_ptr {
    uint16_t limit;     // Size of the IDT - 1
    uint32_t base;      // Address of the first entry in the IDT
} __attribute__((packed));

// Declare the IDT and its descriptor
extern struct idt_entry idt[IDT_ENTRIES];
extern struct idt_ptr idt_descriptor;

// Declare the functions
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void init_idt();
void timer_handler_asm();
void _start();

#endif // IDT_H