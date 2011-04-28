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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <queue.h>

#include <ashes.h>
#include <telnet.h>

#define CMD_COUNT (sizeof(cmd_table)/sizeof(struct cmd_list_struct))

typedef struct command_obj {
	char *name;
	void (*func) ();
	TAILQ_ENTRY(command_obj)	entries;
} *CMD_OBJ;

CMD_OBJ create_command();
void destroy_command(CMD_OBJ cmd);
void create_commands();

#endif //COMMANDS_H