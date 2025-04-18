/* nucleus.c */
#include "os.h"

// Forward declaration for the default handler (provided in default_int.asm)
extern void default_interrupt_handler_asm();

// ----------------------------------------------------------------------
// Global Descriptor Table (GDT) Setup
// ----------------------------------------------------------------------
struct gdt_entry gdt[3] = {0};
struct gdt_ptr gdt_ptr;

void gdt_init() {
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uint32_t)&gdt;

    // Code Segment Descriptor
    gdt[1] = (struct gdt_entry){
        .limit_low   = 0xFFFF,
        .base_low    = 0,
        .base_middle = 0,
        .access      = 0x9A,  // present, ring 0, code, executable, readable
        .granularity = 0xCF,  // 4K granularity, 32-bit
        .base_high   = 0
    };

    // Data Segment Descriptor
    gdt[2] = (struct gdt_entry){
        .limit_low   = 0xFFFF,
        .base_low    = 0,
        .base_middle = 0,
        .access      = 0x92,  // present, ring 0, data, writable
        .granularity = 0xCF,
        .base_high   = 0
    };

    asm volatile("lgdt %0" : : "m"(gdt_ptr));
}

// ----------------------------------------------------------------------
// Interrupt Descriptor Table (IDT) Setup
// ----------------------------------------------------------------------
struct idt_entry idt[256];
struct idt_ptr idt_descriptor;

// Helper function to set an IDT gate.
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
}

void idt_init() {
    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base  = (uint32_t)&idt;

    // Set gates for timer (IRQ0 => interrupt 32) and keyboard (IRQ1 => interrupt 33)
    idt_set_gate(32, (uint32_t) timer_handler_asm, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t) keyboard_handler_asm, 0x08, 0x8E);

    // Install default handler for all remaining entries.
    for (uint8_t i = 0; i < 256; i++) {
        if (i != 32 && i != 33)
            idt_set_gate(i, (uint32_t) default_interrupt_handler_asm, 0x08, 0x8E);
    }

    asm volatile("lidt %0" : : "m"(idt_descriptor));
}

// ----------------------------------------------------------------------
// Graphics and Shell Definitions
// ----------------------------------------------------------------------
#define SCREEN_WIDTH   640
#define SCREEN_HEIGHT  480
volatile uint32_t* framebuffer = (volatile uint32_t*)0xE0000000;

// Global shell variables
Window *active_shell;
char shell_buffer[128];
int shell_buffer_index = 0;

// Framebuffer graphics routines:
void put_pixel(int x, int y, uint32_t color) {
    if(x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
        framebuffer[y * SCREEN_WIDTH + x] = color;
}

void fill_rect(int x, int y, int width, int height, uint32_t color) {
    for (int j = y; j < y + height; j++) {
        for (int i = x; i < x + width; i++) {
            put_pixel(i, j, color);
        }
    }
}

// For simplicity, each character is drawn as a filled rectangle.
#define CHAR_WIDTH   8
#define CHAR_HEIGHT  8

void draw_char(char c, int x, int y, uint32_t color) {
    /* In a full OS, you might draw a bitmap font.
       Here we simply draw a rectangle as a placeholder. */
    fill_rect(x, y, CHAR_WIDTH, CHAR_HEIGHT, color);
}

void draw_text(const char* text, int x, int y, uint32_t color) {
    int posx = x;
    for (int i = 0; text[i] != '\0'; i++) {
        draw_char(text[i], posx, y, color);
        posx += CHAR_WIDTH;
    }
}

// Window functions for the GUI/shell.
void draw_window(Window *win) {
    // Draw window background.
    fill_rect(win->x, win->y, win->width, win->height, win->bg_color);
    // Draw title bar (fixed height of 20 pixels).
    fill_rect(win->x, win->y, win->width, 20, 0x0000FF);
    if (win->title) {
        draw_text(win->title, win->x + 5, win->y + 5, 0xFFFFFF);
    }
}

void clear_shell_window(Window *win) {
    // Clear the window's content area (below the title bar).
    fill_rect(win->x, win->y + 20, win->width, win->height - 20, win->bg_color);
}

// Shell command execution function.
void execute_command(const char *cmd, Window *shell_win) {
    if (strcmp(cmd, "clear") == 0) {
        clear_shell_window(shell_win);
    } else if (strcmp(cmd, "help") == 0) {
        draw_text("Commands: help, clear", shell_win->x + 5, shell_win->y + 45, 0xFFFFFF);
    } else {
        draw_text("Unknown command", shell_win->x + 5, shell_win->y + 45, 0xFFFFFF);
    }
}

// Process a key pressed on the shell.
// This function is called by the keyboard handler (in drivers.c).
void shell_handle_key(char key, Window *shell_win) {
    if (key == '\n' || key == '\r') {
        shell_buffer[shell_buffer_index] = '\0';
        execute_command(shell_buffer, shell_win);
        shell_buffer_index = 0;
        clear_shell_window(shell_win);
        draw_text("ParaOS> ", shell_win->x + 5, shell_win->y + 25, 0xFFFFFF);
    } else if (key == '\b') {
        if (shell_buffer_index > 0) {
            shell_buffer_index--;
            clear_shell_window(shell_win);
            draw_text("ParaOS> ", shell_win->x + 5, shell_win->y + 25, 0xFFFFFF);
            draw_text(shell_buffer, shell_win->x + 5 + (CHAR_WIDTH * shell_buffer_index),
                      shell_win->y + 25, 0xFFFFFF);
        }
    } else {
        if (shell_buffer_index < (int)(sizeof(shell_buffer) - 1)) {
            shell_buffer[shell_buffer_index++] = key;
            clear_shell_window(shell_win);
            draw_text("ParaOS> ", shell_win->x + 5, shell_win->y + 25, 0xFFFFFF);
            draw_text(shell_buffer, shell_win->x + 5 + (CHAR_WIDTH * shell_buffer_index),
                      shell_win->y + 25, 0xFFFFFF);
        }
    }
}

// ----------------------------------------------------------------------
// Main Kernel Function
// ----------------------------------------------------------------------
int main() {
    // Initialize segmentation and interrupts.
    gdt_init();
    idt_init();
    pic_remap();

    // Initialize timer and keyboard drivers (from drivers.c).
    init_timer();
    init_keyboard();

    // Set up graphics: clear the screen.
    fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x000000);

    // Create a shell window.
    Window shell_win = {
        .x = 50,
        .y = 50,
        .width = 540,
        .height = 380,
        .bg_color = 0x000000,
        .title = "ParaOS Shell"
    };

    draw_window(&shell_win);
    active_shell = &shell_win;
    draw_text("ParaOS> ", shell_win.x + 5, shell_win.y + 25, 0xFFFFFF);

    // Enable interrupts.
    asm volatile("sti");

    // Main loop: efficient idle using hlt.
    while (1) {
        asm volatile("hlt");
    }

    return 0;
}
