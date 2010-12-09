#include <stdio.h> //vsprintf
#include <string.h> //strncpy
#include <stdarg.h> //va_list

#include <telnet.h>
#include <ashes.h>

void process_telnet_command(char *cmds, int len, RES_OBJ res) {
	char left_cmds[1024]; //TODO: longest string of telnet commands? what should it be?
	
	if(WILL_DO(cmds[1])) {
		switch((unsigned char)cmds[2]) {
			case NAWS: //+3 to not pass the IAC, WILL and NAWS
				if(len > 3) { //more commands to be processed
					strncpy(left_cmds, cmds+3,len-4);
					process_telnet_command(left_cmds, len-4,res);
				}
				return;
			default:
				write_talker("unknown will do\n");
				return;
		}	
	} else if(WONT_DO(cmds[1])){
		if(cmds[1] == NAWS) {
			BIT_SET(res->wont,NAWS);
		}
	} else if((unsigned char)cmds[1] == SB) {
		switch((unsigned char)cmds[2]) {
			case NAWS: //TODO: make sure all byes exist
				res->columns = (unsigned char)cmds[3];
				res->columns = res->columns << 8; //shift to high byte order
				res->columns |= (unsigned char)cmds[4]; //or the lower bytes in
			
				res->rows = (unsigned char)cmds[5];
				res->rows = res->rows << 8; //shift to high byte order
				res->rows |= (unsigned char)cmds[6]; //or the lower bytes in

				return;
			default:
				write_talker("unknown will do\n");
				return;
		}
	} else { 
			write_talker("unknown telnet command\n");
	}
}

void vwrite_user_x_y(RES_OBJ res, uint16_t rows, uint16_t cols, char *str, ...) {
	char test_buff[strlen(str)];
	va_list arglist;
	va_start(arglist, str); //TODO: should I redo the va function to add our own %x in?
	vsprintf(test_buff, str, arglist);
	va_end(arglist);
	//TODO: is the last line of putting the character back, really the best?
	vwrite_user(res->socket, "\033[%d;%dH %s", rows, cols, test_buff);
}

void clear_screen(RES_OBJ res) {
	vwrite_user(res->socket,"\033[2J\n");
}

void request_naws(RES_OBJ res) {
	vwrite_user(res->socket,"%c%c%c",IAC,DO,NAWS);
}