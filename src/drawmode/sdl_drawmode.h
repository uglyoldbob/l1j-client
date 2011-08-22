#ifndef __SDL_DRAWMODE_H_
#define __SDL_DRAWMODE_H_

#include <SDL.h>

class graphics_data;
class prepared_graphics;
class sdl_user;
class sdl_widget;

enum drawmode
{
	INVALID,
	DRAWMODE_LOADING,
	DRAWMODE_LOGIN,
	DRAWMODE_CHARSEL,
	DRAWMODE_NEWCHAR
};

//this is a pure virtual (or abstract) class
class sdl_drawmode
{
	public:
		sdl_drawmode(graphics_data *stuff, sdl_user *self);
		virtual ~sdl_drawmode();
		sdl_user *owner;
	
		virtual void draw(SDL_Surface *display);
		
		virtual void key_press(SDL_KeyboardEvent *button);
		virtual void mouse_click(SDL_MouseButtonEvent *here) = 0;
		virtual void mouse_to(SDL_MouseMotionEvent *to) = 0;
		virtual void mouse_from(SDL_MouseMotionEvent *from) = 0;
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to) = 0;
		virtual bool mouse_leave() = 0;	//is it ok for the mouse to leave?
	protected:
		graphics_data *graphx;
		prepared_graphics *pg;
		sdl_widget **widgets;
		int num_widgets;
		int widget_key_focus;
		
		int get_widget(int x, int y);	//returns the index of the widget for this point (-1 for none)
};

#endif