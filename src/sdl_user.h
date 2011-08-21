#ifndef _SDL_USER_H_
#define _SDL_USER_H_

#include <SDL.h>

class client;
#include "globals.h"
#include "resources/pack.h"
#include "resources/prepared_graphics.h"
#include "widgets/sdl_widget.h"

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
		void wait_for_char_select();
		void set_load_amount(int size);
		void add_loaded(int size);
		void load_done();
		volatile bool done;	//used to terminate the client
		
		friend class sdl_master;
	private:
		bool ready;
		volatile int draw_mode;
		int load_progress;
		int load_amount;
		
		client *game;
		graphics_data *graphx;
		SDL_Surface *display;
		
		SDL_mutex *draw_mtx;
		prepared_graphics *pg;
		sdl_widget **widgets;
		int num_widgets;
		int widget_key_focus;
		
		void draw();
		void prepare_char_sel();
		void prepare_load1();
		void draw_load1();
		void prepare_login();
		void draw_login();
		
		void update_load();

		int get_widget(int x, int y);	//returns the index of the widget for this point (-1 for none)
				
		//for tracking mouse movements 
		void mouse_to(SDL_MouseMotionEvent *to);
		void mouse_from(SDL_MouseMotionEvent *from);
		void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		bool mouse_leave();	//is it ok for the mouse to leave?
				
		void mouse_click(SDL_MouseButtonEvent *here);
		void key_press(SDL_KeyboardEvent *button);
};

#endif