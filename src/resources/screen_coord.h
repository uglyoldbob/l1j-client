#ifndef __SCREEN_COORD_H_
#define __SCREEN_COORD_H_

class pixel_coord;
#include "coord.h"

class screen_coord : public coord
{
	public:
		screen_coord(int tx, int ty);
		~screen_coord();
		
		pixel_coord get_pixel();
		map_coord get_map();
		screen_coord get_screen();
};

#endif