#include "io.h"
#include "idt.h"

#define PIT_FREQ 1193182
#define TIMER_HZ 50
volatile uint32_t ticks = 0;

static void timer_set_freq(uint32_t hz){
    uint16_t divisor = PIT_FREQ / hz;
    outb(0x43, 0x36)
}