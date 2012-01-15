#include "draw_char_sel.h"

#include "client.h"
#include "globals.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_char_info.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_widget.h"

dcs_ptr::dcs_ptr(draw_char_sel *bla, enum dcs_funcs sel)
{
	ref = bla;
	what = sel;
}

dcs_ptr::dcs_ptr(draw_char_sel *bla, enum dcs_funcs sel, int* val)
{
	ref = bla;
	what = sel;
	which = val;
}

void dcs_ptr::go()
{
	switch(what)
	{
		case DCS_FUNC_SELCHAR:
			ref->select_char();
			break;
		case DCS_FUNC_PREVPAGE:
			ref->prevpage();
			break;
		case DCS_FUNC_NEXTPAGE:
			ref->nextpage();
			break;
		case DCS_FUNC_DELCHAR:
			ref->delete_char(*which);
			break;
		default:
			break;
	}
}

draw_char_sel::draw_char_sel(sdl_user *self)
	: sdl_drawmode(self)
{	
	draw_mtx = SDL_CreateMutex();
	ready = false;
	cur_char_slot = -1;
	page_num = 0;
	pg = new prepared_graphics;
	pg->num_pg = 1;
	pg->pg = new prepared_graphic[1];
	
	//1c1
	//0
	
	pg->pg[0].surf = get_png_image(815, owner->game);
		//TODO Replace with 105.img
		//or 1763.img
		//1764.img locked
		//1769-1772 - numeric indicators
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = false;
	pg->ready = true;
	
	num_widgets = 12;
	widgets = new sdl_widget*[num_widgets];
	
	//character select animating buttons
	widgets[0] = new sdl_animate_button(0xf4, 0x013, 0, owner->game, 0);
	widgets[1] = new sdl_animate_button(0xf4, 0x0b0, 0, owner->game, 0);
	widgets[2] = new sdl_animate_button(0xf4, 0x14d, 0, owner->game, 0);
	widgets[3] = new sdl_animate_button(0xf4, 0x1ea, 0, owner->game, 0);
	widgets[0]->set_key_focus(true);
	widgets[1]->set_key_focus(true);
	widgets[2]->set_key_focus(true);
	widgets[3]->set_key_focus(true);
	widgets[0]->cursor_on();
	widget_key_focus = 0;
	
	widgets[4] = new sdl_plain_button(0x6e5, 0x0f7, 0x10b, owner->game, 
		(funcptr*)new dcs_ptr(this, DCS_FUNC_PREVPAGE));	//left arrow
	widgets[5] = new sdl_plain_button(0x6e7, 0x16c, 0x10b, owner->game, 
		(funcptr*)new dcs_ptr(this, DCS_FUNC_NEXTPAGE));	//left arrow
	widgets[6] = new sdl_plain_button(0x334, 0x20d, 0x185, owner->game, 
		(funcptr*)new dcs_ptr(this, DCS_FUNC_SELCHAR));	//login
	widgets[7] = new sdl_plain_button(0x336, 0x20d, 0x19a, owner->game, 0);	//cancel
	widgets[8] = new sdl_plain_button(0x134, 0x20d, 0x1b5, owner->game, 
		(funcptr*)new dcs_ptr(this, DCS_FUNC_DELCHAR, &cur_char_slot));	//delete
	widgets[4]->set_key_focus(true);
	widgets[5]->set_key_focus(true);
	widgets[6]->set_key_focus(true);
	widgets[7]->set_key_focus(true);
	widgets[8]->set_key_focus(true);
	
	widgets[9] = new sdl_widget(0x6e9, 0x127, 0x10f, owner->game);
	widgets[10] = new sdl_widget(0x6eb, 0x146, 0x10f, owner->game);
	widgets[11] = new sdl_char_info(owner->game);

	if (owner->game->check_login_chars() != 0)
	{
		get_login_chars();
	}
}

draw_char_sel::~draw_char_sel()
{
}

void draw_char_sel::nextpage()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (page_num < 1)
	{
		page_num++;
		get_login_chars();
	}
	SDL_mutexV(draw_mtx);
}

void draw_char_sel::prevpage()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (page_num > 0 )
	{
		page_num--;
		get_login_chars();
	}
	SDL_mutexV(draw_mtx);
}

void draw_char_sel::delete_char(int which)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	lin_char_info **data = owner->game->get_login_chars();
	switch(which)
	{
		case -1:
			break;
		default:
			char_deleting = which;
			//TODO: implement timer for character deletion
			owner->game->send_packet("csdd", CLIENT_DELETE_CHAR, 
				data[which]->name, 0, 0);
			break;
	}
	SDL_mutexV(draw_mtx);
}

void draw_char_sel::do_delete()
{
	lin_char_info **data = owner->game->get_login_chars();
	sdl_animate_button **chars;
	chars = (sdl_animate_button**)&widgets[0];
	delete data[char_deleting];
	data[char_deleting] = 0;
	chars[char_deleting]->set_info(0);
	cur_char_slot = -1;
	chars[0]->animate(false);
	chars[1]->animate(false);
	chars[2]->animate(false);
	chars[3]->animate(false);
}

void draw_char_sel::select_char()
{
	if (cur_char_slot != -1)
	{
		sdl_animate_button **chars;
		chars = (sdl_animate_button**)&widgets[0];
		lin_char_info *bob;
		if (chars[cur_char_slot]->char_info == 0)
		{
			owner->change_drawmode(DRAWMODE_NEWCHAR);
		}
		else
		{
			bob = chars[cur_char_slot]->get_info();			
			owner->game->send_packet("csdd", CLIENT_USE_CHAR, bob->name, 0, 0);
		}
	}
}

void draw_char_sel::get_login_chars()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	lin_char_info **data = owner->game->get_login_chars();
	sdl_animate_button **chars;
	chars = (sdl_animate_button**)&widgets[0];
	for (int i = 0; i < 4; i++)
	{
		chars[i]->set_info(data[i + (page_num*4)]);
	}
	ready = true;
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
				cur_char_slot = 0;
				chars[0]->animate(true);
				chars[1]->animate(false);
				chars[2]->animate(false);
				chars[3]->animate(false);
				break;
			case 1:
				stuff->hand_info(chars[1]->get_info());
				cur_char_slot = 1;
				chars[0]->animate(false);
				chars[1]->animate(true);
				chars[2]->animate(false);
				chars[3]->animate(false);
				break;
			case 2:
				stuff->hand_info(chars[2]->get_info());
				cur_char_slot = 2;
				chars[0]->animate(false);
				chars[1]->animate(false);
				chars[2]->animate(true);
				chars[3]->animate(false);
				break;
			case 3:
				stuff->hand_info(chars[3]->get_info());
				cur_char_slot = 3;
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
	if (ready)
		sdl_drawmode::draw(display);
	SDL_mutexV(draw_mtx);	
}