#include "scan_code.h"

#define   ESC 		0x01
#define   ONE		0x02
#define   TWO      	0x03
#define   THREE     0x04
#define   FOUR      0x05
#define   FIVE      0x06
#define   SIX      	0x07
#define   SEVEN     0x08
#define   EIGHT     0x09
#define   NINE      0x0A
#define   TEN    	0x0B
#define	  BACKTICK	0x29
#define	  BS 		0x2B
#define   SLASH		0x35
#define   ENTER		0x1C

#define KEYPAD0	82
#define KEYPAD1	79
#define KEYPAD2	80
#define KEYPAD3	81
#define KEYPAD4	75
#define KEYPAD5	76
#define KEYPAD6	77
#define KEYPAD7	71
#define KEYPAD8	72
#define KEYPAD9	73
#define KEYPADENTER	96
#define KEYPADPLUS	78
#define KEYPADMINUS	74

#define KEYPADMUL 0x37
 

#define   Q		0x10
#define   W		0x11
#define   E		0x12
#define   R		0x13
#define   T		0x14
#define   Y		0x15
#define   U		0x16
#define   I		0x17
#define   O		24
#define   P		25
#define   A		30
#define   S		31
#define   D		32
#define   F		33
#define   G		34
#define   H		35
#define   J		36
#define   K		37

#define   Z		44
#define   X		45
#define   C		46
#define   V		47
#define   B		48
#define   N		49
#define   M		50


#define 	TAB			15
#define 	SPACE		57
#define 	CAPSLOCK	58

#define 	DASH	12
#define 	EQUAL	13
#define 	LSB		26
#define 	RSB		27
#define		FENHAO	0x27
#define		SQ		0x28
#define 	COMMA	51
#define 	POINT	52


#define 	PAGEUP			104
#define 	PAGEDOWN        109
#define 	HOME            102
#define 	END             107
#define 	INSERT          110
#define 	DELETE          111



#define NUM_CONVERT 80

/* array to store all scan codes */
char scan_code[NUM_SCAN_CODES];
char shift_convert[NUM_CONVERT];


/*
* init_scan_code()
*   DESCRIPTION: Keyboards send data to the computer via a number or
*		sequence of numbers called scancodes. These scancodes are 
*		a pre-defined standard for keyboards. This function clears an
*		array of size 256 and proceeds to map each scan code to a character.
*
*   INPUTS: none
*   OUTPUTS: none
*   RETURN VALUE: none
*/
void init_scan_code(){
	int i;
	for (i = 0; i < NUM_SCAN_CODES; i++){
		scan_code[i]=0;
	}
	for (i = 0; i < NUM_CONVERT; i++){
		shift_convert[i]=0;
	}
	
	shift_convert[Q]='Q';
	shift_convert[W]='W';
	shift_convert[E]='E';
	shift_convert[R]='R';
	shift_convert[T]='T';
	shift_convert[Y]='Y';
	shift_convert[U]='U';
	shift_convert[I]='I';
	shift_convert[O]='O';
	shift_convert[P]='P';
	shift_convert[A]='A';
	shift_convert[S]='S';
	shift_convert[D]='D';
	shift_convert[F]='F';
	shift_convert[G]='G';
	shift_convert[H]='H';
	shift_convert[J]='J';
	shift_convert[K]='K';
	shift_convert[L]='L';
	shift_convert[Z]='Z';
	shift_convert[X]='X';
    shift_convert[C]='C';
	shift_convert[V]='V';
    shift_convert[B]='B';
    shift_convert[N]='N';
    shift_convert[M]='M';
	
	shift_convert[BACKTICK]= '~';
	shift_convert[ONE]='!';
	shift_convert[TWO]='@';
	shift_convert[THREE]='#';
	shift_convert[FOUR]='$';
	shift_convert[FIVE]='%';
	shift_convert[SIX]='^';
	shift_convert[SEVEN]='&';
	shift_convert[EIGHT]='*';
	shift_convert[NINE]='(';
	shift_convert[TEN]=')';
	
	
	
	scan_code[Q]='q';
	scan_code[W]='w';
	scan_code[E]='e';
	scan_code[R]='r';
	scan_code[T]='t';
	scan_code[Y]='y';
	scan_code[U]='u';
	scan_code[I]='i';
	scan_code[O]='o';
	scan_code[P]='p';
	scan_code[A]='a';
	scan_code[S]='s';
	scan_code[D]='d';
	scan_code[F]='f';
	scan_code[G]='g';
	scan_code[H]='h';
	scan_code[J]='j';
	scan_code[K]='k';
	scan_code[L]='l';
	scan_code[Z]='z';
	scan_code[X]='x';
    scan_code[C]='c';
    scan_code[V]='v';
    scan_code[B]='b';
    scan_code[N]='n';
    scan_code[M]='m';
	
	scan_code[BACKTICK] = '`';
	scan_code[ONE]='1';
	scan_code[TWO]='2';
	scan_code[THREE]='3';
	scan_code[FOUR]='4';
	scan_code[FIVE]='5';
	scan_code[SIX]='6';
	scan_code[SEVEN]='7';
	scan_code[EIGHT]='8';
	scan_code[NINE]='9';
	scan_code[TEN]='0';
	
	scan_code[KEYPAD0] = '0';
	scan_code[KEYPAD1] = '1';
	scan_code[KEYPAD2] = '2';
    scan_code[KEYPAD3] = '3';
    scan_code[KEYPAD4] = '4';
    scan_code[KEYPAD5] = '5';
    scan_code[KEYPAD6] = '6';
    scan_code[KEYPAD7] = '7';
    scan_code[KEYPAD8] = '8';
    scan_code[KEYPAD9] = '9';
	scan_code[KEYPADENTER] = '\n';
	scan_code[KEYPADPLUS] = '+';
	scan_code[KEYPADMINUS] = '-';
	scan_code[KEYPADMUL] = '*';

	
	scan_code[DASH]	= '-';
    scan_code[EQUAL]= '=';	
    scan_code[LSB]	='[';
    scan_code[RSB]	=	']';
	scan_code[BS] = '\\';
    scan_code[FENHAO]=	';';
    scan_code[SQ]	=	'\'';
    scan_code[COMMA]=	',';
    scan_code[POINT]=	'.';
	scan_code[SLASH]= '/';
	scan_code[TAB] = '	';
	scan_code[ENTER]= '\n';
	scan_code[SPACE]= ' ';
	
	shift_convert[DASH]	= '_';
	shift_convert[EQUAL]= '+';	
	shift_convert[LSB]	='{';
	shift_convert[RSB]	=	'}';
	shift_convert[BS] = '|';
	shift_convert[FENHAO]=	':';
	shift_convert[SQ]	=	'"';
	shift_convert[COMMA]=	'<';
	shift_convert[POINT]=	'>';
	shift_convert[SLASH]= '?';
	
}
