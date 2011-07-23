#include "pack.h"

#include <stdio.h>
#include <string.h>

//in encrypt.cpp
void decrypt(unsigned char* buffer, unsigned char *buf2, unsigned int length);
void encrypt(unsigned char* buffer, unsigned char *buf2, unsigned int length);

int pack::load_index(int encrypted)
{
	index_buf = fopen(index_file, "rb");
	if (index_buf == 0)
	{
		printf("Unable to open file %s\n", index_file);
		return 1;
	}
	int size1;
	fseek(index_buf, 0, SEEK_END);
	int size2 = (ftell(index_buf) - 4) / sizeof(file_entry);
	fseek(index_buf, 0, SEEK_SET);

	fread(&size1, 4, 1, index_buf);
	if (size1 != size2)
	{
		printf("Size mismatch (%d) (%d)\n", size1, size2);
		return 1;
	}
	num_files = size1;
	printf("Loading list of %d files\n", num_files);
	files = new file_entry[num_files];

	//read the files all at once
	fread(files, sizeof(file_entry), num_files, index_buf);

	if (encrypted == 1)
	{
		printf("Attempting to decrypt\n");
		decrypt((unsigned char*)files, (unsigned char*)files, num_files * sizeof(file_entry));
//		crypt.decrypt_block((unsigned char*)files, num_files * sizeof(file_entry));
		for (int i = 0; i < 20; i++)
		{
			if (files[i].name[19] != 0)
			{
				for (int j = 0; j < 20; j++)
				{
					printf("%02x:", files[i].name[j] & 0xFF);
				}
				printf("\n\t");
				printf(" length %08x @ %08x\n", 
					files[i].size, files[i].offset);
			}
			else
			{
				printf("%s length %08x @ %08x\n", 
					files[i].name, files[i].size, files[i].offset);
			}
		}
	}
	if (files[0].offset != 0)
	{	//fix the first offset
		printf("Fixing offset of the first file\n");
		files[0].offset = 0;
	}
	return 0;
}

int pack::detect_dupes()	//detects duplicate files
{
	printf("Checking for duplicate files\n");
	int dupes = 0;
	for (int i = 0; (i+1) < num_files; i++)
	{
		if (strncmp(files[i].name, files[i+1].name, 20) == 0)
		{	//they match, check file contents
			if (files[i].size == files[i+1].size)
			{	//the sizes also match
				unsigned char *filea;
				unsigned char *fileb;
				if (data_buf == 0)
				{
					data_buf = fopen(data_file, "rb");
				}
				
				filea = new unsigned char[files[i].size];
				fseek(data_buf, files[i].offset, SEEK_SET);
				fread(filea, 1, files[i].size, data_buf);
				
				fileb = new unsigned char[files[i].size];
				fseek(data_buf, files[i+1].offset, SEEK_SET);
				fread(fileb, 1, files[i].size, data_buf);
				
				if (memcmp(filea, fileb, files[i].size) == 0)
				{
					dupes++;
					files[i].size = -1;
					files[i].offset = -1;
				}
				delete [] filea;
				delete [] fileb;
			}			
		}
	}
	if (dupes > 0)
	{
		printf("\tThere were %d duplicate files detected\n", dupes);
	}
	return dupes;
}

int pack::load_data(int encrypted)
{
	if (index_buf == 0)
	{
		printf("The pack index has not been loaded\n");
		return -1;
	}
	if (data_buf != 0)
	{
		printf("Pack data already loaded\n");
		return -1;
	}
	if (encrypted == 1)
	{
		printf("Data will not be decrypted\n");
	}
	data_buf = fopen(data_file, "rb");
	file_data = new char*[num_files];
	for (int i = 0; i < num_files; i++)
	{
		//check for actual file size
		fseek(data_buf, files[i].offset, SEEK_SET);
		file_data[i] = new char[files[i].size];
		fread(file_data[i], 1, files[i].size, data_buf);
	}
	return 0;
}

pack::pack(const char *name, int encrypted)
{
	int size;
	size = strlen(FOLDER_PREFIX) + strlen(name) + strlen(DATA_EXT) + 1;
	data_file = new char[size];
	sprintf(data_file, "%s%s%s", FOLDER_PREFIX, name, DATA_EXT);
	
	size = strlen(FOLDER_PREFIX) + strlen(name) + strlen(TEMP_EXT) + 1;
	temp_file = new char[size];
	sprintf(temp_file, "%s%s%s", FOLDER_PREFIX, name, TEMP_EXT);
	
	size = strlen(FOLDER_PREFIX) + strlen(name) + strlen(INDEX_EXT) + 1;
	index_file = new char[size];
	sprintf(index_file, "%s%s%s", FOLDER_PREFIX, name, INDEX_EXT);
	
	num_files = 0;
	
	printf("Loading %s%s\n", FOLDER_PREFIX, name);
	
	data_buf = 0;
	index_buf = 0;
	temp_buf = 0;
	file_data = 0;
	
	unsigned char key[] = {'~', '!', '@', '#', '%', '^', '$', '<'};
//	crypt.initialize(key);
	load_index(encrypted);
	load_data(encrypted);
}

pack::~pack()
{
	for (int i = 0; i < num_files; i++)
	{
		delete [] file_data[i];
	}
	delete [] file_data;
	
	delete [] data_file;
	delete [] temp_file;
	delete [] index_file;
}
