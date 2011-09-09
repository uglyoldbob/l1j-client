#include "sdl_plain_button.h"

sdl_plain_button::sdl_plain_button(int num, int x, int y, graphics_data *packs, funcptr *stuff)
	: sdl_button(num, x, y, packs, stuff)
{
	if (num != -1)
	{
		two = make_sdl_graphic(num+1, x, y, packs);
	}
	else
	{
		two = 0;
	}
}

void sdl_plain_button::draw(SDL_Surface *display)
{
	if (visible == 1)
	{
		if (have_mouse || key_focus)
		{
			if (two != 0)
			{
				if (two->img != 0)
				{
					SDL_BlitSurface(two->img, two->mask, display, two->pos);
				}
			}
		}
		else
		{
			if (one != 0)
			{
				if (one->img != 0)
				{
					SDL_BlitSurface(one->img, one->mask, display, one->pos);
				}
			}
		}
	}
}


sdl_plain_button::~sdl_plain_button()
{
	if (two != 0)
	{
		if (two->cleanup)
			delete [] (short*)two->img->pixels;
		delete two->pos;
		delete two->mask;
		SDL_FreeSurface(two->img);
	}

}
