#ifndef __DRAW_CHARSEL_H_
#define __DRAW_CHARSEL_H_

class lin_char_info;
#include "funcptr.h"
#include "sdl_drawmode.h"

class draw_char_sel;

enum dcs_funcs
{
	DCS_FUNC_SELCHAR,
	DCS_FUNC_PREVPAGE,
	DCS_FUNC_NEXTPAGE,
	DCS_FUNC_DELCHAR
};

class dcs_ptr : public funcptr
{
	public:
		dcs_ptr(draw_char_sel *bla, enum dcs_funcs sel);
		dcs_ptr(draw_char_sel *bla, enum dcs_funcs sel, int* val);
		virtual void go();
	private:
		int *which;
		enum dcs_funcs what;
		draw_char_sel *ref;
};

class draw_char_sel : public sdl_drawmode
{
	public:
		draw_char_sel(graphics_data *stuff, sdl_user *self);
		~draw_char_sel();
		
		virtual void draw(SDL_Surface *display);
		virtual void mouse_click(SDL_MouseButtonEvent *here);
		virtual void mouse_to(SDL_MouseMotionEvent *to);
		virtual void mouse_from(SDL_MouseMotionEvent *from);
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
		
		void select_char();
		void get_login_chars();
		void delete_char(int which);
		void do_delete();
		void nextpage();
		void prevpage();
	private:
		SDL_mutex *draw_mtx;
		int cur_char_slot;
		int page_num;
		int ready;
		
		int char_deleting;
};

#endif