#ifndef __DRAW_LOGIN_H_
#define __DRAW_LOGIN_H_

#include "funcptr.h"
#include "sdl_drawmode.h"

class draw_login;

class login_ptr : public funcptr
{
	public:
		login_ptr(draw_login *bla);
		virtual void go();
	private:
		draw_login *ref;
};

class quit_ptr : public funcptr
{
	public:
		quit_ptr(draw_login *bla);
		virtual void go();
	private:
		draw_login *ref;
};

class draw_login : public sdl_drawmode
{
	public:
		draw_login(sdl_user *self);
		~draw_login();
		void login();
		void quit();
		
		virtual void key_press(SDL_KeyboardEvent *button);
		virtual void mouse_click(SDL_MouseButtonEvent *here);
		virtual void mouse_to(SDL_MouseMotionEvent *to);
		virtual void mouse_from(SDL_MouseMotionEvent *from);
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
};

#endif