#include "sdl_button.h"

sdl_button::sdl_button(int num, int x, int y, sdl_user *who, funcptr *stuff)
	: sdl_widget(num, x, y, who)
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

void sdl_button::key_press(SDL_KeyboardEvent *button)
{
	if (button->type == SDL_KEYDOWN)
	{
		switch(button->keysym.sym)
		{
			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				if (method != 0)
				{
					method->go();
				}
				break;
			default:
				break;
		}
	}	
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