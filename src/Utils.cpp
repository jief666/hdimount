//
//  Utils.cpp
//  hdimount--jief666--wip
//
//  Created by jief on 16/07/2018.
//  Copyright © 2018 jf-luce. All rights reserved.
//

#include "Utils.h"

#include <string.h> // for strlen
#include <limits.h> // for INT_MAX
#include <stdarg.h>
#include <string>
#include <iostream>

#if defined(__linux__) || defined(__APPLE__)
#include <termios.h>
#endif


std::string stringPrintf(const char* fmt, size_t fmt_len, va_list ap)
{
    if ( fmt_len > (INT_MAX-50)/2 ) throw "format paramter to stringPrintf too big";
    int size = (int)fmt_len * 2 + 50;   // Use a rubric appropriate for your code
//size = 1;
    std::string str;
    while (1) {     // Maximum two passes on a POSIX system...
        str.resize(size);
        va_list ap2;
        va_copy(ap2, ap);
        int n = vsnprintf((char *)str.data(), size, fmt, ap2); // vsnprintf returns an int, but size is size_t
        va_end(ap2);
        if (n > -1 && n < size) {  // Everything worked
            str.resize(n);
            return str;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    return str;
}

std::string stringPrintf(const char* fmt, ...)
{
    declare_ap(fmt);
    return stringPrintf(fmt, strlen(fmt), ap);
}

void printHexBufAsCDecl(const uint8_t* buf, size_t count, const char* name, int line_length)
{
	if ( name ) printf("static const uint8_t %s[] = {\n", name);
	for (size_t i = 0; i < count;) {
		printf("\t");
		for (int j = 0; j < line_length; j++) {
			if ( i + j < count-1 ) {
				if ( j < line_length-1 ) {
					printf("0x%02x, ", (uint8_t) (buf[i + j]));
				}else{
					printf("0x%02x,", (uint8_t) (buf[i + j]));
				}
			}
			else if ( i + j < count ) {
				printf("0x%02x", (uint8_t) (buf[i + j]));
			}
		}
		i += line_length;
		printf("\n");
	}
	if ( name ) printf("};\n");
}

#ifdef _WIN32
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '\\' || ch == '/';
    }
};
#else
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};
#endif

std::string basename( std::string const& pathname )
{
#ifdef _WIN32
   char sep = '\\';
#else
   char sep = '/';
#endif

   size_t i = pathname.rfind(sep, pathname.length());
   if (i != std::string::npos) {
      return(pathname.substr(i+1, pathname.length() - i));
   }

   return("");
}

#if defined(DEBUG) && defined(TRACE_FUSE_OP)

void trace_fuse_op(const char* path, const char* format, ...)
{
	if (strcmp("/", path) == 0) return;

	va_list valist;
	va_start(valist, format);

	char buf[2000];
	vsnprintf(buf, sizeof(buf) - 1, format, valist);
	fprintf(stderr, "%s", buf);
}

#endif

