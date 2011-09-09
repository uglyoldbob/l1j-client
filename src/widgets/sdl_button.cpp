#include "sdl_button.h"

sdl_button::sdl_button(int num, int x, int y, graphics_data *packs, funcptr *stuff)
	: sdl_widget(num, x, y, packs)
{
	method = stuff;
}

sdl_button::~sdl_button()
{
	delete method;
}

void sdl_button::cursor_on()
{
	if (allow_key_focus)
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
			if (method != 0)
			{
				method->go();
			}
			pressed = false;
		}
	}
}