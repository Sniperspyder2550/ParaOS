#include "timer.h"
#include "io.h"
#include "pic.h"
#include "interrupts.h"

#define PIT_FREQ 1193182
#define TIMER_HZ 1000  // Präzisere Zeitmessung
volatile uint32_t ticks = 0;

static void timer_set_freq(uint32_t hz) {
    uint16_t divisor = PIT_FREQ / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void timer_handler(void) {
    ticks++;
    pic_send_eoi(0);  // EOI für IRQ0
}

void init_timer(void) {
    timer_set_freq(TIMER_HZ);
    
    // IDT-Eintrag für IRQ0 (Timer)
    extern void timer_handler_asm();
    idt_set_gate(0x20, (uint32_t)timer_handler_asm, 0x08, 0x8E); // Ensure correct arguments for idt_set_gate
    // Verify that idt_set_gate is defined as: void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
    register_interrupt_handler(IRQ0, timer_handler);    
    // PIC konfigurieren
    outb(PIC1_DATA, inb(PIC1_DATA) & ~0x01);  // IRQ0 unmasken
}

uint32_t get_ticks(void) {
    return ticks;
}