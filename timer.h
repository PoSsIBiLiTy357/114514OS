/* timer.h - Header file for RTC device. Contains basic function declarations.
 */

#ifndef _TIMER_H
#define _TIMER_H

/* PIT initialization and interrupt handler routines */
void pit_init();
void pit_int_handler();


#endif  /*_TIMER_H */