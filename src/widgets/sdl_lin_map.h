#ifndef __SDL_LIN_MAP_H_
#define __SDL_LIN_MAP_H_

class pack;
class tile;
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
	unsigned int floor[64][128];	//the floor data for each tile of the map (each tile is a triangle)
	short num_unknown2;
	//comment placeholder for the unknown2 data
	short attr[64][128];	//loaded into mapCache[mapNum?] (offset 4)
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
	sdl_graphic *graphic;
	lin_map_data *mapdata;	//all the necessary map data
	int map;
	//the lowest x,y coordinates of the map section
	int x;
	int y;
	//the offset values for the map section
	int offsetx;
	int offsety;
	//the lowest x,y corresponds to the image coordinates of (0, 756)
};

class sdl_lin_map : public sdl_widget
{
	public:
		sdl_lin_map(tile *thetiles, client *who, int x, int y, int w, int h);
		~sdl_lin_map();
		void draw(SDL_Surface *display);	//draws the full map onto display
		void set_hotspot(int mapn, int x, int y);	//sets the coordinates for the hotspot (the middle of the screen)
	private:
		tile *tile_data;	//used for reference only
		lin_map_segment segs[4];
		int map;
		client *who;
		
		//the master set of offsets for the x and y coordinates of translated coordinates
		int master_offsetx;
		int master_offsety;
		
		lin_map_segment get_map(int mapnum, int x, int y);	//returns a map section
		void check_sections();	//check the loaded sections
};

#endif

//the s32 file

//64 x 128 of int (4 bytes) data [called floor]
//a short (mys1)
	//array of structures 6 bytes in size, there are mys1 of them
//64 x 128 of short (2 bytes) data [called attr]
//int mys2
	//skip 2 bytes
	//short mys2_1
		//???
//int mys3
	//
	
	
//set a certain point on the screen to a set of map coordinates

//pick the corners of the visible map drawing area
//get the mapp coordinates for those points
//determine what map segments need to be drawn, and where to be drawn at


//break 0x7cbcc disp /s $r3
//break 0x7cc28 disp $r0
//break 0x7cc80 if ($r23 > 0)

//4/7ffd8003
	//1196
	//181
//4/7ffc8003
	//97
	//34
//4/7ffd8002
	//1298
	//487
//4/7ffc8002
	//237
	//82