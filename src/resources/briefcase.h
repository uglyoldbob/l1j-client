#ifndef _BRIEFCASE_H_
#define _BRIEFCASE_H_

#include <stdio.h>

//#include "des.h"

#define BCE_EXT ".bce"

struct briefcase_entry
{
	int namelen;
	char *name;
	int size;
	int offset;
	unsigned char *data;	//a buffer to hold the data for this entry
};

class briefcase
{
	public:
		briefcase(const char *name);
		int add_file(const char *file);
		int save();	//save the briefcase to disk
		int sort();
		int detect_dupes();	//detects duplicate files
		unsigned char* load_file(int which);
		char *load_file(const char *name, int *size);
		int check_file(const char *name);
		~briefcase();
	private:
		char* data_file;
		FILE* data_buf;

		int num_files;
		briefcase_entry* files;	//the list of files
		char** file_data;	//the contents for every file
		
		int load();
		void open_data();
};

#endif
