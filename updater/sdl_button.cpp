#include "sdl_button.h"

sdl_button::sdl_button(int num, int x, int y, graphics_data *packs, void (*fnctn)(void*), void *ag)
	: sdl_widget(num, x, y, packs)
{
	two = make_sdl_graphic(num+1, x, y, packs);
	click_ptr = fnctn;
	arg = ag;
}

sdl_button::~sdl_button()
{
}

void sdl_button::draw(SDL_Surface *display)
{
	if (visible == 1)
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
}

void sdl_button::mouse_click(SDL_MouseButtonEvent *here)
{
	static bool pressed = false;
	if ((pressed) && (here->type == SDL_MOUSEBUTTONUP))
	{
		if (click_ptr != 0)
		{
			(*click_ptr)(arg);
		}
	}
	else if (here->type == SDL_MOUSEBUTTONDOWN)
	{
		pressed = true;
	}
}