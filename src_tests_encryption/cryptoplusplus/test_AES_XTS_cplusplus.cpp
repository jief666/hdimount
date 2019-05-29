#include <stdio.h>
#include <stdlib.h> // alloca (except for WIN)
#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef _WIN32
#include <malloc.h> // for alloca
#endif

#include "../../src/crypto++/AesXts.h"



int test_AES_XTS_crypto_cplusplus(const uint8_t* key1, int key1_len, const uint8_t* key2, int key2_len,
                        const uint8_t* encrypted_text, int len, uint64_t uno,
                        const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char* out = (unsigned char*)alloca(expected_result_len);


	AesXts xts;
	xts.SetKey(key1, key2);
	xts.Decrypt(out, encrypted_text, len, uno);

//printHexBufAsCDecl((uint8_t*)&aes_key, sizeof(AES_KEY), "aes_key");
//printHexBufAsCDecl(in, length, "in");
//printHexBufAsCDecl(ivec, 16, "iv");
//printHexBufAsCDecl(out, length, "expected_result");

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

