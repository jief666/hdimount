#include "icu_Convertion.h"


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

using icu::UnicodeString;
using namespace std;

UConverter* g_utf32;
UConverter* g_utf16be;
UConverter* g_utf16le;
UConverter* g_utf8;


static uint16_t be(uint16_t u16)
{
    return (u16<<8) + (u16>>8);
}

UnicodeString icu_Utf8ToUnicodeString(const std::string& str2)
{
    UErrorCode error = U_ZERO_ERROR;
    UnicodeString ustr2 = UnicodeString((char*)str2.data(), str2.length(), g_utf8, error);
    if ( error > 0 ) {
printf("icu_Utf8ToUnicodeString error\n");
        return UnicodeString();
    }
    return ustr2;
}

UnicodeString icu_Utf16BEToUnicodeString(const Utf16String& str2)
{
    UErrorCode error = U_ZERO_ERROR;
    UnicodeString ustr2 = UnicodeString((char*)str2.uint16Array, str2.lengthInBytes, g_utf16be, error);
    if ( error > 0 ) {
printf("icu_Utf16ToUnicodeString error\n");
        return UnicodeString();
    }
    return ustr2;
}

UnicodeString icu_Utf16LEToUnicodeString(const Utf16String& str2)
{
    UErrorCode error = U_ZERO_ERROR;
    UnicodeString ustr2 = UnicodeString((char*)str2.uint16Array, str2.lengthInBytes, g_utf16le, error);
    if ( error > 0 ) {
printf("icu_Utf16LEToUnicodeString error\n");
        return UnicodeString();
    }
    return ustr2;
}

UnicodeString icu_Utf32ToUnicodeString(const Utf32String& str2)
{
    UErrorCode error = U_ZERO_ERROR;
    UnicodeString ustr2 = UnicodeString((char*)str2.uint32Array, str2.lengthInBytes, g_utf32, error);
    if ( error > 0 ) {
printf("icu_Utf32ToUnicodeString error\n");
        return UnicodeString();
    }
    return ustr2;
}

string icu_UnicodeStringToUtf8(const UnicodeString& ustr)
{
    string s;
    ustr.toUTF8String(s);
    return s;
}

Utf16String icu_UnicodeStringToUtf16BE(const UnicodeString& ustr)
{
    Utf16String rv;
    UErrorCode error = U_ZERO_ERROR;
    rv.lengthInBytes = ustr.extract((char*)rv.uint16Array, sizeof(rv.uint16Array), g_utf16be, error);
    if ( error > 0 ) {
printf("icu_UnicodeStringToUtf16 error\n");
        return Utf16String();
    }
    return rv;
}

Utf16String icu_UnicodeStringToUtf16LE(const UnicodeString& ustr)
{
    Utf16String rv;
    UErrorCode error = U_ZERO_ERROR;
    rv.lengthInBytes = ustr.extract((char*)rv.uint16Array, sizeof(rv.uint16Array), g_utf16le, error);
    if ( error > 0 ) {
printf("icu_UnicodeStringToUtf16LE error\n");
        return Utf16String();
    }
    return rv;
}

Utf32String icu_UnicodeStringToUtf32(const UnicodeString& ustr)
{
    Utf32String rv;
    UErrorCode error = U_ZERO_ERROR;
    rv.lengthInBytes = ustr.extract((char*)rv.uint32Array, sizeof(rv.uint32Array), g_utf32, error);
    if ( error > 0 ) {
printf("icu_UnicodeStringToUtf32 error\n");
        return Utf32String();
    }
    return rv;
}

string icu_Utf16BEtoUtf8(const Utf16String& str2)
{
    return icu_UnicodeStringToUtf8(icu_Utf16BEToUnicodeString(str2));
}

Utf16String icu_Utf8toUtf16BE(const string& str2)
{
    return icu_UnicodeStringToUtf16BE(icu_Utf8ToUnicodeString(str2));
}

Utf16String icu_Utf8toUtf16LE(const string& str2)
{
    return icu_UnicodeStringToUtf16LE(icu_Utf8ToUnicodeString(str2));
}

string icu_Utf16LEtoUtf8(const Utf16String& str2)
{
    return icu_UnicodeStringToUtf8(icu_Utf16LEToUnicodeString(str2));
}

string icu_Utf32CodePointToUtf8(uint32_t utf32CodePoint)
{
    return icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String(utf32CodePoint)));
}


uint16Pair_t Utf32CodePointToUtf16Pair(uint32_t utf32CodePoint)
{
    uint16Pair_t uint16Pair;
    uint32_t inputChar = utf32CodePoint - 0x10000;
    uint16Pair.highSurrogate = inputChar >> 10; // leave the top 10 bits
    uint16Pair.highSurrogate += 0xD800;
    uint16Pair.lowSurrogate = inputChar & 0x3FF; // leave the low 10 bits
    uint16Pair.lowSurrogate += 0xDC00;
    return uint16Pair;
}

uint32_t utf16PairToUtf32CodePoint(uint16Pair_t uint16Pair)
{
    uint16Pair.highSurrogate -= 0xD800;
    uint16Pair.lowSurrogate -= 0xDC00;
    uint32_t c2 = (uint32_t(uint16Pair.highSurrogate) << 10) + uint16Pair.lowSurrogate + 0x10000;
    return c2;
}

string icu_Utf16PairToUtf8(uint16Pair_t uint16Pair)
{
    Utf16String utf16String;
    utf16String.uint16Array[0] = uint16Pair.highSurrogate;
    utf16String.uint16Array[1] = uint16Pair.lowSurrogate;
    utf16String.lengthInBytes = 4;
    return icu_Utf16LEtoUtf8(utf16String);
}

string icu_Utf16CharToUtf8(uint16_t uint16)
{
    Utf16String utf16String;
    utf16String.uint16Array[0] = uint16;
    utf16String.lengthInBytes = 2;
    return icu_Utf16LEtoUtf8(utf16String);
}


uint32_t icu_utf32CodePointLower(uint32_t utf32CodePoint)
{
    UnicodeString ustr = icu_Utf32ToUnicodeString(Utf32String(utf32CodePoint));
    UnicodeString ustrLower = ustr.toLower();
    Utf32String utf32StringLower = icu_UnicodeStringToUtf32(ustrLower);
    return utf32StringLower.uint32Array[utf32StringLower.lengthInBytes/4-1];
}

string icu_utf8ToUpper(const string& s)
{
    UnicodeString ustr = icu_Utf8ToUnicodeString(s);
    UnicodeString ustrLower = ustr.toUpper();
    return icu_UnicodeStringToUtf8(ustrLower);
}
