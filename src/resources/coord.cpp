#include <stdio.h>

#include "coord.h"

coord::coord(int tx, int ty)
{
	x = tx;
	y = ty;
}

coord::~coord()
{
}

void coord::print()
{
	printf("Coordinate: %d, %d", x, y);
}