#include "types.h"
#include "lib.h"
#include "paging.h"

/*
void pdt_init(int idx){

       pdt_entry_t pdt_entry;
       page_directory[idx] = pdt_entry
       pdt_entry.pt_base_addr = 0;
       pdt_entry.pt_avail     = 0;
       pdt_entry.pt_global    = 0;
       pdt_entry.pt_size      = 0;
       pdt_entry.pt_reserved  = 0;
       pdt_entry.pt_accessed  = 0;
       pdt_entry.pt_cache_da  = 0;
       pdt_entry.pt_wthrough  = 0;
       pdt_entry.pt_us        = 0;
       pdt_entry.pt_rw        = 0;
       pdt_entry.pt_present   = 0;

}
*/
void pdt_init_mb(int idx){

       pdt_entry_t pdt_entry;
       page_directory[idx] = pdt_entry
       pdt_entry.pt_present = 0;
       pdt_entry.pt_rw     = 0;
       pdt_entry.pt_us    = 0;
       pdt_entry.pt_wthrough      = 0;
       pdt_entry.pt_cache_da  = 0;
       pdt_entry.pt_accessed  = 0;
       pdt_entry.pt_dirty  = 0;
       pdt_entry.pt_size  = 1;
       pdt_entry.pt_global        = 0;
       pdt_entry.pt_avail        = 0;
       pdt_entry.pt_attr   = 0;
       pdt_entry.pt_reserved   = 0;
       pdt_entry.pt_base_addr   = 0;
}


void pt_init(int idx){

      pt_entry_t pt_entry;
      page_table[idx] = pt_entry;
      pt_entry.page_addr     = 0;
      pt_entry.page_avail    = 0;
      pt_entry.page_global   = 0;
      pt_entry.page_attr     = 0;
      pt_entry.page_dirty    = 0;
      pt_entry.page_accessed = 0;
      pt_entry.page_cache_da = 0;
      pt_entry.page_wthrough = 0;
      pt_entry.page_us       = 0;
      pt_entry.page_rw       = 0;
      pt_entry.page_present  = 0;

}


void pdt_init(){

    int i;
    for(i = 1; i < PAGE_ENTRY_SIZE; i++){
        pdt_init_mb(i);
        page_directory[i].pt_base_addr = i*1024;
        page_directory[i].pt_size = 1;
        page_directory[i].pt_rw = 1;
    }

    for(i = 0; i < PAGE_ENTRY_SIZE; i++){
        pt_init(i);
        page_table[i].page_addr = i;
        page_table[i].page_rw = 1;
    }

    //4kB page directory table
    page_directory[0].pt_present = 1;
    page_directory[0].pt_size = 0;
    //mark video mem present in page table
    page_table[0xB8000].page_present = 1;

    //4MB kernel page
    page_directory[1].pt_global = 1;
    page_directory[1].pt_size = 1;
    page_directory[1].pt_present = 1;

/* the PSE flag in CR4 is set, both 4-MByte pages and page tables for 4-KByte pages can
be accessed from the same page directory
*/
asm(



);

}
