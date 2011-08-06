#include <SDL.h>
#include "SDL_image.h"

#include "sdl_master.h"
#include "client.h"
#include "pack.h"

sdl_master::sdl_master(Uint32 flags)
{
	game_client = new SDL_Thread*[4];
	clients = new sdl_user*[4];
	for (int i = 0; i < 4; i++)
	{
		game_client[i] = (SDL_Thread*)0;
		clients[i] = (sdl_user*)0;
	}
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
	}
	else
	{
		display = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, flags);
		if (display == NULL) 
		{
			fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		}
		else
		{
			SDL_WM_SetCaption("Lineage", "Lineage");
			ready = 1;
		}
	}
}

void sdl_master::create_client()
{	
	if (game_client[0] == 0)
	{
		clients[0] = new sdl_user;
		game_client[0] = SDL_CreateThread(run_client, (void*)clients[0]);
	}
}


void sdl_master::process_events()
{
	bool done = false;
	SDL_Event event;
	while(!done)
	{ //While program isn't done
		draw();
		while(SDL_PollEvent(&event))
		{ //Poll events
			switch(event.type)
			{ //Check event type
				case SDL_QUIT: //User hit the X (or equivelent)
					done = true; //Make the loop end
					break; //We handled the event
				case SDL_VIDEORESIZE: //User resized window
					display = SDL_SetVideoMode(event.resize.w, event.resize.h, 32,
					SDL_HWSURFACE | SDL_RESIZABLE); // Create new window
					break; //Event handled, fetch next :)
			} //Finished with current event
		} //Done with all events for now
	} //Program done, exited
//	int status;
//	if (game_client[0] != 0)
//		SDL_WaitThread(game_client[0], &status);
}

void sdl_master::draw()
{
	SDL_Rect rect;
	if (clients[0] != 0)
	{
		clients[0]->draw();
		rect.x = 0;
		rect.y = 0;
		rect.w = clients[0]->display->w;
		rect.h = clients[0]->display->h;
		SDL_BlitSurface(clients[0]->display, &rect, display, &rect);
		SDL_Flip(display);
	}
	
}

sdl_master::~sdl_master()
{
	SDL_Quit();
}