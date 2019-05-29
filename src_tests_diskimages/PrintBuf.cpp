//
//  PrintBuf.cpp
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//

#include "PrintBuf.h"

#include <stdint.h>
#include <sys/types.h> // size_t
#include <ctype.h> // isprint()

void printHexBufAsCDecl(const void* vbuf, size_t count, const char* name, int line_length)
{
	uint8_t* buf = (uint8_t*)vbuf;
	if ( name ) printf("static const uint8_t %s[%zd] = {\n", name, count);
	for (size_t i = 0; i < count;) {
		printf("\t");
		for (int j = 0; j < line_length; j++) {
			if ( i + j < count-1 ) {
				if ( j < line_length-1 ) {
					printf("0x%02x, ", (uint8_t) (buf[i + j]));
				}else{
					printf("0x%02x,", (uint8_t) (buf[i + j]));
				}
			}
			else if ( i + j < count ) {
				printf("0x%02x ", (uint8_t) (buf[i + j]));
			}else{
				printf("      ");
			}
			if ( j%4 == 3 ) printf(" ");
		}
		printf("   // ");
		for (int j = 0; j < line_length; j++) {
			if ( i + j < count ) {
				if ( isprint(buf[i + j]))
					printf("%c", (uint8_t) (buf[i + j]));
				else {
					if ( buf[i + j] == 0) printf("_");
					else printf(".");
				}
			}
		}
		i += line_length;
		printf("\n");
	}
	if ( name ) printf("};\n");
}

void printBuf(const void* buf, size_t count)
{
	const uint8_t* p = (const uint8_t*)buf;
	for (size_t i = 0; i < count;) {
		for (int j = 0; j+i < count && j < 16; j++)
			printf("%02x ", (uint8_t) (p[i + j]));
		for (size_t j = 0; j+i < count && j < 16; j++) {
			if (p[i + j] >= 32)
				printf("%c ", (uint8_t) (p[i + j]));
			else
				printf("  ");
		}
		i += 16;
		if (i % 16 == 0)
			printf("\n");
	}
}
