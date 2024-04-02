#include "irqs.h"

uint32_t irq_disable_counter = 0;

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void* irq_routines[16] = { NULL };

void irq_install_handler(uint8_t irq, void(*handler)(isr_stack_t* stack)) {
  irq_routines[irq] = handler;
}

void irq_remove_handler(uint8_t irq) {
  irq_routines[irq] = NULL;
}

void PIC_remap(int offset1, int offset2)
{
	uint8_t a1, a2;
 
	a1 = inportb(MASTER_DATA);                        // save masks
	a2 = inportb(SLAVE_DATA);
 
	outportb(MASTER_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outportb(SLAVE_COMMAND, ICW1_INIT | ICW1_ICW4);
	outportb(MASTER_DATA, offset1);                 // ICW2: Master PIC vector offset
	outportb(SLAVE_DATA, offset2);                 // ICW2: Slave PIC vector offset
	outportb(MASTER_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outportb(SLAVE_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	outportb(MASTER_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	outportb(SLAVE_DATA, ICW4_8086);
 
	outportb(MASTER_DATA, a1);   // restore saved masks.
	outportb(SLAVE_DATA, a2);
}

void init_irq() {
  int masterOffset = MASTER_OFFSET;
  int slaveOffset = SLAVE_OFFSET;

  memset(irq_routines, 0, sizeof(irq_routines));
  
  PIC_remap(masterOffset, slaveOffset);
  
  // 8 master gates ------------->
  
  idt_create_gate(masterOffset++, (uint32_t)irq0, 0x8, IDT_GATE); // system timer
  idt_create_gate(masterOffset++, (uint32_t)irq1, 0x8, IDT_GATE); // keyboard on PS/2 port
  idt_create_gate(masterOffset++, (uint32_t)irq2, 0x8, IDT_GATE); // cascaded signals from IRQs 8–15 (any devices configured to use IRQ 2 will actually be using IRQ 9)
  idt_create_gate(masterOffset++, (uint32_t)irq3, 0x8, IDT_GATE); // serial port controller for serial port 2 (shared with serial port 4, if present)
  idt_create_gate(masterOffset++, (uint32_t)irq4, 0x8, IDT_GATE); // serial port controller for serial port 1 (shared with serial port 3, if present)
  idt_create_gate(masterOffset++, (uint32_t)irq5, 0x8, IDT_GATE); // parallel port 3 or sound card
  idt_create_gate(masterOffset++, (uint32_t)irq6, 0x8, IDT_GATE); // floppy disk controller
  // parallel port 1 (shared with parallel port 2, if present).
  // It is used for printers or for any parallel port if a printer is not present.
  // It can also be potentially be shared with a secondary sound card with careful management of the port.
  idt_create_gate(masterOffset, (uint32_t)irq7, 0x8, IDT_GATE);

  // 8 slave gates ------------->

  idt_create_gate(slaveOffset++, (uint32_t)irq8, 0x8, IDT_GATE); // real-time clock (RTC)
  // Advanced Configuration and Power Interface (ACPI) system control interrupt on Intel chipsets
  // Other chipset manufacturers might use another interrupt for this purpose,
  // or make it available for the use of peripherals (any devices configured to use IRQ 2 will actually be using IRQ 9)
  idt_create_gate(slaveOffset++, (uint32_t)irq9, 0x8, IDT_GATE);
  idt_create_gate(slaveOffset++, (uint32_t)irq10, 0x8, IDT_GATE); // The Interrupt is left open for the use of peripherals (open interrupt/available, SCSI or NIC)
  idt_create_gate(slaveOffset++, (uint32_t)irq11, 0x8, IDT_GATE); // The Interrupt is left open for the use of peripherals (open interrupt/available, SCSI or NIC)
  idt_create_gate(slaveOffset++, (uint32_t)irq12, 0x8, IDT_GATE); // mouse on PS/2 port
  idt_create_gate(slaveOffset++ , (uint32_t)irq13, 0x8, IDT_GATE); // CPU co-processor or integrated floating point unit or inter-processor interrupt (use depends on OS)
  idt_create_gate(slaveOffset++, (uint32_t)irq14, 0x8, IDT_GATE); // primary ATA channel (ATA interface usually serves hard disk drives and CD drives)
  idt_create_gate(slaveOffset, (uint32_t)irq15, 0x8, IDT_GATE); // secondary ATA channel

  // reload table
  load_idt();
}



/* Handle a default interrupt request */
void irq_handler(isr_stack_t* stack) {

  // Calculate irq_routines index from the stack index which has the offset in it
  uint32_t index = (stack->index & 0xFF) - MASTER_OFFSET;
  
  void (*handler)(isr_stack_t* stack) = irq_routines[index]; 
  
  if (handler) { (*handler)(stack); }

  /* If we are trying to access an IRQ that belongs to the second PIC, send an End of Interrupt command to it */
  if (stack->index >= SLAVE_OFFSET) { outportb(SLAVE_COMMAND, PIC_EOI); }
  
  /* In any case send an End of Interrupt command to the first PIC */
  outportb(MASTER_COMMAND, PIC_EOI);
}

