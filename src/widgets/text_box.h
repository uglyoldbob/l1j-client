#ifndef __TEXT_BOX_H_
#define __TEXT_BOX_H_

#include "sdl_widget.h"

class client;

class text_box : public sdl_widget
{
	public:
		text_box(int x, int y, int w, int h, client *who);
		~text_box();

//		virtual void key_press(SDL_KeyboardEvent *button);

		void set_visible(int top, int height);
		void add_line(char *line);
	private:
		char **lines;
		int num_lines;
		int max_lines;
		
		int first_visible;	//pixel of first visible row
		int height;	//height to display
		
		int width_c;	//number of characters in width
		
		void redraw();
};

#endif