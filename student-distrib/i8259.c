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
	
	master_mask=0xFB;  
	slave_mask=0xFF;
	
	outb(ICW1,0x20);  // starts the initialization sequence (in cascade mode)
	//io_wait();
	outb(ICW1,0xA0);
	//io_wait();
	outb(ICW2_MASTER,0x21);      // ICW2: Master PIC vector offset
	//io_wait();
	outb(ICW2_SLAVE,0xA1);       // ICW2: Slave PIC vector offset
	//io_wait();
	outb(ICW3_MASTER,0x21);      // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	//io_wait();
	outb(ICW3_SLAVE,0xA1);       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	//io_wait();
 
	outb(ICW4,0x21);
	//io_wait();
	outb(ICW4,0xA1);
	//io_wait();
	
	outb(master_mask, 0x21); 
    outb(slave_mask, 0xA1);  
		
	//master_mask=inb(0x21);  
	//slave_mask=inb(0xA1);
	
	//outb(master_mask,0x21);   // restore saved masks.
	//outb(slave_mask,0xA1);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
	
 
    if(irq_num < 8) {
        port = 0x21;
		value = master_mask & ~(1 << irq_num);
		master_mask = value;
    } else {
        port = 0xA1;
        irq_num -= 8;
		value = slave_mask & ~(1 << irq_num);
		slave_mask = value;
    }
    outb(value,port); 
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8) {
        port = 0x21;
		 value = master_mask | (1 << irq_num);
		 master_mask = value;
    } else {
        port = 0xA1;
        irq_num -= 8;
		value = slave_mask | (1 << irq_num);
		slave_mask = value;
    }
    outb(value,port);  	
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
	if(irq_num >= 8){
		irq_num-=8;
		outb(EOI|irq_num,SLAVE_8259_PORT);
		outb(EOI|2,MASTER_8259_PORT);
	} 
	outb(EOI|irq_num,MASTER_8259_PORT);
}
