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

#ifndef WIN32
#include <stdio.h> //printf
#include <signal.h> //signal
#include <stdlib.h> //exit
#include <unistd.h> //access R_OK

#include <unix.h>

void setup_talker() {
	signal(SIGKILL,catch_kill);
	signal(SIGINT,catch_kill);
	signal(SIGPIPE,pipe_handler); //broken sigpipe doesn't need to kill the talker
}

void catch_kill(int sig_num) {
	printf("program is being killed %d\n",sig_num);
	exit(0);
}

void pipe_handler(int sig_num) { //whats the best way to deal with a broken pipe?
	return;
}

//1 for found and 0 for not found
int file_exists(const char *filename) {
	return !access("SREBOOT_FILE",R_OK); //access returns 0 if true, thus we need to flip it
}

#endif //WIN32