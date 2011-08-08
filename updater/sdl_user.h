#ifndef _SDL_USER_H_
#define _SDL_USER_H_

#include <SDL.h>

#include "globals.h"
#include "pack.h"
#include "prepared_graphics.h"
#include "sdl_widget.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
//#define true 1
//#define false 0
#define COLORKEY 255, 0, 255 //Your Transparent colour

class sdl_user
{
	public:
		sdl_user(Uint32 flags = SDL_SWSURFACE);
		~sdl_user();
		
		void give_data(graphics_data *abc);
		
		void set_load_amount(int size);
		void add_loaded(int size);
		void load_done();
		
		friend class sdl_master;
	private:
		bool ready;
		int draw_mode;
		int load_progress;
		int load_amount;
		graphics_data *graphx;
		SDL_Surface *display;
		
		prepared_graphics *pg;
		sdl_widget **widgets;
		int num_widgets;
		
		void draw();
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
};

#endif