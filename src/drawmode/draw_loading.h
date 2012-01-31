#ifndef __DRAW_LOADING_H_
#define __DRAW_LOADING_H_

#include "sdl_drawmode.h"

class draw_loading : public sdl_drawmode
{
	public:
		draw_loading(sdl_user *self);
		~draw_loading();

		void update_load();
		void set_load_amount(int size);
		void add_loaded(int size);
		void load_done();
		
		virtual void mouse_click(SDL_MouseButtonEvent *here);
		virtual void mouse_to(SDL_MouseMotionEvent *to);
		virtual void mouse_from(SDL_MouseMotionEvent *from);
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
	private:
		int load_progress;
		int load_amount;
};

#endif