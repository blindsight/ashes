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

#ifndef USER_H
#define USER_H

#include <stdint.h>

#include <queue.h>


#define LAST_WORDS_MAX	10

typedef struct user_obj {
	char	*name, *password;
} *UR_OBJ;

typedef struct resource_obj {
	int				socket;
	int				naws, charset, term_type, telnet_view;
	char				term[50]; //whats the size?
	char				charcode[20];
	char				buff[4096]; //user input buffer
	TAILQ_ENTRY(resource_obj)	entries;
	uint16_t			rows, columns;
	int				word_count;
	UR_OBJ				user;
	char				*last_words[]; //array max of LAST_WORDS_MAX
} *RES_OBJ;

/*prototypes */
void write_user(int socket, char *message);
void vwrite_user(int socket, char *str, ...);

RES_OBJ create_resource();
UR_OBJ create_user();

void connect_user(RES_OBJ);
void disconnect_user(RES_OBJ);

void write_resources_to_file();
void read_resources_from_file();

void examine(RES_OBJ res);
void resource_quits(RES_OBJ res);

void telnet_view(RES_OBJ res);

void create_last_words(RES_OBJ res);
RES_OBJ get_res(int socket_test);
void turn_off_ga(RES_OBJ);

void set_name(RES_OBJ res);
#endif