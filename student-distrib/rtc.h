/* rtc.h - Header file for RTC device. Contains basic function declarations.
 *  Author: Andrew Chen
 *  Date: 10/20/18 
 */

#ifndef _RTC_H
#define _RTC_H

#include "lib.h"
#include "i8259.h"
#include "tests.h"

/* RTC status registers */
#define STATUS_REG_A    0x0A
#define STATUS_REG_B    0x0B
#define STATUS_REG_C    0x0C

/* RTC init ports and other macros */
#define IRQ_RTC            8
#define CMOS_ADDR       0x70
#define CMOS_DATA       0x71
#define DISABLE_NMI     0x80
#define BIT_6           0x40

/* calculated frequency change */
#define RATE_MASK       0xF0
#define FREQ_2_HZ       0x0F
#define FREQ_4_HZ       0x0E
#define FREQ_8_HZ       0x0D
#define FREQ_16_HZ      0x0C
#define FREQ_32_HZ      0x0B
#define FREQ_64_HZ      0x0A
#define FREQ_128_HZ     0x02
#define FREQ_256_HZ     0x01
#define FREQ_512_HZ     0x07
#define FREQ_1024_HZ    0x06

/* Macros for possible input frequencies */
#define VAL_2_HZ          2
#define VAL_4_HZ          4
#define VAL_8_HZ          8
#define VAL_16_HZ        16
#define VAL_32_HZ        32
#define VAL_64_HZ        64
#define VAL_128_HZ      128
#define VAL_256_HZ      256
#define VAL_512_HZ      512
#define VAL_1024_HZ    1024


/* RTC initialization and interrupt handler routines */
void rtc_init();
void rtc_int_handler();

/* RTC system calls */
int32_t rtc_read(int32_t fd, void * buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const void * buf, int32_t nbytes);
int32_t rtc_open(const uint8_t * filename);
int32_t rtc_close(int32_t fd);

/* RTC helper functions */
void convert_freq(uint32_t *);

/* Wrapper functions */
int rtc_read_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);
int rtc_write_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);
int rtc_open_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);
int rtc_close_wrapper(uint32_t inode,uint32_t offset, uint8_t* buf, uint32_t count);

#endif /* _RTC_H */
