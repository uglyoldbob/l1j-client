#ifndef __CANVASBOX_H__
#define __CANVASBOX_H__

#include "ecs/thing.h"
#include "ecs/hashmap.h"
#include <SDL/SDL.h>

/*! \brief Component manager for canvas.
*/
class CanvasBox
{
public:
	CanvasBox();
	void replace(Thing t, SDL_Surface s);
private:
	HashMap<Thing, SDL_Surface> displays;
};

#endif