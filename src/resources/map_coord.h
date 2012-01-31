#ifndef __MAP_COORD_H_
#define __MAP_COORD_H_

class pixel_coord;
class screen_coord;
#include "coord.h"

class map_coord : public coord
{
	public:
		map_coord(int tx, int ty);
		~map_coord();
		
		pixel_coord get_pixel();
		map_coord get_map();
		screen_coord get_screen();
};

#endif