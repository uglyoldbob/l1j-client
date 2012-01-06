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
			if (two != 0)
			{
				two->draw(display);
			}
		}
		else
		{
			if (one != 0)
			{
				one->draw(display);
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
