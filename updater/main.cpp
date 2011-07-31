#include <stdio.h>

#include "client.h"

int main (int argc, char **argv)
{
	client *game;
	game = new client();
	try
	{
		game->init();
	}
	catch (const char *error)
	{
		printf("%s", error);
	}
	
	delete game;
	return 0;
}
