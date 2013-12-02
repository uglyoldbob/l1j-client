#include "sdl_window.h"

sdl_window::sdl_window(int num, int x, int y, int w, int h, sdl_user *who)
	: sdl_widget(num, x, y, who)
{
	num_widgets = 0;
	widgets = 0;
	this->x = x;
	this->y = y;
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
		sdl_widget::draw(display);
		for (int i = 0; i < num_widgets; i++)
		{
			if (widgets[i] != 0)
				widgets[i]->drawat(x, y, display);
		}
	}
}