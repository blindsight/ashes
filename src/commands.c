/*	Copyright (C) 2010  Timothy Rhodes <phoenixevolution@gmail.com> 
	
This file is part of Ashes.

Ashes is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ashes is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ashes.  If not, see <http://www.gnu.org/licenses/>.
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