//
//  CrossPlateform.cpp
//  hdimount--jief666--wip
//
//  Created by jief on 13.05.19.
//  Copyright © 2019 jf-luce. All rights reserved.
//

#ifdef _MSC_VER


#include <Windows.h>
#include <stddef.h>

#include <string>
using namespace std;

static char pass[256];

char* getpass(const char* prompt)
{
	const char BACKSPACE = 8;
	const char RETURN = 13;

	size_t passLength = 0;

	printf("%s", prompt);

	DWORD con_mode;
	DWORD dwRead;

	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

	GetConsoleMode(hIn, &con_mode);
	SetConsoleMode(hIn, con_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

	while (passLength < sizeof(pass)  &&  ReadConsoleA(hIn, &pass[passLength], 1, &dwRead, NULL) && pass[passLength] != RETURN  )
	{
		if (pass[passLength] == BACKSPACE)
		{
			if (passLength != 0) passLength -= 1;
		}
	}
	printf("\n");
	return pass;
}

/*
realpath() Win32 implementation
By Nach M. S.
Copyright (C) September 8, 2005

I am placing this in the public domain for anyone to use or modify
*/

#include <windows.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>

ssize_t pread(unsigned int fd, void* buf, size_t count, off_t offset)
{
	if (_lseek(fd, offset, SEEK_SET) != offset) {
		return -1;
	}
#ifdef DEBUG
	memset(buf, 0, count);
#endif
	int nbreadtotal = 0;
	while ( nbreadtotal < count ) {
        int nbread = read(fd, (uint8_t*)buf+nbreadtotal, count-nbreadtotal);
        if ( nbread < 0 ) return nbread;
        nbreadtotal += nbread;
        if ( nbread == 0 ) return nbreadtotal;
    }
	return nbreadtotal;
}

char* realpath(const char* path, char* resolved_path)
{
	if (resolved_path != NULL) throw "Not supported";

	if (!path) //Else EINVAL
	{
		errno = EINVAL;
		return NULL;
	}

    char* return_path = (char*)malloc(4096);
    if (!return_path) //Else EINVAL
    {
		errno = EINVAL;
		return NULL;
	}
	
	
	//This is a Win32 API function similar to what realpath() is supposed to do
      size_t size = GetFullPathNameA(path, 4096, return_path, 0);

      //GetFullPathNameA() returns a size larger than buffer if buffer is too small
      if (size > 4096)
      {
          return_path = (char*)realloc(return_path, size);
          if (!return_path)
		  {
			  errno = EINVAL;
			  return NULL;
		  }
		  
		  size_t new_size;
		  new_size = GetFullPathNameA(path, size, return_path, 0); //Try again

          if (new_size > size) //If it's still too large, we have a problem, don't try again
          {
              free(return_path);
              errno = ENAMETOOLONG;
			  return NULL;
          }
      }

      //GetFullPathNameA() returns 0 if some path resolve problem occured
      if (size == 0)
      {
        //Convert MS errors into standard errors
        switch (GetLastError())
        {
          case ERROR_FILE_NOT_FOUND:
            errno = ENOENT;
			return NULL;

          case ERROR_PATH_NOT_FOUND: case ERROR_INVALID_DRIVE:
            errno = ENOTDIR;
			return NULL;

          case ERROR_ACCESS_DENIED:
            errno = EACCES;
			return NULL;

          default: //Unknown Error
            errno = EIO;
			return NULL;
		}
      }

      //If we get to here with a valid return_path, we're still doing good
        struct stat stat_buffer;

        //Make sure path exists, stat() returns 0 on success
        if (stat(return_path, &stat_buffer))
        {
          //stat() will set the correct errno for us
			return NULL;
        }
        //else we succeeded!
    
  return return_path;
}

/*

	Implementation of POSIX directory browsing functions and types for Win32.

	Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
	History: Created March 1997. Updated June 2003 and July 2012.
	Rights:  See end of file.

*/

//#include <dirent.h>
#include <errno.h>
#include <io.h> /* _findfirst and _findnext set errno iff they return -1 */
#include <stdlib.h>
#include <string.h>


	typedef ptrdiff_t handle_type; /* C99's intptr_t not sufficiently portable */

	struct DIR
	{
		handle_type         handle; /* -1 for failed rewind */
		struct _finddata_t  info;
		struct dirent       result; /* d_name null iff first time */
		char* name;  /* null-terminated char string */
	};

	DIR* opendir(const char* name)
	{
		DIR* dir = 0;

		if (name && name[0])
		{
			size_t base_length = strlen(name);
			const char* all = /* search pattern must end with suitable wildcard */
				strchr("/\\", name[base_length - 1]) ? "*" : "/*";

			dir = (DIR*)malloc(sizeof * dir);
			dir->name = (char*)malloc(base_length + strlen(all) + 1);
			strcat(strcpy(dir->name, name), all);
		}
		else
		{
			errno = EINVAL;
		}

		return dir;
	}

	int closedir(DIR* dir)
	{
		int result = -1;

		if (dir)
		{
			if (dir->handle != -1)
			{
				result = _findclose(dir->handle);
			}

			free(dir->name);
			free(dir);
		}

		if (result == -1) /* map all errors to EBADF */
		{
			errno = EBADF;
		}

		return result;
	}

	//struct dirent* readdir(DIR* dir)
	//{
	//	struct dirent* result = 0;

	//	if (dir && dir->handle != -1)
	//	{
	//		if (!dir->result.d_name || _findnext(dir->handle, &dir->info) != -1)
	//		{
	//			result = &dir->result;
	//			result->d_name = dir->info.name;
	//		}
	//	}
	//	else
	//	{
	//		errno = EBADF;
	//	}

	//	return result;
	//}

	//void rewinddir(DIR* dir)
	//{
	//	if (dir && dir->handle != -1)
	//	{
	//		_findclose(dir->handle);
	//		dir->handle = (handle_type)_findfirst(dir->name, &dir->info);
	//		dir->result.d_name = 0;
	//	}
	//	else
	//	{
	//		errno = EBADF;
	//	}
	//}

	int readdir_r(DIR *dir, struct dirent *result, struct dirent **resultPtr)
	{
		if (dir && dir->handle != -1)
		{
			if (dir->handle == 0) {
				dir->handle = (handle_type)_findfirst(dir->name, &dir->info);
				if (dir->handle != -1) {
					strncpy(result->d_name, dir->info.name, sizeof(result->d_name));
					*resultPtr = result;
					return 0;
				}
			}
			if (dir->handle != 0) {
				int rv = _findnext(dir->handle, &dir->info);
				if (rv == 0) {
					strncpy(result->d_name, dir->info.name, sizeof(result->d_name));
					*resultPtr = result;
					return 0;
				}
			}
		}
		*resultPtr = NULL;
		return -1;
	}

/*

	Copyright Kevlin Henney, 1997, 2003, 2012. All rights reserved.

	Permission to use, copy, modify, and distribute this software and its
	documentation for any purpose is hereby granted without fee, provided
	that this copyright and permissions notice appear in all copies and
	derivatives.

	This software is supplied "as is" without express or implied warranty.

	But that said, if there are any problems please get in touch.

*/

#endif

