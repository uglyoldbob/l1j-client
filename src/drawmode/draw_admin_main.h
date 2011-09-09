#ifndef __DRAW_ADMIN_MAIN_H_
#define __DRAW_ADMIN_MAIN_H_

#include "funcptr.h"
#include "sdl_drawmode.h"

class draw_admin_main : public sdl_drawmode
{
	public:
		draw_admin_main(graphics_data *stuff, sdl_user *self);
		~draw_admin_main();

		virtual void mouse_click(SDL_MouseButtonEvent *here);
		virtual void mouse_to(SDL_MouseMotionEvent *to);
		virtual void mouse_from(SDL_MouseMotionEvent *from);
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
	private:
};

#endif