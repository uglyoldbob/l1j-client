#include <stdio.h>
#include <string.h>

#include "config.h"
#include "globals.h"

const char* config::get_port()
{
	if (port[0] == 0)
		return DEFAULT_PORT;
	else
		return port;
}

const char* config::get_game_port()
{
	if (game_port[0] == 0)
		return DEFAULT_GAME_PORT;
	else
		return game_port;
}


const char* config::get_addr(int which)
{
	if (which < num_names)
	{
		return names[which];
	}
	else
	{
		return DEFAULT_IP;
	}
}

int config::get_num_names()
{
	return num_names;
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
	port = new char[MAX_LINE_LENGTH];
	port[0] = 0;
	game_port = new char[MAX_LINE_LENGTH];
	game_port[0] = 0;
	char *all_names;
	all_names = new char[MAX_LINE_LENGTH];

	num_errors = 0;
	lineage_dir = (char*)0;	
	
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
				else if (sscanf(line_read, "Path = %[^\t\n\r]", all_names) == 1)
				{
					lineage_dir = new char[strlen(line_read)];
					strcpy(lineage_dir, all_names);
					printf("Lineage is located at: %s\n", lineage_dir);
				}
				else if (sscanf(line_read, "Names = %[^\t\n\r]", all_names) == 1)
				{
					printf("Server IP address is ");
					num_names = 1;
					for (unsigned int i = 0; i < strlen(all_names); i++)
					{
						if (all_names[i] == ',')
							num_names++;
					}
					names = new char*[num_names];
					char *temp_name;
					temp_name = strtok(all_names, ",");
					names[0] = new char[strlen(temp_name)+1];
					strcpy(names[0], temp_name);
					for (int j = 1; j < num_names; j++)
					{
						temp_name = strtok(NULL, ",");
						names[j] = new char[strlen(temp_name)+1];
						strcpy(names[j], temp_name);
					}
					printf(" (%d)", num_names);
					for (int k = 0; k < num_names; k++)
					{
						printf("\n\t%s", names[k]);
					}
					printf("\n");
				}
				else
				{
					printf ("Error reading from line number: %d\n\t%s\n", line_number, line_read);
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
		if (lineage_dir == 0)
		{
			printf("ERROR: Lineage path is missing\n");
			num_errors++;
		}
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
		if (num_names == 0)
		{
			printf("ERROR: No names defined\n");
			num_errors++;
		}
	}
	else if (num_errors > 0)
	{
		printf("ERROR: %i errors were found in your config file. Fix them and restart!\n", num_errors);
	}

}

config::~config()
{
	for (int i = 0; i < num_names; i++)
	{
		delete [] names[i];
	}
//	delete [] names;
	delete [] port;
	delete [] game_port;
}
