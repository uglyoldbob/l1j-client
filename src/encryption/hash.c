// MD5DEEP - hash.c
//
// By Jesse Kornblum
//
// This is a work of the US Government. In accordance with 17 USC 105,
// copyright protection is not available for any work of the US Government.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// $Id: hash.c 272 2011-04-23 21:05:35Z jessekornblum $


#include "main.h"
#include <stdlib.h>
#include <string.h>

#ifdef __MD5DEEP_H

// Code for md5deep 

#define HASH_INITIALIZE()      s->hash_init(s->hash_context)
#define HASH_UPDATE(BUF,SIZE)  s->hash_update(s->hash_context,BUF,SIZE)
#define HASH_FINALIZE()        s->hash_finalize(s->hash_context,s->hash_sum)

#else
		    
// Code for hashdeep 

#define HASH_INITIALIZE()      multihash_initialize(s)
#define HASH_UPDATE(BUF,SIZE)  multihash_update(s,BUF,SIZE)
#define HASH_FINALIZE()        multihash_finalize(s);

#endif // ifdef __MD5DEEP_H 
		    

int display_hash(state *s)
{
  if (NULL == s)
    return TRUE;
  
  if (s->mode & mode_triage)
  {
    printf ("\t%s\t", s->hash_result);
//    display_filename(stdout,s->full_name);
    //make_newline(s);
    return FALSE;
  }    


//  display_size(s);

  printf ("%s", s->hash_result);

  if (s->mode & mode_quiet)
    printf ("  ");
  else
  {
    if ((s->mode & mode_piecewise) ||
	!(s->is_stdin))
    {
      if (s->mode & mode_timestamp)
      {
	struct tm * my_time = _gmtime64(&(s->timestamp));

	// The format is four digit year, two digit month, 
	// two digit hour, two digit minute, two digit second
	strftime(s->time_str, 
		 MAX_TIME_STRING_LENGTH, 
		 "%Y:%m:%d:%H:%M:%S", 
		 my_time);

	printf ("%c%s", (s->mode & mode_csv?',':' '), s->time_str);
      }

      
      if (s->mode & mode_csv)
	printf(",");
      else
	printf("\n");      

 //     display_filename(stdout,s->full_name);
    }
  }

  //make_newline(s);
  return FALSE;
}




static void shorten_filename(char *dest, char *src)
{
  char *basen;

  if (NULL == dest || NULL == src)
    return;

  if (strlen(src) < MAX_FILENAME_LENGTH)
  {
    strncpy(dest,src, MAX_FILENAME_LENGTH);
    return;
  }

  basen = strdup(src);
  if (NULL == basen)
    return;

  if (my_basename(basen))
    return;

  if (strlen(basen) < MAX_FILENAME_LENGTH)
  {
    strncpy(dest,basen,MAX_FILENAME_LENGTH);
    return;
  }

  basen[MAX_FILENAME_LENGTH - 3] = 0;
  snprintf(dest,MAX_FILENAME_LENGTH,"%s...",basen);
  free(basen);
}


// Returns TRUE if errno is currently set to a fatal error. That is,
// an error that can't possibly be fixed while trying to read this file
static int file_fatal_error(void)
{
  switch(errno) 
    {
    case EINVAL:   // Invalid argument (happens on Windows)
    case EACCES:   // Permission denied
    case ENODEV:   // Operation not supported (e.g. trying to read 
                   //   a write only device such as a printer)
    case EBADF:    // Bad file descriptor
    case EFBIG:    // File too big
    case ETXTBSY:  // Text file busy
      // The file is being written to by another process.
      // This happens with Windows system files 
    case EIO:      // Input/Output error
      // Added 22 Nov 2010 in response to user email

      return TRUE;  
    }
  
  return FALSE;
}


static int compute_hash(state *s)
{
  time_t current_time;
  uint64_t current_read, mysize, remaining, this_start;
  unsigned char buffer[MD5DEEP_IDEAL_BLOCK_SIZE];

  if (NULL == s)
    return TRUE;

  // Although we need to read MD5DEEP_BLOCK_SIZE bytes before
  // we exit this function, we may not be able to do that in 
  // one read operation. Instead we read in blocks of 8192 bytes 
  // (or as needed) to get the number of bytes we need. 

  if (s->block_size < MD5DEEP_IDEAL_BLOCK_SIZE)
    mysize = s->block_size;
  else
    mysize = MD5DEEP_IDEAL_BLOCK_SIZE;

  remaining = s->block_size;

  // We get weird results calling ftell on stdin!
  if (!(s->is_stdin))
    s->read_start = ftello(s->handle);
  s->read_end   = s->read_start;
  s->bytes_read = 0;

  while (TRUE) 
  {    
    // Clear the buffer in case we hit an error and need to pad the hash 
    memset(buffer,0,mysize);

    this_start = s->read_end;

    current_read = fread(buffer, 1, mysize, s->handle);
    
    s->actual_bytes += current_read;
    s->read_end     += current_read;
    s->bytes_read   += current_read;
      
    // If an error occured, display a message but still add this block 
    if (ferror(s->handle))
    {
      if ( ! (s->mode & mode_silent))
	printf("error at offset %"PRIu64": %s",
			    ftello(s->handle),
			    strerror(errno));
	   
      if (file_fatal_error())
	return FALSE; 
      
      HASH_UPDATE(buffer,current_read);
      
      clearerr(s->handle);
      
      // The file pointer's position is now undefined. We have to manually
      // advance it to the start of the next buffer to read. 
      fseeko(s->handle,SEEK_SET,this_start + mysize);
    } 
    else
    {
      // If we hit the end of the file, we read less than MD5DEEP_BLOCK_SIZE
      // bytes and must reflect that in how we update the hash.
      HASH_UPDATE(buffer,current_read);
    }
    
    // Check if we've hit the end of the file 
    if (feof(s->handle))
    {	
      // If we've been printing time estimates, we now need to clear the line.
      if (s->mode & mode_estimate)
	fprintf(stderr,"\r%s\r",BLANK_LINE);

      return TRUE;
   } 

    // In piecewise mode we only hash one block at a time
    if (s->mode & mode_piecewise)
    {
      remaining -= current_read;
      if (remaining == 0)
	return TRUE;

      if (remaining < MD5DEEP_IDEAL_BLOCK_SIZE)
	mysize = remaining;
    }
    
    if (s->mode & mode_estimate)
    {
      time(&current_time);
      
      // We only update the display only if a full second has elapsed 
      if (s->last_time != current_time) 
      {
	s->last_time = current_time;
//	update_display(s,current_time - s->start_time);
      }
    }
  }      
}


// Macro to convert raw hex bytes to ASCII output
#ifdef __MD5DEEP_H
static char hex[] = "0123456789abcdef";	
#define HASH_TO_STR(SRC,DEST,LEN)				\
  size_t __i;							\
  for (__i = 0; __i < LEN ; ++__i)				\
    {								\
      DEST[2 * __i    ] = hex[(SRC[__i] >> 4) & 0xf];		\
      DEST[2 * __i + 1] = hex[ SRC[__i]       & 0xf];		\
    }

static int hash_triage(state *s)
{
  if (NULL == s)
    return TRUE;

  memset(s->hash_result,0,(2 * s->hash_length) + 1);

  // We use the piecewise mode to get a partial hash of the first 
  // 512 bytes of the file. But we'll have to remove piecewise mode
  // before returning to the main hashing code
  s->block_size = 512;
  s->mode |= mode_piecewise;

  HASH_INITIALIZE();
    
  if (!compute_hash(s))
  {
    if (s->mode & mode_piecewise)
      free(s->full_name);
    return TRUE;
  }

  s->mode -= mode_piecewise;
  
  HASH_FINALIZE();
  HASH_TO_STR(s->hash_sum, s->hash_result, s->hash_length);

  printf ("%"PRIu64"\t%s", s->stat_bytes, s->hash_result);
  
  return FALSE;
}
#endif


static int hash(state *s)
{
  int done = FALSE, status = FALSE;
  char *tmp_name = NULL;
  //  uint64_t start_offset;
  
  if (NULL == s)
    return TRUE;

  s->actual_bytes = 0LL;

  if (s->mode & mode_estimate)
  {
    time(&(s->start_time));
    s->last_time = s->start_time;
  }

#ifdef __MD5DEEP_H
  if (s->mode & mode_triage)
  {
    // Hash and display the first 512 bytes of this file
    hash_triage(s);

    // Rather than muck about with updating the state of the input
    // file, just reset everything and process it normally.
    s->actual_bytes = 0;
    fseeko(s->handle, 0, SEEK_SET);
  }
#endif
  
  if ( s->mode & mode_piecewise )
  {
    s->block_size = s->piecewise_size;
    
    // We copy out the original file name and saved it in tmp_name
    tmp_name = s->full_name;
    s->full_name = (char *)malloc(sizeof(char) * PATH_MAX);
    if (NULL == s->full_name)
    {
      return TRUE;
    }
  }
  
  while (!done)
  {
#ifdef __MD5DEEP_H
    memset(s->hash_result,0,(2 * s->hash_length) + 1);
#endif
    HASH_INITIALIZE();
    
    s->read_start = s->actual_bytes;

    if (!compute_hash(s))
    {
      if (s->mode & mode_piecewise)
	free(s->full_name);
      return TRUE;
    }

    // We should only display a hash if we've processed some
    // data during this read OR if the whole file is zero bytes long.
    // If the file is zero bytes, we won't have read anything, but
    // still need to display a hash.
    if (s->bytes_read != 0 || 0 == s->stat_bytes)
    {
      if (s->mode & mode_piecewise)
      {
	uint64_t tmp_end = 0;
	if (s->read_end != 0)
	  tmp_end = s->read_end - 1;
	snprintf(s->full_name,PATH_MAX,"%s offset %"PRIu64"-%"PRIu64,
		   tmp_name, s->read_start, tmp_end);
      }
      
      HASH_FINALIZE();

#ifdef __MD5DEEP_H
      HASH_TO_STR(s->hash_sum, s->hash_result, s->hash_length);
      
      // Under not matched mode, we only display those known hashes that
      // didn't match any input files. Thus, we don't display anything now.
      // The lookup is to mark those known hashes that we do encounter
      if (s->mode & mode_not_matched)
		status = 1;//is_known_hash(s->hash_result,NULL);
      else
		display_hash(s);
#else
      display_hash(s);
#endif    
    }

    if (s->mode & mode_piecewise)
      done = feof(s->handle);
    else
      done = TRUE;
  }


  
  if (s->mode & mode_piecewise)
  {
    free(s->full_name);
    s->full_name = tmp_name;
  }
  
  return status;
}


static int setup_barename(state *s, char *fn)
{
  if (NULL == s || NULL == fn)
    return TRUE;

  char *basen = strdup(fn);
  if (basen == NULL)
  {
    printf("Out of memory");
    return TRUE;
  }

  if (my_basename(basen))
  {
    free(basen);
    printf("%s: Illegal filename", "CANT SAY");
    return TRUE;
  }

  s->full_name = basen;
  return FALSE;
}


int hash_file(state *s, char *fn)
{
  int status = STATUS_OK;

  if (NULL == s || NULL == fn)
    return TRUE;

  s->is_stdin = FALSE;

  if (s->mode & mode_barename)
  {
    if (setup_barename(s,fn))
      return TRUE;
  }
  else
    s->full_name = fn;

  if ((s->handle = fopen(fn,"rb")) != NULL)
  {
    // We should have the file size already from the stat functions
    // called during digging. If for some reason that failed, we'll
    // try some ioctl calls now to get the full size.
    if (UNKNOWN_FILE_SIZE == s->stat_bytes)
      s->stat_bytes = find_file_size(s->handle);

    // If this file is above the size threshold set by the user, skip it
    if ((s->mode & mode_size) && (s->stat_bytes > s->size_threshold))
    {
      if (s->mode & mode_size_all)
      {
	// Whereas md5deep has only one hash to wipe, hashdeep has several
#ifdef __MD5DEEP_H
	memset(s->hash_result, '*', HASH_STRING_LENGTH);
#else
	int i;
	for (i = 0 ; i < NUM_ALGORITHMS ; ++i)
	{
	  if (s->hashes[i]->inuse)
	    memset(s->current_file->hash[i], '*', s->hashes[i]->byte_length);
	}
#endif

	display_hash(s);
      }

      fclose(s->handle);
      return STATUS_OK;
    }


    if (s->mode & mode_estimate)
    {
      s->stat_megs = s->stat_bytes / ONE_MEGABYTE;
      shorten_filename(s->short_name,s->full_name);    
    }    

    status = hash(s);

    fclose(s->handle);
  }
  else
  {
    printf("%s", strerror(errno));
  }

  
  return status;
}


int hash_stdin(state *s)
{
  if (NULL == s)
    return TRUE;

  strncpy(s->full_name,"stdin",PATH_MAX);
  s->is_stdin  = TRUE;
  s->handle    = stdin;

  if (s->mode & mode_estimate)
  {
    s->short_name = s->full_name;
    s->stat_megs = 0LL;
  }

  return (hash(s));
}
