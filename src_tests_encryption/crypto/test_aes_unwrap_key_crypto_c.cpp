#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#ifdef _WIN32
#include <malloc.h> // for alloca
#endif

#include "../../src/crypto/gladman-aes/aes.h"
#include "../../src/crypto/gladman-sha/sha1.h"
#include "../../src/crypto/gladman-sha/hmac.h"
#include "../../src/crypto/PBKDF2_HMAC_SHA1.h"
#include "../../src/crypto/TripleDes.h"
#include "../../src/crypto/Rfc3394.h"


int test_AES_wrap_unwrap_crypto_c(const unsigned char *kek, int keybits,
			 const unsigned char *wrapped_key,
			 int wrapped_key_len,
			 const unsigned char *expectedIv,
			 const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char* ptmp = (unsigned char*)alloca(expected_result_len);

	int r = AES_unwrap_key_2(ptmp, wrapped_key, wrapped_key_len, kek, keybits, (uint64_t*)expectedIv);

	if ( r != expected_result_len )
		goto err;
	if (memcmp(expected_result, ptmp, expected_result_len))
		goto err;
	// iv not updated
	(void)expectedIv;
	
	ret = 1;

	err:

	return ret;

}
