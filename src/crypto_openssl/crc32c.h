//
//  crc32c.h
//  hdimount--jief666--wip
//
//  Created by jief on 17/07/2018.
//  Copyright © 2018 jf-luce. All rights reserved.
//

#ifndef crc32c_h
#define crc32c_h

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


uint32_t crypto_openssl_crc32c(uint32_t crc32c, const unsigned char *buffer, unsigned int length);


#ifdef __cplusplus
}
#endif

#endif /* crc32c_h */
