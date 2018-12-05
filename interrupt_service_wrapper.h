#ifndef _INTERRUPT_SERVICE_WRAPPER_H
#define _INTERRUPT_SERVICE_WRAPPER_H
#ifndef ASM
#include "x86_desc.h"

extern void keyboard_handler_asm();
extern void rtc_handler_asm();
extern void pit_handler_asm();
extern void syscall_asm();

#endif
#endif  /* _INTERRUPT_SERVICE_WRAPPER_H */
