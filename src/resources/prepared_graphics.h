#ifndef __PREPARED_GRAPHICS_H_
#define __PREPARED_GRAPHICS_H_

#include <SDL.h>

struct prepared_graphic
{
	SDL_Surface *surf;
	bool cleanup;	//does pixel data have to be manually cleaned up?
	SDL_Rect *position;
	SDL_Rect *mask;	//what portion of the surface to display
};

class prepared_graphics
{
	public:
		prepared_graphics();
		~prepared_graphics();
		
		void draw(SDL_Surface *display);

		bool ok();

		prepared_graphic *pg;
		int num_pg;
		bool ready;
};

#endif