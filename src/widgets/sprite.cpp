#include "client.h"
#include "sprite.h"

sprite::sprite(int x, int y, char *filename, client *who)
	: sdl_widget(x, y, who)
{
	printf("Opening sprite %s\n", filename);
	bool tiles_loaded = false;
	int tiles_location = 0;
	int size;
	char *data = (char*)myclient->getfiles->load_file(filename, &size, FILE_SPRITESPACK, 0);
	
	if (data == 0) throw "Invalid sprite";
	
	SDL_RWops *file = SDL_RWFromMem(data, size);
	
	if (strncmp(data, "SPR_1", 5) == 0)
	{
		printf("Convert frame (SPR_1)\n");
		SDL_RWseek(file, 5, SEEK_CUR);
		throw "Unsupported sprite";
	}
	else if (strncmp(data, "SPR_0", 5) == 0)
	{
		printf("SPR_0 signature found\n");
		SDL_RWseek(file, 5, SEEK_CUR);
	}
	
	char mystery1;
	
	SDL_RWread(file, &mystery1, 1, 1);
	
	if (mystery1 < 0)
	{
		printf("mys1 < 0\n");
		SDL_RWread(file, &mystery1, 1, 1);
		if (mystery1 == 0)
			mystery1 = 0x100;
		
		tiles_loaded = true;
		tiles_location = SDL_RWtell(file);
		for (int i = 0; i < mystery1; i++)
		{	//these are probably an offset related to tile data
			short stupid;
			SDL_RWread(file, &stupid, 2, 1);
			printf("\tSkipping %d\n", stupid);
		}
		
		SDL_RWread(file, &mystery1, 1, 1);
	}
	
	printf("There are %d sequences\n", mystery1);
	for (int i = 0; i < mystery1; i++)
	{
		short x1, y1, x2, y2, n;
		
		SDL_RWread(file, &x1, 2, 1);
		SDL_RWread(file, &y1, 2, 1);
		SDL_RWread(file, &x2, 2, 1);
		SDL_RWread(file, &y2, 2, 1);
		SDL_RWseek(file, 4, SEEK_CUR);
		SDL_RWread(file, &n, 2, 1);
		printf("(%d, %d) -> (%d, %d), %d frames\n", x1, y1, x2, y2, n);
		for (int j = 0; j < n; j++)
		{
			char tx, ty, th;
			short ttile;
			SDL_RWread(file, &tx, 1, 1);
			SDL_RWread(file, &ty, 1, 1);
			SDL_RWread(file, &th, 1, 1);
			SDL_RWread(file, &ttile, 2, 1);
			printf("\t[%d, %d] %d %d\n", tx, ty, th, ttile);
		}
	}
	
	int tiles_size;
	int *tile_offsets;
	SDL_RWread(file, &num_tiles, 4, 1);
	
	tile_offsets = new int[num_tiles];
	tiles = new sdl_graphic*[num_tiles];
	
	for (int k = 0; k < num_tiles; k++)
	{	//these values are offsets
		SDL_RWread(file, &tile_offsets[k], 4, 1);
	}
	
	SDL_RWread(file, &tiles_size, 4, 1);
	
	printf("There are %d tiles\n", num_tiles);
	printf("All the tiles take %d bytes\n", tiles_size);
	if (tiles_loaded)
	{	//strange tile formats
		printf("Unable to load strange tile format\n");
		SDL_RWseek(file, tiles_location, SEEK_SET);
	}
	else
	{
		printf("Loading tile data\n");
		tiles_location = SDL_RWtell(file);
		for (int i = 0; i < num_tiles; i++)
		{
			SDL_RWseek(file, tiles_location + tile_offsets[i], SEEK_SET);
			tiles[i] = new sdl_graphic(0, 0, 
				(short*)&data[tiles_location + tile_offsets[i]], 
				GRAPH_STIL);
			char testname[256];
			sprintf(testname, "%s-%d.bmp", filename, i);
			tiles[i]->make_bmp(testname);
		}
	}
	
	SDL_RWclose(file);
	delete [] data;
	
	printf("Reached end of sprite loading\n");
}

sprite::~sprite()
{
}
