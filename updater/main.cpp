#include <stdio.h>

#include "config.h"
#include "connection.h"
#include "pack.h"

#define TRANSFER_AMOUNT 0x400

int main (int argc, char **argv)
{
	config *main_config;
	connection *server;
	unsigned long checksum = 0xdeadbeef;
	unsigned long temp;
	long sign_temp;
	
	main_config = new config("Lineage.ini");
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
	
	try
	{
		if (server->connection_ok() == 1)
		{
			server->snd(&checksum, 4);
			server->rcv(&sign_temp, 4);
			if (sign_temp < 0)
			{
				temp = -sign_temp;
				printf("Protocol : %x, ServerId : %x\n", temp & 0xFFFF, temp>>16);
				server->rcv(&sign_temp, 4);
			}
			if (sign_temp > 0)
			{	//receive files
				int num_files = sign_temp;
				printf("Receiving %d files\n", num_files);
				unsigned char name_length;
				unsigned long file_length;
				unsigned char* file_buffer;
				char *filename;
				char *actual_name;
				for (int i = 0; i < num_files; i++)
				{
					server->rcv(&name_length, 1);
					printf(" Length of filename %d, ", name_length);
					filename = new char[name_length];
					server->rcv(filename, name_length);
					filename[name_length] = 0;
					server->rcv(&file_length, 4);
					file_buffer = new unsigned char[file_length];
					printf("Downloading %s, %d bytes ...", filename, file_length);
					//do stuff so the file can be saved
					char *dump_name;
					dump_name = new char[name_length + 6];
					sprintf(dump_name, "%s.gz", filename);
					FILE *filedump = fopen(dump_name, "wb");
					while (file_length > 0)
					{
						if (file_length > TRANSFER_AMOUNT)
						{
							server->rcv(file_buffer, TRANSFER_AMOUNT);
							fwrite(file_buffer, 1, TRANSFER_AMOUNT, filedump);
							file_length -= TRANSFER_AMOUNT;
						}
						else
						{
							server->rcv(file_buffer, file_length);
							fwrite(file_buffer, 1, file_length, filedump);
							file_length = 0;
						}
					}
					fclose(filedump);
					printf(" done\n");
					//todo: do something with this number (the new checksum)
					server->rcv(&sign_temp, 4);
					delete [] dump_name;
					delete [] filename;
					delete [] file_buffer;
				}
			}
			unsigned long num_servers;
			unsigned short* num_users;
			server->rcv(&num_servers, 4);
			printf("There are %d servers: ", num_servers);
			num_servers = ntohl(num_servers);
			num_users = new unsigned short[num_servers];
			server->rcv(num_users, sizeof(num_users));
			for (int i = 0; i < num_servers; i++)
			{
				printf("%d, ", num_users[i]);
			}
			printf("\n");
		}
	}
	catch(int e)
	{
		printf("An error occurred %d\n", e);
	}
	
	delete server;
	delete main_config;
	return 0;
}