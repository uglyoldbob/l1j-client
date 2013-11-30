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
	delay_mtx = SDL_CreateMutex();
	delay_loading = false;
	loader = 0;
}

tile::~tile()
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	if (delay_loading)
	{
		loader->cancel_request(delay_load_id);
	}
	SDL_mutexV(delay_mtx);
	SDL_DestroyMutex(delay_mtx);
	if (filebuf != 0)
	{
		delete [] filebuf;
		filebuf = 0;
	}
	if (tdata != 0)
	{
		for (int i = 0; i < tdata->num_tiles; i++)
		{
			if (tdata->ltile[i] != 0)
			{
				delete tdata->ltile[i];
				tdata->ltile[i] = 0;
			}
			if (tdata->rtile[i] != 0)
			{
				delete tdata->rtile[i];
				tdata->rtile[i] = 0;
			}
			if (tdata->stile[i] != 0)
			{
				delete tdata->stile[i];
				tdata->stile[i] = 0;
			}
		}
		if (tdata->num_tiles > 0)
		{
			delete [] tdata->ltile;
			tdata->ltile = 0;
			delete [] tdata->rtile;
			tdata->rtile = 0;
			delete [] tdata->stile;
			tdata->stile = 0;
		}
		delete [] tdata->offset;
		tdata->offset = 0;
		delete tdata;
		tdata = 0;
	}
}

int tile::get_amnt()
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	if (tdata != 0)
	{
		SDL_mutexV(delay_mtx);
		return tdata->num_tiles;
	}
	else
	{
		SDL_mutexV(delay_mtx);
		return 0;
	}
}

sdl_graphic *tile::get_tile_right(int which)
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	sdl_graphic *ret;
	if (tdata != 0)
	{
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
	}
	else
	{
		ret = 0;
	}
	SDL_mutexV(delay_mtx);
	return ret;
}

sdl_graphic *tile::get_tile_left(int which)
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	sdl_graphic *ret;
	if (tdata != 0)
	{
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
	}
	else
	{
		ret = 0;
	}
	SDL_mutexV(delay_mtx);
	return ret;
}

sdl_graphic *tile::get_special(int which)
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	sdl_graphic *ret;
	if (tdata != 0)
	{
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
	}
	else
	{
		ret = 0;
	}
	SDL_mutexV(delay_mtx);
	return ret;
}

void tile::delay_load(int which, sdl_user *orig, client *who)
{
	if (delay_loading == false)
	{
		client_request t_sdl;
		t_sdl.request = CLIENT_REQUEST_LOAD_TILE;
		t_sdl.data.tload.which = which;
		t_sdl.data.tload.item = this;
		delay_load_id = orig->add_request(t_sdl);
		delay_loading = true;
		loader = orig;
	}
}

void tile::load(int which, client *who)
{
	while (SDL_mutexP(delay_mtx) == -1) {};
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
			printf("Failed to load tileset %d\n", which);
			SDL_mutexV(delay_mtx);
			return;
		}

//		printf("The size of %s is %d\n", name, size);

		tdata = new tileset;
		tdata->num_tiles = *(short*)data;
		tdata->num_tiles = SWAP16(tdata->num_tiles);
//		printf("\tThere are %d tiles\n", tdata->num_tiles);
		tdata->offset = new int[tdata->num_tiles + 1];
		
		tdata->ltile = new sdl_graphic*[tdata->num_tiles];
		tdata->rtile = new sdl_graphic*[tdata->num_tiles];
		tdata->stile = new sdl_graphic*[tdata->num_tiles];
		
		//load the offsets for the tiles
		for (int i = 0; i <= tdata->num_tiles; i++)
		{
			int *temp = (int*)data;
			tdata->offset[i] = SWAP32(temp[i+1]);
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
	SDL_mutexV(delay_mtx);
}