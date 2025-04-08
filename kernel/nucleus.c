// nucleus.c
#include "os.h"

struct gdt_ptr gdt_ptr;

// --------------------------------------------------------
// Neue Definitionsbereiche für Grafik und GUI/Shell
// --------------------------------------------------------

// Framebuffer-Adresse und Auflösung
#define SCREEN_WIDTH   640
#define SCREEN_HEIGHT  480
volatile uint32_t* framebuffer = (volatile uint32_t*)0xE0000000;

// Globale Shell-Variablen
Window *active_shell;  // Wichtig: Muss mit os.h-Deklaration übereinstimmen
char shell_buffer[128];
int shell_buffer_index = 0;

// --------------------------------------------------------
// Grafikfunktionen
// --------------------------------------------------------
void put_pixel(int x, int y, uint32_t color) {
    if(x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
        framebuffer[y * SCREEN_WIDTH + x] = color;
}

void fill_rect(int x, int y, int width, int height, uint32_t color) {
    for(int j = y; j < y + height; j++) {
        for(int i = x; i < x + width; i++) {
            put_pixel(i, j, color);
        }
    }
}

// --------------------------------------------------------
// GUI-Funktionen
// --------------------------------------------------------
#define CHAR_WIDTH   8
#define CHAR_HEIGHT  8

void draw_char(char c, int x, int y, uint32_t color) {
    fill_rect(x, y, CHAR_WIDTH, CHAR_HEIGHT, color);
}

void draw_text(const char* text, int x, int y, uint32_t color) {
    int posx = x;
    for(int i = 0; text[i] != '\0'; i++) {
        draw_char(text[i], posx, y, color);
        posx += CHAR_WIDTH;
    }
}

// --------------------------------------------------------
// Fensterfunktionen
// --------------------------------------------------------
void draw_window(Window *win) {
    // Fensterhintergrund
    fill_rect(win->x, win->y, win->width, win->height, win->bg_color);
    // Titelleiste
    fill_rect(win->x, win->y, win->width, 20, 0x0000FF);
    // Titeltext
    if(win->title) {
        draw_text(win->title, win->x + 5, win->y + 5, 0xFFFFFF);
    }
}

void clear_shell_window(Window *win) {
    fill_rect(win->x, win->y + 20, win->width, win->height - 20, win->bg_color);
}

// --------------------------------------------------------
// Shell-Implementierung
// --------------------------------------------------------
void execute_command(const char *cmd, Window *shell_win) {
    if(strcmp(cmd, "clear") == 0) {
        clear_shell_window(shell_win);
    } else if(strcmp(cmd, "help") == 0) {
        draw_text("Commands: help, clear", shell_win->x + 5, shell_win->y + 45, 0xFFFFFF);
    } else {
        draw_text("Unknown command", shell_win->x + 5, shell_win->y + 45, 0xFFFFFF);
    }
}

void shell_handle_key(char key, Window *shell_win) {
    if(key == '\n' || key == '\r') {
        shell_buffer[shell_buffer_index] = '\0';
        execute_command(shell_buffer, shell_win);
        shell_buffer_index = 0;
        draw_text("ParaOS> ", shell_win->x + 5, shell_win->y + 25, 0xFFFFFF);
    } else if(key == '\b') {
        if(shell_buffer_index > 0) {
            shell_buffer_index--;
            clear_shell_window(shell_win);
            draw_text("ParaOS> ", shell_win->x + 5, shell_win->y + 25, 0xFFFFFF);
            draw_text(shell_buffer, shell_win->x + 5 + (CHAR_WIDTH * shell_buffer_index), shell_win->y + 25, 0xFFFFFF);
        }
    } else {
        if(shell_buffer_index < sizeof(shell_buffer) - 1) {
            shell_buffer[shell_buffer_index++] = key;
            draw_text(shell_buffer, shell_win->x + 5 + (CHAR_WIDTH * shell_buffer_index), shell_win->y + 25, 0xFFFFFF);
        }
    }
}

// --------------------------------------------------------
// Hauptfunktion
// --------------------------------------------------------
int main() {
    // Hardware-Initialisierung
    gdt_init();
    idt_init();
    pic_remap();
    init_timer();
    init_keyboard();

    // Grafikmodus initialisieren
    fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x000000);

    // Shell-Fenster erstellen
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

    // Hauptschleife
    while(1) {
        asm volatile("hlt");
    }
}