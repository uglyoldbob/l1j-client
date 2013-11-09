#include "client.h"
#include "sdl_radio_button.h"

sdl_radio_button::sdl_radio_button(int num, int x, int y, sdl_user *who, funcptr *stuff,
	sdl_widget**bob, int amnt, int indx)
	: sdl_check_button(num, x, y, who, stuff)
{
	checked = 0; 
	options = bob;
	num_options = amnt;
	me = indx;
}

void sdl_radio_button::set()
{
	checked = true;
	if (options != 0)
	{
		for (int i = 0; i < num_options; i++)
		{
			if (i != me)
			{
				sdl_radio_button* jim;
				jim = (sdl_radio_button*)options[i];
				jim->checked = false;
			}
		}
	}
}

void sdl_radio_button::mouse_click(SDL_MouseButtonEvent *here)
{
	sdl_check_button::mouse_click(here);
	if (checked)
	{
		if (options != 0)
		{
			for (int i = 0; i < num_options; i++)
			{
				if (i != me)
				{
					sdl_radio_button* jim;
					jim = (sdl_radio_button*)options[i];
					jim->checked = false;
				}
			}
		}
	}
}

sdl_radio_button::~sdl_radio_button()
{
}
