#https://wiki.osdev.org/Interrupt_Service_Routines



.globl keyboard_handler_asm
.globl  rtc_handler_asm 
.align   4
 
keyboard_handler_asm:
    pusha
    cld 
    call keyboard_handler
    popa
    iret
	
rtc_handler_asm:
    pusha
    cld 
    call rtc_int_handler
    popa
    iret
    