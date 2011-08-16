#ifndef __SDL_ANIMATE_BUTTON_H_
#define __SDL_ANIMATE_BUTTON_H_

#include <SDL.h>

#include "globals.h"
#include "sdl_button.h"

sdl_graphic *make_sdl_graphic(int num, int x, int y, graphics_data *packs);
SDL_Rect *make_sdl_rect(int x, int y, int w, int h);

#include <SDL.h>

class sdl_animate_button : public sdl_button
{
	public:
		sdl_animate_button(int num, int x, int y, graphics_data *packs, void (*fnctn)(void*), void* ag);
		virtual ~sdl_animate_button();
		virtual void draw(SDL_Surface *display);
		
		virtual void cursor_on();
		virtual void cursor_off();
	protected:
		sdl_graphic test;
		
		sdl_graphic *animates;
		int num_anim;
		int first_repeat;
		sdl_graphic inactive;
		sdl_graphic hover;
		graphics_data *blabla;
		static int frame_time;
		int in;
		virtual void key_press(SDL_KeyboardEvent *button);
};

#endif