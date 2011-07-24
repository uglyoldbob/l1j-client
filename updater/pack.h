#ifndef _PACK_H_
#define _PACK_H_

#include <stdio.h>

//#include "des.h"

#define FOLDER_PREFIX "./"
#define DATA_EXT ".pak"
#define INDEX_EXT ".idx"
#define TEMP_EXT ".$$$"

struct file_entry
{
	long offset;
	char name[20];
	int size;
};

class pack
{
	public:
		pack(const char *name, int encrypted);
		int add_file(const char *file);
		int sort();
		
		int detect_dupes();	//detects duplicate files
		~pack();
	private:
		char* data_file;
		FILE* data_buf;
		char* temp_file;
		FILE* temp_buf;
		char* index_file;
		FILE* index_buf;
		int num_files;
		file_entry* files;	//the list of files
		char** file_data;	//the contents for every file
//		des crypt;
		
		int load_index(int encrypted);
		int load_data(int encrypted);
		void open_data();
};

#endif
