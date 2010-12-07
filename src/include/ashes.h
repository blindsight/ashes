#ifndef ASHES_H
#define ASHES_H

#define SERVER_TELNET_SOCKET	2000
#define MAX_CLIENTS		3
	
static int connected_clients = 0;

void catch_kill(int sig_num);

char talker_buff[4096]; //TODO: make this work with threads or fork

void write_talker(char *message);
void vwrite_talker(char *str, ...);

#endif