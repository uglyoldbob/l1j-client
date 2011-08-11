#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "lindes.h"
#include "pack.h"

//in encrypt.cpp
//void decrypt(unsigned char* buffer, unsigned char *buf2, unsigned int length);
//void encrypt(unsigned char* buffer, unsigned char *buf2, unsigned int length);

int getHashIndex(const char *name)
{
	unsigned int i = 0;
	int j = 0;
	while (i < strlen(name))
	{
		j += name[i];
		i++;
	}
	//addze?
	return j&0xF;
}

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
//	printf("Loading list of %d files\n", num_files);
	files = new file_entry[num_files];

	//read the files all at once
	fread(files, sizeof(file_entry), num_files, index_buf);

	if (encrypted == 1)
	{
//		printf("Attempting to decrypt\n");
		DesReadBlock((char*)files, num_files*sizeof(file_entry));
//		decrypt((unsigned char*)files, (unsigned char*)files, num_files * sizeof(file_entry));
//		crypt.decrypt_block((unsigned char*)files, num_files * sizeof(file_entry));
	}
	
	for (int i = 0; i < num_files; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			files[i].name[j] = tolower(files[i].name[j]);
		}
	}
	
//	if (files[0].offset != 0)
//	{	//fix the first offset
//		printf("\tFixing offset of the first file %02lx%02lx%02lx%02lx\n", 
//			(files[0].offset>>16)&0xFF,
//			files[0].offset>>24, 
//			(files[0].offset>>8)&0xFF,
//			files[0].offset&0xFF);
//		files[0].offset = 0;
//	}
	
//	for (int i = 0; i < 20; i++)
//	{
//		if (files[i].name[19] != 0)
//		{
//			for (int j = 0; j < 20; j++)
//			{
//				printf("%02x:", files[i].name[j] & 0xFF);
//			}
//			printf("\n\t");
//			printf(" length %08x @ %08lx\n", 
//				files[i].size, files[i].offset);
//		}
//		else
//		{
//			printf("%s length %08x @ %08lx\n", 
//				files[i].name, files[i].size, files[i].offset);
//		}
//	}

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
	if (num_invalid > 0)
		printf("\tThere were %d invalid file entries detected\n", num_invalid);

	return 0;
}

int pack::detect_dupes()	//detects duplicate files
{
//	printf("Checking for duplicate files\n");
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
					//dont decrypt files to compare them
					filea = load_file(i, 0);
					fileb = load_file(i+1, 0);
				
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
	if (dupes > 0)
		printf("\tThere were %d duplicate files detected\n", dupes);
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
	size = strlen(lineage_dir) + strlen(name) + strlen(DATA_EXT) + 1;
	data_file = new char[size];
	sprintf(data_file, "%s%s%s", lineage_dir, name, DATA_EXT);
	
	size = strlen(lineage_dir) + strlen(name) + strlen(TEMP_EXT) + 1;
	temp_file = new char[size];
	sprintf(temp_file, "%s%s%s", lineage_dir, name, TEMP_EXT);
	
	size = strlen(lineage_dir) + strlen(name) + strlen(INDEX_EXT) + 1;
	index_file = new char[size];
	sprintf(index_file, "%s%s%s", lineage_dir, name, INDEX_EXT);
	
	num_files = 0;
	
	printf("Loading %s%s\n", lineage_dir, name);
	
	data_buf = 0;
	index_buf = 0;
	temp_buf = 0;
	file_data = 0;
	
//	unsigned char key[] = {'~', '!', '@', '#', '%', '^', '$', '<'};
//	crypt.initialize(key);
	load_index();
	load_data();
}

unsigned char* pack::load_png(const char *name, int *size, int decrypting)
{	//because the file might need to be transformed before it can be used
	char *buffer = load_file(name, size, decrypting);
	if (buffer != 0)
	{
		if (buffer[3] == 0x58)
		{	//c963c
//			printf("Normalizing mangled PNG file\n");
			buffer[3] = 0x47;
			if (*size > 5)
			{	//c9654
				for (int i = 1; i <= (*size-5); i++)
				{	//c9660, i = ctr
					buffer[*size-i] ^= buffer[*size-i-1];
					buffer[*size-i] ^= 0x52;
				}
			}
		}
//		printf("Finished loading %s\n", name);
	}
	else
	{
//		printf("Failed to load %s\n", name);
	}
	return (unsigned char*)buffer;
}

char *pack::load_file(const char *name, int *size, int decrypting)
{
	char *ret_buf = (char*)0;
	int i;
	for (i = 0; i < num_files; i++)
	{
		if (strncmp(name, files[i].name, 20) == 0)
		{
			break;
		}
	}
	if (i < num_files)
	{
//		printf("Loading %s from %s\n", files[i].name, data_file);
		*size = files[i].size;
		ret_buf = (char*)load_file(i, decrypting);
	}
	else
	{
//		printf("File %s not found in %s\n", name, index_file);
		ret_buf = (char*)0;
	}
	return ret_buf;
}

unsigned char* pack::load_file(int which, int decrypting)
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
			if (decrypting == 1)
			{
//				decrypt(buf, buf, files[which].size);
				DesReadBlock((char*)buf, files[which].size);
			}
			buf[files[which].size] = 0;
			buf[files[which].size + 1] = 0;
			buf[files[which].size + 2] = 0;
			buf[files[which].size + 3] = 0;
			buf[files[which].size + 4] = 0;
			buf[files[which].size + 5] = 0;
			buf[files[which].size + 6] = 0;
			buf[files[which].size + 7] = 0;
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

void pack::open_data()
{
	if (data_buf == 0)
		data_buf = fopen(data_file, "rb");
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
