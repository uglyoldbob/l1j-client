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
	graphics->init_tiles();

	graphics->change_drawmode(DRAWMODE_LOADING);

	while (!graphics->are_you_ready())
	{
		check_requests();
	}
	draw_loading *load;
	int what_server;
	load = (draw_loading*)graphics->get_drawmode();

	//wait for the user to pick a server
	do
	{
		what_server = load->get_server_pick();
		check_requests();
	} while (what_server == -1);
	
	server_name = new char[strlen(main_config->get_name(what_server)) + 1];
	strcpy(server_name, main_config->get_name(what_server));

	server = new connection(main_config, what_server);
//	if (get_updates(server, load) > 0)
//	{
//	}

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
		delete [] copcodes;
		copcodes = 0;
	}

	//begin game portion of client
	if (server->connection_ok() == 1)
	{
		if (server->change() != 1)
		{
			throw "Failed to connect to game server\n";
		}
	}

	init_codepage(0);
	init_math_tables();
//	printf("STUB Load player config\n");
//	printf("STUB Initialize emblem cache\n");
	init_strings();
	load->load_done();
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
			//proc->reset();
			packet_data temp_packet = proc->get_packet(false);
			temp_packet >> temp >> num_files;
			if ((temp == 2) || 
				(strcmp(hash, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") == 0) )
			{
				printf("Creating new briefcase\n");
				server_data->new_data();
				if (temp == 2)
				{
					//proc->reset();
					temp_packet = proc->get_packet(false);
					temp_packet >> temp >> num_files;
				}
			}
			else
			{
				printf("Adding to existing briefcase\n");
				server_data->add_data();
			}
			for (unsigned int i = 0; i < num_files; i++)
			{
				unsigned char file_buffer[TRANSFER_AMOUNT];	//buffer space
				char *file;
				unsigned int offset = 0;
				unsigned int filesize, orig_filesize;
				char *filename;
				//proc->reset();
				packet_data temp_packet = proc->get_packet(false);
				temp_packet >> temp >> filename >> filesize;
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
				delete [] file;
				file = 0;
				delete [] filename;
				filename = 0;
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
		graphics->wait_for_mode(DRAWMODE_CHARSEL);
		bob = (draw_char_sel *)graphics->get_drawmode();
		bob->get_login_chars();
	}
}

void client::change_map(int map_num)
{
	if (graphics->is_in_mode(DRAWMODE_GAME))
	{
		draw_game *dg = (draw_game*)graphics->get_drawmode();
		dg->change_map(map_num);
	}
}

int client::process_packet()
{
	packet bob(this, server);
	
	bob.get_packet(true);
	return bob.process_packet();
}

void client::send_packet(packet_data sendme)
{
	packet bob(this, server);
	bob.send_packet(sendme);
}

int client::run()
{
	try
	{
		init();
		while (process_packet() == 0)
		{
			check_requests();
		}
	}
	catch (const char *error)
	{
		printf("FATAL ERROR: %s\n", error);
	}
	graphics->done = true;
	return 0;
}
