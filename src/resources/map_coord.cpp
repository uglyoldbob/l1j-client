#include "map_coord.h"
#include "pixel_coord.h"
#include "screen_coord.h"

map_coord::map_coord(int tx, int ty)
	: coord(tx, ty)
{
}

map_coord::~map_coord()
{
}

pixel_coord map_coord::get_pixel()
{
	pixel_coord ret(x * 24, y * 24);
	return ret;
}

map_coord map_coord::get_map()
{
	return *this;
}

screen_coord map_coord::get_screen()
{
	int tx, ty;
	tx = x * 24;
	ty = y * 24;
	screen_coord ret((tx-ty) / 2, (tx+ty));
	return ret;
}
