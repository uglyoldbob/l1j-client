#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "client.h"
#include "drawmode/draw_char_sel.h"
#include "drawmode/draw_game.h"
#include "drawmode/draw_new_char.h"
#include "drawmode/draw_loading.h"
#include "drawmode/draw_login.h"
#include "globals.h"
#include "resources/sdl_font.h"
#include "sdl_user.h"

/** signal the client thread through the client object that we are quitting */
void sdl_user::quit_client()
{
	game->stop();
	done = true;
}

/** Initiate the login into the game server 
 \todo Move this function entirely to the client class */
void sdl_user::login(const char *name, const char *pass)
{
	game->send_packet("css", CLIENT_LOGIN, name, pass);
}

/** Initialize ourself, create our screen, etc.. */
sdl_user::sdl_user(Uint32 flags)
{
	draw_mode = INVALID;
	change_draw = INVALID;
	done = false;
	
	draw_mtx = SDL_CreateMutex();

	ready = false;

	drawmode = 0;
	map_tiles = 0;
}

/** copy the reference to the client object */
void sdl_user::init_client(client *clnt)
{
	game = clnt;
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

/** cleanup */
sdl_user::~sdl_user()
{
	SDL_DestroyMutex(draw_mtx);
	draw_mtx = 0;
	if (drawmode != 0)
	{
		game->delete_requests();
		delete drawmode;
		drawmode = 0;
	}
	if (map_tiles != 0)
	{
		delete [] map_tiles;
		map_tiles = 0;
	}
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

void sdl_user::mouse_from(SDL_MouseMotionEvent *from)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		drawmode->mouse_from(from);
	}
	SDL_mutexV(draw_mtx);
}

/** Do I allow the mouse to leave my drawing area? */
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

/** Block until the drawing mode is achieved. Call this from the client thread only. */
void sdl_user::wait_for_mode(enum drawmode wait)
{
	while (draw_mode != wait)
	{
		SDL_Delay(100);
	};
}

/** Am I in a specific drawmode? */
bool sdl_user::is_in_mode(enum drawmode here)
{
	if (draw_mode == here)
		return true;
	else
		return false;
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

/** Handle keypresses that are common to all drawmodes, pass the rest to the drawmode for handling */
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

/** Return the drawmode object */
sdl_drawmode *sdl_user::get_drawmode()
{
	return drawmode;
}

/** spin until we are ready. Called from the client thread after changing drawmode*/
bool sdl_user::are_you_ready()
{
	return ready;
}

/** Pass the quit request on to the drawmode */
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

/** change the current drawmode */
void sdl_user::change_drawmode(enum drawmode chg)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	change_draw = chg;
	if (draw_mode == INVALID)
		check_for_change_drawmode();
	SDL_mutexV(draw_mtx);
}

void sdl_user::check_for_change_drawmode()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (change_draw != INVALID)
	{
		enum drawmode chg = change_draw;
		ready = false;
		if (drawmode != 0)
		{
			game->delete_requests();
			delete drawmode;
			drawmode = 0;
		}
		switch(chg)
		{
			case DRAWMODE_LOADING:
				drawmode = new draw_loading(this);
				draw_mode = chg;
				ready = true;
				break;
			case DRAWMODE_LOGIN:
				drawmode = new draw_login(this);
				draw_mode = chg;
				ready = true;
				break;
			case DRAWMODE_CHARSEL:
				drawmode = new draw_char_sel(this);
				draw_mode = chg;
				ready = true;
				break;
			case DRAWMODE_NEWCHAR:
				drawmode = new draw_new_char(this);
				draw_mode = chg;
				ready = true;
				break;
			case DRAWMODE_GAME:
				drawmode = new draw_game(this);
				draw_mode = chg;
				ready = true;
				break;
			default:
				drawmode = 0;
				draw_mode = INVALID;
				break;
		}
	}
	change_draw = INVALID;
	SDL_mutexV(draw_mtx);
}

void sdl_user::add_request(client_request obj)
{
	game->add_request(obj);
}

/** Draw the game if the current drawmode is valid and if we are ready */
void sdl_user::draw(SDL_Surface *display)
{
	check_for_change_drawmode();
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

/** Return the config object from the client */
config * sdl_user::get_config()
{
	return game->get_config();
}
