#include "gdt.h"

static GlobalDescriptor gdt[GDT_SIZE];
static GlobalDescriptorPointer gdt_ptr;

void setup_gdt() {
  // Initialize input for CPU LGDT instruction
  gdt_ptr.limit = sizeof(GlobalDescriptor) * GDT_SIZE - 1;
  memset(gdt, 0, sizeof(GlobalDescriptor) * GDT_SIZE);
  gdt_ptr.offset = (uint32_t)&gdt;

  /* Install null gate as well as kernel data and code segments */
  gdt_create_gate(0, 0, 0, 0, 0);   // First gate is ignored by the CPU
  gdt_create_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel code
  gdt_create_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel data
  gdt_create_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User code
  gdt_create_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User data

  cpu_load_gdt(&gdt_ptr);
}

void gdt_create_gate(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
  
  gdt[index].base_low = base & 0xFFFF;
  gdt[index].base_mid = (base >> 16) & 0xFF;
  gdt[index].base_high = base >> 24;
  gdt[index].limit = limit & 0xFFFF;
  gdt[index].flags_and_limit = (limit >> 16) & 0x0F;
  gdt[index].flags_and_limit |= granularity & 0xF0;   // Only the higher half bits
  gdt[index].access = access;
}
