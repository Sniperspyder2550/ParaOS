#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* ======================
   Erweiterte IDT-Definition
   ====================== */
#define IDT_ENTRIES 256
#define PIC1_CMD 0x20
#define PIC1_DATA 0x21

// IDT-Gate-Typen
#define IDT_TASK_GATE    0x5
#define IDT_INT_GATE     0xE
#define IDT_TRAP_GATE    0xF

#pragma pack(push, 1)  // Exakte Struct-Packing

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;       // Interrupt Stack Table Offset
    uint8_t  type_attr; // P(1) | DPL(2) | 0(1) | GateType(4)
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;      // Reserviert
} idt_entry_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} idt_ptr_t;

struct idt_entry {
    uint16_t base_low;  // Lower 16 bits of the handler function address
    uint16_t sel;       // Kernel segment selector
    uint8_t always0;    // This must always be zero
    uint8_t flags;      // Flags (type and attributes)
    uint16_t base_high; // Upper 16 bits of the handler function address
};

struct idt_ptr {
    uint16_t limit; // Size of the IDT
    uint32_t base;  // Base address of the IDT
};

#pragma pack(pop)

/* ======================
   Funktionsdeklarationen
   ====================== */
// Kernelfunktionen
void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags); // Matches implementation in idt.c

// Interrupt-Handler
void keyboard_handler_asm(void);
extern void keyboard_handler_c(void);
extern void timer_handler_asm();

// PIC-Steuerung
void pic_remap(void);
void pic_enable_irq(uint8_t irq);
static inline void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) outb(PIC2_CMD, 0x20);
    outb(PIC1_CMD, 0x20);
}

/* ======================
   Vordefinierte Einstellungen
   ====================== */
#define IDT_FLAG_PRESENT  (1 << 7)
#define IDT_FLAG_RING0    (0 << 5)
#define IDT_FLAG_RING3    (3 << 5)
#define IDT_FLAG_INT_GATE (IDT_INT_GATE << 0)
#define IDT_FLAG_TRAP_GATE(x) ((x) << 0)

#endif // IDT_H