#ifndef __TABLE_H_
#define __TABLE_H_

#include "pack.h"

class table
{
	public:
		table();
		
		const char *operator[](int);
						
		void load_local(const char *name, pack* from);
		void load(const char *real_name, pack* from);
		void sort();
		static int compare(const void *a, const void *b);
		void print();
	private:
		int num_entries;
		char **entries;
		char *table_name;
		char get_lang_char();
};

#endif