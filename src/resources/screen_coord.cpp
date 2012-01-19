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
	tx = x - 2*y;
	ty = x + 2*y;
	map_coord ret(tx/48, ty/48);
	return ret;
}

screen_coord screen_coord::get_screen()
{
	return *this;
}