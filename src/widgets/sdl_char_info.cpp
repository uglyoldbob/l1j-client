#include <SDL/SDL.h>

#include "client.h"
#include "globals.h"
#include "sdl_char_info.h"

sdl_char_info::sdl_char_info(client *who)
	: sdl_widget(0, 0, 0, who)
{
	fonts = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCCOLORKEY, 640, 480, 16, 
		0x7C00, 0x03E0, 0x001F, 0);
	SDL_SetColorKey(fonts, SDL_SRCCOLORKEY, SDL_MapRGB(fonts->format, COLORKEY) );
	SDL_FillRect(fonts, NULL, SDL_MapRGB(fonts->format, COLORKEY));
	
	data = 0;
}

void sdl_char_info::hand_info(lin_char_info *bla)
{
	data = bla;
	redraw();
}

bool sdl_char_info::contains(int x, int y)
{
	return false;
}

sdl_char_info::~sdl_char_info()
{
}

void sdl_char_info::redraw()
{
	SDL_FillRect(fonts, NULL, SDL_MapRGB(fonts->format, COLORKEY));
	if (data != 0)
	{
		char temp[25];
		lineage_font.draw(fonts, 167, 297, data->name, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		lineage_font.draw(fonts, 167, 316, data->pledge, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->char_type);
		lineage_font.draw(fonts, 167, 340, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->alignment);
		lineage_font.draw(fonts, 167, 360, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->hp);
		lineage_font.draw(fonts, 167, 397, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->mp);
		lineage_font.draw(fonts, 167, 419, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->ac);
		lineage_font.draw(fonts, 167, 441, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->level);
		lineage_font.draw(fonts, 403, 315, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		
		sprintf(temp, "%d", data->str);
		lineage_font.draw(fonts, 403, 350, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->dex);
		lineage_font.draw(fonts, 403, 369, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->con);
		lineage_font.draw(fonts, 403, 387, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->wis);
		lineage_font.draw(fonts, 404, 405, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->cha);
		lineage_font.draw(fonts, 403, 423, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
		sprintf(temp, "%d", data->intl);
		lineage_font.draw(fonts, 404, 442, temp, 
			SDL_MapRGB(fonts->format, 0xFF, 0xFF, 0xFE));
	}
}

void sdl_char_info::draw(SDL_Surface *display)
{
	if (visible)
	{
		SDL_BlitSurface(fonts, NULL, display, NULL);
	}
}

