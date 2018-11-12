#include "types.h"
#include "lib.h"
#include "paging.h"
#include "x86_desc.h"
#define ASM 1
#define _4MB_ (0x100000*4)
#define _8MB_ (0x100000*8)
#define program_pageIdx 32

/*
 *void pdt_init_kb(int idx)
 *     DESCRIPTION: PDT entery 4KB initialization 
 *     INPUTS: int idx
 *     OUTPUTS: none
 *     RETURN VALUE: none
 *     SIDE EFFECTS: initialize PDT idx entry as 4KB entry
 */
void pdt_init_kb(int pid, int idx){

       pdt_entry_t pdt_entry;
       pdt_entry.kb.pt_present  = 0;
       pdt_entry.kb.pt_rw       = 0;
       pdt_entry.kb.pt_us       = 0;
       pdt_entry.kb.pt_wthrough = 0;
       pdt_entry.kb.pt_cache_da = 0;
       pdt_entry.kb.pt_accessed = 0;
       pdt_entry.kb.pt_reserved = 0;
       pdt_entry.kb.pt_size     = 0;
       pdt_entry.kb.pt_global   = 0;
       pdt_entry.kb.pt_avail    = 0;
       pdt_entry.kb.pt_base_addr = 0;

    page_directory[idx] = pdt_entry;
}

/*
 *void pdt_init_mb(int idx)
 *     DESCRIPTION: PDT entery 4MB initialization 
 *     INPUTS: int idx
 *     OUTPUTS: none
 *     RETURN VALUE: none
 *     SIDE EFFECTS: initialize PDT idx entry as 4MB entry
 */
void pdt_init_mb(int pid, int idx){

    pdt_entry_t pdt_entry;
       pdt_entry.mb.pt_present  = 0;
       pdt_entry.mb.pt_rw       = 0;
       pdt_entry.mb.pt_us       = 0;
       pdt_entry.mb.pt_wthrough = 0;
       pdt_entry.mb.pt_cache_da = 0;
       pdt_entry.mb.pt_accessed = 0;
       pdt_entry.mb.pt_dirty    = 0;
       pdt_entry.mb.pt_size     = 1;
       pdt_entry.mb.pt_global   = 0;
       pdt_entry.mb.pt_avail    = 0;
       pdt_entry.mb.pt_attr     = 0;
       pdt_entry.mb.pt_reserved = 0;
       pdt_entry.mb.pt_base_addr = 0;

    page_directory[idx] = pdt_entry;
}

/*
 *void pt_init(int idx)
 *     DESCRIPTION: PT entery initialization 
 *     INPUTS: int idx
 *     OUTPUTS: none
 *     RETURN VALUE: none
 *     SIDE EFFECTS: initialize PT idx entry
 */
void pt_init(int idx){

      pt_entry_t pt_entry;
      pt_entry.page_present  = 0;
      pt_entry.page_rw       = 0;
      pt_entry.page_us       = 0;
      pt_entry.page_wthrough = 0;
      pt_entry.page_cache_da = 0;
      pt_entry.page_accessed = 0;
      pt_entry.page_dirty    = 0;
      pt_entry.page_attr     = 0;
      pt_entry.page_global   = 0;
      pt_entry.page_avail    = 0;
      pt_entry.page_addr     = 0;

      page_table[idx] = pt_entry;
}


/*
 *void paging_init()
 *     DESCRIPTION: PT and PDT initialization 
 *     INPUTS: none
 *     OUTPUTS: none
 *     RETURN VALUE: none
 *     SIDE EFFECTS:  
 *      1.creating a page directory and a page table with valid PDEs and PTEs. 
 *      2.0-4 MB of broken down into 4 kB pages, 4-8MB is single 4 MB page.
 *      3.Mark Video Mem present
 *      4.Set CR0, CR4 registers properly
 */
void paging_init(){

    int i;

    //create index 2-1023 PDEs
    for(i = 2; i < PAGE_ENTRY_SIZE; i++){
        pdt_init_mb(0,i);
         page_directory[i].mb.pt_base_addr = i;
    }

    //create all 1024 PTEs
    for(i = 0; i < PAGE_ENTRY_SIZE; i++){
        pt_init(i);
        page_table[i].page_present = 0;
        page_table[i].page_addr = i;
    }

    //create index 0-4MB as 4kB page directory entry
    pdt_init_kb(0, 0);
     page_directory[0].kb.pt_present = 1;
     page_directory[0].kb.pt_rw = 1;
    //assign page table base addr(right shift 12 bits) to PDE 0
     page_directory[0].kb.pt_base_addr = (uint32_t)page_table>>12; 

    //mark video mem present in corresponding page table entry
    page_table[PT_VIDEO].page_present = 1;
    page_table[PT_VIDEO].page_rw = 1;
    page_table[PT_VIDEO].page_cache_da = 1;
    page_table[PT_VIDEO].page_addr = PT_VIDEO;

    //create 4-8MB kernel as 4MB page directory entry
    pdt_init_mb(0, 1);
     page_directory[1].mb.pt_present = 1;
     page_directory[1].mb.pt_rw = 1;
     page_directory[1].mb.pt_base_addr = 1;

/*
    pdt_init_mb(pid, program_pageIdx);
    process[pid].page_directory[program_pageIdx].mb.pt_present = 1;
    process[pid].page_directory[program_pageIdx].mb.pt_size = 1;
    process[pid].page_directory[program_pageIdx].mb.pt_us = 1;
    process[pid].page_directory[program_pageIdx].mb.pt_rw = 1;
    process[pid].page_directory[program_pageIdx].mb.pt_base_addr = ((pid*_4MB_) + _8MB_)>>22;
*/
    printf("PAGE_INIT OK\n");

    __asm__ (
            //page_directory address to cr3
            "movl %0, %%cr3           \n"
            //set PSE(bit4) of cr4 4-MB page enable
            "movl %%cr4, %%ebx        \n"
            "orl $0x00000010, %%ebx   \n"
            "movl %%ebx, %%cr4        \n"

            //set PG(bit31) and protection(bit0) of cr0
            "movl %%cr0, %%ebx         \n"
            "orl $0x80000001, %%ebx   \n"
            "movl %%ebx, %%cr0"
            : // no output
            : "a"(page_directory)
            : "%ebx", "cc"
        );   
}



void flush_tlb(void){
	asm volatile(
                 "mov %%cr3, %%eax;"
                 "mov %%eax, %%cr3;"
                 :                      /* no outputs */
                 :                      /* no inputs */
                 :"%eax"                /* clobbered register */
                 );
}

void pid_page_map(int pid){


    //pdt_init_mb(0, program_pageIdx);
     page_directory[program_pageIdx].mb.pt_present = 1;
     page_directory[program_pageIdx].mb.pt_size = 1;
     page_directory[program_pageIdx].mb.pt_us = 1;
     page_directory[program_pageIdx].mb.pt_rw = 1;
     page_directory[program_pageIdx].mb.pt_base_addr = ((pid*_4MB_) + _8MB_)>>22;

    flush_tlb();

}



