#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "partial_table.h"

void partial_table::load(const char *real_name, files *place, file_locations location)
{
	char *buffer;
	int size;
	buffer = (char*)place->load_file(real_name, &size, location, 1);

	int i = 0;
	char* temp_entry;
	char delimiters[] = {0x0d, 0x0a, 0};
	char delimiters2[] = {0x0d, 0x0a, ' ', 0};
	temp_entry = strtok(buffer, delimiters);
	num_entries = atoi(temp_entry);
//	printf("There are %d possible entries in %s\n", num_entries, real_name);
	entries = new char*[num_entries];
	temp_entry = strtok(NULL, delimiters2);
	
	for (int i = 0; i < num_entries; i++)
	{
		entries[i] = (char*)0;
	}
	while (temp_entry != NULL)
	{
		i = atoi(temp_entry);
		temp_entry = strtok(NULL, delimiters);

		if (i < num_entries)
		{
			if (entries[i] == 0)
			{
				entries[i] = new char[strlen(temp_entry) + 1];
				strcpy(entries[i], temp_entry);
			}
			else
			{	/** \todo should the first item or the second item actually be used for the entry? */
				printf("Entry %d already exists\n\told: %s\n\tdupe: %s\n", i, entries[i], temp_entry);
			}
		}
		temp_entry = strtok(NULL, delimiters2);
	}
	delete [] buffer;
	buffer = 0;
}

partial_table::partial_table()
{
}

partial_table::~partial_table()
{
	printf("Deleting partial table\n");
}