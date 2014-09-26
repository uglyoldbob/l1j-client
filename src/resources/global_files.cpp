#include <stdio.h>
#include <string.h>

#include "global_files.h"
#include "sdl_user.h"

pack *global_files::textpack = 0;
pack *global_files::tilepack = 0;
pack **global_files::spritepack = 0;
int global_files::num_sprite_pack = 0;

void global_files::init()
{
	num_sprite_pack = 17;
	textpack = new pack("Text", 1);
	tilepack = new pack("Tile", 0);
	spritepack = new pack*[num_sprite_pack];
	spritepack[0] = new pack("Sprite", 0);
	for (int i = 0; i < (num_sprite_pack-1); i++)
	{
		char name[10];
		sprintf(name, "Sprite%02d", i);
		spritepack[i+1] = new pack(name, 0);
		//new_surf_pack and spritepack are the same thing
		//new_icon_pack and spritepack are the same thing
		//spriteFile is simply the first element of spritepack
	}
	//TODO: verify all packs were loaded successfully
}

void global_files::killpacks()
{
	delete textpack;
	textpack = 0;
	
	delete tilepack;
	tilepack = 0;
	
	for (int i = 0; i < num_sprite_pack; i++)
	{
		delete spritepack[i];
		spritepack[i] = 0;
	}
	num_sprite_pack = 0;
	delete [] spritepack;
	spritepack = 0;
}

global_files::global_files()
{
}

/** loads a file of the given name, returns the size unless size is a null pointer, and optionally decrypts it */
int global_files::check_file(const char *name, file_locations location)
{
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
				return -1;
			fclose(thefile);
			}
			break;
		case FILE_REGULAR2:
			{
			FILE *thefile = fopen(name, "rb");
			if (thefile == 0)
				return -1;
			fclose(thefile);
			}
			break;
		case FILE_TILEPACK:
			return tilepack->check_file(name);
			break;
		case FILE_SPRITEPACK:
			return spritepack[0]->check_file(name);
			break;
		case FILE_SPRITESPACK:
			{
			int index = getHashIndex(name) + 1;
			return spritepack[index]->check_file(name);
			}
			break;
		case FILE_TEXTPACK:
			return textpack->check_file(name);
			break;
		case FILE_LAUNCHER:
		case FILE_LAUNCHERU:
		case FILE_NOTHING:
		default:
			return -1;
		break;
	}
	return 1;
}

/** loads a file of the given name, returns the size unless size is a null pointer, and optionally decrypts it */
unsigned char* global_files::load_file(const char *name, int *size, file_locations location, int decrypting)
{
	unsigned char *buffer;
	buffer = 0;
	size_t filesize = 0;

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
			if (size != 0)
				*size = filesize;
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
//	if (buffer == 0)
//		printf("File %s was not loaded properly\n", name);
//	else
//		printf("File %s was loaded properly\n", name);
	return buffer;
}

