/*	Copyright (C) 2010  Timothy Rhodes <phoenixevolution@gmail.com> 
	Project: Ashes
*/

#ifndef COMMAND_LIST_H
#define COMMAND_LIST_H

struct cmd_list_struct {
	char *name;
	void (*fname) ();
} static cmd_table[]={
	{"quit",	resource_quits	},
	{"clear",	clear_screen	},
	{"examine",	examine		},
	{"name",	set_name	},
#ifdef __unix__
	{"sreboot",	talker_sreboot	},
#endif
	{"shutdown",	talker_shutdown	},
	{"tv",		telnet_view	}
};

#endif