// MD5DEEP - main.c
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
// $Id: main.c 311 2011-06-15 22:39:59Z jessekornblum $

#include "main.h"
#include <stdlib.h>
#include <string.h>


static int initialize_state(state *s)
{
  /* We use PATH_MAX as a handy constant for working with strings
     It may be overkill, but it keeps us out of trouble */
  MD5DEEP_ALLOC(char,s->msg,PATH_MAX);
  MD5DEEP_ALLOC(char,s->full_name,PATH_MAX);
  MD5DEEP_ALLOC(char,s->short_name,PATH_MAX);

  MD5DEEP_ALLOC(unsigned char,s->hash_sum,PATH_MAX);
  MD5DEEP_ALLOC(char,s->hash_result,PATH_MAX);
  MD5DEEP_ALLOC(char,s->known_fn,PATH_MAX);

  s->mode            = mode_none;
  s->hashes_loaded   = FALSE;
  s->return_value    = STATUS_OK;
  s->piecewise_size  = 0;
  s->size_threshold  = 0;
  s->block_size      = MD5DEEP_IDEAL_BLOCK_SIZE;

  if (setup_hashing_algorithm(s))
    return TRUE;

  return FALSE;
}

int sha256_hash(char *filename, char *dest) 
{
  state *s;

  s = (state *)malloc(sizeof(state));
  if (NULL == s)
  {
    // We can't use fatal_error because it requires a valid state
    printf("%s: Unable to allocate state variable", "WHO CARES");
    return STATUS_INTERNAL_ERROR;
  }

  if (initialize_state(s))
  {
    printf("%s: Unable to initialize state variable", "WHO CARES");
    return STATUS_INTERNAL_ERROR;
  }

  // Anything left on the command line at this point is a file
  // or directory we're supposed to process. If there's nothing
  // specified, we should tackle standard input UNLESS the user
  // has specified a list of input files
  
  {
	if (dest == 0)
    	printf("%s: ", filename);
    s->return_value = hash_file(s, filename, dest);//process_win32(s,fn);
  }

  int temp = s->return_value;
  free(s->msg);
  free(s->short_name);
  free(s->hash_sum);
  free(s->hash_result);
  free(s->known_fn);
  free(s->hash_context);
  free(s);
  return temp;
}
