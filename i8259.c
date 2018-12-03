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


/*
* i8259_init
*   DESCRIPTION: Initializes the Master and Slave PICs with the four
*		initialization control words as specified below.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS: writes initialization words to Master and Slave PICS
*/
void i8259_init(void) {
	/* Save current IMR bitmasks for Master and Slave PICS globally */
	master_mask = PIC_MSTR_MASK;  
	slave_mask = PIC_SLAV_MASK;
	
	/* Start initialization sequence in cascade mode for both PICs */
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);

	/* Writes high bits of vector offset for both PICs */
	outb(ICW2_MASTER, MASTER_DATA_PORT);
	outb(ICW2_SLAVE, SLAVE_DATA_PORT);

	/* Specifies Master what pins slaves are on (IRQ2 0000 0100) 	*/
	/* and tells Slave PIC its cascade identity (0000 0010)			*/
	outb(ICW3_MASTER,MASTER_DATA_PORT);
	outb(ICW3_SLAVE,SLAVE_DATA_PORT);
 
	/* ISA = x86, normal/auto EOI */
	outb(ICW4,MASTER_DATA_PORT);
	outb(ICW4,SLAVE_DATA_PORT);
	
	/* Writes IMR masks defined earlier to the two PICS */
	outb(master_mask, MASTER_DATA_PORT); 
    outb(slave_mask, SLAVE_DATA_PORT);  

}


/*
* enable_irq
*   DESCRIPTION: This function enables interrupts from a specified IRQ
*		line that is passed in. This is done by unmasking the IRQ line with a 
*		0 in the Interrupt Mask Register (IMR) in the correct PIC.
*
*   INPUTS: uint32_t irq_num -- number of IRQ line to ignore interrupts from
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS: writes a bitmask to a PIC based on which IRQ line was given
*/
void enable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
	
 	/* Check for if passed in IRQ is from Master or Slave */
    if(irq_num < MSTR_NUM_IRQS) {
		/* Save the Master PIC data port */
        port = MASTER_DATA_PORT;

		/* Calculate a bitmask for the specified by passed in IRQ	*/
		/* and global Master PIC mask then update this global mask	*/
		value = master_mask & ~(1 << irq_num);
		master_mask = value;
    } else {
		/* Save the Slave PIC data port and subtract 8 to convert */
		/* passed in IRQ into corresponding Slave IRQ			  */
        port = SLAVE_DATA_PORT;
        irq_num -= MSTR_NUM_IRQS;

		/* Calculate a bitmask for the specified by passed in IRQ	*/
		/* and global Slave PIC mask then update this global mask	*/
		value = slave_mask & ~(1 << irq_num);
		slave_mask = value;
    }

	/* Write IRQ bitmask to specified PIC (Master or Slave) */
    outb(value,port); 
}


/*
* disable_irq
*   DESCRIPTION: This function disables or ignores interrupts from a specified
*		IRQ line that is passed in. This is done by masking the IRQ line with a 
*		1 in the Interrupt Mask Register (IMR) in the correct PIC.
*
*   INPUTS: uint32_t irq_num -- number of IRQ line to ignore interrupts from
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS: writes a bitmask to a PIC based on which IRQ line was given
*/
void disable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
 
	/* Check for if passed in IRQ is from Master or Slave */
    if(irq_num < MSTR_NUM_IRQS) {
		/* Save the Master PIC data port */
        port = MASTER_DATA_PORT;

		/* Calculate a bitmask for the specified by passed in IRQ	*/
		/* and global Master PIC mask then update this global mask	*/
		value = master_mask | (1 << irq_num);
		master_mask = value;
    } else {
		/* Save the Slave PIC data port and subtract 8 to convert */
		/* passed in IRQ into corresponding Slave IRQ			  */
        port = SLAVE_DATA_PORT;
        irq_num -= MSTR_NUM_IRQS;

		/* Calculate a bitmask for the specified by passed in IRQ	*/
		/* and global Slave PIC mask then update this global mask	*/
		value = slave_mask | (1 << irq_num);
		slave_mask = value;
    }

	/* Write IRQ bitmask to specified PIC (Master or Slave) */
    outb(value, port);  	
}


/*
* send_eoi
*   DESCRIPTION: End of Interrupt (EOI 0x20) byte must be sent to the PIC
*		from the IRQ line the interrupt was handled. If the EOI is being sent
*		from an IRQ on the Master PIC, EOI only needs to be sent to MAaster PIC.
*		If EOI is being sent from an IRQ on the slave PIC, then EOI must be
*		sent to both master and slave PICs.
*
*   INPUTS: uint32_t irq_num -- number of IRQ line to send EOI from
*   OUTPUTS: none
*   RETURN VALUE: none
*	SIDE EFFECTS: sends EOI byte to specified IRQ to specified PIC.
*/
void send_eoi(uint32_t irq_num) {
	/* Check if EOI is being sent from a Master or Slave IRQ */
	if(irq_num >= MSTR_NUM_IRQS){
		/* Subtract by 8 so we address the correct corresponding Slave IRQ */
		irq_num -= MSTR_NUM_IRQS;

		/* Send EOI to both Master and Slave PICs */
		outb(EOI | irq_num, SLAVE_8259_PORT);
		outb(EOI | MSTR_IRQ2_MASK, MASTER_8259_PORT);
	}

	/* EOI is from a Master PIC IRQ, so only send EOI to master PIC */
	outb(EOI | irq_num, MASTER_8259_PORT);
}
