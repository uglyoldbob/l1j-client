#include "resources/prepared_graphics.h"
#include "sdl_drawmode.h"
#include "sdl_user.h"
#include "widgets/sdl_widget.h"

dam_ptr::dam_ptr(sdl_user *bla, enum drawmode val)
{
	owner = bla;
	which = val;
}

void dam_ptr::go()
{
	owner->change_drawmode(which);
}

sdl_drawmode::sdl_drawmode(sdl_user *self)
{
	draw_mtx = SDL_CreateMutex();
	draw_scene = true;
	owner = self;
	widget_key_focus = 0;
	gfx = 0;
	num_gfx = 0;
	num_widgets = 0;
	widgets = 0;
}

bool sdl_drawmode::quit_request()
{
	return false;
}

sdl_widget *sdl_drawmode::get_widget(int i)
{
	if ((i >= 0) && (i < num_widgets))
	{
		return widgets[i];
	}
	else
	{
		return 0;
	}
}

sdl_drawmode::~sdl_drawmode()
{
	SDL_DestroyMutex(draw_mtx);
	if (num_gfx > 0)
	{
		for (int i = 0; i < num_gfx; i++)
		{
			if (gfx[i] != 0)
			{
				delete gfx[i];
				gfx[i] = 0;
			}
		}
		delete [] gfx;
		gfx = 0;
	}
	if (widgets != 0)
	{
		for (int i = 0; i < num_widgets; i++)
		{
			delete widgets[i];
			widgets[i] = 0;
		}
		delete [] widgets;
		widgets = 0;
	}
}

int sdl_drawmode::get_widget(int x, int y)
{
	//priority among who gets mouse when there is overlap? 
		//implement a tracking variable and update it using rules
	for (int i = num_widgets - 1; i >= 0; i--)
	{
		if (widgets[i] != 0)
		{
			if ((widgets[i]->contains(x,y)) && (widgets[i]->is_visible()) )
			{
				return i;
			}
		}
	}
	return -1;	//no widget contains that
}

void sdl_drawmode::mouse_click(SDL_MouseButtonEvent *here)
{
	if (num_widgets > 0)
	{
		int index = get_widget(here->x, here->y);
		if (index != -1)
		{
			widgets[index]->mouse_click(here);
		}
	}
}

void sdl_drawmode::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void sdl_drawmode::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void sdl_drawmode::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
{	//TODO: handle click and drag movable widgets
	if (num_widgets > 0)
	{
		int from_w = get_widget(from->x, from->y);
		int to_w = get_widget(to->x, to->y);
		if ((from_w != -1) && (to_w != -1))
		{
			if (from_w != to_w)
			{	//only send events if the widgets are different
				mouse_from(from);
				mouse_to(to);
			}
			else
			{
				mouse_to(to);
			}
		}
		if ((from_w == -1) && (to_w != -1))
		{
			mouse_to(to);
		}
		if ((from_w != -1) && (to_w == -1))
		{
			mouse_from(from);
		}
	}
}


void sdl_drawmode::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (draw_scene)
	{
		for (int i = 0; i < num_gfx; i++)
		{
			if (gfx[i] != 0)
				gfx[i]->draw(display);
		}
		for (int i = 0; i < num_widgets; i++)
		{
			if (widgets[i] != 0)
				widgets[i]->draw(display);
		}
	}
	SDL_mutexV(draw_mtx);
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
				{
					//TODO: possibly change focus to a different widget
					widgets[widget_key_focus]->cursor_off();
					bool cont = true;
					while (cont)
					{
						if (++widget_key_focus == num_widgets)
						{
							widget_key_focus = 0;
						}
						widgets[widget_key_focus]->cursor_on();
						cont = !(widgets[widget_key_focus]->check_key_focus());
					}
					break;
				}
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
