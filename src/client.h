#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <queue>

#include "sdl_user.h"
#include "config.h"
#include "connection.h"
#include "globals.h"
#include "resources/briefcase.h"
#include "resources/files.h"
#include "resources/sdl_font.h"
#include "resources/sdl_graphic.h"
#include "resources/table.h"
#include "resources/small_font.h"
#include "resources/pack.h"
#include "widgets/sdl_lin_map.h"
#include "widgets/sprite.h"

#define TRANSFER_AMOUNT 0x400

class draw_loading;

int run_client(void *moron);

/** This enumeration defines what the request for the client is for. It defines which structure to use */
enum CLIENT_REQUEST_TYPE
{
	CLIENT_REQUEST_CHECK_MAP,
	CLIENT_REQUEST_LOAD_SDL_GRAPHIC,
	CLIENT_REQUEST_LOAD_SPRITE,
	CLIENT_REQUEST_LOAD_TILE,
	CLIENT_REQUEST_QUIT
};

enum CLIENT_REQUEST_LOAD_TYPE
{
	CLIENT_REQUEST_LOAD_1,
	CLIENT_REQUEST_LOAD_2,
	CLIENT_REQUEST_LOAD_3,
	CLIENT_REQUEST_LOAD_4,
};

void do_load(int x, int y, short *source, int type);
void do_load(int x, int y, SDL_RWops *source, short *palette, int type);
void do_load(char *name, int x, int y, int type, client * who = 0);
void do_load(int num, int x, int y, int type, client * who = 0);

struct client_request_sdl_graphic
{
	CLIENT_REQUEST_LOAD_TYPE load_type;	/**< This determines what type of load to perform */
	sdl_graphic *item;	/**< The item that gets loaded */
	char *name;	/**< Name of the file to use */
	int num;	/**< Number of the file to use */
	int x, y;	/**< Coordinates for the graphic */
	int type;
	short *buffer;	/**< A memory buffer for loading data from */
	short *palette;	/**< A memory buffer for palette data */
	SDL_RWops *source;	/**< An alternate place to get data from */
};

struct client_request_tile
{
	tile *item;
	int which;
};

struct client_request_sprite
{
	sprite *item;
	char *name;
	int x;
	int y;
};

struct client_request_map
{
	sdl_lin_map *item;
};

/** This is a structure that contains data used to make a request to the client */
struct client_request
{
	CLIENT_REQUEST_TYPE request;	/**< Request identifier */
	union
	{
		client_request_sdl_graphic rload;
		client_request_tile			tload;
		client_request_map			mcheck;
		client_request_sprite		sload;
	} data;
};

class client
{
	public:
		client(sdl_user *stuff);
		~client();
		void init();	/**< Performs the typical game startup that is server specific */
		int run();	/**< This is the top level function of client that runs in a thread */
		int process_packet();	/**< Performs processing on packets received from the server */
		void send_packet(const char *format, ...);	/**< Sends a packet to the server */
		
		void change_map(int map_num);	/**< Changes the current map */
		void change_drawmode(enum drawmode chg);	/**< Change the current drawmode */
		
		//unsorted public functions
		void LoadDurationTable(const char *file);
		void init_codepage(unsigned int arg);
		
		void create_chars(int used, int usable, int slots);	/**< Create the blank slots for character select*/
		void register_char(lin_char_info *data);	/**< Put character information into one of the slots for character select */
		int check_login_chars();	/**< Returns the number of characters stored */
		lin_char_info** get_login_chars();	/**< Returns the array of characters for character select */

		files *getfiles;	/**< The object responsible for retrieving all file based game resources */
		briefcase *server_data;	/**< used to hold all server specific data */
		void add_request(client_request rqst);	/**< The function used by the sdl_user class */
		void delete_requests();	/**< This function deletes all pending requests */
		
		//these two variables are used to translate packets
			//for servers using different opcodes
		
		char convert_client_packets[256];	/**< Used to translate packet opcodes */
		char convert_server_packets[256];	/**< Used to translate packet opcodes */
		config *get_config();	/**< Used to retrieve the configuration object used by this server */
		void stop();	/**< */
		small_font smallfont;	/**< A font object */
	private:
		config *main_config;	/**< The configuration used by the server */
		connection *server;	/**< The connection to the server */
		sdl_user *graphics;	/**< The sdl_user object that handles our gui stuff */
		table bad_words;
		table strings;
		table teleport;
		table un_transaction;
		table important_items;
		table pet;
		table solvent;
		table todays_tip;
		volatile bool stop_thread;	/**< A variable that indicates the client thread should stop */

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
		void delete_packs();	/**< Helper for deleting packs */
		int init_strings();	/**< Initializes string data used by the game */

		//unsorted functions
		void init_math_tables();	/**< Unimportant function */
		int getNumber(char **buf);	/**< I dont remember what this does */
		
		void check_requests();	/**< Check for any requests that have been made */
		SDL_mutex *requests_mtx;	/**< The mutex that prevents checks and requests from happening at the same time */
		std::queue<client_request*> request_queue;
};

#endif
