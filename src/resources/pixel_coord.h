#ifndef __PIXEL_COORD_H_
#define __PIXEL_COORD_H_

#include "coord.h"
class map_coord;
class screen_coord;

class pixel_coord : public coord
{
	public:
		pixel_coord(int tx, int ty);
		~pixel_coord();
		
		pixel_coord get_pixel();
		map_coord get_map();
		screen_coord get_screen();
};

#endif