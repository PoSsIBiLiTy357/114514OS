#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#include "lib.h"

#define PAGE_ENTRY_SIZE 1024


/* An page descriptor entry (goes into the PDT) */
typedef union pdt_entry_t {
    uint32_t val;

    struct kb {

        uint32_t pt_present  : 1;
        uint32_t pt_rw       : 1;
        uint32_t pt_us       : 1;
        uint32_t pt_wthrough : 1;
        uint32_t pt_cache_da : 1;
        uint32_t pt_accessed : 1;
        uint32_t pt_reserved : 1;
        uint32_t pt_size     : 1;
        uint32_t pt_global   : 1;
        uint32_t pt_avail    : 3;
        uint32_t pt_base_addr  : 20;

    }__attribute__ ((packed)) kb;


    struct mb {

        uint32_t pt_present  : 1;
        uint32_t pt_rw       : 1;
        uint32_t pt_us       : 1;
        uint32_t pt_wthrough : 1;
        uint32_t pt_cache_da : 1;
        uint32_t pt_accessed : 1;
        uint32_t pt_dirty    : 1;
        uint32_t pt_size     : 1;
        uint32_t pt_global   : 1;
        uint32_t pt_avail    : 3;
        uint32_t pt_attr     : 1;
        uint32_t pt_reserved : 9;
        uint32_t pt_base_addr  : 10;

    }__attribute__ ((packed)) mb;

} pdt_entry_t;



/* An page table entry (goes into the PT) */
typedef union pt_entry_t {
    uint32_t val;
    struct {

      uint32_t page_present  : 1;
      uint32_t page_rw       : 1;
      uint32_t page_us       : 1;
      uint32_t page_wthrough : 1;
      uint32_t page_cache_da : 1;
      uint32_t page_accessed : 1;
      uint32_t page_dirty    : 1;
      uint32_t page_attr     : 1;
      uint32_t page_global   : 1;
      uint32_t page_avail    : 3;
      uint32_t page_addr     : 20;


    } __attribute__ ((packed));
} pt_entry_t;

extern pte_entry_t page_table[PAGE_ENTRY_SIZE] __attribute__((aligned (4096)));
extern pdt_entry_t page_directory[PAGE_ENTRY_SIZE] __attribute__((aligned (4096)));
