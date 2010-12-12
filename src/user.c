#include <stdio.h> //vsprintf
#include <stdarg.h> //va_list
#include <sys/socket.h> //send
#include <string.h>
#include <unistd.h> // close
#include <stdlib.h>

#include <ashes.h>
#include <user.h>
#include <telnet.h>

TAILQ_HEAD(, resource_obj) head = TAILQ_HEAD_INITIALIZER(head);
extern int connected_clients;

void vwrite_user(int socket, char *str, ...) {
	talker_buff[0]='\0';
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
	
	temp_res->rows = 0;
	temp_res->columns = 0;
	temp_res->naws = 0;
	temp_res->term_type = 0;
	temp_res->charset = 0;
	temp_res->telnet_view = 0;
	
	return temp_res;
}

void connect_user(RES_OBJ res) {
	request_naws(res);
	request_charset(res);
	request_option(res, TERMINAL_TYPE);
	write_user(res->socket,"welcome new user to the talker!\n");
}

void disconnect_user(RES_OBJ res) {	
	connected_clients--;
	
	TAILQ_REMOVE(&head, res, entries);
	
	close(res->socket);
	free(res);
}

void write_resources_to_file() {
	FILE *sreboot_file;
	
	if((sreboot_file = fopen(SREBOOT_FILE,"w")) != NULL) {
		RES_OBJ temp_res;
	
		//dump all sockets to the sreboot file
		TAILQ_FOREACH(temp_res, &head, entries) {
			fwrite(&temp_res->socket,sizeof(temp_res->socket),1, sreboot_file);
			TAILQ_REMOVE(&head, temp_res, entries);
		}
		
		fclose(sreboot_file);
	} else {
		perror("unable to do a seamless reboot\n");
	}
	
}

void read_resources_from_file() {
	FILE *sreboot_file;
	
	if((sreboot_file = fopen(SREBOOT_FILE,"r")) != NULL) {
		RES_OBJ loop_res = (RES_OBJ)malloc(sizeof(struct resource_obj));
		RES_OBJ temp_res;
		
		while((fread(&loop_res->socket,sizeof(loop_res->socket), 1,sreboot_file))) {
			connected_clients++;
			temp_res = create_resource();	
			temp_res->socket = loop_res->socket;
		}		
		fclose(sreboot_file);
	} else {
		perror("can't open sreboot file\n");
		exit(1);
	}
}

void examine(RES_OBJ res, RES_OBJ to_res) {
	char output[1024];
	
	if(res == NULL) return;
	
	sprintf(output, "user %d's details\n",res->socket);
	sprintf(output, "%s charset: %d %s\n",output, res->charset, res->charcode);
	sprintf(output, "%s window size: %d rows: %d cols: %d\n",output, res->naws, res->rows, res->columns);
	sprintf(output, "%s terminal type: %d %s\n\n",output, res->term_type, res->term);
	
	write_user(to_res->socket, output);
}