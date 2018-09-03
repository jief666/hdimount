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

#include "../../src/crypto++/Crypto.h"

int test_HMAC_SHA1_test_cryptoplusplus(const uint8_t *key, int key_len, const unsigned char *d, size_t n,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];

	HMAC_SHA1(key, key_len, d, n, out);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

static const uint8_t key[] = {
	0x98, 0x35, 0x11, 0x34, 0xd5, 0x78, 0x26, 0xb0,
	0x87, 0x82, 0x97, 0x4d, 0x9c, 0x9b, 0x32, 0x9a,
	0x00, 0xbc, 0xb3, 0x07
};
static const uint8_t data[] = {
	0x00, 0x00, 0x4c, 0x72
};
static const uint8_t expected_result[] = {
	0x8d, 0x9a, 0xfc, 0x86, 0xcc, 0xc0, 0x3d, 0xdd,
	0x51, 0x05, 0xd2, 0x3a, 0x62, 0x33, 0xa8, 0x18,
	0x76, 0xa8, 0x02, 0xdc
};

int test_HMAC_SHA1_plusplus()
{


	bool all_tests_ok = true;
	int ret;

	ret = test_HMAC_SHA1_test_cryptoplusplus(key, sizeof(key), data, sizeof(data), expected_result, sizeof(expected_result));
	fprintf(stderr, "test_HMAC_SHA1_test_cryptoplusplus %d\n", ret);
	if ( ret == 0 ) all_tests_ok = false;

	return 1;
}

