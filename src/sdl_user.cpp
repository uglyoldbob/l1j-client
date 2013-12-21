#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "drawmode/draw_char_sel.h"
#include "drawmode/draw_game.h"
#include "drawmode/draw_new_char.h"
#include "drawmode/draw_loading.h"
#include "drawmode/draw_login.h"
#include "globals.h"
#include "lindes.h"
#include "packet.h"
#include "resources/sdl_font.h"
#include "sdl_user.h"

/** signal the client thread through the client object that we are quitting */
void sdl_user::quit_client()
{
	stop();
	done = true;
}

int sdl_user::check_login_chars()
{
	return login_opts_stored;
}

lin_char_info** sdl_user::get_login_chars()
{	//retrieves the entire array
	return login_opts;
}

/** Initiate the login into the game server 
 \todo Move this function entirely to the client class */
void sdl_user::login(const char *name, const char *pass)
{
	packet_data sendme;
	sendme << ((uint8_t)CLIENT_LOGIN) << name << pass;
	send_packet(sendme);
}

/** Initialize ourself, create our screen, etc.. */
sdl_user::sdl_user(Uint32 flags)
{
	draw_mode = INVALID;
	change_draw = INVALID;
	done = false;
	
	draw_mtx = SDL_CreateMutex();
	mode_mtx = SDL_CreateMutex();

	ready = false;

	drawmode = 0;
	map_tiles = 0;
	common_construct();
}

int sdl_user::init_tiles()
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
	if (number_map_tiles > 0)
	{
		map_tiles = new tile[number_map_tiles];
	}
	else
	{
		map_tiles = 0;
	}
	smallfont.init("Font/SMALL.FNT", this);
	return 0;
}

/** cleanup */
sdl_user::~sdl_user()
{
	printf("Deleting the sdl_user\n");
	SDL_DestroyMutex(draw_mtx);
	SDL_DestroyMutex(mode_mtx);
	draw_mtx = 0;
	mode_mtx = 0;
	if (drawmode != 0)
	{
		delete drawmode;
		drawmode = 0;
	}
	if (map_tiles != 0)
	{
		delete [] map_tiles;
		map_tiles = 0;
	}
	common_destruct();
}

tile *sdl_user::get_tiles()
{
	return map_tiles;
}

void sdl_user::mouse_to(SDL_MouseMotionEvent *to)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		drawmode->mouse_to(to);
	}
	SDL_mutexV(draw_mtx);
}

void sdl_user::mouse_from(SDL_MouseMotionEvent *from)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		drawmode->mouse_from(from);
	}
	SDL_mutexV(draw_mtx);
}

/** Do I allow the mouse to leave my drawing area? */
bool sdl_user::mouse_leave()
{
	bool ret_val;
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		ret_val = drawmode->mouse_leave();
	}
	else
	{
		ret_val = false;
	}
	SDL_mutexV(draw_mtx);
	return ret_val;
}

void sdl_user::mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to)
{	//TODO: handle click and drag movable widgets
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		drawmode->mouse_move(from, to);
	}
	SDL_mutexV(draw_mtx);
}

/** Block until the drawing mode is achieved. Call this from the client thread only. */
void sdl_user::wait_for_mode(enum drawmode wait, bool mutex)
{
	while (draw_mode != wait)
	{
		SDL_Delay(100);
	};
	if (mutex)
	{
		while (SDL_mutexP(mode_mtx) == -1) {};
	}
}

/** Am I in a specific drawmode? */
bool sdl_user::is_in_mode(enum drawmode here, bool mutex)
{
	if (draw_mode == here)
	{
		if (mutex)
		{
			while (SDL_mutexP(mode_mtx) == -1) {};
		}
		return true;
	}
	else
	{
		return false;
	}
}

/** Return the drawmode object */
sdl_drawmode *sdl_user::get_drawmode(bool mutex)
{
	if (mutex)
	{
		while (SDL_mutexP(mode_mtx) == -1) {};
	}
	return drawmode;
}

/** unlocks the mode mutex */
void sdl_user::done_with_drawmode()
{
	SDL_mutexV(mode_mtx);
}

void sdl_user::mouse_click(SDL_MouseButtonEvent *here)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		drawmode->mouse_click(here);
	}
	SDL_mutexV(draw_mtx);
}

/** Handle keypresses that are common to all drawmodes, pass the rest to the drawmode for handling */
void sdl_user::key_press(SDL_KeyboardEvent *button)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (drawmode != 0)
	{
		if (button->type == SDL_KEYDOWN)
		{
			switch(button->keysym.sym)
			{
				default:
					drawmode->key_press(button);
					break;
			}
		}
	}
	SDL_mutexV(draw_mtx);
}

/** spin until we are ready. Called from the client thread after changing drawmode*/
bool sdl_user::are_you_ready()
{
	return ready;
}

/** Pass the quit request on to the drawmode */
bool sdl_user::quit_request()
{
	bool temp = drawmode->quit_request();
	if (temp)
	{
		printf("Telling the client to stop\n");
		delete_requests();
		stop();
	}
	return temp;
}

/** change the current drawmode */
void sdl_user::change_drawmode(enum drawmode chg)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	change_draw = chg;
	if (draw_mode == INVALID)
		check_for_change_drawmode();
	SDL_mutexV(draw_mtx);
}

void sdl_user::check_for_change_drawmode()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	while (SDL_mutexP(mode_mtx) == -1) {};
	if (change_draw != INVALID)
	{
		enum drawmode chg = change_draw;
		ready = false;
		if (drawmode != 0)
		{
			delete_requests();
			delete drawmode;
			drawmode = 0;
		}
		switch(chg)
		{
			case DRAWMODE_LOADING:
				drawmode = new draw_loading(this);
				draw_mode = chg;
				ready = true;
				break;
			case DRAWMODE_LOGIN:
				drawmode = new draw_login(this);
				draw_mode = chg;
				ready = true;
				break;
			case DRAWMODE_CHARSEL:
				drawmode = new draw_char_sel(this);
				draw_mode = chg;
				ready = true;
				break;
			case DRAWMODE_NEWCHAR:
				drawmode = new draw_new_char(this);
				draw_mode = chg;
				ready = true;
				break;
			case DRAWMODE_GAME:
				drawmode = new draw_game(this);
				draw_mode = chg;
				ready = true;
				break;
			default:
				drawmode = 0;
				draw_mode = INVALID;
				break;
		}
	}
	change_draw = INVALID;
	SDL_mutexV(mode_mtx);
	SDL_mutexV(draw_mtx);
}

/** Draw the game if the current drawmode is valid and if we are ready */
void sdl_user::draw(SDL_Surface *display)
{
	check_for_change_drawmode();
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (ready)
	{
		if (drawmode != 0)
		{
			drawmode->draw(display);
		}
	}
	SDL_mutexV(draw_mtx);
}

void sdl_user::register_char(lin_char_info *data)
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
		wait_for_mode(DRAWMODE_CHARSEL, true);
		bob = (draw_char_sel *)get_drawmode(false);
		bob->get_login_chars();
		done_with_drawmode();
	}
}

void sdl_user::init()
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
	
	DesKeyInit("~!@#%^$<");	//TODO : move this code to a class and use an object
	init_packs();
	init_tiles();

	change_drawmode(DRAWMODE_LOADING);

	while (!are_you_ready())
	{
		check_requests();
	}
	draw_loading *load;
	int what_server;
	load = (draw_loading*)get_drawmode(false);

	//wait for the user to pick a server
	do
	{
		what_server = load->get_server_pick();
		check_requests();
	} while (what_server == -1);
	
	server_name = new char[strlen(main_config->get_name(what_server)) + 1];
	strcpy(server_name, main_config->get_name(what_server));

	server = new connection(main_config, what_server);
	proc = new packet(server, this);
	if (get_updates(server, load) > 0)
	{
	}

	//check for custom opcodes
	unsigned char *copcodes;
	copcodes = (unsigned char*)getfiles->load_file("opcodes.txt", 0, FILE_REGULAR3, 0);
	if (copcodes != 0)
	{	//there are custom opcodes for this server
		printf("This server has custom opcodes\n");
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
//	if (server->connection_ok() == 1)
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

int sdl_user::get_updates(connection* server, draw_loading *scrn)
{
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
				printf("%s\n", display);
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
		}
	}
	catch(int e)
	{
		printf("An error occurred %d\n", e);
		status = -1;
	}
	return status;
}

void sdl_user::change_map(int map_num)
{
	if (is_in_mode(DRAWMODE_GAME, true))
	{
		draw_game *dg = (draw_game*)get_drawmode(false);
		dg->change_map(map_num);
		done_with_drawmode();
	}
}

int sdl_user::process_packet()
{
	proc->get_packet(true);
	return proc->process_packet();
}

void sdl_user::send_packet(packet_data &sendme)
{
	proc->send_packet(sendme);
}

int sdl_user::run()
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
	done = true;
	return 0;
}
