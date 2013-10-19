#include "draw_admin_main.h"

#include "client.h"
#include "globals.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_text_button.h"

void change_mode(void *a, void* b)
{
	sdl_user *owner;
	owner = (sdl_user*)a;
	owner->change_drawmode(*(drawmode*)b);
}

draw_admin_main::draw_admin_main(sdl_user *self)
	: sdl_drawmode(self)
{
	owner->game_music.change_music("sound/music1.mp3");
	
	int index;
	SDL_Rect *rect;

	num_gfx = 1;
	gfx = new sdl_graphic*[num_gfx];
	gfx[0] = new sdl_graphic(811, 0, 0, owner->game, GRAPH_PNG);
	gfx[0]->disable_clear();
	
	num_widgets = 5;
	widgets = new sdl_widget*[num_widgets];
	
	int x = 200;
	int y = 200;
	widgets[0] = new sdl_text_button("IMG Files", x, y, owner->game, 
		(funcptr*)new dam_ptr(this, DRAWMODE_MAINT_IMG));
	widgets[0]->set_key_focus(true);
	widgets[0]->cursor_on();
	widgets[1] = new sdl_text_button("PNG Files", x, y+(15*1), owner->game, 
		(funcptr*)new dam_ptr(this, DRAWMODE_MAINT_PNG));
	widgets[1]->set_key_focus(true);
	widgets[2] = new sdl_text_button("TIL Files", x, y+(15*2), owner->game, 
		(funcptr*)new dam_ptr(this, DRAWMODE_MAINT_TIL));
	widgets[2]->set_key_focus(true);
	widgets[3] = new sdl_text_button("MAPS", x, y+(15*3), owner->game, 
		(funcptr*)new dam_ptr(this, DRAWMODE_MAINT_MAP));
	widgets[3]->set_key_focus(true);
	widgets[4] = new sdl_text_button("Exit", x, y+(15*4), owner->game, 0);
	widgets[4]->set_key_focus(true);
}

draw_admin_main::~draw_admin_main()
{
}

bool draw_admin_main::quit_request()
{
	owner->game->stop_thread = true;
	return true;
}

bool draw_admin_main::mouse_leave()
{
	return false;
}

void draw_admin_main::mouse_click(SDL_MouseButtonEvent *here)
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

void draw_admin_main::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void draw_admin_main::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void draw_admin_main::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
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
