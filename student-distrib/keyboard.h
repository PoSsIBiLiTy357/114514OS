#include "lib.h"
#include "i8259.h"
#include "scan_code.h"

/* initialization of keyboard*/
extern void init_keyboard(void);

/* keyboard handler body function */
extern void keyboard_handler(void);

extern char* terminal_read();

extern int terminal_write(char * buf);
