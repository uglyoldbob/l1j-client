#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "sdl_user.h"
#include "config.h"
#include "connection.h"
#include "resources/briefcase.h"
#include "resources/files.h"
#include "resources/table.h"
#include "resources/tile.h"
#include "resources/sdl_font.h"
#include "resources/small_font.h"
#include "resources/pack.h"

#define TRANSFER_AMOUNT 0x400

class draw_loading;

int run_client(void *moron);

class client
{
	public:
		client(sdl_user *stuff);
		~client();
		void init();	/**< Performs the typical game startup that is server specific */
		int process_packet();	/**< Performs processing on packets received from the server */
		void send_packet(const char *format, ...);	/**< Sends a packet to the server */
		
		void change_map(int map_num);	/**< Changes the current map */
		
		//unsorted public functions
		void LoadDurationTable(const char *file);
		void init_codepage(unsigned int arg);
		
		void create_chars(int used, int usable, int slots);	/**< Create the blank slots for character select*/
		void register_char(lin_char_info *data);	/**< Put character information into one of the slots for character select */
		int check_login_chars();	/**< Returns the number of characters stored */
		lin_char_info** get_login_chars();	/**< Returns the array of characters for character select */
		tile *get_tiles();	/**< Returns the array of tilesets used for maps */
		
		sdl_user *graphics;	/**< The sdl_user object that handles our gui stuff */
		
		files *getfiles;	/**< The object responsible for retrieving all file based game resources */
		briefcase *server_data;	/**< used to hold all server specific data */
		
		//these two variables are used to translate packets
			//for servers using different opcodes
		char convert_client_packets[256];	/**< Used to translate packet opcodes */
		char convert_server_packets[256];	/**< Used to translate packet opcodes */
		config *get_config();	/**< Used to retrieve the configuration object used by this server */
		void stop();	/**< */
		bool stop_thread;	/**< A variable that indicates the client thread should stop */
		small_font smallfont;	/**< A font object */
	private:
		config *main_config;	/**< The configuration used by the server */
		connection *server;	/**< The connection to the server */
		table bad_words;
		table strings;
		table teleport;
		table un_transaction;
		table important_items;
		table pet;
		table solvent;
		table todays_tip;
		tile *map_tiles;	/**< The array of all possible tiles a map can use, not all of them are loaded though */
		
		int number_map_tiles;	/**< The number of map tiles covered by the map_tiles object */
		
		char *server_name;	/**< A string with the name of the server */
		
		lin_char_info **login_opts;	/**< The array of characters for the character select screen */
		int num_login_opts;	/**< number of character slots for the character select screen */
		int login_opts_stored;	/**< Number of characters actually on the character select screen */
		volatile int login_opts_used; /**< Number of characters that are supposed to be on the character select screen */
		
		int acp[2];	//-1, 0xec8a0 (a function pointer to badwords compare?)
		unsigned int serverId;	//0
		unsigned char countryCode;

		int get_updates(connection* server, draw_loading *scrn);	/**< Retrieve updates from the update server. Only updates the briefcase */
		int old_get_updates(connection* server); /**< No longer used */
		int pack_resources();	/**< Not implemented yet */
		int init_packs();	/**< Performs some basic initialization of packs */
		int init_strings();	/**< Initializes string data used by the game */
		int init_tiles();	/**< Initializes the map_tiles object */
		
		//unsorted functions
		void init_math_tables();	/**< Unimportant function */
		int getNumber(char **buf);	/**< I dont remember what this does */
};

#endif
