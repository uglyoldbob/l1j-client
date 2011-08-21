#include "sdl_plain_button.h"

sdl_plain_button::sdl_plain_button(int num, int x, int y, graphics_data *packs, void (*fnctn)(void*), void *ag)
	: sdl_button(num, x, y, packs, fnctn, ag)
{
	two = make_sdl_graphic(num+1, x, y, packs);
}

void sdl_plain_button::draw(SDL_Surface *display)
{
	if (visible == 1)
	{
		if (have_mouse || key_focus)
		{
			if (two->img != 0)
			{
				SDL_BlitSurface(two->img, two->mask, display, two->pos);
			}
		}
		else
		{
			if (one->img != 0)
			{
				SDL_BlitSurface(one->img, one->mask, display, one->pos);
			}
		}
	}
}


sdl_plain_button::~sdl_plain_button()
{
}
