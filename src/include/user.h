#ifndef USER_H
#define USER_H

#include <stdint.h>
#include <sys/queue.h>

#define LAST_WORDS_MAX	10

typedef struct resource_obj {
	int				socket;
	int				naws, charset, term_type, telnet_view;
	char				name[24];
	char				term[50]; //whats the size?
	char				charcode[20];
	char				buff[4096]; //user input buffer
	TAILQ_ENTRY(resource_obj)	entries;
	uint16_t			rows, columns;
	int				word_count;
	char				*last_words[]; //array max of LAST_WORDS_MAX
} *RES_OBJ;

/*prototypes */
void write_user(int socket, char *message);
void vwrite_user(int socket, char *str, ...);

RES_OBJ create_resource();

void connect_user(RES_OBJ);
void disconnect_user(RES_OBJ);

void write_resources_to_file();
void read_resources_from_file();

void examine(RES_OBJ res);
void resource_quits(RES_OBJ res);

void telnet_view(RES_OBJ res);

void create_last_words(RES_OBJ res);
RES_OBJ get_res(int socket_test);
#endif