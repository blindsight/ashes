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
	temp_res->word_count = 0;
	
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

RES_OBJ get_res(int socket_test) {
	RES_OBJ res_about;
	
	TAILQ_FOREACH(res_about, &head, entries) {
		if(res_about->socket == socket_test) {
			return res_about;
		}
	}
	
	return NULL;
}

void examine(RES_OBJ res) {
	char output[1024];
	RES_OBJ about_res;
	
	if(res->word_count<2) {
		about_res = res;
	} else {
		if((about_res = get_res(strtol(res->last_words[1], NULL, 10))) == NULL) {
			vwrite_talker("user invalid %d", res->socket);
			about_res = res;
		}
	}
	
	sprintf(output, "user %d's details\n",about_res->socket);
	sprintf(output, "%s charset: %d %s\n",output, about_res->charset, about_res->charcode);
	sprintf(output, "%s window size: %d rows: %d cols: %d\n",output, about_res->naws, about_res->rows, about_res->columns);
	sprintf(output, "%s terminal type: %d %s\n\n",output, about_res->term_type, about_res->term);
	
	write_user(res->socket, output);
}

void resource_quits(RES_OBJ res) {
	vwrite_talker("user %d leaves\n", res->socket);
	disconnect_user(res);	
}

void telnet_view(RES_OBJ res) {
	if(res->telnet_view) {
		write_user(res->socket,"telnet view commands off");
		res->telnet_view = 0;
	} else {
		write_user(res->socket,"telnet view commands on");
		res->telnet_view = 1;
	}
}

void create_last_words(RES_OBJ res) {
	char *inputcopy, *token;
	int last_index = 0;
	
	inputcopy = strdup(res->buff);
	
	while((token = strsep(&inputcopy, " ")) != NULL && last_index<LAST_WORDS_MAX) {
		res->last_words[last_index] = token;
		
		last_index++;
	}
	
	if(last_index == 0) { //no spaces were ever used.
		res->last_words[last_index] = res->buff;
		last_index++;
	}
	
	res->word_count = last_index;
}