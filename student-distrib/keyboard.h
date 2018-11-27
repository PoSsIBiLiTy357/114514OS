#include "lib.h"
#include "i8259.h"
#include "scan_code.h"

/* initialization of keyboard*/
extern void init_keyboard(void);

/* keyboard handler body function */
extern void keyboard_handler(void);

extern int terminal_read(char* buf, int count);

extern int terminal_write(char * buf,int count);
int terminal_read_wrap(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count);
int terminal_write_wrap(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count);
