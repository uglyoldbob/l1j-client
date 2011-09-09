#include "sdl_check_button.h"

sdl_check_button::sdl_check_button(int num, int x, int y, graphics_data *packs, funcptr *stuff)
	: sdl_plain_button(num, x, y, packs, stuff)
{
	checked = 0;
}

void sdl_check_button::draw(SDL_Surface *display)
{
	if (visible == 1)
	{
		if (have_mouse || key_focus || checked)
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

void sdl_check_button::mouse_click(SDL_MouseButtonEvent *here)
{
	static bool pressed = false;
	
	if (here->button == 1)
	{
		if (here->type == SDL_MOUSEBUTTONDOWN)
		{
			pressed = true;
		}
		if ((pressed) && (here->type == SDL_MOUSEBUTTONUP))
		{
			checked = true;
			if (method != 0)
			{
				method->go();
			}
			pressed = false;
		}
	}
}

sdl_check_button::~sdl_check_button()
{
}
