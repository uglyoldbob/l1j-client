#include <math.h>

#include "map_coord.h"
#include "pixel_coord.h"
#include "screen_coord.h"

pixel_coord::pixel_coord(int tx, int ty)
	: coord(tx, ty)
{
}

pixel_coord::~pixel_coord()
{
}

pixel_coord pixel_coord::get_pixel()
{
	return *this;
}

map_coord pixel_coord::get_map()
{
	map_coord temp(x / 24, y / 24);
	return temp;
}

screen_coord pixel_coord::get_screen()
{	
	screen_coord ret((x-y) / 2, (x+y));
	return ret;
}
