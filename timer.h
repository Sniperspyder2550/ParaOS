#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern volatile uint32_t ticks;

void init_timer;
void timer_handler;

#endif