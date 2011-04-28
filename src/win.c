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