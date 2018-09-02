#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/modes.h>

#include "../src/crypto/Rfc3394.h"
#include "../src/crypto/PBKDF2_HMAC_SHA1.h"
#include "../src/crypto/TripleDes.h"

#include "../src/crypto++/Crypto.h"
#include "../src/crypto++/TripleDes.h"

int test_DES_CBC_openssl(const unsigned char *key, const unsigned char *iv, const unsigned char *in, int inl,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];

	EVP_CIPHER_CTX ctx;
	int outl, outl2;

	EVP_CIPHER_CTX_init(&ctx);
	EVP_DecryptInit_ex(&ctx, EVP_des_ede3_cbc(), NULL, key, iv);
	if(!EVP_DecryptUpdate(&ctx, out, &outl, in, inl)) {
		goto err;
	}
	if(!EVP_DecryptFinal_ex(&ctx, out + outl, &outl2)) {
		goto err;
	}
	outl += outl2;
	EVP_CIPHER_CTX_cleanup(&ctx);

	if ( outl != expected_result_len )
		goto err;
	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}


int test_DES_CBC_crypto(const unsigned char *key, const unsigned char *iv, const unsigned char *in, int inl,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];

	TripleDESData_t ctx;
	TripleDesSetKey(key, &ctx);
	TripleDesSetIV(iv, &ctx);
	TripleDesDecryptCBC(out, in, inl, &ctx);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

int test_DES_CBC_test_cryptoplusplus(const unsigned char *key, const unsigned char *iv, const unsigned char *in, int inl,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];

	TripleDES tdes;
	tdes.SetKey(key);
	tdes.SetIV(iv);
	tdes.DecryptCBC(out, in, inl);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

static const uint8_t key[] = {
	0xb9, 0x5e, 0xb6, 0x81, 0x60, 0x59, 0x60, 0xa5,
	0x83, 0x88, 0x41, 0x9a, 0x54, 0x45, 0xc8, 0x61,
	0x18, 0x4c, 0xdd, 0x3a, 0xf5, 0x69, 0x51, 0xbd
};
static const uint8_t iv[] = {
	0xa8, 0x46, 0x7a, 0x0c, 0x56, 0xfb, 0x6a, 0x4f,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t in[] = {
	0x0f, 0xae, 0x28, 0x87, 0x25, 0x9b, 0xae, 0x4b,
	0xc3, 0x78, 0x9c, 0xa8, 0x90, 0xd0, 0x69, 0x99,
	0xb4, 0x2e, 0x27, 0x63, 0x6e, 0xe6, 0xf4, 0xef,
	0xa7, 0x3c, 0x60, 0x73, 0xb1, 0x67, 0xf3, 0x55,
	0xe2, 0xb5, 0x24, 0x57, 0xc6, 0xb4, 0x69, 0x9f,
	0x61, 0x17, 0x96, 0x54, 0x28, 0xe1, 0x6c, 0xce
};

static const uint8_t expected_result[] = {
	0x98, 0x35, 0x11, 0x34, 0xd5, 0x78, 0x26, 0xb0,
	0x87, 0x82, 0x97, 0x4d, 0x9c, 0x9b, 0x32, 0x9a,
	0x98, 0x35, 0x11, 0x34, 0xd5, 0x78, 0x26, 0xb0,
	0x87, 0x82, 0x97, 0x4d, 0x9c, 0x9b, 0x32, 0x9a,
	0x00, 0xbc, 0xb3, 0x07, 0x43, 0x4b, 0x49, 0x45,
	0x00
};

int test_DES_CBC()
{


	bool all_tests_ok = true;
	int ret;
	ret = test_DES_CBC_openssl(key, iv, in, sizeof(in), expected_result, sizeof(expected_result));
	fprintf(stderr, "test_DES_CBC_openssl %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	ret = test_DES_CBC_crypto(key, iv, in, sizeof(in), expected_result, sizeof(expected_result));
	fprintf(stderr, "test_DES_CBC_crypto %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	ret = test_DES_CBC_test_cryptoplusplus(key, iv, in, sizeof(in), expected_result, sizeof(expected_result));
	fprintf(stderr, "test_DES_CBC_test_cryptoplusplus %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	return 1;
}

