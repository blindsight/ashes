/*	Copyright (C) 2010  Timothy Rhodes <phoenixevolution@gmail.com> 
	
This file is part of Ashes.

Ashes is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ashes is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ashes.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TELNET_H
#define TELNET_H

#include <user.h>

#define TRANS_BINARY	0
#define ECHO		1	//rfc857
#define STATUS		5
#define TERMINAL_TYPE	24
#define NAWS		31
#define LINEMODE	34	//rfc1116
#define CHARSET		42
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

#define IS	0
#define SEND	1
#define REPLY	2
#define NAME	3

//RFC2066
#define REQUEST		1
#define ACCEPTED	2
#define REJECTED	3
#define TTABLE_SEND	4
#define TTABLE_IS	5
#define TTABLE_REJECTED	6
#define TTABLE_ACK	7
#define TTABLE_NAK	8
         

//RFC858
#define SUPPRESS_GA	3

#define IS_TELNET_CMD(unsigned_char) (unsigned char)unsigned_char[0] == IAC

#define WILL_DO(option) (unsigned char)option == WILL 
#define WONT_DO(option) (unsigned char)option == WONT

void process_telnet_command(char *cmds, int len, RES_OBJ res);
void vwrite_user_x_y(RES_OBJ res, uint16_t rows, uint16_t cols, char *str, ...);
void write_out_telnet_cmd(RES_OBJ res, char *telnet_cmd, int length);

void clear_screen(RES_OBJ res);
void request_naws(RES_OBJ res);

void request_trans_bin(RES_OBJ res);
void request_charset(RES_OBJ res);
void request_option(RES_OBJ res, unsigned char option);
void request_status(RES_OBJ res);
#endif