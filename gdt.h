#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// GDT entry structure
struct gdt_entry {
    uint16_t limit_low;  // Lower 16 bits of the limit
    uint16_t base_low;   // Lower 16 bits of the base
    uint8_t base_middle; // Next 8 bits of the base
    uint8_t access;      // Access flags
    uint8_t granularity; // Granularity flags
    uint8_t base_high;   // Last 8 bits of the base
} __attribute__((packed));

// GDT pointer structure
struct gdt_ptr {
    uint16_t limit;      // Limit of the GDT
    uint32_t base;       // Base address of the GDT
} __attribute__((packed));

// Function declarations
void gdt_init();
extern void gdt_flush(uint32_t); // Declare gdt_flush as an external assembly function

#endif // GDT_H
