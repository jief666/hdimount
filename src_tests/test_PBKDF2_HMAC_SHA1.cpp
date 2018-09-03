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

#include "../src/crypto++/Crypto.h"

int test_PBKDF2_HMAC_SHA1_openssl(const char *pass, int passlen,
                                  const unsigned char *salt, int saltlen, int iter,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];

	AES_KEY wctx;
	
	int rv = PKCS5_PBKDF2_HMAC_SHA1(pass, passlen, salt, saltlen, iter, expected_result_len, out);

	if ( rv != 1 )
		goto err;
	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}


int test_PBKDF2_HMAC_SHA1_crypto(const char *pass, int passlen,
                                  const unsigned char *salt, int saltlen, int iter,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];

	AES_KEY wctx;
	
	Password_Based_Key_Derivation_Function_2_SHA1((const uint8_t*)pass, (size_t)passlen, (const uint8_t*)salt, (size_t)saltlen, iter, (uint8_t*)out, (size_t)expected_result_len);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

int test_PBKDF2_HMAC_SHA1_test_cryptoplusplus(const char *pass, int passlen,
                                  const unsigned char *salt, int saltlen, int iter,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];

	AES_KEY wctx;
	
	PBKDF2_HMAC_SHA1((const uint8_t*)pass, (size_t)passlen, (const uint8_t*)salt, (size_t)saltlen, iter, (uint8_t*)out, (size_t)expected_result_len);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

static const char pass[] = "foo";
static const unsigned char salt[] = {
	0x76, 0xc7, 0xb1, 0x0d, 0xfb, 0xce, 0x6f, 0x91,
	0xb4, 0x97, 0x55, 0x82, 0xef, 0x50, 0x0a, 0xc8,
	0x7c, 0xb5, 0xba, 0x76
};

static const uint8_t expected_result[] = {
	0xb9, 0x5e, 0xb6, 0x81, 0x60, 0x59, 0x60, 0xa5,
	0x83, 0x88, 0x41, 0x9a, 0x54, 0x45, 0xc8, 0x61,
	0x18, 0x4c, 0xdd, 0x3a, 0xf5, 0x69, 0x51, 0xbd
};

int test_PBKDF2_HMAC_SHA1()
{


	bool all_tests_ok = true;
	int ret;
	ret = test_PBKDF2_HMAC_SHA1_openssl(pass, strlen(pass), salt, sizeof(salt), 192307, expected_result, sizeof(expected_result));
	fprintf(stderr, "test_PBKDF2_HMAC_SHA1_openssl %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	ret = test_PBKDF2_HMAC_SHA1_crypto(pass, strlen(pass), salt, sizeof(salt), 192307, expected_result, sizeof(expected_result));
	fprintf(stderr, "test_PBKDF2_HMAC_SHA1_crypto %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	ret = test_PBKDF2_HMAC_SHA1_test_cryptoplusplus(pass, strlen(pass), salt, sizeof(salt), 192307, expected_result, sizeof(expected_result));
	fprintf(stderr, "test_PBKDF2_HMAC_SHA1_test_cryptoplusplus %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	return 1;
}

