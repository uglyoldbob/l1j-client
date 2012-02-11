#ifndef __SDL_ANIMATE_BUTTON_H_
#define __SDL_ANIMATE_BUTTON_H_

#include <SDL/SDL.h>
class client;

#include "globals.h"
#include "sdl_plain_button.h"
#include "sdl_widget.h"

class sdl_animate_button : public sdl_plain_button
{
	public:
		sdl_animate_button(int num, int x, int y, client *who, funcptr *stuff);
		virtual ~sdl_animate_button();
		virtual void draw(SDL_Surface *display);
		
		void animate(bool animate);
		
		void set_info(lin_char_info *data);
		lin_char_info *get_info();
		
		lin_char_info *char_info;
		
	protected:
		int x;
		int y;
		bool animating;
		sdl_graphic **animates;
		int num_anim;
		int first_anim;
		int first_repeat;
		client *blabla;
		static int frame_time;
		int cur_frame;
		Uint32 change_time;
		virtual void key_press(SDL_KeyboardEvent *button);
		void delete_animation();
};

#endif