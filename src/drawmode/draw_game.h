#ifndef __DRAW_GAME_H_
#define __DRAW_GAME_H_

#include "sdl_drawmode.h"

class draw_game : public sdl_drawmode
{
	public:
		draw_game(sdl_user *self);
		~draw_game();
		
		virtual void key_press(SDL_KeyboardEvent *button);

		virtual void draw(SDL_Surface *display);
		virtual void mouse_click(SDL_MouseButtonEvent *here);
		virtual void mouse_to(SDL_MouseMotionEvent *to);
		virtual void mouse_from(SDL_MouseMotionEvent *from);
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
		
		virtual bool quit_request();
		
		void update_hpbar(int cur, int max);
		void update_mpbar(int cur, int max);
	private:
		SDL_mutex *draw_mtx;
		
		char *exp_str;
		char *hp_str;
		float hp_width;
		char *mp_str;
		float mp_width;
		char *ac_str;
		char *evil_str;
		char *weight_str;
		char *food_str;
};

#endif