#ifndef USER_H
#define USER_H

#include <stdint.h>
#include <sys/queue.h>

typedef struct resource_obj {
	int				socket;
	int				naws, charset, term_type, telnet_view;
	char				name[24];
	char				term[50]; //whats the size?
	char				charcode[20];
	char				buff[4096]; //user input buffer
	TAILQ_ENTRY(resource_obj)	entries;
	uint16_t			rows, columns;
} *RES_OBJ;

/*prototypes */
void write_user(int socket, char *message);
void vwrite_user(int socket, char *str, ...);

RES_OBJ create_resource();

void connect_user(RES_OBJ);
void disconnect_user(RES_OBJ);

void write_resources_to_file();
void read_resources_from_file();

void examine(RES_OBJ res, RES_OBJ to_res);
#endif