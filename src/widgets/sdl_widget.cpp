#include <SDL.h>

#include "globals.h"
#include "sdl_widget.h"

sdl_graphic *make_sdl_graphic(int num, int x, int y, graphics_data *packs)
{
	sdl_graphic *ret;
	if (num != -1)
	{
		ret = new sdl_graphic;
		ret->img = get_img(num, packs->spritepack);
		if (ret->img != 0)
		{
			ret->pos = make_sdl_rect(x, y, ret->img->w, ret->img->h);
			ret->mask = make_sdl_rect(0, 0, ret->img->w, ret->img->h);
			ret->cleanup = true;
		}
		else
		{
			ret->pos = 0;
			ret->mask = 0;
			ret->cleanup = false;
		}
	}
	else
	{
		ret = 0;
	}
	return ret;
}

SDL_Rect *make_sdl_rect(int x, int y, int w, int h)
{
	SDL_Rect *ret;
	ret = new SDL_Rect;
	ret->x = x;
	ret->y = y;
	ret->w = w;
	ret->h = h;
	return ret;
}

sdl_widget::sdl_widget(int num, int x, int y, graphics_data *packs)
{
	visible = false;
	key_focus = false;
	allow_key_focus = false;
	
	one = make_sdl_graphic(num, x, y, packs);
	if (one != 0)
	{
		visible = true;
	}
	have_mouse = false;
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
	if ((one != 0) && (one->pos != 0))
	{
		int x_check1 = one->pos->x;
		int y_check1 = one->pos->y;
	
		int x_check2 = one->pos->x + one->pos->w;
		int y_check2 = one->pos->y + one->pos->h;

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
		if (one->cleanup)
			delete [] (short*)one->img->pixels;
		delete one->pos;
		delete one->mask;
		SDL_FreeSurface(one->img);
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
			if (one->img != 0)
			{
				SDL_BlitSurface(one->img, one->mask, display, one->pos);
			}
		}
	}
}

