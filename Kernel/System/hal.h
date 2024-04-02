// Hardware Abstraction Layer
#ifndef HAL_H
#define HAL_H

#include <stdint.h>

uint8_t inportb(uint16_t port);
uint16_t inportw(uint16_t port);

void outportw(uint16_t port, uint16_t value);
void outportb(uint16_t port, uint8_t value);

static inline void cpu_load_idt(void* addr) {
  __asm__ ("lidt (%0)\n" :: "r" (addr));
}

static inline void cpu_load_gdt(void* addr) {
  __asm__ ("lgdt (%0)\n" :: "r" (addr));
}

/* Set interrupts */
static inline void sti() {
  __asm__ ("sti");
}

/* Clear interrupts (disable them) */
static inline void cli() {
  __asm__ ("cli");
}

#endif

