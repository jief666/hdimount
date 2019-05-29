/* ICU4C */
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/ucnv.h>
#include <unicode/unorm2.h>
#include <unicode/unistr.h>
#include <unicode/ucnv.h>
#include <unicode/errorcode.h>

#include "../src/darling-dmg/src/hfsplus.h" // for HFSString

extern UConverter* g_utf32;
extern UConverter* g_utf16be;
extern UConverter* g_utf16le;
extern UConverter* g_utf8;


typedef uint8_t uint8Array_t[510];
typedef uint16_t uint16Array_t[255];
typedef uint32_t uint32Array_t[127];

typedef u_int16_t             UniChar;
typedef const UniChar *        ConstUniCharArrayPtr;

typedef struct uint16Pair_st {
    uint16_t highSurrogate;
    uint16_t lowSurrogate;
} uint16Pair_t;

class Utf16String
{
  public:
    uint16_t lengthInBytes;
    union {
        uint16Array_t uint16Array;
        uint8Array_t uint8Array;
    };
    Utf16String() {
        lengthInBytes = 0;
        memset(uint8Array, 0, sizeof(uint8Array));
    }
    Utf16String(const HFSString& hfsString) {
        lengthInBytes = ((hfsString.length<<8) + (hfsString.length>>8))*2;
        memset(uint8Array, 0, sizeof(uint8Array));
        memcpy(uint16Array, hfsString.string, lengthInBytes);
    }
    operator const HFSString()
    {
        HFSString hfsString;
        hfsString.length = ((lengthInBytes/2) << 8) + ((lengthInBytes/2) >> 8);
        memcpy(hfsString.string, uint16Array,lengthInBytes);
        return hfsString;
    }
};

class Utf32String
{
  public:
    uint32_t lengthInBytes;
    union {
        uint32Array_t uint32Array;
        uint8Array_t uint8Array;
    };
    Utf32String() {
        lengthInBytes = 0;
        memset(uint8Array, 0, sizeof(uint8Array));
    }
    Utf32String(const uint32_t uint32) {
        memset(uint8Array, 0, sizeof(uint8Array));
        uint32Array[0] = 0xFEFF;
        uint32Array[1] = uint32;
        lengthInBytes = 8;
    }
};

icu::UnicodeString icu_Utf8ToUnicodeString(const std::string& str2);
icu::UnicodeString icu_Utf16BEToUnicodeString(const Utf16String& str2);
icu::UnicodeString icu_Utf16LEToUnicodeString(const Utf16String& str2);
icu::UnicodeString icu_Utf32ToUnicodeString(const Utf32String& str2);
std::string icu_UnicodeStringToUtf8(const icu::UnicodeString& ustr);
Utf16String icu_UnicodeStringToUtf16BE(const icu::UnicodeString& ustr);
Utf16String icu_UnicodeStringToUtf16LE(const icu::UnicodeString& ustr);
Utf32String icu_UnicodeStringToUtf32(const icu::UnicodeString& ustr);
std::string icu_Utf16BEtoUtf8(const Utf16String& str2);
Utf16String icu_Utf8toUtf16BE(const std::string& str2);
Utf16String icu_Utf8toUtf16LE(const std::string& str2);
std::string icu_Utf16LEtoUtf8(const Utf16String& str2);
std::string icu_Utf32CodePointToUtf8(uint32_t utf32CodePoint);
uint16Pair_t Utf32CodePointToUtf16Pair(uint32_t utf32CodePoint);
uint32_t utf16PairToUtf32CodePoint(uint16Pair_t uint16Pair);
std::string icu_Utf16PairToUtf8(uint16Pair_t uint16Pair);
std::string icu_Utf16CharToUtf8(uint16_t uint16);
uint32_t icu_utf32CodePointLower(uint32_t utf32CodePoint);
std::string icu_utf8ToUpper(const std::string& s);
