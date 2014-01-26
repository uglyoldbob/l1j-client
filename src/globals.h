#ifndef __GLOBALS_H_
#define __GLOBALS_H_

#ifdef HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <SDL/SDL_image.h>

#include "resources/pack.h"
#include "resources/reg_font.h"
#include "resources/table.h"
class sdl_user;

extern char *lineage_dir;	//string to hold the absolute path to the lineage directory
extern reg_font lineage_font;

//pack data is the same for all clients thanks to the briefcase object
extern pack *textpack;
extern pack *tilepack;
extern pack **spritepack;
extern int num_sprite_pack;

#ifdef MAC
	#if ENDIAN_BIG == 1
	#define DO_SWAP 1
	#define SWAP16(X)    SDL_Swap16(X)
	#define SWAP32(X)    SDL_Swap32(X)
	#warning Swap is being used!!!!
	#elif ENDIAN_LITTLE == 1
	#undef DO_SWAP 1
	#define SWAP16(X)    (X)
	#define SWAP32(X)    (X)
	#warning There is no SWAP
	#else
	#error WHAT IS HAPPENING I AM BROKEN?
	#endif
#else
//these are used so that the correct byte order is used
	#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	#undef DO_SWAP
	#define SWAP16(X)    (X)
	#define SWAP32(X)    (X)
	#else
	#define DO_SWAP 1
	#define SWAP16(X)    SDL_Swap16(X)
	#define SWAP32(X)    SDL_Swap32(X)
#endif

#endif

enum server_packet_types
{	//the numbers will eventually be deleted
	SERVER_CHAT_NORM = 8,	//?
	SERVER_REMOVE_OBJECT,
	SERVER_VERSIONS,
	SERVER_DISCONNECT = 18,
	SERVER_LOGIN = 21,
	SERVER_CHAR_DELETE = 33,
	SERVER_AC_ELEMENTAL_UPDATE = 41,
	SERVER_CHAT_SHOUT,
	SERVER_INV_ITEMS = 47,
	SERVER_MOVE_OBJECT = 61,
	SERVER_ENTERGAME = 63,
	SERVER_GROUNDITEM,
	SERVER_KEY,
	SERVER_CHAR_STAT = 69,
	SERVER_TIME,
	SERVER_MAP = 76,
	SERVER_TITLE,
	SERVER_CHANGE_SPMR = 80,
	SERVER_WEATHER = 83,
	SERVER_MESSAGE = 87,
	SERVER_NEWS = 90,
	SERVER_CHAT_WHISPER,
	SERVER_MPVALS = 96,
	SERVER_LOGIN_CHAR = 99,
	SERVER_HPVALS = 101,
	SERVER_LIGHT,
	SERVER_STR_UPDATE = 104,
	SERVER_CHAT_GLOBAL,
	SERVER_CREATE_STAT,
	SERVER_DEX_UPDATE = 112,
	SERVER_NUM_CHARS,
	SERVER_LAST,
	SERVER_CHANGE_HEADING = 127
};

#if SERVER_LAST > 256
#error "Too many server packet types"
#endif

enum client_packet_types
{	//the numbers will eventually be deleted
	CLIENT_LOGIN = 12,
	CLIENT_CHAT_WHISPER,
	CLIENT_DELETE_CHAR = 34,
	CLIENT_CHAT_GLOBAL = 40,
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

SDL_Rect *make_sdl_rect(int x, int y, int w, int h);

extern "C" int sha256_hash(char *filename, char *dest);

#endif
