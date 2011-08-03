#ifndef __SDL_WIDGET_H_
#define __SDL_WIDGET_H_

#include <SDL.h>

class sdl_widget
{
	public:
		sdl_widget(SDL_Rect *ps, SDL_Rect *msk, SDL_Surface *data);
		~sdl_widget();
		void draw(SDL_Surface *display);

		friend class sdl_user;
	private:
		int visible;	//determines if the widget should be drawn
		SDL_Surface *img;
		SDL_Rect *pos;
		SDL_Rect *mask;
};

#endif