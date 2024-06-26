#include "keyboard.h"


char keybuff[100];
uint8_t key_head = 0;
uint8_t key_tail = 0;

char keyboard[128] = {

    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
    'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 
    'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 
    'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,  ' ', 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, 
    '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


void init_keyboard() {
  
  key_head = key_tail = 0;

  irq_install_handler(1, &keyboard_handler);
}

/* Write input to screen */
void keyboard_handler() {
  
  // Wait until the input buffer is not empty
  while (!(inportb(KEYBOARD_CTRL_STATS_REG) & KEYBOARD_CTRL_STATS_MASK_OUT_BUF)) { }
  
  int scancode = inportb(KEYBOARD_ENC_INPUT_BUF);

  if (scancode & 0x80) { return; }
  key_push(keyboard[scancode]);
}


void key_push(char key) {

  key_head += (key_tail - key_head == KEYBUFF_SIZE);
  keybuff[key_tail++ % KEYBUFF_SIZE] = key;
}


char key_pop() { 

  if (key_tail == key_head) { return (char)0; }
  return keybuff[key_head++ % KEYBUFF_SIZE];
}


