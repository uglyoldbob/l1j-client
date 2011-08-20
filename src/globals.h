#ifndef __GLOBALS_H_
#define __GLOBALS_H_

#include <SDL.h>
#include "SDL_endian.h"
#include "SDL_image.h"

#include "resources/pack.h"
#include "resources/sdl_font.h"
#include "resources/table.h"

extern char *lineage_dir;	//string to hold the absolute path to the lineage directory
extern sdl_font lineage_font;

//these are used so that the correct byte order is used
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SWAP16(X)    (X)
#define SWAP32(X)    (X)
#else
#define SWAP16(X)    SDL_Swap16(X)
#define SWAP32(X)    SDL_Swap32(X)
#endif

struct lin_char_info
{
	char *name;
	char *pledge;
	char char_type;
	char gender;
	short alignment;
	short hp;
	short mp;
	char ac;
	char level;
	char str;
	char dex;
	char con;
	char wis;
	char cha;
	char intl;
};

struct sdl_graphic
{
	SDL_Surface *img;
	SDL_Rect *pos;
	SDL_Rect *mask;
};

struct graphics_data
{
	pack *tilepack;
	pack **spritepack;
	int num_sprite_pack;
};

SDL_Surface *get_image(const char *name, pack *source);
SDL_Surface *get_png_image(int num, pack **source);
SDL_Surface *get_img(int num, pack **source);	//loads raw image data
SDL_Surface *get_image(SDL_RWops *buf);

#endif