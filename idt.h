#ifndef IDT_H
#define IDT_H

// Define custom integer types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define IDT_ENTRIES 256

struct idt_entry {
    uint16_t base_low;
    uint16_t sel;       // Selector
    uint8_t always0;    // Always zero
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit; // Size of the IDT - 1
    uint32_t base;  // Address of the first entry in the IDT
} __attribute__((packed));

void init_idt();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void default_handler(void);

#endif