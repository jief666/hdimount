//
//  PrintBuf.hpp
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//

#ifndef PrintBuf_h
#define PrintBuf_h

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h> // size_t

void printHexBufAsCDecl(const void* vbuf, size_t count, const char* name, int line_length = 16);
void printBuf(const void* buf, size_t count);

#endif /* PrintBuf_hpp */
