#ifndef _PACK_H_
#define _PACK_H_

#include <stdio.h>

//#include "des.h"

#define DATA_EXT ".pak"
#define INDEX_EXT ".idx"
#define TEMP_EXT ".$$$"

struct file_entry
{
	int offset;
	char name[20];
	int size;
};

int getHashIndex(const char *name);

class pack
{
	public:
		pack(const char *name, int encrypted);
		int add_file(const char *file);
		int detect_dupes();	//detects duplicate files
		unsigned char* load_file(int which, int decrypting);
		unsigned char* load_png(const char *name, int *size, int decrypting);
		char *load_file(const char *name, int *size, int decrypting);
		int check_file(const char *name);
		void sort();
		static int compare(const void *a, const void *b);
		void list_files();
		~pack();
	private:
		char* data_file;
		FILE* data_buf;
		char* temp_file;
		FILE* temp_buf;
		char* index_file;
		FILE* index_buf;
		int num_files;
		int encrypted;
		int sorted;	//has the pack been sorted
		file_entry* files;	//the list of files
		char** file_data;	//the contents for every file
//		des crypt;
		
		int load_index();
		int load_data();
		void open_data();
};

#endif
