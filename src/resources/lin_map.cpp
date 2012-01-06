#include <SDL.h>
#include "SDL_endian.h"
#include <stdio.h>

#include "coord.h"
#include "map_coord.h"
#include "pixel_coord.h"
#include "screen_coord.h"
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

//southeast	+24, +12
//northwest	-24, -12
//northeast	+24, -12
//southwest	-24, +12
//north		  0, +24
//south		  0, -24
//east		  0, +48
//west		  0, -48
	
sdl_graphic *lin_map::get_map(int x, int y)
{
	sdl_graphic *ret;
	
	int modx, mody;
	modx = ((x - 36)>>7) + 0x7f01;
	mody = (y>>6) + 0x7e00;
	
	int beg_x, beg_y;
	beg_x = x & (int)~0x3F;
	beg_y = y & (int)~0x3F;
	
	int width, height;
	width = 688;
	height = 528;
	
	//map x, y to 320, 240 by calculating the map offsets (x and y)
	//load and draw tile data for tiles visible on screen
		//there are two tiles to draw for each map coordinate
	
	char filename[100];
	
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