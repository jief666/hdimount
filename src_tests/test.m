//#import <Foundation/Foundation.h>
//#import <Cocoa/Cocoa.h>

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

#include "../src/Utils.h"
#include "../src_tests_diskimages/io.h"
#include "../src/utf8proc/utf8proc.h"
#include "icu_Convertion.h"
#include "ns_Convertion.h"

#include "../src/conversion/fast_unicode_compare_apple.h"
#include "../src/conversion/utf816Conversion.h"

#include "../src/utf8proc/utf8proc.h"


typedef uint32_t table32_t[256];
typedef uint16_t table16_t[256];

#include "utf16ApfsTables.cpp" // include source to have sizeof(gLowerCaseTable)

#ifdef UTF_TABLE_APFS
table16_t& gLatinCaseFold = gLatinCaseFoldApfs;
uint16_t gLowerCaseTableIndexSize = gLowerCaseTableApfsIndexSize;
uint16_t* gLowerCaseTable = gLowerCaseTableApfs;
#elif defined(UTF_TABLE_HFS)
#else
uint16_t gLatinCaseFold[256] = { 0 };
uint16_t gLowerCaseTableIndexSize = 0;
uint16_t gLowerCaseTable[1] = { 0 };
#endif


uint32_t apple_tolower(uint32_t c)
{
    if ( c >= 0x10000 ) return c;
    if (c < 0x0100) {
        return gLatinCaseFold[c];
    }
    uint16_t temp;
    if ((temp = gLowerCaseTable[c>>8]) != 0)
        return gLowerCaseTable[temp + (c & 0x00FF)];
    return c;
}



using icu::UnicodeString;
using namespace std;

uint16_t be(uint16_t u16)
{
    return (u16<<8) + (u16>>8);
}



int test_Utf8_to_Utf16_for_codepoint(uint32_t codePoint)
{
    Utf32String utf32String = codePoint;
    string src = icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(utf32String));

    Utf16String utf16StringExpectedResult = icu_Utf8toUtf16BE(src);

    Utf16String utf16String;
    int error;
    utf16String.lengthInBytes = 2*utf8_to_utf16LE(utf16String.uint16Array, 255, src.c_str(), src.length(), 0, &error);
    if ( error > 0 ) {
        printf("test_Utf8_to_Utf16_for_codepoint error1, code point %x\n", codePoint);
        return 1;
    }

    if ( utf16StringExpectedResult.lengthInBytes != utf16String.lengthInBytes ) {
        printf("test_Utf8_to_Utf16_for_codepoint error2, code point %x\n", codePoint);
        return 2;
    }
    if ( memcmp(utf16StringExpectedResult.uint8Array, utf16String.uint8Array, utf16StringExpectedResult.lengthInBytes) != 0 ) {
        printf("test_Utf8_to_Utf16_for_codepoint error3, code point %x\n", codePoint);
        return 3;
    }
    return 0;
}

int test_Utf16_to_Utf8_for_codepoint(uint32_t codePoint)
{
    Utf32String utf32String = codePoint;
    string ExpectedResult = icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(utf32String));
const char* sdata = ExpectedResult.data();

    Utf16String src = icu_Utf8toUtf16BE(ExpectedResult);


    uint8Array_t uint8Array;
    int error;
    size_t l = utf16LE_to_utf8((char*)uint8Array, sizeof(uint8Array), src.uint16Array, src.lengthInBytes/2, &error);
    if ( error > 0 ) {
        printf("test_Utf16_to_Utf8_for_codepoint error1, code point %x\n", codePoint);
        return 1;
    }

    if ( ExpectedResult.length() != l ) {
        printf("test_Utf16_to_Utf8_for_codepoint error2, code point %x\n", codePoint);
        return 2;
    }
    if ( memcmp(ExpectedResult.data(), uint8Array, l) != 0 ) {
        printf("test_Utf16_to_Utf8_for_codepoint error3, code point %x\n", codePoint);
        return 3;
    }
    return 0;
}


typedef u_int16_t             UniChar;
typedef const UniChar *        ConstUniCharArrayPtr;


int test_case_insensitive_compare_for_codepoint(uint32_t codePoint1, uint32_t codePoint2)
{
    Utf32String utf32String1 = codePoint1;
    Utf32String utf32String2 = codePoint2;
//    string srcUtf8 = icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(utf32String1));
//const char* srcUtf8data = srcUtf8.data();

    Utf16String src1 = icu_Utf8toUtf16BE(icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(utf32String1)));
    Utf16String src2 = icu_Utf8toUtf16BE(icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(utf32String2)));

    UnicodeString ustr1 = icu_Utf16BEToUnicodeString(src1);
    UnicodeString ustr2 = icu_Utf16BEToUnicodeString(src2);
    
    int icuResult = ustr1.caseCompare(ustr2, 0);
    int appleResult = FastUnicodeCompare(src1.uint16Array, src1.lengthInBytes/2, src2.uint16Array, src2.lengthInBytes/2);

    if ( icuResult != appleResult ) {
        printf("test_case_insensitive_compare_for_codepoint error, codepoint %x - codepoint %x, got %d, want %d\n", codePoint1, codePoint2, appleResult, icuResult);
        return 1;
    }
    return 0;
}

int test_Utf8_to_Utf16LE(const string& s)
{
    Utf16String utf16String;
    utf16String.lengthInBytes = utf8_to_utf16LE(utf16String.uint16Array, sizeof(utf16String.uint16Array), s.data(), s.length(), 0, NULL)*2;
    Utf16String expectedResult = icu_Utf8toUtf16LE(s);

    if ( utf16String.lengthInBytes != expectedResult.lengthInBytes) {
        printf("test_Utf8_to_Utf16LE, len is %d and should be %d\n", utf16String.lengthInBytes, expectedResult.lengthInBytes);
        return 1;
    }

    for (uint32_t i = 0 ; i < expectedResult.lengthInBytes/2 ; i++) {
        if ( utf16String.uint16Array[i] != expectedResult.uint16Array[i] ) {
            printf("test_Utf8_to_Utf16LE, difference at index %d. String is '%s' and should be '%s'\n", i, icu_Utf16LEtoUtf8(utf16String).c_str(), s.c_str());
            return 1;
        }
    }
    printf("test_Utf8_to_Utf16LE ok : '%s'\n", icu_Utf16LEtoUtf8(utf16String).c_str());
    return 0;
}

int test_Utf8_to_Utf16BE(const string& s)
{
    HFSString hfsString;
    utf8_to_utf16BE(s, &hfsString);
    Utf16String expectedResult = icu_Utf8toUtf16BE(s);

    if ( be(hfsString.length) != expectedResult.lengthInBytes/2) {
        printf("test_Utf8_to_Utf16BE, len is %d and should be %d\n", be(hfsString.length), expectedResult.lengthInBytes/2);
        return 1;
    }

    for (uint32_t i = 0 ; i < expectedResult.lengthInBytes/2 ; i++) {
        if ( hfsString.string[i] != expectedResult.uint16Array[i] ) {
            printf("test_Utf8_to_Utf16BE, difference at index %d. String is '%s' and should be '%s'\n", i, icu_Utf16BEtoUtf8(hfsString).c_str(), s.c_str());
            return 1;
        }
    }
    printf("test_Utf8_to_Utf16BE ok : '%s'\n", icu_Utf16BEtoUtf8(hfsString).c_str());
    return 0;
}

int test_Utf16LE_to_Utf8(const string& expectedResult)
{
    Utf16String src = icu_Utf8toUtf16LE(expectedResult);

    char buf[expectedResult.length()+1];
    size_t len = utf16LE_to_utf8(buf, expectedResult.length()+1, src.uint16Array, src.lengthInBytes/2, NULL);
    string result(buf, len);

    if ( result.length() != expectedResult.length()) {
        printf("test_Utf16LE_to_Utf8, len is %zd and should be %zd\n", result.length(), expectedResult.length());
        return 1;
    }

    for (uint32_t i = 0 ; i < expectedResult.length() ; i++) {
        if ( result[i] != expectedResult[i] ) {
            printf("test_Utf16LE_to_Utf8, difference at index %d. String is '%s' and should be '%s'\n", i, result.c_str(), expectedResult.c_str());
            return 1;
        }
    }
    printf("test_Utf16LE_to_Utf8 ok : '%s'\n", expectedResult.c_str());
    return 0;
}

int main(int argc, char **argv)
{

    {
        UErrorCode error = U_ZERO_ERROR;
        g_utf16be = ucnv_open("UTF-16BE", &error);
        assert(U_SUCCESS(error));
        g_utf16le = ucnv_open("UTF-16LE", &error);
        assert(U_SUCCESS(error));
        g_utf8 = ucnv_open("UTF-8", &error);
        assert(U_SUCCESS(error));
        g_utf32 = ucnv_open("UTF-32", &error);
        assert(U_SUCCESS(error));
    }

    {
        UnicodeString ustr("bonjour");
        printf("icu_UnicodeStringToUtf8  : %s\n", icu_UnicodeStringToUtf8(ustr).c_str());
        printf("Utf8 -> UnicodeString -> Utf8(\"coucou\"))=%s\n", icu_UnicodeStringToUtf8(icu_Utf8ToUnicodeString("coucou")).c_str());

        printf("Utf8 -> UnicodeString -> Utf16LE -> UnicodeString -> Utf8(\"coucou\"))=%s\n", icu_UnicodeStringToUtf8(icu_Utf16LEToUnicodeString(icu_UnicodeStringToUtf16LE(icu_Utf8ToUnicodeString("coucou")))).c_str());
        printf("Utf8 -> UnicodeString -> Utf32 -> UnicodeString -> Utf8(\"coucou\"))=%s\n", icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(icu_UnicodeStringToUtf32(icu_Utf8ToUnicodeString("coucou")))).c_str());
    }
    {
        Utf16String utf16String = icu_UnicodeStringToUtf16BE(icu_Utf8ToUnicodeString("bonjour2"));
        printf("%s\n", icu_UnicodeStringToUtf8(icu_Utf16BEToUnicodeString(utf16String)).c_str());
    }
    test_Utf8_to_Utf16BE("bonjour3");
    test_Utf8_to_Utf16LE("bonjour4");
    test_Utf16LE_to_Utf8("bonjour5");
    test_Utf8_to_Utf16BE("ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞĀĂĄĆĈĊČĎĐĒĔĖĘĚĜĞĠĢĤĦĨĪĬĮĲĴĶĹĻĽĿŁŃŅŇŊŌŎŐŒŔŖŘŚŜŞŠŢŤŦŨŪŬŮŰŲŴŶŸŹŻŽƁƂƄƆƇƉƊƋƎƏƐƑƓƔƖƗƘƜƝƟƠƢƤƧƩƬƮƯƱƲƳƵƷƸƼǄǅǇǈǊǋǍǏǑǓǕǗ");
//    test_Utf8_to_Utf16_for_codepoint(0xf8ff);
//    test_Utf16_to_Utf8_for_codepoint(0xF8FF);

    {
        NSString* nsString = ns_utf8ToNSString(icu_Utf32CodePointToUtf8(0x386));
        NSString* nsString2 = ns_utf32CodePointToNSString(0x386);
        string s = icu_Utf32CodePointToUtf8(0x386);
        if ( strcmp(s.c_str(), nsString.UTF8String) != 0 ) {
            printf("NSString porblem\n");
        }

    }

//    
//    test_case_insensitive_compare_for_codepoint(0x41, 0x41);
//    test_case_insensitive_compare_for_codepoint(0x41, 0x42);
//    test_case_insensitive_compare_for_codepoint(0x100, 0x101);
//    test_case_insensitive_compare_for_codepoint('a', 'A');
//    test_case_insensitive_compare_for_codepoint(0x6bc, 0x23a);

//    {
//        for ( uint32_t utf32CodePoint = 1 ; utf32CodePoint < 0xFFFF ; utf32CodePoint ++ ) {
//            string s = icu_Utf32CodePointToUtf8(utf32CodePoint);
//            uint8_t* res = utf8proc_NFD((const uint8_t*)s.data());
//            string sres((char*)res);
//            if ( s != sres ) {
//                printf("Normalization need for code point : %u(%s), len=%zd : normalized len=%zd '%s'\n", utf32CodePoint, s.c_str(), s.length(), sres.length(), sres.c_str());
//            }
//        }
//    }


    {
        string s1 =  "ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞĀĂĄĆĈĊČĎĐĒĔĖĘĚĜĞĠĢĤĦĨĪĬĮĲĴĶĹĻĽĿŁŃŅŇŊŌŎŐŒŔŖŘŚŜŞŠŢŤŦŨŪŬŮŰŲŴŶŸŹŻŽƁƂƄƆƇƉƊƋƎƏƐƑƓƔƖƗƘƜƝƟƠƢƤƧƩƬƮƯƱƲƳƵƷƸƼǄǅǇǈǊǋǍǏǑǓǕǗ";
        string s2 =  "abcdefghijklmnopqrstuvwxyzàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþāăąćĉċčďđēĕėęěĝğġģĥħĩīĭįĳĵķĺļľŀłńņňŋōŏőœŕŗřśŝşšţťŧũūŭůűųŵŷÿźżžɓƃƅɔƈɖɗƌǝəɛƒɠɣɩɨƙɯɲɵơƣƥƨʃƭʈưʊʋƴƶʒƹƽǆǆǉǉǌǌǎǐǒǔǖǘ";
        uint8_t* res = utf8proc_NFD((const uint8_t*)s1.data());
        string sres((char*)res);
        HFSString hfsString1 = icu_Utf8toUtf16BE(sres);
        HFSString hfsString2 = icu_Utf8toUtf16BE(s2);
        printf("res len %zu, hfs len %d\n", strlen((char*)res), be(hfsString1.length));
        if ( FastUnicodeCompare(hfsString1, hfsString2) != 0 ) {
            printf("bug in FastUnicodeCompare\n");
            FastUnicodeCompare(hfsString1, hfsString2);
        }
    }


printf("\n");
printf("\n");
printf("\n");
printf("\n");
    string fs = "Apfs";
    string testFolder = "/Volumes/" + fs + "/unicode_longfilename_test/";

    //---------------------------  Create files with maximumn name length, only with lower case letter
    {
        printf("Letter that has an upper case :\n");
        int count = 0;
        string nameUpper;
        string previousnameUpper;
        string nameLower;
        string previousnameLower;
        for (uint32_t utf32CodePoint = 1 ; utf32CodePoint <= 0xFFFF ; utf32CodePoint++) // 0x10FFFD
        {
            if ( apple_tolower(utf32CodePoint) == 0 ) {
                printf("apple_tolower(utf32CodePoint) == 0 !!!\n");
                continue;
            }
            if ( apple_tolower(utf32CodePoint) != utf32CodePoint ) {
//                printf("%s", icu_Utf32CodePointToUtf8(apple_tolower(utf32CodePoint)).c_str());
                previousnameLower = nameLower;
                previousnameUpper = nameUpper;
                nameLower = nameLower + icu_Utf32CodePointToUtf8(apple_tolower(utf32CodePoint));
                nameUpper = nameUpper + icu_Utf32CodePointToUtf8(utf32CodePoint);
                try {
                    write_at_path_T(testFolder+nameLower, (nameLower+"\n").c_str(), "Can't write");
                    if ( !file_or_dir_exists(testFolder+nameUpper) ) {
                        printf("Upper case file doesn't exist '%s'\n", nameUpper.c_str());
                    }
                    unlink((testFolder+previousnameLower).c_str());
                }catch(...){
                    printf("File generated '%s'\n", previousnameLower.c_str());
                    printf("Upper case counter part '%s'\n", previousnameUpper.c_str());
                    
                    if ( fs == "Hfs" )
                    {
                        HFSString hfsString1 = icu_Utf8toUtf16BE(previousnameLower);
                        HFSString hfsString2 = icu_Utf8toUtf16BE(previousnameUpper);
                        if ( FastUnicodeCompare(hfsString1, hfsString2) != 0 ) {
                            printf("bug in FastUnicodeCompare\n");
                            FastUnicodeCompare(hfsString1, hfsString2);
                        }
                    }

                    
                    nameLower = icu_Utf32CodePointToUtf8(apple_tolower(utf32CodePoint));
                    nameUpper = icu_Utf32CodePointToUtf8(utf32CodePoint);
                    printf("\n");
                }
//                if ( nameUpper.length() >= 100 ) {
//                    printf("File generated '%s'\n", previousnameLower.c_str());
//                    nameLower = icu_Utf32CodePointToUtf8(apple_tolower(utf32CodePoint));
//                    nameUpper = icu_Utf32CodePointToUtf8(utf32CodePoint);
//                }
                count++;
            }
        }
        printf("\n");
        printf("Count=%d\n", count);
    }

return 0;
    // 0x0000 - 0xFFFF translate to 2 bytes (one UTF16 char).
    // 0x10000 - 0x10FFFF translate to 4 bytes
    
    // Check that utf32CodePoint == codePointUtf16 when < 0xFFFF (except 0xD800-0xDFFF)
    for (uint32_t utf32CodePoint = 0 ; utf32CodePoint <= 0xFFFF ; utf32CodePoint++) // 0x10FFFD
    {
        if ( utf32CodePoint >= 0xD800 && utf32CodePoint <= 0xDFFF ) continue;
        Utf16String utf16String = icu_UnicodeStringToUtf16LE(icu_Utf32ToUnicodeString(Utf32String(utf32CodePoint)));
        if ( utf16String.lengthInBytes != 2 ) {
            printf("code %x length != 2\n", utf32CodePoint);
            continue;
        }
        uint16_t codePointUtf16 = utf16String.uint16Array[0];
        if ( codePointUtf16 != utf32CodePoint ) {
            printf("codePointUtf16 != utf32CodePoint for code %x\n", utf32CodePoint);
        }
    }

    // Check that utf16String.lengthInBytes == 4 for range 0x10000-0x10FFFF
    // Check that Utf32CodePointToUtf16Pair and utf32char is working
    for (uint32_t utf32CodePoint = 0x10000 ; utf32CodePoint <= 0x10FFFF ; utf32CodePoint++)
    {
        Utf16String utf16String = icu_UnicodeStringToUtf16LE(icu_Utf32ToUnicodeString(Utf32String(utf32CodePoint)));
        if ( utf16String.lengthInBytes != 4 ) {
            printf("code %x length != 4\n", utf32CodePoint);
            continue;
        }
        uint16Pair_t uint16Pair = Utf32CodePointToUtf16Pair(utf32CodePoint);
        if ( utf16PairToUtf32CodePoint(uint16Pair) != utf32CodePoint ) {
            printf("utf32char(Utf32CodePointToUtf16Pair(utf32CodePoint)) != utf32CodePoint fo code %x\n", utf32CodePoint);
            continue;
        }
        if ( uint16Pair.highSurrogate != utf16String.uint16Array[0] ) {
            printf("uint16Pair.highSurrogate != for code %x\n", utf32CodePoint);
            continue;
        }
        if ( uint16Pair.lowSurrogate != utf16String.uint16Array[1] ) {
            printf("uint16Pair.highSurrogate != for code %x\n", utf32CodePoint);
            continue;
        }
    }
    
    // Check NSString conversion when < 0xFFFF (except 0xD800-0xDFFF)
    for (uint32_t utf32CodePoint = 0xFF ; utf32CodePoint <= 0x10FFFF ; utf32CodePoint++) // 0x10FFFD
    {
        if ( utf32CodePoint >= 0xD800 && utf32CodePoint <= 0xDFFF ) continue;
        if ( utf32CodePoint == 0xFEFF ) continue;

        Utf16String utf16String = icu_UnicodeStringToUtf16LE(icu_Utf32ToUnicodeString(Utf32String(utf32CodePoint)));
        string utf8String = icu_Utf32CodePointToUtf8(utf32CodePoint);
        const char* utf8StringData = utf8String.data();
        size_t utf8StringSize = utf8String.size();
        NSString* nsString = [NSString stringWithUTF8String:icu_Utf32CodePointToUtf8(utf32CodePoint).c_str()];
        if ( utf16String.lengthInBytes/2 != nsString.length ) {
            printf("utf16String.lengthInBytes != nsString.length for code point %x\n", utf32CodePoint);
            continue;
        }
        if ( utf16String.uint16Array[0] != [nsString characterAtIndex:0] ) {
            printf("utf16String.uint16Array[0] != [nsString characterAtIndex:0] for code point %x\n", utf32CodePoint);
            continue;
        }
        if ( nsString.length > 1  &&  utf16String.uint16Array[1] != [nsString characterAtIndex:1] ) {
            printf("utf16String.uint16Array[1] != [nsString characterAtIndex:1] for code point %x\n", utf32CodePoint);
            continue;
        }
    }


    // 0xD800 - 0xDFFF -> in UTF16 0xFFFD
    
    // =================================================  File system check for code point < 0x10000

//    write_at_path_T(folder+icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String('a'))), "bonjour", 7, "Can't write");
//    append_at_path_T(folderHfs+icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String(0x1c90))), "bonjour", "Can't write");
//    append_at_path_T(folderHfs+icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String(0x10d0))), "bonjour", "Can't write");
//    append_at_path_T(folderHfs+icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String(0x13F0))), "bonjour", "Can't write");
//    append_at_path_T(folderHfs+icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String(utf16_tolower(0x13F0)))), "bonjour", "Can't write");
//

}

