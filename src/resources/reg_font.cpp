#include <SDL/SDL.h>
#include <string.h>

#include "files.h"
#include "globals.h"
#include "reg_font.h"
#include "sdl_user.h"

reg_font::reg_font()
{
	final = 0;
	temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 6, 12, 16,
		0x7C00, 0x03E0, 0x001F, 0);
}

reg_font::~reg_font()
{
	if (final != 0)
	{
		SDL_FreeSurface(final);
		final = 0;
	}
	if (temp != 0)
	{
		SDL_FreeSurface(temp);
		temp = 0;
	}
}

bool reg_font::init(const char *name, sdl_user *who)
{	//eng.fnt has from 0x20 - 0x7f
	SDL_Surface *loser;

	final = SDL_CreateRGBSurface(SDL_SWSURFACE, 96, 72, 16,
		0x7C00, 0x03E0, 0x001F, 0);

	unsigned char *buffer;
	buffer = (unsigned char*)(who->getfiles->load_file(name, 0, FILE_REGULAR1, 0));
	
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
	new_buf = 0;
	delete [] buffer;
	buffer = 0;
	SDL_FreeSurface(loser);
	return true;
}

int reg_font::draw(SDL_Surface *surf, int x, int y, const char *str, Uint32 color)
{
	int total = 0;
	int len = strlen(str);
	for (unsigned int i = 0; i < len; i++)
	{
		total += draw(surf, x + total, y, str[i], color);
	}
	return total;
}

int reg_font::draw(SDL_Surface *surf, int x, int y, char val, Uint32 color)
{
	if (val < 0x20)
		return 0;
	if (color == SDL_MapRGB(final->format, COLORKEY))
		color++;
	
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

void reg_font::draw_cursor(SDL_Surface *surf, int x, int y, Uint32 color)
{
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	pos.w = 2;
	pos.h = 12;
	SDL_FillRect(surf, &pos, color);
}