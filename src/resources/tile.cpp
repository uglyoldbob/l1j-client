#include <SDL.h>
#include "SDL_endian.h"
#include <stdio.h>

#include "globals.h"
#include "pack.h"
#include "tile.h"
#include "widgets/sdl_widget.h"

tile::tile()
{
	tdata = 0;
	filebuf = 0;
}

tile::~tile()
{
	if (filebuf != 0)
		delete [] filebuf;
	if (tdata != 0)
	{
		delete [] tdata->offset;
		delete tdata;
	}
}

int tile::get_amnt()
{
	if (tdata != 0)
	{
		return tdata->num_tiles;
	}
	else
	{
		return 0;
	}
}

sdl_graphic *tile::get_tile(int which)
{
	sdl_graphic *ret;
	ret = 0;
	if ((tdata->data[tdata->offset[which]] & 2) != 0)
	{
//		printf("TranspTile %d\n", which);
		ret = new sdl_graphic;
		unsigned char *data = (unsigned char*)&tdata->data[tdata->offset[which]+1];
		int data_offset = 0;
		int x, y;
		int width, height;
		x = data[data_offset++];
		y = data[data_offset++];
		width = data[data_offset++];
		height = data[data_offset++];
//		printf("\tx: %d y:%d w:%d h:%d\n", x, y, width, height);
		ret->img = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 16,
			0x7C00, 0x03E0, 0x001F, 0);
		ret->cleanup = false;
		ret->pos = make_sdl_rect(x, y, width, height);
		ret->mask = make_sdl_rect(0, 0, width, height);
		SDL_SetColorKey(ret->img, SDL_SRCCOLORKEY, SDL_MapRGB(ret->img->format, 255,255,255) );
		SDL_FillRect(ret->img, NULL, SDL_MapRGB(ret->img->format, 255,255,255));
		short *temp = (short*)ret->img->pixels;	//destination pointer
		int source_off = 0;
//		printf("There are %d rows\n", height);
		for (int i = 0; i < height; i++)
		{
			int row_segs = data[data_offset++];
			short *row_temp = temp;
//			printf("\tThere are %d segments in this row\n", row_segs);
			for (int j = 0; j < row_segs; j++)
			{
				int skip = data[data_offset++] / 2;
				int seg_width = data[data_offset++];
				row_temp = &row_temp[skip];
//				printf("\t\tSkip %d, print %d\n", skip, seg_width);
				memcpy(row_temp, &data[data_offset], seg_width * 2);
				data_offset += (seg_width * 2);
				row_temp = &row_temp[seg_width];
			}
			temp = &temp[width];
		}
	}
	else
	{
//		printf("\tNormal Tile %d\n", which);
		ret = new sdl_graphic;
		ret->img = SDL_CreateRGBSurface(SDL_SWSURFACE, 24, 24, 16,
			0x7C00, 0x03E0, 0x001F, 0);
		ret->cleanup = false;
		ret->pos = make_sdl_rect(10, 10, 24, 24);
		ret->mask = make_sdl_rect(0, 0, 24, 24);
		SDL_SetColorKey(ret->img, SDL_SRCCOLORKEY, SDL_MapRGB(ret->img->format, 1,1,1) );
		SDL_FillRect(ret->img, NULL, SDL_MapRGB(ret->img->format, 1,1,1));
		short *temp = (short*)ret->img->pixels;	//destination pointer
		short *source = (short*)&tdata->data[tdata->offset[which]+1];
		int source_off = 0;
		for (int i = 0; i < 24; i++)
		{
			int row_width;
			row_width = 2 * (i+1);
			if (i > 11)
			{
				row_width -= (4 * (i - 11));
			}
			memcpy(temp, source, row_width * 2);
			source = &source[24];
			temp = &temp[24];
		}
	}
	return ret;
}

void tile::load(int which, pack *source)
{
	if (tdata == 0)
	{
		char name[256];
		char *data;
		int size;
		sprintf(name, "%d.til", which);
		data = source->load_file(name, &size, 0);
		filebuf = data;
		if (data == 0)
		{
			return;
		}
		
//		printf("The size of %s is %d\n", name, size);

		tdata = new tileset;
		tdata->num_tiles = *(short*)data;
//		printf("\tThere are %d tiles\n", tdata->num_tiles);
		tdata->offset = new int[tdata->num_tiles + 1];
		
		//load the offsets for the tiles
		for (int i = 0; i <= tdata->num_tiles; i++)
		{
			int *temp = (int*)data;
			tdata->offset[i] = temp[i+1];
		}
		
		//initialize the data pointer
		tdata->data = &data[4 * (2 + tdata->num_tiles)];
		for (int i = 0; i < tdata->num_tiles; i++)
		{
			if (tdata->offset[i] != tdata->offset[i+1])
			{
				if ((tdata->data[tdata->offset[i]] & 2) != 0)
				{
//					printf("\tTranspTile\n");
//					printf("\tLength available is %08x\n", (int)offset[1] - (int)offset[0]);
//					convertTranspTile(mystery_data, data);
				}
				else
				{
//					printf("\tNormal Tile\n");
//					printf("\tLength available is %08x\n", (int)offset[1] - (int)offset[0]);
//					short *tile_data = (short*)&mystery_data[1];
//					for (int j = 0; j < 0x120; j++)
//					{
//						tile_data[j] = SWAP16(tile_data[j]);
//					}
				}
			}
		}
		
//		printf("\n");
	}
}