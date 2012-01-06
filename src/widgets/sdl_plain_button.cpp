#include "resources/sdl_graphic.h"
#include "sdl_plain_button.h"

sdl_plain_button::sdl_plain_button(int num, int x, int y, graphics_data *packs, funcptr *stuff)
	: sdl_button(num, x, y, packs, stuff)
{
	if (num != -1)
	{
		two = new sdl_graphic(num+1, x, y, packs, GRAPH_IMG);
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


sdl_plain_button::~sdl_plain_button()
{
	delete two;

}
