/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
// reference https://wiki.osdev.org/PIC
 
 
#include "i8259.h"
#include "lib.h"

#define PIC1 
#define PIC2

#define MSTR_NUM_IRQS			8
#define MSTR_IRQ2_MASK			2

#define PIC_MSTR_MASK		 0xFB
#define PIC_SLAV_MASK		 0xFF


/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
	
	master_mask = PIC_MSTR_MASK;  
	slave_mask = PIC_SLAV_MASK;
	
	outb(ICW1, MASTER_8259_PORT);  // starts the initialization sequence (in cascade mode)
	outb(ICW1, SLAVE_8259_PORT);

	outb(ICW2_MASTER, MASTER_DATA_PORT);      // ICW2: Master PIC vector offset
	outb(ICW2_SLAVE, SLAVE_DATA_PORT);       // ICW2: Slave PIC vector offset

	outb(ICW3_MASTER,MASTER_DATA_PORT);      // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(ICW3_SLAVE,SLAVE_DATA_PORT);       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(ICW4,MASTER_DATA_PORT);
	outb(ICW4,SLAVE_DATA_PORT);
	
	outb(master_mask, MASTER_DATA_PORT); 
    outb(slave_mask, SLAVE_DATA_PORT);  

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
	
 
    if(irq_num < MSTR_NUM_IRQS) {
        port = MASTER_DATA_PORT;
		value = master_mask & ~(1 << irq_num);
		master_mask = value;
    } else {
        port = SLAVE_DATA_PORT;
        irq_num -= MSTR_NUM_IRQS;
		value = slave_mask & ~(1 << irq_num);
		slave_mask = value;
    }
    outb(value,port); 
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
 
    if(irq_num < MSTR_NUM_IRQS) {
        port = MASTER_DATA_PORT;
		 value = master_mask | (1 << irq_num);
		 master_mask = value;
    } else {
        port = SLAVE_DATA_PORT;
        irq_num -= MSTR_NUM_IRQS;
		value = slave_mask | (1 << irq_num);
		slave_mask = value;
    }
    outb(value, port);  	
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
	if(irq_num >= MSTR_NUM_IRQS){
		irq_num -= MSTR_NUM_IRQS;
		outb(EOI | irq_num, SLAVE_8259_PORT);
		outb(EOI | MSTR_IRQ2_MASK, MASTER_8259_PORT);
	} 
	outb(EOI | irq_num, MASTER_8259_PORT);
}
