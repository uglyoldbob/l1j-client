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

//pack data is the same for all clients thanks to the briefcase object
extern pack *textpack;
extern pack *tilepack;
extern pack **spritepack;
extern int num_sprite_pack;

//these are used so that the correct byte order is used
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#undef DO_SWAP
#define SWAP16(X)    (X)
#define SWAP32(X)    (X)
#else
#define DO_SWAP
#define SWAP16(X)    SDL_Swap16(X)
#define SWAP32(X)    SDL_Swap32(X)
#endif

enum server_packet_types
{	//the numbers will eventually be deleted
	SERVER_CHAT_NORM = 8,	//?
	SERVER_VERSIONS = 10,
	SERVER_CHAT_WHISP = 13,
	SERVER_DISCONNECT = 18,
	SERVER_LOGIN = 21,
	SERVER_CHAR_DELETE = 33,
	SERVER_CHAT_GLOBAL = 40,
	SERVER_ENTERGAME = 63,
	SERVER_KEY = 65,
	SERVER_MAP = 76,
	SERVER_CHAR_STAT = 69,
	SERVER_NEWS = 90,
	SERVER_LOGIN_CHAR = 99,
	SERVER_CREATE_STAT = 106,
	SERVER_NUM_CHARS = 113,
	SERVER_LAST
};

#if SERVER_LAST > 256
#error "Too many server packet types"
#endif

enum client_packet_types
{	//the numbers will eventually be deleted
	CLIENT_LOGIN = 12,
	CLIENT_DELETE_CHAR = 34,
	CLIENT_CLICK = 43,
	CLIENT_ALIVE = 57,
	CLIENT_VERSION = 71,
	CLIENT_CREATE_CHAR,
	CLIENT_USE_CHAR = 83,
	CLIENT_INITGAME = 92,
	CLIENT_MYSTERY = 96,	//todo	reply to 109 "c" (bad)
	CLIENT_STATUS_REPLY,	//todo reply to status packet "ccc" (bad)
	CLIENT_CHAT = 104,
	CLIENT_LAST
};

#if CLIENT_LAST > 256
#error "Too many client packet types"
#endif

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

struct graphics_data
{
	pack *tilepack;
	pack **spritepack;
	int num_sprite_pack;
};

lin_char_info *make_lin_char_info(int char_type, int gender);

SDL_Surface *get_png_image(int num, client *who);
void check_fix_png(char *buffer, int *size);
SDL_Surface *get_img(int num, client *who);	//loads raw image data
SDL_Surface *get_image(SDL_RWops *buf);

SDL_Rect *make_sdl_rect(int x, int y, int w, int h);

int sha256_hash(char *filename, char *dest);

#endif
