#ifndef __SDL_LIN_MAP_H_
#define __SDL_LIN_MAP_H_

class pack;
class tile;
#include "widgets/sdl_widget.h"

struct map_tile_data
{
	char x;
	char y;
	char h;
	unsigned int tiledata;
};

struct map_object
{
	short num_tiles;
	map_tile_data *tiles;
};

struct lin_map_data
{
	unsigned int floor[64][128];	//the floor data for each tile of the map (each tile is a triangle)
	short num_unknown2;
	//comment placeholder for the unknown2 data
	short attr[64][128];	//attributes for all tiles
	int num_objects;
	map_object *objs;
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
		sdl_lin_map(sdl_user *who, int x, int y, int w, int h);
		~sdl_lin_map();
		void draw(SDL_Surface *display);	//draws the full map onto display
		void set_hotspot(int mapn, int x, int y);	//sets the coordinates for the hotspot (the middle of the screen)
		void draw_info(SDL_Surface *display, int x, int y);
		tile *tile_data;	//used for reference only
		int get_offset_x();	/**< Returns the calculated offset for this map */
		int get_offset_y();	/**< Returns the calculated offset for this map */
		void check_sections(client *from);	/**< Check the loaded sections whenever the map gets moved */
		void draw_cursor(int x, int y, SDL_Surface *display);
		void mouse_to(SDL_MouseMotionEvent *to);
		void mouse_from(SDL_MouseMotionEvent *to);
		void mouse_click(SDL_MouseButtonEvent *here);
	private:
		SDL_mutex *edit_mtx;
		lin_map_segment segs[4];
		int map;
		
		//the master set of offsets for the x and y coordinates of translated coordinates
		int master_offsetx;
		int master_offsety;
		
		int cursor_x, cursor_y;
		bool cursor_valid;
		
		lin_map_segment get_map(int mapnum, int x, int y, client *from);	//returns a map section
		void handle_s32(SDL_RWops *sdl_buf, lin_map_segment *ret, int mapnum, int x, int y, client *from);
		void handle_seg(SDL_RWops *sdl_buf, lin_map_segment *ret, int mapnum, int x, int y, client *from);
		void delete_segment(lin_map_segment delme);
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