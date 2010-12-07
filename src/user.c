#include <stdio.h> //vsprintf
#include <stdarg.h> //va_list
#include <sys/socket.h> //send
#include <strings.h>
#include <unistd.h> // close

#include <ashes.h>
#include <user.h>

void vwrite_user(int socket, char *str, ...) {
	va_list arglist;
	va_start(arglist, str); //TODO: should I redo the va function to add our own %x in?
	vsprintf(talker_buff, str, arglist);
	va_end(arglist);
	
	write_user(socket, talker_buff);
}

void write_user(int socket, char *message) {
	send(socket,message,strlen(message),0);
}

int connect_user(int new_socket) {
	write_user(new_socket,"\nconnected\n");
	
	for (int i=0; i<=MAX_CLIENTS; i++) {
		if (client_sock[i] == 0) {
			client_sock[i] = new_socket;
			break;
		}
	}
	
	write_user(new_socket,"welcome a new user to the talker!\n");
	
	return new_socket;
}

void disconnect_user(int socket) {
	connected_clients--;
	close(socket);
}