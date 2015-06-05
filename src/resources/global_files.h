#ifndef __GLOBAL_FILES_H_
#define __GLOBAL_FILES_H_

#include "pack.h"

enum file_locations
{
	FILE_REGULAR1,		//regular file in the lineage folder
	FILE_REGULAR2,		//regular file in the custom client folder
	FILE_REGULAR3,		//file that only is in the server specific data
	FILE_TILEPACK,		//tile.pak
	FILE_SPRITEPACK,	//sprite.pak
	FILE_SPRITESPACK,	//sprite00.pak - sprite15.pak
	FILE_TEXTPACK,		//text.pak
	FILE_LAUNCHER,		//launcher.pak
	FILE_LAUNCHERU,		//launcher-u.pak
	FILE_NOTHING
};

//this class is used to load files that are not server specific in any way
class global_files
{
	public:
		global_files();
		unsigned char* load_file(const char *name, int *size, file_locations location, int decrypting);
		int check_file(const char *name, file_locations location);
		void list_files(file_locations location);
		static void init();
		static void killpacks();
	private:
		static pack *textpack;
		static pack *tilepack;
		static pack **spritepack;
		static int num_sprite_pack;
};

#endif
