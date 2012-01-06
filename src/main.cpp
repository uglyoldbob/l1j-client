#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "resources/music.h"
#include "sdl_master.h"
#include "SDL_mixer.h"

int sha256_hash(char *filename);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		printf("WSAStartup failed!\n");
//		perror("WSAStartup failed!\n");
	}

	sha256_hash("lineage.exe");
	sha256_hash("admin.exe");
	
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		printf("Failed to start SDL\n");
		return 1;
	}
	
	sdl_master graphics;
	graphics.create_client();
	graphics.process_events();
	
	printf("Exiting now\n");
	
	WSACleanup();	
	return 0;
}
