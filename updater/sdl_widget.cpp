#include <SDL.h>

#include "sdl_widget.h"

sdl_widget::sdl_widget(SDL_Rect *ps, SDL_Rect *msk, SDL_Surface *data)
{
	img = data;
	pos = ps;
	mask = msk;
	visible = 1;
}

sdl_widget::~sdl_widget()
{
	delete [] (short*)img->pixels;
	delete pos;
	delete mask;
	SDL_FreeSurface(img);
}

void sdl_widget::draw(SDL_Surface *display)
{
	if (img != 0)
		SDL_BlitSurface(img, mask, display, pos);
}