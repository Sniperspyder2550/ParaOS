#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

// IDT entry structure
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;        // Kernel segment selector
    uint8_t always0;     // This must always be zero
    uint8_t flags;       // Flags
    uint16_t base_hi;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Declare idt_ptr as extern
extern struct idt_ptr idt_ptr;

// Function declarations
void idt_init();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void init_timer();
void idt_enable_irq1();
void pic_enable_keyboard();
void init_idt(); // Declare init_idt function

#endif // INTERRUPTS_H