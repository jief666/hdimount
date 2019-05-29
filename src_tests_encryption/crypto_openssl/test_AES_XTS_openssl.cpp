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



int test_AES_XTS_crypto_openssl(const uint8_t* key1, int key1_len, const uint8_t* key2, int key2_len,
                        const uint8_t* encrypted_text, int len, uint64_t uno,
                        const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	uint8_t keys[key1_len+key2_len];
	memcpy(keys, key1, key1_len);
	memcpy(keys+key1_len, key2, key2_len);

	unsigned char out[expected_result_len];
//printHexBufAsCDecl(userKey, bits/8, "userKey");

	uint8_t tweak[0x10];
	for (size_t zz = 0; zz < 0x10; zz++)
	{
		tweak[zz] = uno & 0xFF;
		uno >>= 8;
	}

	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	int outlen, tmplen;
	if (!EVP_DecryptInit_ex(ctx, EVP_aes_128_xts(), NULL, keys, tweak))
		goto err;
	if (!EVP_DecryptUpdate(ctx, out, &outlen, encrypted_text, len))
		goto err;
	if (!EVP_DecryptFinal_ex(ctx, out + outlen, &tmplen))
		goto err;
	EVP_CIPHER_CTX_free(ctx);
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


