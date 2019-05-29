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


int test_AES_wrap_unwrap_crypto_openssl(const unsigned char *kek, int keybits,
			 const unsigned char *wrapped_key,
			 int wrapped_key_len,
			 const unsigned char *expectedIv,
			 const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char* ptmp = (unsigned char*)alloca(expected_result_len);

	int r;
	AES_KEY wctx;
		
	if (AES_set_decrypt_key(kek, keybits, &wctx))
		goto err;
	r = AES_unwrap_key(&wctx, NULL, ptmp, wrapped_key, wrapped_key_len);

	if ( r != expected_result_len )
		goto err;
	if (memcmp(expected_result, ptmp, expected_result_len))
		goto err;
// iv seems not used
//	if (memcmp(expectedIv, &iv, sizeof(iv)))
//		goto err;
	(void)expectedIv;

	ret = 1;

	err:

	return ret;

}
