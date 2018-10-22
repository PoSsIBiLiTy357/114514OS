#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "scan_code.h"



void init_keyboard(void)
{	
	cli();
	init_scan_code();
	enable_irq(1);
	sti();
}

void keyboard_handler(void){
	cli();
	send_eoi(1);
	sti();
	unsigned char pressed;
	while ((inb(0x64)&0x01)!=0){
		pressed=inb(0x60);
		printf("%c",scan_code[(int)pressed]);
	}

}
