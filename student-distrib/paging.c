#include "types.h"
#include "lib.h"
#include "paging.h"
#define ASM 1

#define PAGE_ENTRY_SIZE 1024
pt_entry_t page_table[PAGE_ENTRY_SIZE] __attribute__((aligned (4096)));
pdt_entry_t page_directory[PAGE_ENTRY_SIZE] __attribute__((aligned (4096)));


void pdt_init_kb(int idx){

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

void pdt_init_mb(int idx){

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


void paging_init(){

    int i;

    //4kB page directory table
    pdt_init_kb(0);
    page_directory[0].kb.pt_present = 1;
    page_directory[0].kb.pt_rw = 1;
    page_directory[0].kb.pt_base_addr = (uint32_t)page_table>>12;

    //4MB kernel page
    pdt_init_mb(1);
    page_directory[1].mb.pt_global = 1;
    page_directory[1].mb.pt_present = 1;
    page_directory[1].mb.pt_base_addr = 1;

    for(i = 2; i < PAGE_ENTRY_SIZE; i++){
        pdt_init_mb(i);
        page_directory[i].mb.pt_base_addr = i;
        page_directory[i].mb.pt_rw = 1;
    }

    for(i = 0; i < PAGE_ENTRY_SIZE; i++){
        pt_init(i);
        page_table[i].page_addr = i;
    }

    //mark video mem present in page table
    page_table[0xB8].page_present = 1;
    page_table[0xB8].page_rw = 1;
    page_table[0xB8].page_cache_da = 1;



    __asm__ (

              //page_directory address to cr3
              "leal page_directory,%eax; \n"
              "movl %eax,%cr3;           \n"

              //set PSE(bit4) of cr4 4-MB page enable
              "movl %cr4, %eax;          \n"
              "orl $0x00000010, %eax;    \n"
              "movl %eax, %cr4;          \n"

              //set PG(bit31) and protection(bit0) of cr0
              "movl %cr0,%eax;           \n"
              "orl $0x80000001, %eax;    \n"
              "movl %eax,%cr0;           \n"
              :/* no output */
              : "a"((page_directory+0))
              : "%eax"
    );

}
