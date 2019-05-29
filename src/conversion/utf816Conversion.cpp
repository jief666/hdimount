//
//  utf816Conversion.cpp
//  hdimount--jief666--wip
//
//  Created by jief on 17.05.19.
//  Copyright © 2019 jf-luce. All rights reserved.
//

#include "utf816Conversion.h"
#include <string.h> // for memcpy
#include <stdlib.h> // for alloca

using namespace std;

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static uint16_t be(uint16_t u16)
{
    return (u16<<8) + (u16>>8);
}


#define UNICODE_DECOMPOSE        0x01
#define UNICODE_PRECOMPOSE        0x02
#define UNICODE_UTF8_LATIN1_FALLBACK    0x03

size_t
utf8_to_utf16LE(uint16_t *dst, size_t dst_len,
            const char *src, size_t src_len,
            int flags, int *errp)
{
    const unsigned char *s;
    size_t spos, dpos;
    int error;
    uint16_t c;

#define IS_CONT(c)    (((c)&0xc0) == 0x80)

    error = 0;
    s = (const unsigned char *)src;
    spos = dpos = 0;
    while (spos < src_len) {
        if (s[spos] < 0x80) {
            c = s[spos++];
        } else if ((flags & UNICODE_UTF8_LATIN1_FALLBACK)
             && (spos >= src_len || !IS_CONT(s[spos+1]))
             && s[spos]>=0xa0) {
            /* not valid UTF-8, assume ISO 8859-1 */
            c = s[spos++];
        } else if (s[spos] < 0xc0 || s[spos] >= 0xf5) {
            /* continuation byte without lead byte
             * or lead byte for codepoint above 0x10ffff */
            error++;
            spos++;
            continue;
        } else if (s[spos] < 0xe0) {
            if (spos >= src_len || !IS_CONT(s[spos+1])) {
                spos++;
                error++;
                continue;
            }
            c = ((s[spos] & 0x3f) << 6) | (s[spos+1] & 0x3f);
            spos += 2;
            if (c < 0x80) {
                /* overlong encoding */
                error++;
                continue;
            }
        } else if (s[spos] < 0xf0) {
            if (spos >= src_len-2 ||
                !IS_CONT(s[spos+1]) || !IS_CONT(s[spos+2])) {
                spos++;
                error++;
                continue;
            }
            c = ((s[spos] & 0x0f) << 12) | ((s[spos+1] & 0x3f) << 6)
                | (s[spos+2] & 0x3f);
            spos += 3;
            if (c < 0x800 || (c >= 0xdf00 && c < 0xe000) ) {
                /* overlong encoding or encoded surrogate */
                error++;
                continue;
            }
        } else {
            uint32_t cc;
            /* UTF-16 surrogate pair */

            if (spos >= src_len-3 || !IS_CONT(s[spos+1])
                || !IS_CONT(s[spos+2]) || !IS_CONT(s[spos+3])) {
                spos++;
                error++;
                continue;
            }
            cc = ((s[spos] & 0x03) << 18) | ((s[spos+1] & 0x3f) << 12)
                | ((s[spos+2] & 0x3f) << 6) | (s[spos+3] & 0x3f);
            spos += 4;
            if (cc < 0x10000) {
                /* overlong encoding */
                error++;
                continue;
            }
            if (dst && dpos < dst_len)
                dst[dpos] = (0xd800 | ((cc-0x10000)>>10));
            dpos++;
            c = 0xdc00 | ((cc-0x10000) & 0x3ff);
        }

        if (dst && dpos < dst_len)
            dst[dpos] = c;
        dpos++;
    }
    if (errp)
        *errp = error;
    return dpos;
#undef IS_CONT
}

size_t
utf8_to_utf16BE(uint16_t *dst, size_t dst_len,
            const char *src, size_t src_len,
            int flags, int *errp)
{
    size_t len = utf8_to_utf16LE(dst, dst_len, src, src_len, flags, errp);
    // I'm pretty sure there is a better way than swapping everything after of utf8_to_utf16LE
    for (size_t i = 0 ; i<len ; i++ ) {
        dst[i] = be(dst[i]);
    }
    return len;
}

size_t
utf16LE_to_utf8(char *dst, size_t dst_len,
          const uint16_t *src, size_t src_len,
          /*int flags,*/ int *errp)
{
    uint16_t spos, dpos;
    int error;

#define CHECK_LENGTH(l)    (dpos > dst_len-(l) ? dst=NULL : NULL)
#define ADD_BYTE(b)    (dst ? dst[dpos] = (b) : 0, dpos++)

    error = 0;
    dpos = 0;
    for (spos = 0; spos < src_len; spos++) {
        if (src[spos] < 0x80) {
            CHECK_LENGTH(1);
            ADD_BYTE(src[spos]);
        } else if (src[spos] < 0x800) {
            CHECK_LENGTH(2);
            ADD_BYTE(0xc0 | (src[spos]>>6));
            ADD_BYTE(0x80 | (src[spos] & 0x3f));
        } else if(src[spos] >= 0xd800 && src[spos] < 0xe000) {
             if ((src[spos] & 0xdc00) == 0xd800) {
                uint32_t c;
                /* first surrogate */
                if (spos == src_len - 1 || (src[spos+1] & 0xdc00) != 0xdc00) {
                    /* no second surrogate present */
                    error++;
                    continue;
                }
                spos++;
                CHECK_LENGTH(4);
                c = (((src[spos-1]&0x3ff) << 10) | (src[spos]&0x3ff)) + 0x10000;
                ADD_BYTE(0xf0 | (c>>18));
                ADD_BYTE(0x80 | ((c>>12) & 0x3f));
                ADD_BYTE(0x80 | ((c>>6) & 0x3f));
                ADD_BYTE(0x80 | (c & 0x3f));
            } else if ((src[spos] & 0xdc00) == 0xdc00) {
                /* second surrogate without preceding first surrogate */
                error++;
            } else {
                /* in surrogate pair range but none found */
                error++;
            }
        } else {
            CHECK_LENGTH(3);
            ADD_BYTE(0xe0 | src[spos]>>12);
            ADD_BYTE(0x80 | ((src[spos]>>6) & 0x3f));
            ADD_BYTE(0x80 | (src[spos] & 0x3f));
        }
    }

    if (errp)
        *errp = error;
    return dpos;
#undef ADD_BYTE
#undef CHECK_LENGTH
}

size_t
utf16BE_to_utf8(char *dst, size_t dst_len,
                const uint16_t *srcBE, size_t src_len,
          /*int flags,*/ int *errp)
{
    // I'm pretty sure there is a better way than swapping everything in front of utf16LE_to_utf8
    uint16_t* src = (uint16_t*)alloca(src_len*2);
    for (size_t i = 0 ; i<src_len ; i++ ) {
        src[i] = be(srcBE[i]);
    }
    return utf16LE_to_utf8(dst, dst_len, src, src_len, errp);
}

//
//int utf16LE_to_utf8(const HFSString& hfsString, std::string* s)
//{
//    char* buf = (char*)alloca(hfsString.length*2);
//    int error;
//    size_t len = utf16LE_to_utf8(buf, hfsString.length*2, hfsString.string, hfsString.length, &error);
//    s->assign(buf, len);
//    return error;
//}

int utf16BE_to_utf8(const HFSString& hfsString, std::string* s)
{
    size_t allocatedSize = be(hfsString.length)*4; // One UTF16 char can 4 bytes
    char* buf = (char*)alloca(allocatedSize);
#ifdef DEBUG
	memset(buf, 0, allocatedSize);
#endif
    int error;
    size_t len = utf16BE_to_utf8(buf, allocatedSize, hfsString.string, be(hfsString.length), &error);
    s->assign(buf, len);
    return error;
}

int utf16BE_to_utf8(const HFSString127& hfsString, std::string* s)
{
    size_t allocatedSize = be(hfsString.length)*4; // One UTF16 char can 4 bytes
    char* buf = (char*)alloca(allocatedSize);
#ifdef DEBUG
	memset(buf, 0, allocatedSize);
#endif
    int error;
    size_t len = utf16BE_to_utf8(buf, allocatedSize, hfsString.string, be(hfsString.length), &error);
    s->assign(buf, len);
    return error;
}
//
//int utf8_to_utf16LE(const std::string& s, HFSString* hfsString)
//{
//    int error;
//    hfsString->length = utf8_to_utf16LE(hfsString->string, sizeof(hfsString->string), s.data(), s.length(), 0, &error);
//    return error;
//}


int utf8_to_utf16BE(const std::string& s, HFSString* hfsString)
{
    int error;
    size_t len = utf8_to_utf16LE(hfsString->string, sizeof(hfsString->string), s.data(), s.length(), 0, &error);
    for (size_t i = 0 ; i<len ; i++ ) {
        hfsString->string[i] = be(hfsString->string[i]);
    }
    hfsString->length = be(len);
    return error;
}

int utf8_to_utf16BE(const std::string& s, HFSString127* hfsString)
{
    int error;
    size_t len = utf8_to_utf16LE(hfsString->string, sizeof(hfsString->string), s.data(), s.length(), 0, &error);
    for (size_t i = 0 ; i<len ; i++ ) {
        hfsString->string[i] = be(hfsString->string[i]);
    }
    hfsString->length = be(len);
    return error;
}

#ifdef DEBUG
// Only DEBUG because inefficient, but handy in a debugger.
string toUtf8(const HFSPlusAttributeKey& key)
{
    size_t allocatedSize = be(key.attrName.length)*4; // One UTF16 char can 4 bytes
    char* buf = (char*)alloca(allocatedSize);
#ifdef DEBUG
	memset(buf, 0, allocatedSize);
#endif
    int error;
    size_t len = utf16BE_to_utf8(buf, allocatedSize, key.attrName.string, be(key.attrName.length), &error);
    string s;
    s.assign(buf, len);
    return s;
}
#endif
