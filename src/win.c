#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h> 
#include <stdio.h>
#include <stdlib.h> //exit

void setup_talker() {
	int result;
	WSADATA wsaData;

	//TODO: check winsock version.
	// Initialize Winsock
	result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error: %d\n", result);
		exit(1);
	}
}


int file_exists(const char *fileName)
{
    DWORD	file_attr;

    file_attr = GetFileAttributes(fileName);

    if (0xFFFFFFFF == file_attr)
        return 0;
    return 1;
}

#endif