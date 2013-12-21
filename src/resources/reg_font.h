#ifndef __REG_FONT_H_
#define __REG_FONT_H_

#include "sdl_font.h"

#define COLORKEY 255, 0, 255 //Your Transparent colour

class reg_font : public sdl_font
{
	public:
		reg_font();
		~reg_font();
		virtual bool init(const char *name, sdl_user *who);
		virtual int draw(SDL_Surface *surf, int x, int y, char val, Uint32 color);
		virtual int draw(SDL_Surface *surf, int x, int y, const char *str, Uint32 color);
		virtual void draw_cursor(SDL_Surface *surf, int x, int y, Uint32 color);
	private:
		SDL_Surface *final;
		SDL_Surface *temp;
};

#endif