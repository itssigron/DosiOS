#include "hal.h"

uint8_t inportb(uint16_t port) {

  uint8_t ret;
  __asm__ __volatile__ ("inb %%dx, %%al":"=a" (ret):"d" (port));
   
  return ret;
}

uint16_t inportw(uint16_t port) {

  uint16_t ret;
  __asm__ __volatile__ ("inw %%dx, %%ax":"=a" (ret):"d" (port));
   
  return ret; 
}


void outportb(uint16_t port, uint8_t value) {  
  __asm__ __volatile__ ("outb %%al, %%dx": :"d" (port), "a" (value));
}


void outportw(uint16_t port, uint16_t value) {  
  __asm__ __volatile__ ("outw %%ax, %%dx": :"d" (port), "a" (value));
}

