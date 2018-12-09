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

#define PID_NEXT(x)                 (((x) + 1) % 6)

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
    // send_eoi(PIT_IRQ);  // delete this if uncommenting chunk below
    // sti();              // delete this if uncommenting chunk below
//////////////////////////////////////////////////////////////////////////////////////////

    int32_t pid;
    int8_t found_process = 0;

    /* Send EOI for PIT interrupt */
    send_eoi(PIT_IRQ);

    if (term2_running() != 1 && term3_running() != 1) {
        sti();  return;
    }

    /* Traverse all processes and find a different active process than current */
    pcb_t * next_pcb;
    for (pid = PID_NEXT(get_curr_pcb()->pid); pid != get_curr_pcb()->pid; pid = PID_NEXT(pid)) {
        if (curr_running(pid)) {
            next_pcb = (pcb_t *)(KSTACK_BOT - PCB_SIZE * pid);
            found_process = 1;
            break;
            //if (next_pcb->c_pid == -1) { break; }
        }
    }

    pcb_t * curr_pcb = (pcb_t *)(KSTACK_BOT - PCB_SIZE * get_curr_pcb()->pid);

    /* Exit and continue running current process if no new one was found */
    if (!found_process) {
        sti();  return;
    }

    /* Update the currently running process to new process and initialize paging */
    pid_page_map(next_pcb->pid);

    tss.ss0 = KERNEL_DS;
    tss.esp0 = KSTACK_BOT - (PCB_SIZE * next_pcb->pid) - MEM_FENCE;
    set_curr_process(next_pcb->pid);
    set_active_terminal(next_pcb->terminal);

    sti();


	asm volatile(
        "movl   %%ebp, %0   ;"
        "movl   %%esp, %1   ;"
        :"=r"(curr_pcb->current_ebp), "=r"(curr_pcb->current_esp)
    );



    asm volatile(
        "movl   %0, %%esp   ;"
        "movl   %1, %%ebp   ;"
        "LEAVE;"
        "RET;"

        : :"r"(next_pcb->current_esp), "r"(next_pcb->current_ebp)
    );

//////////////////////////////////////////////////////////////////////////////////////////



    // start 3 terms at once ???
    // if (PIT_ctr < 3 && PIT_ctr > 0) {
    //     PIT_ctr++;            // delete later
    //     printf("%d\n", PIT_ctr);
    //             execute((uint8_t *)"shell");
    //     sti();
    //     return;
    // }


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
    
   // PIT_ctr++;            // delete later

    //sti();
}
