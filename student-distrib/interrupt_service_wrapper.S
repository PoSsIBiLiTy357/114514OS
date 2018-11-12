#https://wiki.osdev.org/Interrupt_Service_Routines
#define ASM 1
#include "x86_desc.h"

#define SYSCALL_NUM 7

.globl keyboard_handler_asm
.globl  rtc_handler_asm 
.globl syscall_asm
.globl from_syscall
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

https://www.win.tue.nl/~aeb/linux/lk/lk-4.html
syscall_asm:
    pushl %eax   #save orginal eax

    #save all
    cld
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    movl $(KERNEL_DS),%edx
    movl %edx,%ds
    movl %edx,%es

    #check syscall number, if above the range, 
    cmpl $(SYSCALL_NUM),%eax
    jae invalid
    call *sys_call_table(,%eax,4)
    movl %eax,0x18(%esp)    #save return value

    invalid:
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    addl $4,%esp
    iret



sys_call_table:
.long 0x0, halt, execute, read, write,open, close, getargs
