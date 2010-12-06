#define SERVER_TELNET_SOCKET	2000
#define MAX_CLIENTS	3

void catch_kill(int sig_num);

int client_sock[MAX_CLIENTS];
char user_buff[MAX_CLIENTS][1024]; //TODO: move this back to a struct
char talker_buff[1024]; //TODO: make this work with threads or fork

void write_talker(char *message);
void vwrite_talker(char *str, ...);