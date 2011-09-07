#include "draw_maint_map.h"
#include "globals.h"
#include "resources/lin_map.h"
#include "resources/prepared_graphics.h"
#include "resources/tile.h"
#include "sdl_user.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sdl_widget.h"

draw_maint_map::draw_maint_map(graphics_data *stuff, sdl_user *self)
	: sdl_drawmode(stuff, self)
{
	draw_mtx = SDL_CreateMutex();
	owner->game_music.change_music("sound/music0.mp3");
	pg = 0;
	
	num_widgets = 1;
	
	widgets = new sdl_widget*[num_widgets];
	widgets[0] = new sdl_text_button("Return", 320, 255, graphx, change_mode, owner, (void*)DRAWMODE_ADMIN_MAIN);
	widgets[0]->set_key_focus(true);
	
	themap = new lin_map;
	themap->load(3);
}

void draw_maint_map::key_press(SDL_KeyboardEvent *button)
{
	sdl_drawmode::key_press(button);
	if (button->type == SDL_KEYDOWN)
	{
		switch(button->keysym.sym)
		{
			case SDLK_LEFT:
				break;
			case SDLK_RIGHT:
				break;
			case SDLK_UP:
				break;
			case SDLK_DOWN:
				break;
			default:
				break;
		}
	}
}

bool draw_maint_map::mouse_leave()
{
	return false;
}

void draw_maint_map::mouse_click(SDL_MouseButtonEvent *here)
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

void draw_maint_map::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void draw_maint_map::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void draw_maint_map::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
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

draw_maint_map::~draw_maint_map()
{
}

void draw_maint_map::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	SDL_FillRect(display, NULL, 0);
	sdl_drawmode::draw(display);
	lineage_font.draw(display, 320, 240, "Loading map 3", 0xFFFE);
	if (0)//if (themap != 0)
	{
		sdl_graphic *temp = themap->get_map();
		if (temp != 0)
		{
			if (temp->img != 0)
			{
				SDL_BlitSurface(temp->img, NULL, display, NULL);
			}
		}
	}
	SDL_mutexV(draw_mtx);
}