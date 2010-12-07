#include <stdio.h> //perror printf
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h> //FD_ISSET
#include <netinet/in.h>
#include <string.h>	//strlen
#include <errno.h> //int errno
#include <unistd.h> // close
#include <stdarg.h> //va_list
#include <signal.h> //signal
#include <stdlib.h> //exit
#include <arpa/inet.h>

#include <ashes.h>
#include <user.h>

extern TAILQ_HEAD(, resource_obj) head;
int connected_clients = 0;

int main(int argc, char *argv[]) {
	signal(SIGKILL,catch_kill);
	signal(SIGINT,catch_kill);
	int server_socket, activity;
	int true = 1;
	struct sockaddr_in6 addr;
	fd_set socklist; //listening sockets
	
	if((server_socket = socket(PF_INET6, SOCK_STREAM, 0)) == -1) {
		perror("unable to create socket");
		exit(EXIT_FAILURE);
	}
	
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(true))<0) {
		perror(strerror(errno));
	}

	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(SERVER_TELNET_SOCKET);
	inet_pton(AF_INET6, "::1", &addr.sin6_addr);
	
	if(bind(server_socket,(struct sockaddr *)&addr,sizeof(struct sockaddr_in6)) != 0) {
		perror(strerror(errno));
		close(server_socket);
		return 1;
	}
	
	printf("listening for telnet connections on port %d\n",SERVER_TELNET_SOCKET);
	listen(server_socket,3);
	
	printf("waiting to accept connections\n");

	//start resource list
	TAILQ_INIT(&head);
	
	RES_OBJ temp_res;
	
	//before we start looking for new users, lets see if this is a seamless reboot
	FILE *fp = fopen(SREBOOT_FILE,"r");
	if( fp ) {
		fclose(fp);
		//lets readd the seamless reboot users
		read_resources_from_file();
		//the file is no longer needed
		unlink(SREBOOT_FILE);
		
		write_talker("welcome back from the seamless reboot. who says c can't do hot swapable code?\n");
	}
	
	for(;;) {
		FD_ZERO(&socklist);
		
		FD_SET(server_socket, &socklist);
		
		TAILQ_FOREACH(temp_res, &head, entries) {
			FD_SET(temp_res->socket, &socklist);
		}	

		//select needs highest file number +1. Thus 0,1,2 are standard, 3 will be server socket
		//thus connected clients +4 for last number plus 1
		if((activity=select(connected_clients+4, &socklist, NULL, NULL, NULL)) < 0 ) { //select error
			perror(strerror(errno));
		}

		if(FD_ISSET(server_socket, &socklist)) {
			int addrsize=sizeof(struct sockaddr_in6);

			memset (&addr, 0, sizeof (addr));
			//handle new connection
			RES_OBJ res = create_resource();
			if((res->socket = accept(server_socket, (struct sockaddr *)&addr, (socklen_t *)&addrsize))<0) {
				vwrite_talker("unrecoverable error, talker is shutting down.\n");
				perror(strerror(errno));
				close(server_socket);
				close(res->socket);

				return 1;
			}
									
			connected_clients++;
			
			if(connected_clients > MAX_CLIENTS) {
				write_user(res->socket,"\nTalker is filled to capacity... goodbye\n");
				disconnect_user(res);
			} else {
				connect_user(res);
			}
		}
		
		TAILQ_FOREACH(temp_res, &head, entries) {
			if (FD_ISSET(temp_res->socket, &socklist)) {
				int bytes_read = 0;
				
				if ((bytes_read = read(temp_res->socket, temp_res->buff, 4096)) < 0) {
					close(temp_res->socket);
					temp_res->socket = 0;
				}
				
				temp_res->buff[bytes_read] = '\0';
				
				if(temp_res->buff[0] == '.') { //TODO: except more than one word commands					
					if(!strncmp("shutdown",temp_res->buff+1,strlen("shutdown"))) {
						write_talker("talker is shutting down\n");
						return 0;
					} else if (!strncmp("sreboot",temp_res->buff+1,strlen("sreboot"))) {
						write_talker("seamless reboot is about to take place\n");
						write_resources_to_file();
						free(temp_res);
						close(server_socket);
						//first argument is the talker's executable
						execvp(*argv, argv);
					} else if (!strncmp("quit",temp_res->buff+1,strlen("quit"))) {
						vwrite_talker("user %d leaves\n", temp_res->socket);
						disconnect_user(temp_res);
					} else if (!strncmp("test",temp_res->buff+1,strlen("test"))) {
						vwrite_talker("user %d has just conducted a test, thank you\n", temp_res->socket);
					}
				} else { //speech is assumed
					vwrite_talker("user %d says: %s", temp_res->socket, temp_res->buff);
				}
				break;
			}
		}
	}
	
	free(temp_res);
			
	close(server_socket);
	
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
	RES_OBJ temp_res;
		
	TAILQ_FOREACH(temp_res, &head, entries) {
		send(temp_res->socket,message,strlen(message),0);
	}
	
}

void catch_kill(int sig_num) {
	printf("program is being killed %d\n",sig_num);
	exit(0);
}
