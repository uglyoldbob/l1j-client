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

enum server_packet_types
{	//the numbers will eventually be deleted
	SERVER_VERSIONS = 10,
	SERVER_DISCONNECT = 18,
	SERVER_LOGIN = 21,
	SERVER_CHAR_DELETE = 33,
	SERVER_KEY = 65,
	SERVER_NEWS = 90,
	SERVER_LOGIN_CHAR = 99,
	SERVER_CREATE_STAT = 106,
	SERVER_NUM_CHARS = 113
};

enum client_packet_types
{	//the numbers will eventually be deleted
	CLIENT_LOGIN = 12,
	CLIENT_DELETE_CHAR = 34,
	CLIENT_CLICK = 43,
	CLIENT_VERSION = 71,
	CLIENT_CREATE_CHAR
};

enum lin_char_type
{
	CLASS_ROYAL,
	CLASS_KNIGHT,
	CLASS_ELF,
	CLASS_WIZARD,
	CLASS_DARKELF,
	CLASS_DRAGONKNIGHT,
	CLASS_ILLUSIONIST
};

enum lin_stat_type
{
	STAT_STR,
	STAT_CON,
	STAT_DEX,
	STAT_WIS,
	STAT_CHA,
	STAT_INT
};

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
	char max_str;
	char max_dex;
	char max_con;
	char max_wis;
	char max_cha;
	char max_int;
};

struct sdl_graphic
{
	SDL_Surface *img;
	SDL_Rect *pos;
	SDL_Rect *mask;
	bool cleanup;
};

struct graphics_data
{
	pack *tilepack;
	pack **spritepack;
	int num_sprite_pack;
};

lin_char_info *make_lin_char_info(int char_type, int gender);
SDL_Surface *get_image(const char *name, pack *source);
SDL_Surface *get_png_image(int num, pack **source);
SDL_Surface *get_img(int num, pack **source);	//loads raw image data
SDL_Surface *get_image(SDL_RWops *buf);

#endif