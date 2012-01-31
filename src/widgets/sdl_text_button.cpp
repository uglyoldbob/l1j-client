#include "client.h"
#include "sdl_text_button.h"

sdl_text_button::sdl_text_button(const char *bla, int x, int y, client *who, funcptr *stuff)
	: sdl_plain_button(-1, x, y, who, stuff)
{
	xpos = x;
	ypos = y;
	set_display(bla);
}

void sdl_text_button::set_display(const char* disp)
{
	int size = strlen(disp);
	one = new sdl_graphic(xpos, ypos, size * 6, 12);
	lineage_font.draw(one->get_surf(), 0, 0, disp, 0xFFFE);
	
	two = new sdl_graphic(xpos, ypos, size*6, 12);
	lineage_font.draw(two->get_surf(), 0, 0, disp, two->color(255,255,0));
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