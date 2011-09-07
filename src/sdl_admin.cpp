#include <SDL.h>
#include "SDL_image.h"
#include <time.h>

#include "client.h"
#include "drawmode/draw_admin_main.h"
#include "drawmode/draw_loading.h"
#include "drawmode/draw_login.h"
#include "drawmode/draw_maint_img.h"
#include "drawmode/draw_maint_map.h"
#include "drawmode/draw_maint_til.h"
#include "globals.h"
#include "resources/sdl_font.h"
#include "sdl_user.h"

void sdl_user::quit_client()
{
	done = true;
}

void sdl_user::login()
{
}

sdl_user::sdl_user(Uint32 flags)
{
	draw_mode = INVALID;
	graphx = 0;
	done = false;
	
	draw_mtx = SDL_CreateMutex();

	ready = false;
	
	display = SDL_CreateRGBSurface(flags, SCREEN_WIDTH, SCREEN_HEIGHT, 16,
		0x7C00, 0x03E0, 0x001F, 0);
	drawmode = 0;
}

void sdl_user::init_client(client *clnt)
{
	game = clnt;
}

sdl_user::~sdl_user()
{
//	delete game;
	if (drawmode != 0)
		delete drawmode;
	SDL_FreeSurface(display);
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
				case SDLK_PRINT:
					char filename[256];
					time_t rawtime;
					struct tm * timeinfo;
					time ( &rawtime );
					timeinfo = localtime ( &rawtime );
					sprintf(filename, "%s", asctime(timeinfo));
					filename[strlen(filename)-1] = 0;
					strcat(filename, ".bmp");
					for (int i = 0; i < strlen(filename); i++)
					{
						if ((filename[i] == ' ') || (filename[i] == ':'))
							filename[i] = '_';
					}
					printf("Screenshot to %s\n", filename);
					SDL_SaveBMP(display, filename);
					break;
				default:
					drawmode->key_press(button);
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
		change_drawmode(DRAWMODE_ADMIN_MAIN);
	}
}

sdl_drawmode *sdl_user::get_drawmode()
{
	return drawmode;
}

void sdl_user::wait_ready()
{
	while (!ready) {};
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
			drawmode = new draw_admin_main(graphx, this);
			draw_mode = chg;
			ready = true;
			break;
		case DRAWMODE_MAINT_IMG:
			drawmode = new draw_maint_img(graphx, this);
			draw_mode = chg;
			ready = true;
			break;
		case DRAWMODE_MAINT_TIL:
			drawmode = new draw_maint_til(graphx, this);
			draw_mode = chg;
			ready = true;
			break;
		case DRAWMODE_MAINT_MAP:
			drawmode = new draw_maint_map(graphx, this);
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

void sdl_user::draw()
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
