#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <stdio.h>

#include "client.h"
#include "resources/coord.h"
#include "resources/map_coord.h"
#include "resources/pixel_coord.h"
#include "resources/screen_coord.h"
#include "resources/tile.h"
#include "globals.h"
#include "resources/pack.h"
#include "sdl_lin_map.h"
#include "sdl_widget.h"

sdl_lin_map::sdl_lin_map(sdl_user *who, int x, int y, int w, int h)
	: sdl_widget(x, y, who)
{
	tile_data = who->get_tiles();
	edit_mtx = SDL_CreateMutex();
	one = 0;
	
	for (int i = 0; i < 4; i++)
	{
		segs[i].graphic = 0;
		segs[i].mapdata = 0;
		segs[i].x = 0;
		segs[i].y = 0;
	}
	map = -1;
	
	one = new sdl_graphic(x, y, w, h, 0);
}

sdl_lin_map::~sdl_lin_map()
{
	for (int i = 0; i < 4; i++)
	{
		delete_segment(segs[i]);
	}
	SDL_DestroyMutex(edit_mtx);
}

//southeast	+24, +12
//northwest	-24, -12
//northeast	+24, -12
//southwest	-24, +12
//north		  0, +24
//south		  0, -24
//east		  0, +48
//west		  0, -48

void sdl_lin_map::draw_info(SDL_Surface *display, int x, int y)
{
	int startx, starty;
	startx = 100;
	starty = 250;
	char buffer[512];
	memset(buffer, 0, 512);
	
	int i;
	for (i = 0; i < 4; i++)
	{
		if ( (x >= segs[i].x) && (x < (segs[i].x+64)) &&
			(y >= segs[i].y) && (y < (segs[i].y+64)) )
		{
			break;
		}
	}
	if (i != 4)
	{
		sprintf(buffer, "%4d, %3d", segs[i].mapdata->floor[x-segs[i].x][2*(y-segs[i].y)]>>8, 
				segs[i].mapdata->floor[x-segs[i].x][2*(y-segs[i].y)] & 0xFF);
		lineage_font.draw(display, startx, starty, buffer, 0xFFFE);
		sprintf(buffer, "%4d, %3d", segs[i].mapdata->floor[x-segs[i].x][2*(y-segs[i].y)+1]>>8, 
				segs[i].mapdata->floor[x-segs[i].x][2*(y-segs[i].y)+1] & 0xFF);
		lineage_font.draw(display, startx, starty+10, buffer, 0xFFFE);
	}
	else
	{
		sprintf(buffer, "%d, %d not found?", x, y);
		lineage_font.draw(display, startx, starty, buffer, 0xFFFE);
	}
}

void sdl_lin_map::handle_s32(SDL_RWops *sdl_buf, lin_map_segment *ret, int mapnum, int x, int y, client *from)
{
	ret->mapdata = new lin_map_data;
	
	//read tile data
	for (int tx = 0; tx < 64; tx++)
	{
		for (int ty = 0; ty < 64; ty++)
		{
			SDL_RWread(sdl_buf, &(ret->mapdata->floor[ty][tx*2]), 4, 1);
			ret->mapdata->floor[ty][tx*2] = SWAP32(ret->mapdata->floor[ty][tx*2]);
			SDL_RWread(sdl_buf, &(ret->mapdata->floor[ty][tx*2+1]), 4, 1);
			ret->mapdata->floor[ty][tx*2+1] = SWAP32(ret->mapdata->floor[ty][tx*2+1]);
		}
	}
	
	//mystery data
	//TODO figure out this section of data
	SDL_RWread(sdl_buf, &ret->mapdata->num_unknown2, 2, 1);
	ret->mapdata->num_unknown2 = SWAP16(ret->mapdata->num_unknown2);
//	printf("Unknown 2 is %d\n", ret->mapdata->num_unknown2);
	if (ret->mapdata->num_unknown2 > 0)
	{	//7d068
		unsigned char *waste = new unsigned char[ret->mapdata->num_unknown2 * 6];
		SDL_RWread(sdl_buf, waste, 6, ret->mapdata->num_unknown2);
		//TODO : SWAP16 all the values in the array
		delete [] waste;
	}	
		
	//read attributes for each half tile
	for (int tx = 0; tx < 64; tx++)
	{
		for (int ty = 0; ty < 64; ty++)
		{
			ret->mapdata->attr[tx][ty*2] = 0;
			ret->mapdata->attr[tx][ty*2+1] = 0;
			SDL_RWread(sdl_buf, &ret->mapdata->attr[ty][tx*2], 2, 1);
			ret->mapdata->attr[ty][tx*2] = SWAP16(ret->mapdata->attr[ty][tx*2]);
			SDL_RWread(sdl_buf, &ret->mapdata->attr[ty][tx*2+1], 2, 1);
			ret->mapdata->attr[ty][tx*2+1] = SWAP16(ret->mapdata->attr[ty][tx*2+1]);
		}
	}



	int bla;
	SDL_RWread(sdl_buf, &bla, 4, 1);
	bla = SWAP32(bla);
	
	ret->mapdata->num_objects = bla;
	ret->mapdata->objs = new map_object[bla];
		
	for (int i = 0; i < bla; i++)
	{
		short a;
		SDL_RWread(sdl_buf, &a, 2, 1);	//this is an index number
		SDL_RWread(sdl_buf, &a, 2, 1);
		a = SWAP16(a);
		ret->mapdata->objs[i].num_tiles = a;
		ret->mapdata->objs[i].tiles = new map_tile_data[a];
		for (int j = 0; j < a; j++)
		{
			unsigned char b, c;
			SDL_RWread(sdl_buf, &b, 1, 1);
			SDL_RWread(sdl_buf, &c, 1, 1);
			if ((b == 0xcd) && (c == 0xcd))
			{
				char dummy[5];
				SDL_RWread(sdl_buf, dummy, 1, 5);
				j--;
				a--;
			}
			else
			{
				int dummy;
				char h;
				SDL_RWread(sdl_buf, &h, 1, 1);
				SDL_RWread(sdl_buf, &dummy, 1, 4);
				ret->mapdata->objs[i].tiles[j].x = b;
				ret->mapdata->objs[i].tiles[j].y = c;
				ret->mapdata->objs[i].tiles[j].h = h;
				ret->mapdata->objs[i].tiles[j].tiledata = dummy;
			}
		}
	}
	
	if (bla > 0)
	{	//7d12c
		//bla bla putObjecTile()
	}
	
	SDL_RWread(sdl_buf, &bla, 4, 1);
	bla = SWAP32(bla);
//	printf("amount for HideSwitches is %d\n", bla);
	for (int i = 0; i < bla; i++)
	{
		unsigned char a, b, d;
		unsigned short c;
		SDL_RWread(sdl_buf, &a, 1, 1);
		SDL_RWread(sdl_buf, &b, 1, 1);
		SDL_RWread(sdl_buf, &c, 2, 1);
		c = SWAP16(c);
		SDL_RWread(sdl_buf, &d, 1, 1);
	}
	
	SDL_RWread(sdl_buf, &bla, 4, 1);
	bla = SWAP32(bla);
//	printf("amount for tilesets is %d\nset: ", bla);
	for (int i = 0; i < bla; i++)
	{
		int set;
		SDL_RWread(sdl_buf, &set, 4, 1);
		set = SWAP32(set);
//		printf("%d, ", set);
	}
//	printf("\n");
	
	//portalList
	//array of Portal (uchar xOff, uchar yOff, targetmap, short tx, short ty)
	
	//TODO : check for end of file?
	unsigned short num_portals;
	SDL_RWread(sdl_buf, &num_portals, 2, 1);
	num_portals = SWAP16(num_portals);
//	printf("There are %d portals ", num_portals);
	for (int i = 0; i < num_portals; i++)
	{	//7cf54
		char a, b, c;
		short d, e, f;
		
		SDL_RWread(sdl_buf, &a, 1, 1);
		//skip a bytes of file?
		SDL_RWread(sdl_buf, &b, 1, 1);
		SDL_RWread(sdl_buf, &c, 1, 1);
		SDL_RWread(sdl_buf, &d, 2, 1);
		d = SWAP16(d);
		SDL_RWread(sdl_buf, &e, 2, 1);
		e = SWAP16(e);
		SDL_RWread(sdl_buf, &f, 2, 1);
		f = SWAP16(f);
//		printf("Data %d %d %d %d %d %d\n", a, b, c, d, e, f);
	}

//	if (SDL_RWtell(sdl_buf) < size)
	//	printf("Need to read some more stuff for this map segment\n");
	//if no at the end of file?
	{	//7d24c
		
	}
		
//	printf("Finish loading\n");

//end loading map data

//	printf("Took %d millis to load a section\n", SDL_GetTicks() - timecheck);

//dont draw the entire map section at once
#if 1
	int beg_x, beg_y;
	beg_x = x & (int)~0x3F;
	beg_y = y & (int)~0x3F;
	
	ret->graphic = new sdl_graphic(0, 0, 3072, 1535, 0);
	
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
			selal = ret->mapdata->floor[tx][ty*2]>>8;
			selbl = ret->mapdata->floor[tx][ty*2] & 0xFF;
			
			selar = ret->mapdata->floor[tx][ty*2+1]>>8;
			selbr = ret->mapdata->floor[tx][ty*2+1] & 0xFF;
			
			tile_data[selal].load(selal, from);
			tile_data[selar].load(selar, from);
			
			dx = tempmap.get_screen().get_x() + offsetx;
			dy = tempmap.get_screen().get_y() + offsety;
			left = tile_data[selal].get_tile_left(selbl);
			right = tile_data[selar].get_tile_right(selbr);
			
			if (left != 0)
				left->drawat(dx, dy, ret->graphic->get_surf());
			if (right != 0)
				right->drawat(dx+24, dy, ret->graphic->get_surf());
		}
	}
#if 1
	for (int i = 0; i < ret->mapdata->num_objects; i++)
	{
		for (int j = 0; j < ret->mapdata->objs[i].num_tiles; j++)
		{
			sdl_graphic *left, *right;
			int tx = ret->mapdata->objs[i].tiles[j].x / 2;
			int ty = ret->mapdata->objs[i].tiles[j].y;
			int floor = ret->mapdata->objs[i].tiles[j].tiledata;
			map_coord tempmap(x + tx, y + ty);
			int dx, dy;
			int selal, selbl;
			selal = floor>>8;
			selbl = floor & 0xFF;

			tile_data[selal].load(selal, from);

			dx = tempmap.get_screen().get_x() + offsetx;
			dy = tempmap.get_screen().get_y() + offsety;
			if ((ret->mapdata->objs[i].tiles[j].x % 2) == 1)
			{
				dx += 24;
			}
			left = tile_data[selal].get_special(selbl);
			
			if (left != 0)
				left->drawat(dx, dy, ret->graphic->get_surf());
		}
	}
#endif
	ret->map = mapnum;
	ret->x = x & (~0x3F);
	ret->y = y & (~0x3F);
	ret->offsetx = offsetx;
	ret->offsety = offsety;
#endif
//	printf("\t%s %d_%d_%d\n", name, mapnum, ret->x, ret->y);
	
//	sprintf(name, "%d_%d_%d.bmp", mapnum, ret->x, ret->y);
//	SDL_SaveBMP(ret->graphic->get_surf(), name);
	
//	printf("Took %d millis to load/draw a section\n", SDL_GetTicks() - timecheck);
}

void sdl_lin_map::handle_seg(SDL_RWops *sdl_buf, lin_map_segment *ret, int mapnum, int x, int y, client *from)
{
	ret->graphic = 0;
	ret->mapdata = 0;
	ret->map = 0;
	ret->x = 0;
	ret->y = 0;
	ret->offsetx = 0;
	ret->offsety = 0;
}

lin_map_segment sdl_lin_map::get_map(int mapnum, int x, int y, client *from)
{
//	Uint32 timecheck = SDL_GetTicks();
	lin_map_segment ret;
	ret.graphic = 0;
	ret.mapdata = 0;
	ret.map = 0;
	ret.x = 0;
	ret.y = 0;
	ret.offsetx = 0;
	ret.offsety = 0;
	
//begin loading map data
	int modx, mody;
	modx = (x>>6) + 0x7e00;
	mody = (y>>6) + 0x7e00;
	
	int beg_x2, beg_y2;
	beg_x2 = x & (int)~0x3F;
	beg_y2 = y & (int)~0x3F;
	
	char name[256];
	int size;
	sprintf(name, "map/%d/%04x%04x.s32", mapnum, modx, mody);
	char *buffer;
	SDL_RWops *sdl_buf;
	buffer = (char*)from->getfiles->load_file(name, &size, FILE_REGULAR1, 0);
	if (buffer == 0)
	{
		sprintf(name, "map/%d/%04x%04x.seg", mapnum, modx, mody);
		buffer = (char*)from->getfiles->load_file(name, &size, FILE_REGULAR1, 0);
		if (buffer != 0)
		{
			sdl_buf = SDL_RWFromConstMem(buffer, size);
			handle_seg(sdl_buf, &ret, mapnum, x, y, from);
			SDL_RWclose(sdl_buf);
			delete [] buffer;
		}
		else
		{
			//printf("This map section %d, %d does not exist\n",
			//	   beg_x2, beg_y2);
			ret.graphic = 0;
			ret.mapdata = 0;
			ret.map = 0;
			ret.x = 0;
			ret.y = 0;
			ret.offsetx = 0;
			ret.offsety = 0;
		}
	}
	else
	{
		sdl_buf = SDL_RWFromConstMem(buffer, size);
		handle_s32(sdl_buf, &ret, mapnum, x, y, from);
		SDL_RWclose(sdl_buf);	
		delete [] buffer;
	}
	return ret;
}

void sdl_lin_map::delete_segment(lin_map_segment delme)
{
	if (delme.graphic != 0)
	{
		delete delme.graphic;
		delme.graphic = 0;
	}
	if (delme.mapdata != 0)
	{
		for (int i = 0; i < delme.mapdata->num_objects; i++)
		{
			if (delme.mapdata->objs[i].tiles != 0)
			{
				delete [] delme.mapdata->objs[i].tiles;
				delme.mapdata->objs[i].tiles = 0;
			}
		}
		if (delme.mapdata->objs != 0)
		{
			delete [] delme.mapdata->objs;
			delme.mapdata->objs = 0;
		}
		delete delme.mapdata;
		delme.mapdata = 0;
	}
}

/** Make sure the 4 sections that are loaded actually covers the screen. 
 * Swap segments around as necessary to minimize data loading. 
 * Load map segments if required. */
void sdl_lin_map::check_sections(client *from)
{
	int width, height;
	width = one->getw();
	height = one->geth();
	
	int corner_x[4], corner_y[4];
	int goal_x[4], goal_y[4];
	int present[4];	//covers the 4 possible sections required
	int lowx, lowy;
	
	screen_coord corner1(0 - master_offsetx, 0 - master_offsety);
	corner_x[0] = corner1.get_map().get_x() & (~0x3F);
	corner_y[0] = corner1.get_map().get_y() & (~0x3F);

	screen_coord corner2(width - master_offsetx, 0 - master_offsety);
	corner_x[1] = corner2.get_map().get_x() & (~0x3F);
	corner_y[1] = corner2.get_map().get_y() & (~0x3F);
	
	screen_coord corner3(0 - master_offsetx, height - master_offsety);
	corner_x[2] = corner3.get_map().get_x() & (~0x3F);
	corner_y[2] = corner3.get_map().get_y() & (~0x3F);
	
	screen_coord corner4(width - master_offsetx, height - master_offsety);
	corner_x[3] = corner4.get_map().get_x() & (~0x3F);
	corner_y[3] = corner4.get_map().get_y() & (~0x3F);
	
	lowx = corner_x[0];
	lowy = corner_y[0];
	
	for (int i = 0; i < 4; i++)
	{
		present[i] = -1;
		if (corner_x[i] < lowx)
			lowx = corner_x[i];
		if (corner_y[i] < lowy)
			lowy = corner_y[i];
	}
	
	goal_x[0] = lowx;
	goal_y[0] = lowy;
	
	goal_x[1] = lowx;
	goal_y[1] = lowy + 64;
	
	goal_x[2] = lowx + 64;
	goal_y[2] = lowy;
	
	goal_x[3] = lowx + 64;
	goal_y[3] = lowy + 64;

	for (int i = 0; i < 4; i++)
	{	//check to see if each segment is loaded properly
		if ((segs[i].x != goal_x[i]) ||
			(segs[i].y != goal_y[i]) ||
			(segs[i].map != map))
		{
			int match = -1;
			for (int j = i + 1; j < 4; j++)
			{	//check the other segments to see if they match
				if ((segs[j].x == goal_x[i]) &&
					(segs[j].y == goal_y[i]) &&
					(segs[j].map == map))
				{
					match = j;
				}
			}
			if (match != -1)
			{	//swap the segments because a match was found
				lin_map_segment temp;
				temp = segs[i];
				segs[i] = segs[match];
				segs[match] = temp;
			}
			else
			{	//load a section because it's not already loaded
				while (SDL_mutexP(edit_mtx) == -1) {};
				delete_segment(segs[i]);
				segs[i] = get_map(map, goal_x[i], goal_y[i], from);
				SDL_mutexV(edit_mtx);
			}
		}
	}
}

void sdl_lin_map::draw(SDL_Surface *display)
{
	while (SDL_mutexP(edit_mtx) == -1) {};
	SDL_FillRect(one->get_surf(), NULL, 0x1234);

	for (int i = 0; i < 4; i++)
	{
		int temp_offx, temp_offy;
			
		if (segs[i].graphic != 0)
		{
			temp_offx = master_offsetx - segs[i].offsetx;
			temp_offy = master_offsety - segs[i].offsety;
		
			segs[i].graphic->drawat(temp_offx, temp_offy, one->get_surf());
		}
	}

	//draws the full map onto display
	one->draw(display);
	SDL_mutexV(edit_mtx);
}

void sdl_lin_map::set_hotspot(int mapn, int x, int y)
{
	int width, height;
	
	map = mapn;
	width = one->getw();
	height = one->geth();
	
	map_coord themap(x, y);
	screen_coord thescreen = themap.get_screen();

	master_offsetx = (width/2) - thescreen.get_x();
	master_offsety = (height/2) - thescreen.get_y();
	client_request t_sdl;
	t_sdl.request = CLIENT_REQUEST_CHECK_MAP;
	t_sdl.data.mcheck.item = this;
	myclient->add_request(t_sdl);
}

int sdl_lin_map::get_offset_x()
{
	return master_offsetx;
}

int sdl_lin_map::get_offset_y()
{
	return master_offsety;
}
