#ifndef _SDL_INPUT_BOX_H_
#define _SDL_INPUT_BOX_H_

#include "sdl_widget.h"

class sdl_input_box : public sdl_widget
{
	public:
		sdl_input_box(int num, int x, int y, graphics_data *packs);
		virtual void draw(SDL_Surface *display);
		
		void cursor_on();
		void cursor_off();
	protected:
		bool have_cursor;
};

#endif