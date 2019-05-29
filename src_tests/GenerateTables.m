
#include <stdio.h>
#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <math.h>
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
#include <vector>
#include <map>

#include "../src/Utils.h"
#include "../src_tests_diskimages/io.h"
#include "../src/utf8proc/utf8proc.h"
#include "icu_Convertion.h"
#include "ns_Convertion.h"

using icu::UnicodeString;
using namespace std;

typedef uint32_t table32_t[256];
typedef uint16_t table16_t[256];

uint16_t be(uint16_t u16)
{
    return (u16<<8) + (u16>>8);
}


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



// lower 16 bits of utf32CodePoint are ignored
bool tableBijective(uint32_t utf32CodePoint, const table32_t* table)
{
    for (uint32_t ii = 0 ; ii < 256 ; ii++) {
        if ( ((*table)[ii] >> 8) != (utf32CodePoint>>8) ) return false;
        if ( ((*table)[ii] & 0xFF) != ii ) return false;
    }
    return true;
}

// lower 16 bits of utf32CodePoint are ignored
table16_t* getTableFromGLowerCaseTableForCodePoint(uint32_t utf32CodePoint)
{
    if ( utf32CodePoint <= 0xff ) return &gLatinCaseFold;
    if ( utf32CodePoint >> 8 >= gLowerCaseTableIndexSize ) {
        return NULL;
    }
    if ( gLowerCaseTable[utf32CodePoint >> 8] == 0 ) return NULL;
    return (table16_t*)&gLowerCaseTable[gLowerCaseTable[utf32CodePoint >> 8]];
}


bool tablesAreEqual(const uint16_t* table1, const uint32_t* table2, uint32_t len)
{
    bool identical = true;
    for (uint32_t ii = 0 ; ii < len ; ii++) {
        if ( table1[ii] != table2[ii] ) {
printf("Difference : \"embedded table\"[%2x]=%x(%s)(icuLower %x(%s))   vs   table[%2x]=%x(%s)(icuLower %x(%s))\n",
       ii, table1[ii],     icu_Utf32CodePointToUtf8(table1[ii]).c_str(),     icu_utf32CodePointLower(table1[ii]),     icu_Utf32CodePointToUtf8(icu_utf32CodePointLower(table1[ii])).c_str(),
       ii, table2[ii], icu_Utf32CodePointToUtf8(table2[ii]).c_str(), icu_utf32CodePointLower(table2[ii]), icu_Utf32CodePointToUtf8(icu_utf32CodePointLower(table2[ii])).c_str());
            identical = false;
        }
    }
    return identical;
}

// lower 16 bits of utf32CodePoint are ignored
bool tableIsEqualToGLowerCaseTable(uint32_t utf32CodePoint, const table32_t* table)
{
    table16_t* t = getTableFromGLowerCaseTableForCodePoint(utf32CodePoint);
    if ( t == NULL ) return tableBijective(utf32CodePoint, table);
    
    bool identical = true;
    for (uint32_t ii = 0 ; ii < 256 ; ii++) {
        if ( (*table)[ii] != (*t)[ii] ) {
printf("Difference : \"embedded table\"[%2x]=%x(%s)(icuLower %x(%s))   vs   table[%2x]=%x(%s)(icuLower %x(%s))\n",
       ii, (*t)[ii],     icu_Utf32CodePointToUtf8((*t)[ii]).c_str(),     icu_utf32CodePointLower((*t)[ii]),     icu_Utf32CodePointToUtf8(icu_utf32CodePointLower((*t)[ii])).c_str(),
       ii, (*table)[ii], icu_Utf32CodePointToUtf8((*table)[ii]).c_str(), icu_utf32CodePointLower((*table)[ii]), icu_Utf32CodePointToUtf8(icu_utf32CodePointLower((*table)[ii])).c_str());
            identical = false;
        }
    }
    return identical;
}


void append_in_table_cpp(string tableFilename, bool print, const string& s)
{
    if ( tableFilename.length() > 0 ) {
        append_at_path_T(tableFilename, s, "Can't append");
    }
    if ( print ) printf("%s", s.c_str());
}

static void printfTable(string tableFilename, bool print, uint32_t* table, uint32_t l)
{
    for (uint32_t tt = 0 ; tt < l ; ) {
        append_in_table_cpp(tableFilename, print, stringPrintf("    /* %*X */    ", int(log(l/16-1)/log(16)), tt/16));
        for (uint32_t tt2 = tt ; tt2 < l  &&  tt2 < tt+16 ; tt2++) {
            append_in_table_cpp(tableFilename, print, stringPrintf("0x%04X, ", table[tt2]));
        }
        append_in_table_cpp(tableFilename, print, "\n");
        tt += 16;
    }
}

static void printfTable(string tableFilename, bool print, table32_t* table)
{
    printfTable(tableFilename, print, &((*table)[0]), 256);
}

static void fileSystemTestCodePoint(const std::string &folderHfs, table32_t* table, uint32_t utf32CodePoint, bool trace)
{
    NSString* s1 = ns_utf32CodePointToNSString(utf32CodePoint);
    NSString *s1Lower = [s1 lowercaseString];
    
    uint32_t icuLower = icu_utf32CodePointLower(utf32CodePoint);
    
    uint32_t cLowerFromFile = utf32CodePoint;
    string path = folderHfs + "Test " + icu_Utf32CodePointToUtf8(utf32CodePoint);
    try {
        string content = stringPrintf("%x", utf32CodePoint);
        // Don't put the codepoint number in the file name. The only difference has to be the letter being tested.
        
        if ( trace )
            printf("code point %x(%s), icuLower %x(%s), utf16_tolower=%x(%s), NSString %x(%s)\n",
                   utf32CodePoint, icu_Utf32CodePointToUtf8(utf32CodePoint).c_str(),
                   icuLower, icu_Utf32CodePointToUtf8(icuLower).c_str(),
                   apple_tolower(utf32CodePoint), icu_Utf16CharToUtf8(apple_tolower(utf32CodePoint)).c_str(),
                   s1Lower.length>0 ? [s1Lower characterAtIndex:0] : 0, s1.UTF8String);
        
        string contentRead = read_all_at_path_T(path, "Can't read");
        cLowerFromFile = std::stoull(contentRead, NULL, 16);
    }
    catch(const string& s) {
        printf("Code point %x(%s), file '%s' : %s\n", utf32CodePoint, icu_Utf32CodePointToUtf8(utf32CodePoint).c_str(), path.c_str(), s.c_str());
        // File can't be read, which means it wasn't written. It must have been an illegal byte sequence. NOTE : that doesn't happen with HFS, only APFS (High Sierra May 2019)
        table[utf32CodePoint >> 8][utf32CodePoint & 0xFF] = 0;
//        if ( s.find("errno(92)") != std::string::npos ) {
//        }
        return;
    }
    catch(...) {
        printf("Code point %x(%s), file '%s' : unknow error\n", utf32CodePoint, icu_Utf32CodePointToUtf8(utf32CodePoint).c_str(), path.c_str());
        return;
    }
    if ( utf32CodePoint == cLowerFromFile ) {
        // Table is already initialized so table[x >> 8][x & 0xFF] = x. Nothing to do.
        return;
    }
    table[utf32CodePoint >> 8][utf32CodePoint & 0xFF] = cLowerFromFile;
}

static void createFileForUtf32CodePoint(const std::string &folderHfs, uint32_t utf32CodePoint)
{
    try {
        string path = folderHfs + "Test " + icu_Utf32CodePointToUtf8(utf32CodePoint);
        if ( file_or_dir_exists(path) ) {
            string contentRead = read_all_at_path_T(path, "Can't read");
            uint32_t codePointRead = std::stoul(contentRead, NULL, 16);
            if ( codePointRead == utf32CodePoint ) return;
            if ( icu_utf32CodePointLower(codePointRead) == utf32CodePoint ) {
                write_at_path_T(path, stringPrintf("%x\n", utf32CodePoint), "Can't write");
                append_at_path_T(path, stringPrintf("%x\n", codePointRead), "Can't write");
            }else{
                append_at_path_T(path, stringPrintf("%x\n", utf32CodePoint), "Can't write");
            }
        }else{
            write_at_path_T(path, stringPrintf("%x\n", utf32CodePoint), "Can't write");
        }
    }
    catch(const string& s) {
//        printf("code %x(%s) can't be written : %s\n", utf32CodePoint, icu_Utf32CodePointToUtf8(utf32CodePoint).c_str(), s.c_str());
    }
    catch(...) {
//        printf("code %x can't be written\n", utf32CodePoint);
    }
}

static void generateTables(const std::string &fs, table32_t *table32, int nb_total_tables, uint32_t utf32CodePointStart, uint32_t utf32CodePointEnd)
{
    if ( utf32CodePointStart < 0xFFFF) {
        if (utf32CodePointEnd > 0xFFFF) {
            generateTables(fs, table32, nb_total_tables, utf32CodePointStart, 0xFFFF);
            generateTables(fs, table32, nb_total_tables, 0x10000, utf32CodePointEnd);
        }
    }
    bool tables32bits = utf32CodePointStart >= 0x10000;
    string uintType = tables32bits ? "uint32_t" : "uint16_t";
    
    int tableNotIdentical = 0;
    bool onlyPrintModifiedTables = true;
    if ( !onlyPrintModifiedTables ) {
        printf("Tables printed even if there are correct\n");
    }
    string tableFilename = "utf16"+fs+"TablesNew.cpp";
    //        tableFilename = ""; // Comment out to generate tables
    if ( utf32CodePointStart > 0  ||  utf32CodePointEnd < 0xFFFF )  tableFilename = ""; // Cannot generate cpp if not all codepoint were checked
    if ( tableFilename.length()>0 ) {
        write_at_path_T(tableFilename, "#include <inttypes.h>\n", "Can't write");
        append_at_path_T(tableFilename, stringPrintf("#define UTF_TABLE_%s\n", icu_utf8ToUpper(fs).c_str()), "Can't write");
    }else{
        printf("NOTE : tableFilename is empty, cpp not generated\n");
    }
    
    // gLatinCaseFold
    if ( utf32CodePointStart == 0 )
    {
        bool tableModified = !tableIsEqualToGLowerCaseTable(0, &table32[0]);
        bool printThisTable = tableModified  ||  !onlyPrintModifiedTables;
        tableNotIdentical += tableModified;
        append_in_table_cpp(tableFilename, printThisTable, "/*\n");
        append_in_table_cpp(tableFilename, printThisTable, " * For better performance, the case folding table for basic latin\n");
        append_in_table_cpp(tableFilename, printThisTable, " * is seperate from the others.  This eliminates the extra lookup\n");
        append_in_table_cpp(tableFilename, printThisTable, " * to get the offset to this table.\n");
        append_in_table_cpp(tableFilename, printThisTable, " *\n");
        append_in_table_cpp(tableFilename, printThisTable, " * Note: 0x0000 now maps to 0 so that it will be ignored\n");
        append_in_table_cpp(tableFilename, printThisTable, " */\n");
        append_in_table_cpp(tableFilename, printThisTable, "\n");
        append_in_table_cpp(tableFilename, printThisTable, stringPrintf("%s gLatinCaseFold%s%s[] = {\n", uintType.c_str(), fs.c_str(), tables32bits ? "_32" : ""));
        append_in_table_cpp(tableFilename, printThisTable, "\n");
        printfTable(tableFilename, printThisTable, &(table32[0][0]), 256);
        append_in_table_cpp(tableFilename, printThisTable, "\n");
        append_in_table_cpp(tableFilename, printThisTable, "};\n");
    }
    // Table index
    bool tableIndexPrinted = false;
    if ( utf32CodePointStart == 0  &&  utf32CodePointEnd == 0xFFFF )
    {
        int table_count = 1;
        uint32_t last_index = 0;
        uint32_t table0[nb_total_tables];
        for (uint32_t tn = 0 ; tn < 256 ; tn++) {
            if ( tn != 0  &&  !tableBijective(tn*256, &table32[tn]) ) {
                table0[tn] = (table_count++)*256;
                last_index = tn;
            }
        }
        bool tableModified = gLowerCaseTableIndexSize != last_index+1  ||  !tablesAreEqual(gLowerCaseTable, table0, gLowerCaseTableIndexSize);
        tableIndexPrinted = tableModified  ||  !onlyPrintModifiedTables;
        tableNotIdentical += tableModified;
        append_in_table_cpp(tableFilename, tableIndexPrinted, "\n");
        append_in_table_cpp(tableFilename, tableIndexPrinted, "/*    The lower case table consists of a 256-entry high-byte table followed by some number of\n");
        append_in_table_cpp(tableFilename, tableIndexPrinted, "      256-entry subtables. The high-byte table contains either an offset to the subtable for\n");
        append_in_table_cpp(tableFilename, tableIndexPrinted, "      characters with that high byte or zero, which means that there are no case mappings or\n");
        append_in_table_cpp(tableFilename, tableIndexPrinted, "      ignored characters in that block. Ignored characters are mapped to zero.\n");
        append_in_table_cpp(tableFilename, tableIndexPrinted, " */\n");
        if ( last_index+1 >= 0x10000) {
            append_in_table_cpp(tableFilename, tableIndexPrinted, stringPrintf("uint32_t gLowerCaseTable%sIndexSize = %d;\n", fs.c_str(), last_index+1));
        }else{
            append_in_table_cpp(tableFilename, tableIndexPrinted, stringPrintf("uint16_t gLowerCaseTable%sIndexSize = %d;\n", fs.c_str(), last_index+1));
        }
        append_in_table_cpp(tableFilename, tableIndexPrinted, stringPrintf("%s gLowerCaseTable%s%s[] = {\n", uintType.c_str(), fs.c_str(), tables32bits ? "_32" : ""));
        append_in_table_cpp(tableFilename, tableIndexPrinted, "\n");
        append_in_table_cpp(tableFilename, tableIndexPrinted, "    /* High-byte indices ( == 0 iff no case mapping and no ignorables ) */\n");
        append_in_table_cpp(tableFilename, tableIndexPrinted, "\n");
        printfTable(tableFilename, tableIndexPrinted, table0, last_index+1);
    }else{
        printf("Index not generated because not all code point were checked\n");
    }
    {
        int table_count = 1;
        for (uint32_t tableIndex = MAX(1, utf32CodePointStart/256) ; tableIndex <= utf32CodePointEnd/256 ; tableIndex++)
        {
            bool tableModified = !tableIsEqualToGLowerCaseTable(tableIndex*256, &table32[tableIndex]);
            bool printThisTable = tableModified  ||  !onlyPrintModifiedTables;
            tableNotIdentical += tableModified;
            if ( !tableBijective(tableIndex*256, &table32[tableIndex]) )
            {
                append_in_table_cpp(tableFilename, printThisTable, "\n");
                append_in_table_cpp(tableFilename, printThisTable, stringPrintf("    /* Table %x (for high byte 0x%06xxx) */\n", table_count, tableIndex));
                append_in_table_cpp(tableFilename, printThisTable, "\n");
                printfTable(tableFilename, printThisTable, &table32[tableIndex]);
                table_count++;
            }else{
                if ( getTableFromGLowerCaseTableForCodePoint(tableIndex*256) != NULL ) {
                    printf("Table for 0x%06xxx should not exist in gLowerCaseTable\n", tableIndex);
                }
            }
        }
    }
    append_in_table_cpp(tableFilename, tableIndexPrinted, "\n");
    append_in_table_cpp(tableFilename, tableIndexPrinted, "};\n");
    if ( tableNotIdentical == 0 ) {
        printf("All tables GOOD\n");
    }
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
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

    string fs = "Hfs";
    string testFolder = "/Volumes/" + fs + "/unicode_name_test/";

	if ( !file_or_dir_exists(testFolder) ) {
		fprintf(stderr, "Be sure tohave mounted a diskimage with the right filesystem\n");
		fprintf(stderr, "Create it with : hdiutil create %s -size 6g -volname \"%s\" -type SPARSE -fs %s -layout NONE\n", fs.c_str(), fs.c_str(), fs.c_str());
		fprintf(stderr, "Then : hdiutil attach %s\n", fs.c_str());
		fprintf(stderr, "Then : mkdir %s\n", testFolder.c_str());
		exit(1);
	}
    // =================================================  Create table for code point >= 0x10000
    {
        const int nb_total_tables = 0x10FFFD/256+1;
        table32_t table32[nb_total_tables];

        for (uint32_t ii = 0 ; ii < nb_total_tables ; ii++) {
            for (uint32_t jj = 0 ; jj < 256 ; jj++) {
                table32[ii][jj] = ii*256+jj;
            }
        }

append_at_path_T(testFolder + "\u03D3", "test 3D3", "");
append_at_path_T(testFolder + "\u03D2\u0301", "test 3D2 301", "");
append_at_path_T(testFolder + "\u038E", "test 38E", "");
append_at_path_T(testFolder + "\u03A5\u0301", "test 3A5 301", "");

append_at_path_T(testFolder + "\u03D4", "test 3D4", "");
append_at_path_T(testFolder + "\u03D2\u0308", "test 3D2 308", "");
append_at_path_T(testFolder + "\u03AB", "test 3AB", "");
append_at_path_T(testFolder + "\u03A5\u0308", "test 3A5 308", "");

append_at_path_T(testFolder + "\u1E9B", "test 1E9B", "");
append_at_path_T(testFolder + "\u017F\u0307", "test 017F 307", "");
append_at_path_T(testFolder + "\u1E61", "test 1E61", "");
append_at_path_T(testFolder + "\u0073\u0307", "test 0073 307", "");
//createFileForUtf32CodePoint(testFolder, 0x03D3);

//createFileForUtf32CodePoint(testFolder, 0x03D2);
//fileSystemTestCodePoint(folderHfs, table32, 0x10c6, false);

        // ---------------------------------------------  Create all the files. Files with the same cas insensitive name will be overwritten
        // Min=0x10000  Max=0x10FFFD -> 0xFFFFD(1'048'573) codepoint
        uint32_t utf32CodePointStart = 0x10000;
        uint32_t utf32CodePointEnd   = 0x10FFFD;
//        utf32CodePointStart = 0x0000;
//        utf32CodePointEnd   = 0xFFFF;
//        utf32CodePointStart = 0x400;
//        utf32CodePointEnd   = 0x4FF;

        utf32CodePointStart &= ~0xFFU; // to be sure we start on a beginning of a table
        utf32CodePointEnd   |= 0xFFU; // to be sure we end at the end of a table

//printf("%s\n", icu_Utf32CodePointToUtf8(0x3ac).c_str());
        if ( !file_or_dir_exists(testFolder + "Test " + icu_Utf32CodePointToUtf8(utf32CodePointStart+1)) ) {
            for (uint32_t utf32CodePoint = utf32CodePointStart ; utf32CodePoint <= utf32CodePointEnd ; utf32CodePoint++) // 0x10FFFD
            {
                if ( utf32CodePoint == 0 ) continue;
                if ( utf32CodePoint == '/' ) continue;
                createFileForUtf32CodePoint(testFolder, utf32CodePoint);
            }
        }else{
            printf("File '%s' already exists. No test file generation\n", (testFolder + "Test " + icu_Utf32CodePointToUtf8(utf32CodePointStart)).c_str());
        }

        // ---------------------------------------------  Create tables

        // dependant tables : 0x00xx 0x01xx 0x02xx 0x03xx 0x1Fxx
        // independant tables : 0x04xx 0x05xx 0x10xx 0x20xx 0x21xx 0xFFxx
        // tables with 0 in 0x20xx, 0xFExx

//fileSystemTestCodePoint(folderHfs, table32, 0x200c, true);
//fileSystemTestCodePoint(folderHfs, table32, 0x3ac, true);
//fileSystemTestCodePoint(folderHfs, table32, 0x1fbb, true);
//fileSystemTestCodePoint(folderHfs, table32, 0x1f71, true);

        for ( uint32_t utf32CodePoint = utf32CodePointStart ; utf32CodePoint <= utf32CodePointEnd ; utf32CodePoint++ ) // 0x10FFFD
        {
            if ( utf32CodePoint >= 0xD800 && utf32CodePoint <= 0xDFFF ) continue;
            if ( utf32CodePoint == 0xFFEF ) continue;
            if ( utf32CodePoint == 0x0 ) continue;
            if ( utf32CodePoint == '/' ) continue; // '/' can't be tested on file system, it can't be part of the name.

            fileSystemTestCodePoint(testFolder, table32, utf32CodePoint, false);
        }

        generateTables(fs, table32, nb_total_tables, utf32CodePointStart, utf32CodePointEnd);
    }


    // 0xD800 - 0xDFFF -> in UTF16 0xFFFD
    
//    // =================================================  File system check for code point < 0x10000
//
////    write_at_path_T(folder+icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String('a'))), "bonjour", 7, "Can't write");
//    append_at_path_T(folderHfs+icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String(0x1c90))), "bonjour", "Can't write");
//    append_at_path_T(folderHfs+icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String(0x10d0))), "bonjour", "Can't write");
//    append_at_path_T(folderHfs+icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String(0x13F0))), "bonjour", "Can't write");
//    append_at_path_T(folderHfs+icu_UnicodeStringToUtf8(icu_Utf32ToUnicodeString(Utf32String(utf16_tolower(0x13F0)))), "bonjour", "Can't write");
//
//    // Check that utf32CodePoint == codePointUtf16 when < 0xFFFF (except 0xD800-0xDFFF)
//    for (uint32_t utf32CodePoint = 0x1 ; utf32CodePoint <= 0xFFFF ; utf32CodePoint++) // 0x10FFFD
//    {
//        if ( utf32CodePoint >= 0xD800 && utf32CodePoint <= 0xDFFF ) continue;
//
//        string utf8String = icu_Utf32CodePointToUtf8(utf32CodePoint);
//        const char* utf8StringData = utf8String.data();
//        size_t utf8StringSize = utf8String.size();
//        Utf16String utf16String = icu_UnicodeStringToUtf16LE(icu_Utf32ToUnicodeString(Utf32String(utf32CodePoint)));
//
//        write_at_path_T(folderHfs+utf8String, stringPrintf("%x", utf32CodePoint), "Can't write");
//
//    }
//

}

