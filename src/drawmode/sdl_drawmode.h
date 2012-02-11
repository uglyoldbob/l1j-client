#ifndef __SDL_DRAWMODE_H_
#define __SDL_DRAWMODE_H_

#include "funcptr.h"
#include <SDL/SDL.h>

#include "resources/sdl_graphic.h"

class graphics_data;
class sdl_user;
class sdl_widget;

enum drawmode
{
	INVALID,
	DRAWMODE_LOADING,
	DRAWMODE_LOGIN,
	DRAWMODE_CHARSEL,
	DRAWMODE_NEWCHAR,
	DRAWMODE_GAME,
	DRAWMODE_ADMIN_MAIN,
	DRAWMODE_MAINT_IMG,
	DRAWMODE_MAINT_PNG,
	DRAWMODE_MAINT_TIL,
	DRAWMODE_MAINT_MAP
};

enum dam_funcs
{
	DAM_FUNC_CHGMODE
};

class sdl_drawmode;
class dam_ptr : public funcptr
{
	public:
		dam_ptr(sdl_drawmode *bla, enum drawmode val);
		virtual void go();
	private:
		enum drawmode which;
		sdl_drawmode *ref;
};

void change_mode(void *a, void* b);

//this is a pure virtual (or abstract) class
class sdl_drawmode
{
	public:
		sdl_drawmode(sdl_user *self);
		virtual ~sdl_drawmode();
		sdl_user *owner;
	
		virtual void draw(SDL_Surface *display);
		
		virtual void key_press(SDL_KeyboardEvent *button);
		virtual void mouse_click(SDL_MouseButtonEvent *here) = 0;
		virtual void mouse_to(SDL_MouseMotionEvent *to) = 0;
		virtual void mouse_from(SDL_MouseMotionEvent *from) = 0;
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to) = 0;
		virtual bool mouse_leave() = 0;	//is it ok for the mouse to leave?
		
		sdl_widget *get_widget(int i);
		virtual bool quit_request();		
	protected:
		sdl_graphic **gfx;
		int num_gfx;
		sdl_widget **widgets;
		int num_widgets;
		int widget_key_focus;

		SDL_mutex *draw_mtx;
		bool draw_scene;
		
		int get_widget(int x, int y);	//returns the index of the widget for this point (-1 for none)
};

#endif