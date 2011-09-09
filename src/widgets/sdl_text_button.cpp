#include "sdl_text_button.h"

sdl_text_button::sdl_text_button(const char *bla, int x, int y, graphics_data *packs, funcptr *stuff)
	: sdl_plain_button(-1, x, y, packs, stuff)
{
	xpos = x;
	ypos = y;
	set_display(bla);
}

void sdl_text_button::set_display(const char* disp)
{
	int size = strlen(disp);
	one = new sdl_graphic;
	one->img = SDL_CreateRGBSurface(SDL_SWSURFACE, size*6, 12, 16,
		0x7C00, 0x03E0, 0x001F, 0);
	one->pos = make_sdl_rect(xpos, ypos, size*6, 12);
	one->mask = make_sdl_rect(0, 0, size*6, 12);
	one->cleanup = false;
	SDL_SetColorKey(one->img, SDL_SRCCOLORKEY, SDL_MapRGB(one->img->format, 1,1,1) );
	SDL_FillRect(one->img, NULL, SDL_MapRGB(one->img->format, 1,1,1));
	lineage_font.draw(one->img, 0, 0, disp, 0xFFFE);
	
	two = new sdl_graphic;
	two->img = SDL_CreateRGBSurface(SDL_SWSURFACE, size*6, 12, 16,
		0x7C00, 0x03E0, 0x001F, 0);
	two->pos = make_sdl_rect(xpos, ypos, size*6, 12);
	two->mask = make_sdl_rect(0, 0, size*6, 12);
	two->cleanup = false;
	SDL_SetColorKey(two->img, SDL_SRCCOLORKEY, SDL_MapRGB(two->img->format, 1,1,1) );
	SDL_FillRect(two->img, NULL, SDL_MapRGB(two->img->format, 1,1,1));
	lineage_font.draw(two->img, 0, 0, disp, SDL_MapRGB(two->img->format, 255,255,0));
	visible = true;
}

sdl_text_button::~sdl_text_button()
{
}

void sdl_text_button::draw(SDL_Surface *display)
{
	sdl_plain_button::draw(display);
//	lineage_font.draw(display, xpos, ypos, "TEST", 0xFFFE);
}