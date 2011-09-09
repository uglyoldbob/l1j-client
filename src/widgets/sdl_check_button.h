#ifndef __SDL_CHECK_BUTTON_H_
#define __SDL_CHECK_BUTTON_H_

#include <SDL.h>

#include "globals.h"
#include "sdl_plain_button.h"

#include <SDL.h>

class sdl_check_button : public sdl_plain_button
{
	public:
		sdl_check_button(int num, int x, int y, graphics_data *packs, funcptr *stuff);
		virtual ~sdl_check_button();
		virtual void draw(SDL_Surface *display);
		
		virtual void mouse_click(SDL_MouseButtonEvent *here);
	protected:
		int checked;
};

#endif