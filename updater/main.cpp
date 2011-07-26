#include <stdio.h>

#include "config.h"
#include "connection.h"
#include "pack.h"

#define TRANSFER_AMOUNT 0x400

unsigned long checksum = 0xdeadbeef;

int pack_resources()
{
	return 0;
}

int get_updates(connection* server)
{
	unsigned long temp;
	long sign_temp;
	int status;	//> 0 means update occurred
	status = 0;
	try
	{
		if (server->connection_ok() == 1)
		{
			server->snd_var(&checksum, 4);
			server->rcv_var(&sign_temp, 4);
			if (sign_temp < 0)
			{
				temp = -sign_temp;
				printf("Protocol : %lx, ServerId : %lx\n", temp & 0xFFFF, temp>>16);
				server->rcv_var(&sign_temp, 4);
			}
			if (sign_temp > 0)
			{	//receive files
				int num_files = sign_temp;
				printf("Receiving %d files\n", num_files);
				status = num_files;
				unsigned char name_length;
				unsigned long file_length;
				unsigned char* file_buffer;
				char *filename;
				for (int i = 0; i < num_files; i++)
				{
					server->rcv(&name_length, 1);
					printf(" Length of filename %d, ", name_length);
					filename = new char[name_length];
					server->rcv(filename, name_length);
					filename[name_length] = 0;
					server->rcv_var(&file_length, 4);
					file_buffer = new unsigned char[file_length];
					printf("Downloading %s, %ld bytes ...", filename, file_length);
					//do stuff so the file can be saved
					char *dump_name;
					dump_name = new char[name_length + 6];
					sprintf(dump_name, "%s.gz", filename);
//					FILE *filedump = fopen(dump_name, "wb");
					while (file_length > 0)
					{
						if (file_length > TRANSFER_AMOUNT)
						{
							server->rcv(file_buffer, TRANSFER_AMOUNT);
//							fwrite(file_buffer, 1, TRANSFER_AMOUNT, filedump);
							file_length -= TRANSFER_AMOUNT;
						}
						else
						{
							server->rcv(file_buffer, file_length);
//							fwrite(file_buffer, 1, file_length, filedump);
							file_length = 0;
						}
					}
//					fclose(filedump);
					printf(" done\n");
					//todo: do something with this number (the new checksum)
					server->rcv_var(&sign_temp, 4);
					delete [] dump_name;
					delete [] filename;
					delete [] file_buffer;
				}
			}
			long num_servers;
			unsigned short* num_users;
			server->rcv_var(&num_servers, 4);
			printf("There are %ld servers: ", num_servers);
			num_users = new unsigned short[num_servers];
			for (int j = 0; j < num_servers; j++)
				server->rcv_var(&num_users[j], 2);
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
		status = -1;
	}
	return status;
}

int main (int argc, char **argv)
{
	config *main_config;
	connection *server;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		printf("WSAStartup failed!\n");
//		perror("WSAStartup failed!\n");
		return 1;
	}
	
	main_config = new config("Lineage.ini");
	if (!main_config->config_ok())
	{
		printf("ERROR Loading configuration file.\n");
		delete main_config;
		WSACleanup();
		return 1;
	}
	else
	{
		printf("Configuration loaded successfully.\n");
	}

#if 0
	pack *testme;
	testme = new pack("Text", 1);
	testme->detect_dupes();
	int file = 11;
	printf("\tFile number %d:\n\t%s\n", file, testme->load_file(11));
	delete testme;
	testme = new pack("Tile", 0);
	testme->detect_dupes();
	delete testme;
	WSACleanup();
	return 0;
#endif
	
	server = new connection(main_config);
	if (get_updates(server) > 0)
	{
		printf("Packing resources\n");
	}
	
	//begin game portion of client
	if (server->change() == 1)
	{
		printf("Connected to game server\n");
	}
	else
	{
		printf("Failed to connect to game server\n");
	}

	printf("Exiting client now\n");
	delete server;	
	delete main_config;
	WSACleanup();
	return 0;
}
