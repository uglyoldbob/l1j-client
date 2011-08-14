#include <SDL.h>
#include "SDL_image.h"

#include "client.h"
#include "globals.h"
#include "sdl_button.h"
#include "sdl_input_box.h"
#include "sdl_font.h"
#include "sdl_user.h"
#include "sdl_widget.h"

void login_function(void *arg)
{
	sdl_user *bob = (sdl_user*)arg;
	bob->login();
}

void quit_the_client(void *arg)
{
	sdl_user *bob = (sdl_user*)arg;
	bob->quit_client();
}

void sdl_user::quit_client()
{
	done = true;
}

void sdl_user::login()
{
	//TODO : make this function do something
}

sdl_user::sdl_user(Uint32 flags)
{
	load_progress = 0;
	draw_mode = 0;
	graphx = 0;
	pg = 0;
	num_widgets = 0;
	done = false;
	
	draw_mtx = SDL_CreateMutex();
	widgets = (sdl_widget**)0;
	ready = false;
	
	load_amount = 0x249f0;	//not sure how this number is generated
	display = SDL_CreateRGBSurface(flags, SCREEN_WIDTH, SCREEN_HEIGHT, 16,
		0x7C00, 0x03E0, 0x001F, 0);
}

void sdl_user::init_client(client *clnt)
{
	game = clnt;
}

sdl_user::~sdl_user()
{
	delete game;
	SDL_FreeSurface(display);
}

int sdl_user::get_widget(int x, int y)
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

void sdl_user::mouse_to(SDL_MouseMotionEvent *to)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
	SDL_mutexV(draw_mtx);
}

void sdl_user::mouse_from(SDL_MouseMotionEvent *from)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
	SDL_mutexV(draw_mtx);
}

void sdl_user::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
{	//TODO: handle click and drag movable widgets
	while (SDL_mutexP(draw_mtx) == -1) {};
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
	SDL_mutexV(draw_mtx);
}

void sdl_user::mouse_click(SDL_MouseButtonEvent *here)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (num_widgets > 0)
	{
		int index = get_widget(here->x, here->y);
		if (index != -1)
		{
			widgets[index]->mouse_click(here);
		}
	}
	SDL_mutexV(draw_mtx);
}

bool sdl_user::mouse_leave()
{
	return false;
}

void sdl_user::key_press(SDL_KeyboardEvent *button)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (num_widgets > 0)
	{
		if (button->type == SDL_KEYUP)
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
	SDL_mutexV(draw_mtx);
}

void sdl_user::give_data(graphics_data *abc)
{
	if (graphx == 0)
	{
		graphx = abc;
		prepare_load1();
		ready = true;
	}
}

void sdl_user::prepare_load1()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (pg != 0)
		delete pg;
	pg = new prepared_graphics;
	pg->num_pg = 2;
	
	int index;
	SDL_Rect *rect;

	pg->pg = new prepared_graphic[2];
	
	index = getHashIndex("811.png") + 1;
	pg->pg[0].surf = get_png_image("811.png", graphx->spritepack[index]);
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = false;
	
	pg->pg[1].surf = get_img(330, graphx->spritepack);
	rect = new SDL_Rect;
  	rect->x = 0xf1;
  	rect->y = 0x181;
	pg->pg[1].position = rect;
	rect = new SDL_Rect;
	rect->x = 0;
	rect->y = 0;
	rect->h = pg->pg[1].surf->h;
	rect->w = 0;
	pg->pg[1].mask = rect;
	pg->pg[1].cleanup = true;
	pg->ready = true;
	SDL_mutexV(draw_mtx);
}

void sdl_user::add_loaded(int size)
{
	if (draw_mode == 0)
	{
		load_progress += size;
		update_load();
	}
}

void sdl_user::load_done()
{
	if (draw_mode == 0)
	{
		load_progress = load_amount;
		update_load();
		SDL_Delay(250);
		while (SDL_mutexP(draw_mtx) == -1) {};
		ready = false;
		delete pg;
		pg = 0;
		prepare_login();
		ready = true;
		draw_mode = 1;

		SDL_mutexV(draw_mtx);
	}
}

void sdl_user::update_load()
{
	if (draw_mode == 0)
	{
		float temp = (float)pg->pg[1].surf->w * ((float)load_progress / (float)load_amount);
		pg->pg[1].mask->w = (Uint16)temp;
	}
}

void sdl_user::set_load_amount(int size)
{
	if (draw_mode == 0)
	{
		load_amount = size;
	}
}

void sdl_user::draw()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (ready)
	{
		pg->draw(display);
		for (int i = 0; i < num_widgets; i++)
		{
			if (widgets[i] != 0)
				widgets[i]->draw(display);
		}
		switch(draw_mode)
		{
			case 0:
				draw_load1();
				break;
			case 1:
				draw_login();
				break;
			default:
				break;
		}
	}
	SDL_mutexV(draw_mtx);
	SDL_Delay(10);
}

void sdl_user::prepare_login()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	pg = new prepared_graphics;
	pg->num_pg = 1;
	pg->pg = new prepared_graphic[1];
	
	int index = getHashIndex("814.png") + 1;
	pg->pg[0].surf = get_png_image("814.png", graphx->spritepack[index]);
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = false;
	pg->ready = true;
	
	num_widgets = 8;
	widgets = new sdl_widget*[num_widgets];
	
	widgets[0] = new sdl_widget(59, 0x1a9, 0x138, graphx);
		//type 0
	widgets[1] = new sdl_input_box(12, 0x1fb, 0x14a, graphx);
	widgets[1]->cursor_on();
	widget_key_focus = 1;
		//type 7, arg 2, nSub=17
	widgets[2] = new sdl_input_box(13, 0x1fb, 0x160, graphx);
		//type 7, arg 4, nSub=17
	widgets[3] = new sdl_button(53, 0x213, 0x183, graphx, &login_function, this);
		//type 2, login(), nSub=11, subMi=0x19ef70, px=0x25
	widgets[4] = new sdl_button(65, 0x213, 0x195, graphx, 0, 0);
		//type 2, normalMenu(4), nSub=9, subMi=0x19f1ac, px=0x26
	widgets[5] = new sdl_button(55, 0x213, 0x1a8, graphx, 0, 0);
		//type 2, newAccountMenu(5), nSub=24, subMi=0x19f630, px = 0x26
	widgets[6] = new sdl_button(57, 0x213, 0x1c2, graphx, &quit_the_client, this);
		//type 2, normalMenu(6), nSub=38, 
	widgets[7] = new sdl_widget(814, 0x1a, 0x3b, graphx);
		//type 1, null("intro"), px=0xcf, py=0x11a
//	widgets[8] = new sdl_widget(787, 0x244, 0x14, graphx);
//		//type = 10

	SDL_mutexV(draw_mtx);
}

void sdl_user::draw_load1()
{
}

void sdl_user::draw_login()
{
}