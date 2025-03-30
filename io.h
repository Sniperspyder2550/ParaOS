#ifndef IO_H
#define IO_H

// Define custom integer types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port){
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port)); 
}

#endif