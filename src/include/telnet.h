#ifndef TELNET_H
#define TELNET_H

#include <user.h>

#define ECHO		1
#define NAWS		31
#define SE		240    //End of subnegotiation parameters.
#define NOP		241    //No operation.
#define DATA_MARK	242
#define BREAK		243
#define INTERRUPT_PRO	244
#define ABORT_OUTPUT	245
#define ARE_YOU_THERE	246 
#define ERASE_CHAR	247
#define ERASE_LINE	248
#define GO_AHEAD	249
#define SB		250
#define WILL		251
#define WONT		252
#define DO		253   
#define DONT		254 
#define IAC		255	//Interrupt as a Command

#define IS_TELNET_CMD(unsigned_char) (unsigned char)unsigned_char[0] == IAC

#define WILL_DO(option) (unsigned char)option == WILL 
#define WONT_DO(option) (unsigned char)option == WONT

void process_telnet_command(char *cmds, int len, RES_OBJ res);
void vwrite_user_x_y(RES_OBJ res, uint16_t rows, uint16_t cols, char *str, ...);

void clear_screen(RES_OBJ res);
void request_naws(RES_OBJ res);

#endif