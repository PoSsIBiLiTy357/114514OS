#include "lib.h"
#include "i8259.h"
#include "schedule.h"
#include "syscall.h"

extern curr;
extern proc_state;

void pit_init(){
    //////////////////////////////////////////
    //Andrew's pit_init() should be here
    ///////////content SWITCH/////////////////
}


void pit_catch(){

    //////////////////////////////////////////
    //MODs: syscall.c variable curr to syscall.h
    //      syscall.h struct pcb need record current esp ebp
    //      syscall.c excute need record esp ebp 
    ///////////content SWITCH/////////////////
    
    cli();
    send_eoi(0);

    //need if check is there any other process needs to run
    if(!proc_state[1]){
        sti(); 
        return;
    }
    ///////////////Code for process switch////////////////////////

    int32_t nxt_pid = (curr+1)%6; //need find runnable process in other terminal
    pcb_t *cur_pcb = (pcb_t *)(KSTACK_BOT - PCB_SIZE * curr);
    pcb_t *nxt_pcb = (pcb_t *)(KSTACK_BOT - PCB_SIZE * nxt_pid);/////process gonna run


    curr = nxt_pcb->pid;
    //switch paging for next process(cr3)
    pid_page_map(nxt_pcb->pid);
    /* Update the tss.ss0/esp0 */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = KSTACK_BOT - (PCB_SIZE * nxt_pid) - MEM_FENCE;
    sti();

    asm volatile(
        "movl   %%esp, %0   ;"
        "movl   %%ebp, %1   ;"
        :"r"(cur_pcb->esp), "r"(cur_pcb->ebp) : :
    );

    asm volatile(
        "movl   %0, %%esp   ;"
        "movl   %1, %%ebp   ;"
        "jmp RETURN_FROM_IRET;"

        : :"r"(nxt_pcb->esp), "r"(nxt_pcb->ebp) :
    );
    ////////////////////////////////////////////////////////////////
    return;

}


