#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#include "lib.h"

#define PAGE_ENTRY_SIZE 1024
/* An page descriptor entry (goes into the PDT) */
typedef union pdt_entry_t {
    uint32_t val;
    struct {
        uint32_t page_present : 1;
        uint32_t page_ignore : 1;
        uint32_t page_rw : 1;
        uint32_t page_us      : 1;
        uint32_t page_cache_da : 1;
        uint32_t page_accessed       : 1;
        uint32_t page_size : 1;
        uint32_t page_wthrough : 1;
        uint32_t page_avail : 3;
        uint32_t page_pt_addr : 20;

    } __attribute__ ((packed));
} pdt_entry_t;


/* An page table entry (goes into the PT) */
typedef union pte_entry_t {
    uint32_t val;
    struct {
        uint32_t page_present : 1;
        uint32_t page_rw : 1;
        uint32_t page_us      : 1;
        uint32_t page_global : 1;
        uint32_t page_dirty : 1;
        uint32_t page_accessed       : 1;
        uint32_t page_cache_da : 1;
        uint32_t page_wthrough : 1;
        uint32_t page_avail : 3;
        uint32_t page_pa_addr 20;

    } __attribute__ ((packed));
} pte_entry_t;

extern pte_entry_t page_table[PAGE_ENTRY_SIZE] __attribute__((aligned (4096)));
extern pdt_entry_t page_directory[PAGE_ENTRY_SIZE] __attribute__((aligned (4096)));
