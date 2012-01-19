#include <stdio.h>

#include "coord.h"

coord::coord(int tx, int ty)
{
	x = tx;
	y = ty;
}

int coord::get_x()
{
	return x;
}

int coord::get_y()
{
	return y;
}
coord::~coord()
{
}

void coord::print()
{
	printf("%d, %d", x, y);
}