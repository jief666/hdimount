#ifdef _MSC_VER
#include <malloc.h>
#endif
#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include "../../src/crypto/Rfc3394.h"
#include "../../src/crypto/PBKDF2_HMAC_SHA256.h"
#include "../../src/crypto/TripleDes.h"
#include "../../src/crypto/gladman-sha/sha1.h"
#include "../../src/crypto/gladman-sha/sha2.h"
#include "../../src/crypto/gladman-sha/hmac.h"

int test_HMAC_SHA256_crypto_c(const uint8_t *key, int key_len, const unsigned char *d, size_t n,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
    unsigned char* out = (unsigned char*)alloca(expected_result_len);

	hmac_sha(HMAC_SHA256, key, key_len, d, n, out, expected_result_len);
	
	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}
