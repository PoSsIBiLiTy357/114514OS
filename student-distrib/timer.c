/* timer.c - C file for PIT device. Contains functions for PIT
 *  initialization and interrupt handling. Heavy influence from:
 *  http://www.osdever.net/bkerndev/Docs/pit.htm
 */

#include "timer.h"

/* PIT initialization bytes and values*/
#define DESIRED_FREQ                50
#define PIT_CLOCK_FREQ              1193180
#define PIT_COMM_PORT               0x43
#define PIT_COMM_BYTE               0x36
#define PIT_CHANNEL_0               0x40
#define LSB_MASK                    0xFF
#define MSB_SHIFT                   8
#define PIT_IRQ                     0

#define NUM_TERMINALS               3
#define GET_NEXT_TERM(x)            (((x)+ 1) % NUM_TERMINALS)

extern int32_t t_curr[NUM_TERMINALS];
int8_t prog_timer = 0;

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

/*
* pit_int_handler
*   DESCRIPTION: 
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*/
void pit_int_handler() {
    int32_t nxt_terminal;
    cli();

    /* Send EOI for PIT interrupt */
    send_eoi(PIT_IRQ);

    /* Ensure that if the last terminal is ever closed a new shell is spawned */
    if(t_curr[0] == -1){
        clear();
        execute_with_terminal_num((uint8_t *)"shell", 0);
    }
    
    pcb_t *cur_pcb = GET_PCB(curr);

    /* Find next active terminal that needs to run a process */
    nxt_terminal = GET_NEXT_TERM(cur_pcb->terminal);
    while(t_curr[nxt_terminal] == -1){
        nxt_terminal = GET_NEXT_TERM(nxt_terminal);
    }

    pcb_t *nxt_pcb = GET_PCB(t_curr[nxt_terminal]);

    /* Switch paging for next process(cr3) */
    pid_page_map(nxt_pcb->pid);
    vidMem_page_map(132 * _MB_, nxt_pcb->terminal);

    /* Update the tss.ss0/esp0 */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = GET_ESP(nxt_pcb->pid);
    curr = nxt_pcb->pid;
    set_active_terminal(nxt_pcb->terminal);

    sti();

    /* Save currently running process's ebp and esp */
	asm volatile(
        "movl   %%ebp, %0   ;"
        "movl   %%esp, %1   ;"
        :"=r"(cur_pcb->current_ebp), "=r"(cur_pcb->current_esp)
    );

    /* Load in next process's ebp and esp to start running it */
    asm volatile(
        "movl   %0, %%esp   ;"
        "movl   %1, %%ebp   ;"
        "LEAVE;"
        "RET;"
        : :"r"(nxt_pcb->current_esp), "r"(nxt_pcb->current_ebp) 
    );

    return;

}
