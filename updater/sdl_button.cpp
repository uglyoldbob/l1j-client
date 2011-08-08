
#include "sdl_button.h"

sdl_button::sdl_button(int num, int x, int y, graphics_data *packs)
	: sdl_widget(num, x, y, packs)
{
	two = make_sdl_graphic(num+1, x, y, packs);
}

void sdl_button::draw(SDL_Surface *display)
{
	if (have_mouse)
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