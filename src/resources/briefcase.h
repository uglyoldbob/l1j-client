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
	long offset;
	unsigned char *data;	//a buffer to hold the data for this entry
};

class briefcase
{
	public:
		briefcase(const char *name);
		int add_file(const char *file);
		int save();	//save the briefcase to disk
		int detect_dupes();	//detects duplicate files
		unsigned char* load_file(int which);
		char *load_file(const char *name, int *size);
		int check_file(const char *name);
		void sort();
		static int compare(const void *a, const void *b);
		const char *get_hash();
		void new_data();
		void add_data();
		void write_file(char *name, char *data, int size);
		void finish_briefcase();
		
		~briefcase();
	private:
		char* data_file;
		FILE* data_buf;

		char hash[65];
		int sorted;	//is the list of files properly sorted?
		int num_files;
		briefcase_entry* files;	//the list of files
		char** file_data;	//the contents for every file
		
		int load();
		void open_data();
};

#endif
