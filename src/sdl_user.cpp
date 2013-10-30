#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <time.h>

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
	done = false;
	
	draw_mtx = SDL_CreateMutex();

	ready = false;
	
	display = SDL_CreateRGBSurface(flags, SCREEN_WIDTH, SCREEN_HEIGHT, 16,
		0x7C00, 0x03E0, 0x001F, 0);
	drawmode = 0;
}

/** copy the reference to the client object */
void sdl_user::init_client(client *clnt)
{
	game = clnt;
}

/** cleanup */
sdl_user::~sdl_user()
{
	SDL_DestroyMutex(draw_mtx);
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
				case SDLK_PRINT:
					char filename[256];
					time_t rawtime;
					struct tm * timeinfo;
					time ( &rawtime );
					timeinfo = localtime ( &rawtime );
					sprintf(filename, "%s", asctime(timeinfo));
					filename[strlen(filename)-1] = 0;
					strcat(filename, ".bmp");
					for (unsigned int i = 0; i < strlen(filename); i++)
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

/** Return the drawmode object */
sdl_drawmode *sdl_user::get_drawmode()
{
	return drawmode;
}

/** spin until we are ready. Called from the client thread after changing drawmode*/
void sdl_user::wait_ready()
{
	while (!ready) {};
}

/** Pass the quit request on to the drawmode */
bool sdl_user::quit_request()
{
	return drawmode->quit_request();
}

/** change the current drawmode */
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
	SDL_mutexV(draw_mtx);
}

/** Draw the game if the current drawmode is valid and if we are ready */
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
