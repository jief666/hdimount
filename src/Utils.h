//
//  Utils.hpp
//  hdimount--jief666--wip
//
//  Created by jief on 16/07/2018.
//  Copyright © 2018 jf-luce. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <string>


// useful to resolve signed / unsigned warning when it's sure that offset is > 0.
// if off_t isn't long long int on some plateform, might have to #ifdef.
// It should be true for all 64 bits models (ILP32, LP64, LLP64, ILP64) as they all define long long int as 64 bits
//   and most likely, off_t will always be 64 bits on 64 bits plateform, but I'm not sure.
// So far, it's true for Apple 64 bits, linux 32 bits (with _FILE_OFFSET_BITS=64).
#include <stdio.h>
#include <sys/types.h>

# ifdef __USE_FILE_OFFSET64
typedef uint64_t uoff_t;
#else
typedef unsigned long int uoff_t;
#endif

#if defined(__cpp_static_assert)
	static_assert(sizeof(off_t) == sizeof(uoff_t), "sizeof(off_t) == sizeof(uoff_t)");
#else
	#warning Make sure of sizeof(off_t) == sizeof(uoff_t)
#endif


void printHexBufAsCDecl(const uint8_t* buf, size_t count, const char* name, int line_length = 8);
std::string stprintf(const char* format, ...);
bool GetPassword(std::string* pwPtr);



#endif /* Utils_hpp */
