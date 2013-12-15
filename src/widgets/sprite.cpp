#include "client.h"
#include "sprite.h"

sprite::sprite(int x, int y, sdl_user *who)
	: sdl_widget(x, y, who)
{
	delay_mtx = SDL_CreateMutex();
	delay_loading = false;
	loader = 0;
	tiles = 0;
	frames = 0;
	num_frames = 0;
	
	frame_num = 1;
	time_change = SDL_GetTicks() + 200;
}

void sprite::delay_load(int x, int y, char *filename, client *from)
{
	client_request t_sdl;
	t_sdl.request = CLIENT_REQUEST_LOAD_SPRITE;
	t_sdl.data.sload.item = this;
	t_sdl.data.sload.name = filename;
	t_sdl.data.sload.x = x;
	t_sdl.data.sload.y = y;
	delay_load_id = myclient->add_request(t_sdl);
	delay_loading = true;
	loader = myclient;
	visible = true;
}

void sprite::load(int x, int y, char *filename, client *from)
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	printf("Opening sprite %s\n", filename);
	loc_x = x;
	loc_y = y;
	
	bool tiles_loaded = false;
	short *palette = 0;
	int size;
	char *data = (char*)from->getfiles->load_file(filename, &size, FILE_SPRITESPACK, 0);
	
	if (data == 0)
		data = (char*)from->getfiles->load_file(filename, &size, FILE_SPRITEPACK, 0);

	if (data == 0)
	{
		printf("Invalid sprite\n");
		num_frames = 0;
		SDL_mutexV(delay_mtx);
		return;
	}

/*
	printf("File: \n");
	for (int asdf = 0; asdf < size; asdf++)
	{
		if (asdf % 16 == 0)
			printf("\t");
		if (asdf % 32 == 0)
			printf("\n0x%04x: ", asdf);
		printf("%02x ", data[asdf]&0xFF);
	}
	printf("\n");
*/
	SDL_RWops *file = SDL_RWFromMem(data, size);
	
	if (strncmp(data, "SPR_1", 5) == 0)
	{
		printf("Convert frame (SPR_1)\n");
		SDL_RWseek(file, 5, SEEK_CUR);
		SDL_mutexV(delay_mtx);
		throw "Unsupported sprite";
	}
	else if (strncmp(data, "SPR_0", 5) == 0)
	{
		printf("SPR_0 signature found\n");
		SDL_RWseek(file, 5, SEEK_CUR);
	}
	
	char mystery1 = 0;
	uint32_t blank_val;
	
	SDL_RWread(file, &mystery1, 1, 1);
	
	if (mystery1 < 0)
	{
		int pallete_enries = 0;
		SDL_RWread(file, &pallete_enries, 1, 1);
		pallete_enries = SWAP32(pallete_enries);
		if (pallete_enries == 0)
			pallete_enries = 0x100;
		tiles_loaded = true;
//		printf("Pallete_entries: 0x%x @ 0x%x\n", pallete_enries, SDL_RWtell(file));
		palette = new short[pallete_enries];
		for (int i = 0; i < pallete_enries; i++)
		{
			SDL_RWread(file, &palette[i], 2, 1);
			palette[i] = SWAP16(palette[i]);
//			printf("\tSkipping (%d) 0x%04x\n", i, palette[i]);
		}
		
		SDL_RWread(file, &mystery1, 1, 1);
	}
	
	num_frames = mystery1;
	
//	printf("There are %d frames\n", num_frames);
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
		SDL_RWread(file, &blank_val, 4, 1);
		mystery1 = SWAP32(blank_val);
//		printf("Skipped 0x%x (what is this?)\n", mystery1);
		//SDL_RWseek(file, 4, SEEK_CUR);
		SDL_RWread(file, &frames[i].num_tiles, 2, 1);
		frames[i].num_tiles = SWAP16(frames[i].num_tiles);
//		printf("(x1 %d, y1 %d) -> (x2 %d, y2 %d), %d tiles\n", 
//			frames[i].x1, frames[i].y1, 
//			frames[i].x2, frames[i].y2, 
//			frames[i].num_tiles);
		frames[i].tiles = new sprite_tile[frames[i].num_tiles];
		for (int j = 0; j < frames[i].num_tiles; j++)
		{
			SDL_RWread(file, &frames[i].tiles[j].x, 1, 1);
			SDL_RWread(file, &frames[i].tiles[j].y, 1, 1);
			SDL_RWread(file, &frames[i].tiles[j].h, 1, 1);
			SDL_RWread(file, &frames[i].tiles[j].tile, 2, 1);
			frames[i].tiles[j].tile = SWAP16(frames[i].tiles[j].tile);
//			printf("\tframes[?].tiles[?].x,y,h,tile [%d, %d] %d %d\n", 
//				frames[i].tiles[j].x, frames[i].tiles[j].y, 
//				frames[i].tiles[j].h, frames[i].tiles[j].tile);
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
//	printf("Tile size is 0x%08x\n", tiles_size);
	
//	printf("There are %d tiles\n", num_tiles);
	if (tiles_loaded)
	{
		int tiles_location = SDL_RWtell(file);
		for (int i = 0; i < num_tiles; i++)
		{
			SDL_RWseek(file, tiles_location + tile_offsets[i], SEEK_SET);
//			printf("\t(%d) Offset palette 0x%x\n", i, SDL_RWtell(file));
			tiles[i] = new sdl_graphic(0, 0, file, palette, GRAPH_STIL);
		//	char testname[256];
		//	sprintf(testname, "%s-%d.bmp", filename, i);
		//	tiles[i]->make_bmp(testname);
		}
	}
	else
	{
//		printf("Loading tile data\n");
		int tiles_location = SDL_RWtell(file);
		for (int i = 0; i < num_tiles; i++)
		{
			SDL_RWseek(file, tiles_location + tile_offsets[i], SEEK_SET);
//			printf("\t(%d) Offset normal %d\n", i, SDL_RWtell(file));
			tiles[i] = new sdl_graphic(0, 0, 
				(short*)&data[tiles_location + tile_offsets[i]], 
				GRAPH_STIL);
		//	char testname[256];
		//	sprintf(testname, "%s-%d.bmp", filename, i);
		//	tiles[i]->make_bmp(testname);
		}
	}
	
	delete [] palette;
	palette = 0;
	delete [] tile_offsets;
	tile_offsets = 0;
	SDL_RWclose(file);
	delete [] data;
	data = 0;

	frame_num = 0;
	printf("Reached end of sprite loading\n");
	visible = true;
	SDL_mutexV(delay_mtx);
}

//arg2 + frames[frame_num].tiles[i].x - mapX
//arg3 + frames[frame_num].tiles[i].y - mapY
//tile
//h

screen_coord sprite::get_screen()
{
	map_coord temp(loc_x, loc_y);
	return temp.get_screen();
}

void sprite::draw(SDL_Surface *display)
{
	drawat(400, 300, display);
}

void sprite::move(int x, int y)
{
	loc_x = x;
	loc_y = y;
}

void sprite::drawat(int x, int y, SDL_Surface *display)
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	if (num_frames == 0)
	{
		SDL_mutexV(delay_mtx);
		return;
	}
	//amount to shift all tiles by
	int master_x, master_y;
	master_x = frames[frame_num].x1;
	master_y = frames[frame_num].y1;
	
	SDL_Rect tile_location;
	
	int num_tiles = frames[frame_num].num_tiles;

	for (int i = 0; i < num_tiles; i++)
	{
		int tempx, tempy;
		map_coord temp_map(loc_x, loc_y);
		screen_coord temp_screen = temp_map.get_screen();
		tile_location.x = 0;//temp_screen.get_x();
		tile_location.y = 0;//temp_screen.get_y();

		tempx = frames[frame_num].tiles[i].x - 4;
		tempy = frames[frame_num].tiles[i].y + 1;

		tile_location.x += (24 * (tempx/2 + tempy));
		tile_location.y += (12 * (tempy - tempx/2));
		if ((tempx % 2) == 1)
		{
			tile_location.x += 24;
		}
		else if ((tempx % 2) == -1)
		{
			tile_location.y += 12;
		}

		tile_location.x += master_x + tiles[frames[frame_num].tiles[i].tile]->getx() + x;
		tile_location.y += master_y + tiles[frames[frame_num].tiles[i].tile]->gety() + y;

		tile_location.w = 0;
		tile_location.h = 0;
		SDL_BlitSurface(tiles[frames[frame_num].tiles[i].tile]->get_surf(),
			NULL, display, &tile_location);
	}
	
	if ((SDL_GetTicks() + 1000) > time_change)
	{
		frame_num++;
		time_change += 200;
	}
	if (frame_num == num_frames)
		frame_num = 0;
	SDL_mutexV(delay_mtx);
}

sprite::~sprite()
{
	while (SDL_mutexP(delay_mtx) == -1) {};
	if (delay_loading)
	{
		loader->cancel_request(delay_load_id);
	}
	SDL_mutexV(delay_mtx);
	SDL_DestroyMutex(delay_mtx);
	if (frames != 0)
	{
		for (int i = 0; i < num_frames; i++)
		{
			if (frames[i].tiles != 0)
			{
				delete [] frames[i].tiles;
				frames[i].tiles = 0;
			}
		}
		delete [] frames;
		frames = 0;
		num_frames = 0;
	}
	if (tiles != 0)
	{
		for (int i = 0; i < num_tiles; i++)
		{
			if (tiles[i] != 0)
			{
				delete tiles[i];
				tiles[i] = 0;
			}
		}
		delete [] tiles;
		tiles = 0;
		num_tiles = 0;
	}
}