#include "lib.h"
#include "i8259.h"
#include "schedule.h"

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
    //if(only one process) sti(); return?? 


    ///////////////Code for process switch////////////////////////
    pcb_t *cur_pcb = (pcb_t *)(KSTACK_BOT - PCB_SIZE * curr);
    pcb_t *nxt_pcb = (pcb_t *)(KSTACK_BOT - PCB_SIZE * (curr + 1));/////process gonna run

    //tss.esp0 = KSTACK_BOT - PCB_SIZE * curr - 4;  

    curr = nxt_pcb->pid;
    //switch paging for next process(cr3)
    pid_page_map(nxt_pcb->pid);
    /* Update the tss.ss0/esp0 */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = //assign to nxt_pcb esp
    sti();

    asm volatile(
        "movl   %%esp, %0   ;"
        "movl   %%ebp, %1   ;"
        :"r"(cur_pcb->nxt_esp), "r"(cur_pcb->nxt_ebp) : :
    );

    asm volatile(
        "movl   %0, %%esp   ;"
        "movl   %1, %%ebp   ;"
        "jmp RETURN_FROM_IRET;"

        : :"r"(nxt_pcb->parent_esp), "r"(nxt_pcb->parent_ebp) :
    );
    ////////////////////////////////////////////////////////////////
    return 0;

}


