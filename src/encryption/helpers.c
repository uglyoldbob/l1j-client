
// MD5DEEP - helpers.c
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
// $Id: helpers.c 311 2011-06-15 22:39:59Z jessekornblum $ 

#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

uint64_t find_block_size(state *s, char *input_str)
{
  unsigned char c;
  uint64_t multiplier = 1;

  if (NULL == s || NULL == input_str)
    return 0;

  if (isalpha(input_str[strlen(input_str) - 1]))
    {
      c = tolower(input_str[strlen(input_str) - 1]);
      // There are deliberately no break statements in this switch
      switch (c) {
      case 'e':
	multiplier *= 1024;    
      case 'p':
	multiplier *= 1024;    
      case 't':
	multiplier *= 1024;    
      case 'g':
	multiplier *= 1024;    
      case 'm':
	multiplier *= 1024;
      case 'k':
	multiplier *= 1024;
      case 'b':
	break;
      default:
	printf("%s: Improper piecewise multiplier ignored", "WHO CARES");
      }
      input_str[strlen(input_str) - 1] = 0;
    }

#ifdef __HPUX
  return (strtoumax ( input_str, (char**)0, 10) * multiplier);
#else
  return (atoll(input_str) * multiplier);
#endif
}


static int is_absolute_path(char *fn)
{
  if (NULL == fn)
    printf("Unknown error in is_absolute_path");
  
#ifdef _WIN32
  return FALSE;
#endif

  return (DIR_SEPARATOR == fn[0]);
}


// The basename function kept misbehaving on OS X, so I rewrote it.
// This function isn't perfect, nor is it designed to be. Because
// we're guarenteed to be working with a file here, there's no way
// that str will end with a DIR_SEPARATOR (e.g. /foo/bar/). This function
// will not work properly for a string that ends in a DIR_SEPARATOR 
int my_basename(char *str)
{
  size_t len;
  char *tmp;

  if (NULL == str)
    return TRUE;

  // We search for the last slash in the input, if any. If there aren't
  // any slashes in the string, we can stop now. There's nothing to do.
  tmp  = strrchr(str,DIR_SEPARATOR);
  if (NULL == tmp)
    return FALSE;

  len = strlen(tmp);

  // We advance tmp one character to move us past the DIR_SEPARATOR
  memmove(str,tmp+1,len);

  return FALSE;
}


#ifndef _WIN32

// Return the size, in bytes of an open file stream. On error, return 0 
#if defined (__LINUX__)


off_t find_file_size(FILE *f) 
{
  if (NULL == f)
    return 0;

  off_t num_sectors = 0, sector_size = 0;
  int fd = fileno(f);
  struct stat sb;

  if (fstat(fd,&sb))
    return 0;

  if (S_ISREG(sb.st_mode) || S_ISDIR(sb.st_mode))
    return sb.st_size;


  return 0;
}  

#elif defined (__APPLE__)

off_t find_file_size(FILE *f) 
{
  if (NULL == f)
    return 0;

  struct stat info;
  off_t total = 0;
  off_t original = ftello(f);
  int fd = fileno(f);
  uint32_t blocksize = 0;
  uint64_t blockcount = 0;

  // I'd prefer not to use fstat as it will follow symbolic links. We don't
  // follow symbolic links. That being said, all symbolic links *should*
  // have been caught before we got here. 

  if (fstat(fd, &info))
  {
    printf("%s: %s", "WHO CARES",strerror(errno));
    return 0;
  }

#ifdef HAVE_SYS_IOCTL_H
  // Block devices, like /dev/hda, don't return a normal filesize.
  // If we are working with a block device, we have to ask the operating
  // system to tell us the true size of the device. 
  //
  // This isn't the recommended way to do check for block devices, 
  // but using S_ISBLK(info.stmode) wasn't working. 
  if (info.st_mode & S_IFBLK)
  {    
    // Get the block size 
    if (ioctl(fd, DKIOCGETBLOCKSIZE,&blocksize) < 0) 
    {
      printf("%s: ioctl DKIOCGETBLOCKSIZE failed: %s", 
		  "WHO CARES", strerror(errno));
      return 0;
    } 
    
    // Get the number of blocks 
    if (ioctl(fd, DKIOCGETBLOCKCOUNT, &blockcount) < 0) 
    {
      printf("%s: ioctl DKIOCGETBLOCKCOUNT failed: %s", 
		  "WHO CARES", strerror(errno));
    }

    total = blocksize * blockcount;
  }
#endif     // ifdef HAVE_IOCTL_H

  else 
  {
    if ((fseeko(f,0,SEEK_END)))
      return 0;
    total = ftello(f);
    if ((fseeko(f,original,SEEK_SET)))
      return 0;
  }

  return (total - original);
}


#else   // ifdef __APPLE__

//  This is code for general UNIX systems 
// (e.g. NetBSD, FreeBSD, OpenBSD, etc) 

static off_t
midpoint (off_t a, off_t b, long blksize)
{
  off_t aprime = a / blksize;
  off_t bprime = b / blksize;
  off_t c, cprime;

  cprime = (bprime - aprime) / 2 + aprime;
  c = cprime * blksize;

  return c;
}



off_t find_dev_size(int fd, int blk_size)
{
  off_t curr = 0, amount = 0;
  void *buf;
 
  if (blk_size == 0)
    return 0;
  
  buf = malloc(blk_size);
  
  for (;;) {
    ssize_t nread;
    
    lseek(fd, curr, SEEK_SET);
    nread = read(fd, buf, blk_size);
    if (nread < blk_size) 
    {
      if (nread <= 0) 
	{
	  if (curr == amount) 
	  {
	    free(buf);
	    lseek(fd, 0, SEEK_SET);
	    return amount;
	  }
	  curr = midpoint(amount, curr, blk_size);
	}
      else 
	{ // 0 < nread < blk_size 
	  free(buf);
	  lseek(fd, 0, SEEK_SET);
	  return amount + nread;
	}
    } 
    else 
    {
      amount = curr + blk_size;
      curr = amount * 2;
    }
  }

  free(buf);
  lseek(fd, 0, SEEK_SET);
  return amount;
}


off_t find_file_size(FILE *f) 
{
  if (NULL == f)
    return 0;

  // The error checking for this is above. If f is not NULL
  // fd should be vald.
  int fd = fileno(f);
  struct stat sb;
  
  if (fstat(fd,&sb))
    return 0;

  if (S_ISREG(sb.st_mode) || S_ISDIR(sb.st_mode))
    return sb.st_size;
  else if (S_ISCHR(sb.st_mode) || S_ISBLK(sb.st_mode))
    return find_dev_size(fd,sb.st_blksize);

  return 0;
}  

#endif // ifdef __LINUX__
#endif // ifndef _WIN32

#if defined(_WIN32)
off_t find_file_size(FILE *f) 
{
  if (NULL == f)
    return 0;

  off_t total = 0, original = ftello(f);
  
  // Windows does not support running fstat on block devices,
  // so there's no point in mucking about with them.
  // 
  // TODO: Find a way to estimate device sizes on Windows
  // Perhaps an IOTCL_DISK_GET_DRIVE_GEOMETRY_EX would work? 

  // RBF - We don't really have the fseeko and ftello functions
  // on windows. They are functions like _ftelli64 or some such
  // RBF - Fix find_file_size for large files on Win32

  if ((fseeko(f,0,SEEK_END)))
    return 0;

  total = ftello(f);
  if ((fseeko(f,original,SEEK_SET)))
    return 0;
  
  return total;
}
#endif // ifdef _WIN32 
