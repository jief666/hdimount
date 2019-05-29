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
#include "../../src/crypto++/TripleDes++.h"

int test_DES_CBC_crypto_cplusplus(const unsigned char *key, const unsigned char *iv, const unsigned char *in, int inl,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char* out = (unsigned char*)alloca(expected_result_len);

	TripleDES tdes;
	tdes.SetKey(key);
	tdes.SetIV(iv);
	tdes.DecryptCBC(out, in, inl);

	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}
