#include "ns_Convertion.h"


#include <stdio.h>
#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/stat.h>


//#include <iconv.h>

/* ICU4C */
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/ucnv.h>
#include <unicode/unorm2.h>
#include <unicode/unistr.h>
#include <unicode/ucnv.h>
#include <unicode/errorcode.h>

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//#include "../src/darling-dmg/src/be.h"
#include "../src/Utils.h"
#include "../src_tests_diskimages/io.h"
#include "../src/utf8proc/utf8proc.h"

#include "icu_Convertion.h"

#include <Foundation/NSByteOrder.h>
#include <Foundation/NSFileHandle.h>
//#include <Foundation/Foundation.h>

using namespace std;


NSString* ns_utf32CodePointToNSString(uint32_t codePoint)
{
    UTF32Char inputChar = codePoint; // input UTF32 value
    inputChar = NSSwapHostIntToLittle(inputChar); // swap to little-endian if necessary
    NSString* str = [[NSString alloc] initWithBytes:&inputChar length:4 encoding:NSUTF32LittleEndianStringEncoding];


////        NSString* nsString = [NSString stringWithFormat:@"%s", icu_Utf32CodePointToUtf8(0x386).c_str()]; // this doesn't work
//    NSString* path = [NSString stringWithUTF8String:icu_Utf32CodePointToUtf8(codePoint).c_str()];
    return str;
}

NSString* ns_utf8ToNSString(const string& s)
{
//        NSString* nsString = [NSString stringWithFormat:@"%s", icu_Utf32CodePointToUtf8(0x386).c_str()]; // this doesn't work
    NSString* nsString = [NSString stringWithUTF8String:s.c_str()];
    return nsString;
}

void nsAppendToFile(NSString* path, NSString* content)
{
    //get the documents directory:

    NSFileHandle *fileHandle = [NSFileHandle fileHandleForWritingAtPath:path];
    if (fileHandle){
        [fileHandle seekToEndOfFile];
        [fileHandle writeData:[content dataUsingEncoding:NSUTF8StringEncoding]];
        [fileHandle closeFile];
    }
    else{
        [content writeToFile:path
                  atomically:NO
                    encoding:NSUTF8StringEncoding
                       error:nil];
    }
}

void nsAppendCodePointToFile(uint32_t codePoint)
{
    NSString* path = [NSString stringWithFormat:@"/Volumes/SprsHfs/unicode_name_test2/%@", ns_utf32CodePointToNSString(codePoint)];
    nsAppendToFile(path, path);
}
