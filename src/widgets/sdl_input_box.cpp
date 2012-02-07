#include "client.h"
#include "sdl_input_box.h"

#include "globals.h"

sdl_input_box::sdl_input_box(int num, int x, int y, client *who)
	: sdl_widget(num, x, y, who)
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

void sdl_input_box::clear()
{
	delete [] field;
	cursor_pos = 0;
	cursor_idx = 0;
	field_length = 1;
	field = new char[field_length];
	field[field_length - 1] = 0;	
}

void sdl_input_box::draw(SDL_Surface *display)
{	//TODO : implement password field masking (print '****' instead of 'asdf')
	lineage_font.draw(display, one->getx(), one->gety(), field, 0xFFFE);
	if (draw_cursor)
	{
		lineage_font.draw_cursor(display, one->getx() + cursor_pos, one->gety(), 0xFFFE);
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
		case SDLK_ESCAPE:	case SDLK_TAB:
		case SDLK_F1: case SDLK_F2: case SDLK_F3: case SDLK_F4:
		case SDLK_F5: case SDLK_F6: case SDLK_F7: case SDLK_F8: 
		case SDLK_F9: case SDLK_F10: case SDLK_F11: case SDLK_F12: 
		case SDLK_F13: case SDLK_F14: case SDLK_F15: 
		case SDLK_UP:	case SDLK_DOWN:
		case SDLK_LALT:	case SDLK_RALT:
		case SDLK_LCTRL:	case SDLK_RCTRL:
		case SDLK_INSERT:
		case SDLK_CAPSLOCK:	case SDLK_NUMLOCK:	case SDLK_SCROLLOCK:
		case SDLK_PAUSE: case SDLK_BREAK:	case SDLK_SYSREQ:	case SDLK_PRINT:
		case SDLK_HOME:	case SDLK_END:
		case SDLK_PAGEUP:	case SDLK_PAGEDOWN:
		case SDLK_LSHIFT:	case SDLK_RSHIFT:
		case SDLK_LMETA:	case SDLK_RMETA:
		case SDLK_LSUPER:	case SDLK_RSUPER:
			break;
		default:
			//TODO: check for valid character
			add_char(button->keysym.sym);
			cursor_idx++;
			cursor_pos += 6;
			break;
	}
}