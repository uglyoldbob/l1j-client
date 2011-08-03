#ifndef _SDL_USER_H_
#define _SDL_USER_H_

#include <SDL.h>

#include "pack.h"
#include "prepared_graphics.h"
#include "sdl_widget.h"

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
		
		SDL_Surface *get_image(const char *name, pack *source);
		SDL_Surface *get_png_image(const char *name, pack *source);
		SDL_Surface *get_img(const char *name, pack **source);	//loads raw image data
		SDL_Surface *get_image(SDL_RWops *buf);
		
		void draw();
		void prepare_load1();
		void draw_load1();
		void prepare_login();
		void draw_login();
		
		void update_load();
};

#endif