#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <stdio.h>

#include "client.h"
#include "files.h"
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
		for (int i = 0; i < tdata->num_tiles; i++)
		{
			if (tdata->ltile[i] != 0)
				delete tdata->ltile[i];
			if (tdata->rtile[i] != 0)
				delete tdata->rtile[i];
			if (tdata->stile[i] != 0)
				delete tdata->stile[i];
		}
		if (tdata->num_tiles > 0)
		{
			delete [] tdata->ltile;
			delete [] tdata->rtile;
			delete [] tdata->stile;
		}
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

sdl_graphic *tile::get_tile_right(int which)
{
	sdl_graphic *ret;
	if (tdata->rtile[which] == 0)
	{
		if ((tdata->data[tdata->offset[which]] & 2) != 0)
		{
			ret = get_special(which);
		}
		else
		{
			short *source = (short*)&tdata->data[tdata->offset[which]+1];
			tdata->rtile[which] = new sdl_graphic(1, 2, source, GRAPH_RTIL);
			ret = tdata->rtile[which];
		}
	}
	else
	{
		ret = tdata->rtile[which];
	}
	return ret;
}

sdl_graphic *tile::get_tile_left(int which)
{
	sdl_graphic *ret;
	if (tdata->ltile[which] == 0)
	{
		if ((tdata->data[tdata->offset[which]] & 2) != 0)
		{
			ret = get_special(which);
		}
		else
		{
			short *source = (short*)&tdata->data[tdata->offset[which]+1];
			tdata->ltile[which] = new sdl_graphic(1, 2, source, GRAPH_LTIL);
			ret = tdata->ltile[which];
		}
	}
	else
	{
		ret = tdata->ltile[which];
	}
	return ret;
}

sdl_graphic *tile::get_special(int which)
{
	sdl_graphic *ret;
	if (tdata->stile[which] == 0)
	{
		short *source = (short*)&tdata->data[tdata->offset[which]+1];
		tdata->stile[which] = new sdl_graphic(1, 2, source, GRAPH_STIL);
		ret = tdata->stile[which];
	}
	else
	{
		ret = tdata->stile[which];
	}
	return ret;
}

void tile::load(int which, client *who)
{
	if (tdata == 0)
	{
		char name[256];
		char *data;
		int size;
		sprintf(name, "%d.til", which);
		//SDL_RWops *sdl_buf;
		data = (char*)who->getfiles->load_file(name, &size, FILE_TILEPACK, 0);
		filebuf = data;
		//sdl_buf = SDL_RWFromConstMem(buffer, size);
		//SDL_RWread(sdl_buf, &moron2, 2, 1);

		if (data == 0)
		{
			return;
		}
		
//		printf("The size of %s is %d\n", name, size);

		tdata = new tileset;
		tdata->num_tiles = *(short*)data;
//		printf("\tThere are %d tiles\n", tdata->num_tiles);
		tdata->offset = new int[tdata->num_tiles + 1];
		
		tdata->ltile = new sdl_graphic*[tdata->num_tiles];
		tdata->rtile = new sdl_graphic*[tdata->num_tiles];
		tdata->stile = new sdl_graphic*[tdata->num_tiles];
		
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
			tdata->ltile[i] = 0;
			tdata->rtile[i] = 0;
			tdata->stile[i] = 0;
			if (tdata->offset[i] != tdata->offset[i+1])
			{
				if ((tdata->data[tdata->offset[i]] & 2) != 0)
				{
//					printf("\tTranspTile %d\n", i);
//					printf("\tLength available is %08x\n", (int)offset[1] - (int)offset[0]);
//					convertTranspTile(mystery_data, data);
				}
				else
				{
//					printf("\tNormal Tile %d\n", i);
//					printf("\tLength available is %08x\n", (int)offset[1] - (int)offset[0]);
//					short *tile_data = (short*)tdata->data;
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