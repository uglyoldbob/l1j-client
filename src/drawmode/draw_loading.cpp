#include "draw_loading.h"

#include "client.h"
#include "globals.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_text_button.h"
#include "widgets/sprite.h"
#include "widgets/text_box.h"

load_ptr::load_ptr(draw_loading *bla, int val)
{
	ref = bla;
	which = val;
}

void load_ptr::go()
{
	ref->server_picked(which);
}

draw_loading::draw_loading(sdl_user *self)
	: sdl_drawmode(self)
{
	owner->game_music.change_music("sound/music0.mp3");
	load_progress = 0;
	load_amount = 0x249f0;	//not sure how this number is generated
		
	int index;
	SDL_Rect *rect;

	client_request t_sdl;
	t_sdl.request = CLIENT_REQUEST_LOAD_SDL_GRAPHIC;
	t_sdl.data.rload.name = 0;
	t_sdl.data.rload.num = 811;
	t_sdl.data.rload.x = 0;
	t_sdl.data.rload.y = 0;
	t_sdl.data.rload.type = GRAPH_PNG;
	t_sdl.data.rload.load_type = CLIENT_REQUEST_LOAD_4;
	
	num_gfx = 2;
	gfx = new sdl_graphic*[num_gfx];
	gfx[0] = new sdl_graphic();
	t_sdl.data.rload.item = gfx[0];
	self->add_request(t_sdl);
	
	t_sdl.data.rload.num = 330;
	t_sdl.data.rload.x = 241;
	t_sdl.data.rload.y = 385;
	t_sdl.data.rload.type = GRAPH_IMG;
	gfx[1] = new sdl_graphic();
	t_sdl.data.rload.item = gfx[1];
	self->add_request(t_sdl);
	
	num_servers = owner->get_config()->get_num_servers();
	
	num_widgets = num_servers + 2;
	widgets = new sdl_widget*[num_widgets];
	
	server_pick = -1;
	spick_mtx = SDL_CreateMutex();
	
	widget_key_focus = 0;
	for (int i = 0; i < num_servers; i++)
	{
		widgets[i] = new sdl_text_button(owner->get_config()->get_name(i), 276, 254+(15*(i+1)), owner, 
			(funcptr*)new load_ptr(this, i));
		widgets[i]->set_key_focus(true);
	}
	widgets[num_servers] = new text_box(257, 254, 150, 10*12, owner);
	
	widgets[num_servers+1] = new sprite(320, 200, "2786-8.spr", owner);
	//widgets[num_servers+1] = new sprite(50, 50, "6258-0.spr", owner);
		//6256-173 nothing?
		//6256-181 nothing?
		//		
	
	((text_box*)widgets[num_servers])->add_line("Please select a server:");
		
	update_load();
}

int draw_loading::get_server_pick()
{
	return server_pick;
}

bool draw_loading::quit_request()
{
	return true;
}

void draw_loading::add_text(char *bla)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	((text_box*)widgets[num_servers])->add_line(bla);
	SDL_mutexV(draw_mtx);
}

void draw_loading::server_picked(int i)
{
	while (SDL_mutexP(spick_mtx) == -1) {};
	//hide buttons, add text to the textbox
	for (int ij = 0; ij < num_servers; ij++)
		widgets[ij]->hide(false);
	((text_box*)widgets[num_servers])->add_line("Checking for updates");
	server_pick = i;
	SDL_mutexV(spick_mtx);
}

void draw_loading::set_load_amount(int size)
{
	load_amount = size;
}

void draw_loading::update_load()
{
	float temp = (float)gfx[1]->getw() * ((float)load_progress / (float)load_amount);
	gfx[1]->setmw(temp);
}

void draw_loading::add_loaded(int size)
{
	load_progress += size;
	update_load();
}

void draw_loading::load_done()
{
	load_progress = load_amount;
	update_load();
	SDL_Delay(250);
	owner->change_drawmode(DRAWMODE_LOGIN);
}

draw_loading::~draw_loading()
{
	SDL_DestroyMutex(spick_mtx);
}

bool draw_loading::mouse_leave()
{
	return false;
}

void draw_loading::mouse_click(SDL_MouseButtonEvent *here)
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

void draw_loading::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void draw_loading::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void draw_loading::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
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
