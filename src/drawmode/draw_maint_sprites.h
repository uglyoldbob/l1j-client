#ifndef __DRAW_MAINT_SPRITES_H_
#define __DRAW_MAINT_SPRITES_H_

#include "sdl_drawmode.h"

class draw_maint_sprites;
class load_ptr : public funcptr
{
	public:
		load_ptr(draw_maint_sprites *bla, int val);
		virtual void go();
	private:
		draw_maint_sprites *ref;
		int which;
};


class draw_maint_sprites : public sdl_drawmode
{
	public:
		draw_maint_sprites(sdl_user *self);
		~draw_maint_sprites();

		virtual void key_press(SDL_KeyboardEvent *button);

		virtual void mouse_click(SDL_MouseButtonEvent *here);
		virtual void mouse_to(SDL_MouseMotionEvent *to);
		virtual void mouse_from(SDL_MouseMotionEvent *from);
		virtual void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
		
		virtual bool quit_request();
		virtual void draw(SDL_Surface *display);
	private:
		int x, y;
		
		void redo_sprite();
};

#endif