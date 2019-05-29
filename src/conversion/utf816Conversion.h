//
//  utf816Conversion.hpp
//  hdimount--jief666--wip
//
//  Created by jief on 17.05.19.
//  Copyright © 2019 jf-luce. All rights reserved.
//

#ifndef utf816Conversion_hpp
#define utf816Conversion_hpp

#include <stdint.h>
#include <string>
#include "../darling-dmg/src/hfsplus.h"


size_t utf8_to_utf16LE(uint16_t *dst, size_t dst_len, const char *src, size_t src_len, int flags, int *errp);
size_t utf8_to_utf16BE(uint16_t *dst, size_t dst_len, const char *src, size_t src_len, int flags, int *errp);

size_t utf16LE_to_utf8(char *dst, size_t dst_len, const uint16_t *srcLE, size_t src_len, /*int flags,*/ int *errp);
size_t utf16BE_to_utf8(char *dst, size_t dst_len, const uint16_t *srcLE, size_t src_len, /*int flags,*/ int *errp);

//int utf16LE_to_utf8(const HFSString& hfsString, std::string* s);
int utf16BE_to_utf8(const HFSString& hfsString, std::string* s);
int utf16BE_to_utf8(const HFSString127& hfsString, std::string* s);
//int utf8_to_utf16LE(const std::string& s, HFSString* hfsString);
int utf8_to_utf16BE(const std::string& s, HFSString* hfsString);
int utf8_to_utf16BE(const std::string& s, HFSString127* hfsString);

#ifdef DEBUG
// Only DEBUG because inefficient, but handy in a debugger.
std::string toUtf8(const HFSPlusAttributeKey& key);
#endif

#endif /* utf816Conversion_hpp */
