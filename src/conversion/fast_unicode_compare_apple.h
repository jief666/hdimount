#include <stdint.h>
#include "../darling-dmg/src/hfsplus.h"

extern int32_t FastUnicodeCompare(const uint16_t* str1, uint16_t length1, const uint16_t* str2, uint16_t length2);

extern int32_t FastUnicodeCompare(const HFSString& hfsString1, const HFSString& hfsString2);
extern int32_t FastUnicodeCompare(const HFSString127& hfsString1, const HFSString127& hfsString2);
