#include <stdio.h>

#include "globals.h"
#include "music.h"

music::music()
{	
	printf("STUB Check for midi only\n");
}

int music::init()
{
	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
	int audio_channels = 2;
	int audio_buffers = 4096;
	
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) 
	{
	    printf("Unable to open audio!\n");
		return 1;
	}
	
	//Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
	start_music("sound/music0.mp3");

	printf("STUB Initialize music\n");
	return 1;
}

void music::start_music(const char *name)
{
	char real_name[256];
	if (strlen(name) + strlen(lineage_dir) >= 256)
	{
		return;
	}
	strcpy(real_name, lineage_dir);
	strcat(real_name, name);
	printf("Loading music %s\n", real_name);
	
	cur_music = Mix_LoadMUS(real_name);
	Mix_PlayMusic(cur_music, 0);

}