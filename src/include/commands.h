#ifndef COMMANDS_H
#define COMMANDS_H

#include <sys/queue.h>

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