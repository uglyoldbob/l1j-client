#include "draw_char_sel.h"

#include "globals.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_char_info.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_widget.h"

void selected_char(void *arg)
{
	sdl_user *bob = (sdl_user*)arg;
	printf("Selected something to do with a char slot");
	
}

draw_char_sel::draw_char_sel(graphics_data *stuff, sdl_user *self)
	: sdl_drawmode(stuff, self)
{	
	draw_mtx = SDL_CreateMutex();
	pg = new prepared_graphics;
	pg->num_pg = 1;
	pg->pg = new prepared_graphic[1];
	
	//1c1
	//0
	
	pg->pg[0].surf = get_png_image(815, graphx->spritepack);
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = false;
	pg->ready = true;
	
	num_widgets = 12;
	widgets = new sdl_widget*[num_widgets];
	
	//character select animating buttons
	widgets[0] = new sdl_animate_button(0xf4, 0x013, 0, graphx, 0, 0);
	widgets[1] = new sdl_animate_button(0xf4, 0x0b0, 0, graphx, 0, 0);
	widgets[2] = new sdl_animate_button(0xf4, 0x14d, 0, graphx, 0, 0);
	widgets[3] = new sdl_animate_button(0xf4, 0x1ea, 0, graphx, 0, 0);
	widgets[3]->cursor_on();
	widget_key_focus = 3;
	
	widgets[4] = new sdl_plain_button(0x6e5, 0x0f7, 0x10b, graphx, 0, 0);	//left arrow
	widgets[5] = new sdl_plain_button(0x6e7, 0x16c, 0x10b, graphx, 0, 0);	//right arrow
	widgets[6] = new sdl_plain_button(0x134, 0x20d, 0x1b5, graphx, 0, 0);	//delete
	widgets[7] = new sdl_plain_button(0x336, 0x20d, 0x19a, graphx, 0, 0);	//cancel
	widgets[8] = new sdl_plain_button(0x334, 0x20d, 0x185, graphx, selected_char, this);	//login
	
	widgets[9] = new sdl_widget(0x6e9, 0x127, 0x10f, graphx);
	widgets[10] = new sdl_widget(0x6eb, 0x146, 0x10f, graphx);
	widgets[11] = new sdl_char_info(graphx);
}

draw_char_sel::~draw_char_sel()
{
	if (pg != 0)
		delete pg;
	if (widgets != 0)
		delete [] widgets;
}

void draw_char_sel::set_login_char(int num, lin_char_info *data)
{
	int type = (data->char_type * 2) + data->gender;
	while (SDL_mutexP(draw_mtx) == -1) {};
	printf("Registering character %s\n", data->name);
	if (num < 4)
	{
		sdl_animate_button *chars[4];
		chars[0] = (sdl_animate_button*)widgets[0];
		chars[1] = (sdl_animate_button*)widgets[1];
		chars[2] = (sdl_animate_button*)widgets[2];
		chars[3] = (sdl_animate_button*)widgets[3];
		
		chars[num]->set_info(data);
	}
	SDL_mutexV(draw_mtx);
}

bool draw_char_sel::mouse_leave()
{
	return false;
}

void draw_char_sel::mouse_click(SDL_MouseButtonEvent *here)
{
	if (num_widgets > 0)
	{
		int index = get_widget(here->x, here->y);
		sdl_animate_button *chars[4];
		sdl_char_info *stuff;
		stuff = (sdl_char_info*)widgets[11];
		chars[0] = (sdl_animate_button*)widgets[0];
		chars[1] = (sdl_animate_button*)widgets[1];
		chars[2] = (sdl_animate_button*)widgets[2];
		chars[3] = (sdl_animate_button*)widgets[3];
		switch (index)
		{
			case 0:
				stuff->hand_info(chars[0]->get_info());
				chars[0]->animate(true);
				chars[1]->animate(false);
				chars[2]->animate(false);
				chars[3]->animate(false);
				break;
			case 1:
				stuff->hand_info(chars[1]->get_info());
				chars[0]->animate(false);
				chars[1]->animate(true);
				chars[2]->animate(false);
				chars[3]->animate(false);
				break;
			case 2:
				stuff->hand_info(chars[2]->get_info());
				chars[0]->animate(false);
				chars[1]->animate(false);
				chars[2]->animate(true);
				chars[3]->animate(false);
				break;
			case 3:
				stuff->hand_info(chars[3]->get_info());
				chars[0]->animate(false);
				chars[1]->animate(false);
				chars[2]->animate(false);
				chars[3]->animate(true);
				break;
			default:
				break;
		}
		if (index != -1)
		{
			widgets[index]->mouse_click(here);
		}
	}
}

void draw_char_sel::mouse_to(SDL_MouseMotionEvent *to)
{
	int which = get_widget(to->x, to->y);
	if (which >= 0)
	{
		widgets[which]->mouse_to(to);
	}
}

void draw_char_sel::mouse_from(SDL_MouseMotionEvent *from)
{
	int which = get_widget(from->x, from->y);
	if (which >= 0)
	{
		widgets[which]->mouse_from(from);
	}
}

void draw_char_sel::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
{	//TODO: handle click and drag movable widgets
	if (num_widgets > 0)
	{
		int from_w = get_widget(from->x, from->y);
		int to_w = get_widget(to->x, to->y);
		if ((from_w != -1) && (to_w != -1))
		{
			if (from_w != to_w)
			{	//only send events if the widgets are different
				mouse_from(from);
				mouse_to(to);
			}
		}
		if ((from_w == -1) && (to_w != -1))
		{
			mouse_to(to);
		}
		if ((from_w != -1) && (to_w == -1))
		{
			mouse_from(from);
		}
	}
}

void draw_char_sel::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	sdl_drawmode::draw(display);
	SDL_mutexV(draw_mtx);	
}