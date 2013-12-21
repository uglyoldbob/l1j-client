#ifndef __SDL_TEXT_BUTTON_H_
#define __SDL_TEXT_BUTTON_H_

#include <SDL/SDL.h>

#include "globals.h"
#include "sdl_plain_button.h"

sdl_graphic *make_sdl_graphic(int num, int x, int y, graphics_data *packs);
SDL_Rect *make_sdl_rect(int x, int y, int w, int h);

class sdl_text_button : public sdl_plain_button
{
	public:
		sdl_text_button(const char *bla, int x, int y, sdl_user *who, funcptr *stuff);
		virtual ~sdl_text_button();
		void set_display(const char* disp);
		virtual void draw(SDL_Surface *display);

	protected:
		int xpos, ypos;
};

#endif