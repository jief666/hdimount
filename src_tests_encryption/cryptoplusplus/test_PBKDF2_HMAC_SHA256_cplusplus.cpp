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

int test_PBKDF2_HMAC_SHA256_crypto_cplusplus(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, int iterations,
							        const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char* out = (unsigned char*)alloca(expected_result_len);
	
	PBKDF2_HMAC_SHA256(pw, pw_len, salt, salt_len, iterations, out, expected_result_len);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}
