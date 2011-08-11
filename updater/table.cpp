#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "table.h"

table::table()
{
	num_entries = 0;
	entries = 0;
}

table::~table()
{
}

//constant because tables will not be modified by the [] operator
const char *table::operator[](int i)
{
	if (i < num_entries)
	{
		return entries[i];
	}
	else
	{
		return (char*)0;
	}
}

int table::compare(const void *a, const void *b)
{
	const char **aa = (const char**)a;
	const char **ba = (const char**)b;
	return strcmp(*aa, *ba);
}

void table::sort()
{
	//acp[4] is a function pointer to the compare function
	qsort(entries, num_entries, sizeof(char*), table::compare);
}

void table::print()
{
	printf("There are %d entries to print in %s\n", num_entries, table_name);
	for (int i = 0; i < num_entries; i++)
	{
		if (entries[i] != 0)
		{
			printf("Entry %d:\n\t%s \n", i, entries[i]);
			char moron;
			scanf("%c", &moron);
			if (moron == 'q')
				break;
		}
	}
	printf("\n");
}

void table::load(const char *real_name, pack *from)
{
	char *buffer;
	table_name = new char[strlen(real_name) + 1];
	strcpy(table_name, real_name);
	int size;
	buffer = from->load_file(real_name, &size, 1);

	char* temp_entry;
	char delimiters[] = {0x0d, 0x0a, 0};
	temp_entry = strtok(buffer, delimiters);
	num_entries = atoi(temp_entry);
	entries = new char*[num_entries];
	
	for (int i = 0; i < num_entries; i++)
	{
		entries[i] = (char*)0;
	}
	int i = 0;
	
	temp_entry = strtok(NULL, delimiters);
	while (temp_entry != NULL)
	{
		if (i < num_entries)
		{
			entries[i] = new char[strlen(temp_entry) + 1];
			strcpy(entries[i], temp_entry);
			i++;
		}
		temp_entry = strtok(NULL, delimiters);
	}
	if (i < num_entries)
	{
//		printf("WARNING: %s only had (%d of %d) entries\n", real_name, i-1, num_entries);
		while (i < num_entries)
		{
			entries[i++] = (char*)0;
		}
	}

	delete [] buffer;
}

void table::load_local(const char *name, pack* from)
{
	char *real_name;
	real_name = new char[strlen(name) + 7];
	sprintf(real_name, "%s-%c.tbl", name, get_lang_char());
	load(real_name, from);
	delete [] real_name;
}

char table::get_lang_char()
{
	printf("STUB Get Language Character\n");
	return 'e';
}