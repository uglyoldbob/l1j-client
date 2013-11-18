#include "draw_admin_main.h"

#include "client.h"
#include "globals.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_text_button.h"

quit_ptr::quit_ptr(draw_admin_main *bla)
{
	ref = bla;
}

void quit_ptr::go()
{
	ref->quit();
}

draw_admin_main::draw_admin_main(sdl_user *self)
	: sdl_drawmode(self)
{
	owner->game_music.change_music("sound/music1.mp3");
	quitting = false;
	
	int index;
	SDL_Rect *rect;

	num_gfx = 1;
	gfx = new sdl_graphic*[num_gfx];
	gfx[0] = new sdl_graphic();
	gfx[0]->delay_load(811, 0, 0, GRAPH_PNG, owner);
//	gfx[0]->disable_clear();
	
	num_widgets = 6;
	widgets = new sdl_widget*[num_widgets];
	
	int x = 200;
	int y = 200;
	widgets[0] = new sdl_text_button("IMG Files", x, y, owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_MAINT_IMG));
	widgets[0]->set_key_focus(true);
	widgets[0]->cursor_on();
	widgets[1] = new sdl_text_button("PNG Files", x, y+(15*1), owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_MAINT_PNG));
	widgets[1]->set_key_focus(true);
	widgets[2] = new sdl_text_button("TIL Files", x, y+(15*2), owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_MAINT_TIL));
	widgets[2]->set_key_focus(true);
	widgets[3] = new sdl_text_button("MAPS", x, y+(15*3), owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_MAINT_MAP));
	widgets[3]->set_key_focus(true);
	widgets[4] = new sdl_text_button("SPRITES", x, y+(15*4), owner, 
		(funcptr*)new dam_ptr(owner, DRAWMODE_MAINT_SPRITES));
	widgets[4]->set_key_focus(true);
	widgets[5] = new sdl_text_button("Exit", x, y+(15*5), owner, (funcptr*)new quit_ptr(this));
	widgets[5]->set_key_focus(true);
}

draw_admin_main::~draw_admin_main()
{
}

void draw_admin_main::quit()
{
	//owner->stop_thread = true;
	//owner->quit_client();
}

bool draw_admin_main::quit_request()
{
	return true;
}

void draw_admin_main::draw(SDL_Surface *display)
{
	sdl_drawmode::draw(display);
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
