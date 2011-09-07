#ifndef __MUSIC_H_
#define __MUSIC_H_

#include "SDL.h"
#include "SDL_mixer.h"

class music
{
	public:
		music();
		~music();
		bool init();
		
		void change_music(const char *name);
	private:
		Mix_Music *cur_music;
		char *cur_music_name;
		static bool initialized;
};

#endif