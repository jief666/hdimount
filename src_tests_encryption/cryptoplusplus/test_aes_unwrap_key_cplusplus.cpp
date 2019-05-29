#include <stddef.h>
#include <stdlib.h> // alloca (except for WIN)
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef _WIN32
#include <malloc.h> // for alloca
#endif

#include "../../src/crypto/Rfc3394.h"
#include "../../src/crypto++/Crypto.h"

int test_AES_wrap_unwrap_crypto_cplusplus(const unsigned char *kek, int keybits,
			 const unsigned char *wrapped_key,
			 int wrapped_key_len,
			 const unsigned char *expectedIv,
			 const unsigned char *expected_result, int expected_result_len)
{
	assert(wrapped_key_len - 8 == expected_result_len);

	int ret = 0;
	unsigned char* ptmp = (unsigned char*)alloca(expected_result_len);
	uint64_t iv = 0;

	auto aes_mode = AES::AES_128;
	if ( keybits == 256 ) aes_mode = AES::AES_256;
	int r = Rfc3394_KeyUnwrap(ptmp, wrapped_key, expected_result_len, kek, aes_mode, &iv);

	if ( r != 1 )
		goto err;
	if (memcmp(expected_result, ptmp, expected_result_len))
		goto err;
	if (memcmp(expectedIv, &iv, sizeof(iv)))
		goto err;

	ret = 1;

	err:

	return ret;

}
