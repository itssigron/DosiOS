#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include "System/hal.h"
#include "Memory/heap.h"

#define GDT_SIZE 6

typedef struct GlobalDescriptor {

  uint16_t limit;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t access;
  uint8_t flags_and_limit;
  uint8_t base_high;

} __attribute__((packed)) GlobalDescriptor;

typedef struct GlobalDescriptorPointer {

  uint16_t limit;
  uint32_t offset;

} __attribute__((packed)) GlobalDescriptorPointer;

void setup_gdt();
void gdt_create_gate(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

#endif

