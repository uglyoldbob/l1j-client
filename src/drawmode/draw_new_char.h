#ifndef __DRAW_NEWCHAR_H_
#define __DRAW_NEWCHAR_H_

#include "funcptr.h"
#include "globals.h"
#include "sdl_drawmode.h"

enum dnc_funcs
{
	DNC_FUNC_NEWCHAR,
	DNC_FUNC_SETGENDER,
	DNC_FUNC_STATUP,
	DNC_FUNC_STATDOWN,
	DNC_FUNC_CANCEL,
	DNC_FUNC_SUBMIT
};

class draw_new_char;
class dnc_ptr : public funcptr
{
	public:
		dnc_ptr(draw_new_char *bla, enum dnc_funcs sel);
		dnc_ptr(draw_new_char *bla, enum dnc_funcs sel, int val);
		virtual void go();
	private:
		int which;
		enum dnc_funcs what;
		draw_new_char *ref;
};

class draw_new_char : public sdl_drawmode
{
	public:
		draw_new_char(graphics_data *stuff, sdl_user *self);
		~draw_new_char();
		
		virtual void draw(SDL_Surface *display);
		
		virtual void mouse_click(SDL_MouseButtonEvent *here);
		virtual void mouse_to(SDL_MouseMotionEvent *to);
		virtual void mouse_from(SDL_MouseMotionEvent *from);
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?

		void set_gender(int bla);
		void init_new_char(int type);
		void update_char();
		void stat_up(int stat);
		void stat_down(int stat);
		void cancel();
		void submit();
		lin_char_info* get_char();
	private:
		int graphic;
		
		unsigned char gender;
		unsigned char char_type;
		
		void reset_stats();
		char add_str;
		char add_dex;
		char add_con;
		char add_wis;
		char add_cha;
		char add_int;
		char points_remaining;
};

#endif