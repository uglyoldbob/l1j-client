#ifndef __LIN_MAP_H_
#define __LIN_MAP_H_

class pack;
#include "widgets/sdl_widget.h"

struct map_cache_data
{
	int unknown1;
	short unknown3;
	char blabla[14];	//the size is 0x14, or 20
};

struct unknown_struct
{
	char a;
	char b;
	
};

struct unknown4_struct
{
	short num_bla1;
	unknown_struct * bla1;
};

struct lin_map_data
{
	int unknown1[64][128];	//loaded into mapCache[mapNum?] (offset 0)
	short num_unknown2;
	char *unknown2[6];	//char unknown2[num_unknown2][6]
	short unknown3[64][128];	//loaded into mapCache[mapNum?] (offset 4)
	int num_unknown4;
	unknown4_struct *unknown4;	//unknown4 stored into hideObjs[arg]
	//7cc8c for loading of unknown4 (contains three more elements than num_unknown4?)
};

//TODO find highest number of existing tile data ?
//TODO place to store all the tile data (array)
	//load / (unload) tile sections as necessary
	
//TODO method for getting tile data for a map coordinate
	//loading a map section if necessary

struct lin_map_segment
{	//not in file order
	int tile_data[64][128];
};

class lin_map
{
	public:
		lin_map();
		~lin_map();
		void load(int which);
		sdl_graphic *get_map(int x, int y);	//returns an image of a portion of the map
		int get_amnt();
	private:
		lin_map_data *mapdata;	//all the necessary map data
		
		char *filebuf;
};

#endif