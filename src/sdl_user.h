#ifndef _SDL_USER_H_
#define _SDL_USER_H_

#include <list>
#include <SDL/SDL.h>

#include "config.h"
#include "connection.h"
#include "globals.h"
#include "drawmode/sdl_drawmode.h"
#include "packet.h"
#include "resources/briefcase.h"
#include "resources/files.h"
#include "resources/music.h"
#include "resources/small_font.h"
#include "resources/tile.h"
#include "widgets/sdl_lin_map.h"
#include "widgets/sprite_motion.h"
class draw_loading;
class pack;
class packet_data;
class prepared_graphics;
class sdl_widget;
struct client_request;
struct lin_char_info;

//#define true 1
//#define false 0
#define COLORKEY 255, 0, 255 //Your Transparent colour

int run_client(void *moron);

#define TRANSFER_AMOUNT 0x400

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
	sprite_motion *item;
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
	unsigned int id;	/**< Identifier that identifies the load request */
	CLIENT_REQUEST_TYPE request;	/**< Request identifier */
	union
	{
		client_request_sdl_graphic rload;
		client_request_tile			tload;
		client_request_map			mcheck;
		client_request_sprite		sload;
	} data;
};

void test_func(void * arg);
/**
This class is the gui portion of a game instance. This object belongs to sdl_master. This object has a thread that runs the client portion of the instance. I haven't quite figured out how to consistently determine whether data should be stored here or in the client class so there is bound to be some inconsistencies in the placement depending on how I felt that day.
 */
class sdl_user
{
	public:
		sdl_user(Uint32 flags = SDL_SWSURFACE);
		~sdl_user();
		
		//for tracking mouse movements 
		void mouse_to(SDL_MouseMotionEvent *to);
		void mouse_from(SDL_MouseMotionEvent *from);
		void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		bool quit_request(); /**< Is it ok for me to quit? */
		bool mouse_leave();	/**< is it ok for the mouse to leave my drawing area? */
				
		void mouse_click(SDL_MouseButtonEvent *here);
		void key_press(SDL_KeyboardEvent *button);
		
		void draw(SDL_Surface *display);	/**< draw */

		void quit_client();	/**< Quit the game instance by triggering the client thread through the client object */
		void login(const char *name, const char *pass); /**< login to the game server */

		tile *get_tiles();	/**< Retrieve the list of tiles. */

		void wait_for_mode(enum drawmode wait, bool mutex);	/**< wait for the client to be in the given drawmode */
		bool is_in_mode(enum drawmode here, bool mutex);	/**< is the client in the given drawmode */
		sdl_drawmode *get_drawmode(bool mutex);	/**< Retrieve the drawmode object */
		void done_with_drawmode();	/**< Unlocks the drawmode mutex */
		bool are_you_ready();	/**< wait until the ready flag is set */
		void check_for_change_drawmode();	/**< This actually performs the drawmode change */
		volatile bool done;	/**<used to terminate the client */
		music game_music; /**< the game music object */

		unsigned int add_request(client_request rqst);	/**< The function used by the sdl_user class */
		void cancel_request(unsigned int id);	/** Cancel a specific load request */
		void delete_requests();	/**< This function deletes all pending requests */

		int init_tiles();	/**< Initializes the map_tiles object */
		void stop();	/**< Used to stop the thread that talks to the server*/

		void LoadDurationTable(const char *file);
		void init_codepage(unsigned int arg);

		void create_chars(int used, int usable, int slots);	/**< Create the blank slots for character select*/
		void register_char(lin_char_info *data);	/**< Put character information into one of the slots for character select */
		int check_login_chars();	/**< Returns the number of characters stored */
		lin_char_info** get_login_chars();	/**< Returns the array of characters for character select */

		small_font smallfont;	/**< A font object */

		char convert_client_packets[256];	/**< Used to translate packet opcodes */
		char convert_server_packets[256];	/**< Used to translate packet opcodes */
		
		void init();	/**< Performs the typical game startup that is server specific */
		int run();	/**< This is the top level function of client that runs in a thread */
		int process_packet();	/**< Performs processing on packets received from the server */
		void send_packet(packet_data &sendme);	/**< Sends a packet to the server */
		
		void change_map(int map_num);	/**< Changes the current map */
		void change_drawmode(enum drawmode chg);	/**< Change the current drawmode */

		files *getfiles;	/**< The object responsible for retrieving all file based game resources */
		briefcase *server_data;	/**< used to hold all server specific data */
		//these two variables are used to translate packets
			//for servers using different opcodes
		
		config *get_config();	/**< Used to retrieve the configuration object used by this server */
	private:
		config *main_config;	/**< The configuration used by the server */
		connection *server;	/**< The connection to the server */
		packet *proc; /**< The object used to communicate with the server */

		volatile bool ready;	/**< is it ok to draw me */
		volatile enum drawmode draw_mode;	/**< what is the current drawing mode */
		enum drawmode change_draw;

		sdl_drawmode *drawmode;	/**< the object that draws everything for me */
		tile *map_tiles;	/**< The array of all possible tiles a map can use, not all of them are loaded though */
		int number_map_tiles;	/**< The number of map tiles covered by the map_tiles object */

		void init_math_tables();	/**< Unimportant function */
		int getNumber(char **buf);	/**< I dont remember what this does */

		lin_char_info **login_opts;	/**< The array of characters for the character select screen */
		int num_login_opts;	/**< number of character slots for the character select screen */
		int login_opts_stored;	/**< Number of characters actually on the character select screen */
		volatile int login_opts_used; /**< Number of characters that are supposed to be on the character select screen */

		SDL_mutex *draw_mtx;	/**< the mutex the gui thread (sdl_master) uses to share data with the client thread */
		SDL_mutex *mode_mtx;	/**< this mutex is used to protect access to the drawmode */

		void check_requests();	/**< Check for any requests that have been made */
		void free_request(client_request *temp);
		SDL_mutex *requests_mtx;	/**< The mutex that prevents checks and requests from happening at the same time */
		unsigned int request_id;
		std::list<client_request*> request_list;

		volatile bool wait_for_user;	/** A variable that waits for the user to be fully deleted */
		volatile bool stop_thread;	/**< A variable that indicates the client thread should stop */
		
		int get_updates(connection* server, draw_loading *scrn);	/**< Retrieve updates from the update server. Only updates the briefcase */
		int pack_resources();	/**< Not implemented yet */
		int init_packs();	/**< Performs some basic initialization of packs */
		void delete_packs();	/**< Helper for deleting packs */
		int init_strings();	/**< Initializes string data used by the game */
		char *server_name;	/**< A string with the name of the server */

		table bad_words;
		table strings;
		table teleport;
		table un_transaction;
		table important_items;
		table pet;
		table solvent;
		table todays_tip;

		int acp[2];	//-1, 0xec8a0 (a function pointer to badwords compare?)
		unsigned int serverId;	//0
		unsigned char countryCode;
		void common_construct();
		void common_destruct();
};

#endif
