#ifndef TERMINAL_H
#define TERMINAL_H

#include "System/hal.h"
#include "utils.h"

#define VGA_BUFFER_ADDR 0xC03FF000
#define VGA_BUFFER_PHY_ADDR 0xB8000

/* Hardware text mode color constants. */
typedef enum vga_color {

  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GRAY = 7,
  VGA_COLOR_DARK_GRAY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_YELLOW = 14,
  VGA_COLOR_WHITE = 15,

} vga_color;


uint8_t vga_entry_color(vga_color ccolor, vga_color bcolor);
uint16_t vga_entry(unsigned char c, uint8_t color);

void terminal_initialize();
void terminal_setcolor(vga_color color);
void terminal_putentryat(char c, uint8_t color, const uint8_t x, const uint8_t y);
void terminal_putchar(char c);
void terminal_put_colored_char_at(char c, uint8_t color, const uint8_t x, const uint8_t y);
void terminal_write(const char* data, size_t amount);
void terminal_write_string(const char* data);
void terminal_draw_rec(const uint8_t x, const uint8_t y, const uint8_t destx, const uint8_t desty, const vga_color color);
void terminal_draw_background(const vga_color color);
void terminal_display_error(const char* error);

void cursor_enable(uint8_t cursor_start, uint8_t cursor_end);
void cursor_disable();
void cursor_update(int x, int y);

uint16_t cursor_get_position();


#endif

