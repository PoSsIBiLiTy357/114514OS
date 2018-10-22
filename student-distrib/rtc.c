/* rtc.c - C file for RTC device. Contains functions for RTC
 *  initialization and interrupt handling.
 *  Author: Andrew Chen
 *  Date: 10/20/18 
 */

#include "rtc.h"

int clr_flag = 0;


/*
* rtc_init
*   DESCRIPTION: Initializes the real time clock by initializng the clock frequency. 
*       This is done by disabling Non-Maskable Interrupts (NMI) in the CMOS (which
*       contains the RTC in ports 0x70 as a register number of index and 0x71 for R/W
*       data). Frequency is set by writing the divisor into lower bits of register A.
*       We then enable the RTC on IRQ line 8 on the PIC by retrieving hte previous 
*       value and setting the 6th bit in register B.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*/
void rtc_init() {
    char prev_A;
    char prev_B;

    /* Disable other incoming interrupts */
    cli();

    /* Enable RTC to be on IRQ8 on the PIC */
    enable_irq(IRQ_RTC);

    /* Set clock frequency */
    outb(DISABLE_NMI | STATUS_REG_A, CMOS_ADDR);
    prev_A = inb(CMOS_DATA);
    outb(DISABLE_NMI | STATUS_REG_A, CMOS_ADDR);
    outb((prev_A & RATE_MASK) | DIV_FREQ_2, CMOS_DATA);

    /* Enabling IRQ 8 */
    outb(DISABLE_NMI | STATUS_REG_B, CMOS_ADDR);        /* Disable NMI in register B    */
    prev_B = inb(CMOS_DATA);                            /* read current value in reg B  */
    outb(DISABLE_NMI | STATUS_REG_B, CMOS_ADDR);        /* Set index again              */
    outb(prev_B | 0x40, CMOS_DATA);                     /* Write prev value and 6th bit */

    /* Re-enable other incoming interrupts */
    sti();
}


/*
* rtc_int_handler
*   DESCRIPTION: Interrupt handler for RTC interrupts. Should call a test
*       functoin somehow. Needs work.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*/
void rtc_int_handler() {
    
    /* Disable other incoming interrupts */
    cli();

    /* Clear screen only once and call RTC_tests() defined in tests.c */
    //if (clr_flag++ == 0) { clear(); }
    //RTC_test();

    /* Re-enable interrupts by discarding interrupt mask in register C */
    outb(STATUS_REG_C, CMOS_ADDR);              /* Select register C            */
    inb(CMOS_DATA);                             /* Discard interrupt mask       */

    /* Send EOI for RTC interrupt */
    send_eoi(IRQ_RTC);

    /* Re-enable other incoming interrupts */
    sti();
}
