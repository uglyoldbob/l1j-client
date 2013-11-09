#include "draw_login.h"

#include "client.h"
#include "globals.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_input_box.h"
#include "widgets/text_box.h"

login_ptr::login_ptr(draw_login *bla)
{
	ref = bla;
}

void login_ptr::go()
{
	ref->login();
}

quit_ptr::quit_ptr(draw_login *bla)
{
	ref = bla;
}

void quit_ptr::go()
{
	ref->quit();
}

draw_login::draw_login(sdl_user *self)
	: sdl_drawmode(self)
{	
	//273 - 300 is the lantern animation (img)
	owner->game_music.change_music("sound/music0.mp3");
	
	client_request t_sdl;
	t_sdl.request = CLIENT_REQUEST_LOAD_SDL_GRAPHIC;
	t_sdl.data.rload.name = 0;
	t_sdl.data.rload.num = 814;
	t_sdl.data.rload.x = 0;
	t_sdl.data.rload.y = 0;
	t_sdl.data.rload.type = GRAPH_PNG;
	t_sdl.data.rload.load_type = CLIENT_REQUEST_LOAD_4;
	
	num_gfx = 1;
	gfx = new sdl_graphic*[num_gfx];
	gfx[0] = new sdl_graphic();
	t_sdl.data.rload.item = gfx[0];
	self->add_request(t_sdl);
	gfx[0]->disable_clear();
	
	num_widgets = 8;
	widgets = new sdl_widget*[num_widgets];
	
	widgets[0] = new sdl_widget(59, 0x1a9, 0x138, owner);
	widgets[1] = new sdl_input_box(12, 0x1fb, 0x14a, false, owner);
	
	widgets[1]->set_key_focus(true);
	widgets[1]->cursor_on();
	widget_key_focus = 1;
	widgets[2] = new sdl_input_box(13, 0x1fb, 0x160, true, owner);
	widgets[2]->set_key_focus(true);
	
	//set max length for username and password
	((sdl_input_box*)widgets[1])->set_max(20);
	((sdl_input_box*)widgets[2])->set_max(20);
	
	widgets[3] = new sdl_plain_button(53, 0x213, 0x183, owner, (funcptr*)new login_ptr(this));
	widgets[3]->set_key_focus(true);
	widgets[4] = new sdl_plain_button(65, 0x213, 0x195, owner, 0);
	widgets[4]->set_key_focus(true);
	widgets[5] = new sdl_plain_button(55, 0x213, 0x1a8, owner, 0);
	widgets[5]->set_key_focus(true);
	widgets[6] = new sdl_plain_button(57, 0x213, 0x1c2, owner, (funcptr*)new quit_ptr(this));
	widgets[6]->set_key_focus(true);
	
	intro.load("intro-e.tbl", textpack);
	widgets[7] = new text_box(34, 59, 6*57, intro.get_num_entries()*12, owner);
	text_box *temp;
	temp = (text_box*)(widgets[7]);
	for (int i = 0; i < intro.get_num_entries(); i++)
	{
		char temp_bob[256];
		if (intro[i] != 0)
		{
			sprintf(temp_bob, "%s", intro[i]);
		}
		else
		{
			sprintf(temp_bob, " ");
		}
		temp->add_line(temp_bob);
	}
	temp->set_visible(0, 18*12);
	
//	widgets[7] = new sdl_widget(814, 0x1a, 0x3b, graphx);
		//type 1, null("intro"), px=0xcf, py=0x11a
}

draw_login::~draw_login()
{
}

bool draw_login::quit_request()
{
	//owner->stop_thread = true;
	return true;
}


bool draw_login::mouse_leave()
{
	return false;
}

void draw_login::key_press(SDL_KeyboardEvent *button)
{
	if (button->type == SDL_KEYDOWN)
	{
		if (widget_key_focus == 2)
		{
			if ((button->keysym.sym == SDLK_RETURN) ||
				(button->keysym.sym == SDLK_KP_ENTER))
			{
				widgets[3]->key_press(button);
			}
		}
	}
	sdl_drawmode::key_press(button);
}

void draw_login::login()
{
	sdl_input_box *name;
	sdl_input_box *pass;
	name = (sdl_input_box*) widgets[1];
	pass = (sdl_input_box*) widgets[2];
	owner->login(name->get_str(), pass->get_str());
	//send login packet with username and password
	//clear username and password information
}

void draw_login::quit()
{
	//owner->stop_thread = true;
	//owner->quit_client();
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
