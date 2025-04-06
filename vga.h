#ifndef VGA_H
#define VGA_H

#include <stdint.h>

// Function to print a character to the screen
void print_char(char c, uint8_t color, uint8_t x, uint8_t y);

// Function to move the cursor
void move_cursor(uint8_t x, uint8_t y);

#endif // VGA_H
