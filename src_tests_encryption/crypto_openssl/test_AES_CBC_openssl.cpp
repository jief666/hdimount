#include <stddef.h>
#include <stdlib.h> // alloca (except for WIN)
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef _WIN32
#include <malloc.h> // for alloca
#endif

#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/modes.h>


int test_AES_CBC_crypto_openssl(const unsigned char *userKey, const int bits,
                         const unsigned char *in, size_t length,
                         const unsigned char *ivec,
                         const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];
//printHexBufAsCDecl(userKey, bits/8, "userKey");

	AES_KEY aes_key;
	memset(&aes_key, 0, sizeof(AES_KEY)); // not needed
	int rv1 = AES_set_decrypt_key(userKey, bits, &aes_key); // 0 on success
//printHexBufAsCDecl((uint8_t*)&aes_key, sizeof(AES_KEY), "aes_key");
//printHexBufAsCDecl(in, length, "in");
//printHexBufAsCDecl(ivec, 16, "iv");
    uint8_t ivec_copy[16];
    memcpy(ivec_copy, ivec, 16);
	AES_cbc_encrypt((uint8_t*)in, (uint8_t*)out, length, &aes_key, ivec_copy, AES_DECRYPT);
//printHexBufAsCDecl(out, length, "expected_result");

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}


