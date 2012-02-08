#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "resources/music.h"
#include "sdl_master.h"
#include <SDL/SDL_mixer.h>

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		printf("WSAStartup failed!\n");
//		perror("WSAStartup failed!\n");
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		printf("Failed to start SDL\n");
		return 1;
	}
	
	sdl_master graphics;
	graphics.create_client();
	graphics.process_events();
	
	printf("Exiting now\n");

	return 0;
}
