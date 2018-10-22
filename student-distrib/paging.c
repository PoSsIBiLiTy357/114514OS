#include "types.h"
#include "lib.h"
#include "paging.h"


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

       page_directory[idx] = pdt_entry

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
    for(i = 2; i < PAGE_ENTRY_SIZE; i++){
        pdt_init_kb(i);
        page_directory[i].mb.pt_base_addr = i*1024;
        page_directory[i].mb.pt_size = 1;
        page_directory[i].mb.pt_rw = 1;
    }

    for(i = 0; i < PAGE_ENTRY_SIZE; i++){
        pt_init(i);
        page_table[i].page_addr = i;
        page_table[i].page_rw = 1;
    }

    //4kB page directory table
    pdt_init_kb(0);
    page_directory[0].kb.pt_present = 1;
    page_directory[0].kb.pt_size = 0;
    //mark video mem present in page table
    page_table[0xB8000].page_present = 1;

    //4MB kernel page
    page_directory[1].mb.pt_global = 1;
    page_directory[1].mb.pt_size = 1;
    page_directory[1].mb.pt_present = 1;

/* the PSE flag in CR4 is set, both 4-MByte pages and page tables for 4-KByte pages can
be accessed from the same page directory
*/
    
    //1. page_dir address to cr3
    //2. set paging and protection bits of cr0
    //3. set cr4 to enable 4bits
    //0x00000010 for enabling 4mb size
    //0x80000001 for enabling protected mode and paging mode
    __asm__ ( "leal page_dir,%eax;"
              "movl %eax,%cr3;"            
              
              "movl %cr4, %eax;"
              "orl $0x00000010, %eax;"
              "movl %eax, %cr4;"
              
              "movl %cr0,%eax;"
              "orl $0x80000001, %eax;"
              "movl %eax,%cr0;"
    );

}
