#include <stdio.h> //perror printf
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <errno.h> //int errno
#include <unistd.h>
#include <stdarg.h> //va_list
#include <signal.h> //signal
#include <stdlib.h> //exit
#include <arpa/inet.h>

#include <ashes.h>
#include <user.h>

int main() {
	signal(SIGKILL,catch_kill);
	signal(SIGINT,catch_kill);
	int server_socket;
	int true = 1;
	struct sockaddr_in6 addr;
	int new_socket;
	fd_set socklist; //list of what sockets to listen on
	
	if((server_socket = socket(PF_INET6, SOCK_STREAM, 0)) == -1) {
		perror("unable to create socket");
		exit(EXIT_FAILURE);
	}
	
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(true))<0) {
		perror(strerror(errno));
	}

	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(SERVER_TELNET_SOCKET);
	
	if(bind(server_socket,(struct sockaddr *)&addr,sizeof(struct sockaddr_in6)) != 0) {
		perror(strerror(errno));
		close(server_socket);
		return 1;
	}
	
	printf("listening for telnet connections on port %d\n",SERVER_TELNET_SOCKET);
	listen(server_socket,3);
	
	printf("waiting to accept connection\n");

	int activity;
	
	for(;;) {
		FD_ZERO(&socklist);
		
		FD_SET(server_socket, &socklist);
		int i;
		for(i=0; i<=MAX_CLIENTS; i++) {
			if(client_sock[i] > 0 ) {
				FD_SET(client_sock[i], &socklist);
			}
		}
		

		if((activity=select(connected_clients+4, &socklist, NULL, NULL, NULL)) < 0 ) { //select error
			perror(strerror(errno));
		}

		if(FD_ISSET(server_socket, &socklist)) {
			int addrsize=sizeof(struct sockaddr_in6);

			memset (&addr, 0, sizeof (addr));
			//handle new connection
			if((new_socket = accept(server_socket, (struct sockaddr *)&addr, (socklen_t *)&addrsize))<0) {
				vwrite_talker("unrecoverable error, talker is shutting down.\n");
				perror(strerror(errno));
				close(server_socket);
				close(new_socket);

				return 1;
			}

			connected_clients++;

			if(connected_clients > MAX_CLIENTS) {
				write_user(new_socket,"\nTalker is filled to capacity... goodbye\n");
				disconnect_user(new_socket);
			} else {
				connect_user(new_socket);
			}
		}
		
		for (i=0; i<=MAX_CLIENTS; i++) {
			if (FD_ISSET(client_sock[i], &socklist)) {
				int bytes_read = 0;
				
				if ((bytes_read = read(client_sock[i], user_buff[i], 4096)) < 0) {
					close(client_sock[i]);
					client_sock[i] = 0;
				}
				
				user_buff[i][bytes_read] = '\0';
				
				vwrite_talker("user %d says: %s", i, user_buff[i]);
				
				break;
			}
		}
	}
			
	close(server_socket);
	close(new_socket);
	
	return 0;
}

void vwrite_talker(char *str, ...) {
	va_list arglist;
	va_start(arglist, str); //TODO: should I redo the va function to add our own %x in?
	vsprintf(talker_buff, str, arglist);
	va_end(arglist);
	
	write_talker(talker_buff);
}

void write_talker(char *message) {	
	for (int i=0; i<MAX_CLIENTS; i++) {
		if (client_sock[i] > 0) {
			send(client_sock[i],message,strlen(message),0);
		}
	}
}

void catch_kill(int sig_num) {
	printf("program is being killed %d\n",sig_num);
	exit(0);
}
