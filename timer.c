#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_handler(void);
void init_timer(void);

#endif

#include "io.h"
#include "idt.h"
#include "pic.h"
#include "timer.h"

#define PIT_FREQ 1193182
#define TIMER_HZ 50
volatile uint32_t ticks = 0;

static void timer_set_freq(uint32_t hz) {
    uint16_t divisor = PIT_FREQ / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void timer_handler(void) {
    ticks++;
}

void init_timer(void) {
    timer_set_freq(TIMER_HZ);
    extern void timer_handler_asm(void);
    idt_set_gate(32, (uint32_t)timer_handler_asm, 0x08, 0x8E);
    outb(0x43, 0x36);
    outb(0x40, 0xFF);
    outb(0x40, 0xFF);
    outb(PIC1_DATA, 0xFE);
}