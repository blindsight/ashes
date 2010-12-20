#include <stdio.h> //vsprintf
#include <string.h> //strncpy
#include <stdarg.h> //va_list

#include <telnet.h>
#include <ashes.h>

void process_telnet_command(char *cmds, int len, RES_OBJ res) {
	int cmds_used = 0;
	
	if(WILL_DO(cmds[1])) {
		switch((unsigned char)cmds[2]) {
			case NAWS: //+3 to not pass the IAC, WILL and NAWS
			//the client by the standard should sent the size from now on
				break;
			case TERMINAL_TYPE:
				vwrite_user(res->socket,"%c%c%c%c%c%c",IAC, SB, TERMINAL_TYPE, SEND, IAC, SE);
				break;
			case CHARSET:
				break;
			case STATUS:
				vwrite_user(res->socket,"%c%c%c%c%c%c",IAC, SB, STATUS, SEND, IAC, SE);
				break;
			default:
				vwrite_user(res->socket,"unknown will do '%d'\n", (unsigned char)cmds[2]);
				break;
		}
		cmds_used = 3;	
	} else if(WONT_DO(cmds[1])){
		if((unsigned char)cmds[2] == NAWS) {
			cmds_used = 3;
			res->naws = 0;
			write_user(res->socket, "won't do naws\n");
		} else if((unsigned char)cmds[2] == IAC && (unsigned char)cmds[3] == IAC) {
			cmds_used = 4;
			write_user(res->socket, "wont .. iac iac\n");
		} else {
			vwrite_user(res->socket, "won't do %d\n",(unsigned char)cmds[2]);
			cmds_used = 3;
		}
	} else if((unsigned char)cmds[1] == DO) {
		switch((unsigned char)cmds[2]) {
			case CHARSET:
				vwrite_user(res->socket,"%c%c%c%c;UTF-8;ISO-8859-1;ISO-8859-2;US-ASCII;CP437%c%c",IAC,SB,CHARSET,REQUEST,IAC,SE);
				cmds_used = 3;
			break;
		}
	} else if( (unsigned char)cmds[1]== DONT) {
		switch((unsigned char)cmds[2]) {
			case NAWS:
				res->naws = 0;
				break;
			case CHARSET:
				res->charset = 0;
				break;
			case TERMINAL_TYPE:
				res->term_type = 0;
				break;
		}		
		
		cmds_used = 3;
	//	vwrite_user(res->socket,"don't do %d %d %d",(unsigned char)cmds[2], cmds_used, len);
	} else if((unsigned char)cmds[1] == SB) {
		switch((unsigned char)cmds[2]) {
			case NAWS: //TODO: make sure all byes exist
				res->columns = (unsigned char)cmds[3];
				res->columns = res->columns << 8; //shift to high byte order
				res->columns |= (unsigned char)cmds[4]; //or the lower bytes in
			
				res->rows = (unsigned char)cmds[5];
				res->rows = res->rows << 8; //shift to high byte order
				res->rows |= (unsigned char)cmds[6]; //or the lower bytes in
				
				res->naws = 1;
				
				cmds_used = 7;
				
				if(len > cmds_used && (unsigned char)cmds[cmds_used] == IAC && (unsigned char)cmds[cmds_used+1] == SE) {
					cmds_used = cmds_used+2;
				}
				
				break;
			case TERMINAL_TYPE:
				cmds_used = cmds_used+4;
				if((unsigned char)cmds[3] == IS) {
					for(; cmds_used<len; cmds_used++) {
						if(cmds_used <= len && (unsigned char)cmds[cmds_used] == IAC && (unsigned char)cmds[cmds_used+1] == SE) {
							res->term[cmds_used-4]='\0';
							cmds_used = cmds_used+2;
							break;
						}
						res->term[cmds_used-4] = cmds[cmds_used];
					}
					
					res->term_type = 1;
				
					vwrite_user(res->socket, "terminal set to %s\n", res->term);
				} else {
					write_user(res->socket, "unknown type for term\n");
				}

				break;
			case CHARSET:
				cmds_used = cmds_used+4;
				switch((unsigned char)cmds[3]) {
					case ACCEPTED:
						for(; cmds_used<len; cmds_used++) {
							if((unsigned char)cmds[cmds_used] == IAC && (unsigned char)cmds[cmds_used+1] == SE) {
								res->charcode[cmds_used-4]='\0'; 
								cmds_used = cmds_used+2;
								break;
							}
							res->charcode[cmds_used-4] = cmds[cmds_used];
						}
					
						res->charset = 1;
						vwrite_user(res->socket, "charset changed to %s\n", res->charcode);
						break;
					default:
						write_user(res->socket, "unknown charset option\n");
						break;
				} 
				break;
			default:
				vwrite_talker("unknown will do option\n");
				cmds_used = 3;
				break;
		}
	} else if((unsigned char)cmds[1] == NOP) {
		//TODO: update status? what?
		cmds_used = 2;
	} else { 
		vwrite_talker("unknown telnet command %d\n", (unsigned char)cmds[1]);
		cmds_used = 2;
	}
	
	if(len > cmds_used ) { //more commands to be processed
		process_telnet_command(cmds+cmds_used, len-cmds_used,res);
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

void request_trans_bin(RES_OBJ res) {
	vwrite_user(res->socket,"%c%c%c",IAC,DO,TRANS_BINARY);
}

void request_charset(RES_OBJ res) {
	vwrite_user(res->socket,"%c%c%c",IAC,WILL,CHARSET);
}

void request_option(RES_OBJ res, unsigned char option) {
	vwrite_user(res->socket,"%c%c%c",IAC,DO,option);
}

//tells the client to submitting things character by character
void request_ga(RES_OBJ res) {
	vwrite_user(res->socket, "%c%c%c",IAC, WILL, SUPPRESS_GA);
}

void write_out_telnet_cmd(RES_OBJ res, char *telnet_cmd, int length) {
	
	vwrite_user(res->socket,"\n telnet command (%d)", length);
	
	for(int i=1; i<=length; i++) { //not the quickest but it works
		vwrite_user(res->socket," [%d]",(unsigned char)*telnet_cmd);
		telnet_cmd++;
	}
	
	write_user(res->socket, "\n");
}

void request_status(RES_OBJ res) {
	vwrite_user(res->socket,"%c%c%c",IAC,DO,STATUS);	
}