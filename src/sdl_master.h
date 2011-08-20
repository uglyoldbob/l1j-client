#ifndef __SDL_MASTER_H_
#define __SDL_MASTER_H_

#include <SDL.h>
#include <SDL_Thread.h>

#include "sdl_user.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
//#define true 1
//#define false 0
#define COLORKEY 255, 0, 255
//Your Transparent colour

class sdl_master
{
	public:
		sdl_master(Uint32 flags = SDL_DOUBLEBUF | SDL_HWSURFACE);
		~sdl_master();
				
		void create_client();
		
		void process_events();	//this is the function for the first thread
	private:
		SDL_Surface *display;	//all data will be written to this class
		int ready;
		sdl_user **clients;
		SDL_Thread **game_client;	//array of all game clients (only one right now)
		
		void draw();
		
		int get_client(int x, int y);
		void mouse_move(SDL_MouseMotionEvent *old, SDL_MouseMotionEvent *fresh);
		void mouse_click(SDL_MouseButtonEvent *here);
		void key_press(SDL_KeyboardEvent *button);
};

#endif