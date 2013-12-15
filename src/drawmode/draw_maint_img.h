#ifndef __DRAW_MAINT_IMG_H_
#define __DRAW_MAINT_IMG_H_

#include "sdl_drawmode.h"

class draw_maint_img : public sdl_drawmode
{
	public:
		draw_maint_img(sdl_user *self);
		~draw_maint_img();
		
		virtual void key_press(SDL_KeyboardEvent *button);
		
		virtual void draw(SDL_Surface *display);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
		
		virtual bool quit_request();
	private:
		SDL_mutex *draw_mtx;
		
		int background;
};

#endif