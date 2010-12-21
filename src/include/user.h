/*	Copyright (C) 2010  Timothy Rhodes <phoenixevolution@gmail.com> 
	Project: Ashes
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