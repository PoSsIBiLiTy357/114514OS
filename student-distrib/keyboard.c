#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "scan_code.h"

#define KBRD_STATUS_PORT		0x64
#define KBRD_DATA_PORT			0x60

static int caplk_pressd;
static int shift_state;
static int ctrl_state;
static int cursor_idx;
static int overline;
char first[80];
//char second[28];
char keyboard_buffer[129]; ///leave 1 for _


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
	shift_state=0;
	cursor_idx=0;
	overline =0;
	memset(first,'\0',sizeof(first));
	//memset(second,'\0',sizeof(second));
	memset(keyboard_buffer,'\0',sizeof(keyboard_buffer));

	keyboard_buffer[cursor_idx]='_';
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
		if(strlen(keyboard_buffer)>0 && pressed ==BACKSPACE){
			keyboard_buffer[cursor_idx] ='\0';
			if(cursor_idx>=1){
				cursor_idx--;
				keyboard_buffer[cursor_idx] ='_';
			}
			else keyboard_buffer[cursor_idx] ='_';
			if (strlen(keyboard_buffer)==80) screen_y_change(-1);
		}
		else{
			if(pressed==CAPSLOCK){
				caplk_pressd= 1-caplk_pressd; 
			}
			if(pressed==LEFTSHIFT || pressed==RIGHTSHIFT){
				shift_state=1;
			}
			if(pressed==LEFTSHIFT_R || pressed==RIGHTSHIFT_R){
				shift_state=0;
			}
				//	if(scan_code[(int)pressed] == '\n'){
				//		int i;
				//		for(i=0;i<cursor_idx;i++){
				//			keyboard_buffer[i]=0;
				//		}
				//		cursor_idx=0;
				//		keyboard_buffer[cursor_idx]=scan_code[(int)pressed];
				//		cursor_idx++;
				//		keyboard_buffer[cursor_idx]='_';
				//	}
			
				if(caplk_pressd==0){
					if(shift_state==0){
						if (scan_code[(int)pressed]!=0){
							keyboard_buffer[cursor_idx]=scan_code[(int)pressed];
							cursor_idx++;
							keyboard_buffer[cursor_idx]='_';
						}	
					}	
					else{
						if (scan_code[(int)pressed]!=0){
							keyboard_buffer[cursor_idx]=shift_convert[(int)pressed];
							cursor_idx++;
							keyboard_buffer[cursor_idx]='_';
						}
					}
				}
				else{
					if (scan_code[(int)pressed]!=0 && scan_code[(int)pressed]>96 &&  scan_code[(int)pressed]<122){
						if(shift_state==0){
							keyboard_buffer[cursor_idx]=shift_convert[(int)pressed];
							cursor_idx++;
							keyboard_buffer[cursor_idx]='_';
						}
						else{
							keyboard_buffer[cursor_idx]=scan_code[(int)pressed];
							cursor_idx++;
							keyboard_buffer[cursor_idx]='_';
						}
				
					}
					else if(scan_code[(int)pressed]!=0) {
						if(shift_state==0){
							keyboard_buffer[cursor_idx]=scan_code[(int)pressed];
							cursor_idx++;
							keyboard_buffer[cursor_idx]='_';
						}
						else{
							keyboard_buffer[cursor_idx]=shift_convert[(int)pressed];
							cursor_idx++;
							keyboard_buffer[cursor_idx]='_';
							}
						}	
					}
			}
		int i;
		for (i=0;i<80;i++){
			first[i]= keyboard_buffer[i];
		}
///////////////////////////can be put into terminal read
		if (strlen(keyboard_buffer)<=80){
			put_refresh_line(keyboard_buffer);
		}else{
/*			int i;
			for (i=0;i<80;i++){
				first[i] = keyboard_buffer[i];
			}
			for (i=0;i<27;i++){
				second[i] = keyboard_buffer[i+80]; 
			}*/
			if (overline ==0){
				put_refresh_line(first);
				puts("\n");
			}
			put_refresh_line(keyboard_buffer+80);
			overline =1;
		}
		if(scan_code[(int)pressed] == '\n'){
			overline =0;
			int i;
			for(i=0;i<cursor_idx;i++){
				keyboard_buffer[i]='\0';
			}
			cursor_idx=0;
			keyboard_buffer[cursor_idx]='_';
		}

		//puts(keyboard_buffer);
	}

}

int terminal_read(){
	return 0;
}

int terminal_write(){
	return 0;
}

int terminal_open(){
	return 0;
}

