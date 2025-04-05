#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern volatile uint32_t ticks;  // Für externen Zugriff

void timer_handler(void);
void init_timer(void);
uint32_t get_ticks(void);

#endif