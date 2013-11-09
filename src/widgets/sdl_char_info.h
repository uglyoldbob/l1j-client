#ifndef __SDL_CLASS_INFO_H_
#define __SDL_CLASS_INFO_H_

#include <SDL/SDL.h>
class client;

#include "globals.h"
#include "sdl_widget.h"

class sdl_char_info : public sdl_widget
{
	public:
		sdl_char_info(sdl_user *who);
		virtual ~sdl_char_info();
		virtual void draw(SDL_Surface *display);
		void hand_info(lin_char_info *bla);
	protected:
		lin_char_info *data;
		SDL_Surface *fonts;
	
		virtual bool contains(int x, int y);	//does this widget contain the given point?
		
		void redraw();
		
		void mouse_to(SDL_MouseMotionEvent *to);
		void mouse_from(SDL_MouseMotionEvent *from);
		void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
};

#endif