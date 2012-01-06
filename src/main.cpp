#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "resources/music.h"
#include "sdl_master.h"
#include "SDL_mixer.h"

int sha256_hash(char *filename, char *dest);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		printf("WSAStartup failed!\n");
//		perror("WSAStartup failed!\n");
	}

	char hash1[65], hash2[65];
	
	sha256_hash("lineage.exe", hash1);
	sha256_hash("admin.exe", hash2);
	
	printf("Received hashes\nlineage.exe: %s\nadmin.exe: %s\n",
		hash1, hash2);
	
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
