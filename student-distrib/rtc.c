/* rtc.c - C file for RTC device. Contains functions for RTC
 *  initialization and interrupt handling.
 *  Author: Andrew Chen
 *  Date: 10/20/18 
 */

#include "rtc.h"

/* Counter for how many times interrupt handler is reached */
int RTC_ctr = 0;

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
    outb((prev_A & RATE_MASK) | DIV_FREQ_2, CMOS_DATA); /* modifies frequency, needs work */

    /* Enabling IRQ 8 */
    outb(DISABLE_NMI | STATUS_REG_B, CMOS_ADDR);        /* Disable NMI in register B    */
    prev_B = inb(CMOS_DATA);                            /* read current value in reg B  */
    outb(DISABLE_NMI | STATUS_REG_B, CMOS_ADDR);        /* Set index again              */
    outb(prev_B | 0x40, CMOS_DATA);                     /* Write prev value and 6th bit */

    /* Re-enable other incoming interrupts */
    //sti();
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
    if (RTC_ctr == RTC_START_PRIN) { clear(); }
    if (RTC_ctr++ >= RTC_START_PRIN) { RTC_test(); }

    /* Re-enable interrupts by discarding interrupt mask in register C */
    outb(STATUS_REG_C, CMOS_ADDR);              /* Select register C            */
    inb(CMOS_DATA);                             /* Discard interrupt mask       */

    /* Send EOI for RTC interrupt */
    send_eoi(IRQ_RTC);

    /* Re-enable other incoming interrupts */
    //sti();
}


/*
* rtc_open
*   DESCRIPTION: Initializes RTC frequendy to 2 Hz
*
*   INPUTS: const uint8_t * filename -- driver file for RTC
*   OUTPUTS: return value
*   RETURN VALUE: 0 on success, -1 for invalid filename
*/
int32_t rtc_open(const uint8_t * filename) {

}


/*
* rtc_close
*   DESCRIPTION: Closes passed in file descriptor
*
*   INPUTS: int32_t fd -- file descriptor
*   OUTPUTS: return value
*   RETURN VALUE: 0 on success, -1 for invalid filename
*/
int32_t rtc_close(int32_t fd) {

}


/*
* rtc_read
*   DESCRIPTION: Blocks until next interrupt
*
*   INPUTS: int32_t fd -- file descriptor
*           void * buf -- ignore this 
*           int32_t nbytes -- ignore this
*   OUTPUTS: return 0
*/
int32_t rtc_read(int32_t fd, void * buf, int32_t nbytes) {

}


/*
* rtc_write
*   DESCRIPTION: Changes frequency of RTC
*
*   INPUTS: int32_t fd -- file descriptor
*           void * buf -- contains the new frequency
*           int32_t bytes -- size of frequency (4 bytes)
*   OUTPUTS: return 0 on success, -1 on failure
*/
int32_t rtc_write(int32_t fd, const void * buf, int32_t nbytes) {

}
