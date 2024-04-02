#ifndef IDT_H
#define IDT_H

#include "utils.h"

#define IDT_SIZE 256

/* Gate types
0b1110 or 0xE: 32-bit Interrupt Gate
*/

enum {
    IDT_GATE = 0x8E
};

/* Descriptor table entry */
typedef struct InterruptDescriptor {
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  zero;            // unused, set to 0
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
}  __attribute__((packed)) InterruptDescriptor;

typedef struct InterruptDescriptorPointer {

  uint16_t limit;
  uint32_t offset;
  
} __attribute__((packed)) InterruptDescriptorPointer;

/* The stack after an interrupt service routine was running */
typedef struct ISR_STACK_REGS_STRUCT {

  unsigned int gs, fs, es, ds;                          /* pushed the segs last */
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
  unsigned int index, error_code;                       /*'push byte #' and ecodes do this */
  unsigned int eip, cs, eflags, useresp, ss;            /* pushed by the processor automatically */ 

} isr_stack_t;

void setup_idt();
void idt_create_gate(uint8_t index, uint32_t address, uint16_t select, uint8_t attributes);
void load_idt();

#endif

