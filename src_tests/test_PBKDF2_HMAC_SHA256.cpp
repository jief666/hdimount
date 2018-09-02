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

//#include "../src/crypto/gladman-sha/sha1.h"
//#include "../src/crypto/gladman-sha/sha2.h"
//#include "../src/crypto/gladman-sha/hmac.h"
#include "../src/crypto/Rfc3394.h"
#include "../src/crypto/PBKDF2_HMAC_SHA256.h"

#include "../src/crypto++/Crypto.h"

int test_PBKDF2_HMAC_SHA256_openssl(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, int iterations,
							        const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];

	PKCS5_PBKDF2_HMAC((const char*)pw, pw_len, salt, salt_len, iterations, EVP_sha256(), expected_result_len, out);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}


int test_PBKDF2_HMAC_SHA256_crypto(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, int iterations,
							        const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];
	
	Password_Based_Key_Derivation_Function_2_SHA256(pw, pw_len, salt, salt_len, iterations, out, expected_result_len);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

int test_PBKDF2_HMAC_SHA256_test_cryptoplusplus(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, int iterations,
							        const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];

	AES_KEY wctx;
	
	PBKDF2_HMAC_SHA256(pw, pw_len, salt, salt_len, iterations, out, expected_result_len);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

static const char* pass = "foo";
static const uint8_t salt[] = {
	0xe1, 0x37, 0xaf, 0xac, 0xe0, 0x4b, 0x99, 0x88,
	0x2d, 0xc9, 0x60, 0xae, 0x56, 0x32, 0x45, 0xe7
};
static int iterations = 100748;
static const uint8_t expected_result[] = {
	0x52, 0x7b, 0x9a, 0xbd, 0x41, 0x0a, 0x69, 0x93,
	0x11, 0x0e, 0x6c, 0x5f, 0xdf, 0x6f, 0x25, 0xff,
	0xa5, 0xcb, 0xb1, 0xdb, 0x98, 0xbe, 0x18, 0xe3,
	0x3d, 0x45, 0xa5, 0xe3, 0x9e, 0xe7, 0x0d, 0x4c
};
int test_PBKDF2_HMAC_SHA256()
{


	bool all_tests_ok = true;
	int ret;
	ret = test_PBKDF2_HMAC_SHA256_openssl((const uint8_t*)pass, strlen(pass), salt, sizeof(salt), iterations, expected_result, sizeof(expected_result));
	fprintf(stderr, "test_PBKDF2_HMAC_SHA256_openssl %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	ret = test_PBKDF2_HMAC_SHA256_crypto((const uint8_t*)pass, strlen(pass), salt, sizeof(salt), iterations, expected_result, sizeof(expected_result));
	fprintf(stderr, "test_PBKDF2_HMAC_SHA256_crypto %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	ret = test_PBKDF2_HMAC_SHA256_test_cryptoplusplus((const uint8_t*)pass, strlen(pass), salt, sizeof(salt), iterations, expected_result, sizeof(expected_result));
	fprintf(stderr, "test_PBKDF2_HMAC_SHA256_test_cryptoplusplus %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	return 1;
}

