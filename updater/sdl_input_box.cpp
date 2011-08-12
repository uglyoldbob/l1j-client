#include "sdl_input_box.h"

#include "globals.h"

sdl_input_box::sdl_input_box(int num, int x, int y, graphics_data *packs)
	: sdl_widget(num, x, y, packs)
{
	have_cursor = false;
	draw_cursor = false;
	cursor_pos = 0;
	cursor_idx = 0;
	memset(field, 0, MAX_LENGTH);
}

void sdl_input_box::draw(SDL_Surface *display)
{
	lineage_font.draw(display,  one->pos->x, one->pos->y, field, 0xFFFE);
	if (draw_cursor)
	{
		strcpy(field, "1234567890123");
		lineage_font.draw_cursor(display, one->pos->x + cursor_pos, one->pos->y, 0xFFFE);
	}
	cursor_toggle();
}

void sdl_input_box::cursor_toggle()
{
	static Uint32 change = 0;
	if (have_cursor)
	{
		if (SDL_GetTicks() >= change)
		{
			draw_cursor = !draw_cursor;
			change += 333;
		}
	}
}

void sdl_input_box::cursor_on()
{
	have_cursor = true;
}

void sdl_input_box::cursor_off()
{
	have_cursor = false;
}