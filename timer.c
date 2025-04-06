#include "timer.h"
#include "io.h"
#include "pic.h"
#include "interrupts.h"

#define PIT_FREQ 1193182
#define TIMER_HZ 1000
volatile uint32_t ticks = 0;

static void timer_set_freq(uint32_t hz) {
    uint16_t divisor = PIT_FREQ / hz;
    outb(0x43, 0x36); // Channel 0, Mode 3
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void timer_handler(void) {
    ticks++;
    outb(PIC1_CMD, 0x20); // End of Interrupt (EOI) for IRQ0
}

void init_timer(void) {
    timer_set_freq(TIMER_HZ);
    
    // Ensure the external assembly handler is declared correctly
    extern void timer_handler_asm(void);
    
    // Set IDT entry for IRQ0 (Interrupt 0x20)
    // 0x08: Code segment selector, 0x8E: Present, DPL=0, 32-bit interrupt gate
    idt_set_gate(0x20, (uint32_t)timer_handler_asm, 0x08);
    
    // Unmask IRQ0 in the PIC
    outb(PIC1_DATA, inb(PIC1_DATA) & ~0x01);
}

uint32_t get_ticks(void) {
    return ticks;
}