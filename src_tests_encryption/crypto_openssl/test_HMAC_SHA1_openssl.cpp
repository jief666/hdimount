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


int test_HMAC_SHA1_crypto_openssl(const uint8_t *key, int key_len, const unsigned char *d, size_t n,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];
	unsigned int outl = expected_result_len;

	unsigned char* rv = HMAC(EVP_sha1(), key, key_len, d, n, out, &outl);

	if ( outl != expected_result_len )
		goto err;
	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

