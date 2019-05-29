#include <stddef.h>
#include <stdlib.h> // alloca (except for WIN)
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef _WIN32
#include <malloc.h> // for alloca
#endif


#include "../../src/crypto++/Crypto.h"


int test_PBKDF2_HMAC_SHA1_crypto_cplusplus(const char *pass, int passlen,
                                  const unsigned char *salt, int saltlen, int iter,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char* out = (unsigned char*)alloca(expected_result_len);

	PBKDF2_HMAC_SHA1((const uint8_t*)pass, (size_t)passlen, (const uint8_t*)salt, (size_t)saltlen, iter, (uint8_t*)out, (size_t)expected_result_len);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}
