#ifdef _MSC_VER
#include <malloc.h>
#endif
#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include "../../src/crypto/Rfc3394.h"
#include "../../src/crypto/PBKDF2_HMAC_SHA1.h"
#include "../../src/crypto/TripleDes.h"

int test_DES_CBC_crypto_c(const unsigned char *key, const unsigned char *iv, const unsigned char *in, int inl,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char* out = (unsigned char*)alloca(expected_result_len);
	memset(out, 0, expected_result_len);

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
