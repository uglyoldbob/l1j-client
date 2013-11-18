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
#include "resources/briefcase.h"
#include "resources/music.h"
#include "resources/pack.h"
#include "resources/partial_table.h"
#include "resources/table.h"
#include "unsorted.h"

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
	test = 0;
	lineage_font.init("Font/eng.fnt", this);

	DesKeyInit("~!@#%^$<");	//TODO : move this code to a class and use an object
	init_packs();
	graphics->init_tiles();
	
	graphics->change_drawmode(DRAWMODE_ADMIN_MAIN);
	while (!graphics->are_you_ready())
	{
		check_requests();
	}
	
	init_codepage(0);
	init_math_tables();
	printf("STUB Load player config\n");
	printf("STUB Initialize emblem cache\n");
	init_strings();
}

int client::run()
{
	try
	{
		init();
		while (1)
		{
			SDL_Delay(10);
			check_requests();
		}
	}
	catch (const char *error)
	{
		printf("FATAL ERROR: %s\n", error);
		graphics->done = true;
	}
}
