#include "terminal.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color = 0;
static uint16_t* terminal_buffer = 0;
 
/*
vga_entry_color generates a byte containing both given colors
Input: Character color, background color
Output: A byte, where the most significant half is the background color
and the least significant half is the character color
*/
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
/*
vga_entry generates a word containing a character and it's color attribute
Input: Character, color attribute of character
Output: A word, where the most significant half is the color attribute
and the least significant half is the character
*/
uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_initialize() {

  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_DARK_GRAY);

  terminal_buffer = (uint16_t*)VGA_BUFFER_ADDR;	// Virtual address of buffer (Physical address is 0xB8000)

  /* Cleaning the screen and setting background color */
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry('\0', terminal_color);
    }
  }

  terminal_color = vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_DARK_GRAY);

  cursor_update(0, 0); 	// Set cursor position to the start of the screen
}

/*
cursor_enable turns on the cursor in text mode with a given size
Input: Start of cursor, end of cursor
Output: None
*/
void cursor_enable(uint8_t cursor_start, uint8_t cursor_end) {

  /* Prepare and write beginning of cursor data */
  outportb(0x3D4, 0x0A); 
  outportb(0x3D5, (inportb(0x3D5) & 0xC0) | cursor_start); 	
 
  /* Prepare and write end of cursor data */
  outportb(0x3D4, 0x0B);
  outportb(0x3D5, (inportb(0x3D5) & 0xE0) | cursor_end);
}

/*
cursor_disable disables the current cursor in text mode
*/
void cursor_disable() {

  outportb(0x3D4, 0x0A);
  outportb(0x3D5, 0x20);
}

/*
cursor_update updates the coordinates of the cursor with a given new position
Input: Desired x coordinate of cursor, desired y coordinate of cursor
*/
void cursor_update(int x, int y) {

  uint16_t pos = y * VGA_WIDTH + x; 	// Calculate desired position with the linear data

  /* Input new y position */
  outportb(0x3D4, 0xF);
  outportb(0x3D5, (uint8_t)(pos & 0xFF));

  /* Input new x position */
  outportb(0x3D4, 0xE);
  outportb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));

  terminal_column = x;
  terminal_row = y;
}
 
/*
cursor_get_position gets the current cursor coordinates
Output: 2 byte position (x, y)
*/
uint16_t cursor_get_position() {

  uint16_t pos = 0;

  /* Get y position */
  outportb(0x3D4, 0x0F);
  pos |= inportb(0x3D5);

  /* Get x position */
  outportb(0x3D4, 0x0E);
  pos |= ((uint16_t)inportb(0x3D5)) << 8;

  return pos;
}

/*
terminal_setcolor sets the color of the terminal
Input: Color to set the terminal with
*/
void terminal_setcolor(vga_color color) {
  terminal_color = color;
}

void terminal_scroll(void) {
    // Scroll the buffer up by one row.
    const size_t buffer_size = (VGA_HEIGHT - 1) * VGA_WIDTH;

    memmove(terminal_buffer, terminal_buffer + VGA_WIDTH, buffer_size * 2);

    // Clear the last row.
    const size_t last_row_offset = VGA_HEIGHT - 1;
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_putentryat(' ', terminal_color, x, last_row_offset);
    }
}

// /*
// terminal_putentryat puts a character in a given position on the screen
// Input: Character to put, it's color, coords on screen
// */
void terminal_putentryat(char c, uint8_t color, const uint8_t x, const uint8_t y) 
{

  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, vga_entry_color(color, terminal_color));
}
 
/*
terminal_putchar puts a character at the current position on the screen
Intput: Character to put
*/
void terminal_putchar(char c) {
  // backspace - "delete" the last character
  if(c == '\b')
  {
    terminal_column--;
    terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
  }
  // newline - update row position and scroll if needed
  else if (c == '\n') {
      terminal_column = 0;
      if (++terminal_row == VGA_HEIGHT)
      {
        terminal_scroll();
        terminal_row--;
      }

  } else {
      // Otherwise, put the char at the current line and cursor position.
      terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

      // If the cursor is at the end of the line, then roll back to the
      // beggining
      if (++terminal_column == VGA_WIDTH) {
          terminal_column = 0;
          if (++terminal_row == VGA_HEIGHT) {
              terminal_scroll();
              terminal_row--;
          }
      }
  }

  // finally, update cursor position
  cursor_update(terminal_column, terminal_row);
}

void terminal_put_colored_char_at(char c, uint8_t color, const uint8_t x, const uint8_t y) {

  uint8_t tcolor = terminal_color;

  terminal_setcolor(color);
  terminal_putentryat(c, terminal_color, x, y);
  terminal_setcolor(tcolor);
}
 
/*
terminal_write prints a given amount of characters from a character array
to the current index on the screen

Input: Character array to write, amount to write
*/
void terminal_write(const char* data, size_t amount) {

  for (size_t i = 0; i < amount; i++) {
    terminal_putchar(data[i]);
  }

  cursor_update(terminal_column, terminal_row);
}

/*
terminal_write_string prints a character array to the screen
Input: Character array to print
*/
void terminal_write_string(const char* data) {
  terminal_write(data, strl(data));
}

/*

/*
terminal_draw_square draws a square to the screen
Input: X coordinate of left top corner, Y coordinate of left top corner, X coordinate of right bottom corner, Y coordinate of right bottom corner, color
*/
void terminal_draw_rec(const uint8_t x, const uint8_t y, const uint8_t destx, const uint8_t desty, const vga_color color) {

  for (uint8_t i = y; i < desty; i++) {
    for (uint8_t i2 = x; i2 < destx; i2++) {
      terminal_put_colored_char_at(' ', vga_entry_color(color, color), i2, i);
    }
  }
}

void terminal_draw_background(const vga_color color) {
  terminal_draw_rec(0, 0, VGA_WIDTH, VGA_HEIGHT, color);
}

void terminal_display_error(const char* error) {

  terminal_draw_background(VGA_COLOR_LIGHT_RED);
  terminal_color = VGA_COLOR_LIGHT_RED;

  size_t length = strl(error);
  
  terminal_row = VGA_HEIGHT / 2;
  terminal_column = VGA_WIDTH / 2 - length / 2;

  terminal_write_string(error);
}

