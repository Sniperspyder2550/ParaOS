// nucleus.c
#include "os.h"

// --------------------------------------------------------
// Neue Definitionsbereiche für Grafik und GUI/Shell
// --------------------------------------------------------

// Wir definieren einen einfachen Grafikmodus mit festen Parametern:
#define SCREEN_WIDTH   640
#define SCREEN_HEIGHT  480

volatile uint32_t* framebuffer = (volatile uint32_t*)0xE0000000;  // Annahme: Framebuffer-Adresse (muss durch VESA ermittelt werden)

// Grafikfunktionen
void put_pixel(int x, int y, uint32_t color) {
    if(x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
         return;
    framebuffer[y * SCREEN_WIDTH + x] = color;
}

void fill_rect(int x, int y, int width, int height, uint32_t color) {
    for (int j = y; j < y + height; j++) {
         for (int i = x; i < x + width; i++) {
              put_pixel(i, j, color);
         }
    }
}

// Einfache Zeichenfunktionen für die GUI (Statt eines echten Bitmap-Fonts verwenden wir Rechtecke zur Darstellung)
#define CHAR_WIDTH   8
#define CHAR_HEIGHT  8

void draw_char(char c, int x, int y, uint32_t color) {
    // Diese rudimentäre Implementierung ignoriert den tatsächlichen Zeichensatz
    // und stellt jedes Zeichen als gefülltes Rechteck dar.
    fill_rect(x, y, CHAR_WIDTH, CHAR_HEIGHT, color);
}

void draw_text(const char* text, int x, int y, uint32_t color) {
    int posx = x;
    for (int i = 0; text[i] != '\0'; i++) {
       draw_char(text[i], posx, y, color);
       posx += CHAR_WIDTH;
    }
}

// Funktion zum Wechsel in den Grafikmodus (VESA)
// Hinweis: In Protected Mode funktionieren BIOS-Interrupts nicht direkt – diese Funktion dient
// hier nur als Demonstration.
void set_video_mode(uint16_t mode) {
    asm volatile (
        "mov ax, 0x4F02 \n"  // VESA Set Mode
        "mov bx, %0 \n"
        "int $0x10 \n"
        : : "r"(mode)
    );
}

// Struktur eines Fensters
typedef struct {
    int x, y;
    int width, height;
    uint32_t bg_color;
    const char* title;
} Window;

// Zeichnet ein Fenster inklusive Titelzeile (20 Pixel hoch)
void draw_window(Window *win) {
    // Fensterhintergrund
    fill_rect(win->x, win->y, win->width, win->height, win->bg_color);
    // Titelleiste (blau)
    fill_rect(win->x, win->y, win->width, 20, 0x0000FF);
    // Fenster-Titel in der Titelleiste
    if (win->title) {
        draw_text(win->title, win->x + 5, win->y + 5, 0xFFFFFF);
    }
}

// Funktion, um den "Client-Bereich" (ohne Titelleiste) der Shell zu löschen
void clear_shell_window(Window *win) {
    // Lösche den Bereich unter der Titelleiste
    fill_rect(win->x, win->y + 20, win->width, win->height - 20, win->bg_color);
}

// --------------------------------------------------------
// Shell-Implementierung (ganz einfach)
// --------------------------------------------------------
#define SHELL_BUFFER_SIZE 128
char shell_buffer[SHELL_BUFFER_SIZE];
int shell_buffer_index = 0;

void execute_command(const char *cmd, Window *shell_win) {
    if(strcmp(cmd, "clear") == 0) {
        clear_shell_window(shell_win);
    } else if(strcmp(cmd, "help") == 0) {
        draw_text("Available commands: help, clear", shell_win->x + 5, shell_win->y + 45, 0xFFFFFF);
    } else {
        draw_text("Unknown command", shell_win->x + 5, shell_win->y + 45, 0xFFFFFF);
    }
}

void shell_handle_key(char key, Window *shell_win) {
    if(key == '\n' || key == '\r') {
        shell_buffer[shell_buffer_index] = '\0';
        execute_command(shell_buffer, shell_win);
        shell_buffer_index = 0;
        // Zeige Prompt neu
        clear_shell_window(shell_win);
        draw_text("ParaOS> ", shell_win->x + 5, shell_win->y + 25, 0xFFFFFF);
    } else if(key == '\b') {
        if(shell_buffer_index > 0) {
            shell_buffer_index--;
            clear_shell_window(shell_win);
            draw_text("ParaOS> ", shell_win->x + 5, shell_win->y + 25, 0xFFFFFF);
            draw_text(shell_buffer, shell_win->x + 5 + (CHAR_WIDTH * 8), shell_win->y + 25, 0xFFFFFF);
        }
    } else {
        if(shell_buffer_index < SHELL_BUFFER_SIZE - 1) {
            shell_buffer[shell_buffer_index++] = key;
            clear_shell_window(shell_win);
            draw_text("ParaOS> ", shell_win->x + 5, shell_win->y + 25, 0xFFFFFF);
            draw_text(shell_buffer, shell_win->x + 5 + (CHAR_WIDTH * 8), shell_win->y + 25, 0xFFFFFF);
        }
    }
}

// Dummy-Funktion für "refresh_screen" – hier kein doppeltes Buffering implementiert
void refresh_screen() {
    // In einem echten System könnten hier Aktualisierungen oder ein Doublebuffering erfolgen.
}

// Globaler Pointer auf das aktive Shell-Fenster, wird von keyboard_handler() genutzt
Window *active_shell = 0;

// --------------------------------------------------------
// Hauptfunktion: Initialisierung, Grafikmodus, Shell-Fenster
// --------------------------------------------------------
void main() {
    gdt_init();
    idt_init();
    pic_remap();
    init_timer();
    init_keyboard();

    // Wechsel in den Grafikmodus (z.B. VESA Mode 0x101: 640x480x256 Farben)
    set_video_mode(0x101);
    // Bildschirm löschen
    fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x000000);

    // Erzeuge ein Shell-Fenster
    Window shell_win = { .x = 50, .y = 50, .width = 540, .height = 380, .bg_color = 0x000000, .title = "ParaOS Shell" };
    draw_window(&shell_win);
    active_shell = &shell_win;  // Setze aktives Shell-Fenster

    // Shell-Prompt anzeigen
    draw_text("ParaOS> ", shell_win.x + 5, shell_win.y + 25, 0xFFFFFF);

    // Hauptloop
    while(1) {
        refresh_screen();
        asm volatile ("hlt");  // CPU-Wartezustand
    }
}
