#https://wiki.osdev.org/Interrupt_Service_Routines



.globl keyboard_handler_asm
.globl  rtc_handler_asm 
.align   4
 
keyboard_handler_asm:
    pusha
    pushfl
    call keyboard_handler
    popfl
    popa
    iret
	
rtc_handler_asm:
    pusha
    pushfl 
    call rtc_int_handler
    popfl
    popa
    iret
    