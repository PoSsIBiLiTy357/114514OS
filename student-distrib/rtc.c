
/* rtc.c - C file for RTC device. Contains functions for RTC
 *  initialization and interrupt handling. Heavy influence from:
 *  https://wiki.osdev.org/RTC
 *  Author: Andrew Chen
 *  Date: 10/20/18 
 */

#include "rtc.h"

/* Counter for how many times interrupt handler is reached */
int RTC_ctr = 0;

/* Flag indicating whether or not RTC has generated an interrupt */
volatile int RTC_flag = 0;

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

    /* Enable RTC to be on IRQ8 on the PIC */
    enable_irq(IRQ_RTC);

    /* Set clock frequency */
    outb(DISABLE_NMI | STATUS_REG_A, CMOS_ADDR);
    prev_A = inb(CMOS_DATA);
    outb(DISABLE_NMI | STATUS_REG_A, CMOS_ADDR);
    outb((prev_A & RATE_MASK) | FREQ_2_HZ, CMOS_DATA);

    /* Enabling IRQ 8 */
    outb(DISABLE_NMI | STATUS_REG_B, CMOS_ADDR);        /* Disable NMI in register B    */
    prev_B = inb(CMOS_DATA);                            /* read current value in reg B  */
    outb(DISABLE_NMI | STATUS_REG_B, CMOS_ADDR);        /* Set index again              */
    outb(prev_B | BIT_6, CMOS_DATA);                    /* Write prev value and 6th bit */

    /* initialize interrupt flag */
    RTC_flag = 0;
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
    //if (RTC_ctr == RTC_START_PRIN) { clear(); }
    //if (RTC_ctr++ >= RTC_START_PRIN) { RTC_test(); }

    RTC_ctr++;

    /* Set interrupt flag */
    RTC_flag = 1;

    /* Re-enable interrupts by discarding interrupt mask in register C */
    outb(STATUS_REG_C, CMOS_ADDR);              /* Select register C            */
    inb(CMOS_DATA);                             /* Discard interrupt mask       */

    /* Send EOI for RTC interrupt */
    send_eoi(IRQ_RTC);

    /* Re-enable other incoming interrupts */
    sti();
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
    char prev_A;

    /* Set clock frequency */
    outb(DISABLE_NMI | STATUS_REG_A, CMOS_ADDR);
    prev_A = inb(CMOS_DATA);
    outb(DISABLE_NMI | STATUS_REG_A, CMOS_ADDR);
    /* sets frequency to 2 Hz */
    outb((prev_A & RATE_MASK) | FREQ_2_HZ, CMOS_DATA); 

    return 0;
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
    /* Ummm.... I'm not sure what I should do here tbh. A-choo. Excuse me. */
    return 0;
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
    /* Block and wait for interrupt to be generated */
    while (!RTC_flag) {}

    /* Clear interrupt flag */
    RTC_flag = 0;

    return 0;
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
    /* Disable other incoming interrupts */
    cli();

    /* Exit and return -1 if passed in invalid parameters */
    if (buf == NULL || nbytes != sizeof(uint32_t)) { sti(); return -1; }

    char prev_A;
    uint32_t freq, bits, pow2;
    bits = 0;

    /* Initialize frequency and convert to proper rate divisor */
    freq = pow2 = *((uint32_t*)buf);
    convert_freq(&freq);

    /* Exit and return -1 if found to be invalid input */
    if (freq == -1) { sti(); return -1; }

    /* Set clock frequency */
    outb(DISABLE_NMI | STATUS_REG_A, CMOS_ADDR);
    prev_A = inb(CMOS_DATA);
    outb(DISABLE_NMI | STATUS_REG_A, CMOS_ADDR);

    /* sets frequency to whatever is passed in */
    outb((prev_A & RATE_MASK) | freq, CMOS_DATA); 

    /* Re-enable other incoming interrupts */
    sti();

    /* Return number of bytes written */
    return nbytes;
}


/*
* convert_freq
*   DESCRIPTION: Maps the desired integer frequency input to the RTC
*       divisor to set the frequency 
*
*   INPUTS: int32_t *freq -- desired integer frequency
*   OUTPUTS: none
*   RETURN VALUE:none
*   SIDE EFFECTS: 
*/
void convert_freq(uint32_t * freq) {
    /* Test input frequency against all possible valid frequencies */
    switch (*freq)
    {
        case VAL_2_HZ:
            *freq = FREQ_2_HZ;
            break;

        case VAL_4_HZ:
            *freq = FREQ_4_HZ;
            break;

        case VAL_8_HZ:
            *freq = FREQ_8_HZ;
            break;

        case VAL_16_HZ:
            *freq = FREQ_16_HZ;
            break;

        case VAL_32_HZ:
            *freq = FREQ_32_HZ;
            break;

        case VAL_64_HZ:
            *freq = FREQ_64_HZ;
            break;

        case VAL_128_HZ:
            *freq = FREQ_128_HZ;
            break;

        case VAL_256_HZ:
            *freq = FREQ_256_HZ;
            break;

        case VAL_512_HZ:
            *freq = FREQ_512_HZ;
            break;

        case VAL_1024_HZ:
            *freq = FREQ_1024_HZ;
            break;    

        /* Error case for when passed in invalid frequency */
        default:
            *freq = -1;
            break;
    }
}

