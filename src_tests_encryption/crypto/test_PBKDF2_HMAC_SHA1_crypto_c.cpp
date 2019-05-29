#ifdef _MSC_VER
#include <malloc.h>
#endif
#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include "../../src/crypto/gladman-aes/aes.h"
#include "../../src/crypto/gladman-sha/sha1.h"
#include "../../src/crypto/gladman-sha/hmac.h"
#include "../../src/crypto/PBKDF2_HMAC_SHA1.h"
#include "../../src/crypto/TripleDes.h"
#include "../../src/crypto/Rfc3394.h"


int test_PBKDF2_HMAC_SHA1_crypto_c(const char *pass, int passlen,
                                  const unsigned char *salt, int saltlen, int iter,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
    unsigned char* out = (unsigned char*)alloca(expected_result_len);

	Password_Based_Key_Derivation_Function_2_SHA1((const uint8_t*)pass, (size_t)passlen, (const uint8_t*)salt, (size_t)saltlen, iter, (uint8_t*)out, (size_t)expected_result_len);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

