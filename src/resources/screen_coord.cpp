#include "screen_coord.h"
#include "map_coord.h"
#include "pixel_coord.h"

screen_coord::screen_coord(int tx, int ty)
	: coord(tx, ty)
{
}

screen_coord::~screen_coord()
{
}

pixel_coord screen_coord::get_pixel()
{
	pixel_coord ret((y/2 + x), (y/2 - x));
	return ret;
}

map_coord screen_coord::get_map()
{
	int tx, ty;
	tx = y/2 + x;
	ty = y/2 - x;
	map_coord ret(tx/24, ty/24);
	return ret;
}

screen_coord screen_coord::get_screen()
{
	return *this;
}