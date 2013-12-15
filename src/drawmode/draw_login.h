#ifndef __DRAW_LOGIN_H_
#define __DRAW_LOGIN_H_

#include "funcptr.h"
#include "sdl_drawmode.h"
#include "resources/table.h"

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
		void draw(SDL_Surface *display);
		
		virtual void key_press(SDL_KeyboardEvent *button);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
		
		virtual bool quit_request();
	private:
		table intro;
};

#endif