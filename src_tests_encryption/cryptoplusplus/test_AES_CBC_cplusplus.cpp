#include <stddef.h>
#include <stdlib.h> // alloca (except for WIN)
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef _WIN32
#include <malloc.h> // for alloca
#endif

#include "../../src/crypto++/Aes++.h"



int test_AES_CBC_crypto_cplusplus(const unsigned char *userKey, const int bits,
                         const unsigned char *in, size_t length,
                         const unsigned char *ivec,
                         const unsigned char *expected_result, int expected_result_len)
{
    int ret = 0;
	unsigned char* out = (unsigned char*)alloca(expected_result_len);
    out[0] = 0xDE;
    out[1] = 0xEA;
    out[2] = 0xBE;
    out[3] = 0xEF;

    AES aes;
    if ( bits == 128 ) aes.SetKey(userKey, AES::AES_128);
    if ( bits == 256 ) aes.SetKey(userKey, AES::AES_256);;
    aes.SetIV(ivec);
    aes.DecryptCBC(in, out, length);

    if (memcmp(expected_result, out, expected_result_len))
        goto err;
    ret = 1;
  err:
    return ret;
}
