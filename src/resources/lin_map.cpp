#include <SDL.h>
#include "SDL_endian.h"
#include <stdio.h>

#include "globals.h"
#include "pack.h"
#include "lin_map.h"
#include "widgets/sdl_widget.h"

lin_map::lin_map()
{
	mapdata = 0;
	filebuf = 0;
}

lin_map::~lin_map()
{
	if (filebuf != 0)
		delete [] filebuf;
	if (mapdata != 0)
	{
		delete mapdata;
	}
}

sdl_graphic *lin_map::get_map()
{
	sdl_graphic *ret;
	ret = 0;
	return ret;
}

void lin_map::load(int which)
{
	if (mapdata == 0)
	{
		char name[256];
		char *data;
		int size;
		
	}
}