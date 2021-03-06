#ifndef __SDL_RADIO_BUTTON_H_
#define __SDL_RADIO_BUTTON_H_

#include <SDL/SDL.h>

#include "globals.h"
#include "sdl_check_button.h"

class sdl_radio_button : public sdl_check_button
{
	public:
		sdl_radio_button(int num, int x, int y, sdl_user *who, funcptr *stuff,
			sdl_widget**, int amnt, int indx);
		virtual ~sdl_radio_button();
		
		virtual void mouse_click(SDL_MouseButtonEvent *here);
		void set();
	protected:
		sdl_widget **options;
		int num_options;
		int me;
};

#endif