#include <inttypes.h>
/*
 * For better performance, the case folding table for basic latin
 * is seperate from the others.  This eliminates the extra lookup
 * to get the offset to this table.
 *
 * Note: 0x0000 now maps to 0 so that it will be ignored
 */

extern uint16_t gLatinCaseFold[];

/*    The lower case table consists of a 256-entry high-byte table followed by some number of
      256-entry subtables. The high-byte table contains either an offset to the subtable for
      characters with that high byte or zero, which means that there are no case mappings or
      ignored characters in that block. Ignored characters are mapped to zero.
 */
extern uint16_t gLowerCaseTableIndexSize;
extern uint16_t gLowerCaseTable[];
