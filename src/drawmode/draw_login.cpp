#include "draw_login.h"

#include "globals.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_input_box.h"

void login_function(void *arg)
{
	draw_login *bob = (draw_login*)arg;
	bob->owner->login();
	//send login packet with username and password
	//clear username and password information
}

void quit_the_client(void *arg)
{
	draw_login *bob = (draw_login*)arg;
	bob->owner->quit_client();
}


draw_login::draw_login(graphics_data *stuff, sdl_user *self)
	: sdl_drawmode(stuff, self)
{	
	owner->game_music.change_music("sound/music0.mp3");
	pg = new prepared_graphics;
	pg->num_pg = 1;
	pg->pg = new prepared_graphic[1];
	
	pg->pg[0].surf = get_png_image(814, graphx->spritepack);
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = false;
	pg->ready = true;
	
	num_widgets = 7;
	widgets = new sdl_widget*[num_widgets];
	
	widgets[0] = new sdl_widget(59, 0x1a9, 0x138, graphx);
	widgets[1] = new sdl_input_box(12, 0x1fb, 0x14a, graphx);
	widgets[1]->cursor_on();
	widget_key_focus = 1;
	widgets[2] = new sdl_input_box(13, 0x1fb, 0x160, graphx);
	widgets[3] = new sdl_plain_button(53, 0x213, 0x183, graphx, &login_function, this);
	widgets[4] = new sdl_plain_button(65, 0x213, 0x195, graphx, 0, 0);
	widgets[5] = new sdl_plain_button(55, 0x213, 0x1a8, graphx, 0, 0);
	widgets[6] = new sdl_plain_button(57, 0x213, 0x1c2, graphx, &quit_the_client, this);
//	widgets[7] = new sdl_widget(814, 0x1a, 0x3b, graphx);
		//type 1, null("intro"), px=0xcf, py=0x11a
}

draw_login::~draw_login()
{
	if (pg != 0)
		delete pg;
	if (widgets != 0)
		delete [] widgets;
}

bool draw_login::mouse_leave()
{
	return false;
}

void draw_login::mouse_click(SDL_MouseButtonEvent *here)
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

void draw_login::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void draw_login::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void draw_login::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
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
