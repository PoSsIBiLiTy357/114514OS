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

extern int32_t t_curr[3];
int8_t prog_timer = 0;
//static int curr_process;          // set in execute()?

//int PIT_ctr = 0;            // for testing delete later

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
    int32_t nxt_terminal;
    cli();
    /* Send EOI for PIT interrupt */
    send_eoi(PIT_IRQ);

//////////////////////Code for process switch////////////////////////////

    // if only 1st terminal running, return
    if(t_curr[1] == -1 && t_curr[2] == -1){
        return;
    }
    
    pcb_t *cur_pcb = (pcb_t *)(KSTACK_BOT - PCB_SIZE * curr);

    //find next terminal that need run process
    nxt_terminal = (cur_pcb->terminal+1)%3;
    while(t_curr[nxt_terminal] == -1){
        nxt_terminal = (nxt_terminal+1)%3;
    }

    pcb_t *nxt_pcb = (pcb_t *)(KSTACK_BOT - PCB_SIZE * t_curr[nxt_terminal]);

    //switch paging for next process(cr3)
    pid_page_map(nxt_pcb->pid);
    /* Update the tss.ss0/esp0 */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = KSTACK_START - (PCB_SIZE * nxt_pcb->pid) - MEM_FENCE;
    curr = nxt_pcb->pid;
    set_active_terminal(nxt_pcb->terminal);

    sti();

	asm volatile(
        "movl   %%ebp, %0   ;"
        "movl   %%esp, %1   ;"
        :"=r"(cur_pcb->current_ebp), "=r"(cur_pcb->current_esp)
        );

    asm volatile(
        "movl   %0, %%esp   ;"
        "movl   %1, %%ebp   ;"
        "LEAVE;"
        "RET;"
        : :"r"(nxt_pcb->current_esp), "r"(nxt_pcb->current_ebp) 
    );

//////////////////////////////////////////////////////////////////////// 

    return;

}
