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

int test_PBKDF2_HMAC_SHA1_crypto_openssl(const char *pass, int passlen,
                                  const unsigned char *salt, int saltlen, int iter,
							      const unsigned char *expected_result, int expected_result_len)
{
	int ret = 0;
	unsigned char out[expected_result_len];

//	AES_KEY wctx;
	
	int rv = PKCS5_PBKDF2_HMAC_SHA1(pass, passlen, salt, saltlen, iter, expected_result_len, out);

	if ( rv != 1 )
		goto err;
	if (memcmp(expected_result, out, expected_result_len))
		goto err;
	ret = 1;
  err:
	return ret;
}

