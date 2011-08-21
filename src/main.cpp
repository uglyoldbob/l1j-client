#include <stdio.h>

#include "resources/music.h"
#include "sdl_master.h"
#include "SDL_mixer.h"

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		printf("Failed to start SDL\n");
		return 1;
	}
	
	sdl_master graphics;
	graphics.create_client();
	graphics.process_events();
	
	SDL_Quit();
	return 0;
}
