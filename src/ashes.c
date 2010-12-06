#include <stdio.h> //perror printf
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <errno.h> //int errno
#include <unistd.h>
#include <stdarg.h> //va_list
#include <signal.h> //signal
#include <stdlib.h> //exit
#include <arpa/inet.h>


#include <ashes.h>
#include <user.h>

int main() {
	signal(SIGKILL,catch_kill);
	signal(SIGINT,catch_kill);
	int server_socket;
	int true = 1;
	struct sockaddr_in6 addr;
	int addrsize=sizeof(struct sockaddr_in6);
	int new_socket;
	fd_set socklist; //list of what sockets to listen on
	
	if((server_socket = socket(PF_INET6, SOCK_STREAM, 0)) == -1) {
		perror("unable to create socket");
		exit(EXIT_FAILURE);
	}
	
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(true))<0) {
		perror("setsockopt");
		
	}

//	struct sockaddr_in* ia = (struct sockaddr_in*)&addr;
	
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(SERVER_TELNET_SOCKET);
	
//	inet_pton( AF_INET6, TALKER_ADDR, (void *)&address.sin6_addr.s6_addr);
	
	if(bind(server_socket,(struct sockaddr *)&addr,sizeof(struct sockaddr_in6)) != 0) {
		perror(strerror(errno));
		close(server_socket);
		return 1;
		
		/* TODO: Possible errors to check for
			[EACCES]           The requested address is protected, and the current user has inadequate permission to access it.
			[EADDRINUSE]       The specified address is already in use.
			[EADDRNOTAVAIL]    The specified address is not available from the local machine.
			[EAFNOSUPPORT]     address is not valid for the address family of socket.
			[EBADF]            socket is not a valid file descriptor.
			[EDESTADDRREQ]     socket is a null pointer.
			[EFAULT]           The address parameter is not in a valid part of the user address space.
			[EINVAL]           socket is already bound to an address and the protocol does not support binding to a new address.  Alternatively, socket may have been shut
			              down.
			[ENOTSOCK]         socket does not refer to a socket.
			[EOPNOTSUPP]       socket is not of a type that can be bound to an address.
			The following errors are specific to binding names in the UNIX domain.
			[EACCES]           A component of the path prefix does not allow searching or the node's parent directory denies write permission.
			[EIO]              An I/O error occurred while making the directory entry or allocating the inode.
			[EISDIR]           An empty pathname was specified.
			[ELOOP]            Too many symbolic links were encountered in translating the pathname.  This is taken to be indicative of a looping symbolic link.
			[ENAMETOOLONG]     A component of a pathname exceeded {NAME_MAX} characters, or an entire path name exceeded {PATH_MAX} characters.
			[ENOENT]           A component of the path name does not refer to an existing file.
			[ENOTDIR]          A component of the path prefix is not a directory.
			[EROFS]            The name would reside on a read-only file system.
		*/
	}
	
	printf("listening for telnet connections on port %d\n",SERVER_TELNET_SOCKET);
	listen(server_socket,3);
	
	printf("waiting to accept connection\n");

	int activity;
	
	for(;;) {
		FD_ZERO(&socklist);
		
		FD_SET(server_socket, &socklist);
		int i;
		for(i=0; i<=MAX_CLIENTS; i++) {
			if(client_sock[i] > 0 ) {
				FD_SET(client_sock[i], &socklist);
			}
		}
		

		if((activity=select(connected_clients+4, &socklist, NULL, NULL, NULL)) < 0 ) { //select error
			perror("select error");

		}

		if(FD_ISSET(server_socket, &socklist)) {
			memset (&addr, 0, sizeof (addr));
			//handle new connection
			if((new_socket = accept(server_socket, (struct sockaddr *)&addr, (socklen_t *)&addrsize))<0) {
				perror(strerror(errno));
				close(server_socket);
				close(new_socket);

				/* TODO: Possible errors to check for
					[EBADF]            socket is not a valid file descriptor.
					[ECONNABORTED]     The connection to socket has been aborted.
					[EFAULT]           The address parameter is not in a writable part of the user address space.
					[EINTR]            The accept() system call was terminated by a signal.
					[EINVAL]           socket is unwilling to accept connections.
					[EMFILE]           The per-process descriptor table is full.
					[ENFILE]           The system file table is full.
					[ENOMEM]           Insufficient memory was available to complete the operation.
					[ENOTSOCK]         socket references a file type other than a socket.
					[EOPNOTSUPP]       socket is not of type SOCK_STREAM and thus does not accept connections.
					[EWOULDBLOCK]      socket is marked as non-blocking and no connections are present to be accepted.
				*/
				continue;
			}

			connected_clients++;
			
			if(connected_clients > MAX_CLIENTS) {
				write_user(new_socket,"\nTalker is filled to capacity... goodbye\n");
				disconnect_user(new_socket);
				continue;
			}
			write_user(new_socket,"\nconnected\n");
			
			for (i=0; i<=MAX_CLIENTS; i++) {
				if (client_sock[i] == 0) {
					client_sock[i] = new_socket;
					break;
				}
			}
			
			write_user(new_socket,"welcome a new user to the talker!\n");	
		}
		
		for (i=0; i<=MAX_CLIENTS; i++) {
			if (FD_ISSET(client_sock[i], &socklist)) {
				int bytes_read = 0;
				if ((bytes_read = read(client_sock[i], user_buff[i], 1024)) < 0) {
					close(client_sock[i]);
					client_sock[i] = 0;
				}
				
				user_buff[i][bytes_read] = '\0';
				
				vwrite_talker("user %d says: %s", i, user_buff[i]);
				
				break;
			}
		}
	}
			
	close(server_socket);
	close(new_socket);
	
	return 0;
}

void vwrite_talker(char *str, ...) {
	va_list arglist;
	va_start(arglist, str); //TODO: should I redo the va function to add our own %x in?
	vsprintf(talker_buff, str, arglist);
	va_end(arglist);
	
	write_talker(talker_buff);
}

void write_talker(char *message) {
	int i;
	
	for (i=0; i<MAX_CLIENTS; i++) {
		if (client_sock[i] > 0) {
			send(client_sock[i],message,strlen(message),0);
		}
	}
}

void catch_kill(int sig_num) {
	printf("program is being killed %d\n",sig_num);
	exit(0);
}
