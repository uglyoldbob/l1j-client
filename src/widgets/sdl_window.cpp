#include "sdl_window.h"

sdl_window::sdl_window(int num, int x, int y, int w, int h, client *who)
	: sdl_widget(num, 0, 0, who)
{
	num_widgets = 0;
	widgets = 0;
	
	window = new sdl_graphic(x, y, w, h);
}

sdl_window::~sdl_window()
{
	if (widgets != 0)
	{
		for (int i = 0; i < num_widgets; i++)
		{
			delete widgets[i];
		}
		delete [] widgets;
	}
}

void sdl_window::draw(SDL_Surface *display)
{
	if (visible)
	{
		sdl_widget::draw(window->get_surf());
		for (int i = 0; i < num_widgets; i++)
		{
			if (widgets[i] != 0)
				widgets[i]->draw(window->get_surf());
		}
		window->draw(display);
	}
}