#ifndef __SDL_WINDOW_H_
#define __SDL_WINDOW_H_

#include "sdl_widget.h"

class sdl_window : public sdl_widget
{
	public:
		sdl_window(int num, int x, int y, int w, int h, sdl_user *who);
		~sdl_window();
		
		virtual void draw(SDL_Surface *display);
	protected:
		sdl_widget **widgets;
		int x, y;
		int num_widgets;
		int widget_key_focus;		
};

#endif