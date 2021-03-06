#ifndef __SDL_PLAIN_BUTTON_H_
#define __SDL_PLAIN_BUTTON_H_

#include <SDL/SDL.h>

#include "globals.h"
#include "sdl_button.h"

sdl_graphic *make_sdl_graphic(int num, int x, int y, sdl_user *who);
SDL_Rect *make_sdl_rect(int x, int y, int w, int h);

class sdl_plain_button : public sdl_button
{
	public:
		sdl_plain_button(int num, int x, int y, sdl_user *who, funcptr *stuff);
		virtual ~sdl_plain_button();
		virtual void draw(SDL_Surface *display);
				
	protected:
		sdl_graphic *two;
};

#endif