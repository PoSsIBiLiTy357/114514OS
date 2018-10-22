#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "scan_code.h"

#define KBRD_STATUS_PORT		0x64
#define KBRD_DATA_PORT			0x60

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
	while ((inb(KBRD_STATUS_PORT)&0x01)!=0){
		pressed=inb(KBRD_DATA_PORT);
		printf("%c",scan_code[(int)pressed]);
	}

}
