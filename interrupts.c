#include "interrupts.h"
#include "io.h"

// Globale Variablen
uint32_t ticks = 0;

/* IDT-Einträge */
struct idt_entry idt[256];
struct idt_ptr idt_ptr;

/* Standard-Interrupt-Handler */
void default_handler() {
    // Notfall-Ausgabe im Fehlerfall
    volatile char *vga = (volatile char*)0xB8000;
    vga[160] = '!';  // Zeigt einen Interrupt an (2. Zeile)
}

/* IDT-Eintrag setzen */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

/* IDT initialisieren */
void init_idt() {
    idt_ptr.limit = sizeof(struct idt_entry) * 256 - 1;
    idt_ptr.base = (uint32_t)&idt;

    // Alle Interrupts mit Default-Handler belegen
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint32_t)default_handler, 0x08, 0x8E);
    }

    // IDT laden
    asm volatile("lidt (%0)" : : "r"(&idt_ptr));
}

/* PIC neu mappen */
void pic_remap() {
    // ICW1: Initialisierung starten
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);

    // ICW2: Interrupt-Vektoren
    outb(PIC1_DATA, 0x20);  // IRQ 0-7 -> INT 0x20-0x27
    outb(PIC2_DATA, 0x28);  // IRQ 8-15 -> INT 0x28-0x2F

    // ICW3: Verbindung der PICs
    outb(PIC1_DATA, 0x04);  // Slave an IRQ2
    outb(PIC2_DATA, 0x02);

    // ICW4: 8086-Modus
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Alle Interrupts maskieren
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

/* Timer initialisieren (IRQ0) */
void init_timer() {
    // 100 Hz (10 ms) Timer
    uint16_t divisor = 1193180 / 100;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}