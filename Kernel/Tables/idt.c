#include "idt.h"

static InterruptDescriptor idt[IDT_SIZE];
static InterruptDescriptorPointer idt_ptr;

void setup_idt() {
  
  // Initialize input for CPU LIDT instruction
  idt_ptr.limit = sizeof(InterruptDescriptor) * IDT_SIZE - 1;
  memset(idt, 0, sizeof(InterruptDescriptor) * IDT_SIZE);
  idt_ptr.offset = (uint32_t)&idt;

  load_idt();
}

void idt_create_gate(uint8_t index, uint32_t address, uint16_t select, uint8_t attributes) {

  idt[index].type_attributes = attributes;
  idt[index].offset_1  = (uint16_t)(address & 0xFFFF);   // Address' high bits
  idt[index].offset_2  = (uint16_t)(address >> 16);      // Address' low bits
  idt[index].selector   = select;
  idt[index].zero     = 0;
}

void load_idt()
{
  cpu_load_idt(&idt_ptr);
}