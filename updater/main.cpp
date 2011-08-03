#include <stdio.h>

#include "client.h"

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		printf("WSAStartup failed!\n");
//		perror("WSAStartup failed!\n");
		return 1;
	}
	
	client game;
	try
	{
		game.init();
	}
	catch (const char *error)
	{
		printf("%s", error);
	}
	
	WSACleanup();
	return 0;
}
