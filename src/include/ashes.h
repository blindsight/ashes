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

#ifndef ASHES_H
#define ASHES_H

#include <user.h>

#define SERVER_TELNET_SOCKET	"2000"
#define MAX_CLIENTS		10
#define SREBOOT_FILE		"./sreboot_file"

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_TEST(a,b) ((a) & (1<<(b)))

#ifndef __unix__ //on linux this is already defined
	#ifdef __APPLE__
		#define __unix__
	#endif
#endif

char talker_buff[4096]; //TODO: make this work with threads or fork
RES_OBJ temp_res;
int server_socket; 

void write_talker(char *message);
void vwrite_talker(char *str, ...);
void talker_shutdown();

#ifdef __unix__
void talker_sreboot(int argc, char *argv[]);
#endif

#endif