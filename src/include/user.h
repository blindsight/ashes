#ifndef USER_H
#define USER_H

#include <sys/queue.h>

typedef struct resource_obj {
	int				socket;
	char				name[24];
	char				buff[4096]; //user input buffer
	unsigned char			rows, columns;
	TAILQ_ENTRY(resource_obj)	entries;
} *RES_OBJ;

/*prototypes */
void write_user(int socket, char *message);
void vwrite_user(int socket, char *str, ...);

RES_OBJ create_resource();

void connect_user(RES_OBJ);
void disconnect_user(RES_OBJ);

void write_resources_to_file();
void read_resources_from_file();

#endif