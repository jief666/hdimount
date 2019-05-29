#include "fast_unicode_compare_apple.h"
#include "utf16Tables.h"
#include <stdio.h>

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#define u_int16_t uint16_t

static uint16_t be(uint16_t u16)
{
    return (u16<<8) + (u16>>8);
}

int32_t FastUnicodeCompare(const uint16_t* str1,  uint16_t length1, const uint16_t* str2,  uint16_t length2)
{
#ifdef DEBUG
    const uint16_t* str1bak = str1;
    const uint16_t* str2bak = str2;
#endif

     u_int16_t        c1,c2;
     u_int16_t        temp;
     u_int16_t*    lowerCaseTable;

    lowerCaseTable = (u_int16_t*) gLowerCaseTable;

    while (1) {
        /* Set default values for c1, c2 in case there are no more valid chars */
        c1 = 0;
        c2 = 0;
        
        /* Find next non-ignorable char from str1, or zero if no more */
        while (length1 && c1 == 0) {
            c1 = be(*(str1++));
            --length1;
            /* check for basic latin first */
            if (c1 < 0x0100) {
                c1 = gLatinCaseFold[c1];
                break;
            }
            /* case fold if neccessary */
            if ((temp = lowerCaseTable[c1>>8]) != 0)
                c1 = lowerCaseTable[temp + (c1 & 0x00FF)];
        }
        
        
        /* Find next non-ignorable char from str2, or zero if no more */
        while (length2 && c2 == 0) {
            c2 = be(*(str2++));
            --length2;
            /* check for basic latin first */
            if (c2 < 0x0100) {
                c2 = gLatinCaseFold[c2];
                break;
            }
            /* case fold if neccessary */
            if ((temp = lowerCaseTable[c2>>8]) != 0)
                c2 = lowerCaseTable[temp + (c2 & 0x00FF)];
        }
        
        if (c1 != c2) {       //    found a difference, so stop looping
#ifdef DEBUG
//            printf("FastUnicodeCompare difference at index : %ld\n", intptr_t(str1 - str1bak));
#endif
            break;
        }
        
        if (c1 == 0)        //    did we reach the end of both strings at the same time?
            return 0;        //    yes, so strings are equal
    }
    
    if (c1 < c2)
        return -1;
    else
        return 1;
}

int32_t FastUnicodeCompare (const HFSString& hfsString1, const HFSString& hfsString2)

{
    return FastUnicodeCompare(hfsString1.string, be(hfsString1.length), hfsString2.string, be(hfsString2.length));
}

extern int32_t FastUnicodeCompare(const HFSString127& hfsString1, const HFSString127& hfsString2)
{
    return FastUnicodeCompare(hfsString1.string, be(hfsString1.length), hfsString2.string, be(hfsString2.length));
}
