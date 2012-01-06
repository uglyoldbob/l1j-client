#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "sdl_user.h"
#include "config.h"
#include "connection.h"
#include "resources/briefcase.h"
#include "resources/table.h"
#include "resources/tile.h"
#include "resources/pack.h"

#define TRANSFER_AMOUNT 0x400

int run_client(void *moron);

class client
{
	public:
		client(sdl_user *stuff);
		~client();
		void init();
		int process_packet();
		void send_packet(const char *format, ...);
		
		//unsorted public functions
		void LoadDurationTable(const char *file);
		void init_codepage(unsigned int arg);
		
		void create_chars(int used, int usable, int slots);
		void register_char(lin_char_info *data);
		int check_login_chars();
		lin_char_info** get_login_chars();
		sdl_user *graphics;
		
	private:
		config *main_config;
		connection *server;
		briefcase *server_data;	//used to hold all server specific data
		pack *textpack;
		pack *tilepack;
		pack **spritepack;
		int num_sprite_pack;
		table bad_words;
		table strings;
		table teleport;
		table un_transaction;
		table important_items;
		table pet;
		table solvent;
		table todays_tip;
		tile *map_tiles;
		int number_map_tiles;
		
		char *server_name;
		
		lin_char_info **login_opts;
		int num_login_opts;	//number of elements present
		int login_opts_stored;
		volatile int login_opts_used;
		
		int acp[2];	//-1, 0xec8a0 (a function pointer to badwords compare?)
		unsigned int serverId;	//0
		unsigned char countryCode;

		int get_updates(connection* server);
		int old_get_updates(connection* server);
		int pack_resources();
		int init_packs();
		int init_strings();
		int init_tiles();
		
		//unsorted functions
		void init_math_tables();
		int getNumber(char **buf);
};

#endif