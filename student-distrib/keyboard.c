#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "scan_code.h"

#define KBRD_STATUS_PORT		0x64
#define KBRD_DATA_PORT			0x60
#define BUFFER_SIZE		128
#define LINE_SIZE   80

static int caplk_pressd;
static int shift_state;
static int ctrl_state;
static int cursor_idx;
static int overline;
static char first[LINE_SIZE];
//char second[28];
static char keyboard_buffer[BUFFER_SIZE]; ///leave 1 for _


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
	int i;
	while ((inb(KBRD_STATUS_PORT)&0x01)!=0){  // only read from data port when the status is ready
		pressed=inb(KBRD_DATA_PORT);   //get key code
		if(strlen(keyboard_buffer)>0 && pressed ==BACKSPACE){
			keyboard_buffer[cursor_idx] ='\0';
			if(cursor_idx>=1){
				cursor_idx--;
				keyboard_buffer[cursor_idx] ='_';
			}
			else keyboard_buffer[cursor_idx] ='_';
			if (strlen(keyboard_buffer)==LINE_SIZE) {
				screen_y_change(-1);
				overline =0;
			}

		}
		else{
			if(pressed==CAPSLOCK){				// record caps lock status
				caplk_pressd= 1-caplk_pressd; 
			}
			if(pressed==LEFTSHIFT || pressed==RIGHTSHIFT){		/// record shift  status
				shift_state=1;
			}
			if(pressed==LEFTSHIFT_R || pressed==RIGHTSHIFT_R){ 		///change status if released
				shift_state=0;
			}
			if(pressed==LEFTCTRL){			/// record control state
				ctrl_state=1;
			}
			if(pressed==LEFTCTRL_R){		//change state if released
				ctrl_state=0;
			}
			if(scan_code[(int)pressed]=='l' && ctrl_state==1){					///clear screen if ctrl+l
				clear();
				screen_y_set(0);
				for(i=0;i<BUFFER_SIZE;i++){					//empty the buffer
					keyboard_buffer[i]='\0';
				}
				cursor_idx=0;
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
			if(ctrl_state==0){				//check ctrl state 
				if(caplk_pressd==0){		// check cap state
					if(shift_state==0){
						if (scan_code[(int)pressed]!=0){
							keyboard_buffer[cursor_idx]=scan_code[(int)pressed];			// cap not pressed and shift not pressed case 
							if (cursor_idx < 127) {
								cursor_idx++;
								keyboard_buffer[cursor_idx] = '_';
							}
						}	
					}	
					else{
						if (scan_code[(int)pressed]!=0){									// shift pressed case, use shift convert table 
							keyboard_buffer[cursor_idx]=shift_convert[(int)pressed];
							if (cursor_idx < 127) {
								cursor_idx++;
								keyboard_buffer[cursor_idx] = '_';
							}
						}
					}
				}
				else{
					if (scan_code[(int)pressed]!=0 && scan_code[(int)pressed]>96 &&  scan_code[(int)pressed]<122){  // cap case , only convert letters not numbers and symbols
						if(shift_state==0){
							keyboard_buffer[cursor_idx]=shift_convert[(int)pressed];
							if (cursor_idx < 127) {
								cursor_idx++;
								keyboard_buffer[cursor_idx] = '_';
							}
						}
						else{																//cap case with shift hold down, back to lower case letter 
							keyboard_buffer[cursor_idx]=scan_code[(int)pressed];
							if (cursor_idx < 127) {
								cursor_idx++;
								keyboard_buffer[cursor_idx] = '_';
							}
						}
				
					}
					else if(scan_code[(int)pressed]!=0) {		//cap case for non letters 
						if(shift_state==0){
							keyboard_buffer[cursor_idx]=scan_code[(int)pressed];
							if (cursor_idx < 127) {
								cursor_idx++;
								keyboard_buffer[cursor_idx] = '_';
							}
						}
						else{
							keyboard_buffer[cursor_idx]=shift_convert[(int)pressed];
							if (cursor_idx < 127) {
								cursor_idx++;
								keyboard_buffer[cursor_idx] = '_';
							}
							}
						}	
					}
			}
		}
		
		for (i=0;i<LINE_SIZE;i++){
			first[i]= keyboard_buffer[i];
		}
///////////////////////////can be put into terminal read
		if (strlen(keyboard_buffer)<=LINE_SIZE){						//print to screen 1 line case 
			put_refresh_line(keyboard_buffer);
		}else{
/*			int i;
			for (i=0;i<LINE_SIZE;i++){
				first[i] = keyboard_buffer[i];
			}
			for (i=0;i<27;i++){
				second[i] = keyboard_buffer[i+LINE_SIZE]; 
			}*/
			if (overline ==0){						//   print to screen 2 line case ; first line 
				put_refresh_line(first);
				puts("\n");
			}
			put_refresh_line(keyboard_buffer+LINE_SIZE);			//  print to screen 2 line case ; second line 
			overline =1;
		}
		if(scan_code[(int)pressed] == '\n'){					// enter pressed go to next line clear keyboard_buffer
			overline =0;
			int i;
			for(i=0;i<cursor_idx+1;i++){
				keyboard_buffer[i]='\0';
			}
			cursor_idx=0;
			keyboard_buffer[cursor_idx]='_';
		}

		//puts(keyboard_buffer);
	}
	sti();
}

/* terminal_read
 * 
 * return the current keyboard_buffer to whatever caller calls this
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * 
 */
char* terminal_read(){
	return keyboard_buffer;
}


/* terminal_write
 * 
 * print input buffer to the screen while clear old stuff on this line 
 * Inputs: None
 * Outputs: None
 * Side Effects: print to screen, send eoi to processor
 * 
 */
int terminal_write(char * buf){
	put_refresh_line(buf);
}



