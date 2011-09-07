#include "sdl_user.h"
#include "client.h"
#include "config.h"
#include "connection.h"
#include "drawmode/draw_char_sel.h"
#include "drawmode/draw_loading.h"
#include "drawmode/sdl_drawmode.h"
#include "globals.h"
#include "lindes.h"
#include "packet.h"
#include "resources/music.h"
#include "resources/pack.h"
#include "resources/partial_table.h"
#include "resources/table.h"
#include "unsorted.h"

int client::pack_resources()
{
	return 0;
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
		printf("STUB GetACP()\n");
		//acp = GetACP();
	}
	
	partial_table test;
	test.load_local("itemdesc", textpack);
	
	//list of filtered chat words
	bad_words.load_local("obscene", textpack);
	bad_words.sort();
	
	//list of tips for new players?
	todays_tip.load_local("todaystip", textpack);
	printf("STUB Load MercenaryIconData\n");
	printf("STUB Load MagicDollData\n");
	
	//list of items
	solvent.load_local("solvent", textpack);
	
	//list of pet types
	pet.load_local("ntexpet", textpack);
	
	//no important items?
	important_items.load("itemimportant.tbl", textpack);
	
	printf("STUB Load BaseStatus\n");
	
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

int client::get_updates(connection* server)
{
	unsigned int temp;
	int sign_temp;
	int status;	//> 0 means update occurred
	status = 0;
	printf("STUB Get update magic number\n");
	try
	{
		if (server->connection_ok() == 1)
		{
			server->snd_var(&checksum, 4);
			server->rcv_var(&sign_temp, 4);
			if (sign_temp < 0)
			{
				temp = -sign_temp;
				printf("Protocol : %lx, ServerId : %lx\n", temp & 0xFFFF, temp>>16);
				server->rcv_var(&sign_temp, 4);
			}
			if (sign_temp > 0)
			{	//receive files
				int num_files = sign_temp;
				printf("Receiving %d files\n", num_files);
				status = num_files;
				unsigned char name_length;
				unsigned int file_length;
				unsigned char* file_buffer;
				char *filename;
				for (int i = 0; i < num_files; i++)
				{
					server->rcv(&name_length, 1);
					filename = new char[name_length+1];
					server->rcv(filename, name_length);
					filename[name_length] = 0;
					server->rcv_var(&file_length, 4);
					file_buffer = new unsigned char[file_length+1];
					printf("Downloading %s, %ld bytes ...", filename, file_length);
					//do stuff so the file can be saved
					char *dump_name;
					dump_name = new char[name_length + 6];
					sprintf(dump_name, "%s.gz", filename);
//					FILE *filedump = fopen(dump_name, "wb");
					while (file_length > 0)
					{
						if (file_length > TRANSFER_AMOUNT)
						{
							server->rcv(file_buffer, TRANSFER_AMOUNT);
//							fwrite(file_buffer, 1, TRANSFER_AMOUNT, filedump);
							file_length -= TRANSFER_AMOUNT;
						}
						else
						{
							server->rcv(file_buffer, file_length);
//							fwrite(file_buffer, 1, file_length, filedump);
							file_length = 0;
						}
					}
//					fclose(filedump);
					printf(" done\n");
					printf("STUB Update magic number\n");
					server->rcv_var(&sign_temp, 4);
					delete [] dump_name;
					delete [] filename;
					delete [] file_buffer;
				}
			}
			int num_servers;
			unsigned short* num_users;
			server->rcv_var(&num_servers, 4);
			num_users = new unsigned short[num_servers];
			for (int j = 0; j < num_servers; j++)
				server->rcv_var(&num_users[j], 2);
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
	login_opts = 0;
	num_login_opts = 0;
	login_opts_used = 0;
	login_opts_stored = 0;
	main_config = 0;
	graphics = stuff;
	server = 0;
	checksum = 0xdeadbeef;
	num_sprite_pack = 17;
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

	DesKeyInit("~!@#%^$<");	//TODO : move this code to a class and use an object
	init_packs();
	
	graphics_data *temp = new graphics_data;
	temp->tilepack = tilepack;
	temp->spritepack = spritepack;
	temp->num_sprite_pack = num_sprite_pack;
	graphics->give_data(temp);	//pack files will not be created or delete during updates

	graphics->wait_ready();
	
	init_codepage(0);
	init_math_tables();
	printf("STUB Load player config\n");
	printf("STUB Initialize emblem cache\n");
	init_strings();
}

client::~client()
{
	printf("Exiting client now\n");
	if (server != 0)
		delete server;
	if (main_config != 0)
		delete main_config;
		
	delete textpack;
	delete tilepack;
	for (int i = 0; i < num_sprite_pack; i++)
	{
		delete spritepack[i];
	}
	delete [] spritepack;
}

int run_client(void *moron)
{	//the main thread for each client
	client game((sdl_user*)moron);
	sdl_user *temp;
	temp = (sdl_user*)moron;
	temp->init_client(&game);
	try
	{
		game.init();
		while (1)
		{
			SDL_Delay(1000);
		}
	}
	catch (const char *error)
	{
		printf("FATAL ERROR: %s", error);
	}
	printf("Client is done\n");
	return 0;
}