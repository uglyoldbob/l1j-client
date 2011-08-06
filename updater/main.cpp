#include <stdio.h>

#include "sdl_master.h"

int main(int argc, char* argv[])
{
	sdl_master graphics;
	graphics.create_client();
	graphics.process_events();
	
	return 0;
}
