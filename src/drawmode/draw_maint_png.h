#ifndef __DRAW_MAINT_PNG_H_
#define __DRAW_MAINT_PNG_H_

#include "sdl_drawmode.h"

class draw_maint_png : public sdl_drawmode
{
	public:
		draw_maint_png(sdl_user *self);
		~draw_maint_png();
		
		virtual void key_press(SDL_KeyboardEvent *button);
		
		virtual void draw(SDL_Surface *display);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
		
		virtual bool quit_request();
	private:
		SDL_mutex *draw_mtx;
		bool extracting;
		int background;
};

#endif