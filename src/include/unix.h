/*	Copyright (C) 2010  Timothy Rhodes <phoenixevolution@gmail.com> 
	Project: Ashes
*/

#ifndef UNIX_H
#define UNIX_H

#ifndef WIN32
void catch_kill(int sig_num);
void pipe_handler(int sig_num);
int file_exists(const char *filename);
void setup_talker();
#endif

#endif //UNIX_H