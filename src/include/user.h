#ifndef USER_H
#define USER_H

/*prototypes */
void write_user(int socket, char *message);
void vwrite_user(int socket, char *str, ...);

int connect_user(int new_socket);
void disconnect_user(int socket);

#endif