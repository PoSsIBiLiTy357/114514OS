/* rtc.h - Header file for RTC device. Contains basic function declarations.
 *  Author: Andrew Chen
 *  Date: 10/20/18 
 */

#ifndef _RTC_H
#define _RTC_H

#include "lib.h"
#include "i8259.h"
#include "tests.h"

#define STATUS_REG_A    0x0A
#define STATUS_REG_B    0x0B
#define STATUS_REG_C    0x0C

#define IRQ_RTC            8
#define CMOS_ADDR       0x70
#define CMOS_DATA       0x71
#define DISABLE_NMI     0x80

#define RATE_MASK       0xF0
#define DIV_FREQ_2      0x0F

void rtc_init();
void rtc_int_handler();

int32_t rtc_read(int32_t fd, void * buf, int32_t bytes);
int32_t rtc_write(int32_t fd, const void * buf, int32_t nbytes);
int32_t rtc_open(const uint8_t * filename);
int32_t rtc_close(int32_t fd);

#endif /* _RTC_H */
