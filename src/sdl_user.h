#ifndef _SDL_USER_H_
#define _SDL_USER_H_

#include <SDL.h>

class client;
#include "globals.h"
#include "drawmode/sdl_drawmode.h"
#include "resources/music.h"
class pack;
class prepared_graphics;
class sdl_widget;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
//#define true 1
//#define false 0
#define COLORKEY 255, 0, 255 //Your Transparent colour

void test_func(void * arg);

class sdl_user
{
	public:
		sdl_user(Uint32 flags = SDL_SWSURFACE);
		~sdl_user();
		void quit_client();
		void login();
		
		void init_client(client *clnt);
		
		void give_data(graphics_data *abc);
		
		void set_login_char(int num, lin_char_info *data);
		void wait_for_mode(enum drawmode wait);
		sdl_drawmode *get_drawmode();
		void wait_ready();
		void change_drawmode(enum drawmode chg);
		volatile bool done;	//used to terminate the client
		music game_music;
		
		friend class sdl_master;
	private:
		bool ready;
		volatile enum drawmode draw_mode;
				
		client *game;
		graphics_data *graphx;
		SDL_Surface *display;
		
		sdl_drawmode *drawmode;
		
		SDL_mutex *draw_mtx;
		
		void draw();
		void prepare_char_sel();
		
		//for tracking mouse movements 
		void mouse_to(SDL_MouseMotionEvent *to);
		void mouse_from(SDL_MouseMotionEvent *from);
		void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		bool mouse_leave();	//is it ok for the mouse to leave?
				
		void mouse_click(SDL_MouseButtonEvent *here);
		void key_press(SDL_KeyboardEvent *button);
};

#endif