#include <SDL/SDL.h>

#include "client.h"
#include "globals.h"
#include "sdl_graphic.h"

sdl_graphic::sdl_graphic(int x, int y, int w, int h)
{
	img = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 16,
		0x7C00, 0x03E0, 0x001F, 0);
	cleanup = false;
	pos = make_sdl_rect(x, y, w, h);
	mask = make_sdl_rect(0, 0, w, h);
	SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 255,255,255) );
	transp_color = SDL_MapRGB(img->format, 255,255,255);
	SDL_FillRect(img, NULL, SDL_MapRGB(img->format, 255,255,255));
}

void sdl_graphic::erase()
{
	SDL_FillRect(img, NULL, transp_color);
}

sdl_graphic::sdl_graphic(int x, int y, SDL_RWops *source, short *palette, int type)
{
	switch(type)
	{
		case GRAPH_STIL:
		{	//	printf("TranspTile %d\n", which);
			int x = 0, y = 0;
			int width = 0, height = 0;
			int read = 0;
			SDL_RWread(source, &x, 1, 1);
			x = SWAP32(x);
			SDL_RWread(source, &y, 1, 1);
			y = SWAP32(y);
			SDL_RWread(source, &width, 1, 1);
			width = SWAP32(width);
			SDL_RWread(source, &height, 1, 1);
			height = SWAP32(height);
//			printf("\t\tx: 0x%02x y:0x%02x w:0x%02x h:0x%02x ?:0x%02x ?:0x%02x\n", x, y, width, height, x+width, y+height);
			img = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 16,
				0x7C00, 0x03E0, 0x001F, 0);
			cleanup = false;
			pos = make_sdl_rect(x, y, width, height);
			mask = make_sdl_rect(x, y, width, height);
			SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 255,255,0) );
			transp_color = SDL_MapRGB(img->format, 255,255,0);
			SDL_FillRect(img, NULL, SDL_MapRGB(img->format, 255,255,0));
			short *temp = (short*)img->pixels;	//destination pointer
//			printf("\t\tThere are %d rows\n", height);
			for (int i = 0; i < height; i++)
			{
				int row_segs = 0;
				SDL_RWread(source, &row_segs, 1, 1);
				row_segs = SWAP32(row_segs);
				read++;
				short *row_temp = temp;
//				printf("\t\tThere are %d segments in this row\n", row_segs);
				for (int j = 0; j < row_segs; j++)
				{
					char skip = 0;
					SDL_RWread(source, &skip, 1, 1);
					read++;
					skip /= 2;
					int seg_width = 0;
					SDL_RWread(source, &seg_width, 1, 1);
					seg_width = SWAP32(seg_width);
					read++;
					row_temp = &row_temp[skip];
//					printf("\t\t\tSkip %d, print %d [", skip, seg_width);
					for (int k = 0; k < seg_width; k++)
					{
						char pal_color= 0;
						SDL_RWread(source, &pal_color, 1, 1);
						read++;
						row_temp[k] = palette[pal_color&0xFF];
//						printf("%02x ", pal_color&0xFF);
					}
//					row_temp[0] = 0x0000;
//					row_temp[seg_width-1] = 0xFFFF;
//					printf("] %d\n", read);
					row_temp = &row_temp[seg_width];
				}
				temp = &temp[img->pitch / 2];
			}
		}
			break;
		default:
			break;
	}
}

sdl_graphic::sdl_graphic(int x, int y, short *source, int type)
{
	switch(type)
	{
		case GRAPH_STIL:
		{
		//	printf("TranspTile %d\n", which);
			unsigned char *data = (unsigned char*)source;
			int data_offset = 0;
			int x, y;
			int width, height;
			x = data[data_offset++];
			y = data[data_offset++];
			width = data[data_offset++];
			height = data[data_offset++];
		//	printf("\tx: %d y:%d w:%d h:%d\n", x, y, width, height);
			img = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 16,
				0x7C00, 0x03E0, 0x001F, 0);
			short *manip = (short*) &data[data_offset];
			for (int i = 0; i < width * height / 2; i++)
			{
				manip[i] = SWAP16(manip[i]);
			}
			cleanup = false;
			pos = make_sdl_rect(x, y, width, height);
			mask = make_sdl_rect(0, 0, width, height);
			SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 255,255,255) );
			transp_color = SDL_MapRGB(img->format, 255,255,255);
			SDL_FillRect(img, NULL, SDL_MapRGB(img->format, 255,255,255));
			short *temp = (short*)img->pixels;	//destination pointer
		//	printf("There are %d rows\n", height);
			for (int i = 0; i < height; i++)
			{
				int row_segs = data[data_offset++];
				short *row_temp = temp;
		//		printf("\tThere are %d segments in this row\n", row_segs);
				for (int j = 0; j < row_segs; j++)
				{
					int skip = data[data_offset++] / 2;
					int seg_width = data[data_offset++];
					row_temp = &row_temp[skip];
		//			printf("\t\tSkip %d, print %d\n", skip, seg_width);
					memcpy(row_temp, &data[data_offset], seg_width * 2);
					data_offset += (seg_width * 2);
					row_temp = &row_temp[seg_width];
				}
				temp = &temp[img->pitch / 2];
			}

			break;
		}
		case GRAPH_LTIL:
		{
			img = SDL_CreateRGBSurface(SDL_SWSURFACE, 24, 24, 16,
				0x7C00, 0x03E0, 0x001F, 0);
			cleanup = false;
			pos = make_sdl_rect(0, 0, 24, 24);
			mask = make_sdl_rect(0, 0, 24, 24);
			SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 1,1,1) );
			transp_color = SDL_MapRGB(img->format, 1, 1, 1);
			SDL_FillRect(img, NULL, SDL_MapRGB(img->format, 1,1,1));
			short *temp = (short*)img->pixels;	//destination pointer
			for (int i = 0; i < 24; i++)
			{
				int row_width;
				row_width = 2 * (i+1);
				if (i > 11)
				{
					row_width -= (4 * (i - 11));
				}
				for (int j = 0; j < row_width; j++)
				{
					source[j] = SWAP16(source[j]);
				}
				memcpy(&temp[24*i+(24 - row_width)], source, row_width * 2);
				source = &source[row_width];
			}
			break;
		}
		case GRAPH_RTIL:
		{
			img = SDL_CreateRGBSurface(SDL_SWSURFACE, 24, 24, 16,
				0x7C00, 0x03E0, 0x001F, 0);
			cleanup = false;
			pos = make_sdl_rect(0, 0, 24, 24);
			mask = make_sdl_rect(0, 0, 24, 24);
			SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 1,1,1) );
			SDL_MapRGB(img->format, 1, 1, 1);
			SDL_FillRect(img, NULL, SDL_MapRGB(img->format, 1,1,1));
			short *temp = (short*)img->pixels;	//destination pointer
			for (int i = 0; i < 24; i++)
			{
				int row_width;
				row_width = 2 * (i+1);
				if (i > 11)
				{
					row_width -= (4 * (i - 11));
				}
				for (int j = 0; j < row_width; j++)
				{
					source[j] = SWAP16(source[j]);
				}
				memcpy(&temp[i*24], source, row_width * 2);
				source = &source[row_width];
			}
			break;
		}
		default:
			break;
	}
}

sdl_graphic::sdl_graphic(char *name, int x, int y, client *who, int type)
{
	if (name == 0)
	{
		pos = 0;
		mask = 0;
		img = 0;
		cleanup = false;
	}
	else
	{
		switch (type)
		{
			case GRAPH_PNG:
				img = get_png_image(name, who);
				if (img != 0)
				{
					SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 0, 0, 0));
					SDL_MapRGB(img->format, 0, 0, 0);
					pos = make_sdl_rect(x, y, img->w, img->h);
					mask = make_sdl_rect(0, 0, img->w, img->h);
					cleanup = false;
				}
				break;
			default:
				break;
		}
		if (img == 0)
		{
			pos = 0;
			mask = 0;
			cleanup = false;
		}
	}
}

sdl_graphic::sdl_graphic(int num, int x, int y, client *who, int type)
{
	if (num == -1)
	{
		pos = 0;
		mask = 0;
		img = 0;
		cleanup = false;
	}
	else
	{
		switch (type)
		{
			case GRAPH_IMG:
				img = get_img(num, who);
				if (img != 0)
				{
					pos = make_sdl_rect(x, y, img->w, img->h);
					mask = make_sdl_rect(0, 0, img->w, img->h);
					cleanup = true;
				}
				break;
			case GRAPH_PNG:
				img = get_png_image(num, who);
				if (img != 0)
				{
					SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 0, 0, 0));
					SDL_MapRGB(img->format, 0, 0, 0);
					pos = make_sdl_rect(x, y, img->w, img->h);
					mask = make_sdl_rect(0, 0, img->w, img->h);
					cleanup = false;
				}
				break;
			default:
				break;
		}
		if (img == 0)
		{
			pos = 0;
			mask = 0;
			cleanup = false;
		}
	}
}

void sdl_graphic::make_bmp(char *name)
{
	if (img != 0)
	{
		SDL_SaveBMP(img, name);
	}
}

void sdl_graphic::disable_clear()
{
	SDL_SetColorKey(img, 0, 0);
}

sdl_graphic::~sdl_graphic()
{
	if (pos != 0)
		delete pos;
	if (mask != 0)
		delete mask;
	if (img != 0)
	{
		if (cleanup)
			delete [] (short*)img->pixels;
		SDL_FreeSurface(img);
	}
}

void sdl_graphic::drawat(int x, int y, SDL_Surface *display)
{
	if (img != 0)
	{
		SDL_Rect *newpos;
		newpos = make_sdl_rect(x + pos->x, y + pos->y, mask->w, mask->h);
		SDL_BlitSurface(img, mask, display, newpos);
		delete newpos;
	}
}

void sdl_graphic::draw(SDL_Surface *display)
{
	if (img != 0)
	{
		SDL_BlitSurface(img, mask, display, pos);
	}
}

int sdl_graphic::getx()
{
	if (pos != 0)
	{
		return pos->x;
	}
	else
	{
		return 0;
	}
}

int sdl_graphic::gety()
{
	if (pos != 0)
	{
		return pos->y;
	}
	else
	{
		return 0;
	}
}

int sdl_graphic::getw()
{
	if (pos != 0)
	{
		return pos->w;
	}
	else
	{
		return 0;
	}
}

void sdl_graphic::setmx(int nmx)
{
	if (mask != 0)
	{
		mask->x = nmx;
	}
}

void sdl_graphic::setmy(int nmy)
{
	if (mask != 0)
	{
		mask->y = nmy;
	}
}

void sdl_graphic::setmw(int nmw)
{
	if (mask != 0)
	{
		mask->w = nmw;
	}
}

void sdl_graphic::setmh(int nmh)
{
	if (mask != 0)
	{
		mask->h = nmh;
	}
}

int sdl_graphic::geth()
{
	if (pos != 0)
	{
		return pos->h;
	}
	else
	{
		return 0;
	}
}

SDL_Surface *sdl_graphic::get_surf()
{
	return img;
}

Uint32 sdl_graphic::color(Uint8 r, Uint8 g, Uint8 b)
{
	return SDL_MapRGB(img->format, r, g, b);
}

void sdl_graphic::mod_with(SDL_Surface *display)
{
	if (img != 0)
	{
		SDL_BlitSurface(display, mask, img, pos);
	}	
}
