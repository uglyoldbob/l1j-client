#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <time.h>

#include "client.h"
#include "drawmode/draw_admin_main.h"
#include "drawmode/draw_maint_img.h"
#include "drawmode/draw_maint_map.h"
#include "drawmode/draw_maint_sprites.h"
#include "drawmode/draw_maint_til.h"
#include "globals.h"
#include "resources/sdl_font.h"
#include "sdl_user.h"

void sdl_user::quit_client()
{
	done = true;
}

void sdl_user::login(const char *name, const char *pass)
{
}

sdl_user::sdl_user(Uint32 flags)
{
	draw_mode = INVALID;
	done = false;
	
	draw_mtx = SDL_CreateMutex();

	ready = false;
	
	drawmode = 0;
}

void sdl_user::init_client(client *clnt)
{
	game = clnt;
}

sdl_user::~sdl_user()
{
//	delete game;
	SDL_DestroyMutex(draw_mtx);
	if (drawmode != 0)
	{
		delete drawmode;
		drawmode = 0;
	}
	if (map_tiles != 0)
	{
		delete [] map_tiles;
		map_tiles = 0;
	}
}

int sdl_user::init_tiles()
{
	int last_good = 100;
	for (int i = 0; i < (last_good * 2); i++)
	{
		char fname[100];
		sprintf(fname, "%d.til", i);
		if (tilepack->check_file(fname) != -1)
		{	//the tileset is good
			if (i > last_good)
				last_good = i;
		}
	}
	number_map_tiles = last_good;
	if (number_map_tiles > 0)
	{
		map_tiles = new tile[number_map_tiles];
	}
	else
	{
		map_tiles = 0;
	}
	
	return 0;
}

tile *sdl_user::get_tiles()
{
	return map_tiles;
}

void sdl_user::mouse_to(SDL_MouseMotionEvent *to)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		drawmode->mouse_to(to);
	}
	SDL_mutexV(draw_mtx);
}

void sdl_user::add_request(client_request obj)
{
	game->add_request(obj);
}

void sdl_user::mouse_from(SDL_MouseMotionEvent *from)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		drawmode->mouse_from(from);
	}
	SDL_mutexV(draw_mtx);
}

bool sdl_user::mouse_leave()
{
	bool ret_val;
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		ret_val = drawmode->mouse_leave();
	}
	else
	{
		ret_val = false;
	}
	SDL_mutexV(draw_mtx);
	return ret_val;
}

void sdl_user::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
{	//TODO: handle click and drag movable widgets
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		drawmode->mouse_move(from, to);
	}
	SDL_mutexV(draw_mtx);
}

void sdl_user::wait_for_mode(enum drawmode wait)
{
	while (draw_mode != wait)
	{
		SDL_Delay(100);
	};
}

void sdl_user::mouse_click(SDL_MouseButtonEvent *here)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		drawmode->mouse_click(here);
	}
	SDL_mutexV(draw_mtx);
}

void sdl_user::key_press(SDL_KeyboardEvent *button)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		if (button->type == SDL_KEYDOWN)
		{
			switch(button->keysym.sym)
			{
				default:
					drawmode->key_press(button);
					break;
			}
		}
	}
	SDL_mutexV(draw_mtx);
}

sdl_drawmode *sdl_user::get_drawmode()
{
	return drawmode;
}

bool sdl_user::are_you_ready()
{
	return ready;
}

bool sdl_user::quit_request()
{
	bool temp = drawmode->quit_request();
	if (temp)
	{
		printf("Telling the client to stop\n");
		game->stop();
	}
	return temp;
}

void sdl_user::change_drawmode(enum drawmode chg)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	ready = false;
	if (drawmode != 0)
	{
		delete drawmode;
		drawmode = 0;
	}
	switch(chg)
	{
		case DRAWMODE_ADMIN_MAIN:
			drawmode = new draw_admin_main(this);
			draw_mode = chg;
			ready = true;
			break;
		case DRAWMODE_MAINT_IMG:
			drawmode = new draw_maint_img(this);
			draw_mode = chg;
			ready = true;
			break;
		case DRAWMODE_MAINT_TIL:
			drawmode = new draw_maint_til(this);
			draw_mode = chg;
			ready = true;
			break;
		case DRAWMODE_MAINT_MAP:
			drawmode = new draw_maint_map(this);
			draw_mode = chg;
			ready = true;
			break;
		case DRAWMODE_MAINT_SPRITES:
			drawmode = new draw_maint_sprites(this);
			draw_mode = chg;
			ready = true;
			break;
		default:
			drawmode = 0;
			draw_mode = INVALID;
			break;
	}
	SDL_mutexV(draw_mtx);
}

void sdl_user::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (ready)
	{
		if (drawmode != 0)
		{
			drawmode->draw(display);
		}
	}
	SDL_mutexV(draw_mtx);
}
