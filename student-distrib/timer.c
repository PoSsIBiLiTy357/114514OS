/* timer.c - C file for PIT device. Contains functions for PIT
 *  initialization and interrupt handling. Heavy influence from:
 *  http://www.osdever.net/bkerndev/Docs/pit.htm
 */

#include "timer.h"

#define DESIRED_FREQ                50
#define PIT_CLOCK_FREQ              1193180
#define PIT_COMM_PORT               0x43
#define PIT_COMM_BYTE               0x36
#define PIT_CHANNEL_0               0x40
#define LSB_MASK                    0xFF
#define MSB_SHIFT                   8
#define PIT_IRQ                     0

int8_t prog_timer = 0;
//static int curr_process;          // set in execute()?

int PIT_ctr = 0;            // for testing delete later

/*
* pit_init
*   DESCRIPTION: Initializes the Programmable Interval Timer (PIT). Device is
*       initialized on IRQ 0 for the PIC. Command byte (0x36) is sent to 
*       PIT command port (0x43). Calculated frequency is written to channel
*       0 data register for PIT.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*/
void pit_init() {
    /* Calculate frequency */
    int32_t divisor;
    divisor = PIT_CLOCK_FREQ / DESIRED_FREQ;

    /* Enable PIT IRQs */
    enable_irq(PIT_IRQ);

    /* Send command byte to command port and frequency to channel 0 */
    outb(PIT_COMM_BYTE, PIT_COMM_PORT);
    outb(divisor & LSB_MASK, PIT_CHANNEL_0);
    outb(divisor >> MSB_SHIFT, PIT_CHANNEL_0);
}

void pit_int_handler() {
    cli();
    //int i;

     /* Send EOI for PIT interrupt */
    send_eoi(PIT_IRQ);

    // if (PIT_ctr  < 3 && PIT_ctr > 0) {
    //     PIT_ctr++;
    //     printf("%d", PIT_ctr);
    //     execute_with_terminal_num((uint8_t *)"shell",PIT_ctr,1);
    //     sti();
    //     return;
    // }

    // if (prog_timer) {
    //     /* Continue running a process */
    //     prog_timer--;
    //     sti();  
    //     return;
    // } else {
    //     /* Load in a different process to execute */
    //     for (i = 0; i < 6; i++) {
    //         curr_process += (curr_process + 1) % 6;

    //     }
    // }
    
    PIT_ctr++;            // delete later

    sti();
}
