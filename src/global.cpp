#include "globals.h"
#include "resources/files.h"
#include "resources/reg_font.h"

reg_font lineage_font;
char *lineage_dir;

ThingAdministrator all_the_things;
Game games;

lin_char_info *make_lin_char_info(int char_type, int gender)
{
	lin_char_info *ret;
	ret = new lin_char_info;
	ret->name = new char[2];
	strcpy(ret->name, "");
	ret->pledge = new char[2];
	strcpy(ret->pledge, "");
	ret->char_type = char_type;
	ret->gender = gender;
	switch(char_type)
	{
		case CLASS_ROYAL:
			ret->str = 13;	ret->max_str = 18;
			ret->dex = 10;	ret->max_dex = 18;
			ret->con = 10;	ret->max_con = 18;
			ret->wis = 11;	ret->max_wis = 18;
			ret->cha = 13;	ret->max_cha = 18;
			ret->intl = 10;	ret->max_int = 18;
			break;
		case CLASS_KNIGHT:
			ret->str = 16;	ret->max_str = 20;
			ret->dex = 12;	ret->max_dex = 18;
			ret->con = 14;	ret->max_con = 18;
			ret->wis = 9;	ret->max_wis = 18;
			ret->cha = 12;	ret->max_cha = 18;
			ret->intl = 8;	ret->max_int = 18;
			break;
		case CLASS_ELF:
			ret->str = 11;	ret->max_str = 18;
			ret->dex = 12;	ret->max_dex = 18;
			ret->con = 12;	ret->max_con = 18;
			ret->wis = 12;	ret->max_wis = 18;
			ret->cha = 9;	ret->max_cha = 18;
			ret->intl = 12;	ret->max_int = 18;
			break;
		case CLASS_WIZARD:
			ret->str = 8;	ret->max_str = 18;
			ret->dex = 7;	ret->max_dex = 18;
			ret->con = 12;	ret->max_con = 18;
			ret->wis = 12;	ret->max_wis = 18;
			ret->cha = 8;	ret->max_cha = 18;
			ret->intl = 12;	ret->max_int = 18;
			break;
		case CLASS_DARKELF:
			ret->str = 12;	ret->max_str = 18;
			ret->dex = 15;	ret->max_dex = 18;
			ret->con = 8;	ret->max_con = 18;
			ret->wis = 10;	ret->max_wis = 18;
			ret->cha = 9;	ret->max_cha = 18;
			ret->intl = 11;	ret->max_int = 18;
			break;
		case CLASS_DRAGONKNIGHT:
			ret->str = 13;	ret->max_str = 18;
			ret->dex = 11;	ret->max_dex = 18;
			ret->con = 14;	ret->max_con = 18;
			ret->wis = 12;	ret->max_wis = 18;
			ret->cha = 8;	ret->max_cha = 18;
			ret->intl = 11;	ret->max_int = 18;
			break;
		case CLASS_ILLUSIONIST:
			ret->str = 11;	ret->max_str = 18;
			ret->dex = 10;	ret->max_dex = 18;
			ret->con = 12;	ret->max_con = 18;
			ret->wis = 12;	ret->max_wis = 18;
			ret->cha = 8;	ret->max_cha = 18;
			ret->intl = 12;	ret->max_int = 18;
			break;
		default:
			ret->str = 0;	ret->max_str = 18;
			ret->dex = 0;	ret->max_str = 18;
			ret->con = 0;	ret->max_str = 18;
			ret->wis = 0;	ret->max_str = 18;
			ret->cha = 0;	ret->max_str = 18;
			ret->intl = 0;	ret->max_str = 18;
			break;
	}
	return ret;
}

SDL_Rect *make_sdl_rect(int x, int y, int w, int h)
{
	SDL_Rect *ret;
	ret = new SDL_Rect;
	ret->x = x;
	ret->y = y;
	ret->w = w;
	ret->h = h;
	return ret;
}
