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
#define   L		38
#define   Z		44
#define   X		45
#define   C		46
#define   V		47
#define   B		48
#define   N		49
#define   M		50


/* array to store all scan codes */
char scan_code[NUM_SCAN_CODES];


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
	
}
