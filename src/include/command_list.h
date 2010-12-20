#ifndef COMMAND_LIST_H
#define COMMAND_LIST_H

struct cmd_list_struct {
	char *name;
	void (*fname) ();
} static cmd_table[]={
	{"quit",	resource_quits	},
	{"clear",	clear_screen	},
	{"examine",	examine		},
	{"sreboot",	talker_sreboot	},
	{"shutdown",	talker_shutdown	},
	{"tv",		telnet_view	}
};

#endif