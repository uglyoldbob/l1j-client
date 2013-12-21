#ifndef __FILES_H_
#define __FILES_H_

class sdl_user;

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

/** This class handles whether a file should be loaded from the global game resources or the server specific files */
class files
{	//the files loaded depend on which client object wants files because of server specific files
	public:
		files(sdl_user *who);
		~files();
		
		unsigned char* load_file(const char *name, int *size, file_locations location, int decrypting);
	private:
		sdl_user *theclient;
};

#endif