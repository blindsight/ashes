/*	Copyright (C) 2010  Timothy Rhodes <phoenixevolution@gmail.com> 
	Project: Ashes
*/
#ifdef WIN32
	#define _CRT_SECURE_NO_WARNINGS //microsoft has put was string functions as insecure.. I don't care atm :)
#endif

#include <stdio.h> //perror printf
#include <sys/types.h>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h> //getaddrinfo
#else
	#include <sys/socket.h>
	#include <unistd.h> // close
	#include <netdb.h> //getaddrinfo
	#include <sys/select.h> //FD_ISSET
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

#include <string.h>	//strlen
#include <errno.h> //int errno
#include <stdarg.h> //va_list
#include <stdlib.h> //exit

#include <ashes.h>
#include <user.h>
#include <telnet.h>
#include <commands.h>

#ifdef __unix__
	#include <unix.h>
#else
	#include <win.h>
#endif

extern TAILQ_HEAD(, resource_obj) head;
extern TAILQ_HEAD(, command_obj) cmd_list;

int connected_clients = 0;

int main(int argc, char *argv[]) {
	int activity;
	int true = 1;
	struct addrinfo addr, *result;
	fd_set socklist; //listening sockets
	int addr_error = 0;
	
	setup_talker();
	//generate linked list of commands
	create_commands();
	
	memset (&addr, 0, sizeof (addr));
	
	addr.ai_family = AF_INET6;
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_protocol = IPPROTO_TCP;
	addr.ai_flags = AI_PASSIVE;

	if ((addr_error = getaddrinfo(NULL, "2000", &addr, &result)) != 0 ) {
	    printf("getaddrinfo failed with error: %d %s\n", addr_error, strerror(errno));
	    exit(EXIT_FAILURE);
	}

	if((server_socket = socket(result->ai_family, result->ai_socktype, 0)) == -1) {
		perror("unable to create socket");
		exit(EXIT_FAILURE);
	}
	
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(true))<0) {
		perror(strerror(errno));
	}
	
	if(bind(server_socket,result->ai_addr, (int)result->ai_addrlen) != 0) {
		perror(strerror(errno));
		close(server_socket);
		return 1;
	}
	
	printf("listening for telnet connections on port %s\n",SERVER_TELNET_SOCKET);
	listen(server_socket,3);
	
	printf("waiting to accept connections\n");

	//start resource list
	TAILQ_INIT(&head);
	
#ifndef WIN32 //windows doesn't support the function that does this operation
	//before we start looking for new users, lets see if this is a seamless reboot
	if(file_exists(SREBOOT_FILE)) {
		//lets readd the seamless reboot users
		read_resources_from_file();
		//the file is no longer needed
		unlink(SREBOOT_FILE);
		
		TAILQ_FOREACH(temp_res, &head, entries) {
			request_status(temp_res); //lets know what we have already agreed to
			request_naws(temp_res);
			request_charset(temp_res);
		 	request_option(temp_res, TERMINAL_TYPE);
		}
		
		write_talker("welcome back from the seamless reboot. who says c can't do hot swapable code?\n");
	}
#endif
	for(;;) {
		RES_OBJ prev_res;

		FD_ZERO(&socklist);
		
		FD_SET(server_socket, &socklist);
		
		TAILQ_FOREACH_SAFE(temp_res, &head, entries,prev_res) {
			FD_SET(temp_res->socket, &socklist);
		}	

		//select needs highest file number +1. Thus 0,1,2 are standard, 3 will be server socket
		//thus connected clients +4 for last number plus 1
		if((activity=select(200, &socklist, NULL, NULL, NULL)) < 0 ) { //select error
			perror(strerror(errno));
		}

		if(FD_ISSET(server_socket, &socklist)) {
			int addrsize=sizeof(struct sockaddr_in6);
			RES_OBJ res = create_resource();

			memset (&addr, 0, sizeof (addr));
			
			//handle new connection
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
				clear_screen(res);
			}
		}
		
		TAILQ_FOREACH_SAFE(temp_res, &head, entries,prev_res) {
			if (FD_ISSET(temp_res->socket, &socklist)) {
				int bytes_read = 0;

				if ((bytes_read = recv(temp_res->socket, temp_res->buff, 4096, 0)) < 0) {
					vwrite_talker("\nuser %d has been disconnected\n", temp_res->socket);
					disconnect_user(temp_res);
					continue;
				}
				
				if(bytes_read-1>=0 && temp_res->buff[bytes_read-1] == '\r') {
					temp_res->buff[bytes_read-1]='\0';	
				} else if(bytes_read-2>=0 && temp_res->buff[bytes_read-2] == '\r'
					&& temp_res->buff[bytes_read-1] == '\n') {
					temp_res->buff[bytes_read-2] = '\0';	
				} else {
					temp_res->buff[bytes_read] = '\0';
				}
				
				//lets make sure there was more there than just a newline
				if(strlen(temp_res->buff)<1) {
					continue; //no real input
				}
				
				create_last_words(temp_res);
				
				if(IS_TELNET_CMD(temp_res->buff)) {
					if(temp_res->telnet_view) {
						write_out_telnet_cmd(temp_res, temp_res->buff, bytes_read);
					}
					
					process_telnet_command(temp_res->buff, bytes_read, temp_res);
				} else if(temp_res->buff[0] == '.') {
					CMD_OBJ temp_cmd;
					int found_cmd = 0; // is this the best way?
					//remove dot
					strncpy(temp_res->last_words[0],(temp_res->last_words[0]+1),strlen(temp_res->last_words[0])-1);
					temp_res->last_words[0][strlen(temp_res->last_words[0])-1]='\0';
					
					TAILQ_FOREACH(temp_cmd, &cmd_list, entries) {
						if(!strncmp(temp_res->last_words[0],temp_cmd->name,strlen(temp_res->last_words[0])-1)) {
#ifndef WIN32
							if(!strncmp(temp_cmd->name,"sreboot", strlen("sreboot"))) {
								//still has to be special since the arugments are so different
								talker_sreboot(argc, argv);
							} else
#endif
								temp_cmd->func(temp_res);
							
							found_cmd = 1;
							break;
						}
					}
							
					if(!found_cmd)	write_user(temp_res->socket, "invalid command\n");		
						
				} else { //speech is assumed
					vwrite_talker("user %d says: %s\n", temp_res->socket, temp_res->buff);
				}
			}
		}
	}
	
	free(temp_res);
			
	close(server_socket);

#ifdef WIN32
	WSACleanup();
#endif	
	
	return 0;
}

void vwrite_talker(char *str, ...) {
	va_list arglist;

	talker_buff[0]='\0';
	va_start(arglist, str); //TODO: should I redo the va function to add our own %x in?
	vsprintf(talker_buff, str, arglist);
	va_end(arglist);
	
	write_talker(talker_buff);
}

void write_talker(char *message) {
	RES_OBJ temp_res;
		
	TAILQ_FOREACH(temp_res, &head, entries) {
		if(temp_res->rows > 0 && temp_res->columns > 0) {
			vwrite_user_x_y(temp_res, temp_res->rows, 0, message); 
		} else {
			send(temp_res->socket,message,strlen(message),0);
		}
	}
}

void talker_shutdown() {
	write_talker("talker is shutting down\n");
#ifdef WIN32
	WSACleanup();
#endif
	exit(0);
}

#ifdef __unix__
/*windows doesn't support execvup, even copying processes is a pain.
untill more research is done I'm just not going to include this command for
the windows builds */
void talker_sreboot(int argc, char *argv[]) {
	write_talker("seamless reboot is about to take place\n");
	write_resources_to_file();
	free(temp_res);
	close(server_socket);
	//first argument is the talker's executable
	execvp(*argv, argv);
}

#endif