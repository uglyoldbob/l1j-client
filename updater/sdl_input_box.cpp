#include "sdl_input_box.h"

sdl_input_box::sdl_input_box(int num, int x, int y, graphics_data *packs)
	: sdl_widget(num, x, y, packs)
{
	have_cursor = false;
}

void sdl_input_box::draw(SDL_Surface *display)
{
}

void sdl_input_box::cursor_on()
{
	have_cursor = true;
}

void sdl_input_box::cursor_off()
{
	have_cursor = false;
}