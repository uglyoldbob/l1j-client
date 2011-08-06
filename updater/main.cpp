#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "sdl_master.h"

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		printf("WSAStartup failed!\n");
//		perror("WSAStartup failed!\n");
		return 1;
	}
	
	sdl_master graphics;
	graphics.create_client();
	graphics.process_events();
	
	WSACleanup();	
	return 0;
}
