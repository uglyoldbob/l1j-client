#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "files.h"
#include "globals.h"
#include "sdl_user.h"

files::files()
{
	num_files = 0;
	change_num_files = false;
	file = 0;

	data_buf = 0;
	file_data = 0;
	data_file = 0;
}

void files::init(char *name)
{
	int size;
	size = strlen(name) + strlen(BCE_EXT) + 1;
	data_file = new char[size];
	sprintf(data_file, "%s%s", name, BCE_EXT);
	
	num_files = 0;
	change_num_files = false;
	file = 0;
	
	printf("Loading SERVER DATA: %s\n", name);
	
	data_buf = 0;
	file_data = 0;
	load();
}

files::~files()
{
	delete_files();
	if (data_file != 0)
	{
		delete [] data_file;
		data_file = 0;
	}
}

/** loads a file of the given name, returns the size unless size is a null pointer, and optionally decrypts it */
unsigned char* files::load_file(const char *name, int *size, file_locations location, int decrypting)
{
	int try_custom = 1;
	unsigned char *buffer;
	buffer = 0;
	size_t filesize = 0;
	if (try_custom == 1)
	{	//this means there is a valid briefcase in the client object
		buffer = (unsigned char*)load_file(name, size);
		if (buffer == 0)
		{	//this file does not exist as a server specific resource
			try_custom = 0;
		}
	}
	
	if (try_custom == 0)
	{	//just load the file, don't bother with the briefcase
		return global_files::load_file(name, size, location, decrypting);
	}
	else
	{
//		if (buffer == 0)
//			printf("File %s was not loaded properly\n", name);
//		else
//			printf("File %s was loaded properly\n", name);
		return buffer;
	}
}

/** This calculates the SHA256 hash of the briefcase. This is used to determine if the users briefcase matches the briefcase the server requires them to use. This simulataneously allows for updates to be sent to the client and prevents modification of the data. */
const char * files::get_hash()
{
	open_data();
	if (data_buf == 0)
	{	//the file does not exist yet
		data_buf = fopen(data_file, "wb");
	}
	fclose(data_buf);
//	sort_file();	//make sure the briefcase is sorted before hashing
	data_buf = 0;
	
	sha256_hash(data_file, hash);
	return hash;
}

/** Sort the list of filenames. This is not very important until a proper search is done in check_file */
void files::sort()
{
	if (sorted == 0)
	{
		qsort(file, num_files, sizeof(struct briefcase_entry), files::compare);
		sorted = 1;
	}
}

/** The helper function for the qsort function */
int files::compare(const void *a, const void *b)
{
	const briefcase_entry *aa = (const briefcase_entry*)a;
	const briefcase_entry *ba = (const briefcase_entry*)b;
	return strcmp(aa->name, ba->name);
}

/** Checks the briefcase for duplicate files. Duplicate files have the same filename and the same size */
int files::detect_dupes()	//detects duplicate files
{
//	printf("Checking for duplicate files\n");
	int dupes = 0;
	for (int i = 0; (i+1) < num_files; i++)
	{
		if (strncmp(file[i].name, file[i+1].name, 20) == 0)
		{	//they match, check file contents
			if (file[i].size == file[i+1].size)
			{	//the sizes also match
				unsigned char *filea;
				unsigned char *fileb;
				if (data_buf == 0)
				{
					data_buf = fopen(data_file, "rb");
				}
				
				if (!( (file[i].size < 0) || 
					 (file[i+1].size < 0) ||
					 (file[i+1].offset < 0) ||
					 (file[i+1].offset < 0)))
				{
					filea = load_file(i);
					fileb = load_file(i+1);
				
					if (memcmp(filea, fileb, file[i].size) == 0)
					{
						dupes++;
						file[i].size = -1;
						file[i].offset = -1;
					}
					delete [] filea;
					filea = 0;
					delete [] fileb;
					fileb = 0;
				}
			}
		}
	}
	if (dupes > 0)
		printf("\tThere were %d duplicate files detected\n", dupes);
	return dupes;
}

/** Loads all the data from a briefcase file or initializes to a blank briefcase if it is not found. */
int files::load()
{
	if (data_buf != 0)
	{
		printf("briefcase data already loaded\n");
		return -1;
	}
	open_data();
	if (data_buf != 0)
	{
		fread(&num_files, 1, 4, data_buf);
		printf("There are %d files\n", num_files);
		if (num_files > 0)
		{
			file_data = new char*[num_files];
			file = new briefcase_entry[num_files];
			for (int i = 0; i < num_files; i++)
			{
				file[i].namelen = 0;
				fread(&file[i].namelen, 1, 4, data_buf);
				if ((file[i].namelen+1) <= 0)
				{
					printf("ERROR: Briefcase file %d has a negative or zero filename length\n", i);
					file[i].name = 0;
				}
				else
				{
					file[i].name = new char[file[i].namelen+1];
					memset(file[i].name, 0, file[i].namelen+1);
					fread(file[i].name, file[i].namelen+1, 1, data_buf);
				}
				file[i].size = 0;
				fread(&file[i].size, 1, 4, data_buf);
				fread(&file[i].offset, 1, 4, data_buf); 
				if (file[i].size <= 0)
				{
					printf("ERROR: Filesize for file %d is negative or zero\n", i);
					file_data[i] = 0;
				}
				else
				{
					file_data[i] = new char[file[i].size];
					fread(file_data[i], 1, file[i].size, data_buf);
				}
			}
		}
		else
		{
			file_data = 0;
			file = 0;
		}
	}
	return 0;
}

int files::check_file(const char *name, file_locations location)
{
	return global_files::check_file(name, location);
}

/** Check to see if the filename is in the briefcase
 * \todo Implement a binary search
 */
int files::check_file(const char *name)
{	//TODO change to a binary search
	//because the list of files is sorted alphabetically
	int i;
	for (i = 0; i < num_files; i++)
	{
		if (strncmp(name, file[i].name, 20) == 0)
		{
			break;
		}
	}
	if (i < num_files)
	{
		return i;
	}
	else
	{
		return -1;
	}
}

/** Write the file to the briefcase
 * \todo Support replacing an existing file in the briefcase
*/
void files::write_file(char *name, char *data, int size)
{	
	if (check_file(name) == -1)
	{
		briefcase_entry *temp = file;
		char **temp2 = file_data;
		
		file_data = new char*[num_files + 1];
		file = new briefcase_entry[num_files + 1];
		change_num_files = true;
		int i;
		for (i = 0; i < num_files; i++)
		{
			file_data[i] = temp2[i];
			file[i] = temp[i];
		}
		file_data[i] = 0;
		file[i].namelen = strlen(name)+1;
		file[i].name = new char[file[i].namelen];
		strcpy(file[i].name, name);
		file[i].size = size; 
				
		int len = strlen(name);
		if (data_buf != 0)
		{
			fwrite(&len, sizeof(int), 1, data_buf);
			fwrite(name, sizeof(char), len+1, data_buf);
			fwrite(&size, sizeof(int), 1, data_buf);
			long offset = ftell(data_buf) + sizeof(long);
			file[i].offset = offset;
			fwrite(&offset, sizeof(long), 1, data_buf);
			fwrite(data, sizeof(char), size, data_buf);
			
			if (temp != 0)
			{
				delete [] temp;
				temp = 0;
			}
			if (temp2 != 0)
			{
				delete [] temp2;
				temp2 = 0;
			}
			num_files++;
		}
	}
	else
	{
		printf("File replacement not supported\n");
	}
}

/** return the size of the briefcase */
int files::get_size()
{
	int save_spot = ftell(data_buf);
	int size;
	fseek(data_buf, 0, SEEK_END);
	size = ftell(data_buf);
	fseek(data_buf, save_spot, SEEK_SET);
	return size;
}

char* files::load_file(const char *name, int *size)
{
	char *ret_buf = (char*)0;
	int i = check_file(name);
	if (i >= 0)
	{
//		printf("Loading %s from %s\n", files[i].name, data_file);
		if (size != 0)
		{
			*size = file[i].size;
		}
		ret_buf = (char*)load_file(i);
	}
	else
	{
//		printf("File %s not found in %s\n", name, index_file);
		ret_buf = (char*)0;
	}
	return ret_buf;
}

unsigned char* files::load_file(int which)
{
	unsigned char *buf;
	open_data();
	if ((which >= 0) && (which < num_files))
	{
		if ((file[which].size > 0) && (file[which].offset > 0))
		{
			buf = new unsigned char[file[which].size+8];
			fseek(data_buf, file[which].offset, SEEK_SET);
			fread(buf, 1, file[which].size, data_buf);
			buf[file[which].size] = 0;
			buf[file[which].size + 1] = 0;
			buf[file[which].size + 2] = 0;
			buf[file[which].size + 3] = 0;
			buf[file[which].size + 4] = 0;
			buf[file[which].size + 5] = 0;
			buf[file[which].size + 6] = 0;
			buf[file[which].size + 7] = 0;
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

/** Write the number of files to the briefcase file and then close it */
void files::finish_briefcase()
{
	if ((data_buf != 0) && (change_num_files))
	{
		printf("Updating the num_files to %d\n", num_files);
		fseek(data_buf, 0, SEEK_SET);
		fwrite(&num_files, 1, 4, data_buf);
	}
	fclose(data_buf);
	data_buf = 0;
}

/** Create a new data file for the briefcase */
void files::new_data()
{
	if (data_buf == 0)
		data_buf = fopen(data_file, "wb");
	printf("Writing dummy value for num_files\n");
	fwrite(&num_files, 1, 4, data_buf);
	delete_files();
}

void files::add_data()
{
	printf("Appending to existing briefcase\n");
	if (data_buf == 0)
		data_buf = fopen(data_file, "ab");
}

void files::open_data()
{
	if (data_buf == 0)
		data_buf = fopen(data_file, "rb");
}

void files::delete_files()
{
	if (file_data != 0)
	{
		for (int i = 0; i < num_files; i++)
		{
			if (file_data[i] != 0)
			{
				delete [] file_data[i];
				file_data[i] = 0;
			}
			if (file[i].name != 0)
			{
				delete [] file[i].name;
				file[i].name = 0;
			}
		}
		delete [] file_data;
		file_data = 0;
		if (file != 0)
		{
			delete [] file;
			file = 0;
		}
	}
	num_files = 0;
}

