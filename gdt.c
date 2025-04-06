#include "gdt.h"

// Define GDT and GDT pointer
struct gdt_entry gdt[3];
struct gdt_ptr gdt_descriptor;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt[num].base_low = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = limit & 0xFFFF;
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= granularity & 0xF0;
    gdt[num].access = access;
}

void gdt_install() {
    gdt_descriptor.limit = sizeof(gdt) - 1;
    gdt_descriptor.base = (uint32_t)&gdt;

    // Null segment
    gdt_set_gate(0, 0, 0, 0, 0);

    // Code segment
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xCF);

    // Data segment
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xCF);

    // Load the GDT
    gdt_flush((uint32_t)&gdt_descriptor);
}