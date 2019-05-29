#import <Foundation/NSString.h>

#include <string>

NSString* ns_utf32CodePointToNSString(uint32_t codePoint);
NSString* ns_utf8ToNSString(const std::string& s);
