#ifndef __UNISTD_H__
#define __UNISTD_H__

#ifndef _MSC_VER
#error do not include this if not Windows
#endif

#include <stdio.h>
#include <io.h>
#include <sys/types.h> // off_t
#include <inttypes.h>

#define S_IFBLK  0060000

//#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK) // already defined in fuse_win.h
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

typedef unsigned int mode_t;

//#define PRIux64 "llxu"

#endif

