#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "scan_code.h"



void init_keyboard()
{
	init_scan_code();
	enable_irq(1);
}

void keyboard_handler(){
	cli();
	unsigned char pressed;
	if ((inb(0x64)&0x01)!=0){
		pressed=inb(0x60);
		printf("pressed key is :%c\n",scan_code[(int)pressed]);
	}
	send_eoi(1);
	sti();
}
