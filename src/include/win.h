#ifndef WIN_H
#define WIN_H

#define close(socket_closing) closesocket(socket_closing)

int file_exists(const char *fileName);
void setup_talker();
#endif