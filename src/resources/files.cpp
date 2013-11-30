#include <stdio.h>

#include "client.h"
#include "files.h"

files::files(client *who)
{
	theclient = who;
}

files::~files()
{
}

/** loads a file of the given name, returns the size unless size is a null pointer, and optionally decrypts it */
unsigned char* files::load_file(const char *name, int *size, file_locations location, int decrypting)
{
	int try_custom = 1;
	unsigned char *buffer;
	buffer = 0;
	size_t filesize = 0;
	if (theclient == 0)
	{
		try_custom = 0;
	}
	else if (theclient->server_data == 0)
	{
		try_custom = 0;
	}
	if (try_custom == 1)
	{	//this means there is a valid briefcase in the client object
		buffer = (unsigned char*)theclient->server_data->load_file(name, size);
		if (buffer == 0)
		{	//this file does not exist as a server specific resource
			try_custom = 0;
		}
	}
	
	if (try_custom == 0)
	{	//just load the file, don't bother with the briefcase
		switch (location)
		{
			case FILE_REGULAR1:
				{
				char *realname = new char[strlen(name) + strlen(lineage_dir) + 1];
				strcpy(realname, lineage_dir);
				strcat(realname, name);
				FILE *thefile = fopen(realname, "rb");
				delete [] realname;
				realname = 0;
				if (thefile == 0)
					return 0;
				unsigned int result;
				fseek(thefile, 0, SEEK_END);
				filesize = ftell(thefile);
				fseek(thefile, 0, SEEK_SET);
				if (filesize <= 0)
					return 0;
				if (size != 0)
					*size = filesize;
				buffer = new unsigned char[filesize];
				result = fread(buffer, 1, filesize, thefile);
				if (result != filesize)
				{
					delete [] buffer;
					buffer = 0;
				}
				fclose(thefile);
				}
				break;
			case FILE_REGULAR2:
				{
				FILE *thefile = fopen(name, "rb");
				unsigned int result;
				filesize = 0;
				if (thefile != 0)
				{
					fseek(thefile, 0, SEEK_END);
					filesize = ftell(thefile);
					fseek(thefile, 0, SEEK_SET);
				}
				if (filesize == 0)
					return 0;
				buffer = new unsigned char[filesize];
				result = fread(buffer, 1, filesize, thefile);
				if (result != filesize)
				{
					delete [] buffer;
					buffer = 0;
				}
				fclose(thefile);
				}
				break;
			case FILE_TILEPACK:
				buffer = (unsigned char*)tilepack->load_file(name, size, decrypting);
				break;
			case FILE_SPRITEPACK:
				buffer = (unsigned char*)spritepack[0]->load_file(name, size, decrypting);
				break;
			case FILE_SPRITESPACK:
				{
				int index = getHashIndex(name) + 1;
				buffer = (unsigned char*)spritepack[index]->load_file(name, size, decrypting);
				}
				break;
			case FILE_TEXTPACK:
				buffer = (unsigned char*)textpack->load_file(name, size, decrypting);
				break;
			case FILE_LAUNCHER:
			case FILE_LAUNCHERU:
			case FILE_NOTHING:
			default:
				buffer = 0;
			break;
		}
	}
//	if (buffer == 0)
//		printf("File %s was not loaded properly\n", name);
//	else
//		printf("File %s was loaded properly\n", name);
	
	return buffer;
}
