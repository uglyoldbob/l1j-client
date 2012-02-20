#include <SDL/SDL.h>
#include <string.h>

#include "client.h"
#include "files.h"
#include "globals.h"
#include "small_font.h"

small_font::small_font()
{
	final = 0;
}

bool small_font::init(const char *name, client *who)
{	//SMALL.FNT has a strange assortment of fonts (23 of them) 
	memset(convert, 0, 256);
	convert[' '] = 1;
	convert['$'] = 2;
	convert['*'] = 3;
	convert['+'] = 4;
	convert[','] = 5;
	convert['-'] = 6;
	convert['.'] = 7;
	convert['/'] = 8;
	convert['0'] = 9;
	convert['1'] = 10;
	convert['2'] = 11;
	convert['3'] = 12;
	convert['4'] = 13;
	convert['5'] = 14;
	convert['6'] = 15;
	convert['7'] = 16;
	convert['8'] = 17;
	convert['9'] = 18;
	convert[':'] = 19;
	convert['%'] = 20;	//?
	convert['G'] = 21;
	convert['M'] = 22;
	
	SDL_Surface *loser;

	//a grid of 6 wide by 4 tall characters (each char is 6 wide x 8 tall)
	final = SDL_CreateRGBSurface(SDL_SWSURFACE, 36, 32, 16,
		0x7C00, 0x03E0, 0x001F, 0);

	unsigned char *buffer;
	buffer = (unsigned char*)(who->getfiles->load_file(name, 0, FILE_REGULAR1, 0));
	
	short *new_buf = new short[6*8*23];	//23 tiles of 6x8
	
	for (int i = 0; i < (23*8); i++)	//number of rows to process
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
	loser = SDL_CreateRGBSurfaceFrom(new_buf, 6, 23*8, 16, 12,
		0x7C00, 0x03E0, 0x001F, 0);

	for (int i = 0; i < 23; i++)
	{
		SDL_Rect src;
		SDL_Rect dest;
		
		src.x = 0; src.y = 8*i;
		src.w = 6; src.h = 8;
		
		dest.x = (i % 6)*6;
		dest.y = (i / 6)*8;
		dest.w = 6;
		dest.h = 8;
		SDL_BlitSurface(loser, &src, final, &dest);
	}
	delete [] new_buf;
	delete [] buffer;
	
	SDL_FreeSurface(loser);
	return true;
}

int small_font::draw(SDL_Surface *surf, int x, int y, const char *str, Uint32 color)
{
	int total = 0;
	int len = strlen(str);
	for (unsigned int i = 0; i < len; i++)
	{
		total += draw(surf, x + total, y, str[i], color);
	}
	return total;
}

int small_font::draw(SDL_Surface *surf, int x, int y, char val, Uint32 color)
{
	val = convert[val];
	if (color == SDL_MapRGB(final->format, COLORKEY))
		color++;
	
	static SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 6, 8, 16,
		0x7C00, 0x03E0, 0x001F, 0);
	
	SDL_Rect src;
	SDL_Rect dest;
	
	src.x = (val % 6)*6;
	src.y = (val / 6)*8;
	src.w = 6;
	src.h = 8;
	
	SDL_BlitSurface(final, &src, temp, NULL);
	
	short *recolor = (short*)temp->pixels;
	for (int i = 0; i < (8*6); i++)
	{
		if (recolor[i] == 0)
		{
			recolor[i] = color;
		}
	}
	
	dest.x = x;
	dest.y = y;
	dest.w = 6;
	dest.h = 8;
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(final->format, COLORKEY) );
	SDL_BlitSurface(temp, NULL, surf, &dest);
	
	return 6;	//letter width is constant
}

void small_font::draw_cursor(SDL_Surface *surf, int x, int y, Uint32 color)
{
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	pos.w = 2;
	pos.h = 8;
	SDL_FillRect(surf, &pos, color);
}