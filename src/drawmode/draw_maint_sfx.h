#ifndef __DRAW_MAINT_SFX_H_
#define __DRAW_MAINT_SFX_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "sdl_drawmode.h"
#include "resources/music.h"

class draw_maint_sfx : public sdl_drawmode
{
	public:
		draw_maint_sfx(sdl_user *self);
		~draw_maint_sfx();
		
		virtual void key_press(SDL_KeyboardEvent *button);
		
		virtual void draw(SDL_Surface *display);
		virtual bool mouse_leave();	//is it ok for the mouse to leave?
		
		virtual bool quit_request();
	private:
		SDL_mutex *draw_mtx;
		int background;
		int channel;

		Mix_Chunk *sfx;
		
		void load_sfx(int i);
};

#endif