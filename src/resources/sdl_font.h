#ifndef __SDL_FONT_H_
#define __SDL_FONT_H_

class sdl_user;

#define COLORKEY 255, 0, 255 //Your Transparent colour

class sdl_font
{
	public:
		sdl_font();
		virtual bool init(const char *name, sdl_user *who);
		virtual int draw(SDL_Surface *surf, int x, int y, char val, Uint32 color) = 0;
		virtual int draw(SDL_Surface *surf, int x, int y, const char *str, Uint32 color) = 0;
		virtual void draw_cursor(SDL_Surface *surf, int x, int y, Uint32 color) = 0;
	private:
		SDL_Surface *final;
};

#endif