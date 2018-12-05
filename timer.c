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
#define PIT_IRQ_NUM                 0

void pit_init() {
    /* Calculate frequency */
    int32_t divisor;
    divisor = PIT_CLOCK_FREQ / DESIRED_FREQ;

    /* Enable PIT IRQs */
    enable_irq(PIT_IRQ_NUM);

    /* Send command byte to command port and frequency to channel 0 */
    outb(PIT_COMM_BYTE, PIT_COMM_PORT);
    outb(divisor & LSB_MASK, PIT_CHANNEL_0);
    outb(divisor >> MSB_SHIFT, PIT_CHANNEL_0);
}

void pit_int_handler() {

}
