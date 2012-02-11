#ifndef __DRAW_MAINT_TIL_H_
#define __DRAW_MAINT_TIL_H_

#include "sdl_drawmode.h"
class tile;
class sdl_graphic;

class draw_maint_til : public sdl_drawmode
{
	public:
		draw_maint_til(sdl_user *self);
		~draw_maint_til();
		
		virtual void key_press(SDL_KeyboardEvent *button);
		
		virtual void draw(SDL_Surface *display);
		virtual void mouse_click(SDL_MouseButtonEvent *here);
		virtual void mouse_to(SDL_MouseMotionEvent *to);
		virtual void mouse_from(SDL_MouseMotionEvent *from);
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
		
		virtual bool quit_request();
	private:
		SDL_mutex *draw_mtx;
		
		int background;
		tile *tileset;
		sdl_graphic *cur_tile;
		int tileset_num;
		int tile_num;
		
		void update_tile();
};

#endif