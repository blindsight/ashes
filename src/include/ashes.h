#ifndef ASHES_H
#define ASHES_H

#define SERVER_TELNET_SOCKET	2000
#define MAX_CLIENTS		3
#define SREBOOT_FILE		"./sreboot_file"

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_TEST(a,b) ((a) & (1<<(b)))

void catch_kill(int sig_num);

char talker_buff[4096]; //TODO: make this work with threads or fork

void write_talker(char *message);
void vwrite_talker(char *str, ...);
void pipe_handler(int sig_num);

#endif