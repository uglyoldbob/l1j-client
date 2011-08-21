#ifndef __MUSIC_H_
#define __MUSIC_H_

#include "SDL.h"
#include "SDL_mixer.h"

class music
{
	public:
		music();
		int init();
		
		void start_music(const char *name);
	private:
		Mix_Music *cur_music;
};

#endif