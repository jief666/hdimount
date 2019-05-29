#include <stdio.h>
#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef _MSC_VER
#include <windows.h>
#include <malloc.h>
#endif

//#include "../../src/crypto/aes-rijndael/rijndael-aes.h"
#include "../../src/crypto/gladman-aes/aes.h"



int test_AES_CBC_crypto_c(const unsigned char *userKey, const int bits,
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

	aes_decrypt_ctx aes_key;
	if ( bits == 128 ) aes_decrypt_key128(userKey, &aes_key);
	if ( bits == 256 ) aes_decrypt_key256(userKey, &aes_key);

    uint8_t ivec_copy[16];
    memcpy(ivec_copy, ivec, 16);
   	aes_cbc_decrypt(in, out, length, ivec_copy, &aes_key);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}
