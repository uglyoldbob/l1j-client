#include <SDL.h>
#include "SDL_image.h"

#include "class_sdl.h"
#include "pack.h"

class_sdl::class_sdl(Uint32 flags)
{
	ready = 0;
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

void class_sdl::do_stuff()
{
	bool done = false;
	SDL_Event event;
	while(!done)
	{ //While program isn't done
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
}

void class_sdl::give_data(graphics_data *abc)
{
	if (graphx != 0)
		delete graphx;
	graphx = abc;
}

SDL_Surface *class_sdl::get_image(const char *name, pack *source)
{
	char *buffer;
	SDL_RWops *sdl_buf;
	int size;
	buffer = (char*)source->load_file(name, &size, 0);
	sdl_buf = SDL_RWFromConstMem(buffer, size);
	return get_image(sdl_buf);
}

SDL_Surface *class_sdl::get_png_image(const char *name, pack *source)
{
	char *buffer;
	SDL_RWops *sdl_buf;
	int size;
	if (source != 0)
	{
		buffer = (char*)source->load_png(name, &size, 0);
		sdl_buf = SDL_RWFromConstMem(buffer, size);
		return get_image(sdl_buf);
	}
	else
	{
		return 0;
	}
}

SDL_Surface *class_sdl::get_image(SDL_RWops *buf)
{
	printf("Check returns %d\n", IMG_isPNG(buf));
	if (IMG_isPNG(buf) == 1)
	{
		printf("Converting png to surface\n");
		return IMG_LoadPNG_RW(buf);
	}
	return (SDL_Surface *)0;
}

void class_sdl::draw_load1()
{
	int index = getHashIndex("811.png") + 1;
	if (graphx != 0)
	{
		SDL_Surface *data = get_png_image("811.png", graphx->spritepack[index]);
		if (data == NULL)
		{
			printf("Loading surface is null\n");
		}
		else
		{
			SDL_Rect DestR;
	
		  	DestR.x = 0;
		  	DestR.y = 0;
	
		  	SDL_BlitSurface(data, NULL, display, &DestR);
			SDL_Flip(display);
			SDL_FreeSurface(data);
		}
	}
}

class_sdl::~class_sdl()
{
	delete graphx;
	SDL_Quit();
}