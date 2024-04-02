#ifndef IRQS_H
#define IRQS_H

#include "Tables/idt.h"

#define MASTER		0x20		/* IO base address for master PIC */
#define SLAVE		0xA0		/* IO base address for slave PIC */
#define MASTER_COMMAND	MASTER
#define MASTER_DATA	(MASTER+1)
#define SLAVE_COMMAND	SLAVE
#define SLAVE_DATA	(SLAVE+1)

#define MASTER_OFFSET 32
#define SLAVE_OFFSET 40

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define PIC_EOI		0x20		/* End-of-interrupt command code */

void init_irq();
void irq_install_handler(uint8_t irq, void(*handler)(isr_stack_t* stack));
void irq_remove_handler(uint8_t irq);
void irq_handler(isr_stack_t* stack);

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */
void PIC_remap(int offset1, int offset2);


#endif

