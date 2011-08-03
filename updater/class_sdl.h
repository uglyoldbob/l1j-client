#ifndef __CLASS_SDL_H_
#define __CLASS_SDL_H_

#include <SDL.h>

#include "pack.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
//#define true 1
//#define false 0
#define COLORKEY 255, 0, 255 //Your Transparent colour

struct graphics_data
{
	pack *tilepack;
	pack **spritepack;
	int num_sprite_pack;	
};

class class_sdl
{
	public:
		class_sdl(Uint32 flags = SDL_DOUBLEBUF | SDL_SWSURFACE);
		~class_sdl();
		void give_data(graphics_data *abc);
		
		void draw_load1();
		
		void do_stuff();
	private:
		SDL_Surface *display;
		int ready;
		graphics_data *graphx;
		
		SDL_Surface *get_image(const char *name, pack *source);
		SDL_Surface *get_png_image(const char *name, pack *source);
		SDL_Surface *get_image(SDL_RWops *buf);
};

#endif