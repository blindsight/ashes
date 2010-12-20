#ifndef UNIX_H
#define UNIX_H

void catch_kill(int sig_num);
void pipe_handler(int sig_num);
int file_exists(const char *filename);

#endif //UNIX_H