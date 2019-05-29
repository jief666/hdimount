//
//  CrossPlateform.h
//  hdimount--jief666--wip
//
//  Created by jief on 13.05.19.
//  Copyright © 2019 jf-luce. All rights reserved.
//

#ifndef CrossPlateform_h
#define CrossPlateform_h

#include <unistd.h>

#ifndef __FBSDID
#define __FBSDID(x)
#endif

#ifndef _MSC_VER
#define FUSE_STAT stat
#define FUSE_OFF_T off_t
#endif

#ifdef _MSC_VER
#include <stddef.h>
#include <direct.h>

#ifdef _WIN64
typedef __int64 ssize_t;
#define SSIZE_MAX INT64_MAX
#else
typedef int     ssize_t;
#define SSIZE_MAX INT_MAX
#endif


char* realpath(const char* path, char* resolved_name);
ssize_t pread(unsigned int fd, void* buf, size_t count, off_t offset);
char* getpass(const char *prompt);
//inline char* getcwd(char* buf, size_t size)
//{ return _getcwd(buf, size); };
/*

	Declaration of POSIX directory browsing functions and types for Win32.

	Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
	History: Created March 1997. Updated June 2003.
	Rights:  See end of file.

*/


	typedef struct DIR DIR;

	struct dirent
	{
		char d_name[260];
	};

	DIR* opendir(const char*);
	int           closedir(DIR*);
	//struct dirent* readdir(DIR*);
	//void          rewinddir(DIR*);

	/*

		Copyright Kevlin Henney, 1997, 2003. All rights reserved.

		Permission to use, copy, modify, and distribute this software and its
		documentation for any purpose is hereby granted without fee, provided
		that this copyright and permissions notice appear in all copies and
		derivatives.

		This software is supplied "as is" without express or implied warranty.

		But that said, if there are any problems please get in touch.

	*/

	/* added by jief */
	int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);

#else

#define O_BINARY 0

#endif


#endif /* CrossPlateform_h */
