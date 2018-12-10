#include "lib.h"
#include "i8259.h"
#include "tests.h"
#include "scan_code.h"
/*keyboard initialization*/
extern void init_keyboard();
/*handle keyboard interrupt*/
extern void keyboard_interrupt_handler();

int32_t terminal_read(char* buf, int count);
int32_t terminal_read_wrap(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count);
unsigned char char_match(unsigned char c);
int32_t terminal_write(char* buf, int count);
int32_t terminal_write_wrap(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count);
int32_t terminal_close();
extern int terminal_wrong(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count);
extern int terminal_nothing(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t count);
