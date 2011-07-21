#include <stdio.h>

#include "config.h"
#include "connection.h"

int main (int argc, char **argv)
{
	config *main_config;
	connection *server;
	
	main_config = new config("Lin.ini");
	if (!main_config->config_ok())
	{
		delete main_config;
		return 1;
	}
	else
	{
		printf("Configuration loaded successfully.\n");
	}
	
	server = new connection(main_config);
	
	
	delete server;
	delete main_config;
	return 0;
}