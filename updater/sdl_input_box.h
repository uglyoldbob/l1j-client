#ifndef _SDL_INPUT_BOX_H_
#define _SDL_INPUT_BOX_H_

#include "sdl_widget.h"

#define MAX_LENGTH 100

class sdl_input_box : public sdl_widget
{
	public:
		sdl_input_box(int num, int x, int y, graphics_data *packs);
		virtual void draw(SDL_Surface *display);
		
		virtual void cursor_on();
		virtual void cursor_off();
	protected:
		bool have_cursor;
		bool draw_cursor;
		int cursor_pos;	//pixel offset
		int cursor_idx;	//character number offset
		char field[MAX_LENGTH];
		
		void cursor_toggle();
};

#endif