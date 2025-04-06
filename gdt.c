#include "gdt.h"

struct gdt_entry gdt[3];
struct gdt_ptr gdt_descriptor;

void gdt_install(void) {
    gdt[0] = (struct gdt_entry){ .base = 0, .limit = 0, .access = 0, .granularity = 0 }; // Null segment
    gdt[1] = (struct gdt_entry){ .base = 0, .limit = 0xFFFFF, .access = 0x9A, .granularity = 0xCF }; // Code segment
    gdt[2] = (struct gdt_entry){ .base = 0, .limit = 0xFFFFF, .access = 0x92, .granularity = 0xCF }; // Data segment

    gdt_descriptor.limit = sizeof(gdt) - 1;
    gdt_descriptor.base = (uint32_t)&gdt;

    gdt_flush((uint32_t)&gdt_descriptor);
}