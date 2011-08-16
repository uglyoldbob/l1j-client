#include "sdl_button.h"

sdl_button::sdl_button(int num, int x, int y, graphics_data *packs, void (*fnctn)(void*), void *ag)
	: sdl_widget(num, x, y, packs)
{
	click_ptr = fnctn;
	arg = ag;
}

sdl_button::~sdl_button()
{
}

void sdl_button::cursor_on()
{
	key_focus = true;
}

void sdl_button::cursor_off()
{
	key_focus = false;
}


void sdl_button::mouse_click(SDL_MouseButtonEvent *here)
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
			if (click_ptr != 0)
			{
				(*click_ptr)(arg);
			}
		}
	}
}