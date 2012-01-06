#include "globals.h"
#include "resources/sdl_font.h"

sdl_font lineage_font;
char *lineage_dir;

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

SDL_Surface *get_image(const char *name, pack *source)
{
	char *buffer;
	SDL_RWops *sdl_buf;
	int size;
	buffer = (char*)source->load_file(name, &size, 0);
	sdl_buf = SDL_RWFromConstMem(buffer, size);
	return get_image(sdl_buf);
}

SDL_Surface *get_png_image(int num, pack **source)
{
	char name[256];
	sprintf(name, "%d.png", num);
	int index = getHashIndex(name) + 1;
	
	char *buffer;
	SDL_Surface *ret;
	SDL_RWops *sdl_buf;
	int size;
	if (source != 0)
	{
		buffer = (char*)source[index]->load_png(name, &size, 0);
		if (buffer == 0)
		{
			buffer = (char*)source[0]->load_png(name, &size, 0);
		}
		if (buffer != 0)
		{
			sdl_buf = SDL_RWFromConstMem(buffer, size);
			ret = get_image(sdl_buf);
			delete [] buffer;
		}
		else
		{
			printf("Could not load %s\n", name);
			ret = (SDL_Surface*) 0;
		}
	}
	else
	{
		ret = (SDL_Surface*) 0;
	}
	return ret;
}

SDL_Surface *get_image(SDL_RWops *buf)
{
	if (IMG_isPNG(buf) == 1)
	{
		return IMG_LoadPNG_RW(buf);
	}
	return (SDL_Surface *)0;
}

SDL_Surface *get_img(int num, pack **source)
{
	SDL_Surface *image;
	image = (SDL_Surface *)0;
	
	char name[256];
	sprintf(name, "%de.img", num);
	
	char *buffer;
	SDL_RWops *sdl_buf;
	int size;
	int index = getHashIndex(name) + 1;
	if (source != 0)
	{
		buffer = (char*)source[index]->load_file(name, &size, 0);
		if (buffer == 0)
		{
			buffer = (char*)source[0]->load_file(name, &size, 0);
		}
		if (buffer == 0)
		{
			sprintf(name, "%d.img", num);
			index = getHashIndex(name) + 1;
			buffer = (char*)source[index]->load_file(name, &size, 0);
		}
		if (buffer == 0)
		{
			buffer = (char*)source[0]->load_file(name, &size, 0);
		}
		if (buffer != 0)
		{
			sdl_buf = SDL_RWFromConstMem(buffer, size);
			
			unsigned short width, height;
			unsigned short moron, moron2;
			unsigned short *data;
			SDL_RWread(sdl_buf, &width, 2, 1);
			SDL_RWread(sdl_buf, &height, 2, 1);
			SDL_RWread(sdl_buf, &moron, 2, 1);
			SDL_RWread(sdl_buf, &moron2, 2, 1);
			width = SWAP16(width);
			height = SWAP16(height);
			moron = SWAP16(moron);
			moron2 = SWAP16(moron2);
			data = new unsigned short[width * height];
			SDL_RWread(sdl_buf, data, 2, width * height);
			SDL_RWclose(sdl_buf);
			
			for (int i = 0; i < (width * height); i++)
			{
				data[i] = SWAP16(data[i]);
			}
	
			image = SDL_CreateRGBSurfaceFrom(data, width, height, 16, width*2, 
				0x7C00, 0x03E0, 0x001F, 0);
				
			SDL_SetColorKey(image, SDL_SRCCOLORKEY, moron2);
		}
	}
	return image;
}
