#ifndef _SDL_GRAPHIC_H_
#define _SDL_GRAPHIC_H_

#include <SDL/SDL.h>

class sdl_user;

class client;

enum SDL_GRAPH_TYPES
{
	GRAPH_IMG,
	GRAPH_PNG,
	GRAPH_STIL,
	GRAPH_LTIL,
	GRAPH_RTIL
};

class graphics_data;

class sdl_graphic
{
	public:
		sdl_graphic();
		sdl_graphic(int num, int x, int y, int type, client *who = 0);
		sdl_graphic(char *name, int x, int y, int type, client *who = 0);
		sdl_graphic(int x, int y, SDL_RWops *source, short *palette, int type);
		sdl_graphic(int x, int y, short *source, int type);
		sdl_graphic(int x, int y, int w, int h, int dummy);

		void do_load(int x, int y, short *source, int type);
		void do_load(int x, int y, SDL_RWops *source, short *palette, int type);
		void do_load(char *name, int x, int y, int type, client * who = 0);
		void do_load(int num, int x, int y, int type, client * who = 0);

		void delay_load(int x, int y, short *source, int type, sdl_user *orig);
		void delay_load(int x, int y, SDL_RWops *source, short *palette, int type, sdl_user *orig);
		void delay_load(char *name, int x, int y, int type, sdl_user *orig, client * who = 0);
		void delay_load(int num, int x, int y, int type, sdl_user *orig, client * who = 0);

		void disable_clear();
		Uint32 color(Uint8 r, Uint8 g, Uint8 b);
		void drawat(int x, int y, SDL_Surface *display);
		void draw(SDL_Surface *display);	//draws this tile onto display
		void mod_with(SDL_Surface *display);
		SDL_Surface *get_surf();
		~sdl_graphic();
		int getx();
		int gety();
		int getw();
		int geth();
		void erase();
		
		void setmw(int nmw);
		void setmh(int nmh);
		void setmx(int nmx);
		void setmy(int nmy);
		
		void make_bmp(char *name);
	private:
		SDL_Surface *img;
		SDL_Rect *pos;
		SDL_Rect *mask;
		Uint32 transp_color;
		bool cleanup;

		SDL_Surface *get_img(int num, client *who);
		SDL_Surface *get_png_image(char *name, client *who);
		void check_fix_png(char *buffer, int *size);
		SDL_Surface *get_image(SDL_RWops *buf);

		SDL_mutex *delay_mtx;
		bool delay_loading;
		int delay_load_id;
		sdl_user *loader;
};

#endif