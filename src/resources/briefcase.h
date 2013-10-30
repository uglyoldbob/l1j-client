#ifndef _BRIEFCASE_H_
#define _BRIEFCASE_H_

#include <stdio.h>

//#include "des.h"

#define BCE_EXT ".bce"

/**
 * Defines an entry for custom resources of a server
 */
struct briefcase_entry
{
	int namelen;
	char *name;
	int size;
	long offset;
};

/** This class is for handling the custom resources of a server.
 *
 */
class briefcase
{
	public:
		briefcase(const char *name);
		int add_file(const char *file);	/**< Add a file to the briefcase */
		int save();	/**< save the briefcase to disk */
		int detect_dupes();	/**< detects duplicate files in the briefcase */
		unsigned char* load_file(int which); /**< Load a file with the given index */
		char *load_file(const char *name, int *size);	/**< Load a file with the given name, return the size of it as well */
		int check_file(const char *name);	/**< Check to see if the given filename is in the briefcase */
		void sort();	/**< Sort the list of filenames for searching */
		static int compare(const void *a, const void *b);	/**< This function is for qsort */
		const char *get_hash();	/**< This computes the hash of the briefcase for updating purposes */
		void new_data();	/**< Sets up a new briefcase file */
		void add_data();	/**< Opens an existing briefcase */
		void write_file(char *name, char *data, int size);	/**< Adds a file to the briefcase */
		void finish_briefcase();	/**< Finalizes the briefcase by writing all changes to disc */
		
		~briefcase();
	private:
		char* data_file;	/**< Name of the briefcase file */
		FILE* data_buf;	/**< FILE pointer to the file */

		char hash[65];	/**< the sha256 hash **/
		int sorted;	/**< is the list of files properly sorted? */
		int num_files;	/**< Number of files in the briefcase */
		bool change_num_files;	/**< Has the number of files changed since opening/saving the briefcase? */
		briefcase_entry* files;	/**< the list of files with all their information */
		char** file_data;	/**< pointers to the file data for every file, not all are valid all the time */
		
		int load();	/**< Load the briefcase data */
		void open_data();	/**< Open the briefcase data file for reading */
		
		int get_size();	/**< Returns the size of the briefcase */
};

#endif
