#include <stdio.h>
#include <string.h>

#include "config.h"

const char* config::get_port()
{
	if (port[0] == 0)
		return 0;
	else
		return DEFAULT_PORT;
}

const char* config::get_addr(int which)
{
	switch (which)
	{
		case 0:
			if (domain_name[0] == 0)
			{
				if (ip_addr[0] == 0)
					return DEFAULT_IP;
				else
					return ip_addr;
			}
			else
			{
				return domain_name;
			}
			break;
		case 1:
			if (domain_name[0] == 0)
			{
				return 0;
			}
			else
			{
				if (ip_addr[0] == 0)
					return DEFAULT_IP;
				else
					return ip_addr;
			}
			break;
		default:
			return 0;
			break;
	}
}

int config::config_ok()
{
	if (num_errors == 0)
		return 1;
	return 0;
}

config::config(const char *cfile)
{
	printf("Loading configuration data\n");
	domain_name = new char[MAX_LINE_LENGTH];
	domain_name[0] = 0;
	ip_addr = new char[MAX_LINE_LENGTH];
	ip_addr[0] = 0;
	port = new char[MAX_LINE_LENGTH];
	port[0] = 0;
	game_port = new char[MAX_LINE_LENGTH];
	game_port[0] = 0;

	num_errors = 0;
	
	//retrieve configuration settings
	char *line_read = new char[MAX_LINE_LENGTH];
	int length = MAX_LINE_LENGTH - 1;
	int line_number = 0;
	FILE *config;
	if ((config = fopen(cfile, "r")) == 0)
	{	//error
		printf("Error opening configuration file %s\n", cfile);
		num_errors = MAGIC_ERROR_NUMBER;	//no configuration file is a lot of errors...
	}
	else
	{
		while (fgets(line_read, length, config) != NULL)
		{	//stop reading when done
			line_number++;
			if (line_read[0] != '#')
			{	//ignore comments
				if (sscanf(line_read, "Port = %[^\t\n\r]", port) == 1)
				{
					printf("Connect using port number %s\n", port);
				}
				else if (sscanf(line_read, "GamePort = %[^\t\n\r]", game_port) == 1)
				{
					printf("Connect using game port number %s\n", game_port);
				}
				else if (sscanf(line_read, "Ip = %[^\t\n\r]", ip_addr) == 1)
				{
					printf("Server IP address is %s\n", ip_addr);
				}
				else if (sscanf(line_read, "DomainName = %[^\t\n\r]", domain_name) == 1)
				{
					printf("Server domain name is %s\n", domain_name);
				}
				else
				{
					printf ("Error reading from line number: %d\n", line_number);
					num_errors++;
				}
			}
		}
		fclose(config);
	}
	delete [] line_read;
	line_read = (char*)0;
	if (num_errors == 1)
	{
		printf("ERROR: 1 error was found in your config file. Fix it and restart!\n");
	}
	else if ((num_errors == MAGIC_ERROR_NUMBER) || (num_errors == 0))
	{	//apply defaults if necessary
		if (num_errors == MAGIC_ERROR_NUMBER)
		{
			printf("ERROR: %s is missing\n", cfile);
		}
		if (port[0] == 0)
		{
			strcpy(port, DEFAULT_PORT);
			printf("WARNING: Using default port %s\n", port);
		}
		if (game_port[0] == 0)
		{
			strcpy(game_port, DEFAULT_GAME_PORT);
			printf("WARNING: Using default game port %s\n", game_port);
		}
		if ((ip_addr[0] == 0) && (domain_name[0] == 0))
		{
			strcpy(ip_addr, DEFAULT_IP);
			printf("WARNING: Using default ip address %s\n", ip_addr);
		}
	}
	else if (num_errors > 0)
	{
		printf("ERROR: %i errors were found in your config file. Fix them and restart!\n", num_errors);
	}

}

config::~config()
{
	delete [] domain_name;
	delete [] ip_addr;
	delete [] port;
	delete [] game_port;
}
