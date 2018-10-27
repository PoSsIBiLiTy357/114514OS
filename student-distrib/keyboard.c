#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "scan_code.h"

#define KBRD_STATUS_PORT		0x64
#define KBRD_DATA_PORT			0x60

static caplk_pressd;


/* init_keyboard
 * 
 * initialize scan codes for letters and numbers, then unmask the keyboard irq in PIC
 * Inputs: None
 * Outputs: None
 * Side Effects: Create a scan code array
 * 
 */
void init_keyboard(void)
{	
	
	init_scan_code();
	caplk_pressd=0;
	enable_irq(1);  //unmask IRQ1 of PIC
	
}


/* keyboard_handler
 * 
 * take the pressed key code from data port of keyboard,fetch that letter from scan code array,print letter to screen, also tell processor the end of interrupt 
 * Inputs: None
 * Outputs: None
 * Side Effects: print to screen, send eoi to processor
 * 
 */
void keyboard_handler(void){
	cli();
	send_eoi(1);   // protected send of eoi
	unsigned char pressed;
	while ((inb(KBRD_STATUS_PORT)&0x01)!=0){  // only read from data port when the status is ready
		pressed=inb(KBRD_DATA_PORT);   //get key code
		if(pressed==CAPSLOCK){
			caplk_pressd= 1-caplk_pressd; 
		}
		if(caplk_pressd==0){
				if (scan_code[(int)pressed]!=0){
					putc(scan_code[(int)pressed]);  //get letter
				}
		}
		else{
			if (scan_code[(int)pressed]!=0){
				putc(shift_convert[(int)pressed]);  //get letter
			}
		}
	}

}

int terminal_read(){
	return 0;
}

int terminal_open(){
	return 0;
}

