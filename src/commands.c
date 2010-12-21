/*	Copyright (C) 2010  Timothy Rhodes <phoenixevolution@gmail.com> 
	Project: Ashes
*/

#include <stdio.h> //NULL
#include <stdlib.h> //malloc

#include <commands.h>
#include <command_list.h>

TAILQ_HEAD(, command_obj) cmd_list = TAILQ_HEAD_INITIALIZER(cmd_list);

CMD_OBJ create_command() {
	CMD_OBJ temp_cmd = (CMD_OBJ) malloc(sizeof(struct command_obj));
	
	TAILQ_INSERT_TAIL(&cmd_list, temp_cmd, entries);
	
	return temp_cmd;	
}

void destroy_command(CMD_OBJ cmd) {
	TAILQ_REMOVE(&cmd_list, cmd, entries);
	
	free(cmd);
}

void create_commands() {
	CMD_OBJ temp_cmd;
	int i;
	
	for(i=0; i<CMD_COUNT; i++) {
		temp_cmd = create_command();
		temp_cmd->name = cmd_table[i].name;
		temp_cmd->func = cmd_table[i].fname;
	}
}