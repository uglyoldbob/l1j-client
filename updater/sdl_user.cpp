#include <SDL.h>
#include "SDL_image.h"

#include "globals.h"
#include "sdl_user.h"
#include "sdl_widget.h"

sdl_user::sdl_user(Uint32 flags)
{
	load_progress = 0;
	draw_mode = 0;
	graphx = 0;
	pg = 0;
	num_widgets = 0;
	widgets = (sdl_widget**)0;
	ready = false;
	
	load_amount = 0x249f0;	//not sure how this number is generated
	display = SDL_CreateRGBSurface(flags, SCREEN_WIDTH, SCREEN_HEIGHT, 16,
		0x7C00, 0x03E0, 0x001F, 0);
}

sdl_user::~sdl_user()
{
	SDL_FreeSurface(display);
}

void sdl_user::give_data(graphics_data *abc)
{
	if (graphx == 0)
	{
		graphx = abc;
		prepare_load1();
		ready = true;
	}
}

void sdl_user::prepare_load1()
{
	if (pg != 0)
		delete pg;
	pg = new prepared_graphics;
	pg->num_pg = 2;
	
	int index;
	SDL_Rect *rect;

	pg->pg = new prepared_graphic[2];
	
	index = getHashIndex("811.png") + 1;
	pg->pg[0].surf = get_png_image("811.png", graphx->spritepack[index]);
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = false;
	
	pg->pg[1].surf = get_img("330.img", graphx->spritepack);
	rect = new SDL_Rect;
  	rect->x = 0xf1;
  	rect->y = 0x181;
	pg->pg[1].position = rect;
	rect = new SDL_Rect;
	rect->x = 0;
	rect->y = 0;
	rect->h = pg->pg[1].surf->h;
	rect->w = 0;
	pg->pg[1].mask = rect;
	pg->pg[1].cleanup = true;
	pg->ready = true;
}

void sdl_user::add_loaded(int size)
{
	if (draw_mode == 0)
	{
		load_progress += size;
		update_load();
	}
}

void sdl_user::load_done()
{
	if (draw_mode == 0)
	{
		load_progress = load_amount;
		update_load();
		SDL_Delay(250);
		ready = false;
		delete pg;
		pg = 0;
		prepare_login();
		ready = true;
		draw_mode = 1;
	}
}

void sdl_user::update_load()
{
	if (draw_mode == 0)
	{
		float temp = (float)pg->pg[1].surf->w * ((float)load_progress / (float)load_amount);
		pg->pg[1].mask->w = (Uint16)temp;
	}
}

void sdl_user::set_load_amount(int size)
{
	if (draw_mode == 0)
	{
		load_amount = size;
	}
}


SDL_Surface *sdl_user::get_image(const char *name, pack *source)
{
	char *buffer;
	SDL_RWops *sdl_buf;
	int size;
	buffer = (char*)source->load_file(name, &size, 0);
	sdl_buf = SDL_RWFromConstMem(buffer, size);
	return get_image(sdl_buf);
}

SDL_Surface *sdl_user::get_png_image(const char *name, pack *source)
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

SDL_Surface *sdl_user::get_image(SDL_RWops *buf)
{
	printf("Check returns %d\n", IMG_isPNG(buf));
	if (IMG_isPNG(buf) == 1)
	{
		printf("Converting png to surface\n");
		return IMG_LoadPNG_RW(buf);
	}
	return (SDL_Surface *)0;
}

SDL_Surface *sdl_user::get_img(const char *name, pack **source)
{
	SDL_Surface *image;
	
	char *buffer;
	SDL_RWops *sdl_buf;
	int size;
	int index = getHashIndex(name) + 1;
	if (source != 0)
	{
		printf("Loading %s\n", name);
		buffer = (char*)source[index]->load_file(name, &size, 0);
		printf("Buffer = %08x\n", (int)buffer);
		if (buffer == 0)
		{
			printf("Attempting to load %s again\n", name);
			buffer = (char*)source[0]->load_file(name, &size, 0);
		}
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
		printf("Unknown data in %s: %02x %02x\n", name, moron & 0xFF, 
			moron2 & 0xFF);
		printf("\tImage size %d %d %04x %04x %04x\n", size, index, width, height, 
			(size/2) - (width*height));
		data = new unsigned short[width * height];
		SDL_RWread(sdl_buf, data, 2, width * height);
		for (int i = 0; i < width * height; i++)
		{
			data[i] = SWAP16(data[i]);
		}
//		SDL_RWclose(sdl_buf);

		image = SDL_CreateRGBSurfaceFrom(data, width, height, 16, width*2, 
			0x7C00, 0x03E0, 0x001F, 0);
	}
	else
	{
		image = (SDL_Surface *)0;
	}
	return image;
}

void sdl_user::draw()
{
	if (ready)
	{
		pg->draw(display);
		for (int i = 0; i < num_widgets; i++)
		{
			if (widgets[i] != 0)
				widgets[i]->draw(display);
		}
		switch(draw_mode)
		{
			case 0:
				draw_load1();
				break;
			case 1:
				draw_login();
				break;
			default:
				break;
		}
	}
}

void sdl_user::prepare_login()
{
	ready = false;
	pg = new prepared_graphics;
	pg->num_pg = 1;
	pg->pg = new prepared_graphic[1];
	
	int index = getHashIndex("814.png") + 1;
	pg->pg[0].surf = get_png_image("814.png", graphx->spritepack[index]);
	pg->pg[0].mask = NULL;
	pg->pg[0].position = NULL;
	pg->pg[0].cleanup = false;
	pg->ready = true;
	
	SDL_Rect *pos;
	SDL_Rect *mask;
	SDL_Surface *surf;
	
	num_widgets = 5;
	widgets = new sdl_widget*[num_widgets];
	
	pos = new SDL_Rect;
	pos->x = 0x1a9;
	pos->y = 0x138;
	surf = get_img("59.img", graphx->spritepack);
	mask = new SDL_Rect;
	mask->x = 0;
	mask->y = 0;
	mask->w = surf->w;
	mask->h = surf->h;
	widgets[0] = new sdl_widget(pos, mask, surf);
	
	pos = new SDL_Rect;
	pos->x = 0x213;
	pos->y = 0x183;
	surf = get_img("53.img", graphx->spritepack);
	mask = new SDL_Rect;
	mask->x = 0;
	mask->y = 0;
	mask->w = surf->w;
	mask->h = surf->h;
	widgets[1] = new sdl_widget(pos, mask, surf);
	
	pos = new SDL_Rect;
	pos->x = 0x213;
	pos->y = 0x195;
	surf = get_img("65.img", graphx->spritepack);
	mask = new SDL_Rect;
	mask->x = 0;
	mask->y = 0;
	mask->w = surf->w;
	mask->h = surf->h;
	widgets[2] = new sdl_widget(pos, mask, surf);
	
	pos = new SDL_Rect;
	pos->x = 0x213;
	pos->y = 0x1a8;
	surf = get_img("55.img", graphx->spritepack);
	mask = new SDL_Rect;
	mask->x = 0;
	mask->y = 0;
	mask->w = surf->w;
	mask->h = surf->h;
	widgets[3] = new sdl_widget(pos, mask, surf);
	
	pos = new SDL_Rect;
	pos->x = 0x213;
	pos->y = 0x1c2;
	surf = get_img("57.img", graphx->spritepack);
	mask = new SDL_Rect;
	mask->x = 0;
	mask->y = 0;
	mask->w = surf->w;
	mask->h = surf->h;
	widgets[4] = new sdl_widget(pos, mask, surf);
	
	ready = true;
}

void sdl_user::draw_load1()
{
	
}

void sdl_user::draw_login()
{	//59.img createSubControls, Bitmap	(0x1907000, 0x1a9, 0x138)
	//53.img createSubControls, HighlightButton, LButton (0x213, 0x183, login, 0, 0)
		//54 highlight
	//65.img createSubControls, HighlightButton, LButton (0x213, 0x195, normalMenu, 4, 0)
	//55.img createSubControls, HighlightButton, LButton (0x213, 0x1a8, newAccountMenu, 5, 0)
	//57.img createSubControls, HighlightButton, LButton (0x213, 0x1c2, normalMenu, 6, 0)
	//814.png
	
	
}