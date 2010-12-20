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
void talker_sreboot(int argc, char *argv[]);

void strip_newline_at_end(char *line_strip, int len);

#endif