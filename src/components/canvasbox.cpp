#include "canvasbox.h"

template <>
uint32_t HashMap<Thing, SDL_Surface>::hash_function(const Thing& t)
{
	return t.id();
}

CanvasBox::CanvasBox() :
	displays(3)
{
}

void CanvasBox::replace(Thing t, SDL_Surface s)
{
	displays.replace(t, s);
}
