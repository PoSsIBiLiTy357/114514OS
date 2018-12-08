/* timer.h - Header file for RTC device. Contains basic function declarations.
 */

#ifndef _TIMER_H
#define _TIMER_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "syscall.h"

/* PIT initialization and interrupt handler routines */
void pit_init();
void pit_int_handler();


#endif  /*_TIMER_H */
