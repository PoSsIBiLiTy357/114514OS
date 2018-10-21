#https://wiki.osdev.org/Interrupt_Service_Routines
#define ASM     1
#include "interrupt_service_wrapper.h"
.globl keyboard_handler_asm
.globl  rtc_handler_asm 
.align   4
 
keyboard_handler_asm:
    pushad
    cld 
    call keyboard_handler
    popad
    iret
	
rtc_handler_asm:
    pushad
    cld 
    call rtc_handler
    popad
    iret