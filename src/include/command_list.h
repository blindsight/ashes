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