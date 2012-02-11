#include <SDL/SDL.h>

#include "client.h"
#include "globals.h"
#include "sdl_widget.h"

sdl_widget::sdl_widget(int x, int y, client *who)
{
	visible = false;
	key_focus = false;
	allow_key_focus = false;
	
	one = 0;
	have_mouse = false;
	myclient = who;
}

sdl_widget::sdl_widget(int num, int x, int y, client *who)
{
	visible = false;
	key_focus = false;
	allow_key_focus = false;
	
	if (num != -1)
	{
		one = new sdl_graphic(num, x, y, who, GRAPH_IMG);
	}
	else
	{
		one = 0;
	}
	if (one != 0)
	{
		visible = true;
	}
	have_mouse = false;
	myclient = who;
}

void sdl_widget::set_key_focus(bool arg)
{
	allow_key_focus = arg;
}

bool sdl_widget::check_key_focus()
{
	return allow_key_focus;
}

void sdl_widget::mouse_to(SDL_MouseMotionEvent *to)
{
	have_mouse = true;
}

void sdl_widget::mouse_from(SDL_MouseMotionEvent *from)
{
	have_mouse = false;
}

void sdl_widget::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
{
}

void sdl_widget::mouse_click(SDL_MouseButtonEvent *here)
{
}

void sdl_widget::key_press(SDL_KeyboardEvent *button)
{
}

bool sdl_widget::contains(int x, int y)
{
	if (one != 0)
	{
		int x_check1 = one->getx();
		int y_check1 = one->gety();
	
		int x_check2 = one->getx() + one->getw();
		int y_check2 = one->gety() + one->geth();

		if ((x_check1 <= x) && (x <= x_check2))
		{
			if ((y_check1 <= y) && (y <= y_check2))
			{
				return true;
			}
		}
	}
	return false;
}

sdl_widget::~sdl_widget()
{
	if (one != 0)
	{
		delete one;
	}
}

void sdl_widget::cursor_on()
{
}

void sdl_widget::cursor_off()
{
}

void sdl_widget::draw(SDL_Surface *display)
{
	if (visible)
	{
		if (one != 0)
		{
			one->draw(display);
		}
	}
}

