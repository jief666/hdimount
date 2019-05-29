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




int test_PBKDF2_HMAC_SHA256_crypto_c(const uint8_t* pw, size_t pw_len, const uint8_t* salt, size_t salt_len, int iterations,
							        const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
    unsigned char* out = (unsigned char*)alloca(expected_result_len);

	Password_Based_Key_Derivation_Function_2_SHA256(pw, pw_len, salt, salt_len, iterations, out, expected_result_len);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}
