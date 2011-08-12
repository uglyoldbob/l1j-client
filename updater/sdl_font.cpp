#include <SDL.h>
#include <string.h>

#include "globals.h"
#include "sdl_font.h"

sdl_font::sdl_font()
{
}

bool sdl_font::init(const char *name)
{	//eng.fnt has from 0x20 - 0x7f
	char *real_name = new char[strlen(name) + strlen(lineage_dir) + 1];
	strcpy(real_name, lineage_dir);
	strcat(real_name, name);

	SDL_Surface *loser;

	final = SDL_CreateRGBSurface(SDL_SWSURFACE, 96, 72, 16,
		0x7C00, 0x03E0, 0x001F, 0);
			
	SDL_RWops *file;	//TODO: replace this with regular C FILE * handling code
	file = SDL_RWFromFile(real_name, "r");
	unsigned char *buffer;
	buffer = new unsigned char[1200];	//TODO Find filesize before loading it
	SDL_RWread(file, buffer, 1, 1140);
	
	short *new_buf = new short[6*12*95];
	
	for (int i = 0; i < (95*12); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if ((buffer[i] & (0x80>>j)) != 0)
			{
				new_buf[i*6 + j] = 0;
			}
			else
			{
				new_buf[i*6 + j] = SDL_MapRGB(final->format, COLORKEY) & 0xFFFF;
			}
		}			
	}
	loser = SDL_CreateRGBSurfaceFrom(new_buf, 6, 95*12, 16, 12,
		0x7C00, 0x03E0, 0x001F, 0);

	for (int i = 0; i < 95; i++)
	{
		SDL_Rect src;
		SDL_Rect dest;
		
		src.x = 0; src.y = 12*i;
		src.w = 6; src.h = 12;
		
		dest.x = (i % 0x10)*6;
		dest.y = (i / 0x10)*12;
		dest.w = 6;
		dest.h = 12;
		SDL_BlitSurface(loser, &src, final, &dest);
	}
	delete [] new_buf;
	delete [] buffer;
	SDL_FreeSurface(loser);
}

int sdl_font::draw(SDL_Surface *surf, int x, int y, const char *str, Uint32 color)
{
	int total = 0;
	for ( int i = 0; i < strlen(str); i++)
	{
		total += draw(surf, x + total, y, str[i], color);
	}
	return total;
}

int sdl_font::draw(SDL_Surface *surf, int x, int y, char val, Uint32 color)
{
	if (val < 0x20)
		return 0;
	if (color == SDL_MapRGB(final->format, COLORKEY))
		color++;
	
	static SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 6, 12, 16,
		0x7C00, 0x03E0, 0x001F, 0);
	
	SDL_Rect src;
	SDL_Rect dest;
	val -= 0x20;	
	src.x = (val % 0x10)*6;
	src.y = (val / 0x10)*12;
	src.w = 6;
	src.h = 12;
	
	SDL_BlitSurface(final, &src, temp, NULL);
	
	short *recolor = (short*)temp->pixels;
	for (int i = 0; i < (12*6); i++)
	{
		if (recolor[i] == 0)
		{
			recolor[i] = color;
		}
	}
	
	dest.x = x;
	dest.y = y;
	dest.w = 6;
	dest.h = 12;
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(final->format, COLORKEY) );
	SDL_BlitSurface(temp, NULL, surf, &dest);
	
	return 6;	//letter width is constant
}

void sdl_font::draw_cursor(SDL_Surface *surf, int x, int y, Uint32 color)
{
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	pos.w = 2;
	pos.h = 12;
	SDL_FillRect(surf, &pos, color);
}