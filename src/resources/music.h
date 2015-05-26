#ifndef __MUSIC_H_
#define __MUSIC_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
class sdl_user;

class music
{
	public:
		music();
		~music();
		bool init();
		
		void change_music(const char *name);
		void load_sfx(int i);
		int play_sfx(int i, sdl_user *user);
		void stop_sfx(int channel);
	private:
		Mix_Music *cur_music;
		char *cur_music_name;
		static bool initialized;
};

#endif

//text/music.def contains what music to play
//text/lineage.mcf appears to have the same content but slightly different structure (but is definitely OLDER)
	//format
	//; is a commented line
	
	//System Setting Begin
		//Login, Prince, Knight, Elf, Magician, Darkelf, DragonKnight, illusionist, Credits,
			//Hunting, PlayrtKilling, PledgeWar, PledgeWin, PledgeLose, SiegeWar, SiegeWin, SiegeOccupy, SiegeFail,
			//Death, Fanfare, Wedding
		//SystemMenu Exit
	
	//"LOC" map_number start_x start_y end_x end_y (start_x start_y end_x end_y are optional)
		//if the x/y coordinate are missing then it means the entire map
		//multiple entries are seperated by a "\"
	//"MUSIC" then number
	//"INTERVAL" time for each piece of music (not present if only 1 music)
	//unknown parameters
	//SPRITE, SPAWN, BATTLE, DEATH, TWIN_SPRITE1, TWIN_SPRITE2
//text/envsound.def
	//defines properties for environment sounds based on location and time
	//some define that the sound effect changes each time and the order in which to play
	//lots of properties define here
//text/charsound.def
	//links a png image with a sound
//text/equipsound.def
	//defines what sound to use then the items they go with
