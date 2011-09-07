#ifndef __TILE_H_
#define __TILE_H_

class pack;
#include "widgets/sdl_widget.h"

struct tileset
{
	short num_tiles;
	int *offset;	//contains num_tiles + 1 listings
	char *data;
};

class tile
{
	public:
		tile();
		~tile();
		void load(int which, pack *source);
		sdl_graphic *get_tile(int which);
		int get_amnt();
	private:
		tileset *tdata;	//holds all the information from the file
		
		char *filebuf;
};

#endif