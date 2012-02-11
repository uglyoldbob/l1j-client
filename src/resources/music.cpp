#include <stdio.h>

#include "globals.h"
#include "music.h"

bool music::initialized = false;

music::music()
{	
	printf("STUB Check for midi only\n");
	cur_music_name = 0;
	if (!initialized)
	{
		initialized = init();
	}
}

music::~music()
{
	if (initialized)
	{
		Mix_CloseAudio();
		initialized = false;
	}
	if (cur_music != 0)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(cur_music);
		cur_music = 0;
	}
	if (cur_music_name != 0)
	{
		delete [] cur_music_name;
		cur_music_name = 0;
	}
}

bool music::init()
{
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
	int audio_channels = 2;
	int audio_buffers = 4096;
	
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) 
	{
	    printf("Unable to open audio!\n");
		return false;
	}
	
	//Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
	cur_music = 0;
	
	cur_music_name = 0;
	cur_music_name = new char[5];
	strcpy(cur_music_name, "");

	return true;
}

void music::change_music(const char *name)
{
	if (initialized)
	{
		if (strcmp(name, cur_music_name))
		{
			if (cur_music != 0)
			{
				Mix_HaltMusic();
				Mix_FreeMusic(cur_music);
				cur_music = 0;
			}
			
			if (cur_music_name != 0)
			{
				delete [] cur_music_name;
				cur_music_name = 0;
			}
			
			cur_music_name = new char[strlen(name) + 1];
			char * real_name;
			real_name = new char[strlen(name) + strlen(lineage_dir) + 1];
			strcpy(real_name, lineage_dir);
			strcpy(cur_music_name, name);
			strcat(real_name, name);
			printf("Loading music %s\n", real_name);
			
			cur_music = Mix_LoadMUS(real_name);
			delete [] real_name;
			Mix_PlayMusic(cur_music, -1);
		}
	}
}