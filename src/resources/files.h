#ifndef __FILES_H_
#define __FILES_H_

class client;

enum file_locations
{
	FILE_REGULAR1,		//regular file in the lineage folder
	FILE_REGULAR2,		//regular file in the custom client folder
	FILE_TILEPACK,		//tile.pak
	FILE_SPRITEPACK,	//sprite.pak
	FILE_SPRITESPACK,	//sprite00.pak - sprite15.pak
	FILE_TEXTPACK,		//text.pak
	FILE_LAUNCHER,		//launcher.pak
	FILE_LAUNCHERU,		//launcher-u.pak
	FILE_NOTHING
};

class files
{	//the files loaded depend on which client object wants files because of server specific files
	public:
		files(client *who);
		~files();
		
		unsigned char* load_file(const char *name, int *size, file_locations location, int decrypting);
	private:
		client *theclient;
};

#endif