#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

// PIC-Ports
#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

// IDT-Strukturen
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Globale Variablen
extern uint32_t ticks;

// Funktionen
void init_idt();
void pic_remap();
void init_timer();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif