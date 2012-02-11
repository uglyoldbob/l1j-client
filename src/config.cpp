#include <stdio.h>
#include <string.h>

#include "config.h"
#include "globals.h"

const char* config::get_port(int srv)
{
	if (srv < num_servers)
	{
		if (sdata[srv].port[0] != 0)
			return sdata[srv].port;
	}
	return DEFAULT_PORT;
}

const char* config::get_game_port(int srv)
{
	if (srv < num_servers)
	{
		if (sdata[srv].game_port[0] != 0)
			return sdata[srv].game_port;
	}
	return DEFAULT_GAME_PORT;
}


const char* config::get_addr(int srv, int which)
{
	if (srv < num_servers)
	{
		if (which < sdata[srv].num_names)
		{
			return sdata[srv].names[which];
		}
	}
	return DEFAULT_IP;
}

const char* config::get_name(int srv)
{
	if (srv < num_servers)
	{
		return sdata[srv].server_name;
	}
	return "INTERNAL ERROR";
}

int config::get_num_names(int srv)
{
	if (srv < num_servers)
	{
		return sdata[srv].num_names;
	}
	else
	{
		return 0;
	}
}

int config::get_num_servers()
{
	return num_servers;
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
	char *all_names;
	all_names = new char[MAX_LINE_LENGTH];

	num_errors = 0;
	lineage_dir = (char*)0;	
	
	//retrieve configuration settings
	char *line_read = new char[MAX_LINE_LENGTH];
	int length = MAX_LINE_LENGTH - 1;
	int line_number = 0;
	int read_mode = 0;	//defines the mode of reading for the current line
		//0 means Path or NumServers
		//1 or higher means server specific data
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
				if (read_mode == 0)
				{
					 if (sscanf(line_read, "Path = %[^\t\n\r]", all_names) == 1)
					 {
						lineage_dir = new char[strlen(line_read)];
						strcpy(lineage_dir, all_names);
						printf("Lineage is located at: %s\n", lineage_dir);
					}
					else if (sscanf(line_read, "NumServers = %d ", &num_servers) == 1)
					{
						printf("There are %d servers\n", num_servers);
						
						if (num_servers != 0)
						{
							sdata = new server_data[num_servers];
							for (int i = 0; i < num_servers; i++)
							{
								sdata[i].port = new char[MAX_LINE_LENGTH];
								sdata[i].port[0] = 0;
								sdata[i].game_port = new char[MAX_LINE_LENGTH];
								sdata[i].game_port[0] = 0;
							}
						}
						else
						{
							sdata = 0;
						}
						read_mode++;
					}
					else
					{
						printf ("Error reading from line number: %d\n\t%s\n", line_number, line_read);
						num_errors++;
					}
				}
				else if (read_mode <= num_servers)
				{
					if (sscanf(line_read, "%[^\t\n\r]", all_names) == 1)
					{
						sdata[read_mode - 1].server_name = new char[strlen(all_names)+1];
						strcpy(sdata[read_mode-1].server_name, all_names);
						printf("Reading data for server named %s\n", sdata[read_mode-1].server_name);
					}
					else if (sscanf(line_read, "\tPort = %[^\t\n\r]", sdata[read_mode-1].port) == 1)
					{
						printf("Connect using port number %s\n", sdata[read_mode-1].port);
					}
					else if (sscanf(line_read, "\tGamePort = %[^\t\n\r]", sdata[read_mode-1].game_port) == 1)
					{
						printf("Connect using game port number %s\n", sdata[read_mode-1].game_port);
					}
					else if (sscanf(line_read, "\tNames = %[^\t\n\r]", all_names) == 1)
					{
						printf("Server IP address is ");
						sdata[read_mode-1].num_names = 1;
						for (unsigned int i = 0; i < strlen(all_names); i++)
						{
							if (all_names[i] == ',')
								sdata[read_mode-1].num_names++;
						}
						sdata[read_mode-1].names = new char*[sdata[read_mode-1].num_names];
						char *temp_name;
						temp_name = strtok(all_names, ",");
						sdata[read_mode-1].names[0] = new char[strlen(temp_name)+1];
						strcpy(sdata[read_mode-1].names[0], temp_name);
						for (int j = 1; j < sdata[read_mode-1].num_names; j++)
						{
							temp_name = strtok(NULL, ",");
							sdata[read_mode-1].names[j] = new char[strlen(temp_name)+1];
							strcpy(sdata[read_mode-1].names[j], temp_name);
						}
						printf(" (%d)", sdata[read_mode-1].num_names);
						for (int k = 0; k < sdata[read_mode-1].num_names; k++)
						{
							printf("\n\t%s", sdata[read_mode-1].names[k]);
						}
						printf("\n");
						read_mode++;
					}
					else
					{
						printf ("Error reading from line number: %d\n\t%s\n", line_number, line_read);
						num_errors++;
					}
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
		if (num_servers <= 0)
		{
			printf("ERROR: Invalid number of servers specified (%d)\n", num_servers);
			num_errors++;
		}
		for (int i = 0; i < num_servers; i++)
		{
			if (sdata[i].port[0] == 0)
			{
				strcpy(sdata[i].port, DEFAULT_PORT);
				printf("WARNING: Using default port %s\n", sdata[i].port);
			}
			if (sdata[i].game_port[0] == 0)
			{
				strcpy(sdata[i].game_port, DEFAULT_GAME_PORT);
				printf("WARNING: Using default game port %s\n", sdata[i].game_port);
			}
			if (sdata[i].num_names == 0)
			{
				printf("ERROR: No names defined\n");
				num_errors++;
			}
		}
	}
	else if (num_errors > 0)
	{
		printf("ERROR: %i errors were found in your config file. Fix them and restart!\n", num_errors);
	}

	delete [] all_names;
	delete [] line_read;
}

config::~config()
{
	if (num_servers > 0)
	{
		for (int j = 0; j < num_servers; j++)
		{
			for (int i = 0; i < sdata[j].num_names; i++)
			{
				delete [] sdata[j].names[i];
			}
			delete [] sdata[j].names;
			delete [] sdata[j].server_name;
			delete [] sdata[j].port;
			delete [] sdata[j].game_port;
		}
		delete [] sdata;
	}
}
