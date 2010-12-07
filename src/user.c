#include <stdio.h> //vsprintf
#include <stdarg.h> //va_list
#include <sys/socket.h> //send
#include <strings.h>
#include <unistd.h> // close
#include <stdlib.h>

#include <ashes.h>
#include <user.h>

TAILQ_HEAD(, resource_obj) head = TAILQ_HEAD_INITIALIZER(head);

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

RES_OBJ create_resource() {
	RES_OBJ temp_res = (RES_OBJ) malloc(sizeof(struct resource_obj));
	
	TAILQ_INSERT_TAIL(&head, temp_res, entries);
	
	return temp_res;
}

void connect_user(RES_OBJ res) {	
	write_user(res->socket,"\nconnected\n");
	
	write_user(res->socket,"welcome new user to the talker!\n");
}

void disconnect_user(RES_OBJ res) {	
	connected_clients--;
	
	TAILQ_REMOVE(&head, res, entries);
	
	close(res->socket);
	free(res);
}