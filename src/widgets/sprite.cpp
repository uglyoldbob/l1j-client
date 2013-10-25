#include "client.h"
#include "sprite.h"

sprite::sprite(int x, int y, char *filename, client *who)
	: sdl_widget(x, y, who)
{
	printf("Opening sprite %s\n", filename);
	frame_num = 1;
	time_change = SDL_GetTicks();
	
	loc_x = x;
	loc_y = y;
	
	bool tiles_loaded = false;
	short *palette = 0;
	int size;
	char *data = (char*)myclient->getfiles->load_file(filename, &size, FILE_SPRITESPACK, 0);
	
	if (data == 0)
		data = (char*)myclient->getfiles->load_file(filename, &size, FILE_SPRITEPACK, 0);
	
	printf("File: ");
	for (int asdf = 0; asdf < size; asdf++)
	{
		printf("%02x ", data[asdf]&0xFF);
	} printf("\n");
	
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
	
	char mystery1 = 0;
	
	SDL_RWread(file, &mystery1, 1, 1);
	
	if (mystery1 < 0)
	{
		int pallete_enries = 0;
		SDL_RWread(file, &pallete_enries, 1, 1);
		pallete_enries = SWAP32(pallete_enries);
		if (pallete_enries == 0)
			pallete_enries = 0x100;
		tiles_loaded = true;
		palette = new short[pallete_enries];
		for (int i = 0; i < pallete_enries; i++)
		{
			SDL_RWread(file, &palette[i], 2, 1);
			palette[i] = SWAP16(palette[i]);
			printf("\tSkipping (%d) 0x%04x\n", i, palette[i]);
		}
		
		SDL_RWread(file, &mystery1, 1, 1);
	}
	
	num_frames = mystery1;
	
	printf("There are %d frames\n", num_frames);
	frames = new sprite_frame[num_frames];
	for (int i = 0; i < num_frames; i++)
	{
		SDL_RWread(file, &frames[i].x1, 2, 1);
		frames[i].x1 = SWAP16(frames[i].x1);
		SDL_RWread(file, &frames[i].y1, 2, 1);
		frames[i].y1 = SWAP16(frames[i].y1);
		SDL_RWread(file, &frames[i].x2, 2, 1);
		frames[i].x2 = SWAP16(frames[i].x2);
		SDL_RWread(file, &frames[i].y2, 2, 1);
		frames[i].y2 = SWAP16(frames[i].y2);
		SDL_RWseek(file, 4, SEEK_CUR);
		SDL_RWread(file, &frames[i].num_tiles, 2, 1);
		frames[i].num_tiles = SWAP16(frames[i].num_tiles);
		printf("(%d, %d) -> (%d, %d), %d tiles\n", 
			frames[i].x1, frames[i].y1, 
			frames[i].x2, frames[i].y2, 
			frames[i].num_tiles);
		frames[i].tiles = new sprite_tile[frames[i].num_tiles];
		for (int j = 0; j < frames[i].num_tiles; j++)
		{
			SDL_RWread(file, &frames[i].tiles[j].x, 1, 1);
			SDL_RWread(file, &frames[i].tiles[j].y, 1, 1);
			SDL_RWread(file, &frames[i].tiles[j].h, 1, 1);
			SDL_RWread(file, &frames[i].tiles[j].tile, 2, 1);
			frames[i].tiles[j].tile = SWAP16(frames[i].tiles[j].tile);
			printf("\t[%d, %d] %d %d\n", 
				frames[i].tiles[j].x, frames[i].tiles[j].y, 
				frames[i].tiles[j].h, frames[i].tiles[j].tile);
		}
	}
	
	int tiles_size;
	int *tile_offsets;
	SDL_RWread(file, &num_tiles, 4, 1);
	num_tiles = SWAP32(num_tiles);
	
	tile_offsets = new int[num_tiles];
	tiles = new sdl_graphic*[num_tiles];
	
	for (int k = 0; k < num_tiles; k++)
	{	//these values are offsets
		SDL_RWread(file, &tile_offsets[k], 4, 1);
		tile_offsets[k] = SWAP32(tile_offsets[k]);
	}
	
	SDL_RWread(file, &tiles_size, 4, 1);
	tiles_size = SWAP32(tiles_size);
	
	printf("There are %d tiles\n", num_tiles);
	if (tiles_loaded)
	{
		int tiles_location = SDL_RWtell(file);
		for (int i = 0; i < num_tiles; i++)
		{
			SDL_RWseek(file, tiles_location + tile_offsets[i], SEEK_SET);
			printf("\t(%d) Offset palette %d\n", i, SDL_RWtell(file));
			tiles[i] = new sdl_graphic(0, 0, file, palette, GRAPH_STIL);
		//	char testname[256];
		//	sprintf(testname, "%s-%d.bmp", filename, i);
		//	tiles[i]->make_bmp(testname);
		}
	}
	else
	{
		printf("Loading tile data\n");
		int tiles_location = SDL_RWtell(file);
		for (int i = 0; i < num_tiles; i++)
		{
			SDL_RWseek(file, tiles_location + tile_offsets[i], SEEK_SET);
			printf("\t(%d) Offset normal %d\n", i, SDL_RWtell(file));
			tiles[i] = new sdl_graphic(0, 0, 
				(short*)&data[tiles_location + tile_offsets[i]], 
				GRAPH_STIL);
		//	char testname[256];
		//	sprintf(testname, "%s-%d.bmp", filename, i);
		//	tiles[i]->make_bmp(testname);
		}
	}
	
	SDL_RWclose(file);
	delete [] data;
	
	x_mod = new int*[num_frames];
	y_mod = new int*[num_frames];
	for (int asdf = 0; asdf < num_frames; asdf++)
	{
		x_mod[asdf] = new int[frames[asdf].num_tiles];
		memset(x_mod[asdf], 0, frames[asdf].num_tiles * sizeof(int));
		y_mod[asdf] = new int[frames[asdf].num_tiles];
		memset(y_mod[asdf], 0, frames[asdf].num_tiles * sizeof(int));
	}
	
	printf("Reached end of sprite loading\n");
}

void sprite::draw(SDL_Surface *display)
{
	//amount to shift all tiles by
	int master_x, master_y;
	master_x = frames[frame_num].x1;
	master_y = frames[frame_num].y1;
	
	SDL_Rect tile_location;
	
	int num_tiles = frames[frame_num].num_tiles;
	
	for (int i = 0; i < num_tiles; i++)
	{
		int tempx, tempy;
		tile_location.x = loc_x;// + x_mod[frame_num][i];// + tiles[frames[frame_num].tiles[i].tile]->getx();
		tempx = frames[frame_num].tiles[i].x;// + 
			//frames[frame_num].x1;
		tile_location.y = loc_y;// + y_mod[frame_num][i];// + tiles[frames[frame_num].tiles[i].tile]->getx();
		tempy = frames[frame_num].tiles[i].y;// + 
			//frames[frame_num].y1;
		//tile_location.x += (12 * (tempx + tempy));
		//tile_location.y += (12 * (tempy - tempx));
		
		tile_location.w = 0;
		tile_location.h = 0;
		SDL_BlitSurface(tiles[frames[frame_num].tiles[i].tile]->get_surf(),
			NULL, display, &tile_location);
	}
	
	if ((SDL_GetTicks() + 1000) > time_change)
	{
		frame_num++;
		time_change += 1000;
	}
	if (frame_num == num_frames)
		frame_num = 0;
}

sprite::~sprite()
{
}
