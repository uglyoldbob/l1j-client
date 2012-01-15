#ifndef __TILE_H_
#define __TILE_H_

class client;
class pack;
#include "widgets/sdl_widget.h"

struct tileset
{
	short num_tiles;
	int *offset;	//contains num_tiles + 1 listings
	sdl_graphic **ltile;
	sdl_graphic **rtile;
	sdl_graphic **stile;
	char *data;
};

class tile
{
	public:
		tile();
		~tile();
		void load(int which, client *who);
		sdl_graphic *get_tile_right(int which);
		sdl_graphic *get_tile_left(int which);
		int get_amnt();
	private:
		tileset *tdata;	//holds all the information from the file
		
		char *filebuf;
		sdl_graphic *get_special(int which);
};

#endif