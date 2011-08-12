#include "globals.h"
#include "sdl_font.h"

sdl_font lineage_font;//("Font/eng.fnt");
char *lineage_dir;	//TODO : add a variable into the config file to initialize this

SDL_Surface *get_image(const char *name, pack *source)
{
	char *buffer;
	SDL_RWops *sdl_buf;
	int size;
	buffer = (char*)source->load_file(name, &size, 0);
	sdl_buf = SDL_RWFromConstMem(buffer, size);
	return get_image(sdl_buf);
}

SDL_Surface *get_png_image(const char *name, pack *source)
{
	char *buffer;
	SDL_Surface *ret;
	SDL_RWops *sdl_buf;
	int size;
	if (source != 0)
	{
		buffer = (char*)source->load_png(name, &size, 0);
		sdl_buf = SDL_RWFromConstMem(buffer, size);
		ret = get_image(sdl_buf);
		delete [] buffer;
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
		}
	}
	return image;
}
