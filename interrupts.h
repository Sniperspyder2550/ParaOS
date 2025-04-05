#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>
#include "pic.h"

// IDT-Einträge
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Globale IDT
extern struct idt_entry idt[256];
extern struct idt_ptr idt_ptr;

// Funktionen
void idt_init();
void idt_set_gate(uint8_t num, void* handler, uint8_t flags);

// Interrupt-Handler
extern void keyboard_handler();
extern void timer_handler();  // Nicht vergessen!

#endif