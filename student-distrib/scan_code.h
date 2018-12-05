#ifndef _SCAN_CODE_H
#define _SCAN_CODE_H

#define 	LEFTSHIFT	42
#define 	RIGHTSHIFT	54
#define		LEFTSHIFT_R 0xAA
#define		RIGHTSHIFT_R 0xB6
#define     BACKSPACE 0x0E
#define NUM_SCAN_CODES		255
#define 	CAPSLOCK	58
#define  NUM_CONVERT 80
#define		LEFTCTRL	0x1D	
#define		LEFTCTRL_R	0x9D
#define     LEFTALT  56
#define     LEFTALT_R 0xB8

#define 	F1             59
#define 	F2             60
#define 	F3             61
#define 	F4             62
#define 	F5             63
#define 	F6             64
#define 	F7             65
#define 	F8             66
#define 	F9             67
#define 	F10            68
#define 	F11            87
#define 	F12            88

/* declare array to store all scan codes */
char scan_code[NUM_SCAN_CODES];
char shift_convert[NUM_CONVERT];
/* scan code initialization function */
void init_scan_code();

#endif  /* _SCAN_CODE_H */
