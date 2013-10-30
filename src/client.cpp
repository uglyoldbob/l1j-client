#include "sdl_user.h"
#include "client.h"
#include "config.h"
#include "connection.h"
#include "drawmode/draw_char_sel.h"
#include "drawmode/draw_game.h"
#include "drawmode/draw_loading.h"
#include "drawmode/sdl_drawmode.h"
#include "globals.h"
#include "lindes.h"
#include "packet.h"
#include "resources/briefcase.h"
#include "resources/music.h"
#include "resources/pack.h"
#include "resources/partial_table.h"
#include "resources/table.h"
#include "unsorted.h"

#include <math.h>

int client::pack_resources()
{
	return 0;
}

int client::init_tiles()
{
	int last_good = 100;
	for (int i = 0; i < (last_good * 2); i++)
	{
		char fname[100];
		sprintf(fname, "%d.til", i);
		if (tilepack->check_file(fname) != -1)
		{	//the tileset is good
			if (i > last_good)
				last_good = i;
		}
	}
	number_map_tiles = last_good;
	map_tiles = new tile[number_map_tiles];
	
	return 0;
}

tile* client::get_tiles()
{
	return map_tiles;
}

int client::init_packs()
{
	textpack = new pack("Text", 1);
	tilepack = new pack("Tile", 0);
	spritepack = new pack*[num_sprite_pack];
	spritepack[0] = new pack("Sprite", 0);
	for (int i = 0; i < (num_sprite_pack-1); i++)
	{
		char name[10];
		sprintf(name, "Sprite%02d", i);
		spritepack[i+1] = new pack(name, 0);
		//new_surf_pack and spritepack are the same thing
		//new_icon_pack and spritepack are the same thing
		//spriteFile is simply the first element of spritepack
	}
	//TODO: verify all packs were loaded successfully
	
	return 0;
}

int client::init_strings()
{
	int acp = -1;
	if (acp == -1)
	{
//		printf("STUB GetACP()\n");
		//acp = GetACP();
	}
	
	partial_table test;
	test.load_local("itemdesc", textpack);
	
	//list of filtered chat words
	bad_words.load_local("obscene", textpack);
	bad_words.sort();
	
	//list of tips for new players?
	todays_tip.load_local("todaystip", textpack);
//	printf("STUB Load MercenaryIconData\n");
//	printf("STUB Load MagicDollData\n");
	
	//list of items
	solvent.load_local("solvent", textpack);
	
	//list of pet types
	pet.load_local("ntexpet", textpack);
	
	//no important items?
	important_items.load("itemimportant.tbl", textpack);
	
//	printf("STUB Load BaseStatus\n");
	
	//unknown
	teleport.load("telbook.tbl", textpack);
	
	//unknown
	un_transaction.load("untransaction.tbl", textpack);
	
//	if (battleServer != 0)
//	{
//		table notices;
//		notices.load("notice_bs.tbl");
//		notices.print();
//	}

	strings.load_local("string", textpack);
	
	return 0;
}

int client::get_updates(connection* server, draw_loading *scrn)
{
	packet *proc;
	unsigned char temp;
	unsigned short temp2;
	unsigned int num_files;
	int status;	//> 0 means update occurred
	char hash[65];	//the hash is 64 bytes plus a null terminator
	char display[256];	//the string for displaying stuff
	status = 0;
	try
	{
		if (server->connection_ok() == 1)
		{
			proc = new packet(this, server);	//init the packet class instance
			server_data = new briefcase(server_name);
			strcpy(hash, server_data->get_hash());
			temp2 = 0x4400;	//68 bytes of packet data
			server->snd_var(&temp2, 2);
			temp = 1;
			server->snd_var(&temp, 1);
			printf("The hash is: %s\n", hash);
			server->snd_var(hash, 65);
			proc->reset();
			proc->get_packet(false);
			proc->disass("cd", &temp, &num_files);
			if ((temp == 2) || 
				(strcmp(hash, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") == 0) )
			{
				server_data->new_data();
				if (temp == 2)
				{
					proc->reset();
					proc->get_packet(false);
					proc->disass("cd", &temp, &num_files);
				}
			}
			else
			{
				server_data->add_data();
			}
			for (unsigned int i = 0; i < num_files; i++)
			{
				unsigned char file_buffer[TRANSFER_AMOUNT];	//buffer space
				char *file;
				unsigned int offset = 0;
				unsigned int filesize, orig_filesize;
				char *filename;
				proc->reset();
				proc->get_packet(false);
				proc->disass("csd", &temp, &filename, &filesize);
				orig_filesize = filesize;
				file = new char[filesize];
				sprintf(display, "Receiving %s size %d (%d of %d)", filename, filesize, i+1, num_files);
				scrn->add_text(display);
				while (filesize > 0)
				{
					if (filesize > TRANSFER_AMOUNT)
					{
						server->rcv(file_buffer, TRANSFER_AMOUNT);
						memcpy(&file[offset], file_buffer, TRANSFER_AMOUNT);
						offset += TRANSFER_AMOUNT;
						filesize -= TRANSFER_AMOUNT;
					}
					else
					{
						server->rcv(file_buffer, filesize);
						memcpy(&file[offset], file_buffer, filesize);
						offset += filesize;
						filesize = 0;
					}
				}
				server_data->write_file(filename, file, orig_filesize);
			}
			server_data->finish_briefcase();
			delete server_data;
			server_data = new briefcase(server_name);
			status = num_files;
			delete proc;
		}			
	}
	catch(int e)
	{
		printf("An error occurred %d\n", e);
		status = -1;
	}
	return status;
}

client::client(sdl_user *stuff)
{
	stop_thread = false;
	getfiles = new files(this);
	login_opts = 0;
	num_login_opts = 0;
	login_opts_used = 0;
	login_opts_stored = 0;
	main_config = 0;
	graphics = stuff;
	server = 0;
	server_data = 0;
	num_sprite_pack = 17;
	for (int i = 0; i < 256; i++)
	{	//this loop sets up a no translation scheme for default
		convert_client_packets[i] = i;
		convert_server_packets[i] = i;
	}
}

client::~client()
{
	delete getfiles;
	if (server != 0)
		delete server;
	if (main_config != 0)
		delete main_config;
	
	if (num_login_opts > 0)
	{
		for (int i = 0; i < num_login_opts; i++)
		{
			delete login_opts[i];
		}
		delete [] login_opts;
	}

	if (map_tiles != 0)
		delete [] map_tiles;
}

void client::init()
{
	main_config = new config("Lineage.ini");
	if (!main_config->config_ok())
	{
		delete main_config;
		main_config = 0;
		throw "ERROR Loading configuration file.\n";
	}
	char *test;
	test = (char*)getfiles->load_file("Sprite00.idx", 0, FILE_REGULAR1, 0);
	if (test == 0)
	{
		throw "Lineage Data not found";
	}
	delete [] test;
	lineage_font.init("Font/eng.fnt", this);	//TODO : make a client specific version of the font
	smallfont.init("Font/SMALL.FNT", this);
	
	DesKeyInit("~!@#%^$<");	//TODO : move this code to a class and use an object
	init_packs();
	init_tiles();
	
	int spritesize;
	short *sprite_test = (short*)getfiles->load_file("2786-284.spr", &spritesize, FILE_SPRITESPACK, 0);
	
	if (sprite_test != 0)
	{	
		printf("Size: %d\n\t", spritesize);
		spritesize /= 2;
		spritesize = (int)sqrt(spritesize) + 1;
		printf("Size %d x %d = %d\n", spritesize, spritesize, spritesize * spritesize * 2);
		SDL_Surface *spriteing = SDL_CreateRGBSurfaceFrom(sprite_test, spritesize, spritesize, 16, spritesize*2, 
			0x7C00, 0x03E0, 0x001F, 0);
		SDL_SaveBMP(spriteing, "sprite.bmp");
	}
	else
	{
		printf("File not found\n");
	}
	graphics->change_drawmode(DRAWMODE_LOADING);

	graphics->wait_ready();
	draw_loading *load;
	int what_server;
	load = (draw_loading*)graphics->get_drawmode();

	//wait for the user to pick a server
	what_server = load->wait_server_pick();
	
	server_name = new char[strlen(main_config->get_name(what_server)) + 1];
	strcpy(server_name, main_config->get_name(what_server));
	
	server = new connection(main_config, what_server);
	if (get_updates(server, load) > 0)
	{
	}

	//check for custom opcodes
	unsigned char *copcodes;
	copcodes = (unsigned char*)getfiles->load_file("opcodes.txt", 0, FILE_REGULAR3, 0);
	if (copcodes != 0)
	{	//there are custom opcodes for this server
		int offset;
		char *data = (char*)copcodes;
		for (int i = 0; i < 256; i++)
		{
			char *p;
			p = strtok(data, "\n");
			if (data != NULL)
				data = NULL;
			int temp;
			sscanf(p, "%d", &temp);
			convert_client_packets[i] = (unsigned char)temp; 
		}
		for (int i = 0; i < 256; i++)
		{
			char *p;
			p = strtok(data, "\n");
			if (data != NULL)
				data = NULL;
			int temp;
			sscanf(p, "%d", &temp);
			convert_server_packets[i] = (unsigned char)temp;
		}
	}

	//begin game portion of client
	if (server->change() != 1)
	{
		throw "Failed to connect to game server\n";
	}

	init_codepage(0);
	init_math_tables();
//	printf("STUB Load player config\n");
//	printf("STUB Initialize emblem cache\n");
	init_strings();
	load->load_done();
}

int client::process_packet()
{
	packet bob(this, server);
	
	bob.get_packet(true);
	return bob.process_packet();
}

void client::send_packet(const char *format, ...)
{
	va_list temp_args;
	va_start(temp_args, format);
	
	packet bob(this, server);
	bob.send_packet(format, temp_args);			
	va_end(temp_args);
}

void client::change_map(int map_num)
{
	if (graphics->is_in_mode(DRAWMODE_GAME))
	{
		draw_game *dg = (draw_game*)graphics->get_drawmode();
		dg->change_map(map_num);
	}
}

int client::check_login_chars()
{
	return login_opts_stored;
}

void client::create_chars(int used, int usable, int slots)
{	//creates the array and makes them all blank
	if (num_login_opts == 0)
	{
		num_login_opts = usable;	//the unusable slots will not be modified
		login_opts_used = used;
		login_opts = new lin_char_info*[slots];
		for (int i = 0; i < slots; i++)
		{
			login_opts[i] = 0;
		}
		for (int i = num_login_opts; i < slots; i++)
		{	
			//possibly initialize them so they will display as not usable
			printf("STUB: Slot %d is not usable\n", i+1);
		}
	}
}

config *client::get_config()
{
	return main_config;
}

lin_char_info** client::get_login_chars()
{	//retrieves the entire array
	return login_opts;
}

void client::register_char(lin_char_info *data)
{	//puts character data into the array
	if ((num_login_opts > 0) && (data != 0))
	{
		int i;
		for (i = 0; i <= num_login_opts; i++)
		{
			if (login_opts[i] == 0)
				break;
		}
		if (i < num_login_opts)
		{
			login_opts[i] = data;
			login_opts_stored++;
		}
	}
	if (login_opts_stored == login_opts_used)
	{
		draw_char_sel *bob;
		bob = (draw_char_sel *)graphics->get_drawmode();
		graphics->wait_for_mode(DRAWMODE_CHARSEL);
		bob->get_login_chars();
	}
}

void client::stop()
{
	stop_thread = true;
}

int run_client(void *moron)
{	//the main thread for each client
	client *game = new client((sdl_user*)moron);
	sdl_user *temp;
	temp = (sdl_user*)moron;
	temp->init_client(game);
	try
	{
		game->init();
		while (game->process_packet() == 0)
		{
		}
	}
	catch (const char *error)
	{
		printf("FATAL ERROR: %s\n", error);
	}
	game->graphics->done = true;
	delete game;
	return 0;
}
