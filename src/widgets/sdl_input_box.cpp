#include "sdl_input_box.h"

#include "globals.h"

sdl_input_box::sdl_input_box(int num, int x, int y, graphics_data *packs)
	: sdl_widget(num, x, y, packs)
{
	key_focus = false;
	draw_cursor = false;
	cursor_pos = 0;
	cursor_idx = 0;
	field_length = 1;
	field = new char[field_length];
	field[field_length - 1] = 0;
}

sdl_input_box::~sdl_input_box()
{
	delete [] field;
}

const char *sdl_input_box::get_str()
{
	return field;
}

void sdl_input_box::draw(SDL_Surface *display)
{	//TODO : implement password field masking (print '****' instead of 'asdf')
	lineage_font.draw(display, one->pos->x, one->pos->y, field, 0xFFFE);
	if (draw_cursor)
	{
		lineage_font.draw_cursor(display, one->pos->x + cursor_pos, one->pos->y, 0xFFFE);
	}
	cursor_toggle();
}

void sdl_input_box::cursor_toggle()
{
	static Uint32 change = 0;
	if (key_focus)
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
	key_focus = true;
}

void sdl_input_box::cursor_off()
{
	key_focus = false;
	draw_cursor = false;
}

void sdl_input_box::add_char(char dat)
{
	char *temp;
	temp = new char[field_length + 1];
	int i;
	for (i = 0; i < cursor_idx; i++)
	{
		temp[i] = field[i];
	}
	temp[i++] = dat;
	for (; i < (field_length + 1); i++)
	{
		temp[i] = field[i-1];
	}
	delete [] field;
	field_length++;
	field = temp;
}

void sdl_input_box::key_press(SDL_KeyboardEvent *button)
{
	switch(button->keysym.sym)
	{
		case SDLK_LEFT:
			if (cursor_idx > 0)
			{
				cursor_idx--;
				cursor_pos -= 6;
			}
			break;
		case SDLK_RIGHT:
			if (cursor_idx < (field_length-1))
			{
				cursor_idx++;
				cursor_pos += 6;
			}
			break;
		case SDLK_BACKSPACE:
			if (cursor_idx > 0)
			{
				for (int i = cursor_idx; i < field_length; i++)
				{
					field[i-1] = field[i];
				}
				cursor_idx--;
				cursor_pos -= 6;
				field_length--;
			}
			break;
		case SDLK_DELETE:
			if (cursor_idx < (field_length-1))
			{
				for (int i = cursor_idx+1; i < field_length; i++)
				{
					field[i-1] = field[i];
				}
				field_length--;
			}
			break;
		case SDLK_CLEAR:
		case SDLK_RETURN:
		case SDLK_ESCAPE:
		case SDLK_UP:
		case SDLK_DOWN:		
			break;
		default:
			//TODO: check for valid character
			add_char(button->keysym.sym);
			cursor_idx++;
			cursor_pos += 6;
			break;
	}
}