#ifndef __DRAW_MAINT_MAP_H_
#define __DRAW_MAINT_MAP_H_

#include "sdl_drawmode.h"
class lin_map;
class sdl_graphic;

class draw_maint_map : public sdl_drawmode
{
	public:
		draw_maint_map(sdl_user *self);
		~draw_maint_map();
		
		virtual void key_press(SDL_KeyboardEvent *button);
		
		virtual void draw(SDL_Surface *display);
		virtual void mouse_click(SDL_MouseButtonEvent *here);
		virtual void mouse_to(SDL_MouseMotionEvent *to);
		virtual void mouse_from(SDL_MouseMotionEvent *from);
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
	private:
		SDL_mutex *draw_mtx;
		
		lin_map *themap;
		sdl_graphic *map_vis;
		int x, y;
};

#endif