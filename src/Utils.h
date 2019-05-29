//
//  Utils.hpp
//  hdimount--jief666--wip
//
//  Created by jief on 16/07/2018.
//  Copyright © 2018 jf-luce. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <stdarg.h>
#include <stdint.h>
#include <string>

// useful to resolve signed / unsigned warning when it's sure that offset is > 0.
// if off_t isn't long long int on some plateform, might have to #ifdef.
// It should be true for all 64 bits models (ILP32, LP64, LLP64, ILP64) as they all define long long int as 64 bits
//   and most likely, off_t will always be 64 bits on 64 bits plateform, but I'm not sure.
// So far, it's true for Apple 64 bits, linux 32 bits (with _FILE_OFFSET_BITS=64).
#include <stdio.h>
#include <sys/types.h>

#include "apfs-fuse/ApfsLib/Global.h"




//#define TRACE_FUSE_OP












#ifndef MIN
#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#endif

class __ap
{
public:
    va_list* m_apPtr;
    __ap(va_list* apPtr) : m_apPtr(apPtr) {};
    __ap() { va_end(*m_apPtr); }
};

#define declare_ap(arg) \
    va_list ap; \
    va_start(ap, arg); \
    __ap ap2(&ap);

std::string stringPrintf(const char* fmt, ...)
#ifndef _MSC_VER
                  __attribute__((format(printf, 1, 2))) // __attribute__ format printf
#endif
                  ;

void printHexBufAsCDecl(const uint8_t* buf, size_t count, const char* name, int line_length = 8);
//std::string stringPrintf(const char* format, ...);
//bool GetPassword(std::string* pwPtr);

std::string basename( std::string const& pathname );

#if defined(DEBUG) && defined(TRACE_FUSE_OP)

#ifndef _MSC_VER
void trace_fuse_op(const char* path, const char* format, ...) __attribute__((__format__(printf, 2, 3)));
#else
void trace_fuse_op(const char* path, const char* format, ...);
#endif

#else

#define trace_fuse_op(path, format, ...)

#endif



#endif /* Utils_hpp */
