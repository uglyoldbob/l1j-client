#include <SDL.h>
#include "SDL_endian.h"
#include <stdio.h>

#include "client.h"
#include "coord.h"
#include "map_coord.h"
#include "pixel_coord.h"
#include "screen_coord.h"
#include "tile.h"
#include "globals.h"
#include "pack.h"
#include "lin_map.h"
#include "widgets/sdl_widget.h"

lin_map::lin_map(tile *thetiles, client *who)
{
	tile_data = thetiles;
	whole_map = 0;
	this->who = who;
	
	for (int i = 0; i < 4; i++)
	{
		segs[i].graphic = 0;
		segs[i].mapdata = 0;
	}
}

lin_map::~lin_map()
{
	for (int i = 0; i < 4; i++)
	{
		if (segs[i].graphic != 0)
			delete segs[i].graphic;
		if (segs[i].mapdata != 0)
			delete segs[i].mapdata;
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
	
sdl_graphic *lin_map::get_map(int mapnum, int x, int y, int segnum)
{
	sdl_graphic *ret;
	
//begin loading map data
	int modx, mody;
	modx = ((x - 36)>>7) + 0x7f01;
	mody = (y>>6) + 0x7e00;
	
	printf("Loading file map/%d/%04x%04x.s32\n", mapnum, modx, mody);
	char name[256];
	int size;
	sprintf(name, "map/%d/%04x%04x.s32", mapnum, modx, mody);
	char *buffer;
	SDL_RWops *sdl_buf;
	buffer = (char*)who->getfiles->load_file(name, &size, FILE_REGULAR1, 0);
	if (buffer == 0)
	{
		return 0;
	}
	
	sdl_buf = SDL_RWFromConstMem(buffer, size);
	segs[segnum].mapdata = new lin_map_data;
	
	//read tile data
	for (int tx = 0; tx < 64; tx++)
	{
		for (int ty = 0; ty < 64; ty++)
		{
			SDL_RWread(sdl_buf, &segs[segnum].mapdata->floor[tx][ty*2], 4, 1);
			SDL_RWread(sdl_buf, &segs[segnum].mapdata->floor[tx][ty*2+1], 4, 1);
		}
	}
	
	//mystery data
	SDL_RWread(sdl_buf, &segs[segnum].mapdata->num_unknown2, 2, 1);
	for (int i = 0; i < segs[segnum].mapdata->num_unknown2; i++)
	{
		short *waste = new short[segs[segnum].mapdata->num_unknown2];
		SDL_RWread(sdl_buf, waste, 2, segs[segnum].mapdata->num_unknown2);
		delete [] waste;
	}
		
	//read attributes for each half tile
	for (int tx = 0; tx < 64; tx++)
	{
		for (int ty = 0; ty < 64; ty++)
		{
			SDL_RWread(sdl_buf, &segs[segnum].mapdata->attr[tx][ty*2], 2, 1);
			SDL_RWread(sdl_buf, &segs[segnum].mapdata->attr[tx][ty*2+1], 2, 1);
		}
	}
	
	
	
	SDL_RWclose(sdl_buf);

//end loading map data

	int beg_x, beg_y;
	beg_x = x & (int)~0x3F;
	beg_y = y & (int)~0x3F;
	
	int width, height;
	width = 3072;
	height = 1535;
	
	ret = new sdl_graphic(0, 0, width, height);
	segs[segnum].graphic = ret;
	
	//draw all the tiles for the map section
	int offsetx, offsety;
	map_coord themap(x, y);
	screen_coord thescreen = themap.get_screen();
	offsetx = 0 - thescreen.get_x();
	offsety = 756 - thescreen.get_y();
	
	for (int tx = 0; tx < 64; tx++)
	{
		for (int ty = 0; ty < 64; ty++)
		{
			sdl_graphic *left, *right;
			map_coord tempmap(x + tx, y + ty);
			int dx, dy;
			int selal, selbl, selar, selbr;
			selal = segs[segnum].mapdata->floor[tx][ty*2]>>8;
			selbl = segs[segnum].mapdata->floor[tx][ty*2] & 0xFF;
			
			selar = segs[segnum].mapdata->floor[tx][ty*2+1]>>8;
			selbr = segs[segnum].mapdata->floor[tx][ty*2+1] & 0xFF;
			
			tile_data[selal].load(selal, who);
			tile_data[selar].load(selar, who);
			
			dx = tempmap.get_screen().get_x() + offsetx;
			dy = tempmap.get_screen().get_y() + offsety;
			left = tile_data[selal].get_tile_left(selbl);
			right = tile_data[selar].get_tile_right(selbr);
			
			left->drawat(dx, dy, ret->get_surf());
			right->drawat(dx+24, dy, ret->get_surf());
		}
	}

	segs[segnum].map = mapnum;
	segs[segnum].x = x & (~0x3F);
	segs[segnum].y = y & (~0x3F);
	segs[segnum].offsetx = offsetx;
	segs[segnum].offsety = offsety;
	
	return ret;
}


void lin_map::draw(SDL_Surface *display)
{
	//update the drawn portion of the map
	//TODO!
	//draws the full map onto display
	whole_map->draw(display);
}

void lin_map::set_window(int x, int y, int w, int h)
{
	whole_map = new sdl_graphic(x, y, w, h);
}
