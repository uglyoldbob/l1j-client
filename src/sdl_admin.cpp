#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <time.h>

#include "drawmode/draw_admin_main.h"
#include "drawmode/draw_maint_img.h"
#include "drawmode/draw_maint_map.h"
#include "drawmode/draw_maint_sprites.h"
#include "drawmode/draw_maint_til.h"
#include "globals.h"
#include "lindes.h"
#include "resources/sdl_font.h"
#include "sdl_user.h"

void sdl_user::quit_client()
{
	done = true;
}

void sdl_user::login(const char *name, const char *pass)
{
}

int sdl_user::check_login_chars()
{
	return 0;
}

lin_char_info** sdl_user::get_login_chars()
{
	return 0;
}

sdl_user::sdl_user(Uint32 flags)
{
	draw_mode = INVALID;
	done = false;
	
	draw_mtx = SDL_CreateMutex();
	mode_mtx = SDL_CreateMutex();

	ready = false;
	
	drawmode = 0;
	common_construct();
}

void sdl_user::send_packet(packet_data &sendme)
{
	printf("STUB Send a packet\n");
}

sdl_user::~sdl_user()
{
	printf("Deleting the sdl_user\n");
	SDL_DestroyMutex(draw_mtx);
	SDL_DestroyMutex(mode_mtx);
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
	common_destruct();
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

void sdl_user::wait_for_mode(enum drawmode wait, bool mutex)
{
	while (draw_mode != wait)
	{
		SDL_Delay(100);
	};
	if (mutex)
	{
		while (SDL_mutexP(mode_mtx) == -1) {};
	}
}

/** unlocks the mode mutex */
void sdl_user::done_with_drawmode()
{
	SDL_mutexV(mode_mtx);
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

sdl_drawmode *sdl_user::get_drawmode(bool mutex)
{
	if (mutex)
	{
		while (SDL_mutexP(mode_mtx) == -1) {};
	}
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
		delete_requests();
		stop();
	}
	return temp;
}

/** Am I in a specific drawmode? */
bool sdl_user::is_in_mode(enum drawmode here, bool mutex)
{
	if (draw_mode == here)
	{
		if (mutex)
		{
			while (SDL_mutexP(mode_mtx) == -1) {};
		}
		return true;
	}
	else
	{
		return false;
	}
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
	while (SDL_mutexP(mode_mtx) == -1) {};
	if (change_draw != INVALID)
	{
		enum drawmode chg = change_draw;
		ready = false;
		if (drawmode != 0)
		{
			delete_requests();
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
	}
	change_draw = INVALID;
	SDL_mutexV(mode_mtx);
	SDL_mutexV(draw_mtx);
}

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

void sdl_user::init()
{
	main_config = new config("Lineage.ini");
	if (!main_config->config_ok())
	{
		delete main_config;
		main_config = 0;
		throw "ERROR Loading configuration file.\n";
	}
	char *test;
	test = (char*)getfiles->load_file("Sprite00.idx", 0, FILE_REGULAR1, 0);
	if (test == 0)
	{
		throw "Lineage Data not found";
	}
	delete [] test;
	test = 0;
	lineage_font.init("Font/eng.fnt", this);

	DesKeyInit("~!@#%^$<");	//TODO : move this code to a class and use an object
	init_packs();
	init_tiles();
	
	change_drawmode(DRAWMODE_ADMIN_MAIN);
	while (!are_you_ready())
	{
		check_requests();
	}
	
	init_codepage(0);
	init_math_tables();
	printf("STUB Load player config\n");
	printf("STUB Initialize emblem cache\n");
	init_strings();
}

void sdl_user::register_char(lin_char_info *data)
{
}

int sdl_user::run()
{
	try
	{
		init();
		while (1)
		{
			SDL_Delay(10);
			check_requests();
		}
	}
	catch (const char *error)
	{
		ready = true;
		printf("FATAL ERROR: %s\n", error);
		ready = true;
		done = true;
	}
}
