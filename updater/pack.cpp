#include "pack.h"

#include <stdio.h>
#include <string.h>

//in encrypt.cpp
void decrypt(unsigned char* buffer, unsigned char *buf2, unsigned int length);
void encrypt(unsigned char* buffer, unsigned char *buf2, unsigned int length);

int pack::load_index()
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
	}
	if (files[0].offset != 0)
	{	//fix the first offset
		printf("Fixing offset of the first file %02lx%02lx%02lx%02lx\n", 
			(files[0].offset>>16)&0xFF,
			files[0].offset>>24, 
			(files[0].offset>>8)&0xFF,
			files[0].offset&0xFF);
		files[0].offset = 0;
	}
	
	for (int i = 0; i < 20; i++)
	{
		if (files[i].name[19] != 0)
		{
			for (int j = 0; j < 20; j++)
			{
				printf("%02x:", files[i].name[j] & 0xFF);
			}
			printf("\n\t");
			printf(" length %08x @ %08lx\n", 
				files[i].size, files[i].offset);
		}
		else
		{
			printf("%s length %08x @ %08lx\n", 
				files[i].name, files[i].size, files[i].offset);
		}
	}

	open_data();	//open the data file it is not already
	int num_invalid = 0;
	fseek(data_buf, 0, SEEK_END);
	int size_data = ftell(data_buf);
	fseek(data_buf, 0, SEEK_SET);
	
	for (int i = 0; i < num_files; i++)
	{
		if (files[i].offset >= size_data)
		{
			num_invalid++;
		}
		else if ((files[i].offset + files[i].size) > size_data)
		{
			num_invalid++;
		}
		//check offset is valid
		//size+offset not too large
	}
	printf("There were %d invalid file entries detected\n", num_invalid);

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
				
				if (!( (files[i].size < 0) || 
					 (files[i+1].size < 0) ||
					 (files[i+1].offset < 0) ||
					 (files[i+1].offset < 0)))
				{

					filea = load_file(i);
					fileb = load_file(i+1);
				
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
	}
	if (dupes >= 0)
	{
		printf("\tThere were %d duplicate files detected\n", dupes);
	}
	return dupes;
}

int pack::load_data()
{
	return 0;
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
	open_data();
	file_data = new char*[num_files];
	for (int i = 0; i < num_files; i++)
	{
		//check for actual file size
		if (fseek(data_buf, files[i].offset, SEEK_SET) != 0)
		{
			printf("Error seeking to file offset\n");
			file_data[i] = 0;
		}
		else
		{
			file_data[i] = new char[files[i].size];
			fread(file_data[i], 1, files[i].size, data_buf);
		}
	}
	return 0;
}


pack::pack(const char *name, int encrypt)
{
	int size;
	encrypted = encrypt;
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
	
//	unsigned char key[] = {'~', '!', '@', '#', '%', '^', '$', '<'};
//	crypt.initialize(key);
	load_index();
	load_data();
}

void pack::open_data()
{
	if (data_buf == 0)
		data_buf = fopen(data_file, "rb");
}

unsigned char* pack::load_file(int which)
{
	unsigned char *buf;
	open_data();
	if ((which > 0) && (which < num_files))
	{
		if ((files[which].size > 0) && (files[which].offset > 0))
		{
			buf = new unsigned char[files[which].size+8];
			fseek(data_buf, files[which].offset, SEEK_SET);
			fread(buf, 1, files[which].size, data_buf);
			if (encrypted == 1)
			{
				decrypt(buf, buf, files[which].size);
			}
		}
		else
		{
			buf = (unsigned char *)0;
		}
	}
	else
	{
		buf = (unsigned char *)0;
	}
	
	return buf;
}

pack::~pack()
{
	if (file_data != 0)
	{
		for (int i = 0; i < num_files; i++)
		{
			if (file_data[i] != 0)
				delete [] file_data[i];
		}
		delete [] file_data;
	}
	
	delete [] data_file;
	delete [] temp_file;
	delete [] index_file;
}
