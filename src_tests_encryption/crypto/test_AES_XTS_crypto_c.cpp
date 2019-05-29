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

#include "../../src/crypto/gladman-aes/aes.h"
#include "../../src/crypto/gladman-aes-modes/xts.h"



int test_AES_XTS_crypto_c(const uint8_t* key1, int key1_len, const uint8_t* key2, int key2_len,
                        const uint8_t* encrypted_text, int len, uint64_t uno,
                        const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;

	uint8_t* keys = (uint8_t *)alloca(key1_len+key2_len);
	memcpy(keys, key1, key1_len);
	memcpy(keys+key1_len, key2, key2_len);

	uint8_t tweak[0x10];
	for (size_t zz = 0; zz < 0x10; zz++)
	{
		tweak[zz] = uno & 0xFF;
		uno >>= 8;
	}

	unsigned char* out = (unsigned char*)alloca(expected_result_len);
	memcpy(out, encrypted_text, expected_result_len);

	assert(len == expected_result_len);

	xts_ctx ctx;
	xts_key(keys, key1_len+key2_len, &ctx);
//	xts_key(key1, key1_len, &ctx);
	xts_decrypt(out, expected_result_len*8, tweak, &ctx);
	

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


