#ifndef __DRAW_ADMIN_MAIN_H_
#define __DRAW_ADMIN_MAIN_H_

#include "funcptr.h"
#include "sdl_drawmode.h"

class draw_admin_main;
class quit_ptr : public funcptr
{
	public:
		quit_ptr(draw_admin_main *bla);
		virtual void go();
	private:
		draw_admin_main *ref;
};

class draw_admin_main : public sdl_drawmode
{
	public:
		draw_admin_main(sdl_user *self);
		~draw_admin_main();

		void quit();

		virtual bool mouse_leave();	//is it ok for the mouse to leave?
		
		virtual bool quit_request();
		virtual void draw(SDL_Surface *display);
	private:
		bool quitting;
		table sprites_table;
};

#endif