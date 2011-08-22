#include "resources/prepared_graphics.h"
#include "sdl_drawmode.h"
#include "widgets/sdl_widget.h"

sdl_drawmode::sdl_drawmode(graphics_data *stuff, sdl_user *self)
{
	graphx = stuff;
	owner = self;
}

sdl_drawmode::~sdl_drawmode()
{
}

int sdl_drawmode::get_widget(int x, int y)
{
	//priority among who gets mouse when there is overlap? 
		//implement a tracking variable and update it using rules
	for (int i = 0; i < num_widgets; i++)
	{
		if (widgets[i]->contains(x,y))
		{
			return i;
		}
	}
	return -1;	//no widget contains that
}

void sdl_drawmode::draw(SDL_Surface *display)
{
	if (pg != 0)
		pg->draw(display);
	for (int i = 0; i < num_widgets; i++)
	{
		if (widgets[i] != 0)
			widgets[i]->draw(display);
	}
}

void sdl_drawmode::key_press(SDL_KeyboardEvent *button)
{
	if (num_widgets > 0)
	{
		if (button->type == SDL_KEYDOWN)
		{
			switch(button->keysym.sym)
			{
				case SDLK_TAB:
					//TODO: possibly change focus to a different widget
					widgets[widget_key_focus]->cursor_off();
					if (++widget_key_focus == num_widgets)
					{
						widget_key_focus = 0;
					}
					widgets[widget_key_focus]->cursor_on();
					break;
				case SDLK_F1: case SDLK_F2: case SDLK_F3: case SDLK_F4:
				case SDLK_F5: case SDLK_F6: case SDLK_F7: case SDLK_F8:
				case SDLK_F9: case SDLK_F10: case SDLK_F11: case SDLK_F12:
				case SDLK_F13: case SDLK_F14: case SDLK_F15:
					break;
				default:
					widgets[widget_key_focus]->key_press(button);
					break;
			}
		}
	}
}
