/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
// reference https://wiki.osdev.org/PIC
 
 
#include "i8259.h"
#include "lib.h"

#define PIC1 
#define PIC2


/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
	
	master_mask=inb(0x21);  
	slave_mask=inb(0xA1);
	
	outb(0x20, ICW1);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(0xA0, ICW1);
	io_wait();
	outb(0x21, ICW2_MASTER);      // ICW2: Master PIC vector offset
	io_wait();
	outb(0xA1, ICW2_SLAVE);       // ICW2: Slave PIC vector offset
	io_wait();
	outb(0x21, ICW3_MASTER);      // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(0xA1, ICW3_SLAVE);       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(0x21, ICW4);
	io_wait();
	outb(0xA1, ICW4);
	io_wait();
	
	outb(0x21, master_mask);   // restore saved masks.
	outb(0xA1, slave_mask);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8) {
        port = 0x21;
    } else {
        port = 0xA1;
        irq_num -= 8;
    }
    value = inb(port) & ~(1 << irq_num);
    outb(port, value); 
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8) {
        port = 0x21;
    } else {
        port = 0xA1;
        irq_num -= 8;
    }
    value = inb(port) | (1 << irq_num);
    outb(port, value);  	
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
	if(irq_num >= 8){
		outb(SLAVE_8259_PORT,EOI);
	} 
	outb(MASTER_8259_PORT,EOI);
}
